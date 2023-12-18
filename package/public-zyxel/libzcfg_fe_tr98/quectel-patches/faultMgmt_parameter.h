

tr98Parameter_t para_FaultMgmt[] = {
	{ "CurrentAlarmNumberOfEntries", PARAMETER_ATTR_READONLY, 0, json_type_uint32},
	{ NULL, 0, 0, 0}
};

tr98Parameter_t para_FaultMgmtCurrentAlarm[] = {
	{ "AlarmIdentifier", PARAMETER_ATTR_READONLY, 64, json_type_string},
	{ "AlarmRaisedTime", PARAMETER_ATTR_READONLY, 31, json_type_time},
	{ "AlarmChangedTime", PARAMETER_ATTR_READONLY, 31, json_type_time},
	{ "ManagedObjectInstance", PARAMETER_ATTR_READONLY, 512, json_type_string},
	{ "EventType", PARAMETER_ATTR_READONLY, 64, json_type_string},
	{ "ProbableCause", PARAMETER_ATTR_READONLY, 64, json_type_string},
	{ "SpecificProblem", PARAMETER_ATTR_READONLY, 128, json_type_string},
	{ "PerceivedSeverity", PARAMETER_ATTR_READONLY, 13, json_type_string},
	{ "AdditionalText", PARAMETER_ATTR_READONLY, 256, json_type_string},
	{ "AdditionalInformation", PARAMETER_ATTR_READONLY, 256, json_type_string},
	{ NULL, 0, 0, 0}
};

