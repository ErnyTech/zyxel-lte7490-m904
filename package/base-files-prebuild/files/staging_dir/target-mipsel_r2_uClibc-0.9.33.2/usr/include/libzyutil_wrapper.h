#ifndef _LIBZYUTIL_WRAPPER_H_
#define _LIBZYUTIL_WRAPPER_H_

#include <ctype.h>
#include <unistd.h>
#include <sys/syscall.h>
#include "zcfg_common.h"
#include "zos_api.h"

#define zyutil_dbg_printf(_fmt_, ...) \
    fprintf(stderr, "[PID %ld] %s line %d, %s(), " _fmt_, syscall(__NR_gettid), __FILE__, __LINE__, __FUNCTION__, ##__VA_ARGS__)

#ifdef SUPPORT_LANVLAN
zcfgRet_t zyUtilISetIfState(char *ifName, bool up);
#endif

#define zyUtilIGetSerailNum zyUtilIGetSerialNumber
zcfgRet_t zyUtilIGetBaseMAC(char *mac);
zcfgRet_t zyUtilIGetBaseMACwithColon(char *mac);
zcfgRet_t zyUtilIGetNumberOfMAC(int *num);
zcfgRet_t zyUtilIGetNthMAC(char *mac, int nth);
zcfgRet_t zyUtilIGetNthMACwithColon(char *mac, int nth);
zcfgRet_t zyUtilIGetModelID(char *model_id);
zcfgRet_t zyUtilIGetProductName(char *pdname);
zcfgRet_t zyUtilIGetFirmwareVersion(char *fwversion);
zcfgRet_t zyUtilIGetExternalFirmwareVersion(char *fwversion);
zcfgRet_t zyUtilIGetSerialNumber(char *serianumber);
zcfgRet_t zyUtilIGetSubSerialNumber(char *sub_serianumber);
zcfgRet_t zyUtilIGetDSLVersion(char *DSLversion);
zcfgRet_t zyUtilIGetBootingFlag(int *boot_flag);
#ifdef ZYXEL_FIRMWARE_UPGRADE_DUALBANK_MODE
zcfgRet_t zyUtilIGetFirmwareVersionByIndex(char *fwversion, int index);
#define zyUtilIGetFirmwareVersion(fwversion) zyUtilIGetFirmwareVersionByIndex(fwversion, -1)
#endif
zcfgRet_t zyUtilISetBootingFlag(int boot_flag);

#ifdef BROADCOM_PLATFORM
zcfgRet_t zyUtilIGetDefaultGponSnPw(char *serianumber, char *password);

#if defined(ZYXEL_BOARD_DETECT_BY_GPIO) || defined(CONFIG_BCM96856) || defined(CONFIG_BCM96858)
//Reference bcmdrivers/opensource/include/bcm963xx/board.h
#define ZYXEL_RDPA_WAN_TYPE_VALUE_XGS     "XGS"	//simulate RDPA_WAN_TYPE_VALUE_XGS
#define ZYXEL_RDPA_WAN_TYPE_VALUE_GBE     "GBE" //simulate RDPA_WAN_TYPE_VALUE_GBE

zcfgRet_t zyUtilIGetScratchPadSetting(char *psp_wantype, char *psp_wanrate);

#if defined(ZYXEL_BOARD_DETECT_BY_GPIO) && defined(ZYXEL_SUPPORT_RDPAWAN_CONFIGURE)
#define ZYXEL_CLI_READ_RDPA_WAN_INFO     "/tmp/rdpawaninfo" //temporary file for cli read.
int zyUtilISetScratchPadSetting(const char *key, const void *buf, int bufLen);
#endif

#endif
#endif

#ifdef ECONET_PLATFORM
zcfgRet_t zyUtilIGetGponSerialNumber(char *serianumber);
zcfgRet_t zyUtilIGetBootingFlag(int *boot_flag);
zcfgRet_t zyUtilISetBootingFlag(int boot_flag);
zcfgRet_t zyUtilGetBootImageHeader(void *booting_header);  //should conver to "struct trx_header"
zcfgRet_t zyUtilGetSpecifiedBootImageHeader(int bootFlag, void *booting_header);  //should convert to "struct trx_header"
#if defined (ZYXEL_SUPPORT_EXT_FW_VER) || defined(ZYXEL_SUPPORT_BACKUP_FW_VER)
zcfgRet_t zyUtilGetDualImageExtInfo(bool bBackup, char *fwversion);
#endif
zcfgRet_t zyUtilGetDualImageInfo(int bootFlag, char *fwversion);
zcfgRet_t zyUtilGetWlanReserveAreaOffset(char*, unsigned int*);
#if defined(ZYXEL_BOARD_DETECT_BY_GPIO)
zcfgRet_t zyUtilIGetScratchPadSetting(char *psp_wantype, char *psp_wanrate); //dummy API for compile issue in ECONET
#endif //defined(ZYXEL_BOARD_DETECT_BY_GPIO)
#endif //#ifdef ECONET_PLATFORM

#ifdef BROADCOM_PLATFORM
zcfgRet_t memaccess(int type, unsigned long long addr, unsigned long *value);
#endif

typedef enum
{
    ZYLED_POWER_GREEN = 0,
    ZYLED_POWER_RED,
#ifdef ZYXEL_OPAL_EXTENDER
    ZYLED_LINKQUALITY_GREEN,
    ZYLED_LINKQUALITY_RED,
    ZYLED_LINKQUALITY_AMBER,
    ZYLED_WIRELESS_GREEN,
#endif
#if defined(LTE_SUPPORT)
    ZYLED_CELLSIGNAL1_GREEN,
    ZYLED_CELLSIGNAL2_GREEN,
    ZYLED_CELLSIGNAL3_GREEN,
#endif
    ZYLED_INDEX_MAX
} ZYLED_INDEX;

typedef enum
{
    ZYLED_ACTION_OFF = 0,
    ZYLED_ACTION_BLINK,
    ZYLED_ACTION_ON,
#ifdef ZYXEL_OPAL_EXTENDER
    ZYLED_ACTION_UP,
    ZYLED_ACTION_DOWN,
#endif
    ZYLED_ACTION_MAX
} ZYLED_ACTION;

typedef enum
{
    ZYLED_RATE_NONE = 0,
    ZYLED_RATE_SLOW,
    ZYLED_RATE_FAST,
    ZYLED_RATE_MAX
} ZYLED_RATE;

/*!
*  zyUtilSetSoftwareLedState
*  @param[in]  index       - ZYLED_INDEX
*              action      - ZYLED_ACTION
*              blink_rate  - ZYLED_RATE, may only work on broadcom platform, econet platform is fix rate
*
*  @return   ZCFG_INTERNAL_ERROR   - control fail
*            ZCFG_SUCCESS          - control succee
*
*  @note set software led state
*/
int   zyUtilSetSoftwareLedState(ZYLED_INDEX index ,ZYLED_ACTION action, ZYLED_RATE blink_rate);


/*!
 *  ZIGBEE reset (get state)
 *
 *  @return    -1 - ioctl failed
 *              0 - Low
 *              1 - High
 *              2 - ZIGBEE not supported
 *  @note get zigbee reset states
 */
int   zyzigbeereset_get(void);

/*!
 *  ZIGBEE reset
 *
 *  @return    -1 - ioctl failed
 *              0 - Fail to reset zigbee module
 *              1 - Reset zigbee module succeed
 *  @note ZIGBEE reset
 */
int   zyzigbeereset_set(void);

/*!
 *  get UPS state
 *
 *  @return    -1           - ioctl failed
 *             Globe value  - UPS state
 *
 *  @note   Globe value: ups_active_status
 *          (bit3,bit2,bit1,bit0) => (miss battery, on battery, low battery, fail battery)
 *           bit0 : 1(active) - fail Self Test, 0(inactive) - battery is charged
 *           bit1 : 1(active) - battery < 45% capacity, 0(inactive) - battery full capacity
 *           bit2 : 1(active) - operating from battery, 0(inactive) - operating from utility line
 *           bit3 : 1(active) - battery is missing, 0(inactive) - battery is present
 */
int   zyupsstate_get(void);

/*!
 *  sfp operation
 *  @param[in]  subvalue    - SFP operation options (0-6)
 *              input       - inactive/active(0/1)
 *
 *  @return       -1        - ioctl failed
 *                 0        - inactive
 *                 1        - active
 *
 *  @note get/set sfp  
 */
int   zysfpoperation(int subvalue ,int input);

typedef enum //Increase before MAX_NUM_COMMANDS
{
    GET_SFP_PRESENT = 0,
    GET_SFP_AE_LOS,
    GET_SFP_TX_FAULT,
    GET_SFP_TX_DISABLE,
    SET_SFP_TX_DISABLE,
    GET_SFP_RX_SLEEP,
    SET_SFP_RX_SLEEP,
    ZYGOIOSATAT_MAX_NUM_COMMANDS
} zygpio_sfp_msgs_e;


int   zyUtilGetGPIO(int gpioNum);
float transferOpticalTemperature(int input_value);
float transferOpticalPower(float input_value);
int   ras_size();
int   magic_number_check(char *uploadfilename);

#if defined(ABUU_CUSTOMIZATION) //IChiaHuang 20200323
typedef enum
{
    ZY_LED_OFF,
    ZY_LED_ON,
    ZY_LED_SLOW_BLINK,
    ZY_LED_FAST_BLINK,
    ZY_LED_UP,
    ZY_LED_DOWN,
    ZY_MAX_LED_ACTION
}ZY_LED_ACTION;
#endif
#endif // _LIBZYUTIL_WRAPPER_H_
