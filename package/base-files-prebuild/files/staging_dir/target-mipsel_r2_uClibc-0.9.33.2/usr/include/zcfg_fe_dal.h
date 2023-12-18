
zcfgRet_t zcfgFeDalHandler(const char *handlerName, const char *method, struct json_object *Jobj, struct json_object *Jarray, char *replyMsg);
zcfgRet_t getWanLanList(struct json_object **Jarray);

void invalidCharFilter(char *input, int maxlen);
bool HostNameFilter(const char *input);
bool DomainFilter(const char *input);
bool IPFilter(const char *input);
bool IPv6Filter(const char *input);

int isDigitString(char *getString);
zcfgRet_t insertLoginUserInfo(struct json_object *Jobj, const char *userName, const char *level);
zcfgRet_t getLoginUserInfo(struct json_object *Jobj, char *userName, char *level);
bool isSameSubNet( char *lanIp, char *lansubnetMask, char *staticIP);
