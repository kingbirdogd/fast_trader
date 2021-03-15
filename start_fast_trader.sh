#!/bin/bash
log_file=${HOME}/log/$(date +%Y%m%d%H%M%S).log;

onload ${HOME}/fast_trader_build/fast_trader/bin/smfh_srv fork ${HOME}/fast_trader/config/config-FT-104.176.json > ${log_file} 2>&1



