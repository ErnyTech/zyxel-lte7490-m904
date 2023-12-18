#include "vendor_zcfg.h"
#include "vendor_operate_sync.h"
#include "usp_err_codes.h"
#include "vendor_api.h"
#include "vendor_utils.h"
#include "zcfg_debug.h"
#include "zcfg_common.h"
#include "zcfg_rdm_oid.h"
#include "zcfg_rdm_obj.h"
#include "zcfg_fe_rdm_access.h"
#include "zcfg_fe_rdm_struct.h"
#include "zcfg_eid.h"
#include "zcfg_net.h"
#include "device.h"
#include <sys/stat.h>//for stat
#include "zyutil.h"//zyUtilIsAppRunning()


zcfgRet_t parseCertPemFile(FILE *fp, char *certificate, char *privateKey){
	char linebuf[65535];
	bool startPub = false;
	bool startPriv = false;
	while(fgets(linebuf, sizeof(linebuf), fp)){
		if(strstr(linebuf,"-----BEGIN CERTIFICATE-----")){
			startPub=true;
		}
		if(strstr(linebuf,"-----BEGIN RSA PRIVATE KEY-----")){
			startPriv=true;
		}

		if(startPub && certificate)
			strcat(certificate, linebuf);
		if(startPriv && privateKey)
			strcat(privateKey, linebuf);

		if(strstr(linebuf,"-----END CERTIFICATE-----")){
			startPub = false;
		}
		if(strstr(linebuf,"-----END RSA PRIVATE KEY-----")){
			startPriv = false;
		}
	}
	return ZCFG_SUCCESS;
}

bool convertCACertToPemFormat(char *filePath)
{
	char cmd[256];
	bool result = false;
	struct stat fileStat;

	USP_LOG_Info("Enter convertCACertToPemFormat()\n");
	if(stat(TEMP_CERT_PATH, &fileStat) == 0)
		remove(TEMP_CERT_PATH);

	if (filePath == NULL || strlen(filePath) == 0)
	{	USP_LOG_Info(" filePath is NULL\n");

		return false;
	}

	//try PEM
	USP_SNPRINTF(cmd, sizeof(cmd), "openssl x509 -inform pem -in %s -out %s", filePath, TEMP_CERT_PATH);
	USP_LOG_Info("Certificate Import: cmd:%s\n", cmd);
	system(cmd);

	if(stat(TEMP_CERT_PATH, &fileStat) == 0){
		if (fileStat.st_size != 0)
			return true;
		else
			remove(TEMP_CERT_PATH);
	}

	//try DER
	if (result == false){
		USP_SNPRINTF(cmd, sizeof(cmd), "openssl x509 -inform der -in %s -out %s", filePath, TEMP_CERT_PATH);
		USP_LOG_Info("Certificate Import: cmd:%s\n", cmd);
		system(cmd);

		if(stat(TEMP_CERT_PATH, &fileStat) == 0){
			if (fileStat.st_size != 0)
				result = true;
			else
				remove(TEMP_CERT_PATH);
		}
	}

	//try pkcs7
	if (result == false){
		USP_SNPRINTF(cmd, sizeof(cmd), "openssl pkcs7 -print_certs -in %s -out %s", filePath, TEMP_CERT_PATH);
		USP_LOG_Info("Certificate Import: cmd:%s\n", cmd);
		system(cmd);

		if(stat(TEMP_CERT_PATH, &fileStat) == 0){
			if (fileStat.st_size != 0)
				result = true;
			else
				remove(TEMP_CERT_PATH);
		}
	}

	//try pkcs7 DER
	if (result == false){
		USP_SNPRINTF(cmd, sizeof(cmd), "openssl pkcs7 -inform der -print_certs -in %s -out %s", filePath, TEMP_CERT_PATH);
		USP_LOG_Info("Certificate Import: cmd:%s\n", cmd);
		system(cmd);

		if(stat(TEMP_CERT_PATH, &fileStat) == 0){
			if (fileStat.st_size != 0)
				result = true;
			else
				remove(TEMP_CERT_PATH);
		}
	}

	return result;

}

bool importCertificate(char *name, char *filePath)
{
	bool ret;
	objIndex_t objIid;
	struct json_object *objCert = NULL;
	FILE *fpCert = NULL;
	int bytes;
	char buf[65535] = {0};
	char path[256] = {0};

	//check certificate before import
	ret = convertCACertToPemFormat(filePath);


	if (ret){
		fpCert = fopen(TEMP_CERT_PATH, "r");
		if (fpCert == NULL)
			ret = false;
	}
	
	//write object to data model
	IID_INIT(objIid);
	if(ret && zcfgFeObjJsonAdd(RDM_OID_DEV_SEC_CERT, &objIid, NULL) != ZCFG_SUCCESS){
		printf("Certificate Import: certificate fail to add\n");
		ret = false;
	}
		printf("Certificate Import: objIid.idx[objIid.level-1] = %d\n", objIid.idx[objIid.level-1]);


	if( ret && zcfgFeObjJsonGet(RDM_OID_DEV_SEC_CERT, &objIid, &objCert) != ZCFG_SUCCESS){
		printf("Certificate Import: certificate fail to get\n");
		zcfgFeObjJsonDel(RDM_OID_DEV_SEC_CERT, &objIid, NULL);
		ret = false;
	}
		printf("Certificate Import: objIid.idx[objIid.level-1] = %d\n", objIid.idx[objIid.level-1]);

	if(ret){
		json_object_object_add(objCert, "Enable", json_object_new_boolean(true));
		json_object_object_add(objCert, "X_ZYXEL_Name", json_object_new_string(name));

		//Cert
		parseCertPemFile(fpCert, buf, NULL);
		json_object_object_add(objCert, "X_ZYXEL_Certificate", json_object_new_string(buf));

		json_object_object_add(objCert, "X_ZYXEL_Type", json_object_new_string(CERT_TYPE_CA));

	}

	if(ret && zcfgFeObjJsonBlockedSet(RDM_OID_DEV_SEC_CERT, &objIid, objCert, NULL) != ZCFG_SUCCESS){
		ret = false;
		printf("Certificate Import: certificate fail to set, start deleting the new object...\n");
		zcfgFeObjJsonDel(RDM_OID_DEV_SEC_CERT, &objIid, NULL);
	}

	if(fpCert)
		fclose(fpCert);
	remove(filePath);
	remove(TEMP_CERT_PATH);
	if(objCert)
		json_object_put(objCert);

	return ret;

}

int localAgentAddCertificate(oper_input_cond_t *cond)
{
	char certificate[65535] = {0};
	char alias[64] = {0};
	char *certStart = NULL;
	char *certEnd = NULL;
	int i;
	struct json_object *certObj = NULL;
	objIndex_t certObjIid;
	int err = USP_ERR_OK;
	FILE *fpCert = NULL;

	USP_LOG_Info("Now enter %s(%d).\n", __func__, __LINE__);

	if(cond->input_args->num_entries) {
		for(i=0; i<cond->input_args->num_entries; ++i) {
			kv_pair_t *kv = &cond->input_args->vector[i];
			USP_LOG_Info("[%s:%d] INPUT key |%s| value|%s|",__func__, __LINE__, kv->key, kv->value);
			if(strcmp(kv->key, "Alias") == 0){
				snprintf(alias, sizeof(alias), "%s", kv->value);
			} else if(strcmp(kv->key, "Certificate") == 0){
				snprintf(certificate, sizeof(certificate), "%s", kv->value);
			}
		}
	}

	//check mandatory arguments
	if(strlen(certificate) == 0 || strlen(alias) == 0){
		USP_LOG_Error("%s(%d) Error: Missing mandatory arguments.\n", __func__, __LINE__);
		return USP_ERR_INVALID_VALUE;	
	}

	fpCert = fopen(LOCAL_CERT_STORE_PATH, "w");
	if (fpCert == NULL)	{
		USP_LOG_Error("%s(%d) Error: Cannot open %s file.\n", __func__, __LINE__, LOCAL_CERT_STORE_PATH);
		return USP_ERR_INTERNAL_ERROR;
	}


	USP_LOG_Info("[%s:%d]Cert = ",__func__, __LINE__);
	if(strstr(certificate,"-----BEGIN CERTIFICATE-----")){
		fwrite("-----BEGIN CERTIFICATE-----\n", 1, strlen("-----BEGIN CERTIFICATE-----\n"), fpCert);
		certStart = certificate + strlen("-----BEGIN CERTIFICATE-----");
		USP_LOG_Info("-----BEGIN CERTIFICATE-----");
	}
	else if(strstr(certificate,"-----BEGIN RSA PRIVATE KEY-----")){
		fwrite("-----BEGIN RSA PRIVATE KEY-----\n", 1, strlen("-----BEGIN RSA PRIVATE KEY-----\n"), fpCert);
		certStart = certificate + strlen("-----BEGIN RSA PRIVATE KEY-----");
		USP_LOG_Info("-----BEGIN RSA PRIVATE KEY-----");
	}

	if((certEnd = strstr(certificate,"-----END CERTIFICATE-----")) && certStart != NULL){
		certEnd[0] = '\n';
		certEnd[1] = '\0';
		fwrite(certStart, 1, strlen(certStart), fpCert);
		USP_LOG_Info("%s", certStart);
		fwrite("-----END CERTIFICATE-----\n", 1, strlen("-----END CERTIFICATE-----\n"), fpCert);
		USP_LOG_Info("-----END CERTIFICATE-----");
	}
	else if((certEnd = strstr(certificate,"-----END RSA PRIVATE KEY-----")) && certStart != NULL){
		certEnd[0] = '\n';
		certEnd[1] = '\0';
		fwrite(certStart, 1, strlen(certStart), fpCert);
		USP_LOG_Info("%s", certStart);
		fwrite("-----END RSA PRIVATE KEY-----\n", 1, strlen("-----END RSA PRIVATE KEY-----\n"), fpCert);
		USP_LOG_Info("-----END RSA PRIVATE KEY-----");
	}

	if(fpCert)
		fclose(fpCert);


	IID_INIT(certObjIid);
	while(zcfgFeObjJsonGetNext(RDM_OID_DEV_SEC_CERT, &certObjIid, &certObj) == ZCFG_SUCCESS) {
		if(strcmp(json_object_get_string(json_object_object_get(certObj, "X_ZYXEL_Name")), alias) == 0) {
			USP_LOG_Error("%s(%d) Error: A certificate with the same name already exists.\n", __func__, __LINE__);
			return USP_ERR_INTERNAL_ERROR;
		}
	}

	if(!importCertificate(alias, LOCAL_CERT_STORE_PATH)) {
		USP_LOG_Error("%s(%d) Error: Import Certificate Fail.\n", __func__, __LINE__);
		return USP_ERR_INTERNAL_ERROR;
	}


	USP_LOG_Info("Now leave %s(%d).\n", __func__, __LINE__);
	return err;
}

int localAgentAddMyCertificate(oper_input_cond_t *cond)
{
	char certificate[65535] = {0};
	char alias[64] = {0};
	int i;
	struct json_object *certObj = NULL;
	struct json_object *localAgentCertObj = NULL;
	char path[256] = {0};
	char param[256] = {0};
	char credential[256];
	char securityPath[256] = {0};
	objIndex_t certObjIid;
	objIndex_t localAgentCertIid;
	int err = USP_ERR_INTERNAL_ERROR;
	int len;

	USP_LOG_Info("Now enter %s(%d).\n", __func__, __LINE__);

	if(cond->input_args->num_entries) {
		for(i=0; i<cond->input_args->num_entries; ++i) {
			kv_pair_t *kv = &cond->input_args->vector[i];
			USP_LOG_Info("[%s:%d] INPUT key |%s| value|%s|",__func__, __LINE__, kv->key, kv->value);
			if(strcmp(kv->key, "Alias") == 0){
				snprintf(alias, sizeof(alias), "%s", kv->value);
			} else if(strcmp(kv->key, "Certificate") == 0){
				snprintf(certificate, sizeof(certificate), "%s", kv->value);
			}
		}
	}

	//check mandatory arguments
	if(strlen(certificate) == 0){
		USP_LOG_Error("%s(%d) Error: Missing mandatory arguments.\n", __func__, __LINE__);
		return USP_ERR_INVALID_VALUE;	
	}

	if((err = localAgentAddCertificate(cond)) != USP_ERR_OK)
	{
		USP_LOG_Error("%s(%d) Error: localAgentAddCertificate Fail.\n", __func__, __LINE__);	
		return err;
	}

	//Set Certificate path to Device.LocalAgent.Controller.i.Credential
	IID_INIT(certObjIid);
	while(zcfgFeObjJsonGetNext(RDM_OID_DEV_SEC_CERT, &certObjIid, &certObj) == ZCFG_SUCCESS) {
		if(strcmp(json_object_get_string(json_object_object_get(certObj, "X_ZYXEL_Name")), alias) == 0){ //Comparison input args & database X_ZYXEL_Name is identical

			
			snprintf(securityPath, sizeof(securityPath), "Device.Security.Certificate.%d", certObjIid.idx[0]);
			IID_INIT(localAgentCertIid);
			while((zcfgFeObjJsonGetNext(RDM_OID_DEV_LOCAL_AGENT_CERTIFICATE, &localAgentCertIid, (void **)&localAgentCertObj)) == ZCFG_SUCCESS) {
				if(strcmp(json_object_get_string(json_object_object_get(localAgentCertObj, "X_ZYXEL_SecurityCert")), securityPath) == 0) {
					if(strcmp(json_object_get_string(json_object_object_get(localAgentCertObj, "Alias")), alias) == 0 \
						&& strcmp(json_object_get_string(json_object_object_get(localAgentCertObj, "Issuer")), json_object_get_string(json_object_object_get(certObj, "Issuer"))) == 0 \
							&& strcmp(json_object_get_string(json_object_object_get(localAgentCertObj, "SerialNumber")), json_object_get_string(json_object_object_get(certObj, "SerialNumber"))) == 0)	{

						snprintf(path, sizeof(path), "Device.LocalAgent.Certificate.%d", localAgentCertIid.idx[0]);
						snprintf(credential, sizeof(credential),"%sCredential",cond->parentPath);
						USP_LOG_Info("credential = %s.\n", credential);
						if(zcfg_get_value(credential, param, &len) != 0)
						{
							printf("[%s]zcfg_get_value fail!!!\n", __FUNCTION__);
							return USP_ERR_INTERNAL_ERROR;
						}

						if(strcmp(param, "") == 0) {
							strcpy(param, path);
						}
						else {
							strcat(param, ",");
							strcat(param, path);
						}

						USP_LOG_Info("param = %s.\n", param);
						if((err = zcfg_set_value(credential, param))!= 0)
						{
							printf("[%s]zcfg_set_value fail!!!\n", __FUNCTION__);
							return err;
						}
						break;
					}
				}
			}
		}
	}

	USP_LOG_Info("Now leave %s(%d).\n", __func__, __LINE__);
	return err;
}

int localAgentCertificateDelete(oper_input_cond_t *cond)
{
	int i;
	int err = USP_ERR_OK;
	char securityCertificatePath[256] = {0};
	char *strtmp;
	int len = strlen("Device.Security.Certificate.");
	int securityCertificateLen = 0;
	char path[256] = {0};

	USP_LOG_Info("Now enter %s(%d).\n", __func__, __LINE__);
	snprintf(path, sizeof(path), "%s%s", cond->parentPath, "X_ZYXEL_SecurityCert");
	zcfg_get_value(path, securityCertificatePath, &securityCertificateLen);
	strtmp = securityCertificatePath + len;

	USP_LOG_Info("securityCertificatePath is %s\n", securityCertificatePath);
	if((err = zcfg_del_object(securityCertificatePath)) != USP_ERR_OK)	{
		USP_LOG_Error("%s(%d) Error: Delete Certificate Fail.\n", __func__, __LINE__);
	}

	USP_LOG_Info("Now leave %s(%d).\n", __func__, __LINE__);
	return err;
}

#if 1//Jessie InstallDU() SetRequestedState()
extern char *device_sweu_root_with_dot;
int swModulesEUSetRequestedState(oper_input_cond_t *cond)
{
	int i;
	char state[8];//valid value: Idle, Active
	char path[128];
	char cmd[256];
	char *EUInstance = NULL;
	int instance;
	char valueSetPath[256] = {0};
	USP_LOG_Info("Now enter %s(%d).\n", __func__, __LINE__);

	memset(state, 0, sizeof(state));
	if(cond->input_args->num_entries) {
		for(i=0; i<cond->input_args->num_entries; ++i) {
			kv_pair_t *kv = &cond->input_args->vector[i];
			if(kv){
				USP_LOG_Info("[%s:%d] INPUT key |%s| value|%s|",__func__, __LINE__, kv->key, kv->value);
				if(strcmp(kv->key, "RequestedState") == 0){
					snprintf(state, sizeof(state), "%s", kv->value);
				}
			} else {
				USP_LOG_Info("kv NULL\n");
				return USP_ERR_INVALID_VALUE;
			}
		}
	}

	//check mandatory arguments
	if(strlen(state) == 0){
		USP_LOG_Error("%s(%d) Error: Missing mandatory arguments.\n", __func__, __LINE__);
		return USP_ERR_INVALID_VALUE;	
	}
	
	if( strcmp(state, "Idle") && strcmp(state, "Active") ){
		USP_LOG_Error("%s(%d) Error: Invalid input argument value.\n", __func__, __LINE__);
		return USP_ERR_INVALID_VALUE;	
	}

	
	//execute
	if( !strcmp(state, "Active") ){
		USP_LOG_Info("Execute cond->parentPath %s\n", cond->parentPath);
		snprintf(path, sizeof(path), "%s", cond->parentPath);
		EUInstance = strstr(path, device_sweu_root_with_dot);
		if( EUInstance ){
			sscanf(path, "Device.SoftwareModules.ExecutionUnit.%d.", &instance);
			USP_LOG_Info("instance %d\n", instance);

			snprintf(cmd, sizeof(cmd), "%s%d", DU_PATH, instance);
			USP_LOG_Info("cmd %s\n", cmd);
			if(!zyUtilIsAppRunning(cmd)){
				snprintf(cmd, sizeof(cmd), "sh %s%d &", DU_PATH, instance);
				USP_LOG_Info("execute cmd %s\n", cmd);
				system(cmd);
			}
		}
	} else if( !strcmp(state, "Idle") ){
		USP_LOG_Info("Execute cond->parentPath %s\n", cond->parentPath);
		snprintf(path, sizeof(path), "%s", cond->parentPath);
		EUInstance = strstr(path, device_sweu_root_with_dot);
		if( EUInstance ){
			sscanf(path, "Device.SoftwareModules.ExecutionUnit.%d.", &instance);
			USP_LOG_Info("instance %d\n", instance);

			snprintf(cmd, sizeof(cmd), "%s%d", DU_PATH, instance);
			USP_LOG_Info("cmd %s\n", cmd);
			if(zyUtilIsAppRunning(cmd)){
				snprintf(cmd, sizeof(cmd), "kill -9 `ps -w | grep %s%d | grep -v grep | awk '{print $1}'`", DU_PATH, instance);
				USP_LOG_Info("%s() kill command %s\n", __FUNCTION__, cmd);
				system(cmd);
			}
		}
	}

	//Set EU parameters
	{
		USP_SNPRINTF(valueSetPath, sizeof(valueSetPath), "Device.SoftwareModules.ExecutionUnit.%d.Status", instance);
		printf("[Marcus 0810] %s:%d, state %s.\n", __func__, __LINE__, state);
		if(zcfg_set_value(valueSetPath, state) != USP_ERR_OK) {
			USP_LOG_Error("%s(%d) Error: fail to set ExectionUnit Unit(%d) Status value.\n", __func__, __LINE__, instance);
			return USP_ERR_INVALID_VALUE;	
		}
	}

	USP_LOG_Info("Now leave %s(%d).\n", __func__, __LINE__);
	return USP_ERR_OK;
	
}
#endif
