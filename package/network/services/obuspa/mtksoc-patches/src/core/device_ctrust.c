/*
 *
 * Copyright (C) 2021, Broadband Forum
 * Copyright (C) 2017-2019  CommScope, Inc
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its
 *    contributors may be used to endorse or promote products derived from
 *    this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
 * THE POSSIBILITY OF SUCH DAMAGE.
 *
 */

/**
 * \file device_ctrust.c
 *
 * Implements the data model objects associated with controller trust
 *
 */

#include <time.h>
#include <string.h>

#include <openssl/ssl.h>
#include <openssl/bio.h>
#include <openssl/err.h>
#include <openssl/x509.h>
#include <openssl/x509v3.h>
#include <openssl/safestack.h>

#include "common_defs.h"
#include "msg_handler.h"
#include "data_model.h"
#include "usp_api.h"
#include "device.h"
#include "dm_access.h"
#include "vendor_api.h"
#include "iso8601.h"

//------------------------------------------------------------------------------
// Location of the controller trust tables within the data model
#define DEVICE_CTRUST_ROOT "Device.LocalAgent.ControllerTrust"
#define DEVICE_ROLE_ROOT "Device.LocalAgent.ControllerTrust.Role.{i}"
#define DEVICE_PERMISSION_ROOT "Device.LocalAgent.ControllerTrust.Role.{i}.Permission.{i}"
#define DEVICE_CREDENTIAL_ROOT "Device.LocalAgent.ControllerTrust.Credential.{i}"
#define DEVICE_CHALLENGE_ROOT "Device.LocalAgent.ControllerTrust.Challenge.{i}"

//------------------------------------------------------------------------------
// Structure for Permission table
typedef struct
{
    char *targets;
    unsigned permission_bitmask;
} permission_t;

//------------------------------------------------------------------------------
// Structure for Role table
typedef struct
{
    char *name;
    int num_permissions;
    permission_t *permissions;
} role_t;

// Array containing data about each role. It is indexed by the role enumeration. ie role table instance number = role enumeration +1
static role_t roles[kCTrustRole_Max];

//------------------------------------------------------------------------------
// Structure for Credential table
typedef struct
{
    ctrust_role_t role;
    int cert_instance;      // instance number of the certificate in Device.LocalAgent.Certificate.{i} table
} credential_t;

//------------------------------------------------------------------------------
// Variable containing the count of request challenge messages
unsigned request_challenge_count = 0;

//------------------------------------------------------------------------------------
// RequestChallenge() command parameters
static char *request_challenge_input_args[] =
{
    "ChallengeRef",
    "RequestExpiration",
};

static char *request_challenge_output_args[] =
{
    "Instruction",
    "InstructionType",
    "ValueType",
    "ChallengeID",
};

//------------------------------------------------------------------------------------
// ChallengeResponse() command parameters
static char *challenge_response_input_args[] =
{
    "ChallengeID",
    "Value",
};

//------------------------------------------------------------------------------------
// Controller Challenge structure
typedef struct
{
    char *controller_endpoint_id; // endpoint id of the controller that executed the command
                                  // it's used to match a challenge with the controller that sent the command

    char *challenge_id;           // generated challenge id - ChallengeID ouput parameter from RequestChallenge() command
                                  // it's used to match the response with the request

    int expiration;               // RequestExpiration input parameter from RequestChallenge() command
                                  // number of seconds the request is going to expire

    time_t expire_time;           // calculated expiration time using RequestExpiration parameter
                                  // defines when a challenge is going to expire

    char *challenge_ref;          // ChallengeRef input parameter from RequestChallenge() command
                                  // it's a reference to the data model Challenge table

    unsigned retries;             // number of times that the password provided by the controller was wrong
    time_t locked_time;           // calculated lockout time when the number of retries exceeded
} controller_challenge_t;

static controller_challenge_t controller_challenges[MAX_CONTROLLERS];

//------------------------------------------------------------------------------
// Challenge mechanism constants that are currently supported
#define CHALLENGE_TYPE "Passphrase"
#define CHALLENGE_VALUE_TYPE "text/plain"
#define CHALLENGE_INSTRUCTION_TYPE "text/plain"

#define DEFAULT_REQUEST_EXPIRATION 900 // in seconds

// Vector containing credential table entries
static int num_credentials = 0;
static credential_t *credentials = NULL;

//------------------------------------------------------------------------------
// Forward declarations. Note these are not static, because we need them in the symbol table for USP_LOG_Callstack() to show them
int Get_RoleNumEntries(dm_req_t *req, char *buf, int len);
int Get_CredentialNumEntries(dm_req_t *req, char *buf, int len);
int Get_RoleName(dm_req_t *req, char *buf, int len);
int Get_PermissionNumEntries(dm_req_t *req, char *buf, int len);
int Get_PermissionOrder(dm_req_t *req, char *buf, int len);
int Get_PermissionTargets(dm_req_t *req, char *buf, int len);
int Get_ParamPermissions(dm_req_t *req, char *buf, int len);
int Get_ObjPermissions(dm_req_t *req, char *buf, int len);
int Get_InstantiatedObjPermissions(dm_req_t *req, char *buf, int len);
int Get_CommandEventPermissions(dm_req_t *req, char *buf, int len);
int Get_CredentialRole(dm_req_t *req, char *buf, int len);
int Get_CredentialCertificate(dm_req_t *req, char *buf, int len);
role_t *CalcRoleFromReq(dm_req_t *req);
permission_t *CalcPermissionFromReq(dm_req_t *req);
credential_t *CalcCredentialFromReq(dm_req_t *req);

// Controller challenge forward declarations
void DestroyControllerChallenge(controller_challenge_t *controller_challenge);
controller_challenge_t *FindAvailableControllerChallenge(char *controller_endpoint_id);
controller_challenge_t *FindControllerChallengeByEndpointId(char *controller_endpoint_id);

// RequestChallenge forward declarations
int ControllerTrustRequestChallenge(dm_req_t *req, char *command_key, kv_vector_t *input_args, kv_vector_t *output_args);
char *GenerateChallengeId(char *challenge_id, int len);
int Validate_ChallengeRole(dm_req_t *req, char *value);
int Validate_ChallengeType(dm_req_t *req, char *value);
int Validate_ChallengeValueType(dm_req_t *req, char *value);
int Validate_ChallengeInstructionType(dm_req_t *req, char *value);

// ChallengeResponse forward declarations
int ControllerTrustChallengeResponse(dm_req_t *req, char *command_key, kv_vector_t *input_args, kv_vector_t *output_args);

/*********************************************************************//**
**
** DEVICE_CTRUST_Init
**
** Initialises this component, and registers all parameters which it implements
**
** \param   None
**
** \return  USP_ERR_OK if successful
**
**************************************************************************/
int DEVICE_CTRUST_Init(void)
{
    int err = USP_ERR_OK;

    memset(roles, 0, sizeof(roles));
    memset(controller_challenges, 0, sizeof(controller_challenges));

    // Register parameters implemented by this component
    // Device.LocalAgent.ControllerTrust
    // err |= USP_REGISTER_VendorParam_ReadOnly(DEVICE_CTRUST_ROOT ".RoleNumberOfEntries", Get_RoleNumEntries, DM_UINT);
    // err |= USP_REGISTER_VendorParam_ReadOnly(DEVICE_CTRUST_ROOT ".CredentialNumberOfEntries", Get_CredentialNumEntries, DM_UINT);


    // Device.LocalAgent.ControllerTrust.Role.{i}
    // err |= USP_REGISTER_Object(DEVICE_ROLE_ROOT, USP_HOOK_DenyAddInstance, NULL, NULL,   // This table is read only
    //                                              USP_HOOK_DenyDeleteInstance, NULL, NULL);
    // err |= USP_REGISTER_VendorParam_ReadOnly(DEVICE_ROLE_ROOT ".Alias", DM_ACCESS_PopulateAliasParam, DM_STRING);
    // err |= USP_REGISTER_Param_Constant(DEVICE_ROLE_ROOT ".Enable", "true", DM_BOOL);
    // err |= USP_REGISTER_VendorParam_ReadOnly(DEVICE_ROLE_ROOT ".Name", Get_RoleName, DM_STRING);
    // err |= USP_REGISTER_VendorParam_ReadOnly(DEVICE_ROLE_ROOT ".PermissionNumberOfEntries", Get_PermissionNumEntries, DM_UINT);


    // Device.LocalAgent.ControllerTrust.Role.{i}.Permission.{i}
    // err |= USP_REGISTER_Object(DEVICE_PERMISSION_ROOT, USP_HOOK_DenyAddInstance, NULL, NULL,   // This table is read only
    //                                                    USP_HOOK_DenyDeleteInstance, NULL, NULL);

    // err |= USP_REGISTER_VendorParam_ReadOnly(DEVICE_PERMISSION_ROOT ".Alias", DM_ACCESS_PopulateAliasParam, DM_STRING);
    // err |= USP_REGISTER_Param_Constant(DEVICE_PERMISSION_ROOT ".Enable", "true", DM_BOOL);
    // err |= USP_REGISTER_VendorParam_ReadOnly(DEVICE_PERMISSION_ROOT ".Order", Get_PermissionOrder, DM_UINT);
    // err |= USP_REGISTER_VendorParam_ReadOnly(DEVICE_PERMISSION_ROOT ".Targets", Get_PermissionTargets, DM_STRING);
    // err |= USP_REGISTER_VendorParam_ReadOnly(DEVICE_PERMISSION_ROOT ".Param", Get_ParamPermissions, DM_STRING);
    // err |= USP_REGISTER_VendorParam_ReadOnly(DEVICE_PERMISSION_ROOT ".Obj", Get_ObjPermissions, DM_STRING);
    // err |= USP_REGISTER_VendorParam_ReadOnly(DEVICE_PERMISSION_ROOT ".InstantiatedObj", Get_InstantiatedObjPermissions, DM_STRING);
    // err |= USP_REGISTER_VendorParam_ReadOnly(DEVICE_PERMISSION_ROOT ".CommandEvent", Get_CommandEventPermissions, DM_STRING);

    // Device.LocalAgent.ControllerTrust.Credential.{i}
    // err |= USP_REGISTER_Object(DEVICE_CREDENTIAL_ROOT, USP_HOOK_DenyAddInstance, NULL, NULL,   // This table is read only
    //                                                    USP_HOOK_DenyDeleteInstance, NULL, NULL);

    // err |= USP_REGISTER_VendorParam_ReadOnly(DEVICE_CREDENTIAL_ROOT ".Alias", DM_ACCESS_PopulateAliasParam, DM_STRING);
    // err |= USP_REGISTER_Param_Constant(DEVICE_CREDENTIAL_ROOT ".Enable", "true", DM_BOOL);
    // err |= USP_REGISTER_VendorParam_ReadOnly(DEVICE_CREDENTIAL_ROOT ".Role", Get_CredentialRole, DM_STRING);
    // err |= USP_REGISTER_VendorParam_ReadOnly(DEVICE_CREDENTIAL_ROOT ".Credential", Get_CredentialCertificate, DM_STRING);

    // Device.LocalAgent.ControllerTrust.Challenge.{i}
    err |= USP_REGISTER_Object(DEVICE_CHALLENGE_ROOT, NULL, NULL, NULL, NULL, NULL, NULL);
    err |= USP_REGISTER_DBParam_Alias(DEVICE_CHALLENGE_ROOT ".Alias", NULL);
    err |= USP_REGISTER_DBParam_ReadWrite(DEVICE_CHALLENGE_ROOT ".Description", "", NULL, NULL, DM_STRING);
    err |= USP_REGISTER_DBParam_ReadWrite(DEVICE_CHALLENGE_ROOT ".Role", "", Validate_ChallengeRole, NULL, DM_STRING);
    err |= USP_REGISTER_DBParam_ReadWrite(DEVICE_CHALLENGE_ROOT ".Enable", "false", NULL, NULL, DM_BOOL);
    err |= USP_REGISTER_DBParam_ReadWrite(DEVICE_CHALLENGE_ROOT ".Type", CHALLENGE_TYPE, Validate_ChallengeType, NULL, DM_STRING);
    err |= USP_REGISTER_DBParam_Secure(DEVICE_CHALLENGE_ROOT ".Value", "", NULL, NULL);
    err |= USP_REGISTER_DBParam_ReadWrite(DEVICE_CHALLENGE_ROOT ".ValueType", CHALLENGE_VALUE_TYPE, Validate_ChallengeValueType, NULL, DM_STRING);
    err |= USP_REGISTER_DBParam_ReadWrite(DEVICE_CHALLENGE_ROOT ".Instruction", "", NULL, NULL, DM_STRING);
    err |= USP_REGISTER_DBParam_ReadWrite(DEVICE_CHALLENGE_ROOT ".InstructionType", CHALLENGE_INSTRUCTION_TYPE, Validate_ChallengeInstructionType, NULL, DM_STRING);
    err |= USP_REGISTER_DBParam_ReadWrite(DEVICE_CHALLENGE_ROOT ".Retries", "", NULL, NULL, DM_UINT);
    err |= USP_REGISTER_DBParam_ReadWrite(DEVICE_CHALLENGE_ROOT ".LockoutPeriod", "30", NULL, NULL, DM_INT);
    err |= USP_REGISTER_Param_NumEntries(DEVICE_CTRUST_ROOT ".ChallengeNumberOfEntries", DEVICE_CHALLENGE_ROOT);

    // Device.LocalAgent.ControllerTrust.RequestChallenge() command
    err |= USP_REGISTER_SyncOperation(DEVICE_CTRUST_ROOT ".RequestChallenge()", ControllerTrustRequestChallenge);
    err |= USP_REGISTER_OperationArguments(DEVICE_CTRUST_ROOT ".RequestChallenge()",
                        request_challenge_input_args, NUM_ELEM(request_challenge_input_args),
                        request_challenge_output_args, NUM_ELEM(request_challenge_output_args));

    // Device.LocalAgent.ControllerTrust.ChallengeResponse() command
    err |= USP_REGISTER_SyncOperation(DEVICE_CTRUST_ROOT ".ChallengeResponse()", ControllerTrustChallengeResponse);
    err |= USP_REGISTER_OperationArguments(DEVICE_CTRUST_ROOT ".ChallengeResponse()",
                        challenge_response_input_args, NUM_ELEM(challenge_response_input_args),
                        NULL, 0);

    // Register unique keys for tables
    // char *alias_unique_key[] = { "Alias" };
    // char *name_unique_key[]  = { "Name" };
    // char *order_unique_key[]  = { "Order" };
    // char *cred_unique_key[]  = { "Credential" };

    // err |= USP_REGISTER_Object_UniqueKey(DEVICE_ROLE_ROOT, alias_unique_key, NUM_ELEM(alias_unique_key));
    // err |= USP_REGISTER_Object_UniqueKey(DEVICE_ROLE_ROOT, name_unique_key, NUM_ELEM(name_unique_key));

    // err |= USP_REGISTER_Object_UniqueKey(DEVICE_PERMISSION_ROOT, alias_unique_key, NUM_ELEM(alias_unique_key));
    // err |= USP_REGISTER_Object_UniqueKey(DEVICE_PERMISSION_ROOT, order_unique_key, NUM_ELEM(order_unique_key));

    // err |= USP_REGISTER_Object_UniqueKey(DEVICE_CREDENTIAL_ROOT, alias_unique_key, NUM_ELEM(alias_unique_key));
    // err |= USP_REGISTER_Object_UniqueKey(DEVICE_CREDENTIAL_ROOT,  cred_unique_key, NUM_ELEM(cred_unique_key));

    // Exit if any errors occurred
    if (err != USP_ERR_OK)
    {
        return USP_ERR_INTERNAL_ERROR;
    }

    // If the code gets here, then registration was successful
    return USP_ERR_OK;
}

/*********************************************************************//**
**
** DEVICE_CTRUST_Start
**
** Starts this component, adding all instances to the data model
**
** \param   None
**
** \return  USP_ERR_OK if successful
**
**************************************************************************/
int DEVICE_CTRUST_Start(void)
{
    // int i;
    // int err;
    // char path[MAX_DM_PATH];

    // // Inform all role table instances to the data model

    // for (i=0; i<kCTrustRole_Max; i++)
    // {
    //     // Exit if unable to add role instance into the data model
    //     USP_SNPRINTF(path, sizeof(path), "Device.LocalAgent.ControllerTrust.Role.%d", i+1);
    //     err = DATA_MODEL_InformInstance(path);
    //     if (err != USP_ERR_OK)
    //     {
    //         return err;
    //     }
    // }

    // Reset Device.LocalAgent.ControllerTrust.Credential
    int i;
    int err;
    int_vector_t iv;
    int ct_credential_instance;
    char path[MAX_DM_PATH];
    const char device_ct_credential_root[] = "Device.LocalAgent.ControllerTrust.Credential";


    // Exit if unable to get the object instance numbers present in the credential table
    INT_VECTOR_Init(&iv);
    err = DATA_MODEL_GetInstances(device_ct_credential_root, &iv);
    if (err != USP_ERR_OK)
    {
        goto exit;
    }

    // Exit, issuing a warning, if no credential are present in database
    if (iv.num_entries == 0)
    {
        USP_LOG_Warning("%s: WARNING: No instances in %s", __FUNCTION__, device_ct_credential_root);
        err = USP_ERR_OK;
        goto exit;
    }

    // Delete all credential from the credential table
    for (i=0; i < iv.num_entries; i++)
    {
        ct_credential_instance = iv.vector[i];
        USP_SNPRINTF(path, sizeof(path), "%s.%d", device_ct_credential_root, ct_credential_instance);
        // USP_LOG_Warning("%s: Deleting %s.", __FUNCTION__, path);
        err = DATA_MODEL_DeleteInstance(path, 0);
        if (err != USP_ERR_OK)
        {
            goto exit;
        }
    }

    err = USP_ERR_OK;

exit:
    // Destroy the vector of instance numbers for the table
    INT_VECTOR_Destroy(&iv);
    return err;
}

/*********************************************************************//**
**
** DEVICE_CTRUST_Stop
**
** Frees all memory used by this component
**
** \param   None
**
** \return  USP_ERR_OK if successful
**
**************************************************************************/
void DEVICE_CTRUST_Stop(void)
{
    int i, j;
    role_t *rp;
    permission_t *pp;
    int_vector_t iv;


    INT_VECTOR_Init(&iv);
    if (DATA_MODEL_GetInstances(DEVICE_ROLE_ROOT, &iv) != USP_ERR_OK)
    {
		USP_LOG_Error("%s: ERROR: DATA_MODEL_GetInstances is Fail!!!", __FUNCTION__);
        return;
    }


    // Iterate over all roles, freeing memory
    for (i=0; i<iv.num_entries; i++)
    {
        // Free all permissions for this role
        rp = &roles[i];
        for (j=0; j < rp->num_permissions; j++)
        {
            pp = &rp->permissions[j];
            USP_SAFE_FREE(pp->targets);
        }
        USP_SAFE_FREE(rp->permissions);
        USP_SAFE_FREE(rp->name);
    }

    // Free all credentials
    USP_SAFE_FREE(credentials);
}

/*********************************************************************//**
**
** DEVICE_CTRUST_AddCertRole
**
** Adds a reference to a certificate and its associated role
** This function is called at startup when the Trust Store certificates are registered
**
** \param   cert_instance - instance number of the certificate in Device.LocalAgent.Certificate.{i} table
** \param   role - role associated with the certificate
**
** \return  USP_ERR_OK if successful
**
**************************************************************************/
int DEVICE_CTRUST_AddCertRole(int cert_instance, ctrust_role_t role)
{
    int err;
    int new_num_entries;
    credential_t *cp;
    char path[MAX_DM_PATH];

    // First increase the size of the vector
    new_num_entries = num_credentials + 1;
    credentials = USP_REALLOC(credentials, new_num_entries*sizeof(credential_t));

    // Fill in the new entry
    cp = &credentials[ num_credentials ];
    cp->role = role;
    cp->cert_instance = cert_instance;
    num_credentials = new_num_entries;

    // Exit if unable to add credential instance into the data model
    USP_SNPRINTF(path, sizeof(path), "Device.LocalAgent.ControllerTrust.Credential.%d", num_credentials);
    err = DATA_MODEL_InformInstance(path);
    if (err != USP_ERR_OK)
    {
        return err;
    }

    return USP_ERR_OK;
}

/*********************************************************************//**
**
** DEVICE_CTRUST_GetCertRole
**
** Gets the role associated with the specified certificate
**
** \param   cert_instance - Instance number of certificate in Device.LocalAgent.Certificate.{i}
**                          that we want to find the registered role for
**
** \return  Role associated with the certificate, or INVALID_ROLE, if no matching role found
**
**************************************************************************/
ctrust_role_t DEVICE_CTRUST_GetCertRole(int cert_instance)
{
    int i;
    credential_t *cp;

    // Iterate over all entries in the Credentials table
    for (i=0; i<num_credentials; i++)
    {
        // Exit if we've found a matching certificate
        cp = &credentials[i];
        if (cp->cert_instance == cert_instance)
        {
            return cp->role;
        }
    }

    // If the code gets here, then no match was found
    // NOTE: This should never happen, as we ensure that all certificates in the trust store have an associated role
    return INVALID_ROLE;
}

/*********************************************************************//**
**
** DEVICE_CTRUST_GetInstanceFromRole
**
** Gets the instance number of the specified role in Device.LocalAgent.ControllerTrust.Role.{i}
** This is very simple because the way the code works, there is a direct mapping between
** the role enumeration and it's instance number
**
** \param   role - role to get the instance number of
**
** \return  instance number of the specified role in the Device.LocalAgent.ControllerTrust.Role table, or INVALID if not found
**
**************************************************************************/
int DEVICE_CTRUST_GetInstanceFromRole(ctrust_role_t role)
{
    // Exit if role enumeration is out of bounds
    // NOTE: This may happen if a device has been assigned an INVALID_ROLE
    if (((int)role < 0) || (role >= kCTrustRole_Max))
    {
        return INVALID;
    }

    return (int)role + 1;
}

/*********************************************************************//**
**
** DEVICE_CTRUST_GetRoleFromInstance
**
** Gets the role of the specified instance number in Device.LocalAgent.ControllerTrust.Role.{i}
** This is very simple because the way the code works, there is a direct mapping between
** the role enumeration and it's instance number
**
** \param   instance - instance number in Device.LocalAgent.ControllerTrust.Role.{i} to get the role of
**
** \return  role of the specified instance or INVALID_ROLE if the instance number was invalid
**
**************************************************************************/
ctrust_role_t DEVICE_CTRUST_GetRoleFromInstance(int instance)
{
    instance--;

    // Exit if role enumeration is out of bounds
    // NOTE: This may happen if a device has been assigned an INVALID_ROLE
    if ((instance < 0) || (instance >= kCTrustRole_Max))
    {
        return INVALID_ROLE;
    }

    return (ctrust_role_t) instance;
}

/*********************************************************************//**
**
** DEVICE_CTRUST_AddPermissions
**
** Adds a permission entry to the specified role in Device.LocalAgent.ControllerTrust.Role.{i}.Permission.{i} table
**
** \param   role - role to which we want to add a permission
** \param   path_expr - search expression representing the data model nodes which are affected by the permission
** \param   permission_bitmask - bitmask of permissions to apply to the data model nodes, for the specified role
**
** \return  USP_ERR_OK if successful
**
**************************************************************************/
int DEVICE_CTRUST_AddPermissions(ctrust_role_t role, char *path_expr, unsigned short permission_bitmask)
{
    int err;
    int new_num_entries;
    role_t *rp;
    permission_t *pp;
    char path[MAX_DM_PATH];

    // Determine which role to add permissions to
    USP_ASSERT(role < kCTrustRole_Max);
    rp = &roles[role];

    // Increase the size of the permissions vector for this role
    new_num_entries = rp->num_permissions + 1;
    rp->permissions = USP_REALLOC(rp->permissions, new_num_entries*sizeof(permission_t));

    // Fill in the new entry
    pp = &rp->permissions[ rp->num_permissions ];
    pp->targets = USP_STRDUP(path_expr);
    pp->permission_bitmask = permission_bitmask;
    rp->num_permissions = new_num_entries;

    // Exit if unable to add permission instance into the data model
    USP_SNPRINTF(path, sizeof(path), "Device.LocalAgent.ControllerTrust.Role.%d.Permission.%d", role+1, rp->num_permissions);
    err = DATA_MODEL_InformInstance(path);
    if (err != USP_ERR_OK)
    {
        return err;
    }

    return USP_ERR_OK;
}

/*********************************************************************//**
**
** DEVICE_CTRUST_RegisterRoleName
**
** Sets the name of a role
**
** \param   role - role to which we want to assign a name
** \param   name - new name of the role
**
** \return  USP_ERR_OK if successful
**
**************************************************************************/
void DEVICE_CTRUST_RegisterRoleName(ctrust_role_t role, char *name)
{
    role_t *rp;

    // Free the current name (if one exists)
    USP_ASSERT(role < kCTrustRole_Max);
    rp = &roles[role];
    if (rp->name != NULL)
    {
        USP_FREE(rp->name);
    }

    // Set the new name
    rp->name = USP_STRDUP(name);
}

/*********************************************************************//**
**
** Get_RoleNumEntries
**
** Gets the value of Device.LocalAgent.ControllerTrust.RoleNumberOfEntries
**
** \param   req - pointer to structure identifying the parameter
** \param   buf - pointer to buffer into which to return the value of the parameter (as a textual string)
** \param   len - length of buffer in which to return the value of the parameter
**
** \return  USP_ERR_OK if successful
**
**************************************************************************/
int Get_RoleNumEntries(dm_req_t *req, char *buf, int len)
{
    int_vector_t iv;

    INT_VECTOR_Init(&iv);
    if (DATA_MODEL_GetInstances(DEVICE_ROLE_ROOT, &iv) != USP_ERR_OK)
    {
		USP_LOG_Error("%s: ERROR: DATA_MODEL_GetInstances is Fail!!!", __FUNCTION__);
        return;
    }

    val_uint = iv.num_entries;
    return USP_ERR_OK;
}

/*********************************************************************//**
**
** Get_CredentialNumEntries
**
** Gets the value of Device.LocalAgent.ControllerTrust.CredentialNumberOfEntries
**
** \param   req - pointer to structure identifying the parameter
** \param   buf - pointer to buffer into which to return the value of the parameter (as a textual string)
** \param   len - length of buffer in which to return the value of the parameter
**
** \return  USP_ERR_OK if successful
**
**************************************************************************/
int Get_CredentialNumEntries(dm_req_t *req, char *buf, int len)
{
    val_uint = num_credentials;
    return USP_ERR_OK;
}

/*********************************************************************//**
**
** Get_RoleName
**
** Gets the value of Device.LocalAgent.ControllerTrust.Role.{i}.Name
**
** \param   req - pointer to structure identifying the parameter
** \param   buf - pointer to buffer into which to return the value of the parameter (as a textual string)
** \param   len - length of buffer in which to return the value of the parameter
**
** \return  USP_ERR_OK if successful
**
**************************************************************************/
int Get_RoleName(dm_req_t *req, char *buf, int len)
{
    role_t *rp;

    // Copy the name of the role, if one has been set
    rp = CalcRoleFromReq(req);
    if (rp->name != NULL)
    {
        USP_STRNCPY(buf, rp->name, len);
    }
    else
    {
        // This is the default value, if the vendor has not set a name for this role
        *buf = '\0';
    }

    return USP_ERR_OK;
}

/*********************************************************************//**
**
** Get_PermissionNumEntries
**
** Gets the value of Device.LocalAgent.ControllerTrust.Role.{i}.PermissionNumberOfEntries
**
** \param   req - pointer to structure identifying the parameter
** \param   buf - pointer to buffer into which to return the value of the parameter (as a textual string)
** \param   len - length of buffer in which to return the value of the parameter
**
** \return  USP_ERR_OK if successful
**
**************************************************************************/
int Get_PermissionNumEntries(dm_req_t *req, char *buf, int len)
{
    role_t *rp;

    rp = CalcRoleFromReq(req);

    val_uint = rp->num_permissions;

    return USP_ERR_OK;
}

/*********************************************************************//**
**
** Get_PermissionOrder
**
** Gets the value of Device.LocalAgent.ControllerTrust.Role.{i}.Permission.{i}.Order
**
** \param   req - pointer to structure identifying the parameter
** \param   buf - pointer to buffer into which to return the value of the parameter (as a textual string)
** \param   len - length of buffer in which to return the value of the parameter
**
** \return  USP_ERR_OK if successful
**
**************************************************************************/
int Get_PermissionOrder(dm_req_t *req, char *buf, int len)
{
    // Since our vendor interface assumes that the vendor has ordered the permissions, this can just return the instance number in the permission table
    val_uint = inst2 - 1;

    return USP_ERR_OK;
}

/*********************************************************************//**
**
** Get_PermissionTargets
**
** Gets the value of Device.LocalAgent.ControllerTrust.Role.{i}.Permission.{i}.Targets
**
** \param   req - pointer to structure identifying the parameter
** \param   buf - pointer to buffer into which to return the value of the parameter (as a textual string)
** \param   len - length of buffer in which to return the value of the parameter
**
** \return  USP_ERR_OK if successful
**
**************************************************************************/
int Get_PermissionTargets(dm_req_t *req, char *buf, int len)
{
    permission_t *pp;

    pp = CalcPermissionFromReq(req);

    USP_STRNCPY(buf, pp->targets, len);
    return USP_ERR_OK;
}

/*********************************************************************//**
**
** Get_ParamPermissions
**
** Gets the value of Device.LocalAgent.ControllerTrust.Role.{i}.Permission.{i}.Param
**
** \param   req - pointer to structure identifying the parameter
** \param   buf - pointer to buffer into which to return the value of the parameter (as a textual string)
** \param   len - length of buffer in which to return the value of the parameter
**
** \return  USP_ERR_OK if successful
**
**************************************************************************/
int Get_ParamPermissions(dm_req_t *req, char *buf, int len)
{
    permission_t *pp;

    #define PERMISSION_CHAR(pp, c, mask) ( ((pp->permission_bitmask & mask) == 0) ? '-' : c )
    pp = CalcPermissionFromReq(req);

    USP_SNPRINTF(buf, len, "%c%c-%c", PERMISSION_CHAR(pp, 'r', PERMIT_GET),
                                      PERMISSION_CHAR(pp, 'w', PERMIT_SET),
                                      PERMISSION_CHAR(pp, 'n', PERMIT_SUBS_VAL_CHANGE) );
    return USP_ERR_OK;
}


/*********************************************************************//**
**
** Get_ObjPermissions
**
** Gets the value of Device.LocalAgent.ControllerTrust.Role.{i}.Permission.{i}.Obj
**
** \param   req - pointer to structure identifying the parameter
** \param   buf - pointer to buffer into which to return the value of the parameter (as a textual string)
** \param   len - length of buffer in which to return the value of the parameter
**
** \return  USP_ERR_OK if successful
**
**************************************************************************/
int Get_ObjPermissions(dm_req_t *req, char *buf, int len)
{
    permission_t *pp;

    pp = CalcPermissionFromReq(req);
    USP_SNPRINTF(buf, len, "%c%c-%c", PERMISSION_CHAR(pp, 'r', PERMIT_OBJ_INFO),
                                      PERMISSION_CHAR(pp, 'w', PERMIT_ADD),
                                      PERMISSION_CHAR(pp, 'n', PERMIT_SUBS_OBJ_ADD) );
    return USP_ERR_OK;
}

/*********************************************************************//**
**
** Get_InstantiatedObjPermissions
**
** Gets the value of Device.LocalAgent.ControllerTrust.Role.{i}.Permission.{i}.InstantiatedObj
**
** \param   req - pointer to structure identifying the parameter
** \param   buf - pointer to buffer into which to return the value of the parameter (as a textual string)
** \param   len - length of buffer in which to return the value of the parameter
**
** \return  USP_ERR_OK if successful
**
**************************************************************************/
int Get_InstantiatedObjPermissions(dm_req_t *req, char *buf, int len)
{
    permission_t *pp;

    pp = CalcPermissionFromReq(req);
    USP_SNPRINTF(buf, len, "%c%c-%c", PERMISSION_CHAR(pp, 'r', PERMIT_GET_INST),
                                      PERMISSION_CHAR(pp, 'w', PERMIT_DEL),
                                      PERMISSION_CHAR(pp, 'n', PERMIT_SUBS_OBJ_DEL) );
    return USP_ERR_OK;
}

/*********************************************************************//**
**
** Get_CommandEventPermissions
**
** Gets the value of Device.LocalAgent.ControllerTrust.Role.{i}.Permission.{i}.CommandEvent
**
** \param   req - pointer to structure identifying the parameter
** \param   buf - pointer to buffer into which to return the value of the parameter (as a textual string)
** \param   len - length of buffer in which to return the value of the parameter
**
** \return  USP_ERR_OK if successful
**
**************************************************************************/
int Get_CommandEventPermissions(dm_req_t *req, char *buf, int len)
{
    permission_t *pp;

    pp = CalcPermissionFromReq(req);
    USP_SNPRINTF(buf, len, "%c-%c%c", PERMISSION_CHAR(pp, 'r', PERMIT_CMD_INFO),
                                      PERMISSION_CHAR(pp, 'x', PERMIT_OPER),
                                      PERMISSION_CHAR(pp, 'n', PERMIT_SUBS_EVT_OPER_COMP) );
    return USP_ERR_OK;
}

/*********************************************************************//**
**
** Get_CredentialRole
**
** Gets the value of Device.LocalAgent.ControllerTrust.Credential.{i}.Role
**
** \param   req - pointer to structure identifying the parameter
** \param   buf - pointer to buffer into which to return the value of the parameter (as a textual string)
** \param   len - length of buffer in which to return the value of the parameter
**
** \return  USP_ERR_OK if successful
**
**************************************************************************/
int Get_CredentialRole(dm_req_t *req, char *buf, int len)
{
    credential_t *cp;

    cp = CalcCredentialFromReq(req);
    USP_SNPRINTF(buf, len, "Device.LocalAgent.ControllerTrust.Role.%d", cp->role + 1);

    return USP_ERR_OK;
}

/*********************************************************************//**
**
** Get_CredentialCertificate
**
** Gets the value of Device.LocalAgent.ControllerTrust.Credential.{i}.Credential
**
** \param   req - pointer to structure identifying the parameter
** \param   buf - pointer to buffer into which to return the value of the parameter (as a textual string)
** \param   len - length of buffer in which to return the value of the parameter
**
** \return  USP_ERR_OK if successful
**
**************************************************************************/
int Get_CredentialCertificate(dm_req_t *req, char *buf, int len)
{
    credential_t *cp;

    cp = CalcCredentialFromReq(req);
    USP_SNPRINTF(buf, len, "Device.LocalAgent.Certificate.%d", cp->cert_instance);

    return USP_ERR_OK;
}

/*********************************************************************//**
**
** CalcRoleFromReq
**
** Gets a pointer to the role structure located by the specified request
**
** \param   req - pointer to structure identifying the parameter
**
** \return  pointer to role structure
**
**************************************************************************/
role_t *CalcRoleFromReq(dm_req_t *req)
{
    int index;
    role_t *rp;

    index = inst1 - 1;
    USP_ASSERT(index < kCTrustRole_Max);
    USP_ASSERT(index >= 0);
    rp = &roles[index];

    return rp;
}

/*********************************************************************//**
**
** CalcPermissionFromReq
**
** Gets a pointer to the internal permission structure located by the specified request
**
** \param   req - pointer to structure identifying the parameter
**
** \return  pointer to permission structure
**
**************************************************************************/
permission_t *CalcPermissionFromReq(dm_req_t *req)
{
    int index;
    role_t *rp;
    permission_t *pp;

    rp = CalcRoleFromReq(req);

    index = inst2 - 1;
    USP_ASSERT(index < rp->num_permissions);

    pp = &rp->permissions[index];
    return pp;
}

/*********************************************************************//**
**
** CalcCredentialFromReq
**
** Gets a pointer to the credential structure located by the specified request
**
** \param   req - pointer to structure identifying the parameter
**
** \return  pointer to credential structure
**
**************************************************************************/
credential_t *CalcCredentialFromReq(dm_req_t *req)
{
    int index;
    credential_t *cp;

    index = inst1 - 1;
    USP_ASSERT(index < num_credentials);
    cp = &credentials[index];

    return cp;
}

/*********************************************************************//**
**
** DestroyControllerChallenge
**
** Clean up controller challenge structure
**
** \param   controller_challenge - pointer to controller challenge structure
**
** \return  None
**
**************************************************************************/
void DestroyControllerChallenge(controller_challenge_t *controller_challenge)
{
    USP_SAFE_FREE(controller_challenge->challenge_id);
    USP_SAFE_FREE(controller_challenge->challenge_ref);
    USP_SAFE_FREE(controller_challenge->controller_endpoint_id);
}

/*********************************************************************//**
**
** FindAvailableControllerChallenge
**
** Find available controller challenge structure by controller endpoint id
** If the same controller sends a second request, the same structure is going to be cleaned and reused
**
** \param   controller_endpoint_id - pointer to structure containing the controller endpoint id
**
** \return  pointer to controller challenge structure or NULL if all are used.
** NULL should not be returned since the structure uses the same amount of
** available controllers and each controller can have only one request at a time
**
**************************************************************************/
controller_challenge_t *FindAvailableControllerChallenge(char *controller_endpoint_id)
{
    int i;
    controller_challenge_t *cc;

    // verify if the controller has a request challenge
    for (i = 0; i < NUM_ELEM(controller_challenges); i++)
    {
        cc = &controller_challenges[i];

        // if it does, return it
        if (cc->controller_endpoint_id != NULL
                && strcmp(cc->controller_endpoint_id, controller_endpoint_id) == 0)
        {
            // but first, clean up existing information
            // since new requests from controllers should
            // be overwritten
            DestroyControllerChallenge(cc);

            // return same controller challenge
            return cc;
        }
    }

    // there is no request challenge for the controller
    for (i = 0; i < NUM_ELEM(controller_challenges); i++)
    {
        cc = &controller_challenges[i];

        // check for unused challenges
        // no challenge id means there is no challenge ref and no controller endpoint id
        if (cc->challenge_id == NULL)
        {
            return cc;
        }
    }

    // null should never be returned since the amount of available controller challenges
    // is equals to the maximum amount of controllers and each controller can have
    // only one controller challenge
    return NULL;
}

/*********************************************************************//**
**
** FindControllerChallengeByEndpointId
**
** Find controller challenge structure by controller endpoint id
**
** \param   controller_endpoint_id - pointer to structure containing the controller endpoint id
**
** \return  pointer to controller challenge structure or NULL if there's none
**
**************************************************************************/
controller_challenge_t *FindControllerChallengeByEndpointId(char *controller_endpoint_id)
{
    int i;
    controller_challenge_t *cc;

    for (i = 0; i < NUM_ELEM(controller_challenges); i++)
    {
        cc = &controller_challenges[i];
        if (cc->controller_endpoint_id != NULL
                && strcmp(controller_endpoint_id, cc->controller_endpoint_id) == 0)
        {
            return cc;
        }
    }

    return NULL;
}

/*********************************************************************//**
**
** ControllerTrustRequestChallenge
**
** Called when sync command Device.LocalAgent.ControllerTrust.RequestChallenge() is executed
** Validates and generates the RequestChallenge to be sent to the controller
** which requested it.
**
** \param   req - pointer to structure identifying the command
** \param   command_key - not used
** \param   input_args - RequestChallenge() input parameters
** \param   output_args - RequestChallenge() output parameters
**
** \return  USP_ERR_OK if successful
**          USP_ERR_INVALID_VALUE if the challenge reference is empty
**          USP_ERR_INVALID_VALUE if the challenge reference doesn't exist
**          USP_ERR_INVALID_VALUE if the challenge is disabled
**
**************************************************************************/
int ControllerTrustRequestChallenge(dm_req_t *req, char *command_key, kv_vector_t *input_args, kv_vector_t *output_args)
{
    int err = USP_ERR_OK;
    char path[MAX_DM_PATH];
    bool enabled;
    controller_challenge_t *cc;
    controller_info_t ci;
    int challenge_ref_instance;

    // Input variables
    char *challenge_ref;
    int request_expiration;

    // Output variables
    char *instruction = NULL;
    char *instruction_type = NULL;
    char *value_type = NULL;
    char challenge_id[MAX_DM_VALUE_LEN];

    // Extract the input arguments using KV_VECTOR_ functions
    challenge_ref = USP_ARG_Get(input_args, "ChallengeRef", "");
    err = USP_ARG_GetInt(input_args, "RequestExpiration", DEFAULT_REQUEST_EXPIRATION, &request_expiration);
    if (err != USP_ERR_OK)
    {
        goto exit;
    }

    // Execute operation
    // validate if challenge reference is not empty
    if (strcmp(challenge_ref, "") == 0)
    {
        // if it doesn't, return invalid value
        USP_ERR_SetMessage("%s: Invalid value - challenge reference is empty", __FUNCTION__);
        err = USP_ERR_INVALID_VALUE;
        goto exit;
    }

    // validate if challenge reference exists
    err = DM_ACCESS_ValidateReference(challenge_ref, DEVICE_CHALLENGE_ROOT, &challenge_ref_instance);
    if (err != USP_ERR_OK)
    {
        err = USP_ERR_INVALID_VALUE;
        goto exit;
    }

    // controller info that sent the current message
    MSG_HANDLER_GetControllerInfo(&ci);

    // verify if the challenge is enabled
    USP_SNPRINTF(path, sizeof(path), "%s.Enable", challenge_ref);
    err = DM_ACCESS_GetBool(path, &enabled);
    if (err != USP_ERR_OK)
    {
        err = USP_ERR_INVALID_VALUE;
        goto exit;
    }

    if (!enabled)
    {
        USP_ERR_SetMessage("%s: Invalid value - challenge disabled", __FUNCTION__);
        err = USP_ERR_INVALID_VALUE;
        goto exit;
    }

    // set the output parameters
    USP_SNPRINTF(path, sizeof(path), "%s.Instruction", challenge_ref);
    err = DM_ACCESS_GetString(path, &instruction);
    if (err != USP_ERR_OK)
    {
        goto exit;
    }

    USP_SNPRINTF(path, sizeof(path), "%s.InstructionType", challenge_ref);
    err = DM_ACCESS_GetString(path, &instruction_type);
    if (err != USP_ERR_OK)
    {
        goto exit;
    }

    USP_SNPRINTF(path, sizeof(path), "%s.ValueType", challenge_ref);
    err = DM_ACCESS_GetString(path, &value_type);
    if (err != USP_ERR_OK)
    {
        goto exit;
    }

    GenerateChallengeId(challenge_id, sizeof(challenge_id));

    // each controller can have only one request challenge
    // 'There is at most one (1) outstanding RequestChallenge for a requesting Controller.'
    // this should not be null
    cc = FindAvailableControllerChallenge(ci.endpoint_id);
    USP_ASSERT(cc != NULL);

    // store the data
    cc->controller_endpoint_id = USP_STRDUP(ci.endpoint_id);
    cc->challenge_id = USP_STRDUP(challenge_id);
    cc->challenge_ref = USP_STRDUP(challenge_ref);
    cc->expiration = request_expiration;
    cc->expire_time = time(NULL) + request_expiration;
    cc->retries = 0;
    cc->locked_time = 0;

    // Save all results into the output arguments using KV_VECTOR_ functions
    USP_ARG_Add(output_args, "Instruction", instruction);
    USP_ARG_Add(output_args, "InstructionType", instruction_type);
    USP_ARG_Add(output_args, "ValueType", value_type);
    USP_ARG_Add(output_args, "ChallengeID", challenge_id);

    err = USP_ERR_OK;

exit:

    USP_SAFE_FREE(instruction);
    USP_SAFE_FREE(instruction_type);
    USP_SAFE_FREE(value_type);

    return err;
}

/*********************************************************************//**
**
** GenerateChallengeId
**
** Generate challenge id for the request
**
** \param   challenge_id - pointer to buffer in which to write the message id
** \param   len - length of buffer
**
** \return  pointer to challenge id string
**
**************************************************************************/
char *GenerateChallengeId(char *challenge_id, int len)
{
    char buf[MAX_ISO8601_LEN];

    request_challenge_count++;
    USP_SNPRINTF(challenge_id, len, "Challenge-%s-%d", iso8601_cur_time(buf, sizeof(buf)), request_challenge_count);

    return challenge_id;
}

/*********************************************************************//**
**
** Validate_ChallengeRole
**
** Validates Device.LocalAgent.ControllerTrust.Challenge.{i}.Role parameter
**
** \param   req - pointer to structure identifying the parameter
** \param   value - value that the controller would like to set the parameter to
**
** \return  USP_ERR_OK if successful
**
**************************************************************************/
int Validate_ChallengeRole(dm_req_t *req, char *value)
{
    int err;
    int instance;

    err = DM_ACCESS_ValidateReference(value, DEVICE_ROLE_ROOT, &instance);

    return err;
}

/*********************************************************************//**
**
** Validate_ChallengeType
**
** Validates Device.LocalAgent.ControllerTrust.Challenge.{i}.Type parameter
**
** \param   req - pointer to structure identifying the parameter
** \param   value - value that the controller would like to set the parameter to
**
** \return  USP_ERR_OK if successful
**
**************************************************************************/
int Validate_ChallengeType(dm_req_t *req, char *value)
{
    // Exit if trying to set a value outside of the range we accept
    if (strcmp(value, CHALLENGE_TYPE) != 0)
    {
        USP_ERR_SetMessage("%s: Only Challenge type supported is '%s'", __FUNCTION__, CHALLENGE_TYPE);
        return USP_ERR_INVALID_VALUE;
    }

    return USP_ERR_OK;
}

/*********************************************************************//**
**
** Validate_ChallengeValueType
**
** Validates Device.LocalAgent.ControllerTrust.Challenge.{i}.ValueType parameter
**
** \param   req - pointer to structure identifying the parameter
** \param   value - value that the controller would like to set the parameter to
**
** \return  USP_ERR_OK if successful
**
**************************************************************************/
int Validate_ChallengeValueType(dm_req_t *req, char *value)
{
    // Exit if trying to set a value outside of the range we accept
    if (strcmp(value, CHALLENGE_VALUE_TYPE) != 0)
    {
        USP_ERR_SetMessage("%s: Only Challenge value type supported is '%s'", __FUNCTION__, CHALLENGE_VALUE_TYPE);
        return USP_ERR_INVALID_VALUE;
    }

    return USP_ERR_OK;
}

/*********************************************************************//**
**
** Validate_ChallengeInstructionType
**
** Validates Device.LocalAgent.ControllerTrust.Challenge.{i}.InstructionType parameter
**
** \param   req - pointer to structure identifying the parameter
** \param   value - value that the controller would like to set the parameter to
**
** \return  USP_ERR_OK if successful
**
**************************************************************************/
int Validate_ChallengeInstructionType(dm_req_t *req, char *value)
{
    // Exit if trying to set a value outside of the range we accept
    if (strcmp(value, CHALLENGE_INSTRUCTION_TYPE) != 0)
    {
        USP_ERR_SetMessage("%s: Only Challenge instruction type supported is '%s'", __FUNCTION__, CHALLENGE_INSTRUCTION_TYPE);
        return USP_ERR_INVALID_VALUE;
    }

    return USP_ERR_OK;
}

/*********************************************************************//**
**
** ControllerTrustChallengeResponse
**
** Called when sync command Device.LocalAgent.ControllerTrust.ChallengeResponse() is executed
** Validates ChallengeResponse received by the agent.
** Matches the existing request with the response.
** Validates password.
** Assign new role to the controller.
**
** \param   req - pointer to structure identifying the command
** \param   command_key - not used
** \param   input_args - ChallengeResponse() input parameters
** \param   output_args - not used
**
** \return  USP_ERR_OK if successful
**          USP_ERR_INVALID_VALUE if there is no challenge request
**          USP_ERR_INVALID_VALUE if the challenge id is not found
**          USP_ERR_INVALID_VALUE if the challenge expired
**          USP_ERR_COMMAND_FAILURE if the role is invalid
**          USP_ERR_REQUEST_DENIED if the lockout period is not expired
**          USP_ERR_COMMAND_FAILURE if the password doesn't match
**          USP_ERR_COMMAND_FAILURE if the controller is not found
**          USP_ERR_COMMAND_FAILURE if it's not possible to set the role on the controller
**
**************************************************************************/
int ControllerTrustChallengeResponse(dm_req_t *req, char *command_key, kv_vector_t *input_args, kv_vector_t *output_args)
{
    int err = USP_ERR_OK;
    char path[MAX_DM_PATH];
    char value[MAX_DM_VALUE_LEN];
    unsigned retries;
    int lockout;
    controller_challenge_t *cc;
    controller_info_t ci;
    int challenge_ref_instance;
    int controller_instance;
    char role[MAX_DM_VALUE_LEN];

    // Input variables
    char *input_challenge_id;
    char *input_value;

    // Extract the input arguments using KV_VECTOR_ functions
    input_challenge_id = USP_ARG_Get(input_args, "ChallengeID", "");
    input_value = USP_ARG_Get(input_args, "Value", "");

    // Execute operation
    MSG_HANDLER_GetControllerInfo(&ci);

    cc = FindControllerChallengeByEndpointId(ci.endpoint_id);

    // if challenge was never requested
    if (cc == NULL)
    {
        USP_ERR_SetMessage("%s: Invalid value - challenge never requested", __FUNCTION__);
        err = USP_ERR_INVALID_VALUE;
        goto exit;
    }

    // if the challenge is not found (challenge id matches),
    // return 7012 Invalid value according to https://issues.broadband-forum.org/browse/DEV2DM-32
    if (strcmp(cc->challenge_id, input_challenge_id) != 0)
    {
        USP_ERR_SetMessage("%s: Invalid value - challenge id not found", __FUNCTION__);
        err = USP_ERR_INVALID_VALUE;
        goto exit;
    }

    // if the challenge expired,
    // return 7012 Invalid value according to https://issues.broadband-forum.org/browse/DEV2DM-32
    time_t now = time(NULL);
    if (cc->expiration > 0 && cc->expire_time < now)
    {
        USP_ERR_SetMessage("%s: Invalid value - challenge expired", __FUNCTION__);
        err = USP_ERR_INVALID_VALUE;
        goto exit;
    }

    // verify if the challenge object was not removed before receiving challenge response
    err = DM_ACCESS_ValidateReference(cc->challenge_ref, DEVICE_CHALLENGE_ROOT, &challenge_ref_instance);
    if (err != USP_ERR_OK)
    {
        err = USP_ERR_COMMAND_FAILURE;
        // destroy existing request, new challenge request is required
        DestroyControllerChallenge(cc);
        goto exit;
    }

    // retrieve values from challenge
    // Value
    USP_SNPRINTF(path, sizeof(path), "%s.Value", cc->challenge_ref);
    err = DATA_MODEL_GetParameterValue(path, value, sizeof(value), SHOW_PASSWORD);
    if (err != USP_ERR_OK)
    {
        goto exit;
    }

    // Retries
    USP_SNPRINTF(path, sizeof(path), "%s.Retries", cc->challenge_ref);
    err = DM_ACCESS_GetUnsigned(path, &retries);
    if (err != USP_ERR_OK)
    {
        goto exit;
    }

    // LockoutPeriod
    USP_SNPRINTF(path, sizeof(path), "%s.LockoutPeriod", cc->challenge_ref);
    err = DM_ACCESS_GetInteger(path, &lockout);
    if (err != USP_ERR_OK)
    {
        goto exit;
    }

    // verify if the locked period passed
    if (lockout > 0 && cc->locked_time > now)
    {
        // it's still locked out
        USP_ERR_SetMessage("%s: Invalid value - lockout period hasn't expired", __FUNCTION__);
        err = USP_ERR_REQUEST_DENIED;
        goto exit;
    }

    // if value matches value from challenge, return ok
    // otherwise return 7022 Command failure according to https://issues.broadband-forum.org/browse/DEV2DM-32
    if (strcmp(input_value, value) != 0)
    {
        // wrong password
        // increase retries
        cc->retries++;
        if (cc->retries >= retries)
        {
            cc->locked_time = now + lockout;
        }

        USP_ERR_SetMessage("%s: Command failure - invalid password", __FUNCTION__);
        err = USP_ERR_COMMAND_FAILURE;
        goto exit;
    }

    // get controller instance
    controller_instance = DEVICE_CONTROLLER_FindInstanceByEndpointId(ci.endpoint_id);
    if (controller_instance == INVALID)
    {
        USP_ERR_SetMessage("%s: Command failure - controller not found", __FUNCTION__);
        err = USP_ERR_COMMAND_FAILURE;
        goto exit;
    }

    // get role from challenge
    USP_SNPRINTF(path, sizeof(path), "%s.Role", cc->challenge_ref);
    err = DATA_MODEL_GetParameterValue(path, role, sizeof(role), 0);
    if (err != USP_ERR_OK)
    {
        USP_ERR_SetMessage("%s: Command failure - get role from challenge failed", __FUNCTION__);
        err = USP_ERR_COMMAND_FAILURE;
        goto exit;
    }

    // set role to controller
    USP_SNPRINTF(path, sizeof(path), "Device.LocalAgent.Controller.%d.AssignedRole", controller_instance);
    err = DATA_MODEL_SetParameterValue(path, role, 0);
    if (err != USP_ERR_OK)
    {
        USP_ERR_SetMessage("%s: Command failure - set role failed", __FUNCTION__);
        err = USP_ERR_COMMAND_FAILURE;
        goto exit;
    }

    // if the code reached here, everything worked out
    // that means the current challenge should be cleaned up
    DestroyControllerChallenge(cc);

    err = USP_ERR_OK;

exit:

    return err;
}

