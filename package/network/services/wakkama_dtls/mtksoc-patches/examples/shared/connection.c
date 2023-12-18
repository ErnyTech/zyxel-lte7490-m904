/*******************************************************************************
 *
 * Copyright (c) 2013, 2014 Intel Corporation and others.
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
 *    David Navarro, Intel Corporation - initial API and implementation
 *    Pascal Rieux - Please refer to git log
 *
 *******************************************************************************/

#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#ifdef HTTP_PROTOCOL
#include "http_utils.h"
#endif
#ifdef COAP_PROTOCOL
#include "connection.h"
#endif
#include "commandline.h"
#ifdef COAP_PROTOCOL
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#endif
#ifdef HTTP_PROTOCOL
tr369ConnCB_t *connCbPtr = NULL;
#endif

#ifdef COAP_PROTOCOL
int create_socket(const char * portStr, int addressFamily)
{
    int s = -1;
    struct addrinfo hints;
    struct addrinfo *res;
    struct addrinfo *p;

    memset(&hints, 0, sizeof hints);
    hints.ai_family = addressFamily;
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_flags = AI_PASSIVE;

    if (0 != getaddrinfo(NULL, portStr, &hints, &res))
    {
        return -1;
    }

    for(p = res ; p != NULL && s == -1 ; p = p->ai_next)
    {
        s = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
        if (s >= 0)
        {
            if (-1 == bind(s, p->ai_addr, p->ai_addrlen))
            {
                close(s);
                s = -1;
            }
        }
    }

    freeaddrinfo(res);

    return s;
}
#endif

#ifdef HTTP_PROTOCOL
int create_socket(const char * portStr, int addressFamily)
{
    int s = -1;
    struct addrinfo hints;
    struct addrinfo *res;
    struct addrinfo *p;
	fprintf(stderr, "%s() Jessie Enter\n", __FUNCTION__);
    memset(&hints, 0, sizeof hints);
    hints.ai_family = addressFamily;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    if (0 != getaddrinfo(NULL, portStr, &hints, &res))
    {
        return -1;
    }

    for(p = res ; p != NULL && s == -1 ; p = p->ai_next)
    {
        s = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
        if (s >= 0)
        {
            if (-1 == bind(s, p->ai_addr, p->ai_addrlen))
            {
                close(s);
                s = -1;
            }
        }
    }
	fprintf(stderr, "%s() Jessie Leave\n", __FUNCTION__);
    freeaddrinfo(res);

    return s;
}
#endif

connection_t * connection_find(connection_t * connList,
                               struct sockaddr_storage * addr,
                               size_t addrLen)
{
    connection_t * connP;

    connP = connList;
    while (connP != NULL)
    {
        if ((connP->addrLen == addrLen)
         && (memcmp(&(connP->addr), addr, addrLen) == 0))
        {
            return connP;
        }
        connP = connP->next;
    }

    return connP;
}

connection_t * connection_new_incoming(connection_t * connList,
                                       int sock,
                                       struct sockaddr * addr,
                                       size_t addrLen)
{
    connection_t * connP;

    connP = (connection_t *)lwm2m_malloc(sizeof(connection_t));
    if (connP != NULL)
    {
        connP->sock = sock;
        memcpy(&(connP->addr), addr, addrLen);
        connP->addrLen = addrLen;
        connP->next = connList;
    }

    return connP;
}

connection_t * connection_create(connection_t * connList,
                                 int sock,
                                 char * host,
                                 char * port,
                                 int addressFamily)
{
    struct addrinfo hints;
    struct addrinfo *servinfo = NULL;
    struct addrinfo *p;
    int s;
    struct sockaddr *sa;
    socklen_t sl;
    connection_t * connP = NULL;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = addressFamily;
    hints.ai_socktype = SOCK_DGRAM;

    if (0 != getaddrinfo(host, port, &hints, &servinfo) || servinfo == NULL) return NULL;

    // we test the various addresses
    s = -1;
    for(p = servinfo ; p != NULL && s == -1 ; p = p->ai_next)
    {
        s = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
        if (s >= 0)
        {
            sa = p->ai_addr;
            sl = p->ai_addrlen;
            if (-1 == connect(s, p->ai_addr, p->ai_addrlen))
            {
                close(s);
                s = -1;
            }
        }
    }
    if (s >= 0)
    {
        connP = connection_new_incoming(connList, sock, sa, sl);
        close(s);
    }
    if (NULL != servinfo) {
        freeaddrinfo(servinfo);
    }

    return connP;
}

#ifdef HTTP_PROTOCOL
connection_t * http_connection_create(connection_t * connList,
                                 int sock,
                                 char * host,
                                 char * port,
                                 int addressFamily, lwm2m_context_t * contextP)//add lwm2m_context_t * contextP
{
    struct addrinfo hints;
    struct addrinfo *servinfo = NULL;
    struct addrinfo *p;
    int s;
    struct sockaddr *sa;
    socklen_t sl;
    connection_t * connP = NULL;
	char thehostname[256]={0};
	char username[128] = {0};
	char password[128] = {0};
	char ip_addr[128 + 1];
	int err = 0;
	CgSocket *sock_cg = NULL;
	int port_int;
	servInfo_t servInfoPtr;
	tr369SessCB_t *sessionCb = NULL;
	transferInfo_t  *transferInfo = NULL;
	connCbPtr = cwmp_conn_new();
	
	memset(&servInfoPtr, 0, sizeof(servInfoPtr));
	strncpy(servInfoPtr.url, host, sizeof(servInfoPtr.url));
	if ((err = cwmp_parseurl( servInfoPtr.url, thehostname, (struct sockaddr *)&(servInfoPtr.ipAddress), &servInfoPtr.port, servInfoPtr.path, &servInfoPtr.isSecure, servInfoPtr.hostname, eMULTIP_CHOOSE_REGULAR, "IPv4")) != 0){
		printf("%s: cwmp_parseurl(%d) failed", __FUNCTION__, err);
	}
	cwmp_ip_ip2string(&(servInfoPtr.ipAddress), ip_addr);


#if 0//Jessie hard code test
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = addressFamily;
    hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;
	printf("%s() Jessie Enter host=%s,port=%s\n", __FUNCTION__, host, port);//host http:// is not removed!
	
    if (0 != getaddrinfo(servInfoPtr.hostname, port, &hints, &servinfo) || servinfo == NULL) return NULL;
	printf("%s() Jessie not return NULL\n", __FUNCTION__);
#endif	
		if(connCbPtr){
			cwmp_conn_set_client(connCbPtr, &servInfoPtr, CG_HTTP_POST);

			/*create a session*/
			connCbPtr->sessionCb = cwmp_sess_new();

			sessionCb = connCbPtr->sessionCb;

			/*assign download information*/
			transferInfo = cwmp_transfer_new();

			sessionCb->transferInfo = transferInfo;

			if( cwmp_sess_file_proc(connCbPtr, contextP) != 0){//add contextP
				printf("%s() Jessie 77777777777\n", __FUNCTION__);
			} else {
				printf("%s() Jessie 8888888888888\n", __FUNCTION__);
			}
		}
#if 0//Jessie hard code test		
    if (NULL != servinfo) {
        freeaddrinfo(servinfo);
    }
#endif
    return connP;
}
#endif
void connection_free(connection_t * connList)
{
    while (connList != NULL)
    {
        connection_t * nextP;

        nextP = connList->next;
        lwm2m_free(connList);

        connList = nextP;
    }
}

int connection_send(connection_t *connP,
                    uint8_t * buffer,
                    size_t length)
{
    int nbSent;
    size_t offset;

#ifdef LWM2M_WITH_LOGS
    char s[INET6_ADDRSTRLEN];
    in_port_t port;

    s[0] = 0;

    if (AF_INET == connP->addr.sin6_family)
    {
        struct sockaddr_in *saddr = (struct sockaddr_in *)&connP->addr;
        inet_ntop(saddr->sin_family, &saddr->sin_addr, s, INET6_ADDRSTRLEN);
        port = saddr->sin_port;
    }
    else if (AF_INET6 == connP->addr.sin6_family)
    {
        struct sockaddr_in6 *saddr = (struct sockaddr_in6 *)&connP->addr;
        inet_ntop(saddr->sin6_family, &saddr->sin6_addr, s, INET6_ADDRSTRLEN);
        port = saddr->sin6_port;
    }

    fprintf(stderr, "Sending %lu bytes to [%s]:%hu\r\n", length, s, ntohs(port));

    output_buffer(stderr, buffer, length, 0);
#endif

    offset = 0;
    while (offset != length)
    {
        nbSent = sendto(connP->sock, buffer + offset, length - offset, 0, (struct sockaddr *)&(connP->addr), connP->addrLen);
        if (nbSent == -1) return -1;
        offset += nbSent;
    }
    return 0;
}

uint8_t lwm2m_buffer_send(void * sessionH,
                          uint8_t * buffer,
                          size_t length,
                          void * userdata)
{
    connection_t * connP = (connection_t*) sessionH;

    (void)userdata; /* unused */

    if (connP == NULL)
    {
        fprintf(stderr, "#> failed sending %lu bytes, missing connection\r\n", length);
        return COAP_500_INTERNAL_SERVER_ERROR ;
    }

    if (-1 == connection_send(connP, buffer, length))
    {
        fprintf(stderr, "#> failed sending %lu bytes\r\n", length);
        return COAP_500_INTERNAL_SERVER_ERROR ;
    }

    return COAP_NO_ERROR;
}

bool lwm2m_session_is_equal(void * session1,
                            void * session2,
                            void * userData)
{
    (void)userData; /* unused */

    return (session1 == session2);
}
