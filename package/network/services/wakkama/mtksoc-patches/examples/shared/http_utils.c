#include "http_utils.h"

int checkCertificate = 0;
int checkCertificateCN = 0;
servInfo_t redirectServInfo = {0};
portTable_t gPortTable[] = {
    { kSchemePatternHttp,           kDefaultHttpPort, kNormal}, 		// device rompager port:8080, send message to ACS port:80
    { kSchemePatternHttps,          kDefaultTlsPort, kSecure},
    { kSchemePatternFtp,        	kDefaultFtpControlPort, kNormal},
    { kSchemePatternIpp,            kDefaultIppPort, kNormal},
    { kSchemePatternSmtp,           kDefaultSmtpPort, kNormal},
    { kSchemePatternTelnet,       	kDefaultTelnetPort, kNormal},
    { (char *) 0,                   kDefaultHttpPort, kNormal}
};

/*************************
* Function 	: getPortTable()
* Description	: get the default port for each protocol
* Output 	: 
*************************/
static portTablePtr_t getPortTable(const char *theStringPtr) {
    portTablePtr_t theTablePtr;

    theTablePtr = gPortTable;

    while (theTablePtr->pattern != 0) {
		if (strcasecmp(theStringPtr, theTablePtr->pattern) == 0) {
			return theTablePtr;
        }
        theTablePtr++;
    }

    /*
        We didn't find a match for the scheme.
    */
    return (portTablePtr_t) 0;
}
/*************************
* Function 	: isValidIP()
* Description	: check the input IP address is validate or not
* Output 	: 
*************************/
static bool isValidIP(char *ipStr){	
	int i, cnt = 0, value;
	char *cp = ipStr;

	if(ipStr == NULL)
		return FALSE;
	
	for(; *cp != '\0'; cp++) {
		if((!isdigit(*cp)) && (*cp != '.')) {
			return FALSE;
		}
	}
	
	cp = ipStr;
	for(i = 24; i >= 0; i -= 8) {
		if((*cp == '\0') || (*cp == '.'))
			return FALSE;
		
		value = atoi(cp);
		if((value > 255) || (value < 0))
			return FALSE;

		cnt++;
		if((cp = strchr(cp, '.')) == NULL)
			break;			
		cp++;
	}
	
	if(cnt != 4)
		return FALSE;

	return TRUE;
}
/*************************
* Function 	: cwmp_ip_ip2string()
* Description	: transfer ip address from ip to string
* Output 	: string
*************************/
void cwmp_ip_ip2string(
	struct sockaddr* ip, char *str
){
	memset(str, 0, sizeof(str));
	switch(ip->sa_family)
	{
		case AF_INET:
			inet_ntop(AF_INET, ip->sa_data, str, INET6_ADDRSTRLEN);
			break;
		case AF_INET6:
			inet_ntop(AF_INET6, ip->sa_data, str, INET6_ADDRSTRLEN);
			break;
		default:
			break;
	}
}
/*************************
* Function 	: resolveSrvUrlName()
* Description	: resolve the host name to ip address with DNS request
* Output 	: 
*************************/
static int resolveSrvUrlName(
	char *name, 
	struct sockaddr iPAddress_ori,
	multi_choose_t chostype,
	struct sockaddr *ip,
	const char *availableMode
){
	int num_of_ip = 0;
	int rand_num = 0;
	int issameip = 0;
	struct addrinfo hints, *ifAddrInfo = NULL, he;
	char ip_addr[INET6_ADDRSTRLEN];
	char ip_addr2[INET6_ADDRSTRLEN];
	int ret = 0;
	char *ps1 = NULL, *ps2 = NULL;
	char connReqUrl[257] = {0};
	/* do DNS Request */
	memset(&hints, 0, sizeof hints);
	hints.ai_flags = AI_PASSIVE;

	if (!strcmp(availableMode, "IPv6"))
		hints.ai_family = AF_INET6;
	else if (!strcmp(availableMode, "IPv4"))
		hints.ai_family = AF_INET;
	else if (!strcmp(availableMode, "DualStack"))
		hints.ai_family = AF_UNSPEC;
	else
		printf("%s() availableMode=%s, unknown case!\n", __FUNCTION__, availableMode);
	
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_addr = NULL;
	ret = getaddrinfo(name, "http", &hints, &ifAddrInfo);
	if(0 != ret){
		printf("%s(): getaddrinfo fail!! ret:%d\n", __FUNCTION__, ret);
		return 0;
	}
	he = *ifAddrInfo;
	/* for multiple ip address single hostname */
	if(&he!=NULL){
		while (&he!=NULL)
        	{
			num_of_ip++;
			cwmp_ip_ip2string(&iPAddress_ori, ip_addr);
			cwmp_ip_ip2string(he.ai_addr, ip_addr2);
			if(!strcmp(ip_addr, ip_addr2)){
				switch(iPAddress_ori.sa_family)
				{
					case AF_INET:
						if(strcmp(ip_addr,"0.0.0.0")){
							issameip = 1;
						}
						break;
					case AF_INET6:
						if(strcmp(ip_addr,"::")){
							issameip = 1;
						}
						break;
					default:
						break;
				}
			}
				break;
		}
		he = *ifAddrInfo;
		/*random choose*/
		if( (chostype == eMULTIP_CHOOSE_RANDOM) && (num_of_ip>1)){
	 		srand((unsigned)time(NULL));
			rand_num = rand()%num_of_ip;
			while(rand_num > 0){
				he = *(he.ai_next);
				rand_num--;
			}
	 		bcopy(he.ai_addr, (char *) ip, sizeof(*ip));
			freeaddrinfo(ifAddrInfo);
			return 1;
	 	}
	 	else if( chostype == eMULTIP_CHOOSE_CHECK ){
			if( issameip){
				bcopy(&iPAddress_ori, (char *) ip, sizeof(*ip));
				freeaddrinfo(ifAddrInfo);
				return 1;
			}
		}
		else{
			if (he.ai_addr->sa_family == AF_INET) {
				ip->sa_family = he.ai_addr->sa_family;
				ip->sa_data[0] = he.ai_addr->sa_data[2];
				ip->sa_data[1] = he.ai_addr->sa_data[3];
				ip->sa_data[2] = he.ai_addr->sa_data[4];
				ip->sa_data[3] = he.ai_addr->sa_data[5];
			}
			else if (he.ai_addr->sa_family == AF_INET6) {
				int i;
				ip->sa_family = he.ai_addr->sa_family;
				for (i=0; i<16; ++i)
					ip->sa_data[i] = he.ai_addr->sa_data[i+6];
			}
			freeaddrinfo(ifAddrInfo);
			return 1;
	 	}		
	}
	freeaddrinfo(ifAddrInfo);
	return 0;
}
/*************************
* Function 	: cwmp_parseurl()
* Description	: parse the url to get ip, port and path
* Output 	: 0: succ
			: others : fail
* NOTE		: size of theUrl, theHostName and thePath is SIZE_URL+1
*************************/
int cwmp_parseurl(	const char 	*theUrl,
						char 		*theHostName,
						struct sockaddr	*theHostAddress,
						int		*thePort,
						char			*thePath,
						bool *isSecure, 
						char *hostName, /*hostname instead of ip address*/
						multi_choose_t chostype,
						const char * availableMode) 
{
	int 				theLength;
	portTablePtr_t		theTablePtr;
	char 				theHostipPtr[26];
	struct sockaddr		acsIPAddress_ori;
	struct sockaddr_in	tempV4;
	struct sockaddr_in6	tempV6;
	acsIPAddress_ori = *theHostAddress;
	char tmpUrlPtr[256]={0};
	char *theProtocolPtr = NULL, *theHostPtr = NULL, *thePortPtr = NULL, *thePathPtr = NULL, *theV6Ptr = NULL, *tempPtr = NULL;
	int portFlag = 0, pathFlag = 0;
	char *saveptr1 = NULL, *saveptr2 = NULL, *saveptr3 = NULL;
	//memcpy( tmpUrlPtr, theUrl, strlen(theUrl));
	int lenUrl = 0;
	memcpy(tmpUrlPtr, theUrl, ((lenUrl = strlen(theUrl))>256) ? 256 : lenUrl);
	memset( theHostipPtr, 0, sizeof(theHostipPtr));
	*theHostName = '\0';
	*thePort = kDefaultHttpPort;
	//*thePath = '\0';
	memset(thePath, 0, 256);
	*hostName = '\0';
	/*
		See if this is an absolute pathname.
	*/
	memset(theHostAddress, 0, sizeof (*theHostAddress));
	theProtocolPtr = strtok_r(tmpUrlPtr, ":", &saveptr1);
	if (theProtocolPtr == NULL) {
		return -2;
	}
	
	/*
		Set the default port for the scheme.
	*/
	theTablePtr = getPortTable(theProtocolPtr);

	if (theTablePtr != (portTablePtr_t) 0) {
		*thePort = theTablePtr->defaultPort;
		*isSecure = theTablePtr->isSecure;
	}
	else{
		return -2;
	}


	/*
		Find the end of the host string, find the path
	*/
	portFlag = 0;
	pathFlag = 0;
	theHostPtr = strtok_r(NULL, "/", &saveptr1);
	if( theHostPtr != NULL){
		thePathPtr = strtok_r(NULL, "", &saveptr1);
		if( thePathPtr != NULL){
			//sprintf(thePath, "/%s", thePathPtr);
			snprintf(thePath, 256, "/%s", thePathPtr);
		}
		else {
			strcpy(thePath, kRootPath);
		}
	}
	else{
		strcpy(thePath, kRootPath);
	}

	/*
		Find the end of the host name and port
	*/	
	memcpy(theHostName, theHostPtr, strlen(theHostPtr));
	if(strstr(theHostPtr, "[") != NULL){
		theHostPtr = strtok_r(theHostPtr+1, "]", &saveptr3);
		thePortPtr = strtok_r(NULL, ":", &saveptr3);
		if( thePortPtr != NULL){
			theLength = thePortPtr-theHostPtr;
			*thePort = (int) atoi(thePortPtr);
			strcpy(theHostName, theHostPtr);
		}
		else{
			/*the port is not assigned, provide the default port*/
		}
		inet_pton(AF_INET6, theHostPtr, theHostAddress->sa_data);
		theHostAddress->sa_family = AF_INET6;
	} else {
		theHostPtr = strtok_r(theHostPtr, ":", &saveptr2);
		if( theHostPtr != NULL ){
			thePortPtr = strtok_r(NULL, ":", &saveptr2);
			if( thePortPtr != NULL){
				theLength = thePortPtr-theHostPtr-1;
				*thePort = (int) atoi(thePortPtr);
				strcpy(theHostName, theHostPtr);
			}
		}
		else{
			/*the port is not assigned, provide the default port*/
		}
		/*
			If the host string is an IP address, convert it.
		*/
		if ( isValidIP(theHostName) ){
			inet_pton(AF_INET, theHostName, theHostAddress->sa_data);
			theHostAddress->sa_family = AF_INET;
		}else{
			if(resolveSrvUrlName(theHostName, acsIPAddress_ori,  chostype, theHostAddress, availableMode) == 0)
			{
				printf("%s:  olveSrvUrlName error! url=%s\n", __FUNCTION__, theHostName);
				return -1;
			}
		}
	}
	strcpy(hostName, theHostName);
	return 0;
}

/*************************
* Function      : cwmp_conn_new() from cwmp/src/cwmpconn.c
* Description   : new a connection
* Output        :
*************************/
tr369ConnCB_t *cwmp_conn_new( void ){
        tr369ConnCB_t           *connCbPtr;
        connCbPtr = (tr369ConnCB_t*)malloc(sizeof(tr369ConnCB_t));
        memset(connCbPtr, 0, sizeof(tr369ConnCB_t));
        return connCbPtr;
}

void *cwmp_http_authlist_new(void)//from http/src/cwmphttp.c
{
#ifdef SERISE_AUTH_SUPPORT
        void *authList = (void *)cg_http_auth_storagelist_new();
        return authList;
#else
        return NULL;
#endif
}
/*************************
* Function      : cwmp_conn_set_client() from cwmp/src/cwmpconn.c
* Description   : set a client connection info
* Output        :
*************************/
void cwmp_conn_set_client( tr369ConnCB_t *connCbPtr, servInfo_t *servInfoPtr, char *httpMethod ){
        memcpy( &connCbPtr->servInfo, servInfoPtr, sizeof(connCbPtr->servInfo) );
        connCbPtr->httpMethod = httpMethod;
}
/*************************
* Function      : cwmp_sess_new() from  cwmp/src/cwmpconn.c
* Description   : new a session
* Output        :
*************************/
tr369SessCB_t *cwmp_sess_new( void ){
        tr369SessCB_t           *sessCbPtr;
        sessCbPtr = (tr369SessCB_t*)malloc(sizeof(tr369SessCB_t));
        memset(sessCbPtr, 0, sizeof(tr369SessCB_t));
#if 0
        sessCbPtr->state = eSESS_STATE_CREATE;
#endif
        sessCbPtr->authList = cwmp_http_authlist_new();
        return sessCbPtr;
}
/*************************
* Function      : cwmp_transfer_new() from cwmp/src/cwmpconn.c
* Description   : new a tranfer information
* Output        :
*************************/
transferInfo_t *cwmp_transfer_new( void ){
        transferInfo_t  *transferInfo;
        transferInfo = (transferInfo_t*)malloc(sizeof(transferInfo_t));
        memset(transferInfo, 0, sizeof(transferInfo_t));

        return transferInfo;
}
/*************************
* Function      : cwmp_soapresp_delete() from soap/src/cwmpsoap.c
* Description   : delete soap response
* Output        :
*************************/
void cwmp_soapresp_delete( soapResponse_t *soapRes  ){
        if( soapRes == NULL ){
                return;
        }

        if( soapRes->isHttpResCreated ){
                cg_http_response_delete( soapRes->httpRes );
        }
#if 0
        if( soapRes->rootNodeList !=  NULL){
                cg_xml_nodelist_delete( soapRes->rootNodeList );
        }

        if( soapRes->soapElements != NULL){
                cwmp_soapelement_delete( soapRes->soapElements );
        }

        if( soapRes->rpcElements != NULL ){
                cwmp_rpc_elem_delete( soapRes->rpcElements, soapRes->rpcMethod );
                soapRes->rpcElements = NULL;
        }
#endif
        free( soapRes );
}
/*************************
* Function      : cwmp_soapreq_delete()
* Description   : delete soap request
* Output        :
*************************/
void cwmp_soapreq_delete( soapRequest_t *soapReq ){

        if ( soapReq == NULL ){
                return;
        }

        if( soapReq->isHttpReqCreated ){
                cg_http_request_delete( soapReq->httpReq );
                soapReq->httpReq = NULL;
        }
#if 0
        if( soapReq->rootNodeList !=  NULL){
                cg_xml_nodelist_delete( soapReq->rootNodeList );
                soapReq->rootNodeList = NULL;
        }

        if( soapReq->soapElements != NULL){
                cwmp_soapelement_delete( soapReq->soapElements );
                soapReq->soapElements = NULL;
        }
#endif
        if( soapReq->soapRes != NULL){
                cwmp_soapresp_delete( soapReq->soapRes );
                soapReq->soapRes = NULL;
        }
#if 0
        if( soapReq->rpcElements != NULL ){
                cwmp_rpc_elem_delete( soapReq->rpcElements, soapReq->rpcMethod );
                soapReq->rpcElements = NULL;
        }
#endif
        free( soapReq );
}
/*************************
* Function      : cwmp_conn_remove() from cwmp/src/cwmpconn.c
* Description   : remove a connection from the connection list
* Output        :
*************************/
void cwmp_conn_remove( tr369ConnCB_t *connCbPtr ){

		if( connCbPtr == NULL ){
			printf("%s connCbPtr == NULL!\n", __FUNCTION__);
			return;
        }

        if( connCbPtr->prev != NULL ){
                connCbPtr->prev->next = connCbPtr->next;
        }
        if( connCbPtr->next != NULL ){
                connCbPtr->next->prev = connCbPtr->prev;
        }
        connCbPtr->prev = NULL;
        connCbPtr->next = NULL;
}
/*************************
* Function      : cwmp_soapresp_new() from soap/src/cwmpsoap.c
* Description   : new a soap response
* Output        :
*************************/
soapResponse_t *cwmp_soapresp_new( void ){
        soapResponse_t *soapRes;


        soapRes = (soapResponse_t *)malloc(sizeof(soapResponse_t));
        if ( soapRes == NULL ){
                return NULL;
        }
        memset( soapRes, 0, sizeof(soapResponse_t));

        soapRes->httpRes = cg_http_response_new();
        soapRes->isHttpResCreated = TRUE;
#if 0
        soapRes->rootNodeList = cg_xml_nodelist_new();
        soapRes->soapElements = cwmp_soapelement_new();
#endif
        cg_soap_response_setuserdata( soapRes, NULL );

        return soapRes;
}
/*************************
* Function      : cwmp_soapreq_new()
* Description   : new a soap request
* Output        :
*************************/
soapRequest_t *cwmp_soapreq_new( void ){
        soapRequest_t *soapReq;

        soapReq = (soapRequest_t *)malloc(sizeof(soapRequest_t));
        if( soapReq == NULL ){
                return NULL;
        }
        memset( soapReq, 0, sizeof(soapRequest_t));

        soapReq->soapRes = cwmp_soapresp_new();
        soapReq->httpReq = cg_http_request_new();
        soapReq->isHttpReqCreated = TRUE;
        soapReq->rpcMethod = 0;/*eRPCM_NULL;*/
        soapReq->isCpeCall = FALSE;
#if 0
        soapReq->rootNodeList = cg_xml_nodelist_new();
        soapReq->soapElements = cwmp_soapelement_new();
#endif
        cg_soap_request_setuserdata( soapReq, NULL );
        return soapReq;
}
/*************************
* Function 	: cwmp_upload_build() from rpc/src/cwmprpc.c
* Description	: build the upload request
* Output 	:
*************************/
soapRequest_t *cwmp_upload_build( void *sessCb, servInfo_t *serInfo){
	tr369SessCB_t * theSessCb = (tr369SessCB_t *)sessCb;
	transferInfo_t	*transferInfo = NULL;
	rpcUpload_t	*rpcUpload = NULL;
	soapRequest_t *soapReq = NULL;
	FILE *fp = NULL;
	char *content = NULL;
	char *basicHeader = NULL;
	CgHttpHeader *header = NULL;
	int contentLen =0;
	int rret = 0;
	uint32_t tmId;
	fprintf(stderr, "%s() Jessie Enter\n", __FUNCTION__);
	soapReq = cwmp_soapreq_new();
	if( soapReq == NULL ){
		printf(  "create soap request fail!");
		return NULL;
	}
	/*set content*/
	//contentLen = ftell( fp );
	//content = (char*)malloc(contentLen);
	//memset( content, 0, contentLen );
#if 0//Jessie	
	cg_http_request_setncontent( soapReq->httpReq, content, contentLen);
	cg_http_request_sethost(soapReq->httpReq, "192.168.1.1", 80); // ip and port will be rewrote later
	cg_http_packet_setheadervalue((CgHttpPacket*)soapReq->httpReq, CG_HTTP_USERAGENT, cg_http_request_getuseragent(soapReq->httpReq));

	if (1) {//!(TR369_FEATURE_Flag & TR369_FEATURE_UPLOAD_BY_PUT)

		if (strlen(serInfo->username) || strlen(serInfo->password)) {
			basicHeader = cg_http_request_generate_basicHeader(serInfo->username, serInfo->password);
			/*add header to request*/
			header = cg_http_header_new();
			if (basicHeader && header) {
				cg_http_header_setname(header, CG_HTTP_AUTHENTICATION);
				cg_http_header_setvalue(header, basicHeader);
				cg_http_packet_addheader((CgHttpPacket*)soapReq->httpReq, header);
			}
		}

		cg_http_request_setcontentlength(soapReq->httpReq, contentLen + 59);

		cg_http_request_setcontenttype( soapReq->httpReq, "multipart/form-data; boundary=----------------------------7593072642034666527694302664""\r\n\r\n------------------------------7593072642034666527694302664");

		if(transferInfo->uploadType == eUploadTypeLog){
			cg_http_request_setContentDisposition(soapReq->httpReq, "form-data; name=\"fileupload1\"; filename=\"Log_File\"\r\nContent-Type: application/octet-stream");
		}
		else{
			char scd[256] = {0};
			if (serInfo->path[strlen(serInfo->path)-1] == '/'){
				sprintf(scd, "form-data; name=\"fileupload1\"; filename=\"%s\"\r\nContent-Type: application/octet-stream", "Backup_Restore");

			}else{
				sprintf(scd, "form-data; name=\"fileupload1\"; filename=\"%s\"\r\nContent-Type: application/octet-stream", &serInfo->path[1]);
			}
			cg_http_request_setContentDisposition(soapReq->httpReq, scd);
		}
	}
	else {
		if(transferInfo->uploadType == eUploadTypeLog){
			cg_http_request_setcontenttype( soapReq->httpReq, "text/xml");
		}
		else{
			cg_http_request_setcontenttype( soapReq->httpReq, "application/x-tar");
		}
		cg_http_request_setcontentlength(soapReq->httpReq, contentLen );
	}
#endif
	END:

	if(content != NULL){
		free( content );
	}

	return soapReq;
}
/*************************
* Function      : cwmp_upload_parse() from rpc/src/cwmprpc.c
* Description   : parse the upload response
* Output        :
*************************/
int cwmp_upload_parse( soapResponse_t *soapResp , soapRequest_t *soapReq, transferInfo_t *transferInfo){

        CgHttpResponse *theHttpRes = NULL;
        rpcUpload_t     *rpcUpload = NULL;
        int ret = 0;
printf("%s() Jessie Enter\n", __FUNCTION__);
        /*support GetAllQueuedTransfers, update the queue state*/
        rpcUpload = transferInfo->rpcUpload;

        theHttpRes = cg_soap_response_gethttpresponse(soapResp);

        if(theHttpRes == NULL){
                printf(  "http request fail\n");
                ret = -1;
                goto END;
        }

        if(cg_http_response_getstatuscode(theHttpRes) == 0)
                printf( "no http response status code\n");

        if(cg_http_response_getstatuscode(theHttpRes)!=0 && cg_http_response_issuccessful(theHttpRes) == 0 &&
                cg_http_response_getstatuscode(theHttpRes)!=100 /*100 Continue*/ &&
                cg_http_response_getstatuscode(theHttpRes)!=301 /*301 Moved Permanently*/){
                printf(  "http response not success [status code=%d]\n", cg_http_response_getstatuscode(theHttpRes));
                if(cg_http_response_authfail(theHttpRes)){
                        printf( "TR369_FILE_TRANSFER_SERVER_AUTHENTICATION_FAILURE\n");
                }else
						printf( "TR369_UPLOAD_FAILURE\n");
                ret = -1;
                goto END;
        }

        /*TODO 201 Create*/
#if 0
        if ( cg_http_response_getstatuscode(theHttpRes) != CG_HTTP_STATUS_OK){
                printf(  "http response not success (200)\n");
                printf( "TR369_UPLOAD_FAILURE"\n);
                ret = -1;
                goto END;
        }
#endif

        END:

        return ret;

}
int verify_callback(int ok, X509_STORE_CTX *ctx)
{

        /* Fault Tolerance When Certificate Context Can't Be Verified In Some Situation */
        if (ok == 0)
        {
                #if 0
                TR069LOG( TR069_DEBUG_DB, "ctx->error=%d", ctx->error);
                switch (ctx->error)
                {
                        case X509_V_ERR_UNABLE_TO_GET_ISSUER_CERT_LOCALLY :
                        case X509_V_ERR_UNABLE_TO_VERIFY_LEAF_SIGNATURE :
                        case X509_V_ERR_CERT_UNTRUSTED :
                                ok=1;
                                break;
                        default:
                                break;
                }
                #endif
        }


        return(ok);
}
void cwmp_http_request_set_sslctx( CgHttpRequest *httpReq, servInfo_t *servInfo ){
        Cgctx *c_ctxdata = NULL;
        httpReq->ctxdata = NULL;

        if(!servInfo->isSecure) return;

        c_ctxdata = (Cgctx*) malloc(sizeof(Cgctx));
        memset(c_ctxdata, '\0', sizeof(Cgctx));
#ifdef ZYXEL_USA_PRODUCT
        c_ctxdata->CApath = NULL;
#else
        if(checkCertificate)
                c_ctxdata->CApath = CA_PATH;
        else
                c_ctxdata->CApath = NULL;
#endif
        c_ctxdata->CAfile = NULL;
        c_ctxdata->cert_file = TR69CLIENT_CRT_NAME;
        c_ctxdata->key_file = TR69CLIENT_KEY_NAME;
#ifdef ZYXEL_USA_PRODUCT
        c_ctxdata->verify_mode = SSL_VERIFY_NONE;
#else
        if(checkCertificate)
                c_ctxdata->verify_mode = SSL_VERIFY_PEER;
        else
                c_ctxdata->verify_mode = SSL_VERIFY_NONE;
#endif
        c_ctxdata->verify_callback = verify_callback;
        if(strlen(servInfo->hostname)>0){
#ifdef ZYXEL_USA_PRODUCT
                c_ctxdata->verify_cn = NULL;
#else
                if(checkCertificate && checkCertificateCN)
                        c_ctxdata->verify_cn = servInfo->hostname;
                else
                        c_ctxdata->verify_cn = NULL;
#endif
		}
        httpReq->ctxdata = (void*)c_ctxdata;
}
/*************************
* Function      : cwmp_http_request_setelement() from http/src/cwmphttp.c
* Description   : set the elements of http request
* Output        :
*************************/
void cwmp_http_request_setelement( CgHttpRequest *httpReq, char *method, char *uri, char* userdata ){

        /* http info */

        if(cg_http_request_getcontenttype(httpReq) == NULL)
                cg_http_request_setcontenttype( httpReq, CWMP_SOAP_CONTENT_TYPE );
        else if(strlen(cg_http_request_getcontenttype(httpReq)) == 0)
        cg_http_request_setcontenttype( httpReq, CWMP_SOAP_CONTENT_TYPE );

        cg_http_request_setmethod( httpReq, method);
        cg_http_request_setuserdata( httpReq, userdata );
        cg_http_request_seturi( httpReq, uri );
        /*BBF TR-369:The suggested SessionTimeout MUST be 30 seconds or greater*/
        /*CG_HTTP_CWMP_SEND_RECV_TIMEOUT *CG_HTTP_CWMP_SEND_RECV_TRY_COUNT */
        cg_http_request_settimeout(httpReq,CG_HTTP_CWMP_SEND_RECV_TIMEOUT);
}
#include "skb_defines.h"
#define IPR2_MARK_OFFSET        SKBMARK_SERVICEROUTE_S
#define IPRULE_MASK             SKBMARK_SERVICEROUTE_M
#define MARK_X_ZyXEL_IPR2_MARKING(x)    (x << IPR2_MARK_OFFSET);

int cwmp_get_ifname_mark( char *ifname, uint32_t *MARK){//from cm/src/cwmpcm_func.c
		#if 0
        objIndex_t objIid;
        rdm_MgmtSrv_t *mgmtObj = NULL;
        objIndex_t ipObjIid, pppObjIid;
        rdm_IpIface_t *ipObj = NULL;
        rdm_PppIface_t *pppObj = NULL;
        uint32_t mark = 0, intfMark = 0;

        if(MARK) *MARK = 0;

        IID_INIT(objIid);
        if( zcfgFeObjStructGetWithoutUpdate(RDM_OID_MGMT_SRV, &objIid, (void **)&mgmtObj)!= ZCFG_SUCCESS){
                return USP_ERR_INTERNAL_ERROR;
        }
        if(strlen(mgmtObj->X_ZYXEL_ActiveBoundInterface)==0){
                zcfgFeObjStructFree(mgmtObj);
                return USP_ERR_INTERNAL_ERROR;
        }
        IID_INIT(ipObjIid);
        ipObjIid.level = 1;
        if(sscanf(mgmtObj->X_ZYXEL_ActiveBoundInterface, "IP.Interface.%hhu", ipObjIid.idx)!=1){
                zcfgFeObjStructFree(mgmtObj);
                return USP_ERR_INTERNAL_ERROR;
        }

        /*wait for wan ready*/
        if(zcfgFeObjStructGetWithoutUpdate(RDM_OID_IP_IFACE, &ipObjIid, (void **)&ipObj) == ZCFG_SUCCESS){
                if(strstr(ipObj->LowerLayers, "PPP.Interface") != NULL) {
                        /*PPP Mode*/
                        IID_INIT(pppObjIid);
                        pppObjIid.level = 1;
                        sscanf(ipObj->LowerLayers, "PPP.Interface.%hhu", &pppObjIid.idx[0]);
                        if(zcfgFeObjStructGetWithoutUpdate(RDM_OID_PPP_IFACE, &pppObjIid, (void **)&pppObj) == ZCFG_SUCCESS ) {
                                strcpy(ifname, pppObj->X_ZYXEL_IfName);
                                intfMark = pppObj->X_ZYXEL_IPR2_MARKING;
                                zcfgFeObjStructFree(pppObj);
                        }
                }
                else{
                        strcpy(ifname, ipObj->X_ZYXEL_IfName);
                        intfMark = ipObj->X_ZYXEL_IPR2_MARKING;
                }
                zcfgFeObjStructFree(ipObj);
        }
        if(intfMark > 0){
                mark = MARK_X_ZyXEL_IPR2_MARKING(intfMark);
                if(MARK) *MARK = mark;
        }

        zcfgFeObjStructFree(mgmtObj);
		#endif
        return 0;
}
// Description: delete http message authdata by sock
void cwmp_http_msgauthdata_delete(CgHttpRequest *httpReq)//from http/src/cwmphttp.c
{
#ifdef SERISE_AUTH_SUPPORT
        if(!httpReq) return;

        authStorageList_t *authStorageList = httpReq ? (authStorageList_t *)httpReq->authList : NULL;
        authStorage_t *authStorage = cg_http_auth_storagelist_getbysock(authStorageList, httpReq->sock);
        if(authStorage){
                cg_http_auth_storagelist_remove(authStorageList, authStorage);
                cg_http_auth_storagelist_delete(authStorage);
        }
#endif
}
/*************************
* Function      : cwmp_http_request_post() from http/src/cwmphttp.c
* Description   : send http request
* Output        :
*************************/
/*reference fo cg_http_request_post but keep the socket*/
CgHttpResponse *cwmp_http_request_post(CgHttpRequest *httpReq, char *hostName, char *ipaddr, int port, bool isSecure, char *savedFile)
{
        CgSocket *sock = NULL;
        char *method, *uri, *version;
        CgString *firstLine;
        CgHttpResponse *httpRes = NULL;
        char *content = NULL;
        char boundIfName[32] = "";
        uint32_t MARK = 0;
        int msglen = 0;
        struct json_object *featureobj = NULL;
        struct json_object *devinfoobj = NULL;
		#if 0
        objIndex_t featureIid = {0};
        objIndex_t devinfoIid = {0};
		#endif
        const char *user_agent = NULL;
        const char *firmware_version = NULL;
        const char *product_class = NULL;
        char useragent[256] = {0};


        cg_http_response_clear(httpReq->httpRes);
        cg_http_request_print(httpReq);
        if(strcmp(cg_http_request_getmethod(httpReq), "POST")==0){
                content = cg_http_request_getcontent(httpReq);
				printf("http request:\n%s\n", content);
        }

        /* create new socket or used the exist socket */
        if( httpReq->sock == NULL ){
#if defined(CG_USE_OPENSSL)
                if (isSecure == FALSE)
                        sock = cg_socket_stream_new();
                else{
                        sock = cg_socket_ssl_new();
                        cg_socket_setctx(sock, httpReq->ctxdata);
                }
#else
                sock = cg_socket_stream_new();
#endif
                cg_socket_setremoteaddress(sock, ipaddr);
                cwmp_get_ifname_mark(boundIfName, &MARK);
                // 20160701 Max add
                if (tr069_cg_socket_connect_MARK(sock, ipaddr, port, boundIfName, MARK) == FALSE) {

                        tr069_cg_socket_delete(sock);
                        printf(  "cg_socket_connect fail, ipaddr=%s, port=%d\n", ipaddr, port);

                        cg_http_response_setstatuscode(httpReq->httpRes, CG_HTTP_STATUS_CONNECTION_TIMEDOUT);

                        return httpReq->httpRes;
                }
                // send and receive timeout must set after connect to let it work
                cg_socket_settimeout(sock, cg_http_request_gettimeout(httpReq));
                httpReq->sock = sock;
        }
        else{
                sock = httpReq->sock;
        }

        cg_http_request_sethost(httpReq, hostName, port);
        if(strlen(useragent) != 0){
                cg_http_packet_setheadervalue((CgHttpPacket*)httpReq, CG_HTTP_USERAGENT, useragent);
        }
        else{
                cg_http_packet_setheadervalue((CgHttpPacket*)httpReq, CG_HTTP_USERAGENT, cg_http_request_getuseragent(httpReq));
        }

        method = cg_http_request_getmethod(httpReq);
        uri = cg_http_request_geturi(httpReq);
        version = cg_http_request_getversion(httpReq);

        if (method == NULL || uri == NULL || version == NULL) {
                cwmp_http_msgauthdata_delete(httpReq);
                cwmp_http_sock_close(sock);
                httpReq->sock = NULL;
                printf(  "method == NULL || uri == NULL || version == NULL\n");
                return httpReq->httpRes;
        }

        firstLine = cg_string_new();
        cg_string_addvalue(firstLine, method);
        cg_string_addvalue(firstLine, CG_HTTP_SP);
        cg_string_addvalue(firstLine, uri);
        cg_string_addvalue(firstLine, CG_HTTP_SP);
        cg_string_addvalue(firstLine, version);
        cg_string_addvalue(firstLine, CG_HTTP_CRLF);
        tr069_cg_socket_write(sock, cg_string_getvalue(firstLine), cg_string_length(firstLine)); // 20160519 Max add
        proto_flush(sock);
        cg_string_delete(firstLine);
        /**** send header and content ****/
        tr069_cg_http_packet_post((CgHttpPacket *)httpReq, sock); // 20160519 Max add

        /**** read response ****/
        if(cg_http_response_read(httpReq->httpRes, sock, cg_http_request_isheadrequest(httpReq), savedFile) <= 0){
                printf(  "cg_http_response_read fail\n");
                cwmp_http_msgauthdata_delete(httpReq);
                cwmp_http_sock_close(sock);
                httpReq->sock = NULL;

                // transmission not complete
                cg_http_response_setstatuscode(httpReq->httpRes, CG_HTTP_STATUS_NO_RESPONSE);

                return httpReq->httpRes;
        }

        printf("%s: http response read\n", __FUNCTION__);

        /*->[Feature] Session Persistence persistent
        The purpose of this test is to shwo that the ACS and CPE system can maintain a persistent session across multiple TCP connections
        */
        if((cg_strcaseeq(cg_http_packet_getconnection((CgHttpPacket*)httpReq->httpRes), CG_HTTP_CLOSE) == TRUE)){
                printf(  "packet CG_HTTP_CLOSE\n");
                cwmp_http_msgauthdata_delete(httpReq);
                cwmp_http_sock_close(sock);
                httpReq->sock = NULL;
        }
        /*<-[Feature] Session Persistence persistent
        The purpose of this test is to shwo that the ACS and CPE system can maintain a persistent session across multiple TCP connections
        */

        cg_http_response_print(httpReq->httpRes);

        if(content && content[0] != '\0')
                msglen = httpReq->content->valueSize;

        if(strcmp(cg_http_request_getmethod(httpReq), "POST")==0){
                httpRes = httpReq->httpRes;
                content = cg_http_response_getcontent(httpRes);
        }

        return httpReq->httpRes;
}
/*************************
* Function      : cwmp_http_request_set_cookieset() from http/src/cwmphttp.c
* Description   : set the http request cookieset
* Output        :
*************************/
void cwmp_http_request_set_cookieset( CgHttpRequest *httpReq, cookieInfo_t *cookieinfo ){
        char cookie[STRSIZE(SIZE_COOKIE)]={0};
        int index;
        if( httpReq == NULL ){
                return;
        }
        if( cookieinfo == NULL ){
                return;
        }
        printf(  "httpCookieCount %d\n", cookieinfo->httpCookieCount);
        if( cookieinfo->httpCookieCount == 0){
                return;
        }
        /*choose the cookie*/
        /*support multiple header value, ZyXEL 2013*/
        for(index=0; index<cookieinfo->httpCookieCount; index++){
                if( strlen(cookieinfo->cookiedata[index].name)!=0 && strlen(cookieinfo->cookiedata[index].value)!=0 ){
                        snprintf(cookie, sizeof(cookie), "%s=%s", cookieinfo->cookiedata[index].name, cookieinfo->cookiedata[index].value);
                        cg_http_request_setcookie_multi( httpReq, cookie);
                }
        }
}
/*************************
* Function      : cwmp_http_response_get_cookieset() from http/src/cwmphttp.c
* Description   : get the http response cookieset
* Output        :
*************************/
bool cwmp_http_response_get_cookieset( CgHttpResponse *httpRes, cookieInfo_t *cookieinfo ){

        CgHttpHeader *header;
        char *cookie = NULL;
        bool ret = FALSE;
        int index = 0;

        if( httpRes == NULL ){
                return FALSE;
        }
        if(!cookieinfo) {
                return FALSE;
        }
        memset(cookieinfo, 0, sizeof(cookieInfo_t));
        /*User agents that
        receive in the same response both a Set-Cookie and Set-Cookie2
        response header for the same cookie MUST discard the Set-Cookie
        information and use only the Set-Cookie2 information.*/

        /*set-cookie2*/
        /*support multiple header value, ZyXEL 2013*/
        header = httpRes->headerList;
        while((header = cg_http_headerlist_get(header, CG_HTTP_SETCOOKIE2))) {
                int cookieNameLen = 0;
                printf(  "httpRes has SETCOOKIE2\n");
                cookie = NULL;
                if((cookie = cg_http_header_getvalue(header)) && (index < MAX_NUM_COOKIES)) {
                        printf(  "cookie data: %s\n", cookie);
                        cg_http_cookie_header_parse(cookie, &cookieinfo->cookiedata[index]);
                        cookieNameLen = strlen(cookieinfo->cookiedata[index].name);

                        if(cookieNameLen > 0)
                                index++;
                        else
                                memset(&cookieinfo->cookiedata[index], 0, sizeof(CgHttpCookieData));
                }
        }

        header = httpRes->headerList;
        while((header = cg_http_headerlist_get(header, CG_HTTP_SETCOOKIE))) {
                int cookieNameLen = 0;
                printf(  "httpRes has SETCOOKIE\n");
                cookie = NULL;
                if((cookie = cg_http_header_getvalue(header)) && (index < MAX_NUM_COOKIES)) {
                        printf(  "cookie data: %s\n", cookie);
                        cg_http_cookie_header_parse(cookie, &cookieinfo->cookiedata[index]);
                        cookieNameLen = strlen(cookieinfo->cookiedata[index].name);

                        if(cookieNameLen > 0)
                                index++;
                        else
                                memset(&cookieinfo->cookiedata[index], 0, sizeof(CgHttpCookieData));
                }
        }
        cookieinfo->httpCookieCount = index;

        return (cookieinfo->httpCookieCount) ? TRUE : FALSE;
}
/*************************
* Function      : cwmp_http_request_post_with_auth() from http/src/cwmphttp.c
* Description   : send http request with authentication
* Output        :
*************************/
/*reference fo cg_http_request_post_with_auth but keep the socket*/
CgHttpResponse *cwmp_http_request_post_with_auth(CgHttpRequest *httpReq, char *hostName, char *ipaddr, int port, char *user, char *passwd, bool isSecure, char *savedFile)
{
        CgHttpResponse *httpRes;
        CgHttpResponse *httpResAuthTmp;
        char *authHeader;
        CgHttpHeader *header = NULL;
        char *basicHeader, *digestHeader;
        char *staleFlag;
        CgHttpAuth *auth;
        CgHttpHeaderList *bkAuthHeaderList = NULL;
        authStorage_t *authStorage = NULL;
        int isNewHttpRes = 0;
        cookieInfo_t cookieinfo, *httpResCookie = NULL;
        bool hasCookie = 0;

#if SERISE_AUTH_SUPPORT
        authStorageList_t *authStorageList = (authStorageList_t *)httpReq->authList;
        /*support serise authentication, 201402*/

        if( (authStorage = cg_http_auth_storagelist_getbyaddr(authStorageList, ipaddr, port)) != NULL){
                //assign the previous authentication data
                httpResAuthTmp = cg_http_response_new();
                httpResAuthTmp->headerList = authStorage->authHeaderList;
                //remove the previous authentication data from storage list
                authStorage->authHeaderList = NULL;
                cg_http_auth_storagelist_remove( authStorageList, authStorage);
                cg_http_auth_storage_delete(authStorage);
                authStorage = NULL;
                isNewHttpRes = 1;
                httpRes = httpResAuthTmp ;
                goto ACTIVE_SERIES_AUTH;
        }
#endif

        /*post http request*/
        httpRes = cwmp_http_request_post(httpReq, hostName, ipaddr, port, isSecure, savedFile);

        /*check if the query web page needs authentication*/
        if ((cg_http_response_getstatuscode(httpRes)) != CG_HTTP_STATUS_UNAUTHORIZED)
                return httpRes;
#if SERISE_AUTH_SUPPORT
        else if ((cg_http_response_getstatuscode(httpRes)) == CG_HTTP_STATUS_UNAUTHORIZED) {
                cwmp_http_sock_close(httpReq->sock);
                httpReq->sock = NULL;
        }
#endif
        cg_http_auth_header_parse((CgHttpPacket*)httpRes);
        ACTIVE_SERIES_AUTH:
        /*get authentication header*/
        auth = &httpRes->authData;
        authHeader = cg_http_response_getauth(httpRes);
        httpResCookie = (cookieInfo_t *)malloc(sizeof(cookieInfo_t));
        if((hasCookie = cwmp_http_response_get_cookieset(httpRes, &cookieinfo))){
                // there's cookie specified on http auth message
                printf( "http auth message has cookie\n");
                memcpy(httpResCookie, &cookieinfo, sizeof(cookieInfo_t));
                cwmp_http_request_set_cookieset(httpReq, &cookieinfo);
				        }
#if SERISE_AUTH_SUPPORT
        /*support serise authentication, 201402*/
        /*backup authentication data  to storage list*/
        if((strcmp(authHeader, "Basic") == 0) || (strcmp(authHeader, "basic") == 0) || (strcmp(authHeader, "Digest") == 0) || (strcmp(authHeader, "digest") == 0)) {
                bkAuthHeaderList = cg_http_response_auth_header_backup(httpRes);
                if( (authStorage = cg_http_auth_storagelist_getbyaddr(authStorageList, ipaddr, port))!=NULL){
                        cg_http_auth_storagelist_remove( authStorageList, authStorage);
                        cg_http_auth_storage_delete(authStorage);
                }
                authStorage = cg_http_auth_storage_new();
                strcpy(authStorage->ipaddr, ipaddr);
                authStorage->port = port;
                authStorage->sock = httpReq->sock;
                authStorage->authHeaderList = bkAuthHeaderList;
                if(cg_http_auth_storagelist_add( authStorageList, authStorage)!=0)
                        cg_http_auth_storage_delete( authStorage );
        }
#endif
        if ((strcmp(authHeader, "Basic") == 0) || (strcmp(authHeader, "basic") == 0))  /*basic authentication*/
        {

                basicHeader = cg_http_request_generate_basicHeader(user, passwd);
                if(isNewHttpRes){
                        cg_http_response_clear(httpResAuthTmp);
                        cg_http_response_delete(httpResAuthTmp);
                        httpResAuthTmp = NULL;
                }

                /*add header to request*/
                header = cg_http_header_new();
                cg_http_header_setname(header, CG_HTTP_AUTHENTICATION);
                cg_http_header_setvalue(header, basicHeader);
                cg_http_packet_addheader((CgHttpPacket*)httpReq, header);
                httpRes = cwmp_http_request_post(httpReq, hostName, ipaddr, port, isSecure, savedFile);
                if(httpRes == NULL)
                        printf(  "http response fail\n");

                /*clean up*/
                free(basicHeader);

                auth = &httpRes->authData;

                /*authentication success*/
                if((cg_http_response_getstatuscode(httpRes)) == CG_HTTP_STATUS_OK){
                        auth->result = AUTH_OK;
                }
                /*authentication fail if the 401 response is presented*/
                if((cg_http_response_getstatuscode(httpRes)) == CG_HTTP_STATUS_UNAUTHORIZED)
                        auth->result = AUTH_WRONG_INPUT;

                goto complete;
        }
        else if ((strcmp(authHeader, "Digest") == 0) || (strcmp(authHeader, "digest") == 0))   /*digest authentication*/
        {

                digestHeader = cg_http_request_generate_digestHeader(httpReq, httpRes, user, passwd);
                if(isNewHttpRes){
                        cg_http_response_clear(httpResAuthTmp);
                        cg_http_response_delete(httpResAuthTmp);
                        httpResAuthTmp = NULL;
                }

                /*add header to digest request*/
               header = cg_http_header_new();
                cg_http_header_setname(header, CG_HTTP_AUTHENTICATION);
                cg_http_header_setvalue(header, digestHeader);
                cg_http_packet_addheader((CgHttpPacket*)httpReq, header);
                httpRes = cwmp_http_request_post(httpReq, hostName, ipaddr, port, isSecure, savedFile);
                if(httpRes == NULL)
                        printf(  "http response fail\n");

                free(digestHeader);

                auth = &httpRes->authData;

                /*authentication success*/
                if((cg_http_response_getstatuscode(httpRes)) == CG_HTTP_STATUS_OK){
                        auth->result = AUTH_OK;
                        goto complete;
                }
                /*authentication fail if the 401 response is presented:
                        it presents that the username and password is correct,
                        but the nonce value is old when the http response carries stale header, and the value is "TRUE".

                        otherwise, the username or password is wrong.
                */
                if((cg_http_response_getstatuscode(httpRes)) == CG_HTTP_STATUS_UNAUTHORIZED){
                        cg_http_auth_header_parse((CgHttpPacket*)httpRes);
                        staleFlag = cg_http_response_getstale(httpRes);
                        if(staleFlag && strcasecmp(staleFlag, "TRUE") == 0){
                                auth->result = AUTH_STALE;
                                goto complete;
                        }
                        else{
                                auth->result = AUTH_WRONG_INPUT;
                                goto complete;
                        }
                }
                else
                        goto complete;
        }
        /*the authentication way neither Basic nor Digest*/
        else{
                auth->result = AUTH_UNSUPPORT_AUTH;
                goto complete;
        }

complete:
        if(hasCookie)
                cg_http_response_setcookiedata(httpRes, httpResCookie);
        else {
                hasCookie = (auth->result == AUTH_OK) ? cwmp_http_response_get_cookieset(httpRes, &cookieinfo) : 0;
                if(hasCookie){
                        memcpy(httpResCookie, &cookieinfo, sizeof(cookieInfo_t));
                        cg_http_response_setcookiedata(httpRes, httpResCookie);
                }
        }

        if(!hasCookie && httpResCookie)
                free(httpResCookie);

        return httpRes;
}
/*************************
* Function      : cwmp_soapresp_sethttpresp() from soap/src/cwmpsoap.c
* Description   : assign http response
* Output        :
*************************/
void cwmp_soapresp_sethttpresp(soapResponse_t *soapRes, CgHttpResponse *httpRes)
{

        if (soapRes->isHttpResCreated == TRUE){
                cg_http_response_delete(soapRes->httpRes);
        }
        soapRes->httpRes = httpRes;
        soapRes->isHttpResCreated = FALSE;

}
/*************************
* Function      : cwmp_soapreq_post() from soap/src/cwmpsoap.c
* Description   : post soap request
* Output        :
*************************/
soapResponse_t *cwmp_soapreq_post( soapRequest_t *soapReq, servInfo_t *servInfo, char   *savedFile, lwm2m_context_t *contextP){//Jessie add lwm2m_context_t??
        CgHttpRequest *httpReq;
        CgHttpResponse *httpRes;
        CgHttpHeader *header = NULL;
        char *redirectUrl = NULL;
        char thehostname[STRSIZE(SIZE_URL)]={0};
        char ip_addr[INET6_ADDRSTRLEN+1] = {0};

        //support host name of Host of http header
        cwmp_ip_ip2string(&(servInfo->ipAddress), ip_addr);
		httpRes = cwmp_http_request_post_with_auth(soapReq->httpReq, servInfo->hostname, ip_addr, servInfo->port, servInfo->username, servInfo->password, servInfo->isSecure, savedFile);


        printf(  "http response status code=%d\n", cg_http_response_getstatuscode(httpRes));
        /* Check for HTTP response 405 Method Not Allowed */
        /* BBR TR-369 3.4.1
        An HTTP response that contains any CPE WAN Management Protocol payload
        (a SOAP request to the CPE, a successful SOAP response to the CPE, or a SOAP
        fault response containing a Fault element defined in Section 3.5) MUST use the
        HTTP status code 200 (OK).*/
        /*TODO*/
        if (cg_http_response_getstatuscode(httpRes) == CG_HTTP_STATUS_METHOD_NOT_ALLOWED)
        {
                /* Status code implies that we need to use M-POST */
                cg_http_request_setmethod(soapReq->httpReq, CG_HTTP_MPOST);

                /* Add MAN header */
                header = cg_http_header_new();
                cg_http_header_setname(header, CG_HTTP_MAN);
                cg_http_header_setvalue(header, CG_HTTP_SOAP_MAN_VALUE);
                cg_http_packet_addheader((CgHttpPacket*)soapReq->httpReq, header);

                /* Change soapaction header name to include namespace */
                header = cg_http_packet_getheader((CgHttpPacket*)soapReq->httpReq,
                                                  CG_HTTP_SOAP_ACTION);
                if (header != NULL)
                {
                        cg_http_header_setname(header, CG_HTTP_SOAP_ACTION_WITH_NS);
                }
                /* New attempt */
                httpRes = cwmp_http_request_post_with_auth(soapReq->httpReq, servInfo->hostname, ip_addr, servInfo->port, servInfo->username, servInfo->password, servInfo->isSecure, savedFile);
        }
        /*->[Feature] A CPE MUST support the 302 (Found) and 307 (Temporary Redirect) HTTP status codes*/
        else if(cg_http_response_getstatuscode(httpRes) == CG_HTTP_STATUS_FOUND ||
                cg_http_response_getstatuscode(httpRes) == CG_HTTP_STATUS_TEMPORARY_REDIRECT )
        {
                do{
                        printf(  "CG_HTTP_STATUS_FOUND or CG_HTTP_STATUS_TEMPORARY_REDIRECT\n");
                        redirectUrl = cg_http_packet_getheadervalue((CgHttpPacket*)httpRes,CG_HTTP_LOCATION);
                        printf(  "close the current socket, redirectUrl=%s\n", redirectUrl);
                        /* close the current socket */
                        httpReq = soapReq->httpReq;
                        cwmp_http_msgauthdata_delete(httpReq);
                        cwmp_http_sock_close(httpReq->sock);
                        httpReq->sock = NULL;

                        memset( &redirectServInfo, 0, sizeof(redirectServInfo) );
                        strncpy(redirectServInfo.url, redirectUrl, SIZE_URL);
                        if (cwmp_parseurl( redirectUrl, thehostname, (struct sockaddr *)&(redirectServInfo.ipAddress), &redirectServInfo.port, redirectServInfo.path, &redirectServInfo.isSecure, redirectServInfo.hostname, eMULTIP_CHOOSE_REGULAR, "IPv4"/*, gTR369CB.acsInfo.X_ZYXEL_CurrentMode*/ ) == 0) {
                                strcpy(redirectServInfo.username, servInfo->username);
                                strcpy(redirectServInfo.password, servInfo->password);
                                /* New attempt */
                                cwmp_ip_ip2string(&(redirectServInfo.ipAddress), ip_addr);
                                memcpy(servInfo, &redirectServInfo, sizeof(servInfo_t));
                                if (strcmp(cg_http_request_getmethod(soapReq->httpReq), CG_HTTP_POST) == 0) {
                                        cg_string_setvalue(soapReq->httpReq->uri, servInfo->path);
                                }
                                httpRes = cwmp_http_request_post_with_auth(soapReq->httpReq, servInfo->hostname, ip_addr, redirectServInfo.port, redirectServInfo.username, redirectServInfo.password, redirectServInfo.isSecure, savedFile);
                        }
                }
                while(cg_http_response_getstatuscode(httpRes) == CG_HTTP_STATUS_FOUND ||
                cg_http_response_getstatuscode(httpRes) == CG_HTTP_STATUS_TEMPORARY_REDIRECT );
        }
        /*<-[Feature] A CPE MUST support the 302 (Found) and 307 (Temporary Redirect) HTTP status codes*/
#if 1//Jessie hard code test
		if (cg_http_response_getstatuscode(httpRes) == 201)
        {
			#if 1
			//prv_handleHttpRegistrationReply(contextP, transacP, message);//Jessie hard code test
			//servInfo->status = STATE_REGISTERED;
			contextP->serverList->status = STATE_REGISTERED;
			printf(  "contextP->serverList->status = STATE_REGISTERED;\n");
			#if 0
            if (NULL != servInfo->location)
            {
                lwm2m_free(servInfo->location);
            }
			#endif
			#endif
		} else {
			contextP->serverList->status = STATE_REG_PENDING;//Jessie move from http_prv_register in registration_http.c (send the registration for a single server)
			printf(  "contextP->serverList->status = STATE_REG_PENDING;\n");
		}
#endif
        cwmp_soapresp_sethttpresp(soapReq->soapRes, httpRes);

        return soapReq->soapRes;
}
void cwmp_http_request_free_sslctx( CgHttpRequest *httpReq ){
        Cgctx *c_ctxdata = NULL;
        c_ctxdata = (Cgctx *)httpReq->ctxdata;
        if(c_ctxdata != NULL)
                free(c_ctxdata);
        httpReq->ctxdata = NULL;
}
/*************************
* Function      : cwmp_send_request_downup() from cwmp/src/cwmpconn.c
* Description   : send a http download/upload request
* Output        :
*************************/
int cwmp_send_request_downup( tr369ConnCB_t *connCbPtr, lwm2m_context_t * contextP ){//add lwm2m_context_t * contextP
        tr369SessCB_t *theSessCb = NULL;
        soapRequest_t *theSoapReq = NULL;
        CgHttpRequest *theHttpReq = NULL;
        transferInfo_t  *transferInfo = NULL;
        rpcDownload_t   *rpcDownload = NULL;
        char tmpFileName[STRSIZE(SIZE_URL)] = {0};
        char buf[STRSIZE(SIZE_URL)] = {0};
        char *ptr = NULL;
		char uri[STRSIZE(SIZE_URL)];

		#if 1//Jessie http
		uint8_t * payload;
		int payload_length;
		payload_length = object_getRegisterPayloadBufferLength(contextP);
		if(payload_length == 0) return COAP_500_INTERNAL_SERVER_ERROR;
		payload = (uint8_t*) lwm2m_malloc(payload_length);
		if(!payload) return COAP_500_INTERNAL_SERVER_ERROR;
		payload_length = object_getRegisterPayload(contextP, payload, payload_length);
		if(payload_length == 0)
		{
			lwm2m_free(payload);
			return COAP_500_INTERNAL_SERVER_ERROR;
		}
		#endif
        if( connCbPtr == NULL ){
                printf(  "connCbPtr == NULL\n");
                return -1;
        }
        theSessCb = connCbPtr->sessionCb;
        if( theSessCb == NULL ){
                printf(  "theSessCb == NULL\n");
                return -1;
        }

        theSoapReq = theSessCb->soapReq;
        theHttpReq = theSoapReq->httpReq;
        theHttpReq->authList = theSessCb->authList;
		//set content
		printf(  "%s() Jessie payload=%s,payload_length=%d\n", __FUNCTION__, payload, payload_length);
		cg_http_request_setcontentlength(theHttpReq, payload_length);
		cg_http_request_setcontent(theHttpReq, payload);
		#if 1//Jessie http to a function later
		// /rd?ep={Endpoint Client Name}&lt={Lifetime}&lwm2m={version}&b={binding}
		memset(uri, 0, sizeof(uri));
		strcpy(uri, "/rd");
		strcat(uri, "?");
		strcat(uri, "ep=");
		strcat(uri, contextP->endpointName);
		strcat(uri, "&lt=");
		strcat(uri, "300");//default lifetime
		strcat(uri, "&lwm2m=");
		strcat(uri, "1.0");//default version
		strcat(uri, "&b=");
		strcat(uri, "H");//default TCP
		#endif
		
		cg_http_request_setcontenttype(theHttpReq, "application/link-format");
        /*assign transferinfo */
        transferInfo = theSessCb->transferInfo;
        if(transferInfo == NULL) return -1;

        cg_http_request_settimeout(theHttpReq, CG_HTTP_CWMP_SEND_RECV_TIMEOUT);

        if (strcmp(CG_HTTP_POST, connCbPtr->httpMethod) == 0) {
                char fpath[256] = {0};
                char *p1 = NULL, *p2 = NULL;
                strcpy(fpath, connCbPtr->servInfo.path);
                        p1 = strstr(fpath, "/");
                while (p1 != NULL) {
                        p2 = p1;
                        p1 = strstr(&p1[1], "/");
                }
                if (p2)
                        *p2 = 0;
                cwmp_http_request_setelement( theHttpReq, connCbPtr->httpMethod, fpath, theSoapReq->userData);
        }
        else
        cwmp_http_request_setelement( theHttpReq, connCbPtr->httpMethod, connCbPtr->servInfo.path, theSoapReq->userData);
		
		cg_http_request_seturi(theHttpReq, uri);//Jessie http
		
        cwmp_http_request_set_sslctx( theHttpReq, &connCbPtr->servInfo);
        if(transferInfo->rpcDownload){
			theSessCb->soapResp = cwmp_soapreq_post(theSoapReq, &connCbPtr->servInfo, transferInfo->localFilePath, contextP);
        }
        else
                theSessCb->soapResp = cwmp_soapreq_post(theSoapReq, &connCbPtr->servInfo, NULL, contextP);
		#if 0//Jessie remove temporarily for test socket close initiated by client
        cwmp_http_request_free_sslctx( theHttpReq);
        /*socket close*/
        if( theHttpReq->sock != NULL ){
                cwmp_http_msgauthdata_delete(theHttpReq);
                cwmp_http_sock_close(theHttpReq->sock);
                theHttpReq->sock = NULL;
        }
		#endif
/*404 Not found*/

        return 0;
}
/*************************
* Function 	: cwmp_sess_file_create_upload() from cwmp/src/cwmpfsm.c
* Description	: create a file upload request
* Output 	:
*************************/
int cwmp_sess_file_create_upload( tr369ConnCB_t *connCbPtr, lwm2m_context_t * contextP ){//add lwm2m_context_t * contextP

	tr369SessCB_t		*theSessCb = NULL; 		/* session control block */
printf("%s() Jessie Enter\n", __FUNCTION__);
	if( connCbPtr == NULL ){
		printf(  "connCbPtr == NULL\n");
		return -1;
	}
	theSessCb = connCbPtr->sessionCb;
	if( theSessCb == NULL ){
		printf(  "theSessCb == NULL\n");
		return -1;
	}
	theSessCb->soapReq = cwmp_upload_build( theSessCb, &connCbPtr->servInfo);
	if( theSessCb->soapReq == NULL ){
		printf(  "send request fail!\n");
		return -1;
	}

	if( cwmp_send_request_downup( connCbPtr, contextP )!= 0 ){//add contextP
		printf(  "send request fail!\n");
		return -1;
	}

	if( cwmp_upload_parse( theSessCb->soapResp, theSessCb->soapReq, theSessCb->transferInfo) != 0){
		printf(  "Upload parse fail!\n");
		return -1;
	}

	return 0;
}
// Description: clear http message authdata list
void cwmp_http_authlist_clear(void *authList)//from http/src/cwmphttp.c
{
#ifdef SERISE_AUTH_SUPPORT
        authStorageList_t *authStorageList = (authStorageList_t *)authList;
        cg_http_auth_storagelist_delete(authStorageList);
#endif
}
/*************************
* Function      : cwmp_http_sock_close() from http/src/cwmphttp.c
* Description   : close the socket
* Output        :
*************************/
void cwmp_http_sock_close( CgSocket *theSocket){
        authStorage_t *theStorage = NULL;
        if( theSocket == NULL ){
                return;
        }

        tr069_cg_socket_close( theSocket );
        tr069_cg_socket_delete( theSocket );
}
/*************************
* Function      : cwmp_transfer_delete() from cwmp/src/cwmpconn.c
* Description   : delete a transfer information
* Output        :
*************************/
void cwmp_transfer_delete( transferInfo_t *transferInfo ){
        if( transferInfo == NULL){
                return;
        }

        if( transferInfo->rpcDownload != NULL ){
                free( transferInfo->rpcDownload );
                transferInfo->rpcDownload = NULL;
        }

        if( transferInfo->rpcUpload != NULL ){
                free( transferInfo->rpcUpload );
                transferInfo->rpcUpload = NULL;
        }

        free( transferInfo );
}
/*************************
* Function      : cwmp_sess_delete() from cwmp/src/cwmpconn.c
* Description   : delete a session
* Output        :
*************************/
void cwmp_sess_delete( tr369SessCB_t *sessCbPtr ){
        if( sessCbPtr == NULL){
                return;
        }

        cwmp_http_sock_close( sessCbPtr->sock );

        cwmp_http_authlist_clear(sessCbPtr->authList);
#if 0
        if( sessCbPtr->cookieinfo != NULL ){
                free(sessCbPtr->cookieinfo);
                sessCbPtr->cookieinfo = NULL;
        }
#endif
        if( sessCbPtr->transferInfo != NULL ){
                cwmp_transfer_delete( sessCbPtr->transferInfo );
                sessCbPtr->transferInfo = NULL;
        }

        /* just a link from soapReq , so don't need to release soapResp*/
        /*
        if( sessCbPtr->soapResp != NULL ){
                cwmp_soapresp_delete( sessCbPtr->soapResp );
                sessCbPtr->soapResp = NULL;
        }*/

        if( sessCbPtr->soapReq != NULL ){
                cwmp_soapreq_delete( sessCbPtr->soapReq );
                sessCbPtr->soapReq  = NULL;
        }
        if( sessCbPtr->prevSoapReq != NULL ){
                cwmp_soapreq_delete( sessCbPtr->prevSoapReq );
                sessCbPtr->prevSoapReq  = NULL;
        }

        free( sessCbPtr );
}
/*************************
* Function      : cwmp_conn_delete() from cwmp/src/cwmpconn.c
* Description   : delete a connection
* Output        :
*************************/
void cwmp_conn_delete( tr369ConnCB_t *connCbPtr ){
		tr369ConnCB_t *tmpconnCbPtr = NULL;

        if( connCbPtr == NULL ){
			printf("%s connCbPtr == NULL!\n", __FUNCTION__);
			return;
        }

        while( connCbPtr->next ){
                tmpconnCbPtr = connCbPtr->next;
                cwmp_conn_remove( tmpconnCbPtr );
                cwmp_conn_delete( tmpconnCbPtr );
        }
#if 0
        if( connCbPtr->cookieInfo != NULL ){
                free( connCbPtr->cookieInfo );
                connCbPtr->cookieInfo = NULL;
        }
#endif
        if( connCbPtr->httpServer != NULL ){
                cg_http_server_delete( connCbPtr->httpServer );
                connCbPtr->httpServer = NULL;
        }
        if( connCbPtr->sessionCb != NULL ){
                cwmp_sess_delete( connCbPtr->sessionCb );
                 connCbPtr->sessionCb = NULL;
        }
        free(connCbPtr);
}
/*************************
* Function 	: cwmp_sess_file_proc() from cwmp\src\cwmpfsm.c
* Description	: process the fill session
* Output 	:
*************************/
int cwmp_sess_file_proc( tr369ConnCB_t *theConnCb, lwm2m_context_t * contextP ){//add lwm2m_context_t * contextP
	if(theConnCb){
		if( cwmp_sess_file_create_upload(theConnCb, contextP) == 0 ){//add contextP
			printf("cwmp_sess_file_create_upload OK\n");
		} else {
			printf("cwmp_sess_file_create_upload Fail\n");
			return -1;
		}
	} else {
		printf("%s theConnCb == NULL!\n", __FUNCTION__);
		return -1;
	}

	cwmp_conn_remove( theConnCb );
	cwmp_conn_delete( theConnCb );

	return 0;
}

BOOL zHttpCustomrequest_post(CgNetURL *url,  char *ipaddr, int port, char *requestpath, char *fileStorePath)
{

	CgHttpRequest *httpReq;
	CgHttpResponse *httpRes;

	if (port <= 0)
		port = CG_HTTP_DEFAULT_PORT;

	httpReq = cg_http_request_new();
	cg_http_request_setmethod(httpReq, CG_HTTP_GET);
	cg_http_request_seturi(httpReq, requestpath);
	cg_http_request_setcontentlength(httpReq, 0);

	/*post http request*/
	httpRes = cg_http_customrequest_post(httpReq, ipaddr, port, fileStorePath);

	if ( !cg_http_response_issuccessful(httpRes)) {
		cg_http_request_delete(httpReq);
		return false;
	}

	cg_http_request_delete(httpReq);
	return true;
}
