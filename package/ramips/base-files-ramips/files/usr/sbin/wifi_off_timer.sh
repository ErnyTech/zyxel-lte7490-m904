#!/bin/sh
#
# Script to turn off wifi after 1~60 miniutes
#
[ $# -eq 0 ] && { echo "Usage: $0 [min] to turn off primary wifi"; exit 1; }
time=$1
time="${time:-1}"
i=$time
LOG_TIMER_FILE="/tmp/wifi_off_timer"

if [ $i -eq 0 ]; then
	echo "Timer set to 0 , disable action!"
	rm $LOG_TIMER_FILE
	exit 0
fi

if [ $i -gt 60 ] || [ $i -lt 0 ]; then
	echo "Timer valid range is 0~60!!!"
	exit 1
fi

if [ -s $LOG_TIMER_FILE ]; then
rm $LOG_TIMER_FILE
fi

while [ $i -gt 0 ]; do
	echo "$i" > $LOG_TIMER_FILE ;
	zyledctl WIFI on > /dev/null ;
#test
#	sleep 1;        
	sleep 1m;
        i=`expr $i - 1`;
if [ $i -lt 6 ]; then
        echo "Notice! Remaining $i miniute to turn off wifi" ;
fi
done
echo "Notice! Turning off wifi now!"
wlan config --status primary disable
#sleep 10
#wlan config --status primary disable
rm $LOG_TIMER_FILE
exit 0
