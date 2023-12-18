/******************************************************************************/
/*
*  Copyright (C) 2012 ZyXEL Communications, Corp.
*  All Rights Reserved.
*
* ZyXEL Confidential; Need to Know only.
* Protected as an unpublished work.
*
* The computer program listings, specifications and documentation
* herein are the property of ZyXEL Communications, Corp. and
* shall not be reproduced, copied, disclosed, or used in whole or
* in part for any reason without the prior express written permission of
* ZyXEL Communications, Corp.
*/
/******************************************************************************/

/* Include Kernel Lib */
#include <stdio.h>
#include <string.h>
#include <sys/select.h>

/* Include User Defined Lib */
#include "tr369socket.h"



/*************************
* Function 	: filesock_checkservice()
* Description	: Check Socket Service On or Not
* Output 		: 0 : Service On
*           		: -1 : Service Off
*************************/
int filesock_checkservice(
	char *connectPath	/* Connect Socket Path */
){
	int ret = 0;
	int sockFd = -1;
	struct sockaddr_un sockAddr;

	/* Open File Socket , if error occured , return -1 */
	sockFd = socket(AF_UNIX, SOCK_DGRAM, 0);
	if (-1 == sockFd) {
		//perror("socket() failed");
		ret = -1;
	}

	/* Check Socket Service On or Not */
	if (0 == ret) {
		/* Set Sock Address */
		memset(&sockAddr, 0, sizeof(sockAddr));
		sockAddr.sun_family = AF_UNIX;
		strncpy(sockAddr.sun_path, connectPath, sizeof(sockAddr.sun_path)-1);

		if (-1 == connect(sockFd, (struct sockaddr*)&sockAddr, sizeof(sockAddr))) {
			//perror("connect() failed");
			ret = -1;
		}
	}

	/* Close Socket */
	if (-1 != sockFd) {
		close(sockFd);
	}

	return ret;
}


/*************************
* Function 	: filesock_connect()
* Description	: Connect to socket listen buffer
* Output 		: 0 : create success
*			: -1 : create fail
*************************/
int filesock_connect(
	char *connectPath,	/* Connect Socket Path */
	int *sockFd			/* Socket Fd */
){
	struct sockaddr_un sockAddr;

	/* Open File Socket , if error occured , return -1 */
	*sockFd = socket(AF_UNIX, SOCK_DGRAM, 0);
	if (-1 == *sockFd) {
		perror("socket() failed");
		return -1;
	}

	/* Set Sock Address */
	memset(&sockAddr, 0, sizeof(sockAddr));
	sockAddr.sun_family = AF_UNIX;
	strncpy(sockAddr.sun_path, connectPath, sizeof(sockAddr.sun_path)-1);

	if (-1 == connect(*sockFd, (struct sockaddr*)&sockAddr, sizeof(sockAddr))) {
		perror("connect() failed");
		return -1;
	}

	return 0;
}


/********************************
*
* Function	: filesock_open()
* Description	: Open a socket listen buffer
* Output		: 0 : create success
*			: -1 : create fail
********************************/
int filesock_open(
	char *recvPath,	/* Listened Socket Path */
	int *sockFd		/* Socket Fd */
){
	struct sockaddr_un sockAddr;

	/* Remove Socket FilePath */
	unlink(recvPath);

	/* Open File Socket , if error occured , return -1 */
	*sockFd = socket(AF_UNIX, SOCK_DGRAM, 0);
	if (-1 == *sockFd) {
		perror("socket() failed");
		return -1;
	}

	/* Set Sock Address */
	memset(&sockAddr, 0, sizeof(sockAddr));
	sockAddr.sun_family = AF_UNIX;
	strncpy(sockAddr.sun_path, recvPath, sizeof(sockAddr.sun_path)-1);

	/* bind file socket id with server address , if error occured , return -1 */
	if (-1 == bind(*sockFd, (struct sockaddr *)&sockAddr, sizeof(sockAddr))) {
		perror("bind() failed");
		return -1;
	}

	return 0;
}


/********************************
* Function	: filesock_close()
* Description	: Close a socket 
* Output		: 
********************************/
void filesock_close(
	int *sockFd	/* Socket Fd */
){
	if (-1 != *sockFd) {
		close(*sockFd);
		*sockFd=-1;
 	}
}


/********************************
* Function	: filesock_listen()
* Description	: Listened a socket connection 
* Output		: 
********************************/
void filesock_listen(
	char *recvPath,		/* Listened Socket Path */
	int sockFd,			/* Socket Fd */
	int blockTimeSec,	/* Block Time : Second */
	int blockTimeUsec,	/* Block Time : Milli-Second */
	int recvBufSize,	/* Receive Buf Size */
	SOCKJOB funcPtr
){
	// Receive Data
	char recvBuf[recvBufSize];
	// Declare socket length
	socklen_t sockLen = 0;
	// Delcare socket date byte array
	fd_set fdData;
	// Declare interval of time-out
	struct timeval time_int;
	// Receiv Socket Buf Path
	struct sockaddr_un sockAddr;

	/* Set Sock Address */
	memset(&sockAddr, 0, sizeof(sockAddr));
	sockAddr.sun_family = AF_UNIX;
	strncpy(sockAddr.sun_path, recvPath, sizeof(sockAddr.sun_path)-1);

	while (1) {
		// Set SockAddr Length
		sockLen = sizeof(sockAddr);

		// Set Interval Of Timeout
		time_int.tv_sec = blockTimeSec;
		time_int.tv_usec = blockTimeUsec * 1000;

		// Clear fdData
		FD_ZERO(&fdData);
		// Join sockFd into fdData
		FD_SET(sockFd, &fdData);

		// Check sockFd exist in maxFd or not , until timeout
		// Return Value = -1 : error
		// Return Value = 0  : timeout
		if (0 < select((sockFd + 1) , &fdData , (fd_set*)NULL, (fd_set*)NULL, &time_int)) {
			// Check sockFd exist in fdData or not
			if (0 < FD_ISSET(sockFd, &fdData)) {
				memset(recvBuf, 0, recvBufSize);
				// Receive sockFd data
				if (0 < recvfrom(sockFd, &recvBuf, recvBufSize, 0, (struct sockaddr *)&sockAddr,  &sockLen)) {
					// Process Cmd Job
					if( funcPtr(recvBuf) == -1/*TR369_SOCK_CLOSE*/){ /*break from while loop*/
						break;
					}
				}
			}
		}

	}

}


/********************************
* Function	: filesock_send()
* Description	: Send message to a socket file path
* Output		:
********************************/
void filesock_send(
	char *sendPath,	/* Socket Sendto File Path*/
	int sockFd,		/* Socket Fd */
	void *sendBuf,	/* Send Buf Data */
	int sendBufSize	/* Send Buf Data Size */
){
	struct sockaddr_un sockAddr;

	/* Set Sock Address */
	memset(&sockAddr, 0, sizeof(sockAddr));
	sockAddr.sun_family = AF_UNIX;
	strncpy(sockAddr.sun_path, sendPath, sizeof(sockAddr.sun_path)-1);

	/* Send Msg To SendTo Socket Path */
	sendto(sockFd, sendBuf, sendBufSize, 0,  (struct sockaddr *)&sockAddr, sizeof(sockAddr));
}

