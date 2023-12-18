#ifndef __VENDOR_OPERATE_SYNC__
#define __VENDOR_OPERATE_SYNC__

#include "vendor_defs.h"

#define LOCAL_CERT_STORE_PATH 	"/var/local_cert"
#define TEMP_CERT_PATH 			"/var/temp_cert.pem"
#define CERT_TYPE_CA        	"ca"

int localAgentAddCertificate(oper_input_cond_t* cond);
int localAgentAddMyCertificate(oper_input_cond_t* cond);
int localAgentCertificateDelete(oper_input_cond_t* cond);
#if 1//Jessie InstallDU() SetRequestedState()
int swModulesEUSetRequestedState(oper_input_cond_t *cond);
#endif
#endif