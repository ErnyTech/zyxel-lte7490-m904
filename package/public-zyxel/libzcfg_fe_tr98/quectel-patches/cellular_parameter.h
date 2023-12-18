
tr98Parameter_t para_cellular[] = {

	{ NULL, 0, 0}
};

tr98Parameter_t para_cellularAccessPoint[] = {
	{ "APN", PARAMETER_ATTR_WRITE, 100, json_type_string},
	{ "Enable",PARAMETER_ATTR_WRITE, 0, json_type_boolean},
	{ "X_ZYXEL_ManualAPN",PARAMETER_ATTR_WRITE,0,json_type_boolean},
	{ NULL, 0, 0}
};

tr98Parameter_t para_cellularInterface[] = {
	{ "Enable", PARAMETER_ATTR_WRITE, 0, json_type_boolean},
	{ "Status", PARAMETER_ATTR_READONLY, 16, json_type_string},
	{ "IMEI", PARAMETER_ATTR_READONLY, 15, json_type_string},
	{ "CurrentAccessTechnology", PARAMETER_ATTR_READONLY, 16, json_type_string},
	{ "NetworkInUse", PARAMETER_ATTR_READONLY, 64, json_type_string},
	{ "RSSI", PARAMETER_ATTR_READONLY, 0, json_type_int},
	{ "X_ZYXEL_CurrentBand", PARAMETER_ATTR_READONLY, 1024, json_type_string},
	{ "X_ZYXEL_CellID", PARAMETER_ATTR_READONLY, 0, json_type_uint32},
	{ "X_ZYXEL_RFCN", PARAMETER_ATTR_READONLY, 0, json_type_uint32},
	{ "X_ZYXEL_RSRP", PARAMETER_ATTR_READONLY, 0, json_type_int},
	{ "X_ZYXEL_RSRQ", PARAMETER_ATTR_READONLY, 0, json_type_int},
	{ "X_ZYXEL_PhyCellID", PARAMETER_ATTR_READONLY, 0, json_type_int},
	{ "X_ZYXEL_TAC", PARAMETER_ATTR_READONLY, 0, json_type_uint32},
	{ "X_ZYXEL_SINR", PARAMETER_ATTR_READONLY, 0, json_type_int},
	{ "X_ZYXEL_CQI", PARAMETER_ATTR_READONLY, 0, json_type_int},
	{ "X_ZYXEL_MCS", PARAMETER_ATTR_READONLY, 0, json_type_int},
	{ "X_ZYXEL_ModuleSoftwareVersion", PARAMETER_ATTR_READONLY, 128, json_type_string},
	{ "X_ZYXEL_ModuleSoftwareSubVersion", PARAMETER_ATTR_READONLY, 128, json_type_string},
	{ NULL, 0, 0}
};

tr98Parameter_t para_cellularInterfaceSCC[] = {
	{ "PhysicalCellID", PARAMETER_ATTR_READONLY, 0, json_type_int},
	{ "RFCN", PARAMETER_ATTR_READONLY, 0, json_type_uint32},
	{ "Band", PARAMETER_ATTR_READONLY, 1024, json_type_string},
	{ "RSSI", PARAMETER_ATTR_READONLY, 0, json_type_int},
	{ "RSRP", PARAMETER_ATTR_READONLY, 0, json_type_int},
	{ "RSRQ", PARAMETER_ATTR_READONLY, 0, json_type_int},
	{ "SINR", PARAMETER_ATTR_READONLY, 0, json_type_int},
	{ NULL, 0, 0, 0}
};

tr98Parameter_t para_cellularInterfaceUSIM[] = {//Jessie
	{ "Status", PARAMETER_ATTR_READONLY, 16, json_type_string},
	{ "IMSI", PARAMETER_ATTR_READONLY, 16, json_type_string},
	{ "ICCID", PARAMETER_ATTR_READONLY, 20, json_type_string},
	{ "MSISDN", PARAMETER_ATTR_READONLY, 15, json_type_string},
	{ NULL, 0, 0, 0}
};
tr98Parameter_t para_cellularInterfaceIPPT[] = {
        { "Enable", PARAMETER_ATTR_WRITE, 0, json_type_boolean},
        { "ConnectionMode", PARAMETER_ATTR_WRITE, 16, json_type_string},
        { "MACAddress", PARAMETER_ATTR_WRITE, 17, json_type_string},
        { "ProxyARP_SubnetPrefix", PARAMETER_ATTR_WRITE, 0, json_type_int},
	{ "StaticGwEn", PARAMETER_ATTR_WRITE, 0, json_type_boolean},
	{ "StaticGwIPAddress", PARAMETER_ATTR_WRITE, 15, json_type_string},
	{ "DHCP_LeaseTime", PARAMETER_ATTR_WRITE, 0, json_type_int},
        { NULL, 0, 0, 0}
};
tr98Parameter_t para_CellZyGnssLoc[] = {
	{ "Enable", PARAMETER_ATTR_WRITE, 0, json_type_boolean},
	{ "ScanOnBoot", PARAMETER_ATTR_WRITE, 0, json_type_boolean},
	{ "DisplayFormat", PARAMETER_ATTR_READONLY, 0, json_type_uint32},
	{ "ScanStatus", PARAMETER_ATTR_READONLY, 0, json_type_int},
	{ "Latitude", PARAMETER_ATTR_READONLY, 0, json_type_int},
	{ "Longitude", PARAMETER_ATTR_READONLY, 0, json_type_int},
	{ "HDOP", PARAMETER_ATTR_READONLY, 0, json_type_uint32},
	{ "Elevation", PARAMETER_ATTR_READONLY, 0, json_type_int},
	{ "PositioningMode", PARAMETER_ATTR_READONLY, 0, json_type_uint32},
	{ "CourseOverGround", PARAMETER_ATTR_READONLY, 0, json_type_uint32},
	{ "SpeedOverGround", PARAMETER_ATTR_READONLY, 0, json_type_uint32},
	{ "LastFixTime", PARAMETER_ATTR_READONLY, 32, json_type_string},
	{ "NumberOfSatellites", PARAMETER_ATTR_READONLY, 0, json_type_uint32},
	{NULL,0,0,0}
};
tr98Parameter_t para_CellZyNBR[] = {
        { "Enable", PARAMETER_ATTR_WRITE, 0, json_type_boolean},
        {NULL,0,0,0}
};
tr98Parameter_t para_neighborCell[] = {
	{"Enable", PARAMETER_ATTR_WRITE, 0, json_type_boolean},
	{ "NeighbourType", PARAMETER_ATTR_READONLY, 32, json_type_string},
	{ "ConnectionMode", PARAMETER_ATTR_READONLY, 16, json_type_string},
	{ "PhyCellID", PARAMETER_ATTR_READONLY, 0, json_type_int},
	{ "RFCN", PARAMETER_ATTR_READONLY, 0, json_type_uint32},
	{ "RSSI", PARAMETER_ATTR_READONLY, 0, json_type_uint32},
	{ "RSRP", PARAMETER_ATTR_READONLY, 0, json_type_uint32},
	{ "RSRQ", PARAMETER_ATTR_READONLY, 0, json_type_uint32},
	{ "SINR", PARAMETER_ATTR_READONLY, 0, json_type_uint32},
	{ NULL,0,0,0}
};
