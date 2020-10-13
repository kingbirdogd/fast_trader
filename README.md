#    Provide a ultra Low Latency trading system for HKEX

## Compile Option

### Debug version:
make CONFIG=debug

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
