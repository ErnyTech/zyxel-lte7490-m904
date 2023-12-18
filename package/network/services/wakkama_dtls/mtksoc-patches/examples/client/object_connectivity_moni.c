/*******************************************************************************
 *
 * Copyright (c) 2014 Bosch Software Innovations GmbH Germany.
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
 *    Pascal Rieux - Please refer to git log
 *    Scott Bertin, AMETEK, Inc. - Please refer to git log
 *
 *******************************************************************************/

/*
 *  This Connectivity Monitoring object is optional and has a single instance
 * 
 *  Resources:
 *
 *          Name             | ID | Oper. | Inst. | Mand.|  Type   | Range | Units |
 *  Network Bearer           |  0 |  R    | Single|  Yes | Integer |       |       |
 *  Available Network Bearer |  1 |  R    | Multi |  Yes | Integer |       |       |
 *  Radio Signal Strength    |  2 |  R    | Single|  Yes | Integer |       | dBm   |
 *  Link Quality             |  3 |  R    | Single|  No  | Integer | 0-100 |   %   |
 *  IP Addresses             |  4 |  R    | Multi |  Yes | String  |       |       |
 *  Router IP Addresses      |  5 |  R    | Multi |  No  | String  |       |       |
 *  Link Utilization         |  6 |  R    | Single|  No  | Integer | 0-100 |   %   |
 *  APN                      |  7 |  R    | Multi |  No  | String  |       |       |
 *  Cell ID                  |  8 |  R    | Single|  No  | Integer |       |       |
 *  SMNC                     |  9 |  R    | Single|  No  | Integer | 0-999 |   %   |
 *  SMCC                     | 10 |  R    | Single|  No  | Integer | 0-999 |       |
 *
 */

#include "liblwm2m.h"
#include "lwm2mclient.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#if 1//Jessie Device
#include "zcfg_debug.h"
#include "zcfg_common.h"
#include "zcfg_rdm_oid.h"
#include "zcfg_rdm_obj.h"
#include "zcfg_fe_rdm_access.h"
#include "zcfg_fe_rdm_struct.h"
#include "zcfg_eid.h"
#include "zcfg_net.h"


#include "zcmd_schema.h"
#include "zcfg_msg.h"
#include "zcmd_tool.h"
#include "zlog_api.h"
#include "json-c/json_object.h"
#define WAKKAMA_MAPPING_TABLE "/etc/zcfg_wakkama_mapping_table"
#endif

// Resource Id's:
#define RES_M_NETWORK_BEARER            0
#define RES_M_AVL_NETWORK_BEARER        1
#define RES_M_RADIO_SIGNAL_STRENGTH     2
#define RES_O_LINK_QUALITY              3
#define RES_M_IP_ADDRESSES              4
#define RES_O_ROUTER_IP_ADDRESS         5
#define RES_O_LINK_UTILIZATION          6
#define RES_O_APN                       7
#define RES_O_CELL_ID                   8
#define RES_O_SMNC                      9
#define RES_O_SMCC                      10

#define VALUE_NETWORK_BEARER_GSM    0   //GSM see 
#define VALUE_AVL_NETWORK_BEARER_1  0   //GSM
#define VALUE_AVL_NETWORK_BEARER_2  21  //WLAN
#define VALUE_AVL_NETWORK_BEARER_3  41  //Ethernet
#define VALUE_AVL_NETWORK_BEARER_4  42  //DSL
#define VALUE_AVL_NETWORK_BEARER_5  43  //PLC
#define VALUE_IP_ADDRESS_1              "192.168.178.101"
#define VALUE_IP_ADDRESS_2              "192.168.178.102"
#define VALUE_ROUTER_IP_ADDRESS_1       "192.168.178.001"
#define VALUE_ROUTER_IP_ADDRESS_2       "192.168.178.002"
#define VALUE_APN_1                     "web.vodafone.de"
#define VALUE_APN_2                     "cda.vodafone.de"
#define VALUE_CELL_ID                   69696969
#define VALUE_RADIO_SIGNAL_STRENGTH     80                  //dBm
#define VALUE_LINK_QUALITY              98     
#define VALUE_LINK_UTILIZATION          666
#define VALUE_SMNC                      33
#define VALUE_SMCC                      44

typedef struct
{
    char ipAddresses[2][16];        // limited to 2!
    char routerIpAddresses[2][16];  // limited to 2!
    long cellId;
    int signalStrength;
    int linkQuality;
    int linkUtilization;
} conn_m_data_t;

static bool checkConnMoniValueChange(int resourceId, int resourceInsId/*multi resource using, single resource no effect*/, conn_moni_instance_t * targetP, char * zcfgParamValue)
{
    resource_ins_list_t * resourceIns;

    switch(resourceId)
    {
    case RES_M_NETWORK_BEARER:
        if(strcmp(targetP->network_bearer, zcfgParamValue))
        {
            strncpy(targetP->network_bearer, zcfgParamValue, sizeof(targetP->network_bearer));
            return true;
        }
        break;

    case RES_M_AVL_NETWORK_BEARER:
    {
        if(LWM2M_LIST_FIND(targetP->avl_network_bearer, resourceInsId) == NULL)
        {
            resourceIns = (resource_ins_list_t *)lwm2m_malloc(sizeof(resource_ins_list_t));
            memset(resourceIns, 0, sizeof(resource_ins_list_t));
            resourceIns->resInstId = resourceInsId;
            memset(resourceIns->value, 0, 257);
            targetP->avl_network_bearer = LWM2M_LIST_ADD(targetP->avl_network_bearer, resourceIns);
        }
        else 
        {
            resourceIns = (resource_ins_list_t *)LWM2M_LIST_FIND(targetP->avl_network_bearer, resourceInsId);
        }
        if(strcmp(resourceIns->value, zcfgParamValue))
        {
            strncpy(resourceIns->value, zcfgParamValue, sizeof(resourceIns->value));
            return true;
        }
        break;
    }

    case RES_M_RADIO_SIGNAL_STRENGTH:
        if(strcmp(targetP->radio_signal_strength, zcfgParamValue))
        {
            strncpy(targetP->radio_signal_strength, zcfgParamValue, sizeof(targetP->radio_signal_strength));
            return true;
        }
        break;

    case RES_O_LINK_QUALITY:
        if(strcmp(targetP->link_quality, zcfgParamValue))
        {
            strncpy(targetP->link_quality, zcfgParamValue, sizeof(targetP->link_quality));
            return true;
        }
        break;

    case RES_M_IP_ADDRESSES:
    {
        if(LWM2M_LIST_FIND(targetP->ip_addresses, resourceInsId) == NULL)
        {
            resourceIns = (resource_ins_list_t *)lwm2m_malloc(sizeof(resource_ins_list_t));
            memset(resourceIns, 0, sizeof(resource_ins_list_t));
            resourceIns->resInstId = resourceInsId;
            memset(resourceIns->value, 0, 257);
            targetP->ip_addresses = LWM2M_LIST_ADD(targetP->ip_addresses, resourceIns);
        }
        else 
        {
            resourceIns = (resource_ins_list_t *)LWM2M_LIST_FIND(targetP->ip_addresses, resourceInsId);
        }
        if(strcmp(resourceIns->value, zcfgParamValue))
        {
            strncpy(resourceIns->value, zcfgParamValue, sizeof(resourceIns->value));
            return true;
        }
        break;
    }

    case RES_O_ROUTER_IP_ADDRESS:
    {
        if(LWM2M_LIST_FIND(targetP->router_ip_address, resourceInsId) == NULL)
        {
            resourceIns = (resource_ins_list_t *)lwm2m_malloc(sizeof(resource_ins_list_t));
            memset(resourceIns, 0, sizeof(resource_ins_list_t));
            resourceIns->resInstId = resourceInsId;
            memset(resourceIns->value, 0, 257);
            targetP->router_ip_address = LWM2M_LIST_ADD(targetP->router_ip_address, resourceIns);
        }
        else 
        {
            resourceIns = (resource_ins_list_t *)LWM2M_LIST_FIND(targetP->router_ip_address, resourceInsId);
        }
        if(strcmp(resourceIns->value, zcfgParamValue))
        {
            strncpy(resourceIns->value, zcfgParamValue, sizeof(resourceIns->value));
            return true;
        }
        break;
    }
        
    case RES_O_LINK_UTILIZATION:
        if(strcmp(targetP->link_utilization, zcfgParamValue))
        {
            strncpy(targetP->link_utilization, zcfgParamValue, sizeof(targetP->link_utilization));
            return true;
        }
        break;

    case RES_O_APN:
    {
        if(LWM2M_LIST_FIND(targetP->apn, resourceInsId) == NULL)
        {
            resourceIns = (resource_ins_list_t *)lwm2m_malloc(sizeof(resource_ins_list_t));
            memset(resourceIns, 0, sizeof(resource_ins_list_t));
            resourceIns->resInstId = resourceInsId;
            memset(resourceIns->value, 0, 257);
            targetP->apn = LWM2M_LIST_ADD(targetP->apn, resourceIns);
        }
        else 
        {
            resourceIns = (resource_ins_list_t *)LWM2M_LIST_FIND(targetP->apn, resourceInsId);
        }
        if(strcmp(resourceIns->value, zcfgParamValue))
        {
            strncpy(resourceIns->value, zcfgParamValue, sizeof(resourceIns->value));
            return true;
        }
        break;
    }

    case RES_O_CELL_ID:
        if(strcmp(targetP->cell_id, zcfgParamValue))
        {
            strncpy(targetP->cell_id, zcfgParamValue, sizeof(targetP->cell_id));
            return true;
        }
        break;
    case RES_O_SMNC:
        if(strcmp(targetP->smnc, zcfgParamValue))
        {
            strncpy(targetP->smnc, zcfgParamValue, sizeof(targetP->smnc));
            return true;
        }
        break;
    case RES_O_SMCC:
        if(strcmp(targetP->smcc, zcfgParamValue))
        {
            strncpy(targetP->smcc, zcfgParamValue, sizeof(targetP->smcc));
            return true;
        }
        break;
    
    default:
        break;
    }
    return false;
}

bool updateConnMoniMultiResourceIns(char *omaId, uint16_t resourceId)
{
    objIndex_t objIid;
    objIndex_t ipIfIid;
    objIndex_t v6AddrIid;
    objIndex_t v4AddrIid;
    struct json_object *value = NULL;
    struct json_object *ipIfValue = NULL;
    char param[65] = {0};
    char *token;
    int newCount = 0;
    uint32_t oid;
    int ret = ZCFG_SUCCESS;

    switch(resourceId)
    {
        case RES_O_APN:
            IID_INIT(objIid);
            while(zcfgFeObjJsonGetNext(RDM_OID_CELL_ACCESS_POINT, &objIid, &value) == ZCFG_SUCCESS)
            {    
                newCount++;
                json_object_put(value);
            }
            updateInsInZcfg(omaId, -1, newCount);
            break;
        case RES_M_IP_ADDRESSES:
            IID_INIT(ipIfIid);
            while(zcfgFeObjJsonGetNext(RDM_OID_IP_IFACE, &ipIfIid, &ipIfValue) == ZCFG_SUCCESS)
            {
                strcpy(param, (char *)zcfgFeParamValGet(ipIfValue, "Name"));
                if(!strcmp(param, "WAN"))
                {
                    IID_INIT(v6AddrIid);
                    while(zcfgFeSubInObjJsonGetNext(RDM_OID_IP_IFACE_V6_ADDR, &ipIfIid, &v6AddrIid, &value) == ZCFG_SUCCESS)
                    {
                        strcpy(param, (char *)zcfgFeParamValGet(value, "IPAddress"));
                        if(strcmp(param, ""))
                        {
                            newCount++;
                        }
                        json_object_put(value);
                    }

                    IID_INIT(v4AddrIid);
                    while(zcfgFeSubInObjJsonGetNext(RDM_OID_IP_IFACE_V4_ADDR, &ipIfIid, &v4AddrIid, &value) == ZCFG_SUCCESS)
                    {
                        strcpy(param, (char *)zcfgFeParamValGet(value, "IPAddress"));
                        if(strcmp(param, ""))
                        {
                            newCount++;
                        }
                        json_object_put(value);
                    }
                }
                json_object_put(ipIfValue);
            }
            updateInsInZcfg(omaId, -1, newCount);
            break;

        case RES_O_ROUTER_IP_ADDRESS:
            IID_INIT(objIid);
            while(zcfgFeObjJsonGetNext(RDM_OID_DHCPV4_CLIENT, &objIid, &value) == ZCFG_SUCCESS)
            {    
                newCount++;
                json_object_put(value);
            }
            updateInsInZcfg(omaId, -1, newCount);
            break;

        case RES_M_AVL_NETWORK_BEARER:
            IID_INIT(objIid);
            objIid.idx[objIid.level++] = 1;
            ret = zcfgFeObjJsonGet(RDM_OID_CELL_INTF, &objIid, &value);
            if(ret == ZCFG_SUCCESS)
            {
                strcpy(param, (char *)zcfgFeParamValGet(value, "SupportedAccessTechnologies"));
            }
            else
            {
                printf("*********%s() Jessi ret=%d\n", __FUNCTION__, ret);
            }
            json_object_put(value);
            value = NULL;

            token = strtok(param, ",");
            while( token != NULL ) 
            {
                newCount++;
                token = strtok(NULL, ",");
            }
            updateInsInZcfg(omaId, -1, newCount);
            break;

        default:
            break;
    }  
    return true;
}
static uint8_t prv_conn_mon_discover(lwm2m_context_t *contextP,
                                   uint16_t instanceId,
                                   int * numDataP,
                                   lwm2m_data_t ** dataArrayP,
                                   lwm2m_object_t * objectP)
{
    server_instance_t * targetP;
    uint8_t result;
    int i;

	printf("[Marcus debug] %s(%d) instanceId %d, numDataP %d.\n", __func__, __LINE__, instanceId, numDataP);
    /* unused parameter */
    (void)contextP;

    result = COAP_205_CONTENT;

    targetP = (server_instance_t *)lwm2m_list_find(objectP->instanceList, instanceId);
    if (NULL == targetP) return COAP_404_NOT_FOUND;

    // is the server asking for the full object ?
    if (*numDataP == 0)
    {
        uint16_t resList[] = {
			RES_M_NETWORK_BEARER,
			RES_M_AVL_NETWORK_BEARER,
			RES_M_RADIO_SIGNAL_STRENGTH,
			RES_O_LINK_QUALITY,
			RES_M_IP_ADDRESSES,         
			RES_O_ROUTER_IP_ADDRESS,
			RES_O_LINK_UTILIZATION,
			RES_O_APN,
			RES_O_CELL_ID,
			RES_O_SMNC,
			RES_O_SMCC,
        };
        int nbRes = sizeof(resList) / sizeof(uint16_t);


        *dataArrayP = lwm2m_data_new(nbRes);
        if (*dataArrayP == NULL) return COAP_500_INTERNAL_SERVER_ERROR;
        *numDataP = nbRes;
        for (i = 0; i < nbRes; i++)
        {
            (*dataArrayP)[i].id = resList[i];
        }
    }
    else
    {
        for (i = 0; i < *numDataP && result == COAP_205_CONTENT; i++)
        {
            switch ((*dataArrayP)[i].id)
            {
            case RES_M_NETWORK_BEARER:
            case RES_M_AVL_NETWORK_BEARER:
            case RES_M_RADIO_SIGNAL_STRENGTH:
            case RES_O_LINK_QUALITY:
			case RES_M_IP_ADDRESSES:
			case RES_O_LINK_UTILIZATION:
			case RES_O_APN:
			case RES_O_CELL_ID:
			case RES_O_SMNC:
			case RES_O_SMCC:
                break;

            default:
                result = COAP_404_NOT_FOUND;
                break;
            }
        }
    }

    return result;
}
static uint8_t prv_read(lwm2m_context_t *contextP,
                        uint16_t instanceId,
                        int * numDataP,
                        lwm2m_data_t ** dataArrayP,
                        lwm2m_object_t * objectP)
{
    uint8_t result;
    int i, j;
	int ret = ZCFG_SUCCESS;
	char omaID[16] = {0};
	char desValue[512] = {0};
	lwm2m_data_t * subTlvP;
	size_t count;
    conn_moni_instance_t * targetP;

    /* unused parameter */
    (void)contextP;

    // this is a single instance object
    if (instanceId != 0)
    {
        return COAP_404_NOT_FOUND ;
    }

    targetP = (conn_moni_instance_t *)lwm2m_list_find(objectP->instanceList, instanceId);
    if (NULL == targetP) return COAP_404_NOT_FOUND;

    // is the server asking for the full object ?
    if (*numDataP == 0)
    {
        uint16_t resList[] = {
                RES_M_NETWORK_BEARER,
                RES_M_AVL_NETWORK_BEARER,
                RES_M_RADIO_SIGNAL_STRENGTH,
                RES_O_LINK_QUALITY,
                RES_M_IP_ADDRESSES,
                RES_O_ROUTER_IP_ADDRESS,
                RES_O_LINK_UTILIZATION,
                RES_O_APN,
                RES_O_CELL_ID,
                RES_O_SMNC,
                RES_O_SMCC
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
		snprintf(omaID, 16, "%d_%d", objectP->objID, (*dataArrayP)[i].id);

		switch ((*dataArrayP)[i].id)
		{
            //Int
			case RES_M_NETWORK_BEARER:
			case RES_M_RADIO_SIGNAL_STRENGTH:
			case RES_O_LINK_QUALITY:
			case RES_O_LINK_UTILIZATION:
			case RES_O_CELL_ID:
			case RES_O_SMNC:
            case RES_O_SMCC:

				if ((*dataArrayP)[i].type == LWM2M_TYPE_MULTIPLE_RESOURCE) return COAP_404_NOT_FOUND;
				if((ret = prv_get_zcfg_value(omaID, -1, -1, desValue)) != ZCFG_SUCCESS)
				{
					printf("[%s]prv_get_zcfg_value fail, ret = %d!!!\n", __FUNCTION__, ret);
				}
                if(checkConnMoniValueChange((*dataArrayP)[i].id, -1, targetP, desValue))
                {
                    setChangeValueList(objectP->objID, -1,  (*dataArrayP)[i].id,  -1);
                }
				lwm2m_data_encode_int(atoi(desValue), *dataArrayP + i);
				result = COAP_205_CONTENT;
				break;
            //Multi string
			case RES_O_APN:	
			case RES_M_IP_ADDRESSES:
			case RES_O_ROUTER_IP_ADDRESS:
            {
                updateConnMoniMultiResourceIns(omaID, (*dataArrayP)[i].id);
                setResourceInsToData(omaID, -1, &subTlvP, &count);
                lwm2m_data_encode_instances(subTlvP, count, *dataArrayP + i);

                for (j = 0; j < count; j++)
                {
                    if((ret = prv_get_zcfg_value(omaID, -1, subTlvP[j].id, desValue)) != ZCFG_SUCCESS)
                    {
                        printf("[%s]prv_get_zcfg_value fail, ret = %d!!!\n", __FUNCTION__, ret);
                    }
                    if(checkConnMoniValueChange((*dataArrayP)[i].id, j, targetP, desValue))
                    {
                        setChangeValueList(objectP->objID, -1,  (*dataArrayP)[i].id,  j);
                    }
                    lwm2m_data_encode_string(desValue, &subTlvP[j]);
                }
                result = COAP_205_CONTENT;
                break;
            }	
                break;
            //Multi uint
			case RES_M_AVL_NETWORK_BEARER:
            {
                updateConnMoniMultiResourceIns(omaID, (*dataArrayP)[i].id);
                setResourceInsToData(omaID, -1, &subTlvP, &count);
                lwm2m_data_encode_instances(subTlvP, count, *dataArrayP + i);

                for (j = 0; j < count; j++)
                {
                    if((ret = prv_get_zcfg_value(omaID, -1, subTlvP[j].id, desValue)) != ZCFG_SUCCESS)
                    {
                        printf("[%s]prv_get_zcfg_value fail, ret = %d!!!\n", __FUNCTION__, ret);
                    }
                    if(checkConnMoniValueChange((*dataArrayP)[i].id, j, targetP, desValue))
                    {
                        setChangeValueList(objectP->objID, -1,  (*dataArrayP)[i].id,  j);
                    }
                    lwm2m_data_encode_uint(strtoul(desValue, NULL, 0), &subTlvP[j]);
                }
                result = COAP_205_CONTENT;
                break;
            }			
            default:
				result = COAP_404_NOT_FOUND;
		}
        i++;
    } while (i < *numDataP && result == COAP_205_CONTENT );

    return result;
}

lwm2m_object_t * get_object_conn_m(void)
{
    /*
     * The get_object_conn_m() function create the object itself and return a pointer to the structure that represent it.
     */
    lwm2m_object_t * connObj;

    connObj = (lwm2m_object_t *) lwm2m_malloc(sizeof(lwm2m_object_t));

    if (NULL != connObj)
    {

        conn_moni_instance_t * connMoniInstance;

        memset(connObj, 0, sizeof(lwm2m_object_t));

        /*
         * It assigns his unique ID
         */
        connObj->objID = LWM2M_CONN_MONITOR_OBJECT_ID;
        
        /*
         * and its unique instance
         *
         */
        connMoniInstance = (conn_moni_instance_t *)lwm2m_malloc(sizeof(conn_moni_instance_t));
        if (NULL == connMoniInstance)
        {
            lwm2m_free(connObj);
            return NULL;
        }

        memset(connMoniInstance, 0, sizeof(conn_moni_instance_t));
        connMoniInstance->instanceId = 0;
        memset(connMoniInstance->network_bearer, 0, 257);
        memset(connMoniInstance->radio_signal_strength, 0, 257);
        memset(connMoniInstance->link_quality, 0, 257);
        memset(connMoniInstance->link_utilization, 0, 257);
        memset(connMoniInstance->cell_id, 0, 257);
        memset(connMoniInstance->smnc, 0, 257);
        memset(connMoniInstance->smcc, 0, 257);
        connObj->instanceList = LWM2M_LIST_ADD(connObj->instanceList, connMoniInstance);
        
        /*
         * And the private function that will access the object.
         * Those function will be called when a read/write/execute query is made by the server. In fact the library don't need to
         * know the resources of the object, only the server does.
         */
        connObj->readFunc = prv_read;
        connObj->executeFunc = NULL;
        connObj->discoverFunc = prv_conn_mon_discover;
        connObj->userData = lwm2m_malloc(sizeof(conn_m_data_t));

        /*
         * Also some user data can be stored in the object with a private structure containing the needed variables
         */
        if (NULL != connObj->userData)
        {
            conn_m_data_t *myData = (conn_m_data_t*) connObj->userData;
            myData->cellId          = VALUE_CELL_ID;
            myData->signalStrength  = VALUE_RADIO_SIGNAL_STRENGTH;
            myData->linkQuality     = VALUE_LINK_QUALITY;
            myData->linkUtilization = VALUE_LINK_UTILIZATION;
            strcpy(myData->ipAddresses[0],       VALUE_IP_ADDRESS_1);
            strcpy(myData->ipAddresses[1],       VALUE_IP_ADDRESS_2);
            strcpy(myData->routerIpAddresses[0], VALUE_ROUTER_IP_ADDRESS_1);
            strcpy(myData->routerIpAddresses[1], VALUE_ROUTER_IP_ADDRESS_2);
        }
        else
        {
            lwm2m_free(connObj);
            connObj = NULL;
        }
    }
    return connObj;
}

void free_object_conn_m(lwm2m_object_t * objectP)
{
    lwm2m_free(objectP->userData);
    while (objectP->instanceList != NULL)
    {
        conn_moni_instance_t * connMoniInstance = (conn_moni_instance_t *)objectP->instanceList;

        while (connMoniInstance->avl_network_bearer != NULL)
        {
            resource_ins_list_t * resourceIns = connMoniInstance->avl_network_bearer;
            connMoniInstance->avl_network_bearer = connMoniInstance->avl_network_bearer->next;
            lwm2m_free(resourceIns);
        }
        while (connMoniInstance->ip_addresses != NULL)
        {
            resource_ins_list_t * resourceIns = connMoniInstance->ip_addresses;
            connMoniInstance->ip_addresses = connMoniInstance->ip_addresses->next;
            lwm2m_free(resourceIns);
        }
        while (connMoniInstance->router_ip_address != NULL)
        {
            resource_ins_list_t * resourceIns = connMoniInstance->router_ip_address;
            connMoniInstance->router_ip_address = connMoniInstance->router_ip_address->next;
            lwm2m_free(resourceIns);
        }
        while (connMoniInstance->apn != NULL)
        {
            resource_ins_list_t * resourceIns = connMoniInstance->apn;
            connMoniInstance->apn = connMoniInstance->apn->next;
            lwm2m_free(resourceIns);
        }
        objectP->instanceList = objectP->instanceList->next;
        lwm2m_free(connMoniInstance);
    }
    lwm2m_free(objectP);
}

uint8_t connectivity_moni_change(lwm2m_data_t * dataArray,
                                 lwm2m_object_t * objectP)
{
    int64_t value;
    uint8_t result;
    conn_m_data_t * data;

    data = (conn_m_data_t*) (objectP->userData);

    switch (dataArray->id)
    {
    case RES_M_RADIO_SIGNAL_STRENGTH:
        if (1 == lwm2m_data_decode_int(dataArray, &value))
        {
            data->signalStrength = value;
            result = COAP_204_CHANGED;
        }
        else
        {
            result = COAP_400_BAD_REQUEST;
        }
        break;

    case RES_O_LINK_QUALITY:
        if (1 == lwm2m_data_decode_int(dataArray, &value))
        {
            data->linkQuality = value;
            result = COAP_204_CHANGED;
        }
        else
        {
            result = COAP_400_BAD_REQUEST;
        }
        break;

    case RES_M_IP_ADDRESSES:
        if (sizeof(data->ipAddresses[0]) <= dataArray->value.asBuffer.length)
        {
            result = COAP_400_BAD_REQUEST;
        }
        else
        {
            memset(data->ipAddresses[0], 0, sizeof(data->ipAddresses[0]));
            memcpy(data->ipAddresses[0], dataArray->value.asBuffer.buffer, dataArray->value.asBuffer.length);
            data->ipAddresses[0][dataArray->value.asBuffer.length] = 0;
            result = COAP_204_CHANGED;
        }
        break;

    case RES_O_ROUTER_IP_ADDRESS:
        if (sizeof(data->routerIpAddresses[0]) <= dataArray->value.asBuffer.length)
        {
            result = COAP_400_BAD_REQUEST;
        }
        else
        {
            memset(data->routerIpAddresses[0], 0, sizeof(data->routerIpAddresses[0]));
            memcpy(data->routerIpAddresses[0], dataArray->value.asBuffer.buffer, dataArray->value.asBuffer.length);
            data->routerIpAddresses[0][dataArray->value.asBuffer.length] = 0;
            result = COAP_204_CHANGED;
        }
        break;

    case RES_O_CELL_ID:
        if (1 == lwm2m_data_decode_int(dataArray, &value))
        {
            data->cellId = value;
            result = COAP_204_CHANGED;
        }
        else
        {
            result = COAP_400_BAD_REQUEST;
        }
        break;

    default:
        result = COAP_405_METHOD_NOT_ALLOWED;
    }

    return result;
}

