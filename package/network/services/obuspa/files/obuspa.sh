#!/bin/sh
RUN=1
PROCESS="/usr/sbin/obuspa"
RESTART_INFO="/tmp/tr369_restart_info.dat"
PROCESS_PID=
kill_process() {
        trap '' INT TERM QUIT
        echo "stop ${PROCESS}......."
        if [ -e "/proc/$PROCESS_PID/status" ]; then
                kill -TERM $PROCESS_PID
        else
                killall ${PROCESS##*/}
        fi
        if [ -e "/proc/$PROCESS_PID/status" ]; then
                kill -KILL $PROCESS_PID
        fi
        wait
        RUN=0
        exit
}
trap 'kill_process' INT TERM QUIT
while [ $RUN == 1 ]
do
        (trap - INT TERM QUIT; exec ${PROCESS}) &
        PROCESS_PID=$!
        wait
        # if obuspa process crash, Set "ObuspaRestart" status lets notify don't Send boot inform
        echo "ObuspaRestart" > $RESTART_INFO
        sleep 2
        echo "${PROCESS} restart....."
done
