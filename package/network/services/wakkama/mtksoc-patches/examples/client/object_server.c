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
 *    Julien Vermillard, Sierra Wireless
 *    Bosch Software Innovations GmbH - Please refer to git log
 *    Pascal Rieux - Please refer to git log
 *    Scott Bertin, AMETEK, Inc. - Please refer to git log
 *
 *******************************************************************************/

/*
 *  Resources:
 *
 *          Name                       | ID | Operations | Instances | Mandatory |  Type    |  Range  | Units |
 *  Short ID                           |  0 |     R      |  Single   |    Yes    | Integer  | 1-65535 |       |
 *  Lifetime                           |  1 |    R/W     |  Single   |    Yes    | Integer  |         |   s   |
 *  Default Min Period                 |  2 |    R/W     |  Single   |    No     | Integer  |         |   s   |
 *  Default Max Period                 |  3 |    R/W     |  Single   |    No     | Integer  |         |   s   |
 *  Disable                            |  4 |     E      |  Single   |    No     |          |         |       |
 *  Disable Timeout                    |  5 |    R/W     |  Single   |    No     | Integer  |         |   s   |
 *  Notification Storing               |  6 |    R/W     |  Single   |    Yes    | Boolean  |         |       |
 *  Binding                            |  7 |    R/W     |  Single   |    Yes    | String   |         |       |
 *  Registration Update                |  8 |     E      |  Single   |    Yes    |          |         |       |
#ifndef LWM2M_VERSION_1_0
 *  Registration Priority Order        | 13 |    R/W     |  Single   |    No     | Unsigned |         |       |
 *  Initial Registration Delay Timer   | 14 |    R/W     |  Single   |    No     | Unsigned |         |   s   |
 *  Registration Failure Block         | 15 |    R/W     |  Single   |    No     | Boolean  |         |       |
 *  Bootstrap on Registration Failure  | 16 |    R/W     |  Single   |    No     | Boolean  |         |       |
 *  Communication Retry Count          | 17 |    R/W     |  Single   |    No     | Unsigned |         |       |
 *  Communication Retry Timer          | 18 |    R/W     |  Single   |    No     | Unsigned |         |   s   |
 *  Communication Sequence Delay Timer | 19 |    R/W     |  Single   |    No     | Unsigned |         |   s   |
 *  Communication Sequence Retry Count | 20 |    R/W     |  Single   |    No     | Unsigned |         |       |
#endif
 *
 */

#include "liblwm2m.h"
#include "lwm2mclient.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

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

// Resource Id's:
#define RES_M_SHORT_SERVER_ID                       0
#define RES_M_LIFETIME                              1
#define RES_O_DEFAULT_MINIMUM_PERIOD                2
#define RES_O_DEFAULT_MAXIMUM_PERIOD                3
#define RES_O_DISABLE                               4
#define RES_O_DISABLE_TIMEOUT                       5
#define RES_M_NOTIFICATION_STORING_WHEN_DISABLED_OR_OFFLINE	6
#define RES_M_BINDING                   			7

bool checkServerValueChange(int resourceId, int resourceInsId/*multi resource using, single resource no effect*/, server_instance_t * targetP, char * zcfgParamValue)
{
    resource_ins_list_t * resourceIns;

    switch(resourceId)
    {
    case LWM2M_SERVER_SHORT_ID_ID:
        if(strcmp(targetP->observe_data_shortServerId, zcfgParamValue))
{
            strncpy(targetP->observe_data_shortServerId, zcfgParamValue, sizeof(targetP->observe_data_shortServerId));
            return true;
        }
        break;

    case LWM2M_SERVER_LIFETIME_ID:
        if(strcmp(targetP->observe_data_lifetime, zcfgParamValue))
        {
            strncpy(targetP->observe_data_lifetime, zcfgParamValue, sizeof(targetP->observe_data_lifetime));
            return true;
        }
        break;

    case LWM2M_SERVER_MIN_PERIOD_ID:
        if(strcmp(targetP->observe_data_defaultMinPeriod, zcfgParamValue))
        {
            strncpy(targetP->observe_data_defaultMinPeriod, zcfgParamValue, sizeof(targetP->observe_data_defaultMinPeriod));
            return true;
        }
        break;
        
    case LWM2M_SERVER_MAX_PERIOD_ID:
        if(strcmp(targetP->observe_data_defaultMaxPeriod, zcfgParamValue))
        {
            strncpy(targetP->observe_data_defaultMaxPeriod, zcfgParamValue, sizeof(targetP->observe_data_defaultMaxPeriod));
            return true;
        }
        break;
        
    case LWM2M_SERVER_TIMEOUT_ID:
        if(strcmp(targetP->observe_data_disableTimeout, zcfgParamValue))
        {
            strncpy(targetP->observe_data_disableTimeout, zcfgParamValue, sizeof(targetP->observe_data_disableTimeout));
            return true;
        }
        break;
        
    case LWM2M_SERVER_STORING_ID:
        if(strcmp(targetP->observe_data_storing, zcfgParamValue))
        {
            strncpy(targetP->observe_data_storing, zcfgParamValue, sizeof(targetP->observe_data_storing));
            return true;
        }
        break;
        
    case LWM2M_SERVER_BINDING_ID:
        if(strcmp(targetP->observe_data_binding, zcfgParamValue))
        {
            strncpy(targetP->observe_data_binding, zcfgParamValue, sizeof(targetP->observe_data_binding));
            return true;
        }
        break;

#ifndef LWM2M_VERSION_1_0
    case LWM2M_SERVER_REG_ORDER_ID:
        if(strcmp(targetP->observe_data_registrationPriorityOrder, zcfgParamValue))
        {
            strncpy(targetP->observe_data_registrationPriorityOrder, zcfgParamValue, sizeof(targetP->observe_data_registrationPriorityOrder));
            return true;
        }
        break;
        
    case LWM2M_SERVER_INITIAL_REG_DELAY_ID:
        if(strcmp(targetP->observe_data_initialRegistrationDelayTimer, zcfgParamValue))
        {
            strncpy(targetP->observe_data_initialRegistrationDelayTimer, zcfgParamValue, sizeof(targetP->observe_data_initialRegistrationDelayTimer));
            return true;
        }
        break;
        
    case LWM2M_SERVER_REG_FAIL_BLOCK_ID:
        if(strcmp(targetP->observe_data_registrationFailureBlock, zcfgParamValue))
        {
            strncpy(targetP->observe_data_registrationFailureBlock, zcfgParamValue, sizeof(targetP->observe_data_registrationFailureBlock));
            return true;
        }
        break;
        
    case LWM2M_SERVER_REG_FAIL_BOOTSTRAP_ID:
        if(strcmp(targetP->observe_data_bootstrapOnRegistrationFailure, zcfgParamValue))
        {
            strncpy(targetP->observe_data_bootstrapOnRegistrationFailure, zcfgParamValue, sizeof(targetP->observe_data_bootstrapOnRegistrationFailure));
            return true;
        }
        break;
        
    case LWM2M_SERVER_COMM_RETRY_COUNT_ID:
        if(strcmp(targetP->observe_data_communicationRetryCount, zcfgParamValue))
        {
            strncpy(targetP->observe_data_communicationRetryCount, zcfgParamValue, sizeof(targetP->observe_data_communicationRetryCount));
            return true;
        }
        break;
        
    case LWM2M_SERVER_COMM_RETRY_TIMER_ID:
        if(strcmp(targetP->observe_data_communicationRetryTimer, zcfgParamValue))
        {
            strncpy(targetP->observe_data_communicationRetryTimer, zcfgParamValue, sizeof(targetP->observe_data_communicationRetryTimer));
            return true;
        }
        break;
        
    case LWM2M_SERVER_SEQ_DELAY_TIMER_ID:
        if(strcmp(targetP->observe_data_communicationSequenceDelayTimer, zcfgParamValue))
        {
            strncpy(targetP->observe_data_communicationSequenceDelayTimer, zcfgParamValue, sizeof(targetP->observe_data_communicationSequenceDelayTimer));
            return true;
        }
        break;
        
    case LWM2M_SERVER_SEQ_RETRY_COUNT_ID:
        if(strcmp(targetP->observe_data_communicationSequenceRetryCount, zcfgParamValue))
        {
            strncpy(targetP->observe_data_communicationSequenceRetryCount, zcfgParamValue, sizeof(targetP->observe_data_communicationSequenceRetryCount));
            return true;
        }
        break;
#endif
        
    default:
        break;
    }
    return false;
}

static uint8_t prv_server_delete(lwm2m_context_t *contextP,
                                 uint16_t id,
                                 lwm2m_object_t * objectP);
static uint8_t prv_server_create(lwm2m_context_t *contextP,
                                 uint16_t instanceId,
                                 int numData,
                                 lwm2m_data_t * dataArray,
                                 lwm2m_object_t * objectP);

static uint8_t prv_get_value(lwm2m_data_t * dataP,
                             server_instance_t * targetP)
{
    switch (dataP->id)
    {
    case LWM2M_SERVER_SHORT_ID_ID:
        lwm2m_data_encode_int(targetP->shortServerId, dataP);
        return COAP_205_CONTENT;

    case LWM2M_SERVER_LIFETIME_ID:
        lwm2m_data_encode_int(targetP->lifetime, dataP);
        return COAP_205_CONTENT;

    case LWM2M_SERVER_MIN_PERIOD_ID:
        lwm2m_data_encode_int(targetP->defaultMinPeriod, dataP);
        return COAP_205_CONTENT;

    case LWM2M_SERVER_MAX_PERIOD_ID:
        lwm2m_data_encode_int(targetP->defaultMaxPeriod, dataP);
        return COAP_205_CONTENT;

    case LWM2M_SERVER_DISABLE_ID:
        return COAP_405_METHOD_NOT_ALLOWED;

    case LWM2M_SERVER_TIMEOUT_ID:
        lwm2m_data_encode_int(targetP->disableTimeout, dataP);
        return COAP_205_CONTENT;

    case LWM2M_SERVER_STORING_ID:
        lwm2m_data_encode_bool(targetP->storing, dataP);
        return COAP_205_CONTENT;

    case LWM2M_SERVER_BINDING_ID:
        lwm2m_data_encode_string(targetP->binding, dataP);
        return COAP_205_CONTENT;

    case LWM2M_SERVER_UPDATE_ID:
        return COAP_405_METHOD_NOT_ALLOWED;

#ifndef LWM2M_VERSION_1_0
    case LWM2M_SERVER_REG_ORDER_ID:
        if (targetP->registrationPriorityOrder >= 0)
        {
            lwm2m_data_encode_uint(targetP->registrationPriorityOrder, dataP);
            return COAP_205_CONTENT;
        }
        else
        {
            return COAP_404_NOT_FOUND;
        }

    case LWM2M_SERVER_INITIAL_REG_DELAY_ID:
        if (targetP->initialRegistrationDelayTimer >= 0)
        {
            lwm2m_data_encode_uint(targetP->initialRegistrationDelayTimer, dataP);
            return COAP_205_CONTENT;
        }
        else
        {
            return COAP_404_NOT_FOUND;
        }

    case LWM2M_SERVER_REG_FAIL_BLOCK_ID:
        if (targetP->registrationFailureBlock >= 0)
        {
            lwm2m_data_encode_bool(targetP->registrationFailureBlock > 0, dataP);
            return COAP_205_CONTENT;
        }
        else
        {
            return COAP_404_NOT_FOUND;
        }

    case LWM2M_SERVER_REG_FAIL_BOOTSTRAP_ID:
        if (targetP->bootstrapOnRegistrationFailure >= 0)
        {
            lwm2m_data_encode_bool(targetP->bootstrapOnRegistrationFailure > 0, dataP);
            return COAP_205_CONTENT;
        }
        else
        {
            return COAP_404_NOT_FOUND;
        }

    case LWM2M_SERVER_COMM_RETRY_COUNT_ID:
        if (targetP->communicationRetryCount >= 0)
        {
            lwm2m_data_encode_uint(targetP->communicationRetryCount, dataP);
            return COAP_205_CONTENT;
        }
        else
        {
            return COAP_404_NOT_FOUND;
        }

    case LWM2M_SERVER_COMM_RETRY_TIMER_ID:
        if (targetP->communicationRetryTimer >= 0)
        {
            lwm2m_data_encode_uint(targetP->communicationRetryTimer, dataP);
            return COAP_205_CONTENT;
        }
        else
        {
            return COAP_404_NOT_FOUND;
        }

    case LWM2M_SERVER_SEQ_DELAY_TIMER_ID:
        if (targetP->communicationSequenceDelayTimer >= 0)
        {
            lwm2m_data_encode_uint(targetP->communicationSequenceDelayTimer, dataP);
            return COAP_205_CONTENT;
        }
        else
        {
            return COAP_404_NOT_FOUND;
        }

    case LWM2M_SERVER_SEQ_RETRY_COUNT_ID:
        if (targetP->communicationSequenceRetryCount >= 0)
        {
            lwm2m_data_encode_uint(targetP->communicationSequenceRetryCount, dataP);
            return COAP_205_CONTENT;
        }
        else
        {
            return COAP_404_NOT_FOUND;
        }

#endif

    default:
        return COAP_404_NOT_FOUND;
    }
}

static uint8_t prv_server_read(lwm2m_context_t *contextP,
                               uint16_t instanceId,
                               int * numDataP,
                               lwm2m_data_t ** dataArrayP,
                               lwm2m_object_t * objectP)
{
    server_instance_t * targetP;
    uint8_t result;
    int i;

    int ret = ZCFG_SUCCESS;
    char omaID[16] = {0};
    char desValue[512] = {0};

    /* unused parameter */
    (void)contextP;

    targetP = (server_instance_t *)lwm2m_list_find(objectP->instanceList, instanceId);
    if (NULL == targetP) return COAP_404_NOT_FOUND;

    // is the server asking for the full instance ?
    if (*numDataP == 0)
    {
        uint16_t resList[] = {
            LWM2M_SERVER_SHORT_ID_ID,
            LWM2M_SERVER_LIFETIME_ID,
            LWM2M_SERVER_MIN_PERIOD_ID,
            LWM2M_SERVER_MAX_PERIOD_ID,
            LWM2M_SERVER_TIMEOUT_ID,
            LWM2M_SERVER_STORING_ID,
            LWM2M_SERVER_BINDING_ID,
#ifndef LWM2M_VERSION_1_0
            LWM2M_SERVER_REG_ORDER_ID,
            LWM2M_SERVER_INITIAL_REG_DELAY_ID,
            LWM2M_SERVER_REG_FAIL_BLOCK_ID,
            LWM2M_SERVER_REG_FAIL_BOOTSTRAP_ID,
            LWM2M_SERVER_COMM_RETRY_COUNT_ID,
            LWM2M_SERVER_COMM_RETRY_TIMER_ID,
            LWM2M_SERVER_SEQ_DELAY_TIMER_ID,
            LWM2M_SERVER_SEQ_RETRY_COUNT_ID,
#endif
        };
        int nbRes = sizeof(resList)/sizeof(uint16_t);

#ifndef LWM2M_VERSION_1_0
        /* Remove optional resources that don't exist */
        if(targetP->registrationPriorityOrder < 0)
        {
            for (i=0; i < nbRes; i++)
            {
                if (resList[i] == LWM2M_SERVER_REG_ORDER_ID)
                {
                    nbRes -= 1;
                    memmove(&resList[i], &resList[i+1], (nbRes-i)*sizeof(resList[i]));
                    break;
                }
            }
        }
        if(targetP->initialRegistrationDelayTimer < 0)
        {
            for (i=0; i < nbRes; i++)
            {
                if (resList[i] == LWM2M_SERVER_INITIAL_REG_DELAY_ID)
                {
                    nbRes -= 1;
                    memmove(&resList[i], &resList[i+1], (nbRes-i)*sizeof(resList[i]));
                    break;
                }
            }
        }
        if(targetP->registrationFailureBlock < 0)
        {
            for (i=0; i < nbRes; i++)
            {
                if (resList[i] == LWM2M_SERVER_REG_FAIL_BLOCK_ID)
                {
                    nbRes -= 1;
                    memmove(&resList[i], &resList[i+1], (nbRes-i)*sizeof(resList[i]));
                    break;
                }
            }
        }
        if(targetP->bootstrapOnRegistrationFailure < 0)
        {
            for (i=0; i < nbRes; i++)
            {
                if (resList[i] == LWM2M_SERVER_REG_FAIL_BOOTSTRAP_ID)
                {
                    nbRes -= 1;
                    memmove(&resList[i], &resList[i+1], (nbRes-i)*sizeof(resList[i]));
                    break;
                }
            }
        }
        if(targetP->communicationRetryCount < 0)
        {
            for (i=0; i < nbRes; i++)
            {
                if (resList[i] == LWM2M_SERVER_COMM_RETRY_COUNT_ID)
                {
                    nbRes -= 1;
                    memmove(&resList[i], &resList[i+1], (nbRes-i)*sizeof(resList[i]));
                    break;
                }
            }
        }
        if(targetP->communicationRetryTimer < 0)
        {
            for (i=0; i < nbRes; i++)
            {
                if (resList[i] == LWM2M_SERVER_COMM_RETRY_TIMER_ID)
                {
                    nbRes -= 1;
                    memmove(&resList[i], &resList[i+1], (nbRes-i)*sizeof(resList[i]));
                    break;
                }
            }
        }
        if(targetP->communicationSequenceDelayTimer < 0)
        {
            for (i=0; i < nbRes; i++)
            {
                if (resList[i] == LWM2M_SERVER_SEQ_DELAY_TIMER_ID)
                {
                    nbRes -= 1;
                    memmove(&resList[i], &resList[i+1], (nbRes-i)*sizeof(resList[i]));
                    break;
                }
            }
        }
        if(targetP->communicationSequenceRetryCount < 0)
        {
            for (i=0; i < nbRes; i++)
            {
                if (resList[i] == LWM2M_SERVER_SEQ_RETRY_COUNT_ID)
                {
                    nbRes -= 1;
                    memmove(&resList[i], &resList[i+1], (nbRes-i)*sizeof(resList[i]));
                    break;
                }
            }
        }
#endif

        *dataArrayP = lwm2m_data_new(nbRes);
        if (*dataArrayP == NULL) return COAP_500_INTERNAL_SERVER_ERROR;
        *numDataP = nbRes;
        for (i = 0 ; i < nbRes ; i++)
        {
            (*dataArrayP)[i].id = resList[i];
        }
    }

    i = 0;
    do
    {

#if 0 // original wakkama code
        if ((*dataArrayP)[i].type == LWM2M_TYPE_MULTIPLE_RESOURCE)
        {
            result = COAP_404_NOT_FOUND;
        }
        else
        {
            result = prv_get_value((*dataArrayP) + i, targetP);
        }
#else
        snprintf(omaID, 16, "%d_%d", objectP->objID, (*dataArrayP)[i].id);

        switch ((*dataArrayP)[i].id)
        {
            // Integer
            case LWM2M_SERVER_SHORT_ID_ID:
            case LWM2M_SERVER_LIFETIME_ID:
            case LWM2M_SERVER_MIN_PERIOD_ID:
            case LWM2M_SERVER_MAX_PERIOD_ID:
            case LWM2M_SERVER_TIMEOUT_ID:
            {
                if ((*dataArrayP)[i].type == LWM2M_TYPE_MULTIPLE_RESOURCE) return COAP_404_NOT_FOUND;

                if((ret = prv_get_zcfg_value(omaID, instanceId, -1, desValue)) != ZCFG_SUCCESS)
                {
                    printf("[%s] prv_get_zcfg_value fail, ret = %d!!!\n", __FUNCTION__, ret);
                }

                if(checkServerValueChange((*dataArrayP)[i].id, -1, targetP, desValue))
                {
                    setChangeValueList(objectP->objID, instanceId,  (*dataArrayP)[i].id,  -1);
                }

				lwm2m_data_encode_int(atoi(desValue), *dataArrayP + i);
                result = COAP_205_CONTENT;
                break;
            }

            // Boolean
            case LWM2M_SERVER_STORING_ID:
            {
                if ((*dataArrayP)[i].type == LWM2M_TYPE_MULTIPLE_RESOURCE) return COAP_404_NOT_FOUND;

                if((ret = prv_get_zcfg_value(omaID, instanceId, -1, desValue)) != ZCFG_SUCCESS)
                {
                    printf("[%s]prv_get_zcfg_value fail, ret = %d!!!\n", __FUNCTION__, ret);
                }

                if(checkServerValueChange((*dataArrayP)[i].id, -1, targetP, desValue))
                {
                    setChangeValueList(objectP->objID, instanceId,  (*dataArrayP)[i].id,  -1);
                }

				lwm2m_data_encode_bool((atoi(desValue)) ? true : false, *dataArrayP + i);
                result = COAP_205_CONTENT;
                break;
            }

            // String
            case LWM2M_SERVER_BINDING_ID:
            {
                if ((*dataArrayP)[i].type == LWM2M_TYPE_MULTIPLE_RESOURCE) return COAP_404_NOT_FOUND;
				if((ret = prv_get_zcfg_value(omaID, instanceId, -1, desValue)) != ZCFG_SUCCESS)
				{
					printf("[%s]prv_get_zcfg_value fail, ret = %d!!!\n", __FUNCTION__, ret);
				}

                if(checkServerValueChange((*dataArrayP)[i].id, -1, targetP, desValue))
                {
                    setChangeValueList(objectP->objID, instanceId,  (*dataArrayP)[i].id,  -1);
                }

				lwm2m_data_encode_string(desValue, *dataArrayP + i);
				result = COAP_205_CONTENT;
				break;
            }

            default:
                result = COAP_404_NOT_FOUND;
        }
#endif

        i++;
    } while (i < *numDataP && result == COAP_205_CONTENT);

    return result;
}

static uint8_t prv_server_discover(lwm2m_context_t *contextP,
                                   uint16_t instanceId,
                                   int * numDataP,
                                   lwm2m_data_t ** dataArrayP,
                                   lwm2m_object_t * objectP)
{
    server_instance_t * targetP;
    uint8_t result;
    int i;

    /* unused parameter */
    (void)contextP;

    result = COAP_205_CONTENT;

    targetP = (server_instance_t *)lwm2m_list_find(objectP->instanceList, instanceId);
    if (NULL == targetP) return COAP_404_NOT_FOUND;

    // is the server asking for the full object ?
    if (*numDataP == 0)
    {
        uint16_t resList[] = {
            LWM2M_SERVER_SHORT_ID_ID,
            LWM2M_SERVER_LIFETIME_ID,
            LWM2M_SERVER_MIN_PERIOD_ID,
            LWM2M_SERVER_MAX_PERIOD_ID,
            LWM2M_SERVER_DISABLE_ID,
            LWM2M_SERVER_TIMEOUT_ID,
            LWM2M_SERVER_STORING_ID,
            LWM2M_SERVER_BINDING_ID,
            LWM2M_SERVER_UPDATE_ID,
#ifndef LWM2M_VERSION_1_0
            LWM2M_SERVER_REG_ORDER_ID,
            LWM2M_SERVER_INITIAL_REG_DELAY_ID,
            LWM2M_SERVER_REG_FAIL_BLOCK_ID,
            LWM2M_SERVER_REG_FAIL_BOOTSTRAP_ID,
            LWM2M_SERVER_COMM_RETRY_COUNT_ID,
            LWM2M_SERVER_COMM_RETRY_TIMER_ID,
            LWM2M_SERVER_SEQ_DELAY_TIMER_ID,
            LWM2M_SERVER_SEQ_RETRY_COUNT_ID,
#endif
        };
        int nbRes = sizeof(resList) / sizeof(uint16_t);

#ifndef LWM2M_VERSION_1_0
        /* Remove optional resources that don't exist */
        if(targetP->registrationPriorityOrder < 0)
        {
            for (i=0; i < nbRes; i++)
            {
                if (resList[i] == LWM2M_SERVER_REG_ORDER_ID)
                {
                    nbRes -= 1;
                    memmove(&resList[i], &resList[i+1], (nbRes-i)*sizeof(resList[i]));
                    break;
                }
            }
        }
        if(targetP->initialRegistrationDelayTimer < 0)
        {
            for (i=0; i < nbRes; i++)
            {
                if (resList[i] == LWM2M_SERVER_INITIAL_REG_DELAY_ID)
                {
                    nbRes -= 1;
                    memmove(&resList[i], &resList[i+1], (nbRes-i)*sizeof(resList[i]));
                    break;
                }
            }
        }
        if(targetP->registrationFailureBlock < 0)
        {
            for (i=0; i < nbRes; i++)
            {
                if (resList[i] == LWM2M_SERVER_REG_FAIL_BLOCK_ID)
                {
                    nbRes -= 1;
                    memmove(&resList[i], &resList[i+1], (nbRes-i)*sizeof(resList[i]));
                    break;
                }
            }
        }
        if(targetP->bootstrapOnRegistrationFailure < 0)
        {
            for (i=0; i < nbRes; i++)
            {
                if (resList[i] == LWM2M_SERVER_REG_FAIL_BOOTSTRAP_ID)
                {
                    nbRes -= 1;
                    memmove(&resList[i], &resList[i+1], (nbRes-i)*sizeof(resList[i]));
                    break;
                }
            }
        }
        if(targetP->communicationRetryCount < 0)
        {
            for (i=0; i < nbRes; i++)
            {
                if (resList[i] == LWM2M_SERVER_COMM_RETRY_COUNT_ID)
                {
                    nbRes -= 1;
                    memmove(&resList[i], &resList[i+1], (nbRes-i)*sizeof(resList[i]));
                    break;
                }
            }
        }
        if(targetP->communicationRetryTimer < 0)
        {
            for (i=0; i < nbRes; i++)
            {
                if (resList[i] == LWM2M_SERVER_COMM_RETRY_TIMER_ID)
                {
                    nbRes -= 1;
                    memmove(&resList[i], &resList[i+1], (nbRes-i)*sizeof(resList[i]));
                    break;
                }
            }
        }
        if(targetP->communicationSequenceDelayTimer < 0)
        {
            for (i=0; i < nbRes; i++)
            {
                if (resList[i] == LWM2M_SERVER_SEQ_DELAY_TIMER_ID)
                {
                    nbRes -= 1;
                    memmove(&resList[i], &resList[i+1], (nbRes-i)*sizeof(resList[i]));
                    break;
                }
            }
        }
        if(targetP->communicationSequenceRetryCount < 0)
        {
            for (i=0; i < nbRes; i++)
            {
                if (resList[i] == LWM2M_SERVER_SEQ_RETRY_COUNT_ID)
                {
                    nbRes -= 1;
                    memmove(&resList[i], &resList[i+1], (nbRes-i)*sizeof(resList[i]));
                    break;
                }
            }
        }
#endif

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
            case LWM2M_SERVER_SHORT_ID_ID:
            case LWM2M_SERVER_LIFETIME_ID:
            case LWM2M_SERVER_MIN_PERIOD_ID:
            case LWM2M_SERVER_MAX_PERIOD_ID:
            case LWM2M_SERVER_DISABLE_ID:
            case LWM2M_SERVER_TIMEOUT_ID:
            case LWM2M_SERVER_STORING_ID:
            case LWM2M_SERVER_BINDING_ID:
            case LWM2M_SERVER_UPDATE_ID:
                break;
#ifndef LWM2M_VERSION_1_0
            case LWM2M_SERVER_REG_ORDER_ID:
                if(targetP->registrationPriorityOrder < 0)
                {
                    result = COAP_404_NOT_FOUND;
                }
                break;

            case LWM2M_SERVER_INITIAL_REG_DELAY_ID:
                if(targetP->initialRegistrationDelayTimer < 0)
                {
                    result = COAP_404_NOT_FOUND;
                }
                break;

            case LWM2M_SERVER_REG_FAIL_BLOCK_ID:
                if(targetP->registrationFailureBlock < 0)
                {
                    result = COAP_404_NOT_FOUND;
                }
                break;

            case LWM2M_SERVER_REG_FAIL_BOOTSTRAP_ID:
                if(targetP->bootstrapOnRegistrationFailure < 0)
                {
                    result = COAP_404_NOT_FOUND;
                }
                break;

            case LWM2M_SERVER_COMM_RETRY_COUNT_ID:
                if(targetP->communicationRetryCount < 0)
                {
                    result = COAP_404_NOT_FOUND;
                }
                break;

            case LWM2M_SERVER_COMM_RETRY_TIMER_ID:
                if(targetP->communicationRetryTimer < 0)
                {
                    result = COAP_404_NOT_FOUND;
                }
                break;

            case LWM2M_SERVER_SEQ_DELAY_TIMER_ID:
                if(targetP->communicationSequenceDelayTimer < 0)
                {
                    result = COAP_404_NOT_FOUND;
                }
                break;

            case LWM2M_SERVER_SEQ_RETRY_COUNT_ID:
                if(targetP->communicationSequenceRetryCount < 0)
                {
                    result = COAP_404_NOT_FOUND;
                }
                break;
#endif

            default:
                result = COAP_404_NOT_FOUND;
                break;
            }
        }
    }

    return result;
}

static uint8_t prv_set_int_value(lwm2m_data_t * dataArray, uint32_t * data) {
    uint8_t result;
    int64_t value;

    if (1 == lwm2m_data_decode_int(dataArray, &value))
    {
        if (value >= 0 && value <= 0xFFFFFFFF)
        {
            *data = value;
            result = COAP_204_CHANGED;
        }
        else
        {
            result = COAP_406_NOT_ACCEPTABLE;
        }
    }
    else
    {
        result = COAP_400_BAD_REQUEST;
    }
    return result;
}

static uint8_t prv_server_write(lwm2m_context_t *contextP,
                                uint16_t instanceId,
                                int numData,
                                lwm2m_data_t * dataArray,
                                lwm2m_object_t * objectP,
                                lwm2m_write_type_t writeType)
{
    server_instance_t * targetP;
    int i;
    uint8_t result;

    char param[512] = {0};
    char omaID[16] = {0};
    char valstr[32] = {0};
    int64_t iValue;
    uint64_t uValue;
    bool bValue = false;

    targetP = (server_instance_t *)lwm2m_list_find(objectP->instanceList, instanceId);
    if (NULL == targetP)
    {
        return COAP_404_NOT_FOUND;
    }

    if (writeType == LWM2M_WRITE_REPLACE_INSTANCE)
    {
        result = prv_server_delete(contextP, instanceId, objectP);
        if (result == COAP_202_DELETED)
        {
            result = prv_server_create(contextP, instanceId, numData, dataArray, objectP);
            if (result == COAP_201_CREATED)
            {
                result = COAP_204_CHANGED;
            }
        }
        return result;
    }

    i = 0;
    do
    {
#if 1 // original wakkama code
        /* No multiple instance resources */
        if (dataArray[i].type == LWM2M_TYPE_MULTIPLE_RESOURCE)
        {
            result = COAP_404_NOT_FOUND;
            continue;
        }

        switch (dataArray[i].id)
        {
        case LWM2M_SERVER_SHORT_ID_ID:
            {
                uint32_t value = targetP->shortServerId;
                result = prv_set_int_value(dataArray + i, &value);
                if (COAP_204_CHANGED == result)
                {
                    if (0 < value && 0xFFFF >= value)
                    {
                        targetP->shortServerId = value;
                    }
                    else
                    {
                        result = COAP_406_NOT_ACCEPTABLE;
                    }
                }
            }
            break;

        case LWM2M_SERVER_LIFETIME_ID:
            result = prv_set_int_value(dataArray + i, (uint32_t *)&(targetP->lifetime));
            break;

        case LWM2M_SERVER_MIN_PERIOD_ID:
            result = prv_set_int_value(dataArray + i, &(targetP->defaultMinPeriod));
            break;

        case LWM2M_SERVER_MAX_PERIOD_ID:
            result = prv_set_int_value(dataArray + i, &(targetP->defaultMaxPeriod));
            break;

        case LWM2M_SERVER_DISABLE_ID:
            result = COAP_405_METHOD_NOT_ALLOWED;
            break;

        case LWM2M_SERVER_TIMEOUT_ID:
            result = prv_set_int_value(dataArray + i, &(targetP->disableTimeout));
            break;

        case LWM2M_SERVER_STORING_ID:
        {
            bool value;

            if (1 == lwm2m_data_decode_bool(dataArray + i, &value))
            {
                targetP->storing = value;
                result = COAP_204_CHANGED;
            }
            else
            {
                result = COAP_400_BAD_REQUEST;
            }
        }
        break;

        case LWM2M_SERVER_BINDING_ID:
            if ((dataArray[i].type == LWM2M_TYPE_STRING || dataArray[i].type == LWM2M_TYPE_OPAQUE)
             && dataArray[i].value.asBuffer.length > 0 && dataArray[i].value.asBuffer.length <= 3
#ifdef LWM2M_VERSION_1_0
             && (strncmp((char*)dataArray[i].value.asBuffer.buffer, "U", dataArray[i].value.asBuffer.length) == 0
              || strncmp((char*)dataArray[i].value.asBuffer.buffer, "UQ", dataArray[i].value.asBuffer.length) == 0
              || strncmp((char*)dataArray[i].value.asBuffer.buffer, "S", dataArray[i].value.asBuffer.length) == 0
              || strncmp((char*)dataArray[i].value.asBuffer.buffer, "SQ", dataArray[i].value.asBuffer.length) == 0
              || strncmp((char*)dataArray[i].value.asBuffer.buffer, "US", dataArray[i].value.asBuffer.length) == 0
              || strncmp((char*)dataArray[i].value.asBuffer.buffer, "UQS", dataArray[i].value.asBuffer.length) == 0)
#endif
               )
            {
                strncpy(targetP->binding, (char*)dataArray[i].value.asBuffer.buffer, dataArray[i].value.asBuffer.length);
                result = COAP_204_CHANGED;
            }
            else
            {
                result = COAP_400_BAD_REQUEST;
            }
            break;

        case LWM2M_SERVER_UPDATE_ID:
            result = COAP_405_METHOD_NOT_ALLOWED;
            break;

#ifndef LWM2M_VERSION_1_0
        case LWM2M_SERVER_REG_ORDER_ID:
        {
            uint64_t value;
            if (1 == lwm2m_data_decode_uint(dataArray + i, &value))
            {
                if (value <= INT_MAX)
                {
                    targetP->registrationPriorityOrder = value;
                    result = COAP_204_CHANGED;
                }
                else
                {
                    result = COAP_406_NOT_ACCEPTABLE;
                }
            }
            else
            {
                result = COAP_400_BAD_REQUEST;
            }
            break;
        }

        case LWM2M_SERVER_INITIAL_REG_DELAY_ID:
        {
            uint64_t value;
            if (1 == lwm2m_data_decode_uint(dataArray + i, &value))
            {
                if (value <= INT_MAX)
                {
                    targetP->initialRegistrationDelayTimer = value;
                    result = COAP_204_CHANGED;
                }
                else
                {
                    result = COAP_406_NOT_ACCEPTABLE;
                }
            }
            else
            {
                result = COAP_400_BAD_REQUEST;
            }
            break;
        }

        case LWM2M_SERVER_REG_FAIL_BLOCK_ID:
        {
            bool value;
            if (1 == lwm2m_data_decode_bool(dataArray + i, &value))
            {
                targetP->registrationFailureBlock = value;
                result = COAP_204_CHANGED;
            }
            else
            {
                result = COAP_400_BAD_REQUEST;
            }
            break;
        }

        case LWM2M_SERVER_REG_FAIL_BOOTSTRAP_ID:
        {
            bool value;
            if (1 == lwm2m_data_decode_bool(dataArray + i, &value))
            {
                targetP->bootstrapOnRegistrationFailure = value;
                result = COAP_204_CHANGED;
            }
            else
            {
                result = COAP_400_BAD_REQUEST;
            }
            break;
        }

        case LWM2M_SERVER_COMM_RETRY_COUNT_ID:
        {
            uint64_t value;
            if (1 == lwm2m_data_decode_uint(dataArray + i, &value))
            {
                if (value <= INT_MAX)
                {
                    targetP->communicationRetryCount = value;
                    result = COAP_204_CHANGED;
                }
                else
                {
                    result = COAP_406_NOT_ACCEPTABLE;
                }
            }
            else
            {
                result = COAP_400_BAD_REQUEST;
            }
            break;
        }

        case LWM2M_SERVER_COMM_RETRY_TIMER_ID:
        {
            uint64_t value;
            if (1 == lwm2m_data_decode_uint(dataArray + i, &value))
            {
                if (value <= INT_MAX)
                {
                    targetP->communicationRetryTimer = value;
                    result = COAP_204_CHANGED;
                }
                else
                {
                    result = COAP_406_NOT_ACCEPTABLE;
                }
            }
            else
            {
                result = COAP_400_BAD_REQUEST;
            }
            break;
        }

        case LWM2M_SERVER_SEQ_DELAY_TIMER_ID:
        {
            uint64_t value;
            if (1 == lwm2m_data_decode_uint(dataArray + i, &value))
            {
                if (value <= INT_MAX)
                {
                    targetP->communicationSequenceDelayTimer = value;
                    result = COAP_204_CHANGED;
                }
                else
                {
                    result = COAP_406_NOT_ACCEPTABLE;
                }
            }
            else
            {
                result = COAP_400_BAD_REQUEST;
            }
            break;
        }

        case LWM2M_SERVER_SEQ_RETRY_COUNT_ID:
        {
            uint64_t value;
            if (1 == lwm2m_data_decode_uint(dataArray + i, &value))
            {
                if (value <= INT_MAX)
                {
                    targetP->communicationSequenceRetryCount = value;
                    result = COAP_204_CHANGED;
                }
                else
                {
                    result = COAP_406_NOT_ACCEPTABLE;
                }
            }
            else
            {
                result = COAP_400_BAD_REQUEST;
            }
            break;
        }
#endif

        default:
            return COAP_404_NOT_FOUND;
        }
//#else
        memset(param, 0, sizeof(param));
        snprintf(omaID, 16, "%d_%d", objectP->objID, (dataArray)[i].id);

        switch (dataArray[i].id)
        {
            // Interger
            case LWM2M_SERVER_SHORT_ID_ID:
            case LWM2M_SERVER_LIFETIME_ID:
            case LWM2M_SERVER_MIN_PERIOD_ID:
            case LWM2M_SERVER_MAX_PERIOD_ID:
            case LWM2M_SERVER_TIMEOUT_ID:
            {
                lwm2m_data_decode_int(&dataArray[i], &iValue);

                sprintf(param, "%d", iValue);

                if(prv_set_zcfg_value(omaID, instanceId, -1, LWM2M_TYPE_INTEGER, param) == ZCFG_SUCCESS) // FIXME:dataArray[i].type  is LWM2M_TYPE_OPAQUE,but this case type should be LWM2M_TYPE_INTEGER, I dont know why
                {
                    result = COAP_204_CHANGED;
                }
                else
                {
                    result = COAP_400_BAD_REQUEST;
                }
                break;
            }

            // Boolean
            case LWM2M_SERVER_STORING_ID:
            {
                lwm2m_data_decode_bool(&dataArray[i], &bValue);

                sprintf(param, "%d", (bValue == true) ? 1 : 0);

                if(prv_set_zcfg_value(omaID, instanceId, -1, LWM2M_TYPE_BOOLEAN, param) == ZCFG_SUCCESS) // FIXME:dataArray[i].type  is LWM2M_TYPE_OPAQUE but this case type should be LWM2M_TYPE_BOOLEAN, I dont know why
                {
                    result = COAP_204_CHANGED;
                }
                else
                {
                    result = COAP_400_BAD_REQUEST;
                }
                break;
            }

            // String
            case LWM2M_SERVER_BINDING_ID:
            {
                strncpy(param, dataArray[i].value.asBuffer.buffer, dataArray[i].value.asBuffer.length);

                if(prv_set_zcfg_value(omaID, instanceId, -1, dataArray[i].type, param) == ZCFG_SUCCESS)
                {
                    result = COAP_204_CHANGED;
                }
                else
                {
                    result = COAP_400_BAD_REQUEST;
                }
                break;
            }

            default:
                result = COAP_405_METHOD_NOT_ALLOWED;
        }
#endif
        i++;
    } while (i < numData && result == COAP_204_CHANGED);

    return result;
}

static uint8_t prv_server_execute(lwm2m_context_t *contextP,
                                  uint16_t instanceId,
                                  uint16_t resourceId,
                                  uint8_t * buffer,
                                  int length,
                                  lwm2m_object_t * objectP)

{
    server_instance_t * targetP;

    /* unused parameter */
    //(void)contextP;
	int resultUpdate;
    targetP = (server_instance_t *)lwm2m_list_find(objectP->instanceList, instanceId);
    if (NULL == targetP) return COAP_404_NOT_FOUND;

    switch (resourceId)
    {
    case LWM2M_SERVER_DISABLE_ID:
        // executed in core, if COAP_204_CHANGED is returned
        if (0 < targetP->disableTimeout) return COAP_204_CHANGED;
        else return COAP_405_METHOD_NOT_ALLOWED;
    case LWM2M_SERVER_UPDATE_ID:
        // executed in core, if COAP_204_CHANGED is returned
		resultUpdate = lwm2m_update_registration(contextP, targetP->shortServerId, true);
		if (resultUpdate != 0)
		{
			fprintf(stdout, "Registration update error: ");
			print_status(stdout, resultUpdate);
			fprintf(stdout, "\r\n");
		}
        return COAP_204_CHANGED;

    default:
        return COAP_405_METHOD_NOT_ALLOWED;
    }
}

static uint8_t prv_server_delete(lwm2m_context_t *contextP,
                                 uint16_t id,
                                 lwm2m_object_t * objectP)
{
    server_instance_t * serverInstance;

    /* unused parameter */
    (void)contextP;

    objectP->instanceList = lwm2m_list_remove(objectP->instanceList, id, (lwm2m_list_t **)&serverInstance);
    if (NULL == serverInstance) return COAP_404_NOT_FOUND;

    lwm2m_free(serverInstance);

    return COAP_202_DELETED;
}

static uint8_t prv_server_create(lwm2m_context_t *contextP,
                                 uint16_t instanceId,
                                 int numData,
                                 lwm2m_data_t * dataArray,
                                 lwm2m_object_t * objectP)
{

    server_instance_t * serverInstance;
    uint8_t result;
    char omaID[16] = {0};

    serverInstance = (server_instance_t *)lwm2m_malloc(sizeof(server_instance_t));
    if (NULL == serverInstance) return COAP_500_INTERNAL_SERVER_ERROR;
    memset(serverInstance, 0, sizeof(server_instance_t));

    serverInstance->instanceId = instanceId;
    memset(serverInstance->observe_data_shortServerId, 0, 257);
    memset(serverInstance->observe_data_lifetime, 0, 257);
    memset(serverInstance->observe_data_defaultMinPeriod, 0, 257);
    memset(serverInstance->observe_data_defaultMaxPeriod, 0, 257);
    memset(serverInstance->observe_data_disableTimeout, 0, 257);
    memset(serverInstance->observe_data_storing, 0, 257);
    memset(serverInstance->observe_data_binding, 0, 257);
#ifndef LWM2M_VERSION_1_0
    memset(serverInstance->observe_data_registrationPriorityOrder, 0, 257);
    memset(serverInstance->observe_data_initialRegistrationDelayTimer, 0, 257);
    memset(serverInstance->observe_data_registrationFailureBlock, 0, 257);
    memset(serverInstance->observe_data_bootstrapOnRegistrationFailure, 0, 257);
    memset(serverInstance->observe_data_communicationRetryCount, 0, 257);
    memset(serverInstance->observe_data_communicationRetryTimer, 0, 257);
    memset(serverInstance->observe_data_communicationSequenceDelayTimer, 0, 257);
    memset(serverInstance->observe_data_communicationSequenceRetryCount, 0, 257);
#endif
    objectP->instanceList = LWM2M_LIST_ADD(objectP->instanceList, serverInstance);

    snprintf(omaID, 16, "%d", objectP->objID);
    prv_add_zcfg_object(omaID, instanceId);


    result = prv_server_write(contextP, instanceId, numData, dataArray, objectP, LWM2M_WRITE_REPLACE_RESOURCES);

    if (result != COAP_204_CHANGED)
    {
        (void)prv_server_delete(contextP, instanceId, objectP);
    }
    else
    {
        result = COAP_201_CREATED;
    }

    return result;
}

void copy_server_object(lwm2m_object_t * objectDest, lwm2m_object_t * objectSrc)
{
    memcpy(objectDest, objectSrc, sizeof(lwm2m_object_t));
    objectDest->instanceList = NULL;
    objectDest->userData = NULL;
    server_instance_t * instanceSrc = (server_instance_t *)objectSrc->instanceList;
    server_instance_t * previousInstanceDest = NULL;
    while (instanceSrc != NULL)
    {
        server_instance_t * instanceDest = (server_instance_t *)lwm2m_malloc(sizeof(server_instance_t));
        if (NULL == instanceDest)
        {
            return;
        }
        memcpy(instanceDest, instanceSrc, sizeof(server_instance_t));
        // not sure it's necessary:
        strcpy(instanceDest->binding, instanceSrc->binding);
        instanceSrc = (server_instance_t *)instanceSrc->next;
        if (previousInstanceDest == NULL)
        {
            objectDest->instanceList = (lwm2m_list_t *)instanceDest;
        }
        else
        {
            previousInstanceDest->next = instanceDest;
        }
        previousInstanceDest = instanceDest;
    }
}

void display_server_object(lwm2m_object_t * object)
{
    fprintf(stdout, "  /%u: Server object, instances:\r\n", object->objID);
    server_instance_t * serverInstance = (server_instance_t *)object->instanceList;
    while (serverInstance != NULL)
    {
        fprintf(stdout, "    /%u/%u: instanceId: %u, shortServerId: %u, lifetime: %u, storing: %s, binding: %s",
                object->objID, serverInstance->instanceId,
                serverInstance->instanceId, serverInstance->shortServerId, serverInstance->lifetime,
                serverInstance->storing ? "true" : "false", serverInstance->binding);
#ifndef LWM2M_VERSION_1_0
        if(serverInstance->registrationPriorityOrder >= 0)
            fprintf(stdout, ", registrationPriorityOrder: %d", serverInstance->registrationPriorityOrder);
        if(serverInstance->initialRegistrationDelayTimer >= 0)
            fprintf(stdout, ", initialRegistrationDelayTimer: %d", serverInstance->initialRegistrationDelayTimer);
        if(serverInstance->registrationFailureBlock >= 0)
            fprintf(stdout, ", registrationFailureBlock: %s",
                    serverInstance->registrationFailureBlock > 0 ? "true" : "false");
        if(serverInstance->bootstrapOnRegistrationFailure >= 0)
            fprintf(stdout, ", bootstrapOnRegistrationFaulure: %s",
                    serverInstance->bootstrapOnRegistrationFailure > 0 ? "true" : "false");
        if(serverInstance->communicationRetryCount >= 0)
            fprintf(stdout, ", communicationRetryCount: %d", serverInstance->communicationRetryCount);
        if(serverInstance->communicationRetryTimer >= 0)
            fprintf(stdout, ", communicationRetryTimer: %d", serverInstance->communicationRetryTimer);
        if(serverInstance->communicationSequenceDelayTimer >= 0)
            fprintf(stdout, ", communicationSequenceDelayTimer: %d", serverInstance->communicationSequenceDelayTimer);
        if(serverInstance->communicationSequenceRetryCount >= 0)
            fprintf(stdout, ", communicationSequenceRetryCount: %d", serverInstance->communicationSequenceRetryCount);
#endif
        fprintf(stdout, "\r\n");
        serverInstance = (server_instance_t *)serverInstance->next;
    }
}

lwm2m_object_t * get_server_object(int serverId,
                                   const char* binding,
                                   int lifetime,
                                   bool storing)
{
    lwm2m_object_t * serverObj;
    char omaID[16] = {0};
    int newCount = 0;
    objIndex_t objIid;
    rdm_Lwm2mServer_t *lwm2mServerObj = NULL;
    uint32_t oid;
	int instanceCount = 0;//Jessie is targetP->instanceId exposed to external interface?

    serverObj = (lwm2m_object_t *)lwm2m_malloc(sizeof(lwm2m_object_t));

    if (NULL != serverObj)
    {
        memset(serverObj, 0, sizeof(lwm2m_object_t));

        serverObj->objID = LWM2M_SERVER_OBJECT_ID;
#ifndef LWM2M_VERSION_1_0
        // Not required, but useful for testing.
        serverObj->versionMajor = 1;
        serverObj->versionMinor = 1;
#endif

#if 1
        IID_INIT(objIid);
        while(zcfgFeObjStructGetNext(RDM_OID_LWM2M_SERVER, &objIid, (void **)&lwm2mServerObj) == ZCFG_SUCCESS)
        {
            newCount++;
			#if 1
            printf("YuChih [%s:%d] newCount = %d \n", __func__, __LINE__, newCount);

            printf("YuChih [%s:%d] lwm2mServerObj->Lifetime_1 = %d\n", __func__, __LINE__, lwm2mServerObj->Lifetime_1);
            printf("YuChih [%s:%d] lwm2mServerObj->Notification_Storing_When_Disabled_or_Offline_6 = %d\n", __func__, __LINE__, lwm2mServerObj->Notification_Storing_When_Disabled_or_Offline_6);
            printf("YuChih [%s:%d] lwm2mServerObj->Binding_Preference_7 = %s\n", __func__, __LINE__, lwm2mServerObj->Binding_Preference_7);
			#endif
            
			server_instance_t * serverInstance;

            serverInstance = (server_instance_t *)lwm2m_malloc(sizeof(server_instance_t));
            if (NULL == serverInstance)
            {
                lwm2m_free(serverObj);
                return NULL;
            }

            memset(serverInstance, 0, sizeof(server_instance_t));
            serverInstance->instanceId = instanceCount;//Jessie
			instanceCount++;//Jessie
            serverInstance->shortServerId = lwm2mServerObj->Short_Server_ID_0;
            serverInstance->lifetime = lwm2mServerObj->Lifetime_1;
            serverInstance->storing = lwm2mServerObj->Notification_Storing_When_Disabled_or_Offline_6;
            memcpy(serverInstance->binding, lwm2mServerObj->Binding_Preference_7, strlen(lwm2mServerObj->Binding_Preference_7)+1);

#ifndef LWM2M_VERSION_1_0
            serverInstance->registrationPriorityOrder = -1;
            serverInstance->initialRegistrationDelayTimer = -1;
            serverInstance->registrationFailureBlock = -1;
            serverInstance->bootstrapOnRegistrationFailure = -1;
            serverInstance->communicationRetryCount = -1;
            serverInstance->communicationRetryTimer = -1;
            serverInstance->communicationSequenceDelayTimer = -1;
            serverInstance->communicationSequenceRetryCount = -1;
#endif
            memset(serverInstance->observe_data_shortServerId, 0, 257);
            memset(serverInstance->observe_data_lifetime, 0, 257);
            memset(serverInstance->observe_data_defaultMinPeriod, 0, 257);
            memset(serverInstance->observe_data_defaultMaxPeriod, 0, 257);
            memset(serverInstance->observe_data_disableTimeout, 0, 257);
            memset(serverInstance->observe_data_storing, 0, 257);
            memset(serverInstance->observe_data_binding, 0, 257);
#ifndef LWM2M_VERSION_1_0
            memset(serverInstance->observe_data_registrationPriorityOrder, 0, 257);
            memset(serverInstance->observe_data_initialRegistrationDelayTimer, 0, 257);
            memset(serverInstance->observe_data_registrationFailureBlock, 0, 257);
            memset(serverInstance->observe_data_bootstrapOnRegistrationFailure, 0, 257);
            memset(serverInstance->observe_data_communicationRetryCount, 0, 257);
            memset(serverInstance->observe_data_communicationRetryTimer, 0, 257);
            memset(serverInstance->observe_data_communicationSequenceDelayTimer, 0, 257);
            memset(serverInstance->observe_data_communicationSequenceRetryCount, 0, 257);
#endif
            serverObj->instanceList = LWM2M_LIST_ADD(serverObj->instanceList, serverInstance);
			
            zcfgFeObjStructFree(lwm2mServerObj);
        }

        printf("[%s:%d] update object %d instance to %d\n", __func__, __LINE__, serverObj->objID, newCount);
#endif

        serverObj->readFunc = prv_server_read;
        serverObj->discoverFunc = prv_server_discover;
        serverObj->writeFunc = prv_server_write;
        serverObj->createFunc = prv_server_create;
        serverObj->deleteFunc = prv_server_delete;
        serverObj->executeFunc = prv_server_execute;
    }

    return serverObj;
}

void clean_server_object(lwm2m_object_t * object)
{
    while (object->instanceList != NULL)
    {
        server_instance_t * serverInstance = (server_instance_t *)object->instanceList;
        object->instanceList = object->instanceList->next;
        lwm2m_free(serverInstance);
    }
}
