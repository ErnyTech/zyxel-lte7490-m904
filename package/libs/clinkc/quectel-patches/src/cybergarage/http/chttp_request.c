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
*	File: chttp_request.c
*
*	Revision:
*
*	02/01/05
*		- first revision
*	10/25/05
*		- Added support for libcurl in cg_http_request_post().
*	10/26/05
*		- Changed to use CURLOPT_CUSTOMREQUEST instead of CURLOPT_POST in cg_http_request_post().
*	19-Jan-06 Aapo Makela
*		- Fixed to convert absolute URLs to relative
*		- Fixed evil crash bug, when receiving malformed HTTP request
*	02/01/07
*		- Fixed cg_http_request_post() not to hung up when the request method is HEAD.
*		- Added a onlyHeader parameter to cg_http_response_read() and cg_http_response_packet().
*	02/19/07
*		-  Changed cg_http_request_post() to add a user agent, CyberGarage HTTP/1.0, as default.
*		-  Changed CG_HTTP_USERAGENT to CG_HTTP_USERAGENT_DEFAULT to add CG_HTTP_USERAGENT as the normal header define.
*	03/11/07
*		-  Added a secure parameter to cg_http_request_post() when the compiler flag, CG_USE_OPENSSL,is enabled.
*	06/13/07 Fabrice Fontaine Orange
*		- Fixed a memory leak in cg_http_request_delete().
*	10/22/07 Aapo Makela
*		- Disable Expect header because it causes IOP issues.
*	12/13/07 Aapo Makela
*		- Fixes to work in out-of-memory situations
*	11/18/08
*		- Changed cg_http_request_post_main() to write the first line at a time because SIGPIPE occurred on some HTTP server such as Flickr.
*
******************************************************************/

#include <cybergarage/util/clist.h>
#include <cybergarage/http/chttp.h>
#include <cybergarage/net/csocket.h>
#include <cybergarage/util/clog.h>
#include <cybergarage/util/cstring.h>
#include <cybergarage/util/cthread.h>

#include <string.h>

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#ifdef CG_HTTP_CURL
#include <curl/curl.h>
#endif

#ifdef CG_SHOW_TIMINGS
#include <sys/time.h>
#include <time.h>

long int cg_total_elapsed_time;
#endif

#ifdef ZYXEL_PATCH
//#define FIRMWARE_STORE_PATH "/var/firmwareUpgrade"
#define FIRMWARE_STORE_PATH "/tmp/firmwareUpgrade"
#define CONFIG_STORE_PATH "/var/restoreConfig"
#define LOCAL_CERT_STORE_PATH "/var/local_cert"
#define WWAN_STORE_PATH "/var/wwan/tmp_wwanpackage"
#endif

/****************************************
* cg_http_request_new
****************************************/

CgHttpRequest *cg_http_request_new()
{
	CgHttpRequest *httpReq;

	cg_log_debug_l4("Entering...\n");

	httpReq = (CgHttpRequest *)malloc(sizeof(CgHttpRequest));
	memset(httpReq,0,sizeof(CgHttpRequest));

	if ( NULL != httpReq )
	{	
		cg_http_packet_init((CgHttpPacket *)httpReq);
		httpReq->method = cg_string_new();
		httpReq->version = cg_string_new();
		httpReq->uri = cg_string_new();
		httpReq->userAgent = cg_string_new();
		httpReq->httpRes = cg_http_response_new();
		httpReq->postURL = cg_net_url_new();
#ifdef ZYXEL_PATCH /*support ssl, ZyXEL 2013*/
		httpReq->ctxdata = NULL;
		httpReq->form._FILE = NULL;
		httpReq->form._GET = NULL;
		httpReq->form._POST = NULL;
#endif
		httpReq->authList = NULL;

		cg_http_request_setversion(httpReq, CG_HTTP_VER11);
		cg_http_request_setuseragent(httpReq, CG_HTTP_USERAGENT_DEFAULT);

		cg_http_request_setsocket(httpReq, NULL);
		cg_http_request_setuserdata(httpReq, NULL);

		cg_http_request_settimeout(httpReq, CG_HTTP_CONN_TIMEOUT);
	}
	
	cg_log_debug_l4("Leaving...\n");

	return httpReq;
}

/****************************************
* cg_http_request_delete
****************************************/

void cg_http_request_delete(CgHttpRequest *httpReq)
{
	cg_log_debug_l4("Entering...\n");

	cg_http_packet_clean((CgHttpPacket *)httpReq);
	cg_string_delete(httpReq->method);
	cg_string_delete(httpReq->version);
	cg_string_delete(httpReq->uri);
	/* ADD Fabrice Fontaine Orange 23/04/2007 */
	/* Memory leak correction */
	cg_string_delete(httpReq->userAgent);
	/* ADD END Fabrice Fontaine Orange 23/04/2007 */
	cg_http_response_delete(httpReq->httpRes);
	cg_net_url_delete(httpReq->postURL);
#ifdef ZYXEL_PATCH
	if(httpReq->form._GET){ cg_http_vallist_delete(httpReq->form._GET); httpReq->form._GET = NULL;}
	if(httpReq->form._POST){ cg_http_vallist_delete(httpReq->form._POST); httpReq->form._POST = NULL;}
	if(httpReq->form._FILE){ cg_filelist_real_delete(httpReq->form._FILE); httpReq->form._FILE = NULL;}
#endif
	
	free(httpReq);

	cg_log_debug_l4("Leaving...\n");
}

/****************************************
* cg_http_request_clear
****************************************/

void cg_http_request_clear(CgHttpRequest *httpReq)
{
	cg_log_debug_l4("Entering...\n");

	cg_http_packet_clear((CgHttpPacket *)httpReq);
	cg_http_request_setmethod(httpReq, NULL);
	cg_http_request_seturi(httpReq, NULL);
	cg_http_request_setversion(httpReq, NULL);
	cg_net_url_clear(httpReq->postURL);
	cg_http_request_setuserdata(httpReq, NULL);
#ifdef ZYXEL_PATCH
#ifdef PACKAGE_webgui_vue
	if(httpReq->needChgPasswd) {httpReq->needChgPasswd = FALSE;}
	if(httpReq->loginUserName) {memset(httpReq->loginUserName, '\0', sizeof(httpReq->loginUserName));}
	if(httpReq->loginLevel) {memset(httpReq->loginLevel, '\0', sizeof(httpReq->loginLevel));}
#endif
	if(httpReq->form._GET){ cg_http_vallist_delete(httpReq->form._GET); httpReq->form._GET = NULL;}
	if(httpReq->form._POST){ cg_http_vallist_delete(httpReq->form._POST); httpReq->form._POST = NULL;}
	if(httpReq->form._FILE){ cg_filelist_real_delete(httpReq->form._FILE); httpReq->form._FILE = NULL;}
#endif
	cg_log_debug_l4("Leaving...\n");
}

/****************************************
* cg_http_request_setmethod
****************************************/

void cg_http_request_setmethod(CgHttpRequest *httpReq, char *value)
{
	cg_log_debug_l4("Entering...\n");

	cg_string_setvalue(httpReq->method, value);

	cg_log_debug_l4("Leaving...\n");
}

/****************************************
* cg_http_request_setversion
****************************************/

void cg_http_request_setversion(CgHttpRequest *httpReq, char *value)
{
	cg_log_debug_l4("Entering...\n");

	cg_string_setvalue(httpReq->version, value);

	cg_log_debug_l4("Leaving...\n");
}

/****************************************
* cg_http_request_setuseragent
****************************************/

void cg_http_request_setuseragent(CgHttpRequest *httpReq, char *value)
{
	cg_log_debug_l4("Entering...\n");

	cg_string_setvalue(httpReq->userAgent, value);

	cg_log_debug_l4("Leaving...\n");
}

/****************************************
* cg_http_request_setuseragent
****************************************/

void cg_http_request_addtouseragent(CgHttpRequest *httpReq, char *value)
{
	cg_log_debug_l4("Entering...\n");

	cg_string_addvalue(httpReq->userAgent, value);

	cg_log_debug_l4("Leaving...\n");
}

/****************************************
* cg_http_request_seturi
****************************************/

void cg_http_request_seturi(CgHttpRequest *httpReq, char *value)
{
	cg_log_debug_l4("Entering...\n");

	cg_string_setvalue(httpReq->uri, value);

	cg_log_debug_l4("Leaving...\n");
}

/****************************************
* cg_http_request_getmethod
****************************************/

char *cg_http_request_getmethod(CgHttpRequest *httpReq)
{
	cg_log_debug_l4("Entering...\n");

	cg_log_debug_l4("Leaving...\n");

	return cg_string_getvalue(httpReq->method);
}

/****************************************
* cg_http_request_getversion
****************************************/

char *cg_http_request_getversion(CgHttpRequest *httpReq)
{
	cg_log_debug_l4("Entering...\n");

	cg_log_debug_l4("Leaving...\n");

	return cg_string_getvalue(httpReq->version);
}

/****************************************
* cg_http_request_getversion
****************************************/

char *cg_http_request_getuseragent(CgHttpRequest *httpReq)
{
	cg_log_debug_l4("Entering...\n");

	cg_log_debug_l4("Leaving...\n");

	return cg_string_getvalue(httpReq->userAgent);
}
/****************************************
* cg_http_request_geturi
****************************************/

char *cg_http_request_geturi(CgHttpRequest *httpReq)
{
	cg_log_debug_l4("Entering...\n");

	cg_log_debug_l4("Leaving...\n");

	return cg_string_getvalue(httpReq->uri);
}

/****************************************
* cg_http_request_setsocket
****************************************/

void cg_http_request_setsocket(CgHttpRequest *httpReq, CgSocket *sock)
{
	cg_log_debug_l4("Entering...\n");

	httpReq->sock = sock;

	cg_log_debug_l4("Leaving...\n");
}

/****************************************
* cg_http_request_getsocket
****************************************/

CgSocket *cg_http_request_getsocket(CgHttpRequest *httpReq)
{
	cg_log_debug_l4("Entering...\n");

	cg_log_debug_l4("Leaving...\n");

	return httpReq->sock;
}

/****************************************
* cg_http_request_post
****************************************/

#if !defined(CG_HTTP_CURL)

CgHttpResponse *cg_http_request_post_main(CgHttpRequest *httpReq, char *ipaddr, int port, BOOL isSecure)
{
	CgSocket *sock;
	char *method, *uri, *version;
#ifdef CG_SHOW_TIMINGS
	struct timeval start_time, end_time, elapsed_time;
#endif		
	CgString *firstLine;
	
	cg_log_debug_l4("Entering...\n");

#ifdef CG_SHOW_TIMINGS
	gettimeofday(&start_time, NULL);
#endif		

	cg_http_response_clear(httpReq->httpRes);

	cg_log_debug_s("(HTTP) Posting:\n");
	cg_http_request_print(httpReq);

#if defined(CG_USE_OPENSSL)
	if (isSecure == FALSE)
		sock = cg_socket_stream_new();
	else{
		sock = cg_socket_ssl_new();
#ifdef ZYXEL_PATCH /*support ssl, ZyXEL 2013*/
		cg_socket_setctx(sock, httpReq->ctxdata);
#endif
	}
#else
	sock = cg_socket_stream_new();
#endif

	cg_socket_settimeout(sock, cg_http_request_gettimeout(httpReq));
#ifdef ZYXEL_PATCH /*support binding interface*/
	if (cg_socket_connect(sock, ipaddr, port, NULL) == FALSE) {
#else
	if (cg_socket_connect(sock, ipaddr, port) == FALSE) {
#endif
		cg_socket_delete(sock);
		return httpReq->httpRes;		
	}
	
	cg_http_request_sethost(httpReq, ipaddr, port);
	cg_http_packet_setheadervalue((CgHttpPacket*)httpReq, CG_HTTP_USERAGENT, cg_http_request_getuseragent(httpReq));

	method = cg_http_request_getmethod(httpReq);
	uri = cg_http_request_geturi(httpReq);
	version = cg_http_request_getversion(httpReq);

	if (method == NULL || uri == NULL || version == NULL) {
		cg_socket_close(sock);
		cg_socket_delete(sock);
		return httpReq->httpRes;		
	}	
	
#ifdef CG_SHOW_TIMINGS	
cg_log_debug_s("\nRequest: %s%s%s:%d%s%s%s\n", method, CG_HTTP_SP, ipaddr, port, uri, CG_HTTP_SP, version);
#endif
	/**** send first line ****/
	firstLine = cg_string_new();
	cg_string_addvalue(firstLine, method);
	cg_string_addvalue(firstLine, CG_HTTP_SP);
	cg_string_addvalue(firstLine, uri);
	cg_string_addvalue(firstLine, CG_HTTP_SP);
	cg_string_addvalue(firstLine, version);
	cg_string_addvalue(firstLine, CG_HTTP_CRLF);
	cg_socket_write(sock, cg_string_getvalue(firstLine), cg_string_length(firstLine));
	cg_string_delete(firstLine);
	
	/**** send header and content ****/
	cg_http_packet_post((CgHttpPacket *)httpReq, sock);
	
	/**** read response ****/
#ifdef ZYXEL_PATCH /*support save content to file, by ZyXEL charisse*/
	cg_http_response_read(httpReq->httpRes, sock, cg_http_request_isheadrequest(httpReq), NULL);
#else
	cg_http_response_read(httpReq->httpRes, sock, cg_http_request_isheadrequest(httpReq));
#endif

#ifdef CG_SHOW_TIMINGS	
	gettimeofday(&end_time, NULL);
	timersub(&end_time, &start_time, &elapsed_time);
cg_log_debug_s("Getting HTTP-response completed. Elapsed time: "
	       "%ld msec\n", ((elapsed_time.tv_sec*1000) + 
			      (elapsed_time.tv_usec/1000)));
	cg_total_elapsed_time += (elapsed_time.tv_sec*1000000)+
				 (elapsed_time.tv_usec);
#endif
	cg_socket_close(sock);
	cg_socket_delete(sock);	
	
	cg_http_response_print(httpReq->httpRes);

	cg_log_debug_l4("Leaving...\n");

	return httpReq->httpRes;
}

CgHttpResponse *cg_http_request_post(CgHttpRequest *httpReq, char *ipaddr, int port)
{
	return cg_http_request_post_main(httpReq, ipaddr, port, FALSE);
}

#if defined(CG_USE_OPENSSL)
CgHttpResponse *cg_https_request_post(CgHttpRequest *httpReq, char *ipaddr, int port)
{
	return cg_http_request_post_main(httpReq, ipaddr, port, TRUE);
}
#endif

#endif

/****************************************
* cg_http_request_post (libcurl)
****************************************/

#if defined(CG_HTTP_CURL)

/**
 * Callback function for CURL to read each HTTP header line
 * CgStringTokenizer might have been a viable choice to do the parsing
 * of various fields. Then again, it would not have read correctly a
 * header line with, for example, the time (because of multiple use of colons):
 * Foo: 12:34:56 EEST DST
 */
static size_t cg_http_request_header_callback(void *ptr, size_t size, size_t nmemb, void *stream)
{
	char* headerLine = NULL;
	char* name = NULL;
	char* value = NULL;
	CgStringTokenizer* strTok = NULL;
	CgHttpResponse* httpRes = NULL;
	int head = 0;
	int tail = 0;
	
	cg_log_debug_l4("Entering...\n");

	if (stream == NULL || ptr == NULL)
	{
		return 0;
	}
	
	httpRes = (CgHttpResponse*) stream;
	headerLine = (char*) ptr;
	
	/* Read header items */
	if (cg_strncmp(headerLine, CG_HTTP_VER11, cg_strlen(CG_HTTP_VER11)) == 0
	    ||
	    cg_strncmp(headerLine, CG_HTTP_VER10, cg_strlen(CG_HTTP_VER10)) == 0)
	{
		/* <HTTP/version> <status code> <reason phrase> */
		strTok = cg_string_tokenizer_new(headerLine, CG_HTTP_STATUSLINE_DELIM);
		if (cg_string_tokenizer_hasmoretoken(strTok) == TRUE)
		{
			cg_http_response_setversion(httpRes, cg_string_tokenizer_nexttoken(strTok));
		}
		
		if (cg_string_tokenizer_hasmoretoken(strTok) == TRUE)
		{
			cg_http_response_setstatuscode(httpRes, atoi(cg_string_tokenizer_nexttoken(strTok)));
		}
		
		if (cg_string_tokenizer_hasmoretoken(strTok) == TRUE)
		{
			value = cg_string_tokenizer_nextalltoken(strTok);
			cg_strrtrim(value, CG_HTTP_STATUSLINE_DELIM, cg_strlen(CG_HTTP_STATUSLINE_DELIM));
			cg_http_response_setreasonphrase(httpRes, value);
		}
		
		cg_string_tokenizer_delete(strTok);
	}
	else
	{	
		/* Find the header delimiter */
		for (head = 0; head < size * nmemb; head++)
		{
			if (headerLine[head] == ':')
			{
				break;
			}
		}
		
		/* Unable to find a colon, this is not a valid header line */
		if (head <= 0 || head >= (size * nmemb) - 1)
		{
			return size * nmemb;
		}

		/* Take the header name */
		name = (char*) malloc(head + 1);

		if ( NULL == name ) 
		{
			cg_log_debug_s("Memory allocation failure!\n");
			return 0;
		}
		
		memcpy(name, headerLine, head);
		name[head] = '\0';

		/* Skip colon and space(s) */
		for (head++; head < size * nmemb; head++)
		{
			if (headerLine[head] != 0x20)
			{
				break;
			}
		}
		
		/* Unable to find anything sensible anymore */
		if (head >= (size * nmemb) - 1)
		{
			return size * nmemb;
		}
		
		/* Find the end of the actual value, without CRLF */
		for (tail = size * nmemb; tail > 0; tail --)
		{
			if (headerLine[tail] == '\r')
			{
				break;
			}
			else if (headerLine[tail] == '\n')
			{
				if (tail > 0 && headerLine[tail - 1] == '\r')
				{
					tail--;
					break;
				}
			}
		}

		/* Unable to find CRLF */
		if (tail <= head)
		{
			free(name);
			return size * nmemb;
		}

		/* Take the header value */
		value = (char*) malloc(tail - head + 1);

		if ( NULL == value )
		{
                        cg_log_debug_s("Memory allocation failure!\n");
                        return 0;
		}

		memcpy(value, headerLine + head, tail - head);
		value[tail - head] = '\0';

		/* Set the header value to the response */
		cg_http_response_setheadervalue(httpRes, name, value);

		free(name);
		free(value);
	}

	return size * nmemb;

	cg_log_debug_l4("Leaving...\n");
}

/**
 * Callback function for CURL to read each HTTP header line
 */
static size_t cg_http_request_content_callback(void *ptr, size_t size, size_t nmemb, void *data)
{
	cg_log_debug_l4("Entering...\n");

	/* Append the data */
	cg_http_response_appendncontent((CgHttpResponse*) data, ptr, size * nmemb);

	return size * nmemb;

	cg_log_debug_l4("Leaving...\n");
}

static int cg_http_request_progress_callback(void *ptr, double dltotal, double dlnow, double ultotal, double ulnow)
{
	CgThread *self = cg_thread_self();

	cg_log_debug_s("Curl call progressing... Got thread %p\n", self);

	if ( ( NULL != self ) && !cg_thread_isrunnable(self))
	{
		cg_log_debug_s("Thread is not runnable anymore! Informing libcurl to abort\n");
		return TRUE;
	}
		
	return FALSE;
}

CgHttpResponse *cg_http_request_post(CgHttpRequest *httpReq, char *ipaddr, int port)
{
	CgHttpResponse *httpRes;
	BOOL newCurl = FALSE;
	CURL *curl;
	CgHttpHeader *reqHeader;
	struct curl_slist *curlHeaderList; 
	CgString *headerStr;
	CURLcode res;
	char *uri, *method;
	char url[CG_NET_URI_MAXLEN];
	long retcode;
#ifdef CG_SHOW_TIMINGS
	struct timeval start_time, end_time, elapsed_time;
#endif		

	cg_log_debug_l4("Entering...\n");

#ifdef CG_SHOW_TIMINGS
	gettimeofday(&start_time, NULL);
#endif		

	httpRes = httpReq->httpRes;

	/* Clear the response data because new data will not
	 * overwrite it, but it is appended to the end */
	cg_string_clear(httpRes->content);

	cg_log_debug_s("Posting HTTP request (Curl)\n");
	cg_http_request_print(httpReq);

	cg_http_persistentconnection_lock();	
#ifdef CG_HTTP_USE_PERSISTENT_CONNECTIONS
	cg_log_debug_s("Looking for persistent connection to %s, port %d\n", ipaddr, port);
	curl = (CURL*)cg_http_persistentconnection_get(ipaddr, port);
	
	if (curl == NULL)
	{
		cg_log_debug_s("Persistent connection not found...\n");
#endif	
		curl = curl_easy_init();
		if (curl == NULL)
		{
			cg_http_persistentconnection_unlock();
			return httpReq->httpRes;		
		}
#ifdef CG_HTTP_USE_PERSISTENT_CONNECTIONS
		newCurl = TRUE;
	}
#endif	
	method = cg_http_request_getmethod(httpReq);
	uri = cg_http_request_geturi(httpReq);

	/**** method ****/
	curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, method);

	/**** url ****/
	if (uri && cg_strstr(uri, CG_NET_URI_PROTOCOL_DELIM) > 0)
	{
		curl_easy_setopt(curl, CURLOPT_URL, uri);
	}
	else
	{
		cg_net_gethosturl(ipaddr, port, uri, url, sizeof(url));
		curl_easy_setopt(curl, CURLOPT_URL, url);
		cg_log_debug_s("\n\nCURL: %s\n\n", url);
	}
	
	/**** header ****/
	curlHeaderList = NULL;
	headerStr = cg_string_new();
	for (reqHeader = cg_http_request_getheaders(httpReq); reqHeader; reqHeader = cg_http_header_next(reqHeader)) {
		cg_string_setvalue(headerStr, cg_http_header_getname(reqHeader));
		if (cg_string_addvalue(headerStr, CG_HTTP_COLON CG_HTTP_SP) &&
		    cg_string_addvalue(headerStr, cg_http_header_getvalue(reqHeader)))
			curlHeaderList = curl_slist_append(curlHeaderList, cg_string_getvalue(headerStr));
	}
	cg_string_delete(headerStr);
	/* Disable Expect header because it causes IOP issues */
	curlHeaderList = curl_slist_append(curlHeaderList, "Expect:");
	curl_easy_setopt(curl, CURLOPT_HTTPHEADER, curlHeaderList);

	/**** content ****/
	/*if (cg_http_request_ispostrequest(httpReq) == TRUE) {*/
	if (cg_http_request_getcontentlength(httpReq) > 0) {
		curl_easy_setopt(curl, CURLOPT_POSTFIELDS, cg_http_request_getcontent(httpReq));
		curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, cg_http_request_getcontentlength(httpReq));
	}
        else
        {
		curl_easy_setopt(curl, CURLOPT_POSTFIELDS, NULL);
		curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, 0);
        }

	/* This has to be enabled for progress callback to be called */
	curl_easy_setopt(curl, CURLOPT_NOPROGRESS,
			FALSE);

	/* Used for checking stack state during curl easy perform */
	curl_easy_setopt(curl, CURLOPT_PROGRESSFUNCTION,
			cg_http_request_progress_callback);

	/**** response header callback ****/
	curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION,
			cg_http_request_header_callback);
	curl_easy_setopt(curl, CURLOPT_WRITEHEADER, (void *)httpRes);

	/**** response content callback ****/
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION,
			cg_http_request_content_callback);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)httpRes);

	/**** useragent ****/
	curl_easy_setopt(curl, CURLOPT_USERAGENT, cg_http_request_getuseragent(httpReq) );

	/**** Prohibit curl from using signals ****/
	curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1);
	
	/**** Set the connection timeout so we don't wait forever ****/
	curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 
			       CG_HTTP_CURL_CONNECTTIMEOUT);
        curl_easy_setopt(curl, CURLOPT_TIMEOUT,
                               CG_HTTP_CONN_TIMEOUT);
#ifdef CG_SHOW_TIMINGS
cg_log_debug_s("\nRequest: %s%s%s\n", method, CG_HTTP_SP, url);
#endif
	
	/* Get the XML document with CURL */
	res = curl_easy_perform(curl);
	if (res != CURLE_OK)
		cg_log_debug_s("curl_easy_perform: %s\n",
			curl_easy_strerror(res));

	/* Set the content length, if it wasn't said in the header */
	if (cg_http_response_getcontentlength(httpRes) <= 0)
	{
		cg_http_response_setcontentlength(httpRes,
					  cg_string_length(httpRes->content));
	}

	curl_slist_free_all(curlHeaderList); 

	curl_easy_getinfo (curl, CURLINFO_HTTP_CODE, &retcode);
	cg_http_response_setstatuscode(httpRes, retcode);

#ifdef CG_SHOW_TIMINGS	
	gettimeofday(&end_time, NULL);
	timersub(&end_time, &start_time, &elapsed_time);
cg_log_debug_s("Getting HTTP-response completed. Elapsed time: "
	       "%ld msec\n", ((elapsed_time.tv_sec*1000) + 
			      (elapsed_time.tv_usec/1000)));
	cg_total_elapsed_time += (elapsed_time.tv_sec*1000000)+
				 (elapsed_time.tv_usec);
#endif

#ifdef CG_HTTP_USE_PERSISTENT_CONNECTIONS
	if (newCurl)
	{
		cg_log_debug_s("Putting new connection into cache: %s %d\n", ipaddr, port);
		cg_http_persistentconnection_put(ipaddr, port, curl);
	}
#else
	curl_easy_cleanup(curl);
#endif
	cg_http_persistentconnection_unlock();

	cg_log_debug_s("Response for HTTP request (Curl)\n");
	cg_http_response_print(httpReq->httpRes);
	
	return httpReq->httpRes;

	cg_log_debug_l4("Leaving...\n");
}

#endif

/****************************************
* cg_http_request_read
****************************************/
BOOL cg_http_request_read(CgHttpRequest *httpReq, CgSocket *sock)
{
	char lineBuf[CG_HTTP_READLINE_BUFSIZE] = "";
	CgStringTokenizer *strTok;
	int readLen;
	CgNetURI *uri = NULL;
	BOOL failed = FALSE;
	char *requestUri = NULL;
#ifdef ZYXEL_PATCH	
	char *tmp = NULL;
	char *contentType = NULL;
	char boundary[BOUNDARY_SIZE] = {0};
	char *method = NULL;
#endif

	cg_log_debug_l4("Entering...\n");

	cg_http_request_clear(httpReq);

	/* If first character(s) is \n or \r\n we ignore it(them) and read second line. */
	do {
		readLen = cg_socket_readline(sock, lineBuf, sizeof(lineBuf));
	} while (readLen >= 1 && readLen <=2);
	
	if (readLen <= 0)
		return FALSE;

	strTok = cg_string_tokenizer_new(lineBuf, CG_HTTP_STATUSLINE_DELIM);
	if (cg_string_tokenizer_hasmoretoken(strTok) == TRUE)
		cg_http_request_setmethod(httpReq, cg_string_tokenizer_nexttoken(strTok));
	else
		failed = TRUE;
	if (cg_string_tokenizer_hasmoretoken(strTok) == TRUE)
		cg_http_request_seturi(httpReq, cg_string_tokenizer_nexttoken(strTok));
	else 
		failed = TRUE;

	if (check_http_request_uri(httpReq) == TRUE){ //check uri fail == true ??
		failed = TRUE;
	}

	if (cg_string_tokenizer_hasmoretoken(strTok) == TRUE)
		cg_http_request_setversion(httpReq, cg_string_tokenizer_nexttoken(strTok));
	else 
		failed = TRUE;
	cg_string_tokenizer_delete(strTok);

	if (failed == TRUE) return FALSE;
	
	/* We could do some further validation for the HTTP-request? */
	
	/* Change URI to be relative (absolute not needed anymore) */
	uri = cg_net_uri_new();
	if (uri != NULL)
	{
		cg_net_uri_set(uri, cg_http_request_geturi(httpReq));
		if (cg_net_uri_isabsolute(uri) == TRUE && 
		    cg_net_uri_getrequest(uri) != NULL)
		{
			cg_http_request_seturi(httpReq, 
					       cg_net_uri_getrequest(uri));
		}
		cg_net_uri_delete(uri); uri = NULL;
	}
	
	/* Read headers */
	cg_http_packet_clear((CgHttpPacket *)httpReq);
	cg_http_packet_read_headers((CgHttpPacket *)httpReq, sock, lineBuf, sizeof(lineBuf));

#ifdef ZYXEL_PATCH
	/*File upload preprocessor*/
	cg_http_request_parse_uri(httpReq); //support parse GET variable
	
	method = cg_http_request_getmethod(httpReq);
	if((method && strcmp(method, CG_HTTP_GET)) && //method != GET
		(cg_http_packet_hasheader((CgHttpPacket *)httpReq, CG_HTTP_CONTENT_LENGTH) ||
		cg_http_packet_hasheader((CgHttpPacket *)httpReq, CG_HTTP_TRANSFER_ENCODING)))
	{
		if ((tmp = cg_http_packet_getheadervalue((CgHttpPacket *)httpReq, CG_HTTP_CONTENT_TYPE)))
		{
			contentType = strdup(tmp);
			if((tmp = strchr(contentType,';'))){ *tmp = '\0'; tmp = tmp+1;}
		}
		
		if(contentType && !strcmp(contentType, HTTP_POST_FORM_MULTIPART)){	
#if 0 //__ZYXEL__, Mark, Firmware Upgrade use orignal function, because cg_http_packet_parse_multipart is to slow
			requestUri = cg_http_request_geturi(httpReq);
			if(requestUri && strstr(requestUri, "/cgi-bin/Firmware_Upgrade") != NULL){
				if(!cg_http_packet_read_body_to_file((CgHttpPacket *)httpReq, sock, lineBuf, sizeof(lineBuf), FIRMWARE_STORE_PATH))
					failed = TRUE;
			}
			else
#endif		
			if(tmp && (tmp = strstr(tmp, "boundary="))){ 
				snprintf(boundary, BOUNDARY_SIZE, "--%s", tmp+9);
				if(!cg_http_packet_parse_multipart((CgHttpPacket *)httpReq, sock, boundary, &(httpReq->form)))
					failed = TRUE;
			}
			else failed = TRUE;
		}
		else{
			cg_http_packet_read_body((CgHttpPacket *)httpReq, sock, lineBuf, sizeof(lineBuf));
			if(contentType && !strcmp(contentType, HTTP_POST_FORM_URLENCODE))
				cg_http_request_parse_content(httpReq);
		}
			
		if(contentType) free(contentType);
	}
	
	if (failed == TRUE) return FALSE;
#if 0
	requestUri = cg_http_request_geturi(httpReq);
	if(strstr(requestUri, "/cgi-bin/Firmware_Upgrade") != NULL)
		cg_http_packet_read_body_to_file((CgHttpPacket *)httpReq, sock, lineBuf, sizeof(lineBuf), FIRMWARE_STORE_PATH);
	else if (strstr(requestUri, "/cgi-bin/Backup_Restore?action=Restore") != NULL)
		cg_http_packet_read_body_to_file((CgHttpPacket *)httpReq, sock, lineBuf, sizeof(lineBuf), CONFIG_STORE_PATH);
	else if (strstr(requestUri, "/cgi-bin/Certificates?action=import_local") != NULL)
		cg_http_packet_read_body_to_file((CgHttpPacket *)httpReq, sock, lineBuf, sizeof(lineBuf), LOCAL_CERT_STORE_PATH);
	else if (strstr(requestUri, "/cgi-bin/System?action=Wwan_Upgrade") != NULL)
                cg_http_packet_read_body_to_file((CgHttpPacket *)httpReq, sock, lineBuf, sizeof(lineBuf), WWAN_STORE_PATH);
	else
#endif
#else
	/* HTTP-request must have Content-Length or Transfer-Encoding header
	   in order to have body */
	if (cg_http_packet_hasheader((CgHttpPacket *)httpReq, CG_HTTP_CONTENT_LENGTH) ||
	    cg_http_packet_hasheader((CgHttpPacket *)httpReq, CG_HTTP_TRANSFER_ENCODING))
		cg_http_packet_read_body((CgHttpPacket *)httpReq, sock, lineBuf, sizeof(lineBuf));
#endif	
	cg_log_debug_l4("Leaving...\n");

	return TRUE;
}

/****************************************
* cg_http_response_postresponse
****************************************/

BOOL cg_http_request_postresponse(CgHttpRequest *httpReq, CgHttpResponse *httpRes)
{
	CgSocket *sock;
	char httpDate[CG_HTTP_DATE_MAXLEN];
	char *version, *reasonPhrase;
	int statusCode;
	char statusCodeBuf[CG_STRING_INTEGER_BUFLEN];
	
	cg_log_debug_l4("Entering...\n");

	sock = cg_http_request_getsocket(httpReq);
#ifdef ZYXEL_PATCH
	cg_socket_nopush(sock);
#endif
	cg_log_debug_s("Got request:\n");
	cg_http_request_print(httpReq);
	
	cg_http_response_setdate(httpRes, cg_http_getdate(cg_getcurrentsystemtime(), httpDate, sizeof(httpDate)));
	//prevent ClickJacking attack
	cg_http_response_setheadervalue(httpRes, CG_HTTP_XFRAME, CG_HTTP_XFRAME_SAMEORIGIN);
	cg_http_response_setheadervalue(httpRes, CG_HTTP_CSP, CG_HTTP_CSP_SELF);
	//is a marker used by the server to indicate that the MIME types advertised in the Content-Type headers should not be changed and be followed.
	cg_http_response_setheadervalue(httpRes, CG_HTTP_X_CTO, CG_HTTP_X_CTO_NOSNIFF);
	//provide protections for users of older web browsers that don't yet support CSP.
	cg_http_response_setheadervalue(httpRes, CG_HTTP_XSS_PROTECTION, CG_HTTP_XSS_PROTECTION_1_BLOCK);

#ifdef ZYXEL_SUPPORT_NESSUS
    cg_http_response_setheadervalue(httpRes, CG_HTTP_HSTS, CG_HTTP_HSTS_CONTEXT);
#endif

	version = cg_http_response_getversion(httpRes);
	statusCode = cg_http_response_getstatuscode(httpRes);	
	reasonPhrase = cg_http_response_getreasonphrase(httpRes);

	if (version == NULL || reasonPhrase == NULL)
		return FALSE;		

	cg_int2str(statusCode, statusCodeBuf, sizeof(statusCodeBuf));
		
	/**** send first line ****/
	cg_socket_write(sock, version, cg_strlen(version));
	cg_socket_write(sock, CG_HTTP_SP, sizeof(CG_HTTP_SP)-1);
	cg_socket_write(sock, statusCodeBuf, cg_strlen(statusCodeBuf));
	cg_socket_write(sock, CG_HTTP_SP, sizeof(CG_HTTP_SP)-1);
	cg_socket_write(sock, reasonPhrase, cg_strlen(reasonPhrase));
	cg_socket_write(sock, CG_HTTP_CRLF, sizeof(CG_HTTP_CRLF)-1);
	
	cg_log_debug_s("Posting response:\n");
	cg_http_response_print(httpRes);
		
	/**** send header and content ****/
	cg_http_packet_post((CgHttpPacket *)httpRes, sock);
#ifdef ZYXEL_PATCH
	cg_socket_push(sock);
#endif	
	cg_log_debug_l4("Leaving...\n");

	return TRUE;
}

/****************************************
* cg_http_response_postresponse
****************************************/
#ifdef ZYXEL_PATCH
#include <sys/stat.h> 
#include <fcntl.h>
BOOL cg_http_request_postFile(CgHttpRequest *httpReq, CgHttpResponse *httpRes, const char *file, BOOL chunked)
{
	CgSocket *sock;
	char httpDate[CG_HTTP_DATE_MAXLEN];
	char *version, *reasonPhrase;
	int statusCode;
	char statusCodeBuf[CG_STRING_INTEGER_BUFLEN];
	struct stat st;
	int fp = 0;
	BOOL ret = FALSE;
	
	cg_log_debug_l4("Entering...\n");
	if( ! stat(file, &st) && st.st_mode & S_IFREG && (fp = open(file, O_RDONLY)) >0){
		cg_http_response_setcontentlength(httpRes, st.st_size);
		
		sock = cg_http_request_getsocket(httpReq);
		cg_socket_nopush(sock);

		cg_log_debug_s("Got request:\n");
		cg_http_request_print(httpReq);
		
		cg_http_response_setdate(httpRes, cg_http_getdate(cg_getcurrentsystemtime(), httpDate, sizeof(httpDate)));
		//prevent ClickJacking attack
		cg_http_response_setheadervalue(httpRes, CG_HTTP_XFRAME, CG_HTTP_XFRAME_SAMEORIGIN);
		cg_http_response_setheadervalue(httpRes, CG_HTTP_CSP, CG_HTTP_CSP_SELF);
		//is a marker used by the server to indicate that the MIME types advertised in the Content-Type headers should not be changed and be followed.
		cg_http_response_setheadervalue(httpRes, CG_HTTP_X_CTO, CG_HTTP_X_CTO_NOSNIFF);
		//provide protections for users of older web browsers that don't yet support CSP.
		cg_http_response_setheadervalue(httpRes, CG_HTTP_XSS_PROTECTION, CG_HTTP_XSS_PROTECTION_1_BLOCK);

#ifdef ZYXEL_SUPPORT_NESSUS
        cg_http_response_setheadervalue(httpRes, CG_HTTP_HSTS, CG_HTTP_HSTS_CONTEXT);
#endif

		version = cg_http_response_getversion(httpRes);
		statusCode = cg_http_response_getstatuscode(httpRes);	
		reasonPhrase = cg_http_response_getreasonphrase(httpRes);

		if (version == NULL || reasonPhrase == NULL){
			close(fp);
			return FALSE;		
		}
		cg_int2str(statusCode, statusCodeBuf, sizeof(statusCodeBuf));
			
		/**** send first line ****/
		cg_socket_write(sock, version, cg_strlen(version));
		cg_socket_write(sock, CG_HTTP_SP, sizeof(CG_HTTP_SP)-1);
		cg_socket_write(sock, statusCodeBuf, cg_strlen(statusCodeBuf));
		cg_socket_write(sock, CG_HTTP_SP, sizeof(CG_HTTP_SP)-1);
		cg_socket_write(sock, reasonPhrase, cg_strlen(reasonPhrase));
		cg_socket_write(sock, CG_HTTP_CRLF, sizeof(CG_HTTP_CRLF)-1);
		
		cg_log_debug_s("Posting response:\n");
		cg_http_response_print(httpRes);
			
		/**** send header and content ****/
		cg_http_packet_post((CgHttpPacket *)httpRes, sock);
		#ifdef TELIA_CUSTOMIZATION
		cg_usual_sendfile(sock, fp, st.st_size);
		#else
		cg_socket_sendfile(sock, fp, st.st_size);
		#endif
		close(fp);
		
		ret = TRUE;
		cg_socket_push(sock);
		
	}
	cg_log_debug_l4("Leaving...\n");

	return ret;
}
#endif
/****************************************
* cg_http_request_poststatuscode
****************************************/

BOOL cg_http_request_poststatuscode(CgHttpRequest *httpReq, int httpStatCode)
{
	CgHttpResponse *httpRes;
	BOOL postRet;

	cg_log_debug_l4("Entering...\n");

	httpRes = cg_http_response_new();
	cg_http_response_setstatuscode(httpRes, httpStatCode);
	cg_http_response_setcontentlength(httpRes, 0);
	postRet = cg_http_request_postresponse(httpReq, httpRes);
	cg_http_response_delete(httpRes);

	cg_log_debug_l4("Leaving...\n");

	return postRet;
}

/****************************************
 * cg_http_request_postdata
 ****************************************/

BOOL cg_http_request_postdata(CgHttpRequest *httpReq, void *data, int dataLen)
{
	if (dataLen <= 0)
		return TRUE;
	
	cg_socket_write(cg_http_request_getsocket(httpReq), data, dataLen);
	
	return TRUE;
}

/****************************************
* cg_http_request_postchunkedsize
****************************************/

BOOL cg_http_request_postchunkedsize(CgHttpRequest *httpReq, int dataLen)
{
	CgSocket *sock;
	char chunkedChar[CG_STRING_LONG_BUFLEN+2];

	sock = cg_http_request_getsocket(httpReq);
#if defined(HAVE_SNPRINTF)
	snprintf(chunkedChar, sizeof(chunkedChar), "%x%s", dataLen, CG_HTTP_CRLF);
#else
	sprintf(chunkedChar, "%x%s", dataLen, CG_HTTP_CRLF);
#endif
	cg_socket_write(sock, chunkedChar, cg_strlen(chunkedChar));

	return TRUE;
}

/****************************************
* cg_http_request_postchunkeddata
****************************************/

BOOL cg_http_request_postchunkeddata(CgHttpRequest *httpReq, void *data, int dataLen)
{
	CgSocket *sock;

	if (dataLen <= 0)
		return TRUE;
	
	cg_http_request_postchunkedsize(httpReq, dataLen);
	sock = cg_http_request_getsocket(httpReq);
	cg_socket_write(sock, data, dataLen);
	cg_socket_write(sock, CG_HTTP_CRLF, sizeof(CG_HTTP_CRLF)-1);
	
	return TRUE;
}

/****************************************
 * cg_http_request_postlastchunk
 ****************************************/

BOOL cg_http_request_postlastchunk(CgHttpRequest *httpReq)
{
	CgSocket *sock;
	
	cg_http_request_postchunkedsize(httpReq, 0);
	sock = cg_http_request_getsocket(httpReq);
	cg_socket_write(sock, CG_HTTP_CRLF, sizeof(CG_HTTP_CRLF)-1);
	
	return TRUE;
}

/****************************************
* cg_http_response_copy
****************************************/

void cg_http_request_copy(CgHttpRequest *destHttpReq, CgHttpRequest *srcHttpReq)
{
	cg_log_debug_l4("Entering...\n");

	cg_http_request_setmethod(destHttpReq, cg_http_request_getmethod(srcHttpReq));
	cg_http_request_seturi(destHttpReq, cg_http_request_geturi(srcHttpReq));
	cg_http_request_setversion(destHttpReq, cg_http_request_getversion(srcHttpReq));

	cg_http_packet_copy((CgHttpPacket *)destHttpReq, (CgHttpPacket *)srcHttpReq);

	cg_log_debug_l4("Leaving...\n");
}

/****************************************
* cg_http_request_print
****************************************/

void cg_http_request_print(CgHttpRequest *httpReq)
{
	cg_log_debug_l4("Entering...\n");
	
	cg_log_debug_s("%s %s %s\n",
		cg_http_request_getmethod(httpReq),
		cg_http_request_geturi(httpReq),
		cg_http_request_getversion(httpReq));

	cg_http_packet_print((CgHttpPacket *)httpReq);
	
	cg_log_debug_l4("Leaving...\n");
}

#ifdef ZYXEL_PATCH
/*******************************************
* check_http_request_uri
********************************************/
BOOL check_http_request_uri(CgHttpRequest *httpReq)
{
	char *uri = NULL;
	char *pch = NULL;
	char tempStr[128] = {0};
	int len, i, check_ascii_0, check_ascii;
	BOOL bFail = FALSE;

	if (!httpReq) {
		return FALSE;
	}
	
	uri = cg_http_request_geturi(httpReq);
	if (!uri){
		return FALSE;
	}

	/********* case 1 *********/
	check_ascii_0 = uri[0];
	check_ascii = uri[1];
	//printf("uri[0] = %d : %c\n", check_ascii_0, uri[0]);
	//printf("uri[1] = %d : %c\n", check_ascii, uri[1]);
	if (check_ascii_0 == 47) //check  '/'
	{
		if (((check_ascii >= 48) && (check_ascii <= 57)) ||
			((check_ascii >= 65) && (check_ascii <= 90)) ||
			((check_ascii >= 97) && (check_ascii <= 122)) ||
			(check_ascii == 0)){
			//bFail = FALSE;
		}
		else{
			bFail = TRUE;
		}
	}

	if (bFail == TRUE){ //return bFail;
		//printf("http_request_parse_uri >>>>　Invalid uri -1!!! \n");
		return TRUE;
	}

	/********* case 2 *********/
	pch = strchr(uri, '?');  //check there is  no '=' before '?'
	
	if(pch != NULL){
		len = pch-uri;
		memset(tempStr, '\0', sizeof(tempStr));
		strncpy(tempStr, uri , len);
		if(strchr(tempStr, '=')){
			bFail = TRUE;
		}
	}
	if (bFail == TRUE){ //return bFail;
		//printf("http_request_parse_uri >>>>　Invalid uri -2!!! \n");
		return TRUE;
	}

	return FALSE;
}

/*******************************************
* cg_http_request_parse_uri
********************************************/
void cg_http_request_parse_uri(CgHttpRequest *httpReq)
{
	char *uri = NULL, *valStr = NULL, *tmp = NULL;
	
	if(!httpReq) return;
	
	uri = cg_http_request_geturi(httpReq);
	if(uri){
		if((tmp = strrchr(uri, '?'))){
			valStr = strdup(tmp+1);
		}
		else return;
	}
	if(valStr){
		cg_http_parse_val(valStr, &httpReq->form._GET);
		free(valStr);
	}
}
#if 1//Jessie
/*******************************************
* cg_http_request_parse_uri_POST
********************************************/
void cg_http_request_parse_uri_POST(CgHttpRequest *httpReq)
{
	char *uri = NULL, *valStr = NULL, *tmp = NULL;

	if(!httpReq) return;

	uri = cg_http_request_geturi(httpReq);
	if(uri){
		if((tmp = strrchr(uri, '?'))){
			valStr = strdup(tmp+1);
		}
		else return;
	}
	if(valStr){
		cg_http_parse_val(valStr, &httpReq->form._POST);
		printf("%s() Jessie after valStr=%s cg_http_parse_val &httpReq->form._POST\n", __FUNCTION__, valStr);
		free(valStr);
	}
}
#endif
void cg_http_request_parse_content(CgHttpRequest *httpReq)
{
	char *content = NULL, *valStr = NULL;
	
	if(!httpReq) return;
	
	content = cg_http_request_getcontent(httpReq);
	if(content){
		if(*content == '[' || *content == '{' || 
			(!(*content >= 'a' && *content <= 'z') && 
			!(*content >= 'A' && *content <= 'Z')) &&
			!(*content >= '0' && *content <= '9')) return ;
		valStr = strdup(content);
	}
	if(valStr){
		cg_http_parse_val(valStr, &httpReq->form._POST);
		free(valStr);
	}
}

#endif


//#if defined(ZyXEL_Online_Firmware_Upgrade) || defined(ZYXEL_FWUPGRADE_DHCP_SUB_OPTION43) || defined (ZYXEL_WEB_GUI_SHOW_ENABLE_ONLINE_FW_UPGRADE)
/*******************************************
* cg_http_customrequest_post
********************************************/
CgHttpResponse *cg_http_customrequest_post(CgHttpRequest *httpReq, char *ipaddr, int port, char *savedFile)
{

	CgSocket *sock = NULL;
	char *method, *uri, *version;
	CgString *firstLine;
	
	cg_log_debug_l4("Entering...\n");	
	
	cg_http_response_clear(httpReq->httpRes);

	cg_log_debug_s("(HTTP) Posting:\n");
	cg_http_request_print(httpReq);

	
	sock = cg_socket_stream_new();

	
	if (cg_socket_connect(sock, ipaddr, port, NULL) == FALSE) {
		cg_socket_delete(sock);
		return httpReq->httpRes;		
	}
	
	cg_http_request_sethost(httpReq, ipaddr, port);
	cg_http_packet_setheadervalue((CgHttpPacket*)httpReq, CG_HTTP_USERAGENT, cg_http_request_getuseragent(httpReq));
	
	method = cg_http_request_getmethod(httpReq);
	uri = cg_http_request_geturi(httpReq);
	version = cg_http_request_getversion(httpReq);
	
	if (method == NULL || uri == NULL || version == NULL) {
		cg_socket_close(sock);
		cg_socket_delete(sock);
		return httpReq->httpRes;		
	}	
	
	/**** send first line ****/
	firstLine = cg_string_new();
	cg_string_addvalue(firstLine, method);
	cg_string_addvalue(firstLine, CG_HTTP_SP);
	cg_string_addvalue(firstLine, uri);
	cg_string_addvalue(firstLine, CG_HTTP_SP);
	cg_string_addvalue(firstLine, version);
	cg_string_addvalue(firstLine, CG_HTTP_CRLF);
	cg_socket_write(sock, cg_string_getvalue(firstLine), cg_string_length(firstLine));
	cg_string_delete(firstLine);
	/**** send header and content ****/
	cg_http_packet_post((CgHttpPacket *)httpReq, sock);
	/**** read response ****/
	cg_http_response_read(httpReq->httpRes, sock, cg_http_request_isheadrequest(httpReq), savedFile);
	
	
	cg_socket_close(sock);
	cg_socket_delete(sock);	
	
	cg_http_response_print(httpReq->httpRes);

	cg_log_debug_l4("Leaving...\n");

	return httpReq->httpRes;
	
}
//#endif
