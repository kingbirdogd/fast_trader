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
