/*Parameter*/
extern tr98Parameter_t para_DevInfo[];
extern tr98Parameter_t para_DevInfoVendorCfg[];
extern tr98Parameter_t para_ProcSt[];
extern tr98Parameter_t para_ProcStPs[];
extern tr98Parameter_t para_MemoryStatus[];

#ifdef ZYXEL_CELLWAN_WITH_LTE_MODULE_TEMPERATURE
extern tr98Parameter_t para_TempSt[];
extern tr98Parameter_t para_TempStSensor[];
#endif

/*Handler Function*/
extern zcfgRet_t devInfoObjGet(char *, int, struct json_object **, bool);
extern zcfgRet_t devInfoObjSet(char *, int , struct json_object *, struct json_object *);
extern zcfgRet_t devInfoObjNotify(char *, char *, struct json_object *, int, struct json_object **);
extern int devInfoObjAttrGet(char *, int , char *);
extern zcfgRet_t devInfoObjAttrSet(char *, int , char *, int , struct json_object *);
extern zcfgRet_t devInfoVendorCfgObjGet(char *, int, struct json_object **, bool);
extern zcfgRet_t devInfoVendorCfgObjSet(char *, int , struct json_object *, struct json_object *);
extern zcfgRet_t devProcStGet(char *, int, struct json_object **, bool);
extern zcfgRet_t devProcStPsGet(char *, int, struct json_object **, bool);
/*InternetGatewayDevice.DeviceInfo.MemoryStatus*/
extern zcfgRet_t memoryStatusObjGet(char *, int, struct json_object **, bool);

#ifdef ZYXEL_CELLWAN_WITH_LTE_MODULE_TEMPERATURE
extern zcfgRet_t devTempStGet(char *tr98FullPathName, int handler, struct json_object **tr98Jobj, bool updateFlag);
extern zcfgRet_t devTempStNotify(char *, char *, struct json_object *, int, struct json_object **);
extern int devTempStAttrGet(char *, int, char *);
extern zcfgRet_t devTempStAttrSet(char *, int, char *, int , struct json_object *);

extern zcfgRet_t devTempStSensorGet(char *tr98FullPathName, int handler, struct json_object **tr98Jobj, bool updateFlag);
extern zcfgRet_t devTempStSensorSet(char *tr98FullPathName, int handler, struct json_object *tr98Jobj, struct json_object *multiJobj);
extern zcfgRet_t devTempStSensorAdd(char *tr98FullPathName, int *idx);
extern zcfgRet_t devTempStSensorDel(char *tr98FullPathName);
extern zcfgRet_t devTempStSensorNotify(char *, char *, struct json_object *, int, struct json_object **);
extern int devTempStSensorAttrGet(char *, int, char *);
extern zcfgRet_t devTempStSensorAttrSet(char *, int, char *, int , struct json_object *);
#endif

