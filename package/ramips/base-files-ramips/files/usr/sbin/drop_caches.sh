#!/bin/sh
threshold=75000
SReclaimable=`cat /proc/meminfo | grep SReclaimable | sed 's/^.*imable://g' | sed 's/kB *$//g' | sed 's/ //g'`
echo "SReclaimable = $SReclaimable"
if [ $SReclaimable -ge $threshold ];then
	echo "Clean Drop Caches..."
	cat /proc/slabinfo
	echo 3 > /proc/sys/vm/drop_caches
fi