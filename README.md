#    Provide a ultra Low Latency trading system for HKEX

## Compile Option

### Debug version:
make CONFIG=debug

### Release version with debug log:
make CONFIG=debug_log

### Release version default:
make
make CONFIG=release

### Enable latency Measure(default will disable measure):
make ENABLE_MEASURE=true


### Support SP + DP
make

### Support SF + DF
make ENABLE_FULLTICK=true


### Support SF + DP
make ENABLE_SF=true


### Support SP + DF
make ENABLE_DF=true


### Enable Full OrderBook Fulltick Trigger(default will disable Full OrderBook Fulltick Trigger):
make ENABLE_FULLTICK=true ENABLE_FULLBOOK=true

make ENABLE_SF=true ENABLE_FULLBOOK=true

make ENABLE_DF=true ENABLE_FULLBOOK=true

### Enable Capture Data (Default will not capture any data)
make ENABLE_CAPTURE=true

need to set capture config: "CAPTURE_PATH": "${HOME}/capture"


### Disable Solarflare
make SOLA=no

### How to Start
../fast_trader_build/fast_trader/bin/smfh_srv ./config/confg???.json

### Start by deamon
../fast_trader_build/fast_trader/bin/smfh_srv fork ./config/confg???.json


## Relogin Top Command:
{"cmd": "relogin", "ref":"relogin_ref1"}

## Dynamic Algo config example:
config_dynamic.json

			"ALGOS":
			{
				"leo_csalgo":
				{
					"lib": "/home/test/fast_trader_build/fast_trader/lib64/libempty.so",
					"params":
					{
					}
				}
			}
## Create Client dynamic library algo develop Kits script
pack_user_algo_development_kits.sh

after run, there is a
fast_trader_dev_kits.tar.gz in folder ../
unzip the fast_trader_dev_kits
Client can develop their algo in fast_trader_dev_kits/dynamic_algos




