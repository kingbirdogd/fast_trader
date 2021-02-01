#!/bin/bash
rm -rf ../fast_trader_dev_kits*;
mkdir ../fast_trader_dev_kits;
cp common.mk ../fast_trader_dev_kits/
cp Makefile ../fast_trader_dev_kits/
cp Makefile ../fast_trader_dev_kits/
cp -r inc ../fast_trader_dev_kits/
cp -r rapid_ring ../fast_trader_dev_kits/
cp -r top_client ../fast_trader_dev_kits/
cp -r user_algo ../fast_trader_dev_kits/
cp -r dynamic_algos ../fast_trader_dev_kits/
cp -r net ../fast_trader_dev_kits/
cp -r dbp_common_cbbc ../fast_trader_dev_kits/
cp -r global_memory ../fast_trader_dev_kits/
rm -rf ../fast_trader_dev_kits/net/tcp_service;
cd ..;
tar -cvf fast_trader_dev_kits.tar fast_trader_dev_kits/
gzip -9 fast_trader_dev_kits.tar;
rm -rf fast_trader_dev_kits;
cd -;
