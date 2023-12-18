/*******************************************************************************
 *
 * Copyright (c) 2014 Bosch Software Innovations GmbH, Germany.
 *
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
 ******************************************************************************/
/*! \file
  LWM2M object "Location" implementation

  \author Joerg Hubschneider
*/

/*
 *  Object     |      | Multiple  |     | Description                   |
 *  Name       |  ID  | Instances |Mand.|                               |
 *-------------+------+-----------+-----+-------------------------------+
 *  Location   |   6  |    No     |  No |  see TS E.7 page 101          |
 *
 *  Resources:
 *  Name        | ID  | Oper.|Instances|Mand.|  Type   | Range | Units | Description                                                                      |
 * -------------+-----+------+---------+-----+---------+-------+-------+----------------------------------------------------------------------------------+
 *  Latitude    |  0  |  R   | Single  | Yes | Float   |       |  Deg  | The decimal notation of latitude  e.g. -  45.5723  [Worlds Geodetic System 1984].|
 *  Longitude   |  1  |  R   | Single  | Yes | Float   |       |  Deg  | The decimal notation of longitude e.g. - 153.21760 [Worlds Geodetic System 1984].|
 *  Altitude    |  2  |  R   | Single  | No  | Float   |       |   m   | The decimal notation of altitude in meters above sea level.                      |
 *  Radius      |  3  |  R   | Single  | No  | Float   |       |   m   | The value in the Radius Resource indicates the size in meters of a circular area |
 *              |     |      |         |     |         |       |       | around a point of geometry.                                                      |
 *  Velocity    |  4  |  R   | Single  | No  | Opaque  |       |   *   | The velocity of the device as defined in 3GPP 23.032 GAD specification(*).       |
 *              |     |      |         |     |         |       |       | This set of values may not be available if the device is static.                 |
 *              |     |      |         |     |         |       |       | opaque: see OMA_TS 6.3.2                                                         |
 *  Timestamp   |  5  |  R   | Single  | Yes | Time    |       |   s   | The timestamp when the location measurement was performed.                       |
 *  Speed       |  6  |  R   | Single  | No  | Float   |       |  m/s  | Speed is the time rate of change in position of a LwM2M Client without regard    |
 *              |     |      |         |     |         |       |       | for direction: the scalar component of velocity.                                 |
 */

#include "liblwm2m.h"
#include "lwm2mclient.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

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

#ifdef LWM2M_CLIENT_MODE


// ---- private "object location" specific defines ----
// Resource Id's:
#define RES_M_LATITUDE     0
#define RES_M_LONGITUDE    1
#define RES_O_ALTITUDE     2
#define RES_O_RADIUS       3
#define RES_O_VELOCITY     4
#define RES_M_TIMESTAMP    5
#define RES_O_SPEED        6

//-----  3GPP TS 23.032 V11.0.0(2012-09) ---------
#define HORIZONTAL_VELOCITY                           0  // for Octet-1 upper half(..<<4)
#define HORIZONTAL_VELOCITY_VERTICAL                  1  // set vertical direction bit!
#define HORIZONTAL_VELOCITY_WITH_UNCERTAINTY          2
#define HORIZONTAL_VELOCITY_VERTICAL_WITH_UNCERTAINTY 3

#define VELOCITY_OCTETS                               7  // for HORIZONTAL_VELOCITY_VERTICAL_WITH_UNCERTAINTY

typedef struct
{
    float    latitude;
    float    longitude;
    float    altitude;
    float    radius;
    uint8_t  velocity   [VELOCITY_OCTETS];        //3GPP notation 1st step: HORIZONTAL_VELOCITY_WITH_UNCERTAINTY
    unsigned long timestamp;
    float    speed;
} location_data_t;

bool checkLocationValueChange(int resourceId, int resourceInsId/*multi resource using, single resource no effect*/, location_instance_t * targetP, char * zcfgParamValue)
{
    resource_ins_list_t * resourceIns;

    switch(resourceId)
    {
    case RES_M_LATITUDE:
        if(strcmp(targetP->latitude, zcfgParamValue))
        {
            strncpy(targetP->latitude, zcfgParamValue, sizeof(targetP->latitude));
            return true;
        }
        break;

    case RES_M_LONGITUDE:
        if(strcmp(targetP->longitude, zcfgParamValue))
        {
            strncpy(targetP->longitude, zcfgParamValue, sizeof(targetP->longitude));
            return true;
        }
        break;

    case RES_O_ALTITUDE:
        if(strcmp(targetP->altitude, zcfgParamValue))
        {
            strncpy(targetP->altitude, zcfgParamValue, sizeof(targetP->altitude));
            return true;
        }
        break;

    case RES_O_RADIUS:
        if(strcmp(targetP->altitude, zcfgParamValue))
        {
            strncpy(targetP->altitude, zcfgParamValue, sizeof(targetP->altitude));
            return true;
        }
        break;

    case RES_O_VELOCITY:
        if(strcmp(targetP->velocity, zcfgParamValue))
        {
            strncpy(targetP->velocity, zcfgParamValue, sizeof(targetP->velocity));
            return true;
        }
        break;

    case RES_M_TIMESTAMP:
        if(strcmp(targetP->timestamp, zcfgParamValue))
        {
            strncpy(targetP->timestamp, zcfgParamValue, sizeof(targetP->timestamp));
            return true;
        }
        break;

    case RES_O_SPEED:
        if(strcmp(targetP->speed, zcfgParamValue))
        {
            strncpy(targetP->speed, zcfgParamValue, sizeof(targetP->speed));
            return true;
        }
        break;

    default:
        break;
    }
    return false;
}

/**
implementation for all read-able resources
*/
static uint8_t prv_res2tlv(lwm2m_data_t* dataP,
                           location_data_t* locDataP)
{
    //-------------------------------------------------------------------- JH --
    uint8_t ret = COAP_205_CONTENT;  
    switch (dataP->id)     // location resourceId
    {
    case RES_M_LATITUDE:
        lwm2m_data_encode_float(locDataP->latitude, dataP);
        break;
    case RES_M_LONGITUDE:
        lwm2m_data_encode_float(locDataP->longitude, dataP);
        break;
    case RES_O_ALTITUDE:
        lwm2m_data_encode_float(locDataP->altitude, dataP);
        break;
    case RES_O_RADIUS:
        lwm2m_data_encode_float(locDataP->radius, dataP);
        break;
    case RES_O_VELOCITY:
    {
        size_t length;
        switch( locDataP->velocity[0] >> 4 )
        {
           case HORIZONTAL_VELOCITY:
           {
               length = 4;
              break;
           }
           case HORIZONTAL_VELOCITY_VERTICAL:
           {
               length = 5;
              break;
           }
           case HORIZONTAL_VELOCITY_WITH_UNCERTAINTY:
           {
               length = 5;
              break;
           }
           case HORIZONTAL_VELOCITY_VERTICAL_WITH_UNCERTAINTY:
           {
               length = 7;
              break;
           }
           default:
           {
               length = 0;
               break;
           }
        }
        lwm2m_data_encode_opaque(locDataP->velocity, length, dataP);
        break;
    }
    case RES_M_TIMESTAMP:
        lwm2m_data_encode_int(locDataP->timestamp, dataP);
        break;
    case RES_O_SPEED:
        lwm2m_data_encode_float(locDataP->speed, dataP);
        break;
    default:
        ret = COAP_404_NOT_FOUND;
        break;
    }
  
    return ret;
}


/**
  * Implementation (callback-) function of reading object resources. For whole
  * object, single resources or a sequence of resources
  * see 3GPP TS 23.032 V11.0.0(2012-09) page 23,24.
  * implemented for: HORIZONTAL_VELOCITY_WITH_UNCERTAINT
  * @param contextP     in,     unused pointer to LWM2M context
  * @param objInstId    in,     instances ID of the location object to read
  * @param numDataP     in/out, pointer to the number of resource to read. 0 is the
  *                             exception for all readable resource of object instance
  * @param tlvArrayP    in/out, TLV data sequence with initialized resource ID to read
  * @param objectP      in,     private location data structure
  */
static uint8_t prv_location_read(lwm2m_context_t *contextP,
                                 uint16_t objInstId,
                                 int*  numDataP,
                                 lwm2m_data_t** tlvArrayP,
                                 lwm2m_object_t*  objectP)
{   
    //-------------------------------------------------------------------- JH --
    uint8_t result = COAP_500_INTERNAL_SERVER_ERROR;
    location_data_t* locDataP = (location_data_t*)(objectP->userData);
    int i, j;
    int ret = ZCFG_SUCCESS;
    char omaID[16] = {0};
    char desValue[512] = {0};
    lwm2m_data_t * subTlvP;
    size_t count;
    location_instance_t * targetP;

    /* unused parameter */
    (void)contextP;

    // defined as single instance object!
    if (objInstId != 0) return COAP_404_NOT_FOUND;

    targetP = (location_instance_t *)lwm2m_list_find(objectP->instanceList, objInstId);
    if (NULL == targetP) return COAP_404_NOT_FOUND;


    if (*numDataP == 0)     // full object, readable resources!
    {
        uint16_t readResIds[] = {
                RES_M_LATITUDE,
                RES_M_LONGITUDE,
                RES_O_ALTITUDE,
                RES_O_RADIUS,
                RES_O_VELOCITY,
                RES_M_TIMESTAMP,
                RES_O_SPEED
        }; // readable resources!
        
        *numDataP  = sizeof(readResIds)/sizeof(uint16_t);
        *tlvArrayP = lwm2m_data_new(*numDataP);
        if (*tlvArrayP == NULL) return COAP_500_INTERNAL_SERVER_ERROR;
        
        // init readable resource id's
        for (i = 0 ; i < *numDataP ; i++)
        {
            (*tlvArrayP)[i].id = readResIds[i];
        }
    }
    
    i = 0;
    do
    {
		memset(desValue, 0, sizeof(desValue));
        snprintf(omaID, 16, "%d_%d", objectP->objID, (*tlvArrayP)[i].id);
        switch ((*tlvArrayP)[i].id)
        {
        case RES_M_LATITUDE:
        case RES_M_LONGITUDE:
            if ((*tlvArrayP)[i].type == LWM2M_TYPE_MULTIPLE_RESOURCE) return COAP_404_NOT_FOUND;
            if((ret = prv_get_zcfg_value(omaID, -1, -1, desValue)) != ZCFG_SUCCESS)
            {
                printf("[%s]prv_get_zcfg_value fail, ret = %d!!!\n", __FUNCTION__, ret);
            }

            if(checkDeviceValueChange((*tlvArrayP)[i].id, -1, targetP, desValue))
            {
                setChangeValueList(objectP->objID, -1,  (*tlvArrayP)[i].id,  -1);
            }

            lwm2m_data_encode_float(atof(desValue), *tlvArrayP + i);
            result = COAP_205_CONTENT;
            break;
		case RES_M_TIMESTAMP:
		{
			if ((*tlvArrayP)[i].type == LWM2M_TYPE_MULTIPLE_RESOURCE) return COAP_404_NOT_FOUND;
            if((ret = prv_get_zcfg_value(omaID, -1, -1, desValue)) != ZCFG_SUCCESS)
            {
                printf("[%s]prv_get_zcfg_value fail, ret = %d!!!\n", __FUNCTION__, ret);
            }

            if(checkDeviceValueChange((*tlvArrayP)[i].id, -1, targetP, desValue))
            {
                setChangeValueList(objectP->objID, -1,  (*tlvArrayP)[i].id,  -1);
            }

            //lwm2m_data_encode_int(dateTimeToUnixTime(desValue), *tlvArrayP + i);
            lwm2m_data_encode_int(atoi(desValue), *tlvArrayP + i);
            result = COAP_205_CONTENT;
            break;
        }
        default:
            result = COAP_404_NOT_FOUND;
        }

        i++;
    } while (i < *numDataP && result == COAP_205_CONTENT);
    
    return result;
}

void display_location_object(lwm2m_object_t * object)
{
    location_data_t * data = (location_data_t *)object->userData;
    fprintf(stdout, "  /%u: Location object:\r\n", object->objID);
    if (NULL != data)
    {
        fprintf(stdout, "    latitude: %.6f, longitude: %.6f, altitude: %.6f, radius: %.6f, timestamp: %lu, speed: %.6f\r\n",
                data->latitude, data->longitude, data->altitude, data->radius, data->timestamp, data->speed);
    }
}

/**
  * Convenience function to set the velocity attributes.
  * see 3GPP TS 23.032 V11.0.0(2012-09) page 23,24.
  * implemented for: HORIZONTAL_VELOCITY_WITH_UNCERTAINTY
  * @param locationObj location object reference (to be casted!)
  * @param bearing          [Deg]  0 - 359    resolution: 1 degree
  * @param horizontalSpeed  [km/h] 1 - s^16-1 resolution: 1 km/h steps
  * @param speedUncertainty [km/h] 1-254      resolution: 1 km/h (255=undefined!)
  */
void location_setVelocity(lwm2m_object_t* locationObj,
                          uint16_t bearing,
                          uint16_t horizontalSpeed,
                          uint8_t speedUncertainty)
{
    //-------------------------------------------------------------------- JH --
    location_data_t* pData = locationObj->userData;
    pData->velocity[0] = HORIZONTAL_VELOCITY_WITH_UNCERTAINTY << 4;
    pData->velocity[0] |= (bearing & 0x100) >> 8;
    pData->velocity[1] = (bearing & 0x0FF);
    pData->velocity[2] = horizontalSpeed >> 8;
    pData->velocity[3] = horizontalSpeed & 0xff;
    pData->velocity[4] = speedUncertainty;
}

/**
  * A convenience function to set the location coordinates with its timestamp.
  * @see testMe()
  * @param locationObj location object reference (to be casted!)
  * @param latitude  the second argument.
  * @param longitude the second argument.
  * @param altitude  the second argument.
  * @param timestamp the related timestamp. Seconds since 1970.
  */
void location_setLocationAtTime(lwm2m_object_t* locationObj,
                             float latitude,
                             float longitude,
                             float altitude,
                             uint64_t timestamp)
{
    //-------------------------------------------------------------------- JH --
    location_data_t* pData = locationObj->userData;

    pData->latitude  = latitude;
    pData->longitude = longitude;
    pData->altitude  = altitude;
    pData->timestamp = timestamp;
}

/**
  * This function creates the LWM2M Location. 
  * @return gives back allocated LWM2M data object structure pointer. On error, 
  * NULL value is returned.
  */
lwm2m_object_t * get_object_location(void)
{
    //-------------------------------------------------------------------- JH --
    lwm2m_object_t * locationObj;

    locationObj = (lwm2m_object_t *)lwm2m_malloc(sizeof(lwm2m_object_t));
    if (NULL != locationObj)
    {

        location_instance_t * locationInstance;
       
        memset(locationObj, 0, sizeof(lwm2m_object_t));

        // It assigns its unique ID
        // The 6 is the standard ID for the optional object "Location".
        locationObj->objID = LWM2M_LOCATION_OBJECT_ID;
        
        // and its unique instance
        locationInstance = (location_instance_t *)lwm2m_malloc(sizeof(location_instance_t));
        if (NULL == locationInstance)
        {
            lwm2m_free(locationObj);
            return NULL;
        }

        memset(locationInstance, 0, sizeof(location_instance_t));
        locationInstance->instanceId = 0;
        memset(locationInstance->latitude, 0, 257);
        memset(locationInstance->longitude, 0, 257);
        memset(locationInstance->altitude, 0, 257);
        memset(locationInstance->radius, 0, 257);
        memset(locationInstance->velocity, 0, 257);
        memset(locationInstance->timestamp, 0, 257);
        memset(locationInstance->speed, 0, 257);
        locationObj->instanceList = LWM2M_LIST_ADD(locationObj->instanceList, locationInstance);

        // And the private function that will access the object.
        // Those function will be called when a read query is made by the server.
        // In fact the library don't need to know the resources of the object, only the server does.
        //
        locationObj->readFunc    = prv_location_read;
        locationObj->userData    = lwm2m_malloc(sizeof(location_data_t));

        // initialize private data structure containing the needed variables
        if (NULL != locationObj->userData)
        {
            location_data_t* data = (location_data_t*)locationObj->userData;
            data->latitude    = 27.986065;  // Mount Everest :)
            data->longitude   = 86.922623;
            data->altitude    = 8495.0000;
            data->radius      = 0.0;
            location_setVelocity(locationObj, 0, 0, 255); // 255: speedUncertainty not supported!
            data->timestamp   = time(NULL);
            data->speed       = 0.0;
        }
        else
        {
            lwm2m_free(locationObj);
            locationObj = NULL;
        }
    }
    
    return locationObj;
}

void free_object_location(lwm2m_object_t * object)
{
    lwm2m_list_free(object->instanceList);
    while (object->instanceList != NULL)
    {
        location_instance_t * locationInstance = (location_instance_t *)object->instanceList;
        object->instanceList = object->instanceList->next;
        lwm2m_free(locationInstance);
    }
    lwm2m_free(object);
}

#endif  //LWM2M_CLIENT_MODE
