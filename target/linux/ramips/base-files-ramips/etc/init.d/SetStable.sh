#!/bin/sh
#
# Script to set image stable flag and debug flag
#
#

#
#check if dual image
#
if (`cat /proc/mtd |grep -q Kernel2`) ; then
    CONFIG_DUAL=y
else
    CONFIG_DUAL=n
fi

#
#Image1 boot ok, Add Image1Stable Flag 
#
nvram show uboot > /tmp/.uboot-startup
nvram-factory show Factory >/tmp/.factory-startup

if [ "$CONFIG_DUAL" = "y" ]; then

if (`grep -q Image1Stable=1 /tmp/.uboot-startup`) ; then
	echo "Image1 Already Stable"
else
	echo "Image1 Boot Successfully, Set Image1Stable"
        nvram setro uboot Image1Stable 1 ; nvram setro uboot Image1Try 0 ; nvram show uboot > /tmp/.uboot-startup ;nvram commit ;sync
fi
#
#Check Debug Flag 
#
if (`grep -q DebugFlag=1 /tmp/.uboot-startup`) ; then
        echo "Debug flag is enabled, Set Debug Flag to Disable"
        nvram setro uboot DebugFlag 0 ; nvram show uboot > /tmp/.uboot-startup ;nvram commit ;sync
fi
	
fi
rm /tmp/.nvramcache
rm /tmp/.factorycache
#chmod 666 /tmp/.uboot-startup
#chmod 666 /tmp/.factory-startup

# drop caches
echo 3 > /proc/sys/vm/drop_caches

#
#Manual Set Stable POWER LED
#Currently our zyledctl can handle LED behavior, just call it.
#
#if (`grep -q ModelID=7240 /tmp/.uboot-startup`) ; then
#/bin/zyledctl POWER_G on >&-
#elif (`grep -q ModelID=7480 /tmp/.uboot-startup`) ; then
#/bin/zyledctl POWER_G on >&-
#elif (`grep -q ModelID=7461 /tmp/.uboot-startup`) ; then
/bin/zyledctl POWER_G on >&-
#fi
#
#Show selftest result
#
if [ -f /usr/sbin/self_check.sh ]; then
/usr/sbin/self_check.sh
fi

