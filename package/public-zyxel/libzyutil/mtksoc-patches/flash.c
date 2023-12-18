#define _XOPEN_SOURCE 500

#include "zcfg_common.h"
#include "zyutil.h"
#include "zlog_api.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <unistd.h>
#include <mtd/mtd-user.h>
#include <errno.h>
#include "zcfg_system.h"

//#define ENABLE_DEBUG
#ifdef ENABLE_DEBUG
#define DEBUG_PRINT(fmt, args...)    printf(fmt, ## args)
#else
#define DEBUG_PRINT(fmt, args...)    /* do nothing */
#endif

#if 0 //#ifdef ZYXEL_WWAN
#define ZY_WWANPACKAGE_MAXSIZE	131072  // (1024*128) bytes
#define WWANPACKAGE_HEADER_LEN	32  // theoretically speaking 31 is match
#define FW_WWANPACKAGE_FILE	"/etc/fw_wwanpackage"
#define RUN_WWANPACKAGE_FILE	"/var/wwan/run_wwanpackage"
#endif

#define MTD_MODE_RAW            MTD_FILE_MODE_RAW
#define MEMSETOOBSEL           _IOW('M', 9, struct nand_oobinfo) //this command is useless , the ioctl existed in the past and was removed

/* size of read/write buffer */
#define BUFSIZE (64 * 1024)

static struct nand_oobinfo none_oobinfo = {
	.useecc = MTD_NANDECC_OFF,
};

static int zyUtilOpen(const char *pathname, int flags)
{
	int fd;

	fd = open(pathname, flags);
	if(fd < 0) {
        printf("Try to open %s\n", pathname);
		if(flags & O_RDWR)
			printf("Read/Write access\n");
        else if(flags & O_RDONLY)
			printf("Read access\n");
        else if(flags & O_WRONLY)
			printf("Write access\n");

        printf("%s : %m\n", __FUNCTION__);
		return -1;
	}

	return fd;
}

static int zyUtilRead(int fd, const char *filename, void *buf, size_t count)
{
	ssize_t result;

	result = read(fd, buf, count);

	if (count != result) {
		if (result < 0) {
			printf("While reading data from %s: %m\n", filename);
			return -1;
		}

		printf("Short read count returned while reading from %s\n", filename);
		return -1;
	}

	return 0;
}

zcfgRet_t zyUtilMtdDevGetByName(char *mtdname, char *mtddev)
{
	zcfgRet_t ret = ZCFG_NOT_FOUND;
	FILE *fptr = NULL;
	char mtd[8], size[12], erasesize[12], name[16], tmpname[16];
	int len = 0;
#if 1 //#ifdef ZYXEL_WWAN
	FILE *fp = NULL;

	if(!strcmp(mtdname, WWAN_PARTITION_NAME)){
		fp = popen("cat /proc/mtd > /tmp/mtd", "r");
		if(fp != NULL)
			pclose(fp);
	}
	else 
#endif
	system("cat /proc/mtd > /tmp/mtd");

    if((fptr = fopen("/tmp/mtd", "r")) == NULL) {
        printf("Can't open file\n");
    }
	else {
		sprintf(tmpname, "\"%s\"", mtdname);

		fscanf(fptr, "%s%s%s%s", mtd, size, erasesize, name);
		while(!feof(fptr)) {
			if(strstr(mtd, "dev") != NULL) { //skip first line
				fscanf(fptr, "%s%s%s%s", mtd, size, erasesize, name);
				continue;
			}

			if(!strcmp(name, tmpname)) {
				// Delete ':'
				len = strlen(mtd);
				mtd[len-1] = '\0';
				sprintf(mtddev, "/dev/%s", mtd);
				ret = ZCFG_SUCCESS;
				printf("Find MTD Device %s with name %s.\n", mtddev, mtdname);
				break;
			}

			fscanf(fptr, "%s%s%s%s", mtd, size, erasesize, name);
        }

        fclose(fptr);
	}

	remove("/tmp/mtd");
	return ret;
}

unsigned int zyUtilMtdDevGetSize(char *devName)
{
	int devFd = -1;
	struct mtd_info_user mtd;

	/* Open MTD device */
	if((devFd = zyUtilOpen(devName, O_SYNC | O_RDWR)) == -1) {
		printf("Cannot open MTD device!\n");
		return ZCFG_INTERNAL_ERROR;
	}

	/* get some info about the flash device */
	if(ioctl(devFd, MEMGETINFO, &mtd) < 0) {
        printf("This doesn't seem to be a valid MTD flash device!\n");
		close(devFd);
		return ZCFG_INTERNAL_ERROR;
	}
	close(devFd);
	return mtd.size;
}

#if 0 //201806__ZYXEL__, fixed write fail on bad block
/*!
 *  Write data from buffer to flash.
 *
 *  @param[in]  (char *)    Flash (MTD device name)
 *              (void *)    Buffer
 *              uint32_t    Buffer size
 *  @return     zcfgRet_t   ZCFG_INTERNAL_ERROR/ZCFG_SUCCESS
 *
 *  NOTE:
 *  20180622 -  Rewrite this function.
 *              Set one block size as an unit of erase/read/compare action.
 *              And every time write a page size to flash (flash's character).
 *              Btw, a block needs to unlock before erase/write it.
 */
zcfgRet_t zyUtilWriteBufToFlash(char *devName, void *buf, uint32_t buf_size)
{
    int devFd = -1;
    ssize_t result;
    size_t buf_size_to_write, written, bufOfs;
    struct mtd_info_user mtd;
    struct erase_info_user erase;
    void *dest = NULL;
    int pagesize = 0, unit_to_write = 0, unit_to_use = 0, meet_bad = 0, bad_blk_count = 0;
    loff_t cur_ofs = 0;

    if ((devName == NULL) || (buf == NULL) || (buf_size <= 0))
    {
        ZLOG_ERROR("Incorrect buffer data!\n");
        return ZCFG_INTERNAL_ERROR;
    }

    /* Open MTD device */
    if ((devFd = zyUtilOpen(devName, O_SYNC | O_RDWR)) == -1)
    {
        ZLOG_ERROR("Failed to open %s!\n",devName);
        return ZCFG_INTERNAL_ERROR;
    }

    /* Get info of the flash device */
    if (ioctl(devFd, MEMGETINFO, &mtd) < 0)
    {
        ZLOG_ERROR("Failed to get MTD information of %s!\n",devName);
        close(devFd);
        return ZCFG_INTERNAL_ERROR;
    }

    ZLOG_DEBUG("[%s_%d] devName=%s, buf_size=%d, MTD info: \n"
                "type=0x%08x, flags=0x%08x, size=0x%08x \n"
                "erasesize=0x%08x, writesize=0x%08x, oobsize=0x%08x\n", \
                __FILE__, __LINE__, devName, buf_size,
                mtd.type, mtd.flags, mtd.size, mtd.erasesize, mtd.writesize, mtd.oobsize);

    /* Check if partition size enough */
    if (buf_size > mtd.size)
    {
        ZLOG_ERROR("MTD device %s space is not enough!\n", devName);
        close(devFd);
        return ZCFG_INTERNAL_ERROR;
    }

    /* Basic initial */
    dest = malloc(mtd.erasesize);
    unit_to_use = erase.length = mtd.erasesize; //block size, e.g. 0x20000
    buf_size_to_write = (size_t)buf_size;
    bufOfs = 0;

    /* First to erase device anyway */
    for (erase.start = 0; erase.start < mtd.size; erase.start += mtd.erasesize)
    {
        ioctl(devFd, MEMUNLOCK, &erase);
        if (ioctl(devFd, MEMERASE, &erase))
        {
            ZLOG_DEBUG("[%s_%d] Failed to erase %s block at offset 0x%08x.\n", __FILE__, __LINE__, devName, erase.start);
        } //else do nothing
    }

    for (erase.start = 0; ((erase.start < mtd.size) && (buf_size_to_write > 0)); erase.start += mtd.erasesize)
    {
        int ret;
        loff_t offset = erase.start;

        ZLOG_DEBUG("[%s_%d] Going to offset=0x%08x, still %d size need to write.\n", __FILE__, __LINE__, erase.start, buf_size_to_write);

        /* Bad block checking */
        ret = ioctl(devFd, MEMGETBADBLOCK, &offset);
        if (ret)
        {
            ZLOG_DEBUG("[%s_%d] Skip this block.\n", __FILE__, __LINE__);
            cur_ofs = lseek(devFd, erase.length, SEEK_CUR);
            DEBUG_PRINT("[%s_%d] Shift pointer to ofs 0x%08x.\n", __FILE__, __LINE__, (unsigned int)cur_ofs);
            bad_blk_count++;
            continue;
        } //else do nothing

        DEBUG_PRINT("[%s_%d] Erase 0x%08x-0x%08x ...\n", __FILE__, __LINE__, (unsigned int)erase.start, (unsigned int)(erase.start + erase.length));

        /* Write data */
        {
            if (buf_size_to_write < erase.length) //data not write yet
            {
                unit_to_use = buf_size_to_write; //unit_to_use is how many we want to write to this block
            } //else do nothing

            /* Unlock a block before write */
            ioctl(devFd, MEMUNLOCK, &erase);

            pagesize = mtd.writesize;
            written = 0;
            meet_bad = 0;
            unit_to_write = unit_to_use;

            // Wite by page size (mtd.writesize)
            while (unit_to_write)
            {
                ZLOG_DEBUG("[%s_%d] unit_to_use=0x%08x, unit_to_write=0x%08x, pagesize=0x%08x, bufOfs=0x%08x, written=0x%08x\n", \
                                __FILE__, __LINE__, unit_to_use, unit_to_write, pagesize, bufOfs, written);

                if (unit_to_write < mtd.writesize)
                {
                    pagesize = unit_to_write;
                } //else do nothing

                result = write(devFd, (buf + bufOfs + written), pagesize);
                if (result != pagesize)
                {
                    // Write fail
                    meet_bad = 1;
                    ZLOG_DEBUG("[%s_%d] Write to flash fail.\n", __FILE__, __LINE__);
                    cur_ofs = lseek(devFd, (offset + erase.length), SEEK_SET); //next block
                    DEBUG_PRINT("[%s_%d] Shift pointer to ofs 0x%08x\n", __FILE__, __LINE__, (unsigned int)cur_ofs);
                    break;
                } //else do nothing
                unit_to_write -= pagesize;
                written += pagesize;
            }
        } //Write data end

        /* Data confirm */
        if (!meet_bad)
        {
            memset(dest, 0xff, sizeof(mtd.erasesize));
            cur_ofs = lseek(devFd, offset, SEEK_SET);
            DEBUG_PRINT("[%s_%d] Shift pointer back to ofs 0x%08x to reading.\n", __FILE__, __LINE__, (unsigned int)cur_ofs);

            // Read data
            result = read(devFd, dest, sizeof(unsigned char)*unit_to_use);
            DEBUG_PRINT("[%s_%d] Read return %d\n", __FILE__, __LINE__, result);

            if (result != unit_to_use)
            {
                meet_bad = 1;
                ZLOG_DEBUG("[%s_%d] Read from flash fail.\n", __FILE__, __LINE__);
                cur_ofs = lseek(devFd, (off_t)(-result), SEEK_CUR);
                DEBUG_PRINT("[%s_%d] Shift pointer back to ofs 0x%08x.\n", __FILE__, __LINE__, (unsigned int)cur_ofs);
            }
            else
            {
                // Compare data
                ret = memcmp((buf + bufOfs), dest, sizeof(unsigned char)*unit_to_use);
                if (ret)
                {
                    meet_bad = 1;
                    ZLOG_DEBUG("[%s_%d] Flash data does not match with buffer 0x%08x-0x%08x.\n", \
                                __FILE__, __LINE__, (unsigned int)bufOfs, (unsigned int)(bufOfs + unit_to_use));
                } //else do nothing
            }
        } //Data confirm end

        /* If bad block happen, then jump to next block */
        if (meet_bad == 1)
        {
            ioctl(devFd, MEMERASE, &erase);
            continue; //go to next block without reading & comparing
        } //else do nothing

        bufOfs += unit_to_use;
        buf_size_to_write -= unit_to_use;

    } //end of for(;;)

    ZLOG_DEBUG("[%s_%d] MTD device %s has %d bad block(s).\n", __FILE__, __LINE__, devName, bad_blk_count);

    /* No good block to write */
    if ((erase.start == mtd.size) && (buf_size_to_write != 0))
    {
        for (erase.start = 0; erase.start < mtd.size; erase.start += mtd.erasesize)
        {
            ioctl(devFd, MEMUNLOCK, &erase);
            if (ioctl(devFd, MEMERASE, &erase))
            {
                ZLOG_DEBUG("[%s_%d] Failed to erase block on %s at 0x%08x\n", __FILE__, __LINE__, devName, erase.start);
            } //else do nothing
        }
        ZLOG_ERROR("Erase %s due to incomplete writing to flash.\n", devName);
        free(dest);
        close(devFd);
        return ZCFG_INTERNAL_ERROR;
    } //else do nothing

    free(dest);
    close(devFd);
    return ZCFG_SUCCESS;
}
#else
zcfgRet_t zyUtilWriteBufToFlash(char *devName, void *buf, uint32_t buf_size)
{
	int devFd = -1, i = 0;
	ssize_t result;
	size_t size, written;
	struct mtd_info_user mtd;
	struct erase_info_user erase;
	unsigned char dest[BUFSIZE];

	/* Open MTD device */
	if((devFd = zyUtilOpen(devName, O_SYNC | O_RDWR)) == -1) {
		return ZCFG_INTERNAL_ERROR;
	}

	/* get some info about the flash device */
	if(ioctl(devFd, MEMGETINFO, &mtd) < 0) {
        printf("This doesn't seem to be a valid MTD flash device!\n");
		close(devFd);
		return ZCFG_INTERNAL_ERROR;
	}

	/* Check if it fit into the device/partition? */
	if(buf_size > mtd.size) {
        printf("won't fit into %s!\n", devName);
		close(devFd);
		return ZCFG_INTERNAL_ERROR;
	}

	/* Erase enough blocks */
	erase.start = 0;
	erase.length = buf_size & ~(mtd.erasesize - 1);
	if(buf_size % mtd.erasesize)
		erase.length += mtd.erasesize;

	/* erase the whole chunk in one shot */
	if(ioctl(devFd, MEMERASE, &erase) < 0) {
		printf("While erasing blocks from 0x%.8x-0x%.8x on %s: %m\n", (unsigned int) erase.start,(unsigned int) (erase.start + erase.length), devName);
		close(devFd);
		return ZCFG_INTERNAL_ERROR;
	}

	printf("Erased %u / %uk bytes\n", erase.length, buf_size);

	/* Write the entire file to flash */
	size = buf_size;
	i = BUFSIZE;
	written = 0;
	while(size) {
        if(size < BUFSIZE)
			i = size;

		/* write to device */
		result = write(devFd, (buf+written), i);
		if(i != result) {
			if (result < 0) {
				printf("While writing data to 0x%.8x-0x%.8x on %s: %m\n", written, written+i, devName);
				close(devFd);
				return ZCFG_INTERNAL_ERROR;
			}

			printf("Short write count returned while writing to x%.8x-0x%.8x on %s: %d/%u bytes written to flash\n",
                      written, written+i, devName, written+result, buf_size);

			close(devFd);
			return ZCFG_INTERNAL_ERROR;
		}

		written += i;
		size -= i;
	}

	/* Verify */
	if(lseek(devFd, 0L, SEEK_SET) < 0) {
		printf("While seeking to start of %s: %m\n", devName);
	}

	size = buf_size;
	i = BUFSIZE;
	written = 0;
	while(size) {
		if(size < BUFSIZE)
			i = size;

		/* read from device */
		zyUtilRead(devFd, devName, dest, i);

		/* compare buffers */
		if(memcmp((buf+written), dest, i)) {
			printf("File does not seem to match flash data. First mismatch at 0x%.8x-0x%.8x\n", written, written + i);
			close(devFd);
			return ZCFG_INTERNAL_ERROR;
		}

		written += i;
		size -= i;
	}

	close(devFd);
    return ZCFG_SUCCESS;
}
#endif

#ifndef NEW_MTD_DRIVER 
int zyUtilReadBufFromFlash(char *devName, void *buf, uint32_t buf_size)
{
	int oobinfochanged = 0;
	unsigned long start_addr = 0;
	unsigned long ofs, end_addr = 0;
	unsigned long long blockstart = 1;
	int devFd = -1;
	struct mtd_info_user mtd;
	struct nand_oobinfo old_oobinfo;
	int ret, pagesize, badblock = 0;
	unsigned char readbuf[4096];

	/* Open MTD device */
	if((devFd = zyUtilOpen(devName, O_RDONLY)) == -1) {
		return -1;
	}

	/* Fill in MTD device capability structure */
	if(ioctl(devFd, MEMGETINFO, &mtd) != 0) {
		printf("This doesn't seem to be a valid MTD flash device!\n");
		close(devFd);
		return -1;
	}

	printf("mtd.oobsize %d mtd.writesize %d\n", mtd.oobsize, mtd.writesize);

	/* Make sure device page sizes are valid */
	if (!(mtd.oobsize == 256 && mtd.writesize == 4096) &&
		!(mtd.oobsize == 128 && mtd.writesize == 4096) &&
		!(mtd.oobsize == 128 && mtd.writesize == 2048) &&
		!(mtd.oobsize == 108 && mtd.writesize == 2048) &&
		!(mtd.oobsize == 64 && mtd.writesize == 2048) &&
		!(mtd.oobsize == 32 && mtd.writesize == 1024) &&
		!(mtd.oobsize == 16 && mtd.writesize == 512) &&
		!(mtd.oobsize == 8 && mtd.writesize == 256)) {
		printf("Unknown flash (not normal NAND)\n");
		close(devFd);
		return -1;
	}

#if 0
	/* Read the real oob length */
	oob.length = mtd.oobsize;
#endif

#ifdef QUECTEL_PLATFORM
	ret = ioctl(devFd, MTDFILEMODE, (void *) MTD_FILE_MODE_NORMAL);
#else
	/* No ecc*/
	ret = ioctl(devFd, MTDFILEMODE, (void *) MTD_MODE_RAW);
#endif
	if(ret == 0) {
		oobinfochanged = 2;
	}
	else {
		switch(errno) {
			case ENOTTY:
				if(ioctl(devFd, MEMGETOOBSEL, &old_oobinfo) != 0) {
					perror("MEMGETOOBSEL");
					close(devFd);
					return -1;
				}

				if(ioctl(devFd, MEMSETOOBSEL, &none_oobinfo) != 0) {
					perror("MEMSETOOBSEL");
					close(devFd);
					return -1;
				}

				oobinfochanged = 1;
				break;
			default:
				perror("MTDFILEMODE");
				close(devFd);
				return -1;
		}
	}

	/* Initialize start/end addresses and block size */
	end_addr = start_addr + buf_size;

	pagesize = mtd.writesize;

	/* Dump the flash contents */
	for (ofs = start_addr; ofs < end_addr ; ofs+=pagesize) {
		// new eraseblock , check for bad block
		if (blockstart != (ofs & (~mtd.erasesize + 1))) {
			blockstart = ofs & (~mtd.erasesize + 1);
			if ((badblock = ioctl(devFd, MEMGETBADBLOCK, &blockstart)) < 0) {
				perror("ioctl(MEMGETBADBLOCK)");
				goto closeall;
			}
		}

		if (badblock) {
				continue;
		}
		else {
			/* Read page data and exit on failure */
			if (pread(devFd, readbuf, pagesize, ofs) != pagesize) {
				perror("pread");
				goto closeall;
			}
		}

		if(buf_size < pagesize) {
			memcpy(buf, readbuf, buf_size);
		}
		else {
			memcpy(buf, readbuf, pagesize);
			buf += pagesize;
			buf_size -= pagesize;
		}
	}

	/* reset oobinfo */
	if(oobinfochanged == 1) {
		if(ioctl(devFd, MEMSETOOBSEL, &old_oobinfo) != 0) {
			perror ("MEMSETOOBSEL");
			close(devFd);
			return -1;
		}
	}

	/* Close the output file and MTD device */
	close(devFd);

	return 0;

closeall:
	/* The new mode change is per file descriptor ! */
	if(oobinfochanged == 1) {
		if(ioctl(devFd, MEMSETOOBSEL, &old_oobinfo) != 0)  {
			perror("MEMSETOOBSEL");
		}
	}

	close(devFd);
	return -1;
}
#else
int zyUtilReadBufFromFlash(char *devName, void *buf, uint32_t buf_size)
{
	unsigned long start_addr = 0;
	unsigned long ofs, end_addr = 0;
	unsigned long long blockstart = 1;
	int devFd = -1;
	struct mtd_info_user mtd;
	int ret, pagesize, badblock = 0;
	unsigned char readbuf[4096];

	/* Open MTD device */
	if((devFd = zyUtilOpen(devName, O_RDONLY)) == -1) {
		return -1;
	}

	/* Fill in MTD device capability structure */
	if(ioctl(devFd, MEMGETINFO, &mtd) != 0) {
		printf("This doesn't seem to be a valid MTD flash device!\n");
		close(devFd);
		return -1;
	}

	printf("mtd.oobsize %d mtd.writesize %d\n", mtd.oobsize, mtd.writesize);

	/* Make sure device page sizes are valid */
	if (!(mtd.oobsize == 256 && mtd.writesize == 4096) &&
		!(mtd.oobsize == 128 && mtd.writesize == 4096) &&
		!(mtd.oobsize == 128 && mtd.writesize == 2048) &&
		!(mtd.oobsize == 108 && mtd.writesize == 2048) &&
		!(mtd.oobsize == 64 && mtd.writesize == 2048) &&
		!(mtd.oobsize == 32 && mtd.writesize == 1024) &&
		!(mtd.oobsize == 16 && mtd.writesize == 512) &&
		!(mtd.oobsize == 8 && mtd.writesize == 256)) {
		printf("Unknown flash (not normal NAND)\n");
		close(devFd);
		return -1;
	}

#ifdef QUECTEL_PLATFORM
	if(ioctl(devFd, MTDFILEMODE, (void *) MTD_FILE_MODE_NORMAL) != 0) {
#else
	/* No ecc*/
	if(ioctl(devFd, MTDFILEMODE, (void *) MTD_FILE_MODE_RAW) != 0) {
#endif
		perror("MTDFILEMODE");
		goto closeall;
	}

	/* Initialize start/end addresses and block size */
	// partition size
	if(buf_size > mtd.size)
		goto closeall;
	
	end_addr = mtd.size;

	pagesize = mtd.writesize;

	/* Dump the flash contents */
	for (ofs = start_addr; (ofs < end_addr) && (buf_size > 0); ofs+=pagesize) {
		// new eraseblock , check for bad block
		if (blockstart != (ofs & (~mtd.erasesize + 1))) {
			blockstart = ofs & (~mtd.erasesize + 1);
			if ((badblock = ioctl(devFd, MEMGETBADBLOCK, &blockstart)) < 0) {
				perror("ioctl(MEMGETBADBLOCK)");
				goto closeall;
			}
		}

		if (badblock) {
			ofs += mtd.erasesize - pagesize;
			continue;
		}
		else {
			/* Read page data and exit on failure */
			if (pread(devFd, readbuf, pagesize, ofs) != pagesize) {
				perror("pread");
				goto closeall;
			}
		}

		if(buf_size < pagesize) {
			memcpy(buf, readbuf, buf_size);
			buf_size = 0;
		}
		else {
			memcpy(buf, readbuf, pagesize);
			buf += pagesize;
			buf_size -= pagesize;
		}
	}

	/* Close the output file and MTD device */
	close(devFd);

	return 0;

closeall:

	close(devFd);
	return -1;
}
#endif

#if 1 //#ifdef ZYXEL_WWAN
int zyUtilMtdErase(const char *mtd)
{
	int fd;
	struct mtd_info_user mtdInfo;
	struct erase_info_user mtdEraseInfo;

	/* Open MTD device */
	if((fd = zyUtilOpen(mtd, O_SYNC | O_RDWR)) == -1) {
		fprintf(stderr, "Could not open mtd device: %s\n", mtd);
		return ZCFG_INTERNAL_ERROR;
	}
	
	/* Fill in MTD device capability structure */
	if(ioctl(fd, MEMGETINFO, &mtdInfo)) {
		fprintf(stderr, "Could not get MTD device info from %s\n", mtd);
		close(fd);
		return ZCFG_INTERNAL_ERROR;
		//exit(1);
	}

	mtdEraseInfo.length = mtdInfo.erasesize;

	/* Erase flash */
	for (mtdEraseInfo.start = 0; mtdEraseInfo.start < mtdInfo.size; mtdEraseInfo.start += mtdInfo.erasesize)
	{
		ioctl(fd, MEMUNLOCK, &mtdEraseInfo);
		if (ioctl(fd, MEMERASE, &mtdEraseInfo))
		{
            fprintf(stderr, "Failed to erase %s block at 0x%08x.\n", mtd, mtdEraseInfo.start);
		}
	}

	close(fd);
	return 0;

}


/*______________________________________________________________________________
**	zy_updateWWANPackage
**
**	descriptions:
**		update run time WWAN Package.
**
**
**	parameters:
**
**
**	return:
**		1: Fail
**		0: OK
**____________________________________________________________________________
*/
int
zyUtilUpdateWWANPackage(void)
{
#ifdef MTKSOC_PLATFORM //MTKSOC do not use WWAN package
	printf("%s, not support in current platform!\n", __func__);
	return 1;
#endif
	int fd, i, length;
	char *packageBuf = NULL, *bufPoint = NULL, *flashwwan;
	char fwPkgRevision[16], flashPkgRevision[16], cmd_buf[128], mtddev[16];
	zcfgRet_t ret; 
	FILE *fp = NULL;
	//SINT32 filesize = 0;

	/* get FW WWAN Package REVISION */
	fwPkgRevision[0] = '\0';
	fd = open(FW_WWANPACKAGE_FILE, O_RDONLY);
	if(fd < 0){
		printf("Error: %s, open FW_WWANPACKAGE_FILE fail!\n", __func__);
	}
	else{
		packageBuf = calloc(WWANPACKAGE_HEADER_LEN, sizeof(char));	
		if (packageBuf == NULL)
		{
			printf("packageBuf allocation failed.\n");
			return 1;
		}
		if (read(fd, packageBuf, WWANPACKAGE_HEADER_LEN) != WWANPACKAGE_HEADER_LEN)
	    {
	        printf("Error: %s read function fail.\n", __func__);
		 	free(packageBuf);
	        return 1;
	    }

		if(!(bufPoint = strstr(packageBuf, "REVISION:"))){
			printf("Error: %s, FW isn't a WWAN Package!\n", __func__);
		}
		else{
			bufPoint = strstr(bufPoint, ":");
			bufPoint++;
			while(*(bufPoint) == ' '){
				bufPoint++;
			}
			for(i = 0; (bufPoint[i] == '.') || ((bufPoint[i] >= '0') && (bufPoint[i] <= '9')); i++){
				fwPkgRevision[i] = bufPoint[i];
			}
			fwPkgRevision[i] = '\0';
		}
		bufPoint = NULL;
		free(packageBuf);
		close(fd);
	}


	/* get Flash WWAN Package REVISION */
    flashPkgRevision[0] = '\0';
	flashwwan = calloc(ZY_WWANPACKAGE_MAXSIZE, sizeof(char));
	if(flashwwan == NULL){
		printf("Error: %s, flashwwan allocation failed!\n", __func__);
		//free(flashwwan);
		return 1;
	}
	
	if((ret = zyUtilMtdDevGetByName(WWAN_PARTITION_NAME, mtddev)) != ZCFG_SUCCESS)
	{
		printf("Error: %s, Can't get WWAN MTD!\n", __func__);
	}
	else{
		packageBuf = calloc(WWANPACKAGE_HEADER_LEN, sizeof(char));
		if(packageBuf == NULL){
			printf("packageBuf allocation failed.\n");
			return 1;
		}
		zyUtilReadBufFromFlash(mtddev, (void *)flashwwan, ZY_WWANPACKAGE_MAXSIZE);
	    if (flashwwan[0] == '\0')
	    {
			printf("Notice: %s, WWAN MTD is empty!\n", __func__);
	    }
		if(strncpy(packageBuf, flashwwan, WWANPACKAGE_HEADER_LEN) == NULL ){
			printf("%s: Could not get Revision\n", __func__);
			if(flashwwan != NULL)
				free(flashwwan);
			return 1;
		}
		else {
			if(!(bufPoint = strstr(packageBuf, "REVISION:"))){
				printf("Notice: %s, No WWAN Package in Flash!\n", __func__);
			}
			else{
				bufPoint = strstr(bufPoint, ":");
				bufPoint++;
				while(*(bufPoint) == ' '){
					bufPoint++;
				}
				for(i = 0; (bufPoint[i] == '.') || ((bufPoint[i] >= '0') && (bufPoint[i] <= '9')); i++){
					flashPkgRevision[i] = bufPoint[i];
				}
				flashPkgRevision[i] = '\0';
			}
			bufPoint = NULL;
			free(packageBuf);
			}
	}
	
	/* use newer Revision */
	if((fwPkgRevision[0] == '\0') && (flashPkgRevision[0] == '\0')){
		printf("%s: Could not get Revision\n", __func__);
		return 1;
	}
	else{
		/*sprintf(cmd_buf, "rm -rf %s", RUN_WWANPACKAGE_FILE);
		system(cmd_buf);*/
		unlink(RUN_WWANPACKAGE_FILE);
		if (strcmp(flashPkgRevision, fwPkgRevision) > 0){
#if 1
			//printf("%s: USE Flash WWAN Package\n", __func__); //debug
			bufPoint = strstr(flashwwan, "END");
			length = (bufPoint - flashwwan);
		    fd = open(RUN_WWANPACKAGE_FILE, O_WRONLY|O_TRUNC|O_CREAT);
			if(fd < 0){
				printf("Error: %s, open file fail!\n", __func__);
				unlink(RUN_WWANPACKAGE_FILE);
				return 1;
			}
			if(write(fd, flashwwan, length) != length){
				printf("Error: %s write fail!\n", __func__);
				unlink(RUN_WWANPACKAGE_FILE);
				return 1;
			}
			close(fd);
			bufPoint = NULL;
			//free(flashwwan);
#else
			sprintf(cmd_buf, "cp %s %s", TMP_WWANPACKAGE_FILE, RUN_WWANPACKAGE_FILE);
			system(cmd_buf);
#endif
		}
		else{
			//printf("%s: USE FW WWAN Package\n", __func__); //debug
			sprintf(cmd_buf, "cp %s %s", FW_WWANPACKAGE_FILE, RUN_WWANPACKAGE_FILE);
			//system(cmd_buf);
			fp = popen(cmd_buf, "r");
			if(fp == NULL){
				printf("copy fw_wwanpackage fail\n");
			}
			else
				pclose(fp);
		}
		if(flashwwan != NULL)
			free(flashwwan);
	}
	return 0;
}

#endif
