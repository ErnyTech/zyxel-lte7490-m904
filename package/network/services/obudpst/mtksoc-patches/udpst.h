/*
 * Copyright (c) 2020, Broadband Forum
 * Copyright (c) 2020, AT&T Communications
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its
 *    contributors may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 *
 * UDP Speed Test - udpst.h
 *
 * This file contains all the primary software-specific constants and structure
 * definitions.
 *
 */

#ifndef UDPST_H
#define UDPST_H

#include "config.h"
#include "udpst_common.h"

//----------------------------------------------------------------------------
//
// General
//
#define SOFTWARE_TITLE    "UDP Speed Test"
#define USTEST_TEXT       "Upstream"
#define DSTEST_TEXT       "Downstream"
#define TIME_FORMAT       "%Y-%m-%d %H:%M:%S"
#define STRING_SIZE       1024 // String buffer size
#define AUTH_KEY_SIZE     32   // Authentication key size
#define HS_DELTA_BACKUP   3    // High-speed delta backup multiplier
#define MAX_EPOLL_EVENTS  8    // Max epoll events handled at one time
#define MAX_CONNECTIONS   128  // Max simultaneous connections
#define AUTH_TIME_WINDOW  150  // Authentication +/- time windows (sec)
#define AUTH_ENFORCE_TIME TRUE // Enforce authentication time window
#define WARNING_MSG_LIMIT 50   // Warning message limit
#define WARNING_NOTRAFFIC 1    // Receive traffic stopped warning threshold (sec)
#define TIMEOUT_NOTRAFFIC (WARNING_NOTRAFFIC + 2)
#define STATUS_SUCCESS    0  // Success (test completed without incident)
#define STATUS_WARNING    1  // Warning or soft error (test runs but with anomaly)
#define STATUS_ERROR      -1 // Failure or hard error (test designated as failure)
// Alternative for INET6_ADDRSTRLEN (allows for '%' and textual Zone ID)
#define INET6_ADDR_STRLEN (INET6_ADDRSTRLEN + 1 + IFNAMSIZ)

// DISABLE_INT_TIMER disables the interval timer when compiling for client
// devices that are unable to support the required clock resolution. Because
// this results in high CPU utilization, it is not recommended for standard
// server operation.
#ifndef DISABLE_INT_TIMER
#define SEND_TIMER_ADJ 75 // Data send timer adjustment (us)
#else
#define SEND_TIMER_ADJ 0 // Set to zero when interval timer is disabled
#endif

//#ifdef ZCFG_SUPPORT
#if 1
#define DIAGNOSTIC_OUTPUT_RESULT_PATH "/var/diagJsonResult"
#define DIAGNOSTIC_CONSOLE_RESULT_PATH "/var/diagResult"
#endif
//----------------------------------------------------------------------------
//
// Default and min/max parameter values
//
#define DEF_JUMBO_STATUS     TRUE       // Enable/disable jumbo datagram sizes
#define DEF_USE_OWDELVAR     FALSE      // Use one-way delay instead of RTT
//ifdef ZCFG_SUPPORT
#if 1
#define DEF_DISABLE_IPRR     FALSE      // Enables IPRR (IP packet Reordering Ratio) metric. The default value SHOULD be false.
#define DEF_DISABLE_RIPR     FALSE      // Enables RIPRR (Replicated IP Packet Ratio) metric. The default value SHOULD be false
#endif
#define DEF_IGNORE_OOODUP    FALSE      // Ignore Out-of-Order/Duplicate datagrams
#define DEF_IPTOS_BYTE       0          // IP ToS byte for testing
#define MIN_IPTOS_BYTE       0          //
#define MAX_IPTOS_BYTE       UINT8_MAX  //
#define DEF_SRINDEX_CONF     UINT16_MAX // Sending rate index, <Auto> = UINT16_MAX
#define MIN_SRINDEX_CONF     0          //
#define MAX_SRINDEX_CONF     (MAX_SENDING_RATES - 1)
#define SRIDX_ISSTART_PREFIX '@'        // Prefix char for sending rate starting point
#define DEF_TESTINT_TIME     10         // Test interval time (sec)
#define MIN_TESTINT_TIME     5          //
#define MAX_TESTINT_TIME     3600       //
#define DEF_SUBINT_PERIOD    1          // Sub-interval period (sec)
#define MIN_SUBINT_PERIOD    1          //
#define MAX_SUBINT_PERIOD    10         //
#define DEF_CONTROL_PORT     25000      // Control port
#define MIN_CONTROL_PORT     0          // (0 = Random UDP ephemeral port)
#define MAX_CONTROL_PORT     UINT16_MAX //
#define DEF_BIMODAL_COUNT    0          // Bimodal initial sub-interval count
#define MIN_BIMODAL_COUNT    1          //
#define MAX_BIMODAL_COUNT    (MAX_TESTINT_TIME / MIN_SUBINT_PERIOD)
#define DEF_SOCKET_BUF       1024000        // Socket buffer to request
#define MIN_SOCKET_BUF       0              // (0 = System default/minimum)
#define MAX_SOCKET_BUF       16777216       //
#define DEF_LOW_THRESH       30             // Low delay variation threshold (ms)
#define MIN_LOW_THRESH       1              //
#define MAX_LOW_THRESH       10000          //
#define DEF_UPPER_THRESH     90             // Upper delay variation threshold (ms)
#define MIN_UPPER_THRESH     1              //
#define MAX_UPPER_THRESH     10000          //
#define DEF_TRIAL_INT        50             // Status feedback/trial interval (ms)
#define MIN_TRIAL_INT        5              //
#define MAX_TRIAL_INT        250            //
#define DEF_SLOW_ADJ_TH      3              // Slow adjustment threshold
#define MIN_SLOW_ADJ_TH      1              //
#define MAX_SLOW_ADJ_TH      UINT16_MAX     //
#define DEF_HS_DELTA         10             // High-speed delta (rows)
#define MIN_HS_DELTA         1              //
#define MAX_HS_DELTA         UINT8_MAX      //
#define DEF_SEQ_ERR_TH       10             // Sequence error threshold
#define MIN_SEQ_ERR_TH       0              //
#define MAX_SEQ_ERR_TH       UINT16_MAX     //
#define DEF_LOGFILE_MAX      1000           // Log file max size (KBytes)
#define MIN_LOGFILE_MAX      10             //
#define MAX_LOGFILE_MAX      1000000        //
#define MIN_REQUIRED_BW      1              // Required OR available bandwidth (Mbps)
#define MAX_CLIENT_BW        10000          //
#define MAX_SERVER_BW        100000         //
#define DEF_RA_ALGO          CHTA_RA_ALGO_B // Default rate adjustment algorithm

//----------------------------------------------------------------------------
//
// Sending rate payload, protocol, and buffer sizes
//
#define MAX_SENDING_RATES 1109                   // Max rows in sending rate table
#define BASE_SEND_TIMER1  MIN_INTERVAL_USEC      // Base send timer, transmitter 1 (us)
#define BASE_SEND_TIMER2  1000                   // Base send timer, transmitter 2 (us)
#define MAX_L3_PACKET     1250                   // Max desired L3 packet size
#define MAX_JL3_PACKET    9000                   // Max desired jumbo L3 packet
#define MAX_TL3_PACKET    1500                   // Max desired traditional L3 packet
#define L3DG_OVERHEAD     (8 + 20)               // UDP + IPv4
#define L2DG_OVERHEAD     (8 + 20 + 14)          // UDP + IPv4 + Eth(NoFCS)
#define L1DG_OVERHEAD     (8 + 20 + 18 + 20)     // UDP + IPv4 + Eth(w/FCS) + Preamble/IFG
#define L0DG_OVERHEAD     (8 + 20 + 18 + 20 + 4) // UDP + IPv4 + Eth(w/FCS) + Preamble/IFG + VLAN
#define IPV6_ADDSIZE      20                     // IPv6 additional size (over IPv4)
#define MIN_PAYLOAD_SIZE  (sizeof(struct loadHdr) + IPV6_ADDSIZE)
#define MAX_PAYLOAD_SIZE  (MAX_L3_PACKET - L3DG_OVERHEAD)
#define MAX_JPAYLOAD_SIZE (MAX_JL3_PACKET - L3DG_OVERHEAD)
#define MAX_TPAYLOAD_SIZE (MAX_TL3_PACKET - L3DG_OVERHEAD)
//
// The send buffer needs to contain all the datagram payloads for a burst.
//
#define SND_BUFFER_SIZE (MAX_BURST_SIZE * MAX_JPAYLOAD_SIZE)
#define DEF_BUFFER_SIZE 65536

//----------------------------------------------------------------------------
//
// File descriptor flags and file modes
//
#define LOGFILE_FLAGS (O_WRONLY | O_CREAT | O_APPEND | O_NONBLOCK)
#define LOGFILE_MODE  (S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH)

//----------------------------------------------------------------------------
//
// Socket management actions
//
#define SMA_LOOKUP 0
#define SMA_BIND   1
#define SMA_UPDATE 2

//----------------------------------------------------------------------------
//
// Rate Adjustment Algorithms
//
#define RETRY_THRESH_ALGOC 5 // AlgoC: Initial retry threshold

//----------------------------------------------------------------------------
// Data structures
//----------------------------------------------------------------------------
//
// Configuration structure for settings and parameters
//
struct configuration {
        BOOL usTesting;                  // Upstream testing requested
        BOOL dsTesting;                  // Downstream testing requested
        int addrFamily;                  // Address family
        BOOL ipv4Only;                   // Only allow IPv4 testing
        BOOL ipv6Only;                   // Only allow IPv6 testing
        BOOL isDaemon;                   // Execute as daemon
        BOOL errSuppress;                // Suppress send/receive errors
        BOOL verbose;                    // Enable verbose messaging
        BOOL summaryOnly;                // Do not show sub-interval stats
        BOOL jsonOutput;                 // JSON Output format
        BOOL jsonBrief;                  // JSON Output should be minimized
        BOOL jsonFormatted;              // JSON Output should be formatted
        BOOL jumboStatus;                // Enable/disable jumbo datagram sizes
        BOOL traditionalMTU;             // Traditional (1500 byte) MTU
        BOOL debug;                      // Enable debug messaging
        BOOL randPayload;                // Payload randomization
        int rateAdjAlgo;                 // Rate adjustment algorithm
        BOOL showSendingRates;           // Display sending rate table parameters
        BOOL showLossRatio;              // Display loss ratio
        int bimodalCount;                // Bimodal initial sub-interval count
        BOOL useOwDelVar;                // Use one-way delay instead of RTT
        BOOL ignoreOooDup;               // Ignore Out-of-Order/Duplicate datagrams
        char authKey[AUTH_KEY_SIZE + 1]; // Authentication key
        int ipTosByte;                   // IP ToS byte for testing
        int srIndexConf;                 // Configured sending rate index
        BOOL srIndexIsStart;             // Configured SR index is starting point
        int testIntTime;                 // Test interval time (sec)
        int subIntPeriod;                // Sub-interval period (sec)
        int controlPort;                 // Control port number for setup requests
        int sockSndBuf;                  // Socket send buffer size
        int sockRcvBuf;                  // Socket receive buffer size
        int lowThresh;                   // Low delay variation threshold
        int upperThresh;                 // Upper delay variation threshold
        int trialInt;                    // Status feedback/trial interval (ms)
        int slowAdjThresh;               // Slow rate adjustment threshold
        int highSpeedDelta;              // High-speed row adjustment delta
        int seqErrThresh;                // Sequence error threshold
        int maxBandwidth;                // Required OR available bandwidth
        BOOL intfForMax;                 // Local interface used for maximum
        char intfName[IFNAMSIZ + 1];     // Local interface for supplemental stats
        int logFileMax;                  // Maximum log file size
        char *logFile;                   // Name of log file
        //#ifdef ZCFG_SUPPORT
        #if 1
        int tmax;                   	// seconds. Maximum waiting time for packets to arrive.
        int tmaxrtt;					// seconds. Maximum Round Trip Time waiting time for packets to arrive
        int minpayload;
        int maxpayload;
		int PortMin;
		int PortMax;
		int PortOptionalMin;
		int PortOptionalMax;
        int timeoutnotesttraffic;		// seconds.
        int timeoutnostatusmessage;		// seconds.
        char bindIf[IFNAMSIZ + 1];
		BOOL showSimpleSendingRates;           // Display sending rate table parameters
		BOOL IPRREnable;
		BOOL RIPREnable;
        #endif
};
//----------------------------------------------------------------------------
//
// Repository of global variables and structures
//
struct repository {
        struct timespec systemClock;      // Clock reference (CLOCK_REALTIME)
        int epollFD;                      // Epoll file descriptor
        int maxConnIndex;                 // Largest (current) connection index
        struct sendingRate *sendingRates; // Sending rate table (array)
        int maxSendingRates;              // Size (rows) of sending rate table
        char *sndBuffer;                  // Send buffer for load PDUs
        char *defBuffer;                  // Default buffer for general I/O
        char *randData;                   // Randomized seed data
        char *sndBufRand;                 // Send buffer for randomized load PDUs
        int rcvDataSize;                  // Received data size in default buffer
        struct sockaddr_storage remSas;   // Remote IP sockaddr storage
        socklen_t remSasLen;              // Remote IP sockaddr storage length
        BOOL isServer;                    // Execute as server
        char serverIp[INET6_ADDR_STRLEN]; // Server IP address
        char *serverName;                 // Server hostname or IP address
        int hSpeedThresh;                 // Index of high-speed threshold
        int logFileSize;                  // Current log file size
        int usBandwidth;                  // Current upstream bandwidth
        int dsBandwidth;                  // Current downstream bandwidth
        int endTimeStatus;                // Exit status when end time expires
};
//----------------------------------------------------------------------------
//
// Test totals and overall test statistics
//
struct testSummary {
        unsigned int rxDatagrams; // Total rx datagrams
        unsigned int seqErrLoss;  // Loss sum
        unsigned int seqErrOoo;   // Out-of-Order sum
        unsigned int seqErrDup;   // Duplicate sum
        unsigned int delayVarMin; // Delay variation minimum
        unsigned int delayVarMax; // Delay variation maximum
        unsigned int delayVarSum; // Delay variation sum
        unsigned int rttMinimum;  // Minimum round-trip time
        unsigned int rttMaximum;  // Maximum round-trip time
        double rateSumL3;         // Rate sum at L3
        double rateSumIntf;       // Rate sum of local interface
        unsigned int sampleCount; // Sample count
};
//----------------------------------------------------------------------------
//
// Data structure representing a connection to a device, file, socket, etc.
//
struct connection {
        int fd; // File descriptor
#define T_UNKNOWN  0
#define T_UDP      1
#define T_CONSOLE  2
#define T_LOG      3
#define T_NULL     4
#define T_MAXTYPES 5
        int type;       // Connection type
        int subType;    // Connection subtype
        BOOL connected; // Socket was connected
#define S_FREE      0
#define S_CREATED   1
#define S_BOUND     2
#define S_LISTEN    3
#define S_CONNPEN   4
#define S_DATA      5
#define S_MAXSTATES 6
        int state; // Current state
#define TEST_TYPE_UNK 0
#define TEST_TYPE_US  1
#define TEST_TYPE_DS  2
        int testType;                    // Test type being executed
        int testAction;                  // Test action (see load header)
        int ipProtocol;                  // IPPROTO_IP or IPPROTO_IPV6
        int ipTosByte;                   // IP ToS byte for testing
        char locAddr[INET6_ADDR_STRLEN]; // Local IP address as string
        int locPort;                     // Local port
        char remAddr[INET6_ADDR_STRLEN]; // Remote IP address as string
        int remPort;                     // Remote port
        //
        int srIndex;                 // Sending rate index
        struct sendingRate srStruct; // Sending rate structure
        unsigned int lpduSeqNo;      // Load PDU sequence number
        unsigned int spduSeqNo;      // Status PDU sequence number
        int spduSeqErr;              // Status PDU sequence error count
        //
        int protocolVer;     // Protocol version
        int maxBandwidth;    // Required bandwidth
        int lowThresh;       // Low delay variation threshold
        int upperThresh;     // Upper delay variation threshold
        int slowAdjThresh;   // Slow rate adjustment threshold
        int slowAdjCount;    // Slow rate adjustment counter
        int trialInt;        // Status feedback/trial interval (ms)
        int testIntTime;     // Test interval time (sec)
        int subIntPeriod;    // Sub-interval period (sec)
        int srIndexConf;     // Configured sending rate index
        BOOL srIndexIsStart; // Configured SR index is starting point
        int highSpeedDelta;  // High-speed row adjustment delta
        int seqErrThresh;    // Sequence error threshold
        BOOL randPayload;    // Payload randomization
        int rateAdjAlgo;     // Rate adjustment algorithm
        //
        int algoCRetryCount;  // AlgoC: Waiting timer till next multiplicative retry
        int algoCRetryThresh; // AlgoC: Threshold for multiplicative retry
        BOOL algoCUpdate;     // AlgoC: Indicates when max send rate was updated
        //
        struct timespec endTime;      // Connection end time
        int (*priAction)(int);        // Primary action upon IO
        int (*secAction)(int);        // Secondary action upon IO
        struct timespec timer1Thresh; // First timer threshold
        int (*timer1Action)(int);     // First action upon expiry
        struct timespec timer2Thresh; // Second timer threshold
        int (*timer2Action)(int);     // Second action upon expiry
        struct timespec timer3Thresh; // Third timer threshold
        int (*timer3Action)(int);     // Third action upon expiry
        //
        struct timespec subIntClock;  // Sub-interval clock
        unsigned int accumTime;       // Accumulated time
        unsigned int subIntSeqNo;     // Sub-interval sequence number
        struct subIntStats sisAct;    // Sub-interval active stats
        struct subIntStats sisSav;    // Sub-interval saved stats
        struct subIntStats sisMax[2]; // Sub-interval maximum stats (bimodal)
        double rateMax[2];            // Rate maximums (bimodal)
        struct timespec timeOfMax[2]; // Time of maximums (bimodal)
        int subIntCount;              // Sub-interval count
        //
#define LPDU_HISTORY_SIZE 32 // Size must be power of 2
#define LPDU_HISTORY_MASK (LPDU_HISTORY_SIZE - 1)
        unsigned int lpduHistBuf[LPDU_HISTORY_SIZE]; // History buffer of last seq numbers
        unsigned int lpduHistIdx;                    // History buffer index of next seq number
        BOOL ignoreOooDup;                           // Ignore Out-of-Order/Duplicate datagrams
        unsigned int seqErrLoss;                     // Loss sum
        unsigned int seqErrOoo;                      // Out-of-Order sum
        unsigned int seqErrDup;                      // Duplicate sum
        //
        BOOL useOwDelVar;         // Use one-way delay instead of RTT
        int clockDeltaMin;        // Clock delta minimum
        unsigned int delayVarMin; // Delay variation minimum
        unsigned int delayVarMax; // Delay variation maximum
        unsigned int delayVarSum; // Delay variation sum
        unsigned int delayVarCnt; // Delay variation count
        unsigned int rttMinimum;  // Minimum round-trip time (sampled)
        unsigned int rttSample;   // Last round-trip time (sampled)
        BOOL delayMinUpd;         // Delay minimum(s) updated
        //
        struct timespec trialIntClock; // Trial interval clock
        unsigned int tiDeltaTime;      // Trial interval delta time
        unsigned int tiRxDatagrams;    // Trial interval receive datagrams
        unsigned int tiRxBytes;        // Trial interval receive bytes
        //
        int warningCount;           // Warning message count
        BOOL rxStoppedLoc;          // Local receive traffic stopped indicator
        BOOL rxStoppedRem;          // Remote receive traffic stopped indicator
        struct timespec pduRxTime;  // Receive time of last load or status PDU
        struct timespec spduTime;   // Send time in last received status PDU
        struct testSummary testSum; // Test summary statistics
        //
        cJSON *json_siArray;      // JSON sub-interval array
        int intfFD;               // File descriptor to read interface stats
        unsigned long intfBytes;  // Last byte counter of interface stats
        struct timespec intfTime; // Sample time of interface stats
        double intfMax[2];        // Interface maximums (bimodal)
        //#ifdef ZCFG_SUPPORT
        #if 1
        int tmax;                   	// seconds. Maximum waiting time for packets to arrive.
        int tmaxrtt;					// seconds. Maximum Round Trip Time waiting time for packets to arrive
        int minpayload;
        int maxpayload;
        int timeoutnotesttraffic;		// seconds.
        int timeoutnostatusmessage;		// seconds.
        #endif
};
//----------------------------------------------------------------------------

#endif /* UDPST_H */
