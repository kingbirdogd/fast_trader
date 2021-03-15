#!/bin/bash
ulimit -c unlimited
log_file=${HOME}/log/$(date +%Y%m%d%H%M%S).log;

ZF_ATTR="rx_ring_max=4096" onload --profile=latency ${HOME}/fast_trader_build/fast_trader/bin/smfh_srv fork ${HOME}/fast_trader/config/config-FT-104.176.json > ${log_file} 2>&1



