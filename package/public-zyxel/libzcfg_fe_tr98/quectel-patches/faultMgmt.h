/*Parameter*/

extern tr98Parameter_t para_FaultMgmt[];
extern tr98Parameter_t para_FaultMgmtCurrentAlarm[];


extern zcfgRet_t faultmgmtObjGet(char *tr98FullPathName, int handler, struct json_object **tr98Jobj, bool updateFlag);
extern zcfgRet_t faultmgmtObjNotify(char *, char *, struct json_object *, int, struct json_object **);
extern int faultmgmtObjAttrGet(char *, int, char *);
extern zcfgRet_t faultmgmtObjAttrSet(char *, int, char *, int , struct json_object *);

extern zcfgRet_t faultmgmtCurrentAlarmObjGet(char *tr98FullPathName, int handler, struct json_object **tr98Jobj, bool updateFlag);
extern zcfgRet_t faultmgmtCurrentAlarmObjSet(char *tr98FullPathName, int handler, struct json_object *tr98Jobj, struct json_object *multiJobj);
extern zcfgRet_t faultmgmtCurrentAlarmObjAdd(char *tr98FullPathName, int *idx);
extern zcfgRet_t faultmgmtCurrentAlarmObjDel(char *tr98FullPathName);
extern zcfgRet_t faultmgmtCurrentAlarmObjNotify(char *, char *, struct json_object *, int, struct json_object **);
extern int faultmgmtCurrentAlarmObjAttrGet(char *, int, char *);
extern zcfgRet_t faultmgmtCurrentAlarmObjAttrSet(char *, int, char *, int , struct json_object *);	

