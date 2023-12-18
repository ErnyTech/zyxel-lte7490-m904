#!/bin/sh
#
# mount the ubi partition , default data partition
#
UBICONTROL_DEV_NAME="ubi_ctrl"
UBITOOL_PATH="/usr/sbin"
UBIINFO_EXE="$UBITOOL_PATH/ubinfo"
MTD_EXE="/usr/bin/mtd_write"
Default_Partition="data"
Mount_Location="/data"
Mount_Flag=".ubicreated"
Dev_MajorNum=`$UBIINFO_EXE |grep "device major/minor:" |cut -d " " -f5 |cut -d ":" -f1`
Dev_MinorNum=`$UBIINFO_EXE |grep "device major/minor:" |cut -d ":" -f3`
HEX_DATA_SIZE=`cat /proc/mtd |grep "data" | cut -c 7-14`
HEX_SECTOR_SIZE=`cat /proc/mtd |grep "data" | cut -c 16-24`
DEC_DATA_SIZE=$((0x${HEX_DATA_SIZE}))
DEC_SECTOR_SIZE=$((0x${HEX_SECTOR_SIZE}))
ALL_SECTOR_NUM=$(($DEC_DATA_SIZE/$DEC_SECTOR_SIZE))
LOOP_SECTOR_NUM=$(($ALL_SECTOR_NUM-1))
var=0

check_ubi_dev()
{
	if  ! (`ls /dev |grep -q "$UBICONTROL_DEV_NAME"`); then
	mknod /dev/$UBICONTROL_DEV_NAME c $Dev_MajorNum $Dev_MinorNum
	fi
}
				
get_partition_num()
{
	PART_NAME=$Default_Partition
	PART_NUM=`cat /proc/mtd | grep -m 1 $PART_NAME | awk -F ":" '{print $1}' | grep -Eo '[0-9]+'`	
}
		
ubi_attach()
{
	$UBITOOL_PATH/ubiattach -m $PART_NUM
}
				
check_attach()
{
#check if attach error
	if !($UBIINFO_EXE -a |grep -q 'Present UBI devices:'); then
	echo "UBI attach fail, try to reinit the partition"
	$UBITOOL_PATH/ubidetach -m $PART_NUM
	for i in $(seq 0 $LOOP_SECTOR_NUM);
	do
		let var=i*$DEC_SECTOR_SIZE
		$MTD_EXE -q -q erasesector $var $PART_NAME
	done
	$UBITOOL_PATH/ubiattach -m $PART_NUM
	fi
#check if reattach failed
	if !($UBIINFO_EXE -a |grep -q 'Present UBI devices:'); then
	echo "Reinit fail!!!! EXIT"
	exit
	fi
}
																
get_ubix()
{
	#get last ubi attached number
	UBIX_NAME=`$UBIINFO_EXE -a |grep -w -x ubi[0-9] |tail -1`
	UBIX_MAJOR=`cat /proc/devices |grep $UBIX_NAME |cut -d " " -f1`
	UBIX_MINOR=`$UBIINFO_EXE -a |ubinfo -a |grep "input/output unit size:" -A 1 |tail -1 |cut -d ":" -f3`

        if !(`ls /dev |grep -q $UBIX_NAME`) ; then
	mknod /dev/$UBIX_NAME c $UBIX_MAJOR $UBIX_MINOR
        fi
        
}
        
check_ubi_vol()
{
       	#check if vol is already exist
	if !($UBIINFO_EXE -a |grep -q "$PART_NAME"); then
	$UBITOOL_PATH/ubimkvol /dev/$UBIX_NAME -N $PART_NAME -m
	fi
}
 		        				
mount_check()
{
	mount -t ubifs $UBIX_NAME:$PART_NAME $Mount_Location
	if (`ls -al $Mount_Location |grep -q $Mount_Flag`); then
        echo "Mount successfully!! UBI Partition already with check flag\n"
	else
        echo "This is new UBI partition, write new check flag\n"
        touch "$Mount_Location/$Mount_Flag" ; sync ;sync
	fi

}

check_ubi_dev
get_partition_num
ubi_attach
check_attach
get_ubix
check_ubi_vol
mount_check
