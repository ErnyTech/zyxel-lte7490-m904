/*
 *
 * Copyright (C) 2019, Broadband Forum
 * Copyright (C) 2016-2019  CommScope, Inc
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
 * \file usp_err_codes.h
 *
 * Header file containing error codes defined by the USP specification
 *
 */
#ifndef USP_ERR_CODES_H
#define USP_ERR_CODES_H

//------------------------------------------------------------------------------------
// Defines for all USP error codes
#define USP_ERR_OK                        0           // No error
#define EOK                               USP_ERR_OK

// Message error codes
#define USP_ERR_GENERAL_FAILURE           7000       // Message failed. This error indicates a general failure that is described in an err_msg element.
#define USP_ERR_MESSAGE_NOT_UNDERSTOOD    7001       // Attempted message was not understood
#define USP_ERR_REQUEST_DENIED            7002       // Cannot or will not process message
#define USP_ERR_INTERNAL_ERROR            7003       // Message failed due to an internal error
#define USP_ERR_INVALID_ARGUMENTS         7004       // Message failed due to invalid values in the request elements and/or failure to update one or more parameters during Add or Set requests
#define USP_ERR_RESOURCES_EXCEEDED        7005       // Message failed due to memory or processing limitations
#define USP_ERR_PERMISSION_DENIED         7006       // Source endpoint does not have authorisation to use this message
#define USP_ERR_INVALID_CONFIGURATION     7007       // Message failed because the result of processing the message would result in an invalid or unstable state

// ParamError codes
#define USP_ERR_INVALID_PATH_SYNTAX       7008       // Requested path was invalid or a reference was invalid
#define USP_ERR_PARAM_ACTION_FAILED       7009       // Parameter failed to update for a general reason described in an err_msg element.
#define USP_ERR_UNSUPPORTED_PARAM         7010       // Requested Path Name associated with this ParamError did not match any instantiated parameters
#define USP_ERR_INVALID_TYPE              7011       // Unable to convert string value to correct data type
#define USP_ERR_INVALID_VALUE             7012       // Out of range or invalid enumeration
#define USP_ERR_PARAM_READ_ONLY           7013       // Attempted to write to a read only parameter
#define USP_ERR_VALUE_CONFLICT            7014       // Requested value would result in an invalid configuration


#define USP_ERR_CRUD_FAILURE              7015       // General failure to perform the CRUD operation
#define USP_ERR_OBJECT_DOES_NOT_EXIST     7016       // Requested object instance does not exist
#define USP_ERR_CREATION_FAILURE          7017       // General failure to create the object
#define USP_ERR_NOT_A_TABLE               7018       // The requested pathname was expected to be a multi-instance object, but wasn't
#define USP_ERR_OBJECT_NOT_CREATABLE      7019       // Attempted to create an object which was non-creatable (for non-writable multi-instance objects)
#define USP_ERR_SET_FAILURE               7020       // General failure to set a parameter
#define USP_ERR_REQUIRED_PARAM_FAILED     7021       // The CRUD operation failed because a required parameter failed to update


#define USP_ERR_COMMAND_FAILURE           7022       // Command failed to operate
#define USP_ERR_COMMAND_CANCELLED         7023       // Command failed to complete because it was cancelled
#define USP_ERR_OBJECT_NOT_DELETABLE      7024       // Attempted to delete an object which was non-deletable, or object failed to be deleted
#define USP_ERR_UNIQUE_KEY_CONFLICT       7025       // unique keys would conflict
#define USP_ERR_INVALID_PATH              7026       // Path is not present in the data model schema
#define USP_ERR_INVALID_COMMAND_ARGS      7027       // Command failed due to invalid arguments

// Brokered USP Record Errors
#define USP_ERR_RECORD_NOT_PARSED         7100       // Record could not be parsed
#define USP_ERR_SECURE_SESS_REQUIRED      7101       // A secure session must be started before pasing any records
#define USP_ERR_SECURE_SESS_NOT_SUPPORTED 7102       // Secure session is not supported by this endpoint
#define USP_ERR_SEG_NOT_SUPPORTED         7103       // Segmentation and reassembly is not supported by this endpoint
#define USP_ERR_RECORD_FIELD_INVALID      7104       // A USP record field was invalid

//Device.SoftwareModules. Errors
#define USP_ERR_UNKNOWN_EXEC_ENV		  7223 		 // Unknown Execution Environment
#define USP_ERR_DU_TO_EXEC_ENV_MISSMATCH  7225		 // Deployment Unit to Execution Environment Mismatch
#define USP_ERR_DUPLICATE_DU			  7226		 // Duplicate Deployment Unit
#define USP_ERR_SYSTEM_RESOURCE_EXCEEDED  7227		 // System Resources Exceeded
#define USP_ERR_INVALID_DU_STATE		  7229		 // Invalid Deployment Unit State

// Vendor defined error codes
// These use codes 7800-7999. Currently USP Agent core does not define any codes in this range.


#ifndef TR069ERR_E
#define TR069ERR_E
// need to sync to ztr69/cmm/include/cwmplog.h
typedef enum
{
	TR069_SOCK_CLOSE = -1,    
	TR069_OK = 0,    
	TR069_NOK = 1,    			/* common errors */
	TR069_SERVICE_TIME_OUT,
	TR069_BUFFER_OVERFLOW,
	TR069_DNS_ERR,
	TR069_INIT_FAILED,    	/* errors occurs in initialization */
	TR069_SET_NON_APPLY,	/*All Parameter changes have been validated and committed, but some or all are not yet applied*/
	/* RPC Fault Code*/
	TR069_METHOD_NOT_SUPPORTED = 9000,
	TR069_REQUEST_DENIED = 9001,
	TR069_INTERNAL_ERROR = 9002,
	TR069_INVALID_ARGUMENTS = 9003,
	TR069_RESOURCES_EXCEEDED = 9004,
	TR069_INVALID_PARAMETER_NAME = 9005,
	TR069_INVALID_PARAMETER_TYPE = 9006,
	TR069_INVALID_PARAMETER_VALUE = 9007,
	TR069_ATTEMPT_TO_SET_NONWRITABLE_PARAMETER = 9008,
	TR069_NOTIFICATION_REQUEST_REJECTED = 9009,
	TR069_DOWNLOAD_FAILURE = 9010,
	TR069_UPLOAD_FAILURE = 9011,
	TR069_FILE_TRANSFER_SERVER_AUTHENTICATION_FAILURE = 9012,
	TR069_UNSUPPORTED_PROTOCOL_FOR_FILE_TRANSFER = 9013,
	TR069_FILE_TRANSFER_FAILURE = 9014,
	TR069_FILE_TRANSFER_FAILURE_CONTACT_SERVER = 9015,
	TR069_FILE_TRANSFER_FAILURE_ACCESS_FILE = 9016,
	TR069_FILE_TRANSFER_FAILURE_COMPLETE = 9017,
	TR069_FILE_TRANSFER_FAILURE_CORRUPTED = 9018,
	TR069_FILE_TRANSFER_FAILURE_AUTHENTICATION = 9019,
	TR069_FILE_TRANSFER_FAILURE_TIMEOUT = 9020
	/* 9800 - 9899: Vendor defined fault codes */
	/* RPC Fault Code*/
	,TR069_DOWNLOAD_SCRIPT_RUN_FAIL = 9831
}tr069ERR_e;
#endif

#endif

