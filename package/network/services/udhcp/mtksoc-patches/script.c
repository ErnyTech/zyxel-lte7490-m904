/* script.c
 *
 * Functions to call the DHCP client notification scripts 
 *
 * Russ Dill <Russ.Dill@asu.edu> July 2001
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>

#include "options.h"
#include "dhcpd.h"
#include "dhcpc.h"
#include "packet.h"
#include "options.h"
#include "debug.h"

/*ZyXEL zcfg*/
#include "zcfg_common.h"
#include "zcfg_fe_rdm_access.h"
#include "zcfg_msg.h"

#include "zcfg_rdm_oid.h"
#include "zcfg_rdm_obj.h"
#include "zcfg_fe_rdm_struct.h"

#include <json/json.h>

extern struct client_config_t client_config;

/* get a rough idea of how long an option will be (rounding up...) */
static int max_option_length[] = {
	[OPTION_IP] =		sizeof("255.255.255.255 "),
	[OPTION_IP_PAIR] =	sizeof("255.255.255.255 ") * 2,
	[OPTION_STRING] =	1,
	[OPTION_BOOLEAN] =	sizeof("yes "),
	[OPTION_U8] =		sizeof("255 "),
	[OPTION_U16] =		sizeof("65535 "),
	[OPTION_S16] =		sizeof("-32768 "),
	[OPTION_U32] =		sizeof("4294967295 "),
	[OPTION_S32] =		sizeof("-2147483684 "),
};

zcfgRet_t dhcpMsgSend(int msg_type, zcfgEid_t dstEid, int payloadLen, const char *payload)
{
	zcfgRet_t ret = ZCFG_SUCCESS - 1;
	void *sendBuf = NULL;
	void *recvBuf = NULL;
	zcfgMsg_t *sendMsgHdr = NULL;
	char *recv_str = NULL;
	int buf_size = 0;
	int retryCnt = 0;
	
	buf_size = sizeof(zcfgMsg_t)+payloadLen;
	sendBuf = malloc(buf_size);
	sendMsgHdr = (zcfgMsg_t*)sendBuf;

	sendMsgHdr->type = msg_type;
	sendMsgHdr->length = payloadLen;
	sendMsgHdr->srcEid = ZCFG_EID_UDHCPC;
	sendMsgHdr->dstEid = dstEid;

	if(payload != NULL)
		memcpy(sendBuf+sizeof(zcfgMsg_t), payload, payloadLen);

#if defined(ECONET_PLATFORM) && defined(SUPPORT_XPON)
	while ((ret < ZCFG_SUCCESS) && (retryCnt <= 3)) {
		LOG(LOG_INFO, "udhcp send message to ESMD: msgType[%d] (retry=%d)",  msg_type^ZCFG_NO_WAIT_REPLY, retryCnt);
		ret = zcfgMsgSendAndGetReply(sendMsgHdr, (zcfgMsg_t **)&recvBuf, 0);
		if (ret >= ZCFG_SUCCESS) {
			if (recvBuf) {
			recv_str = (char *)recvBuf+sizeof(zcfgMsg_t);
			printf("Receive message : %s\n", recv_str);
			free(recvBuf);
			}	
			break;
		} else {
			retryCnt++;
			usleep(100);
		}		
	}
#else
	LOG(LOG_INFO, "%s: msgType[%d]", __FUNCTION__, msg_type);
	ret = zcfgMsgSendAndGetReply(sendMsgHdr, (zcfgMsg_t **)&recvBuf, 0);
	if(ret == ZCFG_SUCCESS) {
		recv_str = (char *)recvBuf+sizeof(zcfgMsg_t);
		printf("Receive message : %s\n", recv_str);
		free(recvBuf);
	}
#endif

	return ret;
}

bool getDefaultRouteFromOption121(unsigned char *option, char *gateway){
	int i = 0;
	int dest_len_byte = 0;

	i = OPT_DATA;
	while(i < option[OPT_LEN] + 2){
		if(option[i] == 0){
			sprintf(gateway, "%d.%d.%d.%d", option[i+1], option[i+2], option[i+3], option[i+4]);
			return true;
		}
		else{
			dest_len_byte = option[i]/8 + ((option[i]%8 != 0)?1:0);
			i = i + 1 + dest_len_byte + 4;
		}
	}

	return false;
}


static int upper_length(int length, struct dhcp_option *option)
{
	return max_option_length[option->flags & TYPE_MASK] *
	       (length / option_lengths[option->flags & TYPE_MASK]);
}


static int sprintip(char *dest, char *pre, unsigned char *ip) {
	return sprintf(dest, "%s%d.%d.%d.%d ", pre, ip[0], ip[1], ip[2], ip[3]);
}

// YuChih(20201113) Proxy ARP feature: setting subnet by zcfg_config
static int transSubnetPrefixToIPAddress(int subnet_prefix, unsigned char *address_field, int field_count) {
    printf("subnet_prefix = %d\n", subnet_prefix);
    printf("field_count = %d\n", field_count);

    if (subnet_prefix > 32 || subnet_prefix < 0) {
        return -1;
    }
    if (field_count != 4) {
        return -1;
    }

    int quotient = 0;
    int remainder = 0;

    quotient = subnet_prefix/8;
    remainder = subnet_prefix%8;

    printf("quotient = %d\n", quotient);
    printf("remainder = %d\n", remainder);

    int power2prefix[8] = {
        0,      // 0000 0000
        128,    // 1000 0000
        192,    // 1100 0000
        224,    // 1110 0000
        240,    // 1111 0000
        248,    // 1111 1000
        252,    // 1111 1100
        254,    // 1111 1110
    };

    // all 4 field set to 0 first
    memset(address_field, 0, 4);

    int i = 0;
    for (i = 0; i < field_count; i++) {
        if (quotient > 0) {
            address_field[i] = 255;
            quotient--;
        } else { // quotient == 0
            address_field[i] = power2prefix[remainder];
            break;
        }
    }

    printf("subnet mask address = %d.%d.%d.%d\n", address_field[0], address_field[1], address_field[2], address_field[3]);

    return 0; // success
} // YuChih(20201113) Proxy ARP feature: setting subnet by zcfg_config

/* Fill dest with the text of option 'option'. */
static void fill_options(char *dest, unsigned char *option, struct dhcp_option *type_p, struct json_object *dhcpc_opt)
{
	int type, optlen;
	u_int16_t val_u16;
	int16_t val_s16;
	u_int32_t val_u32;
	int32_t val_s32;
	int len = option[OPT_LEN - 2];
	int tmpLen = 0;
	char *tmp = NULL;
#if 1 /* ZyXEL DHCP option support (porting from BRCM and MSTC), John */
	char optStr[256];
#endif

	dest += sprintf(dest, "%s=", type_p->name);
	tmp = dest;

	type = type_p->flags & TYPE_MASK;
	optlen = option_lengths[type];
	for(;;) {
		switch (type) {
		case OPTION_IP_PAIR:
			dest += sprintip(dest, "", option);
			*(dest++) = '/';
			option += 4;
			optlen = 4;
		case OPTION_IP:	/* Works regardless of host byte order. */
			if(type_p->code == DHCP_NTP_SERVER)
			{
				char ntpsrv_addr[16];

				sprintf(ntpsrv_addr, "%d.%d.%d.%d", option[0], option[1], option[2], option[3]);
				json_object_object_add(dhcpc_opt, type_p->name, json_object_new_string(ntpsrv_addr));
			}
#if 1  // YuChih(20201113) Proxy ARP feature: setting subnet by zcfg_config
			if(type_p->code == DHCP_SUBNET)
			{
				char subnet_addr[16];
                int wwan_index = -1;

				printf("original subnet_addr %d.%d.%d.%d\n", option[0], option[1], option[2], option[3]);

                printf("interface=%s\n", client_config.interface);

                sscanf(client_config.interface, "wwan%d", &wwan_index);
                printf("wwan interface index: %d\n", wwan_index);


                objIndex_t passthruIid;
                zcfgRet_t ret = ZCFG_SUCCESS;
                rdm_CellIntfZyIpPassThru_t *cellIntfZyIpPassThruObj = NULL;
                int prefix = 0;

                IID_INIT(passthruIid);

                passthruIid.level=1;
                passthruIid.idx[0] = wwan_index+1;

                ret = zcfgFeObjStructGet(RDM_OID_CELL_INTF_ZY_IP_PASS_THRU, &passthruIid, (void **)&cellIntfZyIpPassThruObj);

                if(ret != ZCFG_SUCCESS)
                {
                    printf("%s: Get CELL_PASSTHRU Fail, ret = %d\n", __FUNCTION__, ret);

                } else {
                    printf("ProxyARP_SubnetPrefix = %d\n", cellIntfZyIpPassThruObj->ProxyARP_SubnetPrefix);
                    prefix = cellIntfZyIpPassThruObj->ProxyARP_SubnetPrefix;

                    if (prefix != 0) {
                        //  subnet mask prefix to 4 subnet address field
                        printf("original subnet %d.%d.%d.%d\n", option[0], option[1], option[2], option[3]);

                        transSubnetPrefixToIPAddress(prefix, option, 4);
                        printf("manual set subnet %d.%d.%d.%d\n", option[0], option[1], option[2], option[3]);
                    }

                    zcfgFeObjStructFree(cellIntfZyIpPassThruObj);
                }
			}
#endif // YuChih(20201113) Proxy ARP feature: setting subnet by zcfg_config
			dest += sprintip(dest, "", option);
 			break;
		case OPTION_BOOLEAN:
			dest += sprintf(dest, *option ? "yes " : "no ");
			break;
		case OPTION_U8:
			dest += sprintf(dest, "%u ", *option);
			break;
		case OPTION_U16:
			memcpy(&val_u16, option, 2);
			dest += sprintf(dest, "%u ", ntohs(val_u16));
			break;
		case OPTION_S16:
			memcpy(&val_s16, option, 2);
			dest += sprintf(dest, "%d ", ntohs(val_s16));
			break;
		case OPTION_U32:
			memcpy(&val_u32, option, 4);
			dest += sprintf(dest, "%lu ", (unsigned long) ntohl(val_u32));
			break;
		case OPTION_S32:
			memcpy(&val_s32, option, 4);
			dest += sprintf(dest, "%ld ", (long) ntohl(val_s32));
			break;
		case OPTION_STRING:
#if 1 /* ZyXEL DHCP option support (porting from BRCM and MSTC), John */
			/* DHCP option 43 */
			if(type_p->code == 0x2b || type_p->code == 0x79){
				if(type_p->code == 0x79){
					//RFC3442, if option 121 contain default route, the priority is higher than option3.
					if(getDefaultRouteFromOption121((char *)(option - 2), optStr) == true)
						json_object_object_add(dhcpc_opt, "router", json_object_new_string(optStr));
				}
				
				hexOptionToString((char *)(option - 2), optStr);
				json_object_object_add(dhcpc_opt, type_p->name, json_object_new_string(optStr));
			}
			else if(type_p->code == DHCP_6RD_OPT) {
				uint16_t *ptr16;
				char prefix[64];
				char brAddr[16];

				json_object_object_add(dhcpc_opt, "v6rdAssigned", json_object_new_boolean(1));

				json_object_object_add(dhcpc_opt, "v4MaskLen", json_object_new_int((int)option[0]));
				json_object_object_add(dhcpc_opt, "v6PrefixLen", json_object_new_int((int)option[1]));

				ptr16 = (uint16_t*)&option[2];
				
/*__ZYXEL__, ChiHsiang 6RD prefix has endian issue, if cpu is little endian and do not use ntohs() will creat wrong prefix.*/
//				sprintf(prefix, "%x:%x:%x:%x:%x:%x:%x:%x", ptr16[0], ptr16[1], ptr16[2], ptr16[3], ptr16[4], ptr16[5], ptr16[6], ptr16[7]);
				sprintf(prefix, "%x:%x:%x:%x:%x:%x:%x:%x", ntohs(ptr16[0]), ntohs(ptr16[1]), ntohs(ptr16[2]),
														   ntohs(ptr16[3]), ntohs(ptr16[4]), ntohs(ptr16[5]),
														   ntohs(ptr16[6]), ntohs(ptr16[7]));
														   
				json_object_object_add(dhcpc_opt, "v6Prefix", json_object_new_string(prefix));

				sprintf(brAddr, "%d.%d.%d.%d", option[18], option[19], option[20], option[21]);
				json_object_object_add(dhcpc_opt, "brAddr", json_object_new_string(brAddr));
			}else if(type_p->code == 0x78){ //option 120 SIP Server
				//printf("OPTION 120!!");
				//printf("%d %d %d %d %d %d", *(option - 1), option[0], option[1], option[2], option[3], option[4]);
				 u_int8_t op120_total_len = *(option - 1);
				char sip_svr_addr[257];
				char *svr_addr_ptr = sip_svr_addr;
				char *payload = option + 1;
				if(option[0] == 0){ //Domain

					while(payload < (option + op120_total_len)){
						u_int8_t sub_str_len = *payload++;

						strncpy(svr_addr_ptr, payload, sub_str_len);
						svr_addr_ptr += sub_str_len;
						payload += sub_str_len;

						if(*payload != 0){
							*svr_addr_ptr = '.';
						}else{
							*svr_addr_ptr = '\0';
							break;
						}
						svr_addr_ptr++;

					}

				}else if(option[0] == 1){ //IP-Address
					sprintf(sip_svr_addr, "%d.%d.%d.%d", option[1], option[2], option[3], option[4]);
				}
				//printf("type_p->name:%s\n", type_p->name);
				json_object_object_add(dhcpc_opt, type_p->name, json_object_new_string(sip_svr_addr));
			}
#endif
			memcpy(dest, option, len);
			dest[len] = '\0';
			return;	 /* Short circuit this case */
		}
		option += optlen;
		len -= optlen;
		if (len <= 0) break;
	}

	tmpLen = strlen(tmp);
	tmp[tmpLen-1] = '\0';
	json_object_object_add(dhcpc_opt, type_p->name, json_object_new_string(tmp));
}



static char *find_env(const char *prefix, char *defaultstr)
{
	extern char **environ;
	char **ptr;
	const int len = strlen(prefix);

	for (ptr = environ; *ptr != NULL; ptr++) {
		if (strncmp(prefix, *ptr, len) == 0)
			return *ptr;
	}
	return defaultstr;
}


/* put all the paramaters into an environment */
#if 1 //ZYXEL__, Mark
static char **fill_envp(struct dhcpMessage *packet, char *name)
#else
static char **fill_envp(struct dhcpMessage *packet)
#endif
{
	int num_options = 0;
	int i, j;
	//char **envp;
	char buf[256] = {0};
	char ip[16] = {0};
	unsigned char *tmpIp;
	unsigned char *temp;
	char over = 0;
	struct json_object *dhcpc_opt = NULL;
	const char *payload = NULL;
	int payloadLen = 0;
#if 1
	int msgType = ZCFG_MSG_DHCPC_BOUND;
#endif

	if (packet == NULL)
		num_options = 0;
	else {
		for (i = 0; options[i].code; i++)
			if (get_option(packet, options[i].code))
				num_options++;
		if (packet->siaddr) num_options++;
		if ((temp = get_option(packet, DHCP_OPTION_OVER)))
			over = *temp;
		if (!(over & FILE_FIELD) && packet->file[0]) num_options++;
		if (!(over & SNAME_FIELD) && packet->sname[0]) num_options++;		
	}
#if 0
	envp = xmalloc((num_options + 5) * sizeof(char *));
	envp[0] = xmalloc(sizeof("interface=") + strlen(client_config.interface));
	sprintf(envp[0], "interface=%s", client_config.interface);
	envp[1] = find_env("PATH", "PATH=/bin:/usr/bin:/sbin:/usr/sbin");
	envp[2] = find_env("HOME", "HOME=/");
#endif
	if (packet == NULL) {
		//envp[3] = NULL;
		//return envp;
		return NULL;
	}

	dhcpc_opt = json_object_new_object();
	//envp[3] = xmalloc(sizeof("ip=255.255.255.255"));
	tmpIp = (unsigned char *) &packet->yiaddr;
	sprintf(ip, "%d.%d.%d.%d", tmpIp[0], tmpIp[1], tmpIp[2], tmpIp[3]);
	json_object_object_add(dhcpc_opt, "ip", json_object_new_string(ip));
	//sprintip(envp[3], "ip=", (unsigned char *) &packet->yiaddr);
	//sprintf(envp[3], "ip=%s", ip);

	for (i = 0, j = 4; options[i].code; i++) {
		if ((temp = get_option(packet, options[i].code))) {
			//envp[j] = xmalloc(upper_length(temp[OPT_LEN - 2], &options[i]) + strlen(options[i].name) + 2);
			//fill_options(envp[j], temp, &options[i], dhcpc_opt);
			fill_options(buf, temp, &options[i], dhcpc_opt);
			j++;
		}
	}

	//if there is no option 3 or no default route in option121, get giaddr as router ip
	if(json_object_object_get(dhcpc_opt, "router") == NULL){
		tmpIp = (unsigned char *) &packet->giaddr;
		sprintf(ip, "%d.%d.%d.%d", tmpIp[0], tmpIp[1], tmpIp[2], tmpIp[3]);
		json_object_object_add(dhcpc_opt, "router", json_object_new_string(ip));
		printf("dhcp msg giaddr =%s\n",  ip);
	}

	if (packet->siaddr) {
		//envp[j] = xmalloc(sizeof("siaddr=255.255.255.255"));
		//sprintip(envp[j++], "siaddr=", (unsigned char *) &packet->siaddr);
		tmpIp = (unsigned char *)&packet->siaddr;
		sprintf(ip, "%d.%d.%d.%d", tmpIp[0], tmpIp[1], tmpIp[2], tmpIp[3]);
		json_object_object_add(dhcpc_opt, "siaddr", json_object_new_string(ip));
	}
	if (!(over & FILE_FIELD) && packet->file[0]) {
		/* watch out for invalid packets */
		packet->file[sizeof(packet->file) - 1] = '\0';
		//envp[j] = xmalloc(sizeof("boot_file=") + strlen(packet->file));
		//sprintf(envp[j++], "boot_file=%s", packet->file);
		json_object_object_add(dhcpc_opt, "boot_file", json_object_new_string(packet->file));
	}
	if (!(over & SNAME_FIELD) && packet->sname[0]) {
		/* watch out for invalid packets */
		packet->sname[sizeof(packet->sname) - 1] = '\0';
		//envp[j] = xmalloc(sizeof("sname=") + strlen(packet->sname));
		//sprintf(envp[j++], "sname=%s", packet->sname);
		json_object_object_add(dhcpc_opt, "sname", json_object_new_string(packet->sname));
	}

	json_object_object_add(dhcpc_opt, "addressAssigned", json_object_new_boolean(true));
	json_object_object_add(dhcpc_opt, "pidfile", json_object_new_string(client_config.pidfile));

	payload = json_object_to_json_string(dhcpc_opt);

	payloadLen = strlen(payload) + 1;

	//printf("============================UDHCP===========================\n");
	//printf("%s\n", payload);

#if 1 //__ZYXEL__, Mark	
	msgType = (name && strcmp(name, "renew") == 0) ? ZCFG_MSG_DHCPC_RENEW : ZCFG_MSG_DHCPC_BOUND;
	dhcpMsgSend(msgType, ZCFG_EID_ESMD, payloadLen, payload);
#else
	dhcpMsgSend(ZCFG_MSG_DHCPC_BOUND, ZCFG_EID_ESMD, payloadLen, payload);
#endif
	json_object_put(dhcpc_opt);
	
	//envp[j] = NULL;
	//return envp;
	return NULL;
}


/* Call a script with a par file and env vars */
void run_script(struct dhcpMessage *packet, const char *name)
{
#if 0
	int pid;
	char **envp;
#endif
	if (strcmp(name, "bound") == 0 || strcmp(name, "renew") == 0) {
		//envp = fill_envp(packet);
#if 1 //__ZYXEL__, Mark	
		fill_envp(packet, name);
#else
		fill_envp(packet);
#endif
#if 0
		while(*envp != NULL) {
			free(*envp);
			envp++;
		}

		free(envp);
#endif
	}
	else if (strcmp(name, "deconfig") == 0) {
		/* Expired, set the connection down */
		printf("Expired, set the connection down\n");
	}
#if 0
	if (client_config.script == NULL)
		return;

	/* call script */
	pid = fork();
	if (pid) {
		waitpid(pid, NULL, 0);
		return;
	} else if (pid == 0) {
#if 1 //__ZYXEL__, Mark	
		envp = fill_envp(packet, name);
#else
		envp = fill_envp(packet);
#endif
		
		/* close fd's? */
		
		/* exec script */
		DEBUG(LOG_INFO, "execle'ing %s", client_config.script);
		execle(client_config.script, client_config.script,
		       name, NULL, envp);
		LOG(LOG_ERR, "script %s failed: %s",
		    client_config.script, strerror(errno));
		exit(1);
	}
#endif
}
