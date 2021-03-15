#!/bin/bash
#/usr/bin/ps -ef|/usr/bin/grep "^${USER}"|/usr/bin/grep dbp_trader_server|/usr/bin/grep -v grep|/usr/bin/awk '{print $2}'|/usr/bin/xargs -i kill -9 {}
#/usr/bin/ps -ef|/usr/bin/grep "^${USER}"|/usr/bin/grep node|/usr/bin/grep -v grep|/usr/bin/awk '{print $2}'|/usr/bin/xargs -i kill -9 {}

PID=`ps -eaf | grep smfh_srv | grep -v grep | awk '{print $2}'`
if [[ "" !=  "$PID" ]]; then
  echo "killing $PID"
  kill -9 $PID
fi

PID2=`ps -eaf | grep node | grep -v grep | awk '{print $2}'`
if [[ "" !=  "$PID2" ]]; then
  echo "killing $PID2"
  kill -9 $PID2
fi




