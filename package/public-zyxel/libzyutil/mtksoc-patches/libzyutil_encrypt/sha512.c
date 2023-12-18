/**
  SHA512 Hash function with salt by using busybox "passwd" command.

**/

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#ifdef QUECTEL_PLATFORM
#include "libzyutil_quectel_wrapper.h"
#else
#include "libzyutil_mtksoc_wrapper.h"
#endif

#include "zos_api.h"
#include "zlog_api.h"

#include "libzyutil_encrypt.h"
#include "sha512.h"

/*********

**********/
unsigned char *sha512Hash(const char *inputPassword, char *salt){

	FILE *fp = NULL;
	char buf[512] = {0}, cmd[512] = {0};
	char *hashedValue = NULL, *temp = NULL;
	int i = 0, j = 0;

	memset(buf, 0, sizeof(buf));
	memset(cmd, 0, sizeof(cmd));
	if(salt != NULL){
		snprintf(cmd, sizeof(cmd), "passwd -a sha512 -p \"%s\" -n -s \"", inputPassword);
		temp = cmd + strlen(cmd);

		i = 0;
		j = 0;
		while(salt[j] != '\0'){
			if(salt[j] == '$'){
				temp[i] = '\\';
				i++;
			}
			temp[i] = salt[j];
			i++;
			j++;
		}
		strcat(cmd, "\"");
	}
	else{
		snprintf(cmd, sizeof(cmd), "passwd -a sha512 -p \"%s\" -n", inputPassword);
	}

	if((fp = popen(cmd, "r")) != NULL){
		while(fgets(buf, sizeof(buf), fp) != NULL){
			if(buf[0] == '$'){
				if(buf[strlen(buf) - 1] == '\n'){
					buf[strlen(buf) - 1]  = '\0';
				}
				hashedValue = (char*)malloc((strlen(buf)+1) * (sizeof(char)));
				snprintf(hashedValue, (strlen(buf)+1) * (sizeof(char)), "%s", buf);
				break;
			}
		}
	}

	pclose(fp);
	fp = NULL;

	return hashedValue;
}
