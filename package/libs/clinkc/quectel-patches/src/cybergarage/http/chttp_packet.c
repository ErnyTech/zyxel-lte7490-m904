/******************************************************************
*
*	CyberNet for C
*
*	Copyright (C) Satoshi Konno 2005
*
*       Copyright (C) 2006-2007 Nokia Corporation. All rights reserved.
*
*       This is licensed under BSD-style license,
*       see file COPYING.
*
*	File: chttp_packet.cpp
*
*	Revision:
*
*	01/25/05
*		- first revision
*	08/21/05
*		- Thanks for Theo Beisch <theo.beisch@gmx.de>
*		- Changed cg_http_packet_read() to store length in HTTPPacket.
*	10/25/05
*		- The following functions move to define macros.
*		  cg_http_packet_setcontent()
*		  cg_http_packet_setcontentpointer()
*		  cg_http_packet_getcontent()
*		- cg_http_packet_setncontent() is added.
*	10/31/05
*		- cg_http_packet_sethost:
*		  port was an excess parameter for s(n)printf when port <= 0
*	11/11/05
*		- Added cg_http_packet_setheaderlonglong() and cg_http_packet_getheaderlonglong().
*		- Extended cg_http_packet_setcontentlength() and cg_http_packet_getcontentlength() to 64bit
*		  when the compiler is supported C99 or the platform is WIN32.
*	02/01/07
*		- Fixed cg_http_request_post() not to hung up when the request method is HEAD.
*		- Added a onlyHeader parameter to cg_http_response_read() and cg_http_response_packet().
*	03/18/07
*		- Changed the following functions to use CgInt64.
*		  cg_http_packet_setheaderlonglong()
*		  cg_http_packet_getheaderlonglong()
*	11/16\07  Satoshi Konno <skonno@cybergarage.org>
*		- Fixed cg_http_packet_read_body()not to lost data when the response packet is huge.
*	12/13/07  Aapo makela <aapo.makela@nokia.com>
*		- Fix to cg_http_packet_sethost() not to crash in out-of-memory situation
*
******************************************************************/

#include <cybergarage/http/chttp.h>
#include <cybergarage/util/clog.h>

#ifdef HAVE_CONFIG_H
#  include "config.h"
#endif

#include <cybergarage/net/cinterface.h>

#include <limits.h>
#include <unistd.h>
#include "zyutil.h"
#include "zcfg_system.h"

#define READBUF_LENGTH 4096
#define MAX_PATITION_SIZE 150*1024*1024

BOOL bRemoveDaemonsWirelessModule = FALSE;


static int memory_check(int buflen, char *remoteIP);
/****************************************
* cg_http_packet_new
****************************************/

CgHttpPacket *cg_http_packet_new()
{
	CgHttpPacket *httpPkt;

	cg_log_debug_l4("Entering...\n");

	httpPkt= (CgHttpPacket *)malloc(sizeof(CgHttpPacket));

	if ( NULL != httpPkt )
	{
		cg_http_packet_init(httpPkt);
	}

	cg_log_debug_l4("Leaving...\n");

	return httpPkt;
}

/****************************************
* cg_http_packet_delete
****************************************/

void cg_http_packet_delete(CgHttpPacket *httpPkt)
{
	cg_log_debug_l4("Entering...\n");

	cg_http_packet_clean(httpPkt);
	free(httpPkt);

	cg_log_debug_l4("Leaving...\n");
}

/****************************************
* cg_http_packet_init
****************************************/

void cg_http_packet_init(CgHttpPacket *httpPkt)
{
	cg_log_debug_l4("Entering...\n");

	httpPkt->headerList = cg_http_headerlist_new();
	httpPkt->content = cg_string_new();

	cg_log_debug_l4("Leaving...\n");
}

/****************************************
* cg_http_packet_clean
****************************************/

void cg_http_packet_clean(CgHttpPacket *httpPkt)
{
	cg_log_debug_l4("Entering...\n");

	cg_http_headerlist_delete(httpPkt->headerList);
	cg_string_delete(httpPkt->content);

	cg_log_debug_l4("Leaving...\n");
}

/****************************************
* cg_http_packet_clear
****************************************/

void cg_http_packet_clear(CgHttpPacket *httpPkt)
{
	cg_log_debug_l4("Entering...\n");

	cg_http_headerlist_clear(httpPkt->headerList);
	cg_string_setvalue(httpPkt->content, NULL);

	cg_log_debug_l4("Leaving...\n");
}

/****************************************
* cg_http_packet_setheader
****************************************/

void cg_http_packet_setheadervalue(CgHttpPacket *httpPkt, char* name, char *value)
{
	cg_log_debug_l4("Entering...\n");

	cg_http_headerlist_set(httpPkt->headerList, name, value);

	cg_log_debug_l4("Leaving...\n");
}


/****************************************
* cg_http_packet_setheader
****************************************/
#ifdef ZYXEL_PATCH /*support multiple header value, ZyXEL 2013*/

void cg_http_packet_setheadervalue_multi(CgHttpPacket *httpPkt, char* name, char *value)
{
	cg_log_debug_l4("Entering...\n");

	cg_http_headerlist_set_multi(httpPkt->headerList, name, value);

	cg_log_debug_l4("Leaving...\n");
}
#endif

/****************************************
* cg_http_packet_setheaderinteger
****************************************/

void cg_http_packet_setheaderinteger(CgHttpPacket *httpPkt, char* name, int value)
{
	char svalue[CG_STRING_INTEGER_BUFLEN];

	cg_log_debug_l4("Entering...\n");

	cg_http_packet_setheadervalue(httpPkt, name, cg_int2str(value, svalue, sizeof(svalue)));

	cg_log_debug_l4("Leaving...\n");
}

/****************************************
* cg_http_packet_setheaderlong
****************************************/

void cg_http_packet_setheaderlong(CgHttpPacket *httpPkt, char* name, long value)
{
	char svalue[CG_STRING_LONG_BUFLEN];

	cg_log_debug_l4("Entering...\n");

	cg_http_packet_setheadervalue(httpPkt, name, cg_long2str(value, svalue, sizeof(svalue)));

	cg_log_debug_l4("Leaving...\n");
}

/****************************************
* cg_http_packet_setheaderlonglong
****************************************/

#if defined(CG_USE_INT64)
void cg_http_packet_setheaderlonglong(CgHttpPacket *httpPkt, char* name, CgInt64 value)
{
	char svalue[CG_STRING_LONGLONG_BUFLEN];
	cg_http_packet_setheadervalue(httpPkt, name, cg_longlong2str(value, svalue, sizeof(svalue)));
}
#endif

/****************************************
* cg_http_packet_getheadervalue
****************************************/

char *cg_http_packet_getheadervalue(CgHttpPacket *httpPkt, char* name)
{
	return cg_http_headerlist_getvalue(httpPkt->headerList, name);
}

/****************************************
* cg_http_packet_getheadervalue
****************************************/

int cg_http_packet_getheaderinteger(CgHttpPacket *httpPkt, char* name)
{
	char *value;

	cg_log_debug_l4("Entering...\n");

	value = cg_http_packet_getheadervalue(httpPkt, name); 

	cg_log_debug_l4("Leaving...\n");

	return (value != NULL) ? atoi(value) : 0;
}

/****************************************
* cg_http_packet_getheadervalue
****************************************/

long cg_http_packet_getheaderlong(CgHttpPacket *httpPkt, char* name)
{
	char *value;

	cg_log_debug_l4("Entering...\n");

	value = cg_http_packet_getheadervalue(httpPkt, name); 

	cg_log_debug_l4("Leaving...\n");

	return (value != NULL) ? atol(value) : 0;
}

/****************************************
* cg_http_packet_getheaderlonglong
****************************************/

#if defined(CG_USE_INT64)
CgInt64 cg_http_packet_getheaderlonglong(CgHttpPacket *httpPkt, char* name)
{
	char *value;

	cg_log_debug_l4("Entering...\n");

	value = cg_http_packet_getheadervalue(httpPkt, name); 

	cg_log_debug_l4("Leaving...\n");

	return (value != NULL) ? cg_str2longlong(value) : 0;
}
#endif

/****************************************
* cg_http_packet_getheadervalue
****************************************/

void cg_http_packet_sethost(CgHttpPacket *httpPkt, char *addr, int port)
{
	char *host;
	int hostMaxLen;

	cg_log_debug_l4("Entering...\n");
	
	if (addr == NULL)
		return;

	hostMaxLen = cg_strlen(addr) + CG_NET_IPV6_ADDRSTRING_MAXSIZE + CG_STRING_INTEGER_BUFLEN;
	host = malloc(sizeof(char) * hostMaxLen);

	if (host == NULL)
		/* Memory allocation failure */
		return;

#if defined(HAVE_SNPRINTF)
	if (0 < port && port != CG_HTTP_DEFAULT_PORT) {
		if (cg_net_isipv6address(addr) == TRUE)
			snprintf(host, hostMaxLen, "[%s]:%d", addr, port);
		else
			snprintf(host, hostMaxLen, "%s:%d", addr, port);
	}
	else {
		if (cg_net_isipv6address(addr) == TRUE)
			snprintf(host, hostMaxLen, "[%s]", addr);
		else
			snprintf(host, hostMaxLen, "%s", addr);
	}
#else	
	if (0 < port && port != CG_HTTP_DEFAULT_PORT) {
		if (cg_net_isipv6address(addr) == TRUE)
			sprintf(host, "[%s]:%d", addr, port);
		else
			sprintf(host, "%s:%d", addr, port);
	}
	else {
		if (cg_net_isipv6address(addr) == TRUE)
			sprintf(host, "[%s]", addr);
		else
			sprintf(host, "%s", addr);
	}
#endif

	cg_http_packet_setheadervalue(httpPkt, CG_HTTP_HOST, host);

	free(host);

	cg_log_debug_l4("Leaving...\n");
}

/****************************************
* cg_http_packet_post
****************************************/

void cg_http_packet_post(CgHttpPacket *httpPkt, CgSocket *sock)
{
	CgHttpHeader *header;
	char *name, *value;
	char *content;
	CgInt64 contentLen;
	
	cg_log_debug_l4("Entering...\n");

#ifdef ZYXEL_CROSS_SITE_DEBUG
	cg_http_headerlist_set(httpPkt->headerList, "Access-Control-Allow-Origin", "*");
	cg_http_headerlist_set(httpPkt->headerList, "Access-Control-Allow-Methods", "*");
	//cg_http_headerlist_set(httpPkt->headerList, "Access-Control-Allow-Headers", "*");
	//cg_http_headerlist_set(httpPkt->headerList, "Access-Control-Allow-Credentials", "true");
#endif

	/**** send headers ****/
	for (header = cg_http_packet_getheaders(httpPkt); header != NULL; header = cg_http_header_next(header)) {
		name = cg_http_header_getname(header);
		if (name == NULL)
			continue;
		cg_socket_write(sock, name, cg_strlen(name));
		cg_socket_write(sock, CG_HTTP_COLON, sizeof(CG_HTTP_COLON)-1);
		cg_socket_write(sock, CG_HTTP_SP, sizeof(CG_HTTP_SP)-1);
		value = cg_http_header_getvalue(header);
		if (value != NULL)
			cg_socket_write(sock, value, cg_strlen(value));
		cg_socket_write(sock, CG_HTTP_CRLF, sizeof(CG_HTTP_CRLF)-1);
	}
	cg_socket_write(sock, CG_HTTP_CRLF, sizeof(CG_HTTP_CRLF)-1);
	
	/**** send content ****/
	content = cg_http_packet_getcontent(httpPkt);
	contentLen = cg_http_packet_getcontentlength(httpPkt);
	if (content != NULL && 0 < contentLen)
		cg_socket_write(sock, content, contentLen);

	cg_log_debug_l4("Leaving...\n");
}

// 20160519 Max add
void tr069_cg_http_packet_post(CgHttpPacket *httpPkt, CgSocket *sock)
{
	CgHttpHeader *header;
	char *name, *value;
	char *content;
	CgInt64 contentLen;

	cg_log_debug_l4("Entering...\n");

	/**** send headers ****/
	for (header = cg_http_packet_getheaders(httpPkt); header != NULL; header = cg_http_header_next(header))
	{
		name = cg_http_header_getname(header);
		if (name == NULL)
			continue;

		tr069_cg_socket_write(sock, name, cg_strlen(name));
		tr069_cg_socket_write(sock, CG_HTTP_COLON, sizeof(CG_HTTP_COLON)-1);
		tr069_cg_socket_write(sock, CG_HTTP_SP, sizeof(CG_HTTP_SP)-1);
		value = cg_http_header_getvalue(header);
		if (value != NULL)
		{
			tr069_cg_socket_write(sock, value, cg_strlen(value));
		}
		tr069_cg_socket_write(sock, CG_HTTP_CRLF, sizeof(CG_HTTP_CRLF)-1);
	}
	tr069_cg_socket_write(sock, CG_HTTP_CRLF, sizeof(CG_HTTP_CRLF)-1);
	proto_flush(sock);

	/**** send content ****/
	content = cg_http_packet_getcontent(httpPkt);
	contentLen = cg_http_packet_getcontentlength(httpPkt);
	if (content != NULL && 0 < contentLen)
	{
		tr069_cg_socket_write(sock, content, contentLen);
		proto_flush(sock);
	}

	cg_log_debug_l4("Leaving...\n");
}

/****************************************
* cg_http_packet_read_headers
****************************************/

void cg_http_packet_read_headers(CgHttpPacket *httpPkt, CgSocket *sock, char *lineBuf, int lineBufSize)
{
	CgStringTokenizer *strTok;
	CgHttpHeader *header;
	int readLen;
	char *name, *value;
	
	cg_log_debug_l4("Entering...\n");

	while (1) {
		readLen = cg_socket_readline(sock, lineBuf, lineBufSize);
		if (readLen <= 2)
			break;
		name = NULL;
		value = NULL;
		strTok = cg_string_tokenizer_new(lineBuf, CG_HTTP_HEADERLINE_DELIM);
		if (cg_string_tokenizer_hasmoretoken(strTok) == TRUE)
			name = cg_string_tokenizer_nexttoken(strTok);
		if (cg_string_tokenizer_hasmoretoken(strTok) == TRUE) {
			value = cg_string_tokenizer_nextalltoken(strTok);
			cg_strrtrim(value, CG_HTTP_HEADERLINE_DELIM, cg_strlen(CG_HTTP_HEADERLINE_DELIM));
		}
		if (name != NULL) {
			if (value == NULL)
				value = "";
			header = cg_http_header_new();
			cg_http_header_setname(header, name);
			cg_http_header_setvalue(header, value);
			cg_http_packet_addheader(httpPkt, header);
		}
		cg_string_tokenizer_delete(strTok);
	}

	cg_log_debug_l4("Leaving...\n");
}


#define MAX_CHUNK_SIZE 30000

/****************************************
* cg_http_packet_read_chunk
****************************************/
#ifdef ZYXEL_PATCH /*support save content to file, by ZyXEL charisse*/
long cg_http_packet_read_chunk(CgHttpPacket *httpPkt, CgSocket *sock, char *lineBuf, int lineBufSize, FILE *fp)
#else
long cg_http_packet_read_chunk(CgHttpPacket *httpPkt, CgSocket *sock, char *lineBuf, int lineBufSize)
#endif
{
	long readLen = 0;
	long conLen = 0;
#ifdef ZYXEL_PATCH /*fix the bug of process lock when connection lost, by ZyXEL charisse*/
	int tmpLen;
#endif
	int tries = 0;
	char *content = NULL;
	BOOL complete = FALSE;
	
	cg_log_debug_l4("Entering...\n");

	memset(lineBuf, 0, lineBufSize);
	/* Read chunk header */
	readLen = cg_socket_readline(sock, lineBuf, lineBufSize);

	//printf("!!!!!! dbg: read a line before parse: %d/%d <%s>\n", readLen, lineBufSize, lineBuf);

	//char size_line[16] = {0};
	char *ptr = NULL;

	if(readLen == -1 || readLen == 0)
		return -1;


	if(lineBuf[0] == 13 && lineBuf[0] == 0x0a && readLen == 2)
		return 0;

	conLen = cg_strhex2long(lineBuf);
	if(lineBuf[0] == '0' && conLen == 0)
	{
		//End chunk
		printf("chunk 0\n");
		readLen = 0;
		conLen = 0;
		complete = TRUE;
	}
	else if(conLen > 0 && conLen < MAX_CHUNK_SIZE)
	{
		//this line maybe a chunk header or need to append to the trail of last chunk reading
		//*ptr = 0;

		printf("check chunk size line %l\n", conLen);
		readLen -= 2;
	}
	else{
		conLen = 0;
		if(conLen >= MAX_CHUNK_SIZE)
			return -1;
		if(!httpPkt->content){
			return 0;
		}

		printf("receive chunk data fail\n");

		return -1;
	}

	//printf("@@@@@@ dbg: chunk get line buf:%d-%d <%s>\n", conLen, readLen, lineBuf);

	if (conLen < 1)
	{
		//the following strings are chunk content, not size header
		if(readLen)
			cg_http_packet_appendncontent(httpPkt, lineBuf, readLen);

		goto exit;
	}
	
	content = (char *)malloc(conLen+1);

	if (content == NULL)
	{
		cg_log_debug_s("Memory allocation problem!\n");
		return 0;
	}
		
	memset(content, 0, conLen+1);
	
	readLen = 0;
	/* Read content until conLen is reached, or tired of trying */
#ifdef ZYXEL_PATCH /*fix the bug of process lock when connection lost, by ZyXEL charisse*/
	while (readLen < conLen && tries < CG_HTTP_CWMP_SEND_RECV_TRY_COUNT)
	{
		tmpLen = cg_socket_read(sock, (content+readLen), (conLen-readLen));
		if( tmpLen<=0 )
			tries++;
		else{
			readLen += tmpLen;
		}
	}
#else
	while (readLen < conLen && tries < 20)
	{
		readLen += cg_socket_read(sock, (content+readLen), (conLen-readLen));
		tries++;
	}
#endif
	/* Append content to packet */
#ifdef ZYXEL_PATCH /*support save content to file, by ZyXEL charisse*/
	if(fp)
		fwrite(content, readLen, 1, fp);
	else if(readLen)
		cg_http_packet_appendncontent(httpPkt, content, readLen);
#else
	cg_http_packet_appendncontent(httpPkt, content, readLen);
#endif
	free(content); content = NULL;
	
	if (readLen == conLen)
	{
		/* Read CRLF bytes */
		cg_socket_readline(sock, lineBuf, lineBufSize);

		//Jack: ignore it!! it will make another chunk header disappear!!
#if 0 //def ZYXEL_PATCH /*fix the bug when chunk end line is double CRLF, the package parse will error, ZyXEL 2013*/
		cg_socket_readline(sock, lineBuf, lineBufSize);
#endif
		//printf("Ending line check %d <%s>\n", lineBufSize, lineBuf);
	}

exit:
	//printf("###### dbg: done chunk read %d %d %d <%s>\n", readLen, conLen, httpPkt->content->valueSize, httpPkt->content->value);
	cg_log_debug_l4("Leaving...\n");


	return (complete == TRUE) ? -1 : readLen;
}

/****************************************
* cg_http_packet_read_body
****************************************/

BOOL cg_http_packet_read_body(CgHttpPacket *httpPkt, CgSocket *sock, char *lineBuf, int lineBufSize)
{
	long readLen;
	int conLen;
#ifdef ZYXEL_PATCH /*fix the bug of process lock when connection lost, by ZyXEL charisse*/
	int tmpLen;
#endif
	char *content;
	char readBuf[READBUF_LENGTH + 1];
	int tries = 0;

	cg_log_debug_l4("Entering...\n");
	memset(readBuf, 0, sizeof(readBuf));
	conLen = cg_http_packet_getcontentlength(httpPkt);
	if(memory_check(conLen, cg_socket_getremoteaddress(sock)) == FALSE){
		printf("%s: memory fail, content len %d\n", __FUNCTION__, conLen);
		return FALSE;
	}	
	content = NULL;
	if (0 < conLen) {
		//printf("%s: content len %d\n", __FUNCTION__, conLen);
		content = (char *)malloc(conLen+1);
		if (content == NULL)
		{
			printf("%s: memory allocat fail, content len %d\n", __FUNCTION__, conLen);
			cg_log_debug_s("Memory allocation problem!\n");
			return FALSE;
		}
		
		memset(content, 0, conLen+1);
		readLen = 0;
		
		/* Read content until conLen is reached, or tired of trying */
#ifdef ZYXEL_PATCH /*fix the bug of process lock when connection lost, by ZyXEL charisse*/
		while (readLen < conLen && tries < CG_HTTP_CWMP_SEND_RECV_TRY_COUNT)
		{
			tmpLen = cg_socket_read(sock, (content+readLen), (conLen-readLen));
			/* Fixed to increment the counter only when cg_socket_read() doesn't read data */
			if (tmpLen <= 0){
				tries++;
			}
			else{
				readLen += tmpLen;
			}
		}
		if((tries >= CG_HTTP_CWMP_SEND_RECV_TRY_COUNT) || (readLen < conLen)){
			printf("%s: recv fail, content len %d\n", __FUNCTION__, conLen);
			free(content);
			return FALSE;
		}
#else
		while (readLen < conLen && tries < 20)
		{
			readLen += cg_socket_read(sock, (content+readLen), (conLen-readLen));
			/* Fixed to increment the counter only when cg_socket_read() doesn't read data */
			if (readLen <= 0)
				tries++;
		}
#endif
		
		if (readLen <= 0){
			printf("%s: recv nothing, content len %d\n", __FUNCTION__, conLen);
			free(content);
			return TRUE;
		}
		content[readLen] = '\0';
		cg_http_packet_setcontentpointer(httpPkt, content, readLen);
	}
	else if (cg_http_packet_getheadervalue(httpPkt, 
					CG_HTTP_CONTENT_LENGTH) == NULL)
	{
		/* header existance must be checked! otherwise packets which
		   rightly report 0 as content length, will jam the http */
		
		/* Check if we read chunked encoding */
		if (cg_http_packet_ischunked(httpPkt) == TRUE)
		{
			conLen = 0;
			do {
#ifdef ZYXEL_PATCH /*support save content to file, by ZyXEL charisse*/
				readLen = cg_http_packet_read_chunk(httpPkt, sock, lineBuf, lineBufSize, NULL);
#else
				readLen = cg_http_packet_read_chunk(httpPkt, sock, lineBuf, lineBufSize);
#endif
				if(readLen > 0) conLen += readLen;
			} while (readLen >= 0);
			
			cg_http_packet_setcontentlength(httpPkt,conLen);
		} else {
			readLen = 0;
			conLen = 0;
			while ((readLen = cg_socket_read(sock, readBuf, READBUF_LENGTH)) > 0)
			{
				cg_http_packet_appendncontent(httpPkt, readBuf, readLen);
				conLen += readLen;
			}

			cg_http_packet_setcontentlength(httpPkt, conLen);
		}
	}

	cg_log_debug_l4("Leaving...\n");

	return TRUE;
}

#ifdef ZYXEL_PATCH /*support save content to file, by ZyXEL charisse*/
/****************************************
* cg_http_packet_read_body_to_file
****************************************/
BOOL cg_http_packet_read_body_to_file(CgHttpPacket *httpPkt, CgSocket *sock, char *lineBuf, int lineBufSize, char *savedFile)
{
	long readLen;
	int conLen;
	char readBuf[READBUF_LENGTH + 1]={0};
	int tries = 0;
	FILE *fp = NULL;
	int length;
	char *contentType = NULL;
	char *boundaryPtr = NULL;
	char *fileStart = NULL;
	char boundary[BOUNDARY_SIZE] = {0};
	BOOL hasboundary = FALSE;
	int parseFlag = 0;

	cg_log_debug_l4("Entering...\n");
	
	conLen = cg_http_packet_getcontentlength(httpPkt);
	if(memory_check(conLen, cg_socket_getremoteaddress(sock)) == FALSE){
		return FALSE;
	}
	
	if (0 < conLen) {
		fp = fopen( savedFile, "wb" );
		if (!fp) {
			cg_log_debug_l4( "fopen %s fail", savedFile );
			return FALSE;
		}
		
		contentType = cg_http_packet_getheadervalue(httpPkt, CG_HTTP_CONTENT_TYPE);
		if(contentType && (boundaryPtr = strstr(contentType, "boundary=")) != NULL){
			boundaryPtr += strlen("boundary=");
			snprintf(boundary, BOUNDARY_SIZE, "--%s", boundaryPtr);
			hasboundary = TRUE;
		}

		readLen = 0;

		/* Read content until conLen is reached, or tired of trying */
		while (readLen < conLen && tries < CG_HTTP_CWMP_SEND_RECV_TRY_COUNT)
		{
			memset(readBuf, '\0', sizeof(readBuf));
			length = cg_socket_read(sock, readBuf, READBUF_LENGTH);
			/*for http server to parse upload multipart/form-data packet boundary and get the binary file start pointer*/
			if(hasboundary && (strstr(readBuf, boundary) != NULL) && (strstr(readBuf, "Content-Disposition") != NULL)){
				/*the first boundary carry from browser end with '\r\n\r\n' */
				fileStart = strstr(readBuf, "\r\n\r\n") + 4;
				parseFlag = 1;
			}
			else
				parseFlag = 0;
				
			if (length <= 0){
				tries++;
			}
			else{
				readLen += length;
				if (parseFlag)
					fwrite(fileStart, sizeof(char), length-(fileStart-readBuf), fp);
				else
					fwrite(readBuf, length, 1, fp);
			}

		}
		if((tries >= CG_HTTP_CWMP_SEND_RECV_TRY_COUNT) || (readLen < conLen)){
			fclose(fp);
			unlink(savedFile);
			return FALSE;
		}

		if (readLen <= 0){
			fclose(fp);
			unlink(savedFile);
			return FALSE;
		}
		cg_http_packet_setcontentpointer(httpPkt, NULL, readLen);
		fclose(fp);
	}
	else if (cg_http_packet_getheadervalue(httpPkt,
					CG_HTTP_CONTENT_LENGTH) == NULL)
	{
		/* header existance must be checked! otherwise packets which
		   rightly report 0 as content length, will jam the http */
		fp = fopen( savedFile, "wb" );
		if (!fp) {
			cg_log_debug_l4( "fopen %s fail", savedFile );
			return FALSE;
		}

		/* Check if we read chunked encoding */
		if (cg_http_packet_ischunked(httpPkt) == TRUE)
		{
			conLen = 0;
			do {
				readLen = cg_http_packet_read_chunk(httpPkt, sock, lineBuf, lineBufSize, fp);
				conLen += readLen;
				/*TODO... not finished*/
				fwrite(lineBuf, readLen, 1, fp);
			} while (readLen > 0);

			cg_http_packet_setcontentlength(httpPkt,conLen);
		} else {
			readLen = 0;
			conLen = 0;
			while ((readLen = cg_socket_read(sock, readBuf, READBUF_LENGTH)) > 0)
			{
				conLen += readLen;
				fwrite(readBuf, readLen, 1, fp);
			}

			cg_http_packet_setcontentlength(httpPkt, conLen);
		}
		fclose(fp);
	}

	cg_log_debug_l4("Leaving...\n");
	return TRUE;
}

int get_free_memory(){
	int size;
	FILE *fp = NULL;
	char *lst = calloc(128, sizeof(char));

	fp = fopen("/proc/meminfo", "r");
	if(fp != NULL){
		while((fgets(lst, 128, fp)) != NULL) {
			if(strstr(lst, "MemFree:") != NULL){
				sscanf(lst, "MemFree: %d", &size);
				break;
			}
		}

		fclose(fp);
	}
	free(lst);
	
	return (size*1024);
}

#define ISFROMWIFI 1
#define ISFROMLAN 2
#define ISFROMWAN 0

void removeDaemonsWLModule(int clientSrc)
{
	int i;
	char cmd[64];

	printf("killall processes to release memory\n");
	bRemoveDaemonsWirelessModule = TRUE;
	system("killall -q proftpd swmdk smbd zupnp snmpd ramonitor zhotplugd wwanbackupd vcautohunt syslog-ng zybtnchk dropbear ztzu zebra bcmmserver acsd wps_monitor zstun ntpclient ntfs-3g mcpd telnetd pure-ftpd tty_log_echo lld2d eapd nas crond");

#ifdef ZYIMS_SUPPORT
	/*Stop Voice Service. 2017.03.20 Steve*/
	printf("Stop voice service.\n");
	system("zyims.sh stop");

#endif //#ifdef ZYIMS_SUPPORT

	system("echo 768 > /proc/sys/vm/min_free_kbytes");
	system("echo 1 > /proc/sys/vm/drop_caches");
	if(clientSrc == ISFROMWIFI){
		system("/tmp/interface_down.sh lanwan");
		sleep(3);
	}
	else if(clientSrc == ISFROMLAN) {		
		system("/tmp/interface_down.sh wan");
		sleep(3);
#if defined (ZYPRJ_VMG1312) || defined (UPGRADE_FW_RM_WIFI_MODULE) 
		sprintf(cmd, "/tmp/shutDownWifi.sh\n");
		system(cmd);
#endif
	}
	else{//From WAN
#ifndef UPGRADE_FW_RM_WIFI_MODULE
		system("/tmp/interface_down.sh lan");
#endif
#if defined (ZYPRJ_VMG1312) || defined (UPGRADE_FW_RM_WIFI_MODULE)
		sprintf(cmd, "/tmp/shutDownWifi.sh\n");
		system(cmd);
#endif
	}
}

static int ipComeFromWiFi(char *remoteIP)
{
	char cmd[256] = {0};
	FILE *fp = NULL;
	char *lst = calloc(16, sizeof(char));
	int ret;

	sprintf(cmd, "/tmp/searchIPFromWiFi.sh  %s\n", remoteIP);
	system(cmd);

	fp = fopen("/tmp/result", "r");
	if(fp != NULL){
		while((fgets(lst, 16, fp)) != NULL) {
			sscanf(lst, "%d", &ret);
		}

		fclose(fp);
	}
	free(lst);

	return ret;
}

BOOL isSizeExceedPatition(int bufLen){
	int32_t compareSize = MAX_PATITION_SIZE; //the maximum partition size is 56M
	char mtdDev[16];
#ifdef ECONET_PLATFORM
	if(ZCFG_SUCCESS == zyUtilMtdDevGetByName(TCLINUX_PARTITION_NAME, mtdDev))
#else
	if(ZCFG_SUCCESS == zyUtilMtdDevGetByName(IMAGE_PARTITION_NAME, mtdDev))
#endif
	{
		if((compareSize = zyUtilMtdDevGetSize(mtdDev)) != ZCFG_INTERNAL_ERROR){
			if( compareSize < bufLen ){
				return TRUE;
			}else{
				return FALSE;
			}
		}else{
			compareSize = MAX_PATITION_SIZE;
			if( compareSize < bufLen ){
				return TRUE;
			}else{
				return FALSE;
			}
		}
	}else
    {
        compareSize = MAX_PATITION_SIZE;
    	if( compareSize < bufLen ){
    		return TRUE;
    	}else{
            return FALSE;
        }
    }
    return FALSE;
}

static int memory_check(int buflen, char *remoteIP){
	int checkMemCnt=0;
	if(isSizeExceedPatition(buflen)){
		printf("\nexceed patition size\n");
		return FALSE;
	}
check_again:
	if(buflen >= get_free_memory()){
        system("echo 1 > /proc/sys/vm/drop_caches");
        /* require more to hit the threshold of process killer easier */
        if((buflen+(2*1024*1024)) >= (get_free_memory())) {
            system("/usr/sbin/logrotate /etc/logrotate.d/sys_logrotate.conf");
            system("rm /var/log/*.1");
            system("rm /var/log/zcfg.log");
            removeDaemonsWLModule(ipComeFromWiFi(remoteIP));
            if((buflen+(2*1024*1024)) >= (get_free_memory()))
                goto check_again1;
        }
	}

	return TRUE;

check_again1:
	if (checkMemCnt <= 3) {
		checkMemCnt++;
		goto check_again;
	}
	// Daemons/wireless module have been removed, CPE must reboot if upgrade firmware fail.
	printf("upgrade firmware fail: out of memory!!!\n");
	system("reboot");
	return FALSE;
}

static char *find_boundary(char *ptr, int buflen, char *boundary, int len){
	int i, b;
	if(!ptr || !boundary || len<=0  || buflen<=0 || buflen < len) return NULL;
	
	for(i=0,b=0; i<buflen; i++){
		if(ptr[i] != boundary[b++]){
			b = 0;
		}
		
		if(b>=len){
			return ptr+i-len+1;
		}
	}

	return NULL;
}

BOOL cg_http_packet_parse_multipart(CgHttpPacket *httpPkt, CgSocket *sock, const char *boundary, CgHttpForm *form){
	long readLen, wirteFile = 0;
	int conLen;
	char readBuf[READBUF_LENGTH + 1]={0};
	int tries = 0;
	CgFile *fp = NULL;
	CgHttpVal *val = NULL;
	int length, l, w_len, i, maxReadLen, boundaryLen;
	char *contentType = NULL, *contentDispos;
	char *boundaryPtr = NULL, *boundaryCtPtr = NULL;
	char *ptr = NULL, *tmp = NULL, *tmp2 = NULL;
	char *readBufPtr = NULL;
	char *filepath;
	int tmplength = 256;
	char filename[256] = {0};
	char valname[256] = {0};
	int parseFlag = 0;
	BOOL done = FALSE;
	BOOL parseHeader =FALSE;

	cg_log_debug_l4("Entering...\n");
	
	if(!httpPkt || !sock || !httpPkt || !form) return FALSE;
	
	if(!form->_FILE) form->_FILE = cg_filelist_new();
	if(!form->_POST) form->_POST = cg_http_vallist_new();

	boundaryLen = strlen(boundary);
	conLen = cg_http_packet_getcontentlength(httpPkt);
	if(memory_check(conLen, cg_socket_getremoteaddress(sock)) == FALSE){
		return FALSE;
	}
	if (0 < conLen /* && conLen <= HTTPD_MAX_UPLOAD_SIZE */ ) {
		readLen = 0;
		
		readBufPtr = readBuf;
		maxReadLen = READBUF_LENGTH;
		parseHeader = TRUE;
		/* Read content until conLen is reached, or tired of trying */
		while (readLen < conLen && /* readLen <= HTTPD_MAX_UPLOAD_SIZE && */ tries < CG_HTTP_CWMP_SEND_RECV_TRY_COUNT && !done)
		{
			//printf("### maxReadLen = %d\n", maxReadLen);
			length = cg_socket_read(sock, readBufPtr, maxReadLen);
			//printf("length = %d\n", length);
			if (length > 0){
				readLen += length;
				*(readBufPtr+length) = 0;
				length = (readBufPtr+length) - readBuf;
				ptr = readBuf;
				do{
					if(/*parseHeader && */(boundaryPtr = find_boundary(ptr, ((readBuf+length) - ptr), boundary, boundaryLen))){
						printf("== boundry start ==\n");
						if(boundaryPtr > ptr){
							w_len = boundaryPtr - ptr -2; //igonre \r\n before boundry
							if(w_len){
								if(fp) //store other data
									cg_file_write(fp, (CgByte *)ptr, sizeof(char)*w_len);  
								else if(val)
									cg_http_val_appendvalue(val, ptr, w_len);
							
								ptr += w_len;
								wirteFile+=w_len;
							}
						}
						// if has file pointer
						if(fp){
							printf("file size = %d\n",wirteFile);
							cg_file_close(fp);
							cg_filelist_add(form->_FILE, fp);
							fp = NULL;
						}
						// if has variable pointer
						if(val){
							printf("variable size = %d\n",wirteFile);
							cg_http_vallist_add(form->_POST, val);
							val = NULL;
						}
						
						if(!memcmp(boundaryPtr+strlen(boundary), "--", 2)){
							//printf("== [End of boundry] ==\n");
							done = TRUE;
							break;
						}
						else if((boundaryCtPtr = strstr(boundaryPtr, "\r\n\r\n"))){
							*boundaryCtPtr = 0;	
							ptr = boundaryCtPtr+4;
							parseHeader = FALSE;
							//printf("\n== [strlen boundary header] %d ==\n", strlen(boundaryPtr));

							valname[0] = 0;
							filename[0] = 0;
							
							contentDispos = strstr(boundaryPtr, "Content-Disposition:");
							contentType = strstr(boundaryPtr, "Content-Type:");	
							if(contentType){
								contentType+=13;
								if((tmp = strstr(contentType, "\r\n"))) *tmp = 0;
							}
							
							if(contentDispos){
								contentDispos+=20;
								if((tmp = strstr(contentDispos, "\r\n"))) *tmp = 0;
								//printf("== [Content-Disposition] %s ==\n", contentDispos);
								if((tmp=strstr(contentDispos, "name=")) && (tmp = tmp+6) && (tmp2 = strchr(tmp, '"'))){ //strlen("name=") +1 = 6
									if((tmp2-tmp)>tmplength ){
										memcpy(valname, tmp, tmplength);
									}else{
										memcpy(valname, tmp, tmp2-tmp);
									}
									valname[tmp2-tmp] = 0;
									//printf("== [_VAL_NAME_] %s ==\n", valname);
								}
								if((tmp=strstr(contentDispos, "filename=")) && (tmp = tmp+10) && (tmp2 = strchr(tmp, '"'))){//strlen("filename=") +1 = 10
									if(tmp2-tmp){
										if((tmp2-tmp)>tmplength ){
											memcpy(filename, tmp, tmplength);
										}else{
											memcpy(filename, tmp, tmp2-tmp);
										}
										filename[tmp2-tmp] = 0;
										//printf("== [_FILE_] %s ==\n", filename);
										filepath = tempnam(HTTPD_TMP_PATH, "file");
										//printf("filepath: %s\n",filepath);
										if(!fp && filepath){
											wirteFile = 0;
											fp = cg_file_new();
											cg_file_setpath(fp, filepath); free(filepath);
											cg_file_setfilename(fp ,filename);
											if (!cg_file_open(fp, CG_FILE_OPEN_WRITE)) {
												printf("[Error] fopen %s fail\n", cg_file_getname(fp));
												cg_file_delete(fp);
												return FALSE;
											}
											if(valname[0] != 0) cg_file_setvalname(fp, valname);
											if(contentType) cg_file_setContentType(fp, contentType);
										}
										else {
											if(filepath) free(filepath);
											printf("[Error] file pointer is not null\n");
											return FALSE;
										}
									}									
								}
								else{
									if(!val){
										wirteFile = 0;
										val = cg_http_val_new();
										if(valname[0] != 0) cg_http_val_setname(val, valname);
									}
									else {
										printf("[Error] variable pointer is not null\n");
										return FALSE;
									}
								}
							}
						}
						else break; //need read a lot
					}
					
					//if(boundaryPtr != ptr){
						if((tmp =find_boundary(ptr, (readBuf+length-ptr), boundary, boundaryLen))){
							w_len = tmp - ptr;
							parseHeader = TRUE;
						}
						else{
							w_len = (readBuf+length) - ptr - boundaryLen;
							parseHeader = FALSE;
						}
						if(w_len){
							if(fp) cg_file_write(fp, (CgByte *)ptr, sizeof(char)*w_len); //fwrite(ptr, sizeof(char), w_len, fp); 
							else if(val){
								if(w_len) cg_http_val_appendvalue(val, ptr, w_len);
							}
							
							ptr += w_len;
							wirteFile+=w_len;
						}
					//}
				} 
				while(ptr < (readBuf+length) && ((readBuf+length) - ptr) > boundaryLen);
				
				if(!done){
					if((w_len = readBuf+length - ptr)){
						memmove(readBuf, ptr, w_len);
						readBufPtr = readBuf+w_len;
						maxReadLen = READBUF_LENGTH - w_len;
					}
					else{
						readBufPtr = readBuf;
						maxReadLen = READBUF_LENGTH;
					}
				}
			}
			else{
				tries++;
			}
		}
		
		if(fp) {
			printf("file size = %d\n",wirteFile);
			cg_file_close(fp);
			cg_filelist_add(form->_FILE, fp);
		}
		
		if(val){
			printf("variable size = %d\n",wirteFile);
			cg_http_vallist_add(form->_POST, val);
		}
		
		printf("==== [Total ReadLine %d] ====\n", readLen);
		
		if((tries >= CG_HTTP_CWMP_SEND_RECV_TRY_COUNT)){
			printf("[Error] retry too many (tries = %d)\n", tries);
			return FALSE;
		}

		if (readLen <= 0){
			printf("[Error] no read content\n");
			return FALSE;
		}
		//cg_http_packet_setcontentpointer(httpPkt, NULL, readLen);
	}
	else return FALSE;
	
	return TRUE;
}

#endif
/****************************************
* cg_http_packet_read
****************************************/

#ifdef ZYXEL_PATCH /*support save content to file, by ZyXEL charisse*/
BOOL cg_http_packet_read(CgHttpPacket *httpPkt, CgSocket *sock, BOOL onlyHeader, char *lineBuf, int lineBufSize, char *savedFile)
#else
BOOL cg_http_packet_read(CgHttpPacket *httpPkt, CgSocket *sock, BOOL onlyHeader, char *lineBuf, int lineBufSize)
#endif
{
	cg_log_debug_l4("Entering...\n");


	memset(lineBuf, 0, lineBufSize);
	cg_http_packet_clear(httpPkt);
	cg_http_packet_read_headers(httpPkt, sock, lineBuf, lineBufSize);
	
	cg_log_debug_l4("Leaving...\n");

	if (onlyHeader)
		return TRUE;
#ifdef ZYXEL_PATCH  /*support save content to file, by ZyXEL charisse*/
	memset(lineBuf, 0, lineBufSize);

	if(savedFile == NULL)
		return cg_http_packet_read_body(httpPkt, sock, lineBuf, lineBufSize);
	else
		return cg_http_packet_read_body_to_file(httpPkt, sock, lineBuf, lineBufSize, savedFile);
#else
	return cg_http_packet_read_body(httpPkt, sock, lineBuf, lineBufSize);
#endif
}

/****************************************
* cg_http_packet_getheadersize
****************************************/

int cg_http_packet_getheadersize(CgHttpPacket *httpPkt)
{
	CgHttpHeader *header;
	int headerSize;
	char *name;
	char *value;
	
	cg_log_debug_l4("Entering...\n");

	headerSize = 0;
	for (header = cg_http_packet_getheaders(httpPkt); header != NULL; header = cg_http_header_next(header)) {
		name = cg_http_header_getname(header);
		value = cg_http_header_getvalue(header);
		headerSize += cg_strlen(name); 
		headerSize += sizeof(CG_HTTP_COLON)-1; 
		headerSize += sizeof(CG_HTTP_SP)-1; 
		headerSize += cg_strlen(value); 
		headerSize += sizeof(CG_HTTP_CRLF)-1; 
	}
	headerSize += sizeof(CG_HTTP_CRLF)-1; 

	cg_log_debug_l4("Leaving...\n");

	return headerSize;
}

/****************************************
* cg_http_packet_copy
****************************************/

void cg_http_packet_copy(CgHttpPacket *destHttpPkt, CgHttpPacket *srcHttpPkt)
{
	CgHttpHeader *srcHeader;
	CgHttpHeader *destHeader;
	
	cg_log_debug_l4("Entering...\n");

	cg_http_packet_clear(destHttpPkt);
	
	/**** copy headers ****/
	for (srcHeader = cg_http_packet_getheaders(srcHttpPkt); srcHeader != NULL; srcHeader = cg_http_header_next(srcHeader)) {
		destHeader = cg_http_header_new();
		cg_http_header_setname(destHeader, cg_http_header_getname(srcHeader));
		cg_http_header_setvalue(destHeader, cg_http_header_getvalue(srcHeader));
		cg_http_packet_addheader(destHttpPkt, destHeader);
	}
	
	/**** copy content ****/
	cg_http_packet_setcontent(destHttpPkt, cg_http_packet_getcontent(srcHttpPkt));

	cg_log_debug_l4("Leaving...\n");
}

/****************************************
* cg_http_packet_print
****************************************/

void cg_http_packet_print(CgHttpPacket *httpPkt)
{
	CgHttpHeader *header;
	char *content;
	long contentLen;

	cg_log_debug_l4("Entering...\n");

	/**** print headers ****/
	for (header = cg_http_packet_getheaders(httpPkt); header != NULL; header = cg_http_header_next(header)) {
	cg_log_debug_s("%s: %s\n",
			cg_http_header_getname(header),
			cg_http_header_getvalue(header));
	}
	cg_log_debug_s("\n");
	
	/**** print content ****/
	content = cg_http_packet_getcontent(httpPkt);
	contentLen = cg_http_packet_getcontentlength(httpPkt);
	
	if (content != NULL && 0 < contentLen)
		cg_log_debug_s("%s\n", content);

	cg_log_debug_l4("Leaving...\n");
}
