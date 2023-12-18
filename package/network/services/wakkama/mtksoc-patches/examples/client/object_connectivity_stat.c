/*******************************************************************************
 *
 * Copyright (c) 2015 Bosch Software Innovations GmbH Germany.
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v2.0
 * and Eclipse Distribution License v1.0 which accompany this distribution.
 *
 * The Eclipse Public License is available at
 *    http://www.eclipse.org/legal/epl-v20.html
 * The Eclipse Distribution License is available at
 *    http://www.eclipse.org/org/documents/edl-v10.php.
 *
 * Contributors:
 *    Bosch Software Innovations GmbH - Please refer to git log
 *    Scott Bertin, AMETEK, Inc. - Please refer to git log
 *
 *******************************************************************************/

/*
 * This connectivity statistics object is optional and single instance only
 * 
 *  Resources:
 *
 *          Name         | ID | Oper. | Inst. | Mand.|  Type   | Range | Units | Description |
 *  SMS Tx Counter       |  0 |   R   | Single|  No  | Integer |       |       |             |
 *  SMS Rx Counter       |  1 |   R   | Single|  No  | Integer |       |       |             |
 *  Tx Data              |  2 |   R   | Single|  No  | Integer |       | kByte |             |
 *  Rx Data              |  3 |   R   | Single|  No  | Integer |       | kByte |             |
 *  Max Message Size     |  4 |   R   | Single|  No  | Integer |       | Byte  |             |
 *  Average Message Size |  5 |   R   | Single|  No  | Integer |       | Byte  |             |
 *  StartOrReset         |  6 |   E   | Single|  Yes | Integer |       |       |             |
 */

#include "liblwm2m.h"
#include "lwm2mclient.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <signal.h>

#include "zcfg_debug.h"
#include "zcfg_common.h"
#include "zcfg_rdm_oid.h"
#include "zcfg_rdm_obj.h"
#include "zcfg_fe_rdm_access.h"
#include "zcfg_fe_rdm_struct.h"
#include "zcfg_eid.h"
#include "zcfg_net.h"

// Resource Id's:
#define RES_O_SMS_TX_COUNTER            0
#define RES_O_SMS_RX_COUNTER            1
#define RES_O_TX_DATA                   2
#define RES_O_RX_DATA                   3
#define RES_O_MAX_MESSAGE_SIZE          4
#define RES_O_AVERAGE_MESSAGE_SIZE      5
#define RES_M_START_OR_RESET            6
#define RES_M_STOP						7
#define RES_O_COLLECTION_PERIOD			8

int currentTxDataByte;
int currentRxDataByte;

typedef struct
{
    int   smsTxCounter;
    int   smsRxCounter;
    int   txDataByte;             // report in kByte!
    int   rxDataByte;             // report in kByte!
    int   maxMessageSize;
    int   avrMessageSize;
    int   messageCount;           // private for incremental average calc.
    int   collectionPeriod;
    bool  collectDataStarted;
} conn_s_data_t;

bool checkConnStatValueChange(int resourceId, int resourceInsId/*multi resource using, single resource no effect*/, conn_stat_instance_t * targetP, char * zcfgParamValue)
{
    resource_ins_list_t * resourceIns;

    switch(resourceId)
    {
    case RES_O_SMS_TX_COUNTER:
        if(strcmp(targetP->sms_tx_counter, zcfgParamValue))
        {
            strncpy(targetP->sms_tx_counter, zcfgParamValue, sizeof(targetP->sms_tx_counter));
            return true;
        }
        break;  

    case RES_O_SMS_RX_COUNTER:
        if(strcmp(targetP->sms_rx_counter, zcfgParamValue))
        {
            strncpy(targetP->sms_rx_counter, zcfgParamValue, sizeof(targetP->sms_rx_counter));
            return true;
        }
        break; 

    case RES_O_TX_DATA:
        if(strcmp(targetP->tx_data, zcfgParamValue))
        {
            strncpy(targetP->tx_data, zcfgParamValue, sizeof(targetP->tx_data));
            return true;
        }
        break; 

    case RES_O_RX_DATA:
        if(strcmp(targetP->rx_data, zcfgParamValue))
        {
            strncpy(targetP->rx_data, zcfgParamValue, sizeof(targetP->rx_data));
            return true;
        }
        break; 

    case RES_O_MAX_MESSAGE_SIZE:
        if(strcmp(targetP->max_message_size, zcfgParamValue))
        {
            strncpy(targetP->max_message_size, zcfgParamValue, sizeof(targetP->max_message_size));
            return true;
        }
        break; 

    case RES_O_AVERAGE_MESSAGE_SIZE:
        if(strcmp(targetP->average_message_size, zcfgParamValue))
        {
            strncpy(targetP->average_message_size, zcfgParamValue, sizeof(targetP->average_message_size));
            return true;
        }
        break; 

    case RES_O_COLLECTION_PERIOD:
        if(strcmp(targetP->collection_period, zcfgParamValue))
        {
            strncpy(targetP->collection_period, zcfgParamValue, sizeof(targetP->collection_period));
            return true;
        }
        break; 

    default:
        break;
    }
    return false;
}

static uint8_t prv_set_tlv(lwm2m_data_t * dataP, conn_s_data_t * connStDataP)
{
    switch (dataP->id) {
    case RES_O_SMS_TX_COUNTER:
        lwm2m_data_encode_int(connStDataP->smsTxCounter, dataP);
        return COAP_205_CONTENT;
    case RES_O_SMS_RX_COUNTER:
        lwm2m_data_encode_int(connStDataP->smsRxCounter, dataP);
        return COAP_205_CONTENT;
    case RES_O_TX_DATA:
        lwm2m_data_encode_int(connStDataP->txDataByte/1024, dataP);
        return COAP_205_CONTENT;
    case RES_O_RX_DATA:
        lwm2m_data_encode_int(connStDataP->rxDataByte/1024, dataP);
        return COAP_205_CONTENT;
    case RES_O_MAX_MESSAGE_SIZE:
        lwm2m_data_encode_int(connStDataP->maxMessageSize, dataP);
        return COAP_205_CONTENT;
    case RES_O_AVERAGE_MESSAGE_SIZE:
        lwm2m_data_encode_int(connStDataP->avrMessageSize, dataP);
        return COAP_205_CONTENT;
    default:
        return COAP_404_NOT_FOUND ;
    }
}

static uint8_t prv_read(lwm2m_context_t *contextP,
                        uint16_t instanceId,
                        int * numDataP,
                        lwm2m_data_t** dataArrayP,
                        lwm2m_object_t * objectP)
{
    uint8_t result;
    int i;
	objIndex_t objIid;
	rdm_Lwm2mConnectivityStatistics_t *lwm2mConnStatObj = NULL;
	int ret = 0;
	conn_stat_instance_t * targetP;
	char desValue[257] = {0};

    /* unused parameter */
    (void)contextP;

    // this is a single instance object
    if (instanceId != 0)
    {
        return COAP_404_NOT_FOUND ;
    }

	targetP = (conn_stat_instance_t *)lwm2m_list_find(objectP->instanceList, instanceId);
    if (NULL == targetP) return COAP_404_NOT_FOUND;

    // is the server asking for the full object ?
    if (*numDataP == 0)
    {
        uint16_t resList[] = {
                RES_O_SMS_TX_COUNTER,
                RES_O_SMS_RX_COUNTER,
                RES_O_TX_DATA,
                RES_O_RX_DATA,
                RES_O_MAX_MESSAGE_SIZE,
                RES_O_AVERAGE_MESSAGE_SIZE
        };
        int nbRes = sizeof(resList) / sizeof(uint16_t);

        *dataArrayP = lwm2m_data_new(nbRes);
        if (*dataArrayP == NULL)
            return COAP_500_INTERNAL_SERVER_ERROR ;
        *numDataP = nbRes;
        for (i = 0; i < nbRes; i++)
        {
            (*dataArrayP)[i].id = resList[i];
        }
    }

    i = 0;
    do
    {
        if ((*dataArrayP)[i].type == LWM2M_TYPE_MULTIPLE_RESOURCE)
        {
            result = COAP_404_NOT_FOUND;
        }
        else
        {
			switch ((*dataArrayP)[i].id)
			{
			case RES_O_TX_DATA:
				IID_INIT(objIid);
				ret = zcfgFeObjStructGet(RDM_OID_LWM2M_CONNECTIVITY_STATISTICS, &objIid, (void **)&lwm2mConnStatObj);
				if(ret == ZCFG_SUCCESS){
					lwm2mConnStatObj->Tx_Data_2 /= 1000;   //convert to kilobyte
					// printf("%s(%d) Now read Tx Bytes %d.\n", __func__, __LINE__, lwm2mConnStatObj->Tx_Data_2);

					snprintf(desValue, 257, "%d", lwm2mConnStatObj->Tx_Data_2);
					if(checkDeviceValueChange((*dataArrayP)[i].id, -1, targetP, desValue))
					{
						setChangeValueList(objectP->objID, -1,  (*dataArrayP)[i].id,  -1);
					}
					
					lwm2m_data_encode_int(lwm2mConnStatObj->Tx_Data_2, *dataArrayP + i);
					free(lwm2mConnStatObj);
				}
				result = COAP_205_CONTENT;
				break;
			case RES_O_RX_DATA:
				IID_INIT(objIid);
				ret = zcfgFeObjStructGet(RDM_OID_LWM2M_CONNECTIVITY_STATISTICS, &objIid, (void **)&lwm2mConnStatObj);
				if(ret == ZCFG_SUCCESS){
					lwm2mConnStatObj->Rx_Data_3 /= 1000;
					// printf("%s(%d) Now read Rx Bytes %d.\n", __func__, __LINE__, lwm2mConnStatObj->Rx_Data_3);

					snprintf(desValue, 257, "%d", lwm2mConnStatObj->Rx_Data_3);
					if(checkDeviceValueChange((*dataArrayP)[i].id, -1, targetP, desValue))
					{
						setChangeValueList(objectP->objID, -1,  (*dataArrayP)[i].id,  -1);
					}

					lwm2m_data_encode_int(lwm2mConnStatObj->Rx_Data_3, *dataArrayP + i);
					free(lwm2mConnStatObj);
				}
				result = COAP_205_CONTENT;
				break;
			case RES_O_COLLECTION_PERIOD:
				IID_INIT(objIid);
				ret = zcfgFeObjStructGet(RDM_OID_LWM2M_CONNECTIVITY_STATISTICS, &objIid, (void **)&lwm2mConnStatObj);
				if(ret == ZCFG_SUCCESS){
					// printf("%s(%d) Now read Collection_Period_8 is %d.\n", __func__, __LINE__, lwm2mConnStatObj->Collection_Period_8);

					snprintf(desValue, 257, "%d", lwm2mConnStatObj->Collection_Period_8);
					if(checkDeviceValueChange((*dataArrayP)[i].id, -1, targetP, desValue))
					{
						setChangeValueList(objectP->objID, -1,  (*dataArrayP)[i].id,  -1);
					}

					lwm2m_data_encode_int(lwm2mConnStatObj->Collection_Period_8, *dataArrayP + i);
					free(lwm2mConnStatObj);
				}
				result = COAP_205_CONTENT;
				break;
			default:
				result = COAP_404_NOT_FOUND;
			}
            //result = prv_set_tlv((*dataArrayP) + i, (conn_s_data_t*) (objectP->userData));
        }
        i++;
    } while (i < *numDataP && result == COAP_205_CONTENT );

    return result;
}

static uint8_t prv_write(lwm2m_context_t *contextP,
                         uint16_t instanceId,
                         int numData,
                         lwm2m_data_t * dataArray,
                         lwm2m_object_t * objectP,
                         lwm2m_write_type_t writeType)
{
	objIndex_t objIid;
	rdm_Lwm2mConnectivityStatistics_t *lwm2mConnStatObj = NULL;
	int ret = 0, i = 0, result = 0;
	int64_t iValue;
    /* unused parameter */
    (void)contextP;

    // All write types are treated the same here
    (void)writeType;

    // this is a single instance object
    if (instanceId != 0)
    {
        return COAP_404_NOT_FOUND;
    }

	i = 0;
    do
    {
        /* No multiple instance resources */
        if (dataArray[i].type == LWM2M_TYPE_MULTIPLE_RESOURCE)
        {
            result = COAP_404_NOT_FOUND;
            continue;
        }

        switch (dataArray[i].id)
        {
			case RES_O_COLLECTION_PERIOD:
				lwm2m_data_decode_int(&dataArray[i], &iValue);
				IID_INIT(objIid);
				ret = zcfgFeObjStructGet(RDM_OID_LWM2M_CONNECTIVITY_STATISTICS, &objIid, (void **)&lwm2mConnStatObj);
				if(ret == ZCFG_SUCCESS){
					lwm2mConnStatObj->Collection_Period_8 = iValue;
					// printf("%s(%d) Now lwm2mConnStatObj->Collection_Period_8 %d.\n", __func__, __LINE__, lwm2mConnStatObj->Collection_Period_8);

					if((ret = zcfgFeObjStructSet(RDM_OID_LWM2M_CONNECTIVITY_STATISTICS, &objIid, lwm2mConnStatObj, NULL)) != ZCFG_SUCCESS){
						printf("%s(%d) Fail with zcfgFeObjStructSet(ret = %d).\n", __func__, __LINE__, ret);
					}
					free(lwm2mConnStatObj);
				}
				result = COAP_204_CHANGED;
				break;
			default:
				result = COAP_405_METHOD_NOT_ALLOWED;
        }

        i++;
    } while (i < numData && result == COAP_204_CHANGED);

    return result;
}

void collectPeriodAlarm(void){
	objIndex_t objIid;
	rdm_IpIfaceStat_t *lwm2mIpfaceStatObj = NULL;
	rdm_Lwm2mConnectivityStatistics_t *lwm2mConnStatObj = NULL;
	int ret = 0, i = 0;
	int tmp_tx = 0;
	int tmp_rx = 0;
	IID_INIT(objIid);
	//ret = zcfgFeObjStructGet(RDM_OID_IP_IFACE_STAT, &objIid, (void **)&lwm2mIpfaceStatObj);
	tmp_rx = 0;
	tmp_tx = 0;
	while(zcfgFeObjStructGetNext(RDM_OID_IP_IFACE_STAT, &objIid, (void **)&lwm2mIpfaceStatObj) == ZCFG_SUCCESS){
		if(i == 0){
			i++;
			free(lwm2mIpfaceStatObj);
			continue;
		}
		else if(i >= 5){
			break;
		}
		tmp_tx += lwm2mIpfaceStatObj->BytesSent;
		tmp_rx += lwm2mIpfaceStatObj->BytesReceived;
		printf("%s(%d) Index %d Now read the tx_bytes %d.\n", __func__, __LINE__, i, lwm2mIpfaceStatObj->BytesSent);
		printf("%s(%d) Index %d Now read the rx_bytes %d.\n", __func__, __LINE__, i, lwm2mIpfaceStatObj->BytesReceived);
		i++;
		free(lwm2mIpfaceStatObj);
	}
	
	IID_INIT(objIid);
	ret = zcfgFeObjStructGet(RDM_OID_LWM2M_CONNECTIVITY_STATISTICS, &objIid, (void **)&lwm2mConnStatObj);
	if(ret == ZCFG_SUCCESS){
		lwm2mConnStatObj->Tx_Data_2 = (tmp_tx - currentTxDataByte);
		lwm2mConnStatObj->Rx_Data_3 = (tmp_rx - currentRxDataByte);
		lwm2mConnStatObj->Collection_Period_8 = 0;
		if((ret = zcfgFeObjStructSet(RDM_OID_LWM2M_CONNECTIVITY_STATISTICS, &objIid, lwm2mConnStatObj, NULL)) != ZCFG_SUCCESS){
			printf("%s(%d) Fail with zcfgFeObjStructSet(ret = %d).\n", __func__, __LINE__, ret);
		}
	}
}
static void prv_resetCounter(lwm2m_object_t* objectP, bool start)
{
    conn_s_data_t *myData = (conn_s_data_t*) objectP->userData;
    myData->smsTxCounter        = 0;
    myData->smsRxCounter        = 0;
    myData->txDataByte          = 0;
    myData->rxDataByte          = 0;
    myData->maxMessageSize      = 0;
    myData->avrMessageSize      = 0;
    myData->messageCount        = 0;
    myData->collectDataStarted  = start;
}

static void prv_resetCounter2(void)
{
	objIndex_t objIid;
	rdm_IpIfaceStat_t *lwm2mIpfaceStatObj = NULL;
	int ret = 0;

	IID_INIT(objIid);
	ret = zcfgFeObjStructGet(RDM_OID_IP_IFACE_STAT, &objIid, (void **)&lwm2mIpfaceStatObj);
	if(ret == ZCFG_SUCCESS){
		//lwm2m_data_encode_string(lwm2mfwupdateObj->Package_URI_1, *dataArrayP + i);
		printf("%s(%d) Now read the tx_bytes %s.\n", __func__, __LINE__, lwm2mIpfaceStatObj->BytesSent);
		printf("%s(%d) Now read the rx_bytes %s.\n", __func__, __LINE__, lwm2mIpfaceStatObj->BytesReceived);
	}



}
static uint8_t prv_exec(lwm2m_context_t *contextP,
                        uint16_t instanceId,
                        uint16_t resourceId,
                        uint8_t * buffer,
                        int length,
                        lwm2m_object_t * objectP)
{
	objIndex_t objIid;
	rdm_IpIfaceStat_t *lwm2mIpfaceStatObj = NULL;
	rdm_Lwm2mConnectivityStatistics_t *lwm2mConnStatObj = NULL;
	conn_s_data_t *myData = (conn_s_data_t*) objectP->userData;
	int ret = 0, i = 0;
	int tmp_tx = 0;
	int tmp_rx = 0;

    /* unused parameter */
    (void)contextP;

    // this is a single instance object
    if (instanceId != 0)
    {
        return COAP_404_NOT_FOUND;
    }

    if (length != 0) return COAP_400_BAD_REQUEST;


    switch (resourceId)
    {
		case RES_M_START_OR_RESET:
			IID_INIT(objIid);
			//ret = zcfgFeObjStructGet(RDM_OID_IP_IFACE_STAT, &objIid, (void **)&lwm2mIpfaceStatObj);
			tmp_rx = 0;
			tmp_tx = 0;
			while(zcfgFeObjStructGetNext(RDM_OID_IP_IFACE_STAT, &objIid, (void **)&lwm2mIpfaceStatObj) == ZCFG_SUCCESS){
				if(i == 0){
					i++;
					free(lwm2mIpfaceStatObj);
					continue;
				}
				else if(i >= 5){
					break;
				}

				tmp_tx = tmp_tx + lwm2mIpfaceStatObj->BytesSent;
				tmp_rx = tmp_rx + lwm2mIpfaceStatObj->BytesReceived;
				printf("%s(%d) Index %d Now read the tx_bytes %d.\n", __func__, __LINE__, i, lwm2mIpfaceStatObj->BytesSent);
				printf("%s(%d) Index %d Now read the rx_bytes %d.\n", __func__, __LINE__, i, lwm2mIpfaceStatObj->BytesReceived);
				i++;
				free(lwm2mIpfaceStatObj);
			}
			currentTxDataByte = tmp_tx;
			currentRxDataByte = tmp_rx;
			
			myData->collectDataStarted = true;
			
			IID_INIT(objIid);
			ret = zcfgFeObjStructGet(RDM_OID_LWM2M_CONNECTIVITY_STATISTICS, &objIid, (void **)&lwm2mConnStatObj);
			if(ret == ZCFG_SUCCESS){
				lwm2mConnStatObj->Tx_Data_2 = 0;
				lwm2mConnStatObj->Rx_Data_3 = 0;
				if((ret = zcfgFeObjStructSet(RDM_OID_LWM2M_CONNECTIVITY_STATISTICS, &objIid, lwm2mConnStatObj, NULL)) != ZCFG_SUCCESS){
					printf("%s(%d) Fail with zcfgFeObjStructSet(ret = %d).\n", __func__, __LINE__, ret);
				}
				if(lwm2mConnStatObj->Collection_Period_8 != 0){
					alarm(lwm2mConnStatObj->Collection_Period_8);
				}
				free(lwm2mIpfaceStatObj);
			}
			//prv_resetCounter(objectP, true);
			return COAP_204_CHANGED;
		case RES_M_STOP:
			IID_INIT(objIid);
			//ret = zcfgFeObjStructGet(RDM_OID_IP_IFACE_STAT, &objIid, (void **)&lwm2mIpfaceStatObj);
			tmp_rx = 0;
			tmp_tx = 0;
			while(zcfgFeObjStructGetNext(RDM_OID_IP_IFACE_STAT, &objIid, (void **)&lwm2mIpfaceStatObj) == ZCFG_SUCCESS){
				if(i == 0){
					i++;
					free(lwm2mIpfaceStatObj);
					continue;
				}
				else if(i >= 5){
					break;
				}
				tmp_tx += lwm2mIpfaceStatObj->BytesSent;
				tmp_rx += lwm2mIpfaceStatObj->BytesReceived;
				printf("%s(%d) Index %d Now read the tx_bytes %d.\n", __func__, __LINE__, i, lwm2mIpfaceStatObj->BytesSent);
				printf("%s(%d) Index %d Now read the rx_bytes %d.\n", __func__, __LINE__, i, lwm2mIpfaceStatObj->BytesReceived);
				i++;
				free(lwm2mIpfaceStatObj);
			}
			myData->txDataByte = (tmp_tx - currentTxDataByte);
			myData->rxDataByte = (tmp_rx - currentRxDataByte);
			myData->collectDataStarted = false;
			
			IID_INIT(objIid);
			ret = zcfgFeObjStructGet(RDM_OID_LWM2M_CONNECTIVITY_STATISTICS, &objIid, (void **)&lwm2mConnStatObj);
			if(ret == ZCFG_SUCCESS){
				lwm2mConnStatObj->Tx_Data_2 = myData->txDataByte;
				lwm2mConnStatObj->Rx_Data_3 = myData->rxDataByte;
				lwm2mConnStatObj->Collection_Period_8 = 0;
				if((ret = zcfgFeObjStructSet(RDM_OID_LWM2M_CONNECTIVITY_STATISTICS, &objIid, lwm2mConnStatObj, NULL)) != ZCFG_SUCCESS){
					printf("%s(%d) Fail with zcfgFeObjStructSet(ret = %d).\n", __func__, __LINE__, ret);
				}
			}
			return COAP_204_CHANGED;
		default:
			return COAP_405_METHOD_NOT_ALLOWED;
    }
}

void conn_s_updateTxStatistic(lwm2m_object_t * objectP, uint16_t txDataByte, bool smsBased)
{
	return;
    conn_s_data_t* myData = (conn_s_data_t*) (objectP->userData);
    if (myData->collectDataStarted)
    {
        myData->txDataByte += txDataByte;
        myData->messageCount++;
        myData->avrMessageSize = (myData->txDataByte+myData->rxDataByte) /
                                  myData->messageCount;
        if (txDataByte > myData->maxMessageSize)
            myData->maxMessageSize = txDataByte;
        if (smsBased) myData->smsTxCounter++;
    }
}

void conn_s_updateRxStatistic(lwm2m_object_t * objectP, uint16_t rxDataByte, bool smsBased)
{
	return;
    conn_s_data_t* myData = (conn_s_data_t*) (objectP->userData);
    if (myData->collectDataStarted)
    {
        myData->rxDataByte += rxDataByte;
        myData->messageCount++;
        myData->avrMessageSize = (myData->txDataByte+myData->rxDataByte) /
                                  myData->messageCount;
        if (rxDataByte > myData->maxMessageSize)
            myData->maxMessageSize = rxDataByte;
        myData->txDataByte += rxDataByte;
        if (smsBased) myData->smsRxCounter++;
    }
}


lwm2m_object_t * get_object_conn_s(void)
{
    /*
     * The get_object_conn_s() function create the object itself and return
     * a pointer to the structure that represent it.
     */
	signal(SIGALRM, collectPeriodAlarm);
	objIndex_t objIid;
	rdm_Lwm2mConnectivityStatistics_t *lwm2mConnStatObj = NULL;
	int ret = 0;
    lwm2m_object_t * connObj;
	
	IID_INIT(objIid);
	ret = zcfgFeObjStructGet(RDM_OID_LWM2M_CONNECTIVITY_STATISTICS, &objIid, (void **)&lwm2mConnStatObj);
	if(ret == ZCFG_SUCCESS){
		lwm2mConnStatObj->Tx_Data_2 = 0;
		lwm2mConnStatObj->Rx_Data_3 = 0;
		lwm2mConnStatObj->Collection_Period_8 = 0;
		printf("%s(%d) Now lwm2mConnStatObj->Collection_Period_8 %d.\n", __func__, __LINE__, lwm2mConnStatObj->Collection_Period_8);

		if((ret = zcfgFeObjStructSet(RDM_OID_LWM2M_CONNECTIVITY_STATISTICS, &objIid, lwm2mConnStatObj, NULL)) != ZCFG_SUCCESS){
			printf("%s(%d) Fail with zcfgFeObjStructSet(ret = %d).\n", __func__, __LINE__, ret);
		}
		free(lwm2mConnStatObj);
	}

    connObj = (lwm2m_object_t *) lwm2m_malloc(sizeof(lwm2m_object_t));

    if (NULL != connObj)
    {
        memset(connObj, 0, sizeof(lwm2m_object_t));

        conn_stat_instance_t * connstatInstance;

        /*
         * It assign his unique ID
         * The 7 is the standard ID for the optional object "Connectivity Statistics".
         */
        connObj->objID = LWM2M_CONN_STATS_OBJECT_ID;

        /*
         * and its unique instance
         *
         */
        connstatInstance = (conn_stat_instance_t *)lwm2m_malloc(sizeof(conn_stat_instance_t));
        if (NULL == connstatInstance)
        {
            lwm2m_free(connObj);
            return NULL;
        }

        memset(connstatInstance, 0, sizeof(conn_stat_instance_t));
        connstatInstance->instanceId = 0;
        memset(connstatInstance->sms_tx_counter, 0, 257);
        memset(connstatInstance->sms_rx_counter, 0, 257);
        memset(connstatInstance->tx_data, 0, 257);
        memset(connstatInstance->rx_data, 0, 257);
        memset(connstatInstance->max_message_size, 0, 257);
        memset(connstatInstance->average_message_size, 0, 257);
        memset(connstatInstance->collection_period, 0, 257);
        connObj->instanceList = LWM2M_LIST_ADD(connObj->instanceList, connstatInstance);

        /*
         * And the private function that will access the object.
         * Those function will be called when a read/execute/close
         * query is made by the server or core. In fact the library don't need
         * to know the resources of the object, only the server does.
         */
        connObj->readFunc     = prv_read;
		connObj->writeFunc    = prv_write;
        connObj->executeFunc  = prv_exec;
        connObj->userData     = lwm2m_malloc(sizeof(conn_s_data_t));

        /*
         * Also some user data can be stored in the object with a private
         * structure containing the needed variables.
         */
        if (NULL != connObj->userData)
        {
            prv_resetCounter(connObj, false);
        }
        else
        {
            lwm2m_free(connObj);
            connObj = NULL;
        }
    }
    return connObj;
}

void free_object_conn_s(lwm2m_object_t * objectP)
{
    lwm2m_free(objectP->userData);
    while (objectP->instanceList != NULL)
    {
        conn_stat_instance_t * connStatInstance = (conn_stat_instance_t *)objectP->instanceList;
        objectP->instanceList = objectP->instanceList->next;
        lwm2m_free(connStatInstance);
    }
    lwm2m_free(objectP);
}
