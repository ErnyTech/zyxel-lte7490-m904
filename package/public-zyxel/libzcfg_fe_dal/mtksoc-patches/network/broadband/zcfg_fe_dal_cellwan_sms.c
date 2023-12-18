#include <ctype.h>
#include <json/json.h>
#include <time.h>
#include <sys/queue.h>
#include <unistd.h>

#include "zcfg_common.h"
#include "zcfg_fe_rdm_string.h"
#include "zcfg_fe_rdm_struct.h"
#include "zcfg_rdm_oid.h"
#include "zcfg_rdm_obj.h"
#include "zcfg_debug.h"
//#include "zcfg_eid.h"
#include "zcfg_msg.h"
#include "zcfg_fe_dal_common.h"

#ifdef ZYXEL_WEB_GUI_CELLWAN_SUPPORT_SMS

dal_param_t CELLWAN_SMS_param[]={
{"SMS_Enable", dalType_boolean, 0, 0, NULL},
{ NULL,	0,	0,	0,	NULL}

};

#define HEX_STR_OCTET_LEN		2
#define ADDR_LEN_HEX_STR_LEN	(  1 * HEX_STR_OCTET_LEN)
#define ADDR_TYPE_HEX_STR_LEN	(  1 * HEX_STR_OCTET_LEN)
#define ADDR_VALUE_HEX_STR_LEN	( 10 * HEX_STR_OCTET_LEN)
#define PDU_TYPE_HEX_STR_LEN	(  1 * HEX_STR_OCTET_LEN)
#define MR_HEX_STR_LEN			(  1 * HEX_STR_OCTET_LEN)
#define PID_HEX_STR_LEN			(  1 * HEX_STR_OCTET_LEN)
#define DCS_HEX_STR_LEN			(  1 * HEX_STR_OCTET_LEN)
#define SCTS_HEX_STR_LEN		(  7 * HEX_STR_OCTET_LEN)
#define VP_HEX_STR_LEN			(  1 * HEX_STR_OCTET_LEN)
#define UDL_HEX_STR_LEN			(  1 * HEX_STR_OCTET_LEN)
#define UD_HEX_STR_LEN			(160 * HEX_STR_OCTET_LEN)
#define UDH_LEN_HEX_STR_LEN		(  1 * HEX_STR_OCTET_LEN)
#define UDH_IEI_HEX_STR_LEN		(  1 * HEX_STR_OCTET_LEN)
#define UDH_IEDL_HEX_STR_LEN	(  1 * HEX_STR_OCTET_LEN)
#define UDH_REF_NO_HEX_STR_LEN	(  1 * HEX_STR_OCTET_LEN)
#define UDH_MAX_NO_HEX_STR_LEN	(  1 * HEX_STR_OCTET_LEN)
#define UDH_SEQ_NO_HEX_STR_LEN	(  1 * HEX_STR_OCTET_LEN)
#define ADDR_COMPLEMENT_CHAR	'F'
#define ZERO_HEX_STR			"00"
#define ADDR_TYPE_NATIONAL		0x81
#define ADDR_TYPE_INTERNATIONAL	0x91
#define VP_DAY(x)				((x)+166) //range from 1 to 30
#define TIME_STAMP_FORMAT_STR_LEN	20
#define CONCAT_COMMA_CHAR		","
#define CONCAT_INDEX_STR_LEN		64
#define CONCAT_CONTENT_MAX_ENTRY	8
#define SINGLE_CONTENT_STR_LEN		512
#define CONCAT_CONTENT_STR_LEN		(CONCAT_CONTENT_MAX_ENTRY * SINGLE_CONTENT_STR_LEN)


typedef struct
{
	unsigned char addrLength;
	unsigned char addrType;
	unsigned char addrValue[ADDR_VALUE_HEX_STR_LEN + 1];
}sAddrField;

typedef struct
{
	unsigned char MTI: 2;
	unsigned char MMS: 1;
	unsigned char LP: 1;
	unsigned char RESERVED: 1;
	unsigned char SRI: 1;
	unsigned char UDHI: 1;
	unsigned char RP: 1;
}sPduTypeDeliver;

typedef struct
{
	unsigned char MTI: 2;
	unsigned char RD: 1;
	unsigned char VPF: 2;
	unsigned char SRR: 1;
	unsigned char UDHI: 1;
	unsigned char RP: 1;
}sPduTypeSubmit;

typedef struct
{
	unsigned char MC: 2;
	unsigned char CS: 2;
	unsigned char DCI: 4;
}sDcs;

typedef struct
{
	unsigned char length;
	unsigned char IEI;
	unsigned char IEDL;
	unsigned char refNo;
	unsigned char maxNo;
	unsigned char seqNo;
}sUdhConcatSms;

typedef struct
{
	sAddrField smscAddrField;
	sPduTypeDeliver pduType;
	sAddrField origAddrField;
	unsigned char pid;
	sDcs dcs;
	unsigned char scts[SCTS_HEX_STR_LEN + 1];
	unsigned char udl;
	sUdhConcatSms udh;
	unsigned char ud[UD_HEX_STR_LEN + 1];
} sPduHeaderDeliver;

typedef struct
{
	sAddrField smscAddrField;
	sPduTypeSubmit pduType;
	unsigned char mr;
	sAddrField destAddrField;
	unsigned char pid;
	sDcs dcs;
	unsigned char vp;
	unsigned char udl;
	sUdhConcatSms udh;
	unsigned char ud[UD_HEX_STR_LEN + 1];
} sPduHeaderSubmit;

/* Message Type Indicator */
#define MTI_SMS_DELIVER						0x00
#define MTI_SMS_SUBMIT						0x01
#define MTI_SMS_STATUS_REPORT				0x02

/* Data Coding Scheme */
#define DCS_GSM_7BIT						0x00
#define DCS_GSM_8BIT						0x01
#define DCS_UCS2_16BIT						0x02

/* Validity Period Field */
#define VPF_RESERVED						0x01
#define VPF_RELATIVE						0x02
#define VPF_ABSOLUTE						0x03

/* Message Format */
#define MSG_FMT_PDU							0x00
#define MSG_FMT_TEXT						0x01


unsigned char *extractPartHexString(const char *pString, int stringLength)
{
	unsigned char *pTmpHexStr;

	//20200427 Yilun add that if stringLength < 0, cause some system crashes
	if(stringLength < 0)
	{
		stringLength=0;
	}

	pTmpHexStr = (unsigned char *)calloc(stringLength+1, sizeof(unsigned char));
	if(pTmpHexStr == NULL)
	{
		printf("pTmpHexStr calloc error.\n");
		return NULL;
	}
	strncpy((char *)pTmpHexStr, pString, stringLength);
	pTmpHexStr[stringLength] = '\0';

	return pTmpHexStr;
}

unsigned char *hexStringToByte(unsigned char *pString)
{
	unsigned char *buffers, *pBuffers;

	buffers = (unsigned char *)calloc(strlen((const char *)pString)/2+1, sizeof(unsigned char));
	if(buffers == NULL)
	{
		printf("buffers calloc error.\n");
		return NULL;
	}

	pBuffers = buffers;
	while(*pString != '\0')
	{
		sscanf((const char *)pString, "%2hhx", &(*pBuffers));
		pString += 2;
		pBuffers += 1;
	}

	return buffers;
}

char *byteToHexString(unsigned char *pByte, int width)
{
	char *buffers, *pBuffers;

	buffers = (char *)calloc(strlen((const char *)pByte)*width+1, sizeof(char));
	if(buffers == NULL)
	{
		printf("buffers calloc error.\n");
		return NULL;
	}

	pBuffers = buffers;
	while(*pByte != '\0')
	{
		sprintf(pBuffers, "%0*hhx", width, *(const char *)pByte);
		pByte += 1;
		pBuffers += width;
	}

	return buffers;
}

static unsigned char *septet2OctetStr(unsigned char *pdu, unsigned len)
{
	char *buf = NULL;
	unsigned size = 0;
	unsigned i = 0;
	unsigned j = 0;
	unsigned k = 0;

	if (len == 0)
		return NULL;

	size = (len * 8 + 8) / 7 + 1;
	buf = (char *)calloc(size, sizeof(char));

	if (buf == NULL)
	{
		printf("buffers calloc error.\n");
		return NULL;
	}

	for (i = 0, k = 0; i < len && j < size; i++, k++)
	{
		if (k == 0)
		{
			buf[j++] = pdu[i] & 0x7F;
			continue;
		}

		buf[j++] = ((pdu[i] << k) & 0x7F) | ((pdu[i - 1]) >> (8 - k));

		k %= 7;
	}

	if (j < size - 1)
		buf[j++] = pdu[i - 1] >> (8 - k);

	return buf;
}

void swapChar(unsigned char *pBuffer1, unsigned char *pBuffer2)
{
	unsigned char tmpBuffer = *pBuffer1;
	*pBuffer1 = *pBuffer2;
	*pBuffer2 = tmpBuffer;
}

void swapCharOfString(unsigned char *pString)
{
	unsigned char *pOffset = pString;

	while(*pOffset != '\0')
	{
		swapChar(pOffset, pOffset+1);
		pOffset += 2;
	}
}

void showBytes(unsigned char *pBuffer)
{
	int i;

	printf("hex: ");
	for(i=0; i < strlen((const char *)pBuffer); i++)
	{
		printf("%4x", *(pBuffer+i) );
	}
	printf("\n");

	printf("dig: ");
	for(i=0; i < strlen((const char *)pBuffer); i++)
	{
		printf("%4d", *(pBuffer+i) );
	}
	printf("\n");
}

void showPduHeaderDeliverInfo(sPduHeaderDeliver *pPduHeaderDeliver)
{
	printf("------------------------------------------------------------\n");
	printf("pPduHeaderDeliver->smscAddrField.addrLength: %d\n", pPduHeaderDeliver->smscAddrField.addrLength);
	if(pPduHeaderDeliver->smscAddrField.addrLength != 0)
	{
		printf("pPduHeaderDeliver->smscAddrField.addrType: 0x%x\n", pPduHeaderDeliver->smscAddrField.addrType);
		printf("pPduHeaderDeliver->smscAddrField.addrValue: %s\n", pPduHeaderDeliver->smscAddrField.addrValue);
	}

	printf("pPduHeaderDeliver->pduType.MTI: %d\n", pPduHeaderDeliver->pduType.MTI);
	printf("pPduHeaderDeliver->pduType.MMS: %d\n", pPduHeaderDeliver->pduType.MMS);
	printf("pPduHeaderDeliver->pduType.LP: %d\n", pPduHeaderDeliver->pduType.LP);
	printf("pPduHeaderDeliver->pduType.RESERVED: %d\n", pPduHeaderDeliver->pduType.RESERVED);
	printf("pPduHeaderDeliver->pduType.SRI: %d\n", pPduHeaderDeliver->pduType.SRI);
	printf("pPduHeaderDeliver->pduType.UDHI: %d\n", pPduHeaderDeliver->pduType.UDHI);
	printf("pPduHeaderDeliver->pduType.RP: %d\n", pPduHeaderDeliver->pduType.RP);
	switch(pPduHeaderDeliver->pduType.MTI)
	{
		case MTI_SMS_DELIVER:
			printf("MTI_SMS_DELIVER\n");
			break;
		case MTI_SMS_SUBMIT:
			printf("MTI_SMS_SUBMIT\n");
			break;
		case MTI_SMS_STATUS_REPORT:
			printf("MTI_SMS_STATUS_REPORT\n");
			break;
	}

	printf("pPduHeaderDeliver->origAddrField.addrLength: %d\n", pPduHeaderDeliver->origAddrField.addrLength);
	if(pPduHeaderDeliver->origAddrField.addrLength != 0)
	{
		printf("pPduHeaderDeliver->origAddrField.addrType: 0x%x\n", pPduHeaderDeliver->origAddrField.addrType);
		printf("pPduHeaderDeliver->origAddrField.addrValue: %s\n", pPduHeaderDeliver->origAddrField.addrValue);
	}

	printf("pPduHeaderDeliver->pid: %d\n", pPduHeaderDeliver->pid);

	printf("pPduHeaderDeliver->dcs.CS: %d\n", pPduHeaderDeliver->dcs.CS);
	switch(pPduHeaderDeliver->dcs.CS)
	{
		case DCS_GSM_7BIT:
			printf("DCS_GSM_7BIT\n");
			break;
		case DCS_GSM_8BIT:
			printf("DCS_GSM_8BIT\n");
			break;
		case DCS_UCS2_16BIT:
			printf("DCS_UCS2_16BIT\n");
			break;
	}

	printf("pPduHeaderDeliver->scts: %s\n", pPduHeaderDeliver->scts);

	printf("pPduHeaderDeliver->udl: %d\n", pPduHeaderDeliver->udl);

	if(pPduHeaderDeliver->pduType.UDHI)
	{
		printf("pPduHeaderDeliver->udh.length: %d\n", pPduHeaderDeliver->udh.length);
		printf("pPduHeaderDeliver->udh.IEI: %d\n", pPduHeaderDeliver->udh.IEI);
		printf("pPduHeaderDeliver->udh.IEDL: %d\n", pPduHeaderDeliver->udh.IEDL);
		printf("pPduHeaderDeliver->udh.refNo: %d\n", pPduHeaderDeliver->udh.refNo);
		printf("pPduHeaderDeliver->udh.maxNo: %d\n", pPduHeaderDeliver->udh.maxNo);
		printf("pPduHeaderDeliver->udh.seqNo: %d\n", pPduHeaderDeliver->udh.seqNo);
	}

	printf("pPduHeaderDeliver->ud: %s\n", pPduHeaderDeliver->ud);

	printf("------------------------------------------------------------\n");
}

int addrLengthComplement(int addrLength)
{
	int tmpAddrLength = addrLength;

	if(addrLength%2 != 0)
		tmpAddrLength += 1;

	return tmpAddrLength;
}

void removeAddrComplement(unsigned char *pAddress, int addrLength)
{
	if(pAddress[addrLength] == ADDR_COMPLEMENT_CHAR)
		pAddress[addrLength] = '\0';
}

char *appendAddrComplement(char *pAddress)
{
	char *buffers;
	int tmpAddrLength = strlen(pAddress);

	if(tmpAddrLength%2 == 0)
		return pAddress;

	buffers = (char *)calloc(tmpAddrLength+1+1, sizeof(char));
	if(buffers == NULL)
	{
		printf("buffers calloc error.\n");
		return NULL;
	}

	strncpy(buffers, pAddress, tmpAddrLength);
	buffers[tmpAddrLength] = ADDR_COMPLEMENT_CHAR;
	buffers[tmpAddrLength + 1] = '\0';

	return buffers;
}

unsigned char *timeStampFormat(const char *pString)
{
	int i = 0;
	unsigned char *buffers, *pBuffers, dividingMark[]={'/', '/', ',', ':', ':', '+'};

	buffers = (unsigned char *)calloc(TIME_STAMP_FORMAT_STR_LEN+1, sizeof(unsigned char));
	if(buffers == NULL)
	{
		printf("buffers calloc error.\n");
		return NULL;
	}

	pBuffers = buffers;
	while(*pString != '\0')
	{
		strncpy((char *)pBuffers, pString, 2);
		pString += 2;
		pBuffers += 2;
		*pBuffers = dividingMark[i++];
		pBuffers++;
	}
	buffers[TIME_STAMP_FORMAT_STR_LEN] = '\0';

	return buffers;

}

int fetchData(const char *pHexString, void *pData, int stringLength)
{
	unsigned char *pBytes, *pPartHexStr;

	pPartHexStr = extractPartHexString(pHexString, stringLength);
	if(pPartHexStr == NULL)
	{
		printf("extractPartHexString error.\n");
		return -1;
	}

	pBytes = hexStringToByte(pPartHexStr);
	if(pBytes == NULL)
	{
		printf("hexStringToByte error.\n");
		return -1;
	}

	/* showBytes(pBytes); */

	memcpy(pData, pBytes, stringLength/2);

	free(pBytes);
	free(pPartHexStr);
	return 0;
}

int combineData(unsigned char *pBytes, void *pData)
{
	int width = 2;
	char *pHexString;

	pHexString = byteToHexString(pBytes, width);
	if(pHexString == NULL)
	{
		printf("byteToHexString error.\n");
		return -1;
	}

	strcat(pData, (const char *)pHexString);

	free(pHexString);

	return 0;
}

int swapData(const char *pHexString, void *pData, int stringLength)
{
	unsigned char *pPartHexStr;

	pPartHexStr = extractPartHexString(pHexString, stringLength);
	if(pPartHexStr == NULL)
	{
		printf("extractPartHexString error.\n");
		return -1;
	}

	swapCharOfString(pPartHexStr);

	memcpy(pData, pPartHexStr, stringLength+1);

	free(pPartHexStr);
	return 0;
}

int copyData(const char *pHexString, void *pData, int stringLength)
{
	unsigned char *pPartHexStr;

	pPartHexStr = extractPartHexString(pHexString, stringLength);
	if(pPartHexStr == NULL)
	{
		printf("extractPartHexString error.\n");
		return -1;
	}

	/* memcpy(pData, pPartHexStr, strlen((const char *)pPartHexStr)); */
	memcpy(pData, pPartHexStr, stringLength+1);

	free(pPartHexStr);
	return 0;
}

int pduHeaderDeliverDecoder(const char *pHexString, sPduHeaderDeliver *pData)
{
	int ret, offset, addrSwapLength, addrOffset, udhOffset;
	char *pOctetStr = NULL;

	ret = fetchData(pHexString, &(pData->smscAddrField.addrLength), ADDR_LEN_HEX_STR_LEN);
	if(ret == -1)
	{
		printf("fetch SmccAddrLen error.\n");
		return ret;
	}

	offset = ADDR_LEN_HEX_STR_LEN;
	if(pData->smscAddrField.addrLength != 0)
	{
		/* addrOffset = ADDR_LEN_HEX_STR_LEN; */
		addrOffset = offset;
		ret = fetchData(pHexString+addrOffset, &(pData->smscAddrField.addrType), ADDR_TYPE_HEX_STR_LEN);
		if(ret == -1)
		{
			printf("fetch smscAddrField.addrType error.\n");
			return ret;
		}

		addrOffset += ADDR_TYPE_HEX_STR_LEN;
		ret = swapData(pHexString+addrOffset, &(pData->smscAddrField.addrValue),
				(pData->smscAddrField.addrLength * HEX_STR_OCTET_LEN)-ADDR_TYPE_HEX_STR_LEN);
		if(ret == -1)
		{
			printf("fetch smscAddrField.addrValue error.\n");
			return ret;
		}

		offset = ADDR_TYPE_HEX_STR_LEN+(pData->smscAddrField.addrLength * HEX_STR_OCTET_LEN);
	}

	/* offset = ADDR_LEN_HEX_STR_LEN+(pData->smscAddrField.addrLength * HEX_STR_OCTET_LEN); */
	ret = fetchData(pHexString+offset, &(pData->pduType), PDU_TYPE_HEX_STR_LEN);
	if(ret == -1)
	{
		printf("fetch pduType error.\n");
		return ret;
	}

	offset += PDU_TYPE_HEX_STR_LEN;
	ret = fetchData(pHexString+offset, &(pData->origAddrField.addrLength), ADDR_LEN_HEX_STR_LEN);
	if(ret == -1)
	{
		printf("fetch origAddrField.addrLength error.\n");
		return ret;
	}

	offset += ADDR_LEN_HEX_STR_LEN;
	if(pData->origAddrField.addrLength != 0)
	{
		addrOffset = offset;
		ret = fetchData(pHexString+addrOffset, &(pData->origAddrField.addrType), ADDR_TYPE_HEX_STR_LEN);
		if(ret == -1)
		{
			printf("fetch origAddrField.addrType error.\n");
			return ret;
		}

		addrOffset += ADDR_TYPE_HEX_STR_LEN;
		addrSwapLength = addrLengthComplement(pData->origAddrField.addrLength);
	   	if ((pData->origAddrField.addrType & 0x70) == 0x50)
		{
			/* alphanumeric addresses by 7bit encoding */
			if ((ret = fetchData(pHexString+addrOffset, &(pData->origAddrField.addrValue), addrSwapLength)) == 0 &&
				(pOctetStr = septet2OctetStr(pData->origAddrField.addrValue, strlen(pData->origAddrField.addrValue))) != NULL)
			{
				snprintf(pData->origAddrField.addrValue, sizeof(pData->origAddrField.addrValue), "%s", pOctetStr);
				free(pOctetStr);
			}
		}
		else
		{
			/* phone number */
			ret = swapData(pHexString+addrOffset, &(pData->origAddrField.addrValue), addrSwapLength);
			removeAddrComplement(pData->origAddrField.addrValue, pData->origAddrField.addrLength);
		}
				/* (pData->origAddrField.addrLength * HEX_STR_OCTET_LEN)-ADDR_TYPE_HEX_STR_LEN); */
		if(ret == -1)
		{
			printf("fetch origAddrField.addrValue error.\n");
			return ret;
		}

		offset += ADDR_TYPE_HEX_STR_LEN + addrSwapLength;
	}

	ret = fetchData(pHexString+offset, &(pData->pid), PID_HEX_STR_LEN);
	if(ret == -1)
	{
		printf("fetch pid error.\n");
		return ret;
	}

	offset += PID_HEX_STR_LEN;
	ret = fetchData(pHexString+offset, &(pData->dcs), DCS_HEX_STR_LEN);
	if(ret == -1)
	{
		printf("fetch dcs error.\n");
		return ret;
	}

	offset += DCS_HEX_STR_LEN;
	ret = swapData(pHexString+offset, &(pData->scts), SCTS_HEX_STR_LEN);
	if(ret == -1)
	{
		printf("fetch scts error.\n");
		return ret;
	}

	offset += SCTS_HEX_STR_LEN;
	ret = fetchData(pHexString+offset, &(pData->udl), UDL_HEX_STR_LEN);
	if(ret == -1)
	{
		printf("fetch udl error.\n");
		return ret;
	}

	offset += UDL_HEX_STR_LEN;
	if(pData->pduType.UDHI)
	{
		udhOffset = offset;
		ret = fetchData(pHexString+udhOffset, &(pData->udh.length), UDH_LEN_HEX_STR_LEN);
		if(ret == -1)
		{
			printf("fetch udh.length error.\n");
			return ret;
		}

		udhOffset += UDH_LEN_HEX_STR_LEN;
		ret = fetchData(pHexString+udhOffset, &(pData->udh.IEI), UDH_IEI_HEX_STR_LEN);
		if(ret == -1)
		{
			printf("fetch udh.IEI error.\n");
			return ret;
		}

		udhOffset += UDH_IEI_HEX_STR_LEN;
		ret = fetchData(pHexString+udhOffset, &(pData->udh.IEDL), UDH_IEDL_HEX_STR_LEN);
		if(ret == -1)
		{
			printf("fetch udh.IEDL error.\n");
			return ret;
		}

		udhOffset += UDH_IEDL_HEX_STR_LEN;
		ret = fetchData(pHexString+udhOffset, &(pData->udh.refNo), UDH_REF_NO_HEX_STR_LEN);
		if(ret == -1)
		{
			printf("fetch udh.refNo error.\n");
			return ret;
		}

		udhOffset += UDH_REF_NO_HEX_STR_LEN;
		ret = fetchData(pHexString+udhOffset, &(pData->udh.maxNo), UDH_MAX_NO_HEX_STR_LEN);
		if(ret == -1)
		{
			printf("fetch udh.maxNo error.\n");
			return ret;
		}

		udhOffset += UDH_MAX_NO_HEX_STR_LEN;
		ret = fetchData(pHexString+udhOffset, &(pData->udh.seqNo), UDH_SEQ_NO_HEX_STR_LEN);
		if(ret == -1)
		{
			printf("fetch udh.seqNo error.\n");
			return ret;
		}

		offset += UDH_LEN_HEX_STR_LEN + (pData->udh.length * HEX_STR_OCTET_LEN);
	}

	ret = copyData(pHexString+offset, &(pData->ud),
			(pData->udl * HEX_STR_OCTET_LEN)-UDL_HEX_STR_LEN);
	if(ret == -1)
	{
		printf("fetch ud error.\n");
		return ret;
	}

	return 0;
}

int pduHeaderSubmitEecoder(int characterSet, int contentLength, char *pMsgTo, char *pMsgContent, char *pData)
{
	int ret;
	sPduHeaderSubmit pduHeaderSubmit;

	pData[0] = '\0';
	memset(&pduHeaderSubmit, 0, sizeof(sPduHeaderSubmit));

	//smscAddrField.addrLength: 0
	strcat(pData, ZERO_HEX_STR);

	pduHeaderSubmit.pduType.MTI = MTI_SMS_SUBMIT;
	pduHeaderSubmit.pduType.VPF = VPF_RELATIVE;
	ret = combineData((unsigned char *)&pduHeaderSubmit.pduType, pData);
	if(ret == -1)
	{
		printf("combine pduType error.\n");
		return ret;
	}

	//mr: 0
	strcat(pData, ZERO_HEX_STR);

	pduHeaderSubmit.destAddrField.addrLength = strlen(pMsgTo);
	/* printf("pduHeaderSubmit.destAddrField.addrLength: %d\n", pduHeaderSubmit.destAddrField.addrLength); */
	pduHeaderSubmit.destAddrField.addrType = (pMsgTo[0] == '0') ? ADDR_TYPE_NATIONAL : ADDR_TYPE_INTERNATIONAL;
	ret = combineData((unsigned char *)&pduHeaderSubmit.destAddrField, pData);
	if(ret == -1)
	{
		printf("combine destAddrField error.\n");
		return ret;
	}

	strcpy((char *)&(pduHeaderSubmit.destAddrField.addrValue), appendAddrComplement(pMsgTo));
	/* printf("pduHeaderSubmit.destAddrField.addrValue: %s\n", pduHeaderSubmit.destAddrField.addrValue); */
	swapCharOfString(pduHeaderSubmit.destAddrField.addrValue);
	strcat(pData, (const char *)pduHeaderSubmit.destAddrField.addrValue);

	//pid: 0
	strcat(pData, ZERO_HEX_STR);

	if(characterSet == DCS_GSM_7BIT)
	{
		strcat(pData, ZERO_HEX_STR);
	}
	else
	{
		pduHeaderSubmit.dcs.CS = characterSet;
		ret = combineData((unsigned char *)&pduHeaderSubmit.dcs, pData);
		if(ret == -1)
		{
			printf("combine dcs error.\n");
			return ret;
		}
	}

	pduHeaderSubmit.vp = VP_DAY(1);
	ret = combineData((unsigned char *)&pduHeaderSubmit.vp, pData);
	if(ret == -1)
	{
		printf("combine vp error.\n");
		return ret;
	}

	if(characterSet == DCS_GSM_7BIT)
		pduHeaderSubmit.udl = contentLength;
	else
		pduHeaderSubmit.udl = strlen(pMsgContent)/2;
	ret = combineData((unsigned char *)&pduHeaderSubmit.udl, pData);
	if(ret == -1)
	{
		printf("combine udl error.\n");
		return ret;
	}

	strcpy((char *)&(pduHeaderSubmit.ud), pMsgContent);
	strcat(pData, (const char *)pduHeaderSubmit.ud);

	return 0;
}


typedef struct base
{
	int objIndex;
	int msgIndex;
	int state;
	char from[ADDR_VALUE_HEX_STR_LEN/2 + 1];
	char timeStamp[TIME_STAMP_FORMAT_STR_LEN + 1];
	int characterSet;
	int udhi;
	int msgID;
	int msgSegmentSequence;
	int msgSegmentInTotal;
}sBase;
typedef struct singleEntry
{
	sBase data;
	char content[SINGLE_CONTENT_STR_LEN + 1];
	LIST_ENTRY(singleEntry) singleList;
}sSingleEntry;
typedef struct concatEntry
{
	sBase data;
	char concatObjIndex[CONCAT_INDEX_STR_LEN + 1];
	char concatMsgIndex[CONCAT_INDEX_STR_LEN + 1];
	char concatContent[CONCAT_CONTENT_STR_LEN + 1];
	LIST_ENTRY(concatEntry) concatList;
}sConcatEntry;
typedef struct msgIdEntry
{
	int msgId;
	LIST_ENTRY(msgIdEntry) msgIdList;
}sMsgIdEntry;

LIST_HEAD(single_listhead, singleEntry) singleListHead;
LIST_HEAD(concat_listhead, concatEntry) concatListHead;
LIST_HEAD(msgid_listhead, msgIdEntry) msgIdListHead;

void insertSingleList(sSingleEntry *entry){
	sSingleEntry *n1 = NULL;
	n1 = malloc(sizeof(sSingleEntry));
	memcpy(n1, entry, sizeof(sSingleEntry));
	LIST_INSERT_HEAD(&singleListHead, n1, singleList);
}
void traversalSingleList(void){
	sSingleEntry *np = NULL;
	printf("---- [%d] %s ----\n", __LINE__, __func__);
	printf("OI\tMI\tST\tCS\tDI\tRN\tSN\tMN\tFM\t\tTS\n");
	LIST_FOREACH(np, &singleListHead, singleList){
		printf("%d\t", np->data.objIndex);
		printf("%d\t", np->data.msgIndex);
		printf("%d\t", np->data.state);
		printf("%d\t", np->data.characterSet);
		printf("%d\t", np->data.udhi);
		printf("%d\t", np->data.msgID);
		printf("%d\t", np->data.msgSegmentSequence);
		printf("%d\t", np->data.msgSegmentInTotal);
		printf("%s\t", np->data.from);
		printf("%s\t", np->data.timeStamp);
		printf("\n");
		printf("%s\n", np->content);
	}
}
void removeSingleList(void){
	sSingleEntry *n1 = NULL;
	while(!LIST_EMPTY(&singleListHead)){
		n1 = LIST_FIRST(&singleListHead);
		LIST_REMOVE(n1, singleList);
		free(n1);
	}
}

void insertConcatList(sConcatEntry *entry){
	sConcatEntry *n1 = NULL;
	n1 = malloc(sizeof(sConcatEntry));
	memcpy(n1, entry, sizeof(sConcatEntry));
	LIST_INSERT_HEAD(&concatListHead, n1, concatList);
}
void traversalConcatList(void){
	sConcatEntry *np = NULL;
	printf("---- [%d] %s ----\n", __LINE__, __func__);
	printf("OI\tMI\tST\tCS\tDI\tRN\tSN\tMN\tFM\t\tTS\n");
	LIST_FOREACH(np, &concatListHead, concatList){
		printf("%d\t", np->data.objIndex);
		printf("%d\t", np->data.msgIndex);
		printf("%d\t", np->data.state);
		printf("%d\t", np->data.characterSet);
		printf("%d\t", np->data.udhi);
		printf("%d\t", np->data.msgID);
		printf("%d\t", np->data.msgSegmentSequence);
		printf("%d\t", np->data.msgSegmentInTotal);
		printf("%s\t", np->data.from);
		printf("%s\t", np->data.timeStamp);
		printf("\n");
		printf("%s\n", np->concatObjIndex);
		printf("%s\n", np->concatMsgIndex);
		printf("%s\n", np->concatContent);
	}
}
void removeConcatList(void){
	sConcatEntry *n1 = NULL;
	while(!LIST_EMPTY(&concatListHead)){
		n1 = LIST_FIRST(&concatListHead);
		LIST_REMOVE(n1, concatList);
		free(n1);
	}
}

bool isConcat(int compareMsgId){
	sMsgIdEntry *np = NULL;
	LIST_FOREACH(np, &msgIdListHead, msgIdList)
		if(compareMsgId == np->msgId)
			return true;
	return false;
}
void insertMsgIdList(int matchedMsgId){
	sMsgIdEntry *n1 = NULL;
	if(isConcat(matchedMsgId))
		return ;
	n1 = malloc(sizeof(sMsgIdEntry));
	n1->msgId = matchedMsgId;
	LIST_INSERT_HEAD(&msgIdListHead, n1, msgIdList);
}
void gatherMsgIdList(void){
	sConcatEntry *np = NULL;
	/* printf("---- [%d] %s ----\n", __LINE__, __func__); */
	LIST_FOREACH(np, &concatListHead, concatList)
		insertMsgIdList(np->data.msgID);
}
void traversalMsgIdList(void){
	sMsgIdEntry *np = NULL;
	printf("---- [%d] %s ----\n", __LINE__, __func__);
	LIST_FOREACH(np, &msgIdListHead, msgIdList)
		printf("%d ", np->msgId);
	printf("\n");
}
void removeMsgIdList(void){
	sMsgIdEntry *n1 = NULL;
	while(!LIST_EMPTY(&msgIdListHead)){
		n1 = LIST_FIRST(&msgIdListHead);
		LIST_REMOVE(n1, msgIdList);
		free(n1);
	}
}

sConcatEntry *searchConcatList(int msgId, int seq){
	sConcatEntry *np = NULL;
	LIST_FOREACH(np, &concatListHead, concatList){
		if(np->data.msgID == msgId
			&& np->data.msgSegmentSequence == seq){
				return np;
		}
	}
}
int convertContent(char *pData, unsigned char *pBytes)
{
	int width = 4;
	char *pHexString;

	pHexString = byteToHexString(pBytes, width);
	if(pHexString == NULL)
	{
		printf("byteToHexString error.\n");
		return -1;
	}

	strcat(pData, (const char *)pHexString);

	free(pHexString);

	return 0;
}
int combineContent(sConcatEntry *n1, sConcatEntry *npConcat)
{
	if(n1->data.characterSet != npConcat->data.characterSet)
		if(n1->data.characterSet == DCS_UCS2_16BIT
			&& npConcat->data.characterSet == DCS_GSM_7BIT)
			return convertContent(n1->concatContent, npConcat->concatContent);
		else
			printf("[%d] F:%s unsupported case!\n", __LINE__, __func__);
	else
		strcat(n1->concatContent, npConcat->concatContent);

	return 0;
}
void combineIndex(sConcatEntry *n1, char *objIndex, char *msgIndex)
{
	strcat(n1->concatObjIndex, objIndex);
	strcat(n1->concatObjIndex, CONCAT_COMMA_CHAR);
	strcat(n1->concatMsgIndex, msgIndex);
	strcat(n1->concatMsgIndex, CONCAT_COMMA_CHAR);
}
void mergeConcatList(void){
	int msgSn;
	sConcatEntry *n1, *npConcat = NULL;
	sMsgIdEntry *npMsgId = NULL;
	char tmpConcatObjIndex[CONCAT_INDEX_STR_LEN/CONCAT_CONTENT_MAX_ENTRY + 1];
	char tmpConcatMsgIndex[CONCAT_INDEX_STR_LEN/CONCAT_CONTENT_MAX_ENTRY + 1];

	/* printf("---- [%d] %s ----\n", __LINE__, __func__); */
	LIST_FOREACH(npMsgId, &msgIdListHead, msgIdList){
		/* printf("OI\tMI\tST\tCS\tRN\tSN\tMN\n"); */
		msgSn = 1;
		while((npConcat = searchConcatList(npMsgId->msgId, msgSn)) != NULL
			&& msgSn <= npConcat->data.msgSegmentInTotal){
			/* printf("%d\t", npConcat->data.objIndex); */
			/* printf("%d\t", npConcat->data.msgIndex); */
			/* printf("%d\t", npConcat->data.state); */
			/* printf("%d\t", npConcat->data.characterSet); */
			/* printf("%d\t", npConcat->data.msgID); */
			/* printf("%d\t", npConcat->data.msgSegmentSequence); */
			/* printf("%d\t", npConcat->data.msgSegmentInTotal); */
			/* printf("\n"); */
			tmpConcatObjIndex[0] = '\0';
			sprintf(tmpConcatObjIndex, "%d", npConcat->data.objIndex);
			tmpConcatMsgIndex[0] = '\0';
			sprintf(tmpConcatMsgIndex, "%d", npConcat->data.msgIndex);
			if(msgSn > CONCAT_CONTENT_MAX_ENTRY){
				combineIndex(npConcat, tmpConcatObjIndex, tmpConcatMsgIndex);
				msgSn++;
				continue;
			}
			if(msgSn == 1){
				n1 = npConcat;
			}
			else{
				if(combineContent(n1, npConcat) == -1)
					printf("[%d] F:%s combine content error!\n", __LINE__, __func__);
				LIST_REMOVE(npConcat, concatList);
				free(npConcat);
			}
			combineIndex(n1, tmpConcatObjIndex, tmpConcatMsgIndex);
			msgSn++;
		}
		/* printf("%s\n", n1->concatObjIndex); */
		/* printf("%s\n", n1->concatMsgIndex); */
		/* printf("%s\n", n1->concatContent); */
	}
}
json_object *convertJsonArray(void){
	sSingleEntry *npSingle = NULL;
	sConcatEntry *npConcat = NULL;
	json_object *tmpJobj = NULL, *tmpJarray = NULL;

	tmpJarray = json_object_new_array();
	LIST_FOREACH(npSingle, &singleListHead, singleList){
		tmpJobj = json_object_new_object();
		Jaddi(tmpJobj, "ObjIndex", npSingle->data.objIndex);
		Jaddi(tmpJobj, "MsgIndex", npSingle->data.msgIndex);
		Jaddi(tmpJobj, "MsgID", npSingle->data.msgID);
		Jaddi(tmpJobj, "State", npSingle->data.state);
		Jadds(tmpJobj, "From", npSingle->data.from);
		Jadds(tmpJobj, "TimeStamp", npSingle->data.timeStamp);
		Jaddi(tmpJobj, "CharacterSet", npSingle->data.characterSet);
		Jadds(tmpJobj, "Content", npSingle->content);
		json_object_array_add(tmpJarray, JSON_OBJ_COPY(tmpJobj));
		zcfgFeJsonObjFree(tmpJobj);
	}
	LIST_FOREACH(npConcat, &concatListHead, concatList){
		tmpJobj = json_object_new_object();
		Jadds(tmpJobj, "ObjIndex", npConcat->concatObjIndex);
		Jadds(tmpJobj, "MsgIndex", npConcat->concatMsgIndex);
		Jaddi(tmpJobj, "MsgID", npConcat->data.msgID);
		Jaddi(tmpJobj, "State", npConcat->data.state);
		Jadds(tmpJobj, "From", npConcat->data.from);
		Jadds(tmpJobj, "TimeStamp", npConcat->data.timeStamp);
		Jaddi(tmpJobj, "CharacterSet", npConcat->data.characterSet);
		Jadds(tmpJobj, "Content", npConcat->concatContent);
		json_object_array_add(tmpJarray, JSON_OBJ_COPY(tmpJobj));
		zcfgFeJsonObjFree(tmpJobj);
	}
	return tmpJarray;
}

zcfgRet_t zcfgFeDalCellWanSmsGet(struct json_object *Jobj, struct json_object *Jarray, char *replyMsg){
	zcfgRet_t ret = ZCFG_SUCCESS;
	objIndex_t iid = {0};
	json_object *paramJobj = NULL, *intfZySmsObj = NULL, *intfZySmsStorageObj = NULL;
	json_object *intfZySmsInboxObj = NULL, *intfZySmsInboxJarray = NULL;
	char *pTimeStampFormat = NULL;
	sPduHeaderDeliver pduHeaderDeliver;
	paramJobj = json_object_new_object();


	IID_INIT(iid);
	iid.level=1;
	iid.idx[0]=1;
	if((ret =  zcfgFeObjJsonGet(RDM_OID_CELL_INTF_ZY_SMS, &iid, &intfZySmsObj)) == ZCFG_SUCCESS){
		json_object_object_add(paramJobj, "SMS_Enable", JSON_OBJ_COPY(json_object_object_get(intfZySmsObj, "Enable")));
		zcfgFeJsonObjFree(intfZySmsObj);
	}

	IID_INIT(iid);
	iid.level=1;
	iid.idx[0]=1;
	if((ret =  zcfgFeObjJsonGet(RDM_OID_CELL_INTF_ZY_SMS_STORAGE, &iid, &intfZySmsStorageObj)) == ZCFG_SUCCESS){
		json_object_object_add(paramJobj, "SMS_UsedSpace", JSON_OBJ_COPY(json_object_object_get(intfZySmsStorageObj, "UsedSpace")));
		json_object_object_add(paramJobj, "SMS_TotalSpace", JSON_OBJ_COPY(json_object_object_get(intfZySmsStorageObj, "TotalSpace")));
		zcfgFeJsonObjFree(intfZySmsStorageObj);
	}

	LIST_INIT(&singleListHead);
	LIST_INIT(&concatListHead);
	LIST_INIT(&msgIdListHead);

	IID_INIT(iid);
	while(zcfgFeObjJsonGetNext(RDM_OID_CELL_INTF_ZY_SMS_INBOX, &iid, &intfZySmsInboxObj) == ZCFG_SUCCESS) {
		if(!strcmp(Jgets(intfZySmsInboxObj, "PduContent"), ""))
			continue;

		memset(&pduHeaderDeliver, 0, sizeof(sPduHeaderDeliver));
		ret = pduHeaderDeliverDecoder(Jgets(intfZySmsInboxObj, "PduContent"), &pduHeaderDeliver);
		if(ret == -1)
		{
			printf("pduHeaderDeliverDecoder error.\n");
			return ZCFG_INVALID_FORMAT;
		}
		/* showPduHeaderDeliverInfo(&pduHeaderDeliver); */

		pTimeStampFormat = timeStampFormat((const char *)pduHeaderDeliver.scts);
		if(pduHeaderDeliver.pduType.UDHI)
		{
			sConcatEntry concatEntry;
			memset(&concatEntry, 0, sizeof(sConcatEntry));
			concatEntry.data.msgIndex = Jgeti(intfZySmsInboxObj, "Index");
			concatEntry.data.state = Jgeti(intfZySmsInboxObj, "State");
			strcpy(concatEntry.data.from, pduHeaderDeliver.origAddrField.addrValue);
			strcpy(concatEntry.data.timeStamp, pTimeStampFormat);
			concatEntry.data.objIndex = iid.idx[1];
			concatEntry.data.characterSet = pduHeaderDeliver.dcs.CS;
			concatEntry.data.udhi = pduHeaderDeliver.pduType.UDHI;
			concatEntry.data.msgID = pduHeaderDeliver.udh.refNo;
			concatEntry.data.msgSegmentSequence = pduHeaderDeliver.udh.seqNo;
			concatEntry.data.msgSegmentInTotal = pduHeaderDeliver.udh.maxNo;
			strcpy(concatEntry.concatContent, Jgets(intfZySmsInboxObj, "TextContent"));
			insertConcatList(&concatEntry);
		}
		else
		{
			sSingleEntry singleEntry;
			memset(&singleEntry, 0, sizeof(sSingleEntry));
			singleEntry.data.msgIndex = Jgeti(intfZySmsInboxObj, "Index");
			singleEntry.data.state = Jgeti(intfZySmsInboxObj, "State");
			strcpy(singleEntry.data.from, pduHeaderDeliver.origAddrField.addrValue);
			pTimeStampFormat = timeStampFormat((const char *)pduHeaderDeliver.scts);
			strcpy(singleEntry.data.timeStamp, pTimeStampFormat);
			singleEntry.data.objIndex = iid.idx[1];
			singleEntry.data.characterSet = pduHeaderDeliver.dcs.CS;
			strcpy(singleEntry.content, Jgets(intfZySmsInboxObj, "TextContent"));
			insertSingleList(&singleEntry);
		}
		free(pTimeStampFormat);
	}

	gatherMsgIdList();
	mergeConcatList();
	intfZySmsInboxJarray = convertJsonArray();

	json_object_object_add(paramJobj, "SMS_Inbox", JSON_OBJ_COPY(intfZySmsInboxJarray));
	zcfgFeJsonObjFree(intfZySmsInboxJarray);

	json_object_array_add(Jarray, JSON_OBJ_COPY(paramJobj));
	zcfgFeJsonObjFree(paramJobj);

	/* traversalSingleList(); */
	/* traversalConcatList(); */
	/* traversalMsgIdList(); */
	removeSingleList();
	removeConcatList();
	removeMsgIdList();
	LIST_INIT(&singleListHead);
	LIST_INIT(&concatListHead);
	LIST_INIT(&msgIdListHead);

	return ret;
}

/* void zcfgFeDalShowCellWanSms(struct json_object *Jarray){ */
	/* struct json_object *obj = NULL; */

	/* if(json_object_get_type(Jarray) != json_type_array){ */
		/* printf("wrong Jobj format!\n"); */
		/* return; */
	/* } */

	/* obj = json_object_array_get_idx(Jarray, 0); */

	/* printf("\nCellular SMS Settings:\n"); */
	/* printf("%-30s %-10s \n","Enable:", json_object_get_string(json_object_object_get(obj, "SMS_Enable"))); */

/* } */

zcfgRet_t zcfgFeDalCellWanSmsEdit(struct json_object *Jobj, char *replyMsg){
	zcfgRet_t ret = ZCFG_SUCCESS;
	objIndex_t intfZySmsIid = {0};
	struct json_object *intfZySmsObj = NULL;
	bool smsEnable = false;
	bool smsInboxPurge = false;
	bool smsInboxRetrieve = false;

	IID_INIT(intfZySmsIid);
	intfZySmsIid.level=1;
	intfZySmsIid.idx[0]=1;

	smsEnable = json_object_get_boolean(json_object_object_get(Jobj, "SMS_Enable"));
	smsInboxPurge = json_object_get_boolean(json_object_object_get(Jobj, "SMS_InboxPurge"));
	smsInboxRetrieve = json_object_get_boolean(json_object_object_get(Jobj, "SMS_InboxRetrieve"));

	if((ret = zcfgFeObjJsonGetWithoutUpdate(RDM_OID_CELL_INTF_ZY_SMS, &intfZySmsIid, &intfZySmsObj)) == ZCFG_SUCCESS){
		if(json_object_object_get(Jobj, "SMS_Enable"))
			json_object_object_add(intfZySmsObj, "Enable", json_object_new_boolean(smsEnable));
		if(json_object_object_get(Jobj, "SMS_InboxPurge"))
			json_object_object_add(intfZySmsObj, "InboxPurge", json_object_new_boolean(smsInboxPurge));
		if(json_object_object_get(Jobj, "SMS_InboxRetrieve"))
			json_object_object_add(intfZySmsObj, "InboxRetrieve", json_object_new_boolean(smsInboxRetrieve));
		zcfgFeObjJsonSet(RDM_OID_CELL_INTF_ZY_SMS, &intfZySmsIid, intfZySmsObj, NULL);
	}

	zcfgFeJsonObjFree(intfZySmsObj);

	return ret;
}

zcfgRet_t zcfgFeDalCellWanSmsAdd(struct json_object *Jobj, char *replyMsg){
	zcfgRet_t ret = ZCFG_SUCCESS;
	zcfg_offset_t oid;
	objIndex_t intfZySmsOutboxIid = {0};
	struct json_object *intfZySmsOutboxObj = NULL;
	int smsFormat, dataCoding, smsPduLength, smsContentLength;
	char *smsCharacterSet, *smsTo, *smsContent, combineBufferSubmit[SINGLE_CONTENT_STR_LEN];

	IID_INIT(intfZySmsOutboxIid);
	intfZySmsOutboxIid.level=2;
	intfZySmsOutboxIid.idx[0]=1;
	intfZySmsOutboxIid.idx[1]=1;
	oid = RDM_OID_CELL_INTF_ZY_SMS_OUTBOX;
	if((ret = zcfgFeObjJsonGet(oid, &intfZySmsOutboxIid, &intfZySmsOutboxObj)) != ZCFG_SUCCESS){
		IID_INIT(intfZySmsOutboxIid);
		intfZySmsOutboxIid.level=1;
		intfZySmsOutboxIid.idx[0]=1;
		if((ret = zcfgFeObjJsonAdd(oid, &intfZySmsOutboxIid, NULL)) != ZCFG_SUCCESS){
			printf("[%d] %s: zcfgFeObjJsonAdd fail.\n", __LINE__, __func__);
			return ret;
		}
		if((ret = zcfgFeObjJsonGet(oid, &intfZySmsOutboxIid, &intfZySmsOutboxObj)) != ZCFG_SUCCESS){
			printf("[%d] %s: zcfgFeObjJsonGet fail.\n", __LINE__, __func__);
			return ret;
		}
	}

	smsFormat = Jgeti(Jobj, "SMS_Format");
	if(smsFormat == MSG_FMT_PDU){
		smsCharacterSet = Jgets(Jobj, "SMS_CharacterSet");
		switch(smsCharacterSet[0])
		{
			case 'G': //GSM default alphabet
				smsContentLength = Jgeti(Jobj, "SMS_ContentLength");
				dataCoding = DCS_GSM_7BIT;
				break;
			case 'I': //International reference alphabet
				dataCoding = DCS_GSM_8BIT;
				break;
			case 'U': //Unicode alphabet
				dataCoding = DCS_UCS2_16BIT;
				break;
		}
		smsTo = Jgets(Jobj, "SMS_To");
		smsContent = Jgets(Jobj, "SMS_Content");

		ret = pduHeaderSubmitEecoder(dataCoding, smsContentLength, smsTo, smsContent, combineBufferSubmit);
		if(ret == -1)
		{
			printf("pduHeaderSubmitEecoder error.\n");
			return ret;
		}

		smsPduLength = (strlen(combineBufferSubmit)-2)/2;
		/* printf("combineBufferSubmit: %s\n", combineBufferSubmit); */
		/* printf("strlen combineBufferSubmit: %zd\n", smsPduLength ); */
		json_object_object_add(intfZySmsOutboxObj, "PduLength", json_object_new_int(smsPduLength));
		json_object_object_add(intfZySmsOutboxObj, "Content", json_object_new_string(combineBufferSubmit));

	}
	else{
		replaceParam(intfZySmsOutboxObj, "Content", Jobj, "SMS_Content");
	}

	replaceParam(intfZySmsOutboxObj, "Format", Jobj, "SMS_Format");
	replaceParam(intfZySmsOutboxObj, "CharacterSet", Jobj, "SMS_CharacterSet");
	replaceParam(intfZySmsOutboxObj, "To", Jobj, "SMS_To");
	replaceParam(intfZySmsOutboxObj, "TimeStamp", Jobj, "SMS_TimeStamp");

	if((ret = zcfgFeObjJsonBlockedSet(oid, &intfZySmsOutboxIid, intfZySmsOutboxObj, NULL)) != ZCFG_SUCCESS){
		printf("[%d] %s: zcfgFeObjJsonBlockedSet fail.\n", __LINE__, __func__);
		if(ret == ZCFG_EXCEED_MAX_INS_NUM){
			if(replyMsg) strcpy(replyMsg, "Error: Over the upper bound!");
			json_object_object_add(Jobj, "__multi_lang_replyMsg", json_object_new_string("zylang.Broadband.CellWan.Sms.Input.Warning.over_the_upper_bound"));
		}
		zcfgFeJsonObjFree(intfZySmsOutboxObj);
		return ret;
	}

	zcfgFeJsonObjFree(intfZySmsOutboxObj);

	return ret;
}

int setSmsWaitingStatus(char *state)
{
	int ret = 0;
	objIndex_t intfZyUiWaitIid = {0};
	struct json_object *intfZyUiWaitObj = NULL;

	IID_INIT(intfZyUiWaitIid);
	intfZyUiWaitIid.level=1;
	intfZyUiWaitIid.idx[0]=1;

	if((ret = zcfgFeObjJsonGetWithoutUpdate(RDM_OID_CELL_INTF_ZY_UI_WAIT_ST, &intfZyUiWaitIid, &intfZyUiWaitObj)) == ZCFG_SUCCESS){
		json_object_object_add(intfZyUiWaitObj, "SmsWaitingStatus", json_object_new_string(state));
		zcfgFeObjJsonSet(RDM_OID_CELL_INTF_ZY_UI_WAIT_ST, &intfZyUiWaitIid, intfZyUiWaitObj, NULL);
	}

	zcfgFeJsonObjFree(intfZyUiWaitObj);

	return ret;
}
char *getSmsWaitingStatus(){
	zcfgRet_t ret;
	objIndex_t iid = {0};
	char waitStateSms[32];
	json_object *intfZyUiWaitStateObj = NULL;

	IID_INIT(iid);
	iid.level=1;
	iid.idx[0]=1;
	if((ret =  zcfgFeObjJsonGet(RDM_OID_CELL_INTF_ZY_UI_WAIT_ST, &iid, &intfZyUiWaitStateObj)) == ZCFG_SUCCESS){
		strcpy(waitStateSms, Jgets(intfZyUiWaitStateObj, "SmsWaitingStatus"));
		zcfgFeJsonObjFree(intfZyUiWaitStateObj);
	}
	return waitStateSms;
}
zcfgRet_t objDelByIndex(char *objIndex){
	zcfgRet_t ret = ZCFG_SUCCESS;
	objIndex_t iid = {0};
	zcfg_offset_t oid = RDM_OID_CELL_INTF_ZY_SMS_INBOX;
	const char delim[] = ",";
	const char smsLoading[] = "LOADING";
	char *ptr = strtok(objIndex, delim);
	int index;

	while(ptr != NULL)
	{
		setSmsWaitingStatus(smsLoading);
		index = atoi(ptr);
		IID_INIT(iid);
		iid.level=2;
		iid.idx[0]=1;
		iid.idx[1]=index;
		if((ret = zcfgFeObjJsonDel(oid, &iid, NULL)) != ZCFG_SUCCESS)
			printf("[%d] %s zcfgFeObjJsonDel fail.\n", __LINE__, __func__);
		while(strcmp(getSmsWaitingStatus(), smsLoading) == 0)
			sleep(1);

		IID_INIT(iid);
		iid.level=1;
		iid.idx[0]=1;
		if((ret = zcfgFeObjJsonAdd(oid, &iid, NULL)) != ZCFG_SUCCESS){
			printf("[%d] %s: zcfgFeObjJsonAdd fail.\n", __LINE__, __func__);
			/* return ret; */
		}
		ptr = strtok(NULL, delim);
	}

	return ret;
}
zcfgRet_t zcfgFeDalCellWanSmsDelete(struct json_object *Jobj, char *replyMsg){
	zcfgRet_t ret = ZCFG_SUCCESS;
	int objIndex;
	char objIndexDeleteBuffer[32];

	objIndexDeleteBuffer[0] = '\0';
	strcpy(objIndexDeleteBuffer, Jgets(Jobj, "objIndex"));

	if((ret = objDelByIndex(objIndexDeleteBuffer)) != ZCFG_SUCCESS)
		printf("[%d] %s objDelByIndex fail.\n", __LINE__, __func__);


	return ret;
}


zcfgRet_t zcfgFeDalCellWanSms(const char *method, struct json_object *Jobj,  struct json_object *Jarray, char *replyMsg){
	zcfgRet_t ret = ZCFG_SUCCESS;

	if(!strcmp(method, "GET"))
		ret = zcfgFeDalCellWanSmsGet(Jobj, Jarray, replyMsg);
	else if(!strcmp(method, "PUT"))
		ret = zcfgFeDalCellWanSmsEdit(Jobj, replyMsg);
	else if(!strcmp(method, "POST"))
		ret = zcfgFeDalCellWanSmsAdd(Jobj, replyMsg);
	else if(!strcmp(method, "DELETE"))
		ret = zcfgFeDalCellWanSmsDelete(Jobj, replyMsg);
	else
		printf("Unknown method:%s\n", method);

	return ret;
}

#endif

