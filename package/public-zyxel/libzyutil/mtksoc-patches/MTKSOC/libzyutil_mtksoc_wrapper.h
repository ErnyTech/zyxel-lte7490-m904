#ifdef MTKSOC_PLATFORM
#include <ctype.h>
#include <unistd.h>
#include "zcfg_common.h"
#include "zcfg_debug.h"

#ifdef ZYXEL_RANDOM_PASSWORD_IN_MRD
#include "libzyutil_mtksoc_random_password.h"
#endif

/*for different kernel mtd behavior to flash.c */
#define MTD_MODE_RAW            MTD_FILE_MODE_RAW
#define MEMSETOOBSEL           _IOW('M', 9, struct nand_oobinfo) //this command is useless , the ioctl existed in the past and was removed

#if 1//defined(ZYXEL)
//#include "mtk_partition.h"
#include "trx.h"
#include "mrd.h"					/*(STAGING_DIR_HOST)/include/mrd.h; mrd structure */
#include "mrd_env.h" 				/*nvram define paramters*/
#define FLASH_READ_CMD                "/usr/bin/mtd_write -q -q readflash %s %lu %lu %s"
#define FLASH_WRITE_CMD               "/usr/bin/mtd_write -q -q writeflash %s %lu %lu %s"
#define BOOT_FLAG_SIZE                1
//#define BOOT_FLAG_OFFSET              (MRD_RA_OFFSET - BOOT_FLAG_SIZE)
#define BOOT_FLAG_OFFSET	      0x200000
#define RESERVEAREA_NAME	      "reservearea"
#define BOOT_FLAG_FILE                "/tmp/boot_flag"
#define MRDINFO_PARTITON_NAME         "Factory"
#define MRDINFO_OFFSET                MRD_OFFSET
#define MRDINFO_SIZE                  MRD_SIZE
#define MRDINFO_FILE                  "/tmp/MRD_info"
//#define MAIN_ROOTFS_PARTITION_NAME    "rootfs"
//#define SLAVE_ROOTFS_PARTITION_NAME   "rootfs2"
#define BOOTIMG_HEADER_OFFSET         0x0
#define BOOTIMG_HEADER_SIZE           sizeof(struct trx_header)
#define BOOTIMG_HEADER_FILE           "/tmp/BootImg_info"
#endif

// MagicNumber used for ftp firmware upgrade , hexdump -n 2 ras.bin result
#define MagicNubmer	5230

zcfgRet_t zyUtilGetMrdInfo(PMRD_DATA mrd);
zcfgRet_t zyUtilSetMrdInfo(PMRD_DATA mrd);
zcfgRet_t zyUtilIGetBootingFlag(int *boot_flag);
zcfgRet_t zyUtilISetBootingFlag(int boot_flag);

int read_mtd_info(const char *mtd, struct trx_header *p_trx_header);
int32_t nvram_file_get (char * keyname, char * returnstring, char * cachefile );
int32_t banner_file_get (char * productname, char * firmwareversion);

#endif
