#ifndef __RALINK_FLASH_H__
#define __RALINK_FLASH_H__

#if defined (CONFIG_RT2880_FLASH_32M) 
#define MTD_BOOT_PART_SIZE	0x40000	
#define	MTD_CONFIG_PART_SIZE	0x20000
#define	MTD_FACTORY_PART_SIZE	0x20000
#else
#if defined (RECONFIG_PARTITION_SIZE)
#if !defined (MTD_BOOT_PART_SIZE)
#error "Please define MTD_BOOT_PART_SIZE"
#endif
#if !defined (MTD_CONFIG_PART_SIZE)
#error "Please define MTD_CONFIG_PART_SIZE"
#endif
#if !defined (MTD_FACTORY_PART_SIZE)
#error "Please define MTD_FACTORY_PART_SIZE"
#endif
#else
/*Add Boot/Factory/Zyxel Partitions for NAND Flash, Total 20MB, BMT default will reserve 80 BLOCKs (10MB) for bad block*/
#if defined (CONFIG_RT2880_FLASH_64M_NAND) || defined (CONFIG_RT2880_FLASH_128M_NAND)
#define MTD_NAND_FLASH_BLOCK_SIZE	0x20000  //this define can be changed baccording to type of nandflash (0x10000 64k /0x20000 128k /0x40000 256k)
#define MTD_BOOT_PART_SIZE	MTD_NAND_FLASH_BLOCK_SIZE*4	//512k
#define MTD_CONFIG_PART_SIZE    MTD_NAND_FLASH_BLOCK_SIZE*4	//512k
#define MTD_FACTORY_PART_SIZE	MTD_NAND_FLASH_BLOCK_SIZE*2	//256k
#define MTD_WWAN_PART_SIZE	MTD_NAND_FLASH_BLOCK_SIZE*8	//1MB wwan partition size define for nand flash
#define MTD_DATA_PART_SIZE	MTD_NAND_FLASH_BLOCK_SIZE*128   //16MB data partition size define for nand flash
#define MTD_ROMD_PART_SIZE	MTD_NAND_FLASH_BLOCK_SIZE*8	//1MB rom-d partition size define for nand flash
#define MTD_RESERVE_PART_SIZE	MTD_NAND_FLASH_BLOCK_SIZE*4	//512k reserve partition size define for nand flash 
#else
#define MTD_BOOT_PART_SIZE	0x30000
#define	MTD_CONFIG_PART_SIZE	0x10000
#define	MTD_FACTORY_PART_SIZE	0x10000
/*Add Zyxel Partitions for NOR Flash*/
#define MTD_WWAN_PART_SIZE	0x40000		// 256k wwan partition size define for NOR flash
#define MTD_DATA_PART_SIZE	0x80000		// 512K data partition size define for NOR flash
#define MTD_ROMD_PART_SIZE	0x40000		// 256k rom-d partition size define for NOR flash
#define MTD_RESERVE_PART_SIZE	0x30000		// 384k reserve partition size define for NOR flash
#endif //endif CONFIG_RT2880_FLASH_64M_NAND or 128M
#endif //endif RECONFIG_PARTITION_SIZE
#endif //endif CONFIG_RT2880_FLASH_32M


#ifdef CONFIG_RT2880_ROOTFS_IN_FLASH
#ifdef CONFIG_ROOTFS_IN_FLASH_NO_PADDING
#define CONFIG_MTD_KERNEL_PART_SIZ 0
#endif
#define MTD_ROOTFS_PART_SIZE	IMAGE1_SIZE - (MTD_BOOT_PART_SIZE + MTD_CONFIG_PART_SIZE \
					+ MTD_FACTORY_PART_SIZE + CONFIG_MTD_KERNEL_PART_SIZ)
#define	MTD_KERN_PART_SIZE	CONFIG_MTD_KERNEL_PART_SIZ
#else
#if defined CONFIG_EXTEND_NVRAM
#define MTD_KERN_PART_SIZE	IMAGE1_SIZE - (MTD_BOOT_PART_SIZE + MTD_CONFIG_PART_SIZE \
				+ MTD_FACTORY_PART_SIZE + MTD_CONFIG_PART_SIZE)
#else

/*Add Zyxel Kernel size option to fixed size to 16MB or 1/4 flash size */
//#if defined (CONFIG_RT2880_FLASH_64M_NAND) || defined (CONFIG_RT2880_FLASH_128M_NAND)
//#define MTD_KERN_PART_SIZE	IMAGE1_SIZE //fxed to IMAGESZIE
//#else
#define MTD_KERN_PART_SIZE	IMAGE1_SIZE - (MTD_BOOT_PART_SIZE + MTD_CONFIG_PART_SIZE \
					+ MTD_FACTORY_PART_SIZE)
//#endif //endif endif CONFIG_RT2880_FLASH_64M_NAND or 128M
#endif //endif CONFIG_EXTEND_NVRAM
#endif //endif CONFIG_RT2880_ROOTFS_IN_FLASH


#ifdef CONFIG_DUAL_IMAGE
#if defined (CONFIG_RT2880_FLASH_2M)
#define IMAGE1_SIZE		0x100000
#elif defined (CONFIG_RT2880_FLASH_4M)
#define IMAGE1_SIZE		0x200000
#elif defined (CONFIG_RT2880_FLASH_8M)
#define IMAGE1_SIZE		0x400000
#elif defined (CONFIG_RT2880_FLASH_16M)
#define IMAGE1_SIZE		0x800000
#elif defined (CONFIG_RT2880_FLASH_32M)
#define IMAGE1_SIZE		0x1000000
#elif defined (CONFIG_RT2880_FLASH_64M_NAND)
#define IMAGE1_SIZE		0x1000000	//16MB for Kernel image in 64MB NAND flash (equal to 1/4 flash size)
#elif defined (CONFIG_RT2880_FLASH_128M_NAND)
#define IMAGE1_SIZE             0x2000000	//32MB for Kernel image in 128MB NAND flash (equal to 1/4 flash size)
#endif

#define MTD_KERN2_PART_SIZE	MTD_KERN_PART_SIZE
#define MTD_KERN2_PART_OFFSET	IMAGE1_SIZE + (MTD_BOOT_PART_SIZE + MTD_CONFIG_PART_SIZE \
					+ MTD_FACTORY_PART_SIZE)
#define MTD_WWAN_PART_OFFSET (MTD_KERN2_PART_OFFSET + MTD_KERN2_PART_SIZE) //Add wwan offset
#define MTD_DATA_PART_OFFSET (MTD_WWAN_PART_OFFSET + MTD_WWAN_PART_SIZE) //Add data offset
#define MTD_ROMD_PART_OFFSET (MTD_DATA_PART_OFFSET + MTD_DATA_PART_SIZE) //Add rom-d offset
#define MTD_RESERVE_PART_OFFSET (MTD_ROMD_PART_OFFSET + MTD_ROMD_PART_SIZE) //Add reserve offset


#ifdef CONFIG_RT2880_ROOTFS_IN_FLASH
#define MTD_ROOTFS2_PART_SIZE 	MTD_ROOTFS_PART_SIZE	
#define MTD_ROOTFS2_PART_OFFSET (MTD_KERN2_PART_OFFSET + MTD_KERN2_PART_SIZE)
#endif

#else // Non Dual Image
#if defined (CONFIG_RT2880_FLASH_2M)
#define IMAGE1_SIZE		0x200000
#elif defined (CONFIG_RT2880_FLASH_4M)
#define IMAGE1_SIZE		0x400000
#elif defined (CONFIG_RT2880_FLASH_8M)
#define IMAGE1_SIZE		0x800000
#elif defined (CONFIG_RT2880_FLASH_16M)
#define IMAGE1_SIZE		0x1000000
#elif defined (CONFIG_RT2880_FLASH_32M)
#define IMAGE1_SIZE		0x2000000
#else
#define IMAGE1_SIZE		CONFIG_MTD_PHYSMAP_LEN
#endif
#endif

#if defined CONFIG_EXTEND_NVRAM
#define MTD_CONFIG2_PART_OFFSET	IMAGE1_SIZE - MTD_CONFIG_PART_SIZE
#endif

#define BOOT_FROM_NOR	0
#define BOOT_FROM_NAND	2
#define BOOT_FROM_SPI	3

int ra_check_flash_type(void);

#endif //__RALINK_FLASH_H__
