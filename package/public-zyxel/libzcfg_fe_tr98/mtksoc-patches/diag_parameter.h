tr98Parameter_t para_IpPingDiag[] = {
	{ "DiagnosticsState", PARAMETER_ATTR_WRITE, 29, json_type_string},
	{ "Interface", PARAMETER_ATTR_WRITE, 257, json_type_string},
	{ "Host", PARAMETER_ATTR_WRITE, 257, json_type_string},
	{ "NumberOfRepetitions", PARAMETER_ATTR_WRITE, 0, json_type_uint32},
	{ "Timeout", PARAMETER_ATTR_WRITE, 0, json_type_uint32},
	{ "DataBlockSize", PARAMETER_ATTR_WRITE, 0, json_type_uint32},
	{ "DSCP", PARAMETER_ATTR_WRITE, 0, json_type_uint32},
	{ "SuccessCount", PARAMETER_ATTR_READONLY, 0, json_type_uint32},
	{ "FailureCount", PARAMETER_ATTR_READONLY, 0, json_type_uint32},
	{ "AverageResponseTime", PARAMETER_ATTR_READONLY, 0, json_type_uint32},
	{ "MinimumResponseTime", PARAMETER_ATTR_READONLY, 0, json_type_uint32},
	{ "MaximumResponseTime", PARAMETER_ATTR_READONLY, 0, json_type_uint32},
	{ NULL, 0, 0, 0}
};

tr98Parameter_t para_TraceRtDiag[] = {
	{ "DiagnosticsState", PARAMETER_ATTR_WRITE, 29, json_type_string},
	{ "Interface", PARAMETER_ATTR_WRITE, 257, json_type_string},
	{ "Host", PARAMETER_ATTR_WRITE, 257, json_type_string},
	{ "NumberOfTries", PARAMETER_ATTR_WRITE, 0, json_type_uint32},
	{ "Timeout", PARAMETER_ATTR_WRITE, 0, json_type_uint32},
	{ "DataBlockSize", PARAMETER_ATTR_WRITE, 0, json_type_uint32},
	{ "DSCP", PARAMETER_ATTR_WRITE, 0, json_type_uint32},
	{ "MaxHopCount", PARAMETER_ATTR_WRITE, 0, json_type_uint32},
	{ "ResponseTime", PARAMETER_ATTR_READONLY, 0, json_type_uint32},
	{ "RouteHopsNumberOfEntries", PARAMETER_ATTR_READONLY, 0, json_type_uint32},
	{ NULL, 0, 0, 0}
};

tr98Parameter_t para_RtHop[] = {
	{ "HopHost", PARAMETER_ATTR_READONLY, 257, json_type_string},
	{ "HopHostAddress", PARAMETER_ATTR_READONLY, 33, json_type_string},
	{ "HopErrorCode", PARAMETER_ATTR_READONLY, 0, json_type_uint32},
	{ "HopRTTimes", PARAMETER_ATTR_READONLY, 17, json_type_string},
	{ NULL, 0, 0}
};

tr98Parameter_t para_PerpDiag[] = {
	{ "DownloadTransports", PARAMETER_ATTR_READONLY, 17, json_type_string},
	{ "UploadTransports", PARAMETER_ATTR_READONLY, 17, json_type_string},
	{ NULL, 0, 0}
};

tr98Parameter_t para_DlDiag[] = {
	{ "DiagnosticsState", PARAMETER_ATTR_WRITE, 257, json_type_string},
	{ "Interface", PARAMETER_ATTR_WRITE, 257, json_type_string},
	{ "DownloadURL", PARAMETER_ATTR_WRITE, 257, json_type_string},
	{ "DSCP", PARAMETER_ATTR_WRITE, 0, json_type_uint32},
	{ "EthernetPriority", PARAMETER_ATTR_WRITE, 0, json_type_uint32},
	{ "ROMTime", PARAMETER_ATTR_READONLY, 27, json_type_time},
	{ "BOMTime", PARAMETER_ATTR_READONLY, 27, json_type_time},
	{ "EOMTime", PARAMETER_ATTR_READONLY, 27, json_type_time},
	{ "TestBytesReceived", PARAMETER_ATTR_READONLY, 0, json_type_uint32},
	{ "TotalBytesReceived", PARAMETER_ATTR_READONLY, 0, json_type_uint32},
	{ "X_ZYXEL_TotalBytesSent", PARAMETER_ATTR_READONLY, 0, json_type_uint32},
#ifdef ZYXEL_DL_DIAG_CALCULATION
	{ "X_ZYXEL_Latency", PARAMETER_ATTR_READONLY, 17, json_type_string},
	{ "X_ZYXEL_RoundTripTime", PARAMETER_ATTR_READONLY, 0, json_type_uint32},
	{ "X_ZYXEL_DownloadSpeed", PARAMETER_ATTR_READONLY, 17, json_type_string},
#endif
	{ "TCPOpenRequestTime", PARAMETER_ATTR_READONLY, 27, json_type_time},
	{ "TCPOpenResponseTime", PARAMETER_ATTR_READONLY, 27, json_type_time},
	{ NULL, 0, 0}
};

tr98Parameter_t para_UlDiag[] = {
	{ "DiagnosticsState", PARAMETER_ATTR_WRITE, 257, json_type_string},
	{ "Interface", PARAMETER_ATTR_WRITE, 257, json_type_string},
	{ "UploadURL", PARAMETER_ATTR_WRITE, 257, json_type_string},
	{ "DSCP", PARAMETER_ATTR_WRITE, 0, json_type_uint32},
	{ "EthernetPriority", PARAMETER_ATTR_WRITE, 0, json_type_uint32},
	{ "TestFileLength", PARAMETER_ATTR_WRITE, 0, json_type_uint32},
	{ "ROMTime", PARAMETER_ATTR_READONLY, 27, json_type_time},
	{ "BOMTime", PARAMETER_ATTR_READONLY, 27, json_type_time},
	{ "EOMTime", PARAMETER_ATTR_READONLY, 27, json_type_time},
	{ "X_ZYXEL_TestBytesSent", PARAMETER_ATTR_READONLY, 0, json_type_uint32},
	{ "X_ZYXEL_TotalBytesReceived", PARAMETER_ATTR_READONLY, 0, json_type_uint32},
	{ "TotalBytesSent", PARAMETER_ATTR_READONLY, 0, json_type_uint32},
	{ "TCPOpenRequestTime", PARAMETER_ATTR_READONLY, 27, json_type_time},
	{ "TCPOpenResponseTime", PARAMETER_ATTR_READONLY, 27, json_type_time},
	{ NULL, 0, 0}
};

tr98Parameter_t para_UdpEchoCfg[] = {
	{ "Enable", PARAMETER_ATTR_WRITE, 0, json_type_boolean},
	{ "Interface", PARAMETER_ATTR_WRITE, 257, json_type_string},
	{ "SourceIPAddress", PARAMETER_ATTR_WRITE, 17, json_type_string},
	{ "UDPPort", PARAMETER_ATTR_WRITE, 0, json_type_uint32},
	{ "EchoPlusEnabled", PARAMETER_ATTR_WRITE, 0, json_type_boolean},
	{ "EchoPlusSupported", PARAMETER_ATTR_READONLY, 0, json_type_boolean},
	{ "PacketsReceived", PARAMETER_ATTR_READONLY, 0, json_type_uint32},
	{ "PacketsResponded", PARAMETER_ATTR_READONLY, 0, json_type_uint32},
	{ "BytesReceived", PARAMETER_ATTR_READONLY, 0, json_type_uint32},
	{ "BytesResponded", PARAMETER_ATTR_READONLY, 0, json_type_uint32},
	{ "TimeFirstPacketReceived", PARAMETER_ATTR_READONLY, 27, json_type_time},
	{ "TimeLastPacketReceived", PARAMETER_ATTR_READONLY, 27, json_type_time},
	{ NULL, 0, 0}
};

tr98Parameter_t para_UdpEchoDiag[] = {
	{ "DiagnosticsState", PARAMETER_ATTR_WRITE, 29, json_type_string},
	{ "Interface", PARAMETER_ATTR_WRITE, 257, json_type_string},
	{ "Host", PARAMETER_ATTR_WRITE, 257, json_type_string},
	{ "Port", PARAMETER_ATTR_WRITE, 0, json_type_uint32},
	{ "NumberOfRepetitions", PARAMETER_ATTR_WRITE, 0, json_type_uint32},
	{ "Timeout", PARAMETER_ATTR_WRITE, 0, json_type_uint32},
	{ "DataBlockSize", PARAMETER_ATTR_WRITE, 0, json_type_uint32},
	{ "DSCP", PARAMETER_ATTR_WRITE, 0, json_type_uint32},
	{ "InterTransmissionTime", PARAMETER_ATTR_WRITE, 0, json_type_uint32},
	{ "ProtocolVersion", PARAMETER_ATTR_WRITE, 9, json_type_string},
	{ "IPAddressUsed", PARAMETER_ATTR_WRITE, 46, json_type_string},
	{ "SuccessCount", PARAMETER_ATTR_READONLY, 0, json_type_uint32},
	{ "FailureCount", PARAMETER_ATTR_READONLY, 0, json_type_uint32},
	{ "AverageResponseTime", PARAMETER_ATTR_READONLY, 0, json_type_uint32},
	{ "MinimumResponseTime", PARAMETER_ATTR_READONLY, 0, json_type_uint32},
	{ "MaximumResponseTime", PARAMETER_ATTR_READONLY, 0, json_type_uint32},
	{ "EnableIndividualPacketResults", PARAMETER_ATTR_WRITE, 0, json_type_boolean},
	{ "IndividualPacketResultNumberOfEntries", PARAMETER_ATTR_READONLY, 0, json_type_uint32},
	{ "UDPEchoDiagnosticsMaxResults", PARAMETER_ATTR_READONLY, 0, json_type_uint32},
	{ NULL, 0, 0, 0}
};

tr98Parameter_t para_UdpEchoDiagPakRst[] = {
	{ "PacketSuccess", PARAMETER_ATTR_READONLY, 0, json_type_boolean},
	{ "PacketSendTime", PARAMETER_ATTR_READONLY, 27, json_type_time},
	{ "PacketReceiveTime", PARAMETER_ATTR_READONLY, 27, json_type_time},
	{ "TestGenSN", PARAMETER_ATTR_READONLY, 0, json_type_uint32},
	{ "TestRespSN", PARAMETER_ATTR_READONLY, 0, json_type_uint32},
	{ "TestRespRcvTimeStamp", PARAMETER_ATTR_READONLY, 0, json_type_uint32},
	{ "TestRespReplyTimeStamp", PARAMETER_ATTR_READONLY, 0, json_type_uint32},
	{ "TestRespReplyFailureCount", PARAMETER_ATTR_READONLY, 0, json_type_uint32},
	{ NULL, 0, 0, 0}
};
#ifdef ZYXEL_TR471_UDP_SPEED_TEST
tr98Parameter_t para_IpLayerCapMetrics[] = {
	{ "DiagnosticsState", PARAMETER_ATTR_WRITE, 33, json_type_string},
	{ "IPLayerMaxConnections", PARAMETER_ATTR_READONLY, 0, json_type_uint32},
	{ "IPLayerMaxIncrementalResult", PARAMETER_ATTR_READONLY, 0, json_type_uint32},
	{ "IPLayerCapSupportedSoftwareVersion", PARAMETER_ATTR_READONLY, 65, json_type_string},
	{ "IPLayerCapSupportedControlProtocolVersion", PARAMETER_ATTR_READONLY, 65, json_type_string},
	{ "IPLayerCapSupportedMetrics", PARAMETER_ATTR_READONLY, 33, json_type_string},
	{ "Interface", PARAMETER_ATTR_WRITE, 257, json_type_string},
	{ "Role", PARAMETER_ATTR_WRITE, 17, json_type_string},
	{ "Host", PARAMETER_ATTR_WRITE, 257, json_type_string},
	{ "Port", PARAMETER_ATTR_WRITE, 0, json_type_uint32},
	{ "JumboFramesPermitted", PARAMETER_ATTR_WRITE, 0, json_type_boolean},
	{ "NumberOfConnections", PARAMETER_ATTR_WRITE, 0, json_type_uint32},
	{ "EthernetPriority", PARAMETER_ATTR_WRITE, 0, json_type_uint32},
	{ "DSCP", PARAMETER_ATTR_WRITE, 0, json_type_uint32},
	{ "ProtocolVersion", PARAMETER_ATTR_WRITE, 8, json_type_string},
	{ "UDPPayloadMin", PARAMETER_ATTR_WRITE, 0, json_type_uint32},
	{ "UDPPayloadMax", PARAMETER_ATTR_WRITE, 0, json_type_uint32},
	{ "UDPPayloadContent", PARAMETER_ATTR_WRITE, 33, json_type_string},
	{ "PortMin", PARAMETER_ATTR_WRITE, 0, json_type_uint32},
	{ "PortMax", PARAMETER_ATTR_WRITE, 0, json_type_uint32},
	{ "PortOptionalMin", PARAMETER_ATTR_WRITE, 0, json_type_uint32},
	{ "PortOptionalMax", PARAMETER_ATTR_WRITE, 0, json_type_uint32},
	{ "TestType", PARAMETER_ATTR_WRITE, 9, json_type_string},
	{ "IPDVEnable", PARAMETER_ATTR_WRITE, 0, json_type_boolean},
	{ "IPRREnable", PARAMETER_ATTR_WRITE, 0, json_type_boolean},
	{ "RIPREnable", PARAMETER_ATTR_WRITE, 0, json_type_boolean},
	{ "PreambleDuration", PARAMETER_ATTR_WRITE, 0, json_type_uint32},
	{ "StartSendingRate", PARAMETER_ATTR_WRITE, 0, json_type_uint32},
	{ "NumberTestSubIntervals", PARAMETER_ATTR_WRITE, 0, json_type_uint32},
	{ "NumberFirstModeTestSubIntervals", PARAMETER_ATTR_WRITE, 0, json_type_uint32},
	{ "TestSubInterval", PARAMETER_ATTR_WRITE, 0, json_type_uint32},
	{ "StatusFeedbackInterval", PARAMETER_ATTR_WRITE, 0, json_type_uint32},
	{ "TimeoutNoTestTraffic", PARAMETER_ATTR_WRITE, 0, json_type_uint32},
	{ "TimeoutNoStatusMessage", PARAMETER_ATTR_WRITE, 0, json_type_uint32},
	{ "Tmax", PARAMETER_ATTR_WRITE, 0, json_type_uint32},
	{ "TmaxRTT", PARAMETER_ATTR_WRITE, 0, json_type_uint32},
	{ "TimestampResolution", PARAMETER_ATTR_WRITE, 0, json_type_uint32},
	{ "SeqErrThresh", PARAMETER_ATTR_WRITE, 0, json_type_uint32},
	{ "ReordDupIgnoreEnable", PARAMETER_ATTR_WRITE, 0, json_type_boolean},
	{ "LowerThresh", PARAMETER_ATTR_WRITE, 0, json_type_uint32},
	{ "UpperThresh", PARAMETER_ATTR_WRITE, 0, json_type_uint32},
	{ "HighSpeedDelta", PARAMETER_ATTR_WRITE, 0, json_type_uint32},
	{ "SlowAdjThresh", PARAMETER_ATTR_WRITE, 0, json_type_uint32},
	{ "HSpeedThresh", PARAMETER_ATTR_WRITE, 0, json_type_uint32},	
	{ "BOMTime", PARAMETER_ATTR_READONLY, 27, json_type_time},
	{ "EOMTime", PARAMETER_ATTR_READONLY, 27, json_type_time},
	{ "TmaxUsed", PARAMETER_ATTR_READONLY, 0, json_type_uint32},
	{ "TestInterval", PARAMETER_ATTR_READONLY, 0, json_type_uint32},
	{ "MaxIPLayerCapacity", PARAMETER_ATTR_READONLY, 97, json_type_string},
	{ "TimeOfMax", PARAMETER_ATTR_READONLY, 27, json_type_time},
	{ "MaxETHCapacityNoFCS", PARAMETER_ATTR_READONLY, 97, json_type_string},
	{ "MaxETHCapacityWithFCS", PARAMETER_ATTR_READONLY, 97, json_type_string},
	{ "MaxETHCapacityWithFCSVLAN", PARAMETER_ATTR_READONLY, 97, json_type_string},
	{ "LossRatioAtMax", PARAMETER_ATTR_READONLY, 97, json_type_string},
	{ "RTTRangeAtMax", PARAMETER_ATTR_READONLY, 97, json_type_string},
	{ "PDVRangeAtMax", PARAMETER_ATTR_READONLY, 97, json_type_string},
	{ "MinOnewayDelayAtMax", PARAMETER_ATTR_READONLY, 97, json_type_string},
	{ "ReorderedRatioAtMax", PARAMETER_ATTR_READONLY, 97, json_type_string},
	{ "ReplicatedRatioAtMax", PARAMETER_ATTR_READONLY, 97, json_type_string},
	{ "InterfaceEthMbpsAtMax", PARAMETER_ATTR_READONLY, 97, json_type_string},
	{ "IPLayerCapacitySummary", PARAMETER_ATTR_READONLY, 97, json_type_string},
	{ "LossRatioSummary", PARAMETER_ATTR_READONLY, 97, json_type_string},
	{ "RTTRangeSummary", PARAMETER_ATTR_READONLY, 97, json_type_string},
	{ "PDVRangeSummary", PARAMETER_ATTR_READONLY, 97, json_type_string},
	{ "MinOnewayDelaySummary", PARAMETER_ATTR_READONLY, 97, json_type_string},
	{ "MinRTTSummary", PARAMETER_ATTR_READONLY, 97, json_type_string},
	{ "ReorderedRatioSummary", PARAMETER_ATTR_READONLY, 97, json_type_string},
	{ "ReplicatedRatioSummary", PARAMETER_ATTR_READONLY, 97, json_type_string},
	{ "InterfaceEthMbpsSummary", PARAMETER_ATTR_READONLY, 97, json_type_string},
	{ "IncrementalResultNumberOfEntries", PARAMETER_ATTR_READONLY, 0, json_type_uint32},
	{ "ModalResultNumberOfEntries", PARAMETER_ATTR_READONLY, 0, json_type_uint32},
	{ "TmaxRTTUsed", PARAMETER_ATTR_READONLY, 0, json_type_uint32},
	{ "TimestampResolutionUsed", PARAMETER_ATTR_READONLY, 0, json_type_uint32},
	{ NULL, 0, 0}
};

tr98Parameter_t para_IpLayerIncreRes[] = {
	{ "IPLayerCapacity", PARAMETER_ATTR_READONLY, 97, json_type_string},
	{ "TimeOfSubInterval", PARAMETER_ATTR_READONLY, 27, json_type_time},
	{ "LossRatio", PARAMETER_ATTR_READONLY, 97, json_type_string},
	{ "RTTRange", PARAMETER_ATTR_READONLY, 97, json_type_string},
	{ "PDVRange", PARAMETER_ATTR_READONLY, 97, json_type_string},
	{ "MinOnewayDelay", PARAMETER_ATTR_READONLY, 97, json_type_string},
	{ "ReorderedRatio", PARAMETER_ATTR_READONLY, 97, json_type_string},
	{ "ReplicatedRatio", PARAMETER_ATTR_READONLY, 97, json_type_string},
	{ "InterfaceEthMbps", PARAMETER_ATTR_READONLY, 97, json_type_string},
	{ NULL, 0, 0}
};

tr98Parameter_t para_IpLayerModalRes[] = {
	{ "MaxIPLayerCapacity", PARAMETER_ATTR_READONLY, 97, json_type_string},
	{ "TimeOfMax", PARAMETER_ATTR_READONLY, 27, json_type_time},
	{ "MaxETHCapacityNoFCS", PARAMETER_ATTR_READONLY, 97, json_type_string},
	{ "MaxETHCapacityWithFCS", PARAMETER_ATTR_READONLY, 97, json_type_string},
	{ "MaxETHCapacityWithFCSVLAN", PARAMETER_ATTR_READONLY, 97, json_type_string},
	{ "LossRatioAtMax", PARAMETER_ATTR_READONLY, 97, json_type_string},
	{ "RTTRangeAtMax", PARAMETER_ATTR_READONLY, 97, json_type_string},
	{ "PDVRangeAtMax", PARAMETER_ATTR_READONLY, 97, json_type_string},
	{ "MinOnewayDelayAtMax", PARAMETER_ATTR_READONLY, 97, json_type_string},
	{ "ReorderedRatioAtMax", PARAMETER_ATTR_READONLY, 97, json_type_string},
	{ "ReplicatedRatioAtMax", PARAMETER_ATTR_READONLY, 97, json_type_string},
	{ "InterfaceEthMbpsAtMax", PARAMETER_ATTR_READONLY, 97, json_type_string},
	{ NULL, 0, 0}
};
#endif
