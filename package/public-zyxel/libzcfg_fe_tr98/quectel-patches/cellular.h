
/*Parameter*/
extern tr98Parameter_t para_cellular[];
extern tr98Parameter_t para_cellularAccessPoint[];
extern tr98Parameter_t para_cellularInterface[];
extern tr98Parameter_t para_cellularInterfaceSCC[];
extern tr98Parameter_t para_cellularInterfaceUSIM[];//Jessie
extern tr98Parameter_t para_cellularInterfaceIPPT[];
extern tr98Parameter_t para_CellZyGnssLoc[];
extern tr98Parameter_t para_CellZyNBR[];
extern tr98Parameter_t para_neighborCell[];
/*Handler Function*/

//InternetGatewayDevice.Cellular.AccessPoint.
extern zcfgRet_t cellularAccessPointObjGet(char *, int, struct json_object **, bool);
extern zcfgRet_t cellularAccessPointObjSet(char *, int , struct json_object *, struct json_object *);
extern zcfgRet_t cellularAccessPointObjAdd(char *, int *);
extern zcfgRet_t cellularAccessPointObjDel(char *);
extern zcfgRet_t cellularAccessPointObjNotify(char *, char *, struct json_object *, int, struct json_object **);
extern int cellularAccessPointObjAttrGet(char *, int, char *);
extern zcfgRet_t cellularAccessPointObjAttrSet(char *, int, char *, int , struct json_object *);

//InternetGatewayDevice.Cellular.Interface.
extern zcfgRet_t cellularInterfaceObjGet(char *, int, struct json_object **, bool);
extern zcfgRet_t cellularInterfaceObjSet(char *, int , struct json_object *, struct json_object *);
extern zcfgRet_t cellularInterfaceObjAdd(char *, int *);
extern zcfgRet_t cellularInterfaceObjDel(char *);
extern zcfgRet_t cellularInterfaceObjNotify(char *, char *, struct json_object *, int, struct json_object **);
extern int cellularInterfaceObjAttrGet(char *, int, char *);
extern zcfgRet_t cellularInterfaceObjAttrSet(char *, int, char *, int , struct json_object *);


//InternetGatewayDevice.Cellular.Interface.i.X_ZYXEL_SecondaryComponentCarrier
extern zcfgRet_t cellularInterfaceSCCObjGet(char *, int, struct json_object **, bool);
extern zcfgRet_t cellularInterfaceSCCObjSet(char *, int , struct json_object *, struct json_object *);
extern zcfgRet_t cellularInterfaceSCCObjAdd(char *, int *);
extern zcfgRet_t cellularInterfaceSCCObjDel(char *);
extern zcfgRet_t cellularInterfaceSCCObjNotify(char *, char *, struct json_object *, int, struct json_object **);
extern int cellularInterfaceSCCObjAttrGet(char *, int, char *);
extern zcfgRet_t cellularInterfaceSCCObjAttrSet(char *, int, char *, int , struct json_object *);


//InternetGatewayDevice.Cellular.Interface.i.USIM //Jessie
extern zcfgRet_t cellularInterfaceUSIMObjGet(char *, int, struct json_object **, bool);
extern zcfgRet_t cellularInterfaceUSIMObjSet(char *, int , struct json_object *, struct json_object *);
extern zcfgRet_t cellularInterfaceUSIMObjAdd(char *, int *);
extern zcfgRet_t cellularInterfaceUSIMObjDel(char *);
extern zcfgRet_t cellularInterfaceUSIMObjNotify(char *, char *, struct json_object *, int, struct json_object **);
extern int cellularInterfaceUSIMObjAttrGet(char *, int, char *);
extern zcfgRet_t cellularInterfaceUSIMObjAttrSet(char *, int, char *, int , struct json_object *);

//InternetGatewayDevice.Cellular.Interface.i.X_ZYXEL_IP_PassThrough
extern zcfgRet_t cellularInterfaceIPPTObjGet(char *, int, struct json_object **, bool);
extern zcfgRet_t cellularInterfaceIPPTObjSet(char *, int , struct json_object *, struct json_object *);

extern zcfgRet_t cellularInterfaceNeighborCellObjGet(char *, int, struct json_object **, bool);
extern zcfgRet_t cellularInterfaceNeighborCellObjSet(char *, int, char *, int , struct json_object *);
//"InternetGatewayDevice.Cellular.X_ZYXEL_GNSS_Location"
extern zcfgRet_t CellZyGnssLocObjGet(char *, int, struct json_object **, bool);
extern zcfgRet_t CellZyGnssLocObjSet(char *, int, char *, int , struct json_object *);
//"InternetGatewayDevice.Cellular.X_ZYXEL_NBR
extern zcfgRet_t CellZyNBRObjGet(char *, int, struct json_object **, bool);
extern zcfgRet_t CellZyNBRObjSet(char *, int, char *, int , struct json_object *);
