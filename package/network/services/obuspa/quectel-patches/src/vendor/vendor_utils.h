#ifndef __VENDOR_UTILS__
#define __VENDOR_UTILS__

#include <sys/socket.h>
#include <stdbool.h>
#include "common_defs.h"

#include "zcfg_common.h"

#include <netinet/in.h>
#include <sys/stat.h>
#include "vendor_zcfg.h"
#include "usp_err_codes.h"
#include "vendor_api.h"

#include "zcfg_debug.h"
#include "zcfg_rdm_oid.h"
#include "zcfg_rdm_obj.h"
#include "zcfg_fe_rdm_access.h"
#include "zcfg_fe_rdm_struct.h"
#include "zcfg_eid.h"
#include "zcfg_net.h"
#include <time.h>
#include <cybergarage/ssl/cssl.h>
#include <cybergarage/upnp/cupnp.h>
#include <cybergarage/http/chttp_authentication.h>
#include <cybergarage/soap/csoap.h>
#include <cybergarage/xml/cxml.h>

#include <cybergarage/http/cb64.h>
#include <cybergarage/http/cmd5.h>
#include <cybergarage/http/chttp.h>
#include <cybergarage/http/chttp_cookie.h>
#include <cybergarage/util/clist.h>
#include <cybergarage/util/clog.h>
// For Upload/Download method to upload/download file to/from Apache server, it will has problem if SERISE_AUTH_SUPPORT is 1
// The first upload file will success, but the following upload file will failed (digest).
#ifdef ISKON_CUSTOMIZATION
#define SERISE_AUTH_SUPPORT 0
#else
#define SERISE_AUTH_SUPPORT 1
#endif

/*
    Scheme name pattern recognition strings
*/
#define kSchemePatternFtp       "ftp"
#define kSchemePatternHttp      "http"
#define kSchemePatternHttps     "https"
#define kSchemePatternIpp       "ipp"
#define kSchemePatternSmtp      "smtp"
#define kSchemePatternTelnet    "telnet"


/*
    Default Port Constants
*/
#define kDefaultFtpControlPort  21
#define kDefaultFtpDataPort     20
#define kDefaultHttpPort        80
#define kDefaultIppPort         631
#define kDefaultPop3Port        110
#define kDefaultSmtpPort        25
#define kDefaultSshPort         22
#define kDefaultTelnetPort      23
#define kDefaultTlsPort         443

#define SIZE_URL		1024
#define SIZE_USERNAME	256
#define SIZE_PASSWORD	256
#define    STRSIZE(size)   (size+1)
#define MAX_PARAMETER_NAME_LEN 128
#define TRUE true
#define FALSE false
/*
from rpc/include/cwmprpc_elemdef.h
*/
/* FILE Type */
#define UPLOAD_TYPE_LOG                 "Vendor Log File"
#define UPLOAD_TYPE_CFG               "Vendor Configuration File"
#define LOCAL_LOG_PATH          "/var/log/syslog.log"
#define LOCAL_LOG_FOLDER_PATH           "/var/log/"
#define TR369_FEATURE_UPLOAD_BY_PUT               (1 << 0) // RPC upload file by PUT method instead of POST.
#define CWMP_SOAP_CONTENT_TYPE  "text/xml"//from soap/include/cwmpsoap.h

/*
from cwmp/include/cwmpflash.h
*/
/*local file path*/
#define ZCFG_COMPRESSED_CONFIG_FILE "/tmp/zcfg_config.json"
#define LOCAL_CONFIG_PATH       ZCFG_COMPRESSED_CONFIG_FILE

#define MAX_QUEUED_NUM  3
typedef enum{
        ANY = 0,
        NOT_YET_START = 1,
        IN_PROGRESS = 2,
        COMPLETED = 3
}queue_state_e;


/*
from http/include/cwmphttp.h
#define MAX_NUM_COOKIES         20//cybergarage/http/chttp_cookie.h/
*/
typedef struct cookieInfo_s
{
		int                   httpCookieCount;
		CgHttpCookieData        cookiedata[MAX_NUM_COOKIES];
}cookieInfo_t;

/*
from rpc/include/cwmprpc_elemdef.h begin
*/
/* Transfer Upload Type */
typedef enum{
		eUploadTypeNULL,
		eUploadTypeCfg,
		eUploadTypeLog
}uploadType_t;

typedef struct rpcUpload_s {
		char            commandKey[MAX_PARAMETER_NAME_LEN];
		char            fileType[MAX_PARAMETER_NAME_LEN];
		char            url[MAX_PARAMETER_NAME_LEN];
		char            username[MAX_PARAMETER_NAME_LEN];
		char            password[MAX_PARAMETER_NAME_LEN];
		int          delaySeconds;
		int          ntry;
		int          queuedtTmId;    //priviate used for delay timer--
} rpcUpload_t;

typedef struct rpcDownload_s {
		char            commandKey[MAX_PARAMETER_NAME_LEN];
		char            fileType[MAX_PARAMETER_NAME_LEN];
		char            url[MAX_PARAMETER_NAME_LEN];
		char            username[MAX_PARAMETER_NAME_LEN];
		char            password[MAX_PARAMETER_NAME_LEN];
		int          fileSize;
		char            targetFileName[MAX_PARAMETER_NAME_LEN];
		int          delaySeconds;
		int          ntry;
		int          runningNum;
		char            successURL[MAX_PARAMETER_NAME_LEN];
		char            failureURL[MAX_PARAMETER_NAME_LEN];
		int          queuedtTmId;    //priviate used for delay timer--
} rpcDownload_t;


typedef struct transferInfo_s
{
		rpcDownload_t   *rpcDownload;                           /*get from rpc download request*/
		rpcUpload_t             *rpcUpload;                             /*get from rpc download request*/
		char                    path[STRSIZE(SIZE_URL)];
		int                     fileSrvHTTPSEnable;
		char                    downloadCmdKey[MAX_PARAMETER_NAME_LEN];               /* Download command key */
		uploadType_t    uploadType;
		char                    uploadCmdKey[MAX_PARAMETER_NAME_LEN];                 /* Download command key */
		char                    localFilePath[MAX_PARAMETER_NAME_LEN];
} transferInfo_t;
/*
from rpc/include/cwmprpc_elemdef.h end
*/

/*
from soap/include/cwmpsoap.h begin
*/
typedef struct soapResponse_s{
		CgHttpResponse                  *httpRes;                       /* http response info */
		bool                                    isHttpResCreated;
		int                                     rpcMethod;                      /* rpc method */
		void                                    *rpcElements;           /* rpc elements structure */
		void                                    *userData;
}soapResponse_t;

typedef struct soapRequest_s{
        soapResponse_t                  *soapRes;                       /* soap response */
        CgHttpRequest                   *httpReq;                       /* http request info */
        bool                                    isHttpReqCreated;
        int                                     rpcMethod;                      /* rpc method *//*rpcMethodCode_t*/
        bool                                    isCpeCall;                      /* CPE Calling */
        void                                    *rpcElements;           /* rpc elements structure */
        void                                    *userData;
}soapRequest_t;
/*
from soap/include/cwmpsoap.h end
*/
/*
from cwmp/include/cwmpfsm.h begin
*/
typedef struct tr369SessCB_s{
		int                          eventCode;                      /* event flag */
		CgSocket                        *sock;
		transferInfo_t          *transferInfo;          /* file transfer information */
		soapRequest_t           *soapReq;                       /* http req & soap req*/
		soapResponse_t          *soapResp;                      /* http res & soap res*/
		soapRequest_t           *prevSoapReq;                   /* http req & soap req*/
		soapResponse_t          *prevSoapResp;                  /* http res & soap res*/
		void                            *authList;                      /* http message authdata list */
}tr369SessCB_t;

typedef struct servInfo_s{
	int	port;						/* server port*/
	char url[STRSIZE(SIZE_URL)];
	char username[STRSIZE(SIZE_USERNAME)];
	char password[STRSIZE(SIZE_PASSWORD)];
	char path[STRSIZE(SIZE_URL)];
	char hostname[STRSIZE(SIZE_URL)];
	struct sockaddr_storage ipAddress;				/* server IP address*/
	bool isSecure;
}servInfo_t;

typedef struct tr369ConnCB_s{
		struct tr369ConnCB_s    *prev;                  /* linked list pointers */
		struct tr369ConnCB_s    *next;                  /* linked list pointers */
		char                            *httpMethod;    /* POST, GET, PUT... */
		int                          retryCause;
		servInfo_t                      servInfo;
		CgHttpServer            *httpServer;            /* for connection request */
		tr369SessCB_t           *sessionCb;             /* session control block */
}tr369ConnCB_t;
/*
from cwmp/include/cwmpfsm.h end
*/

/*
    This structure is used to convert Scheme strings to Scheme enums.
*/

typedef struct {
    char *         	pattern;
    int      	defaultPort;
    bool      		isSecure;
} portTable_t, *portTablePtr_t;

/* for DNS Request, ip address choosed order rule*/
typedef enum{
	eMULTIP_CHOOSE_PREV,		/* fause choose the previous ip */
	eMULTIP_CHOOSE_REGULAR,	/* choose the first got ip  */
	eMULTIP_CHOOSE_RANDOM,	/* random choose the ip */
	eMULTIP_CHOOSE_CHECK	/* choose the previous ip if check ok */
}multi_choose_t;


void cwmp_ip_ip2string(struct sockaddr* ip, char *str);
static int resolveSrvUrlName(char *name, struct sockaddr iPAddress_ori, multi_choose_t chostype, struct sockaddr *ip, const char *availableMode);
static portTablePtr_t getPortTable(const char *theStringPtr);
static bool isValidIP(char *ipStr);
int cwmp_parseurl(	const char *theUrl,char *theHostName, struct sockaddr *theHostAddress, int *thePort, char *thePath, bool *isSecure, char *hostName, multi_choose_t chostype, const char * availableMode);

#endif
