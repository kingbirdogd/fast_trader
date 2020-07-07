#include <global_memory.hpp>
dbp::cpu::CpuInfo cpuInfo;
COmdOrderMap omdcMap;
COmdOrderMap omddMap;
COmdOrderMap ptomdcMap;
COmdOrderMap s1omdcMap;
COmdcAdditionDefinitionsMap omdcAdditionDefinitionsMap;
CWarrants warrantToUnderlying;
CUnderlyingWarrants underlyingToWarrant;
COmddCodeToNameMap codeToName;
COmddNameToCodeMap nameToCode;
COmddCodeToUnderlying codeTounderlying;
CRetranVec retranVec;
CStreamVec omdcStreams;
CStreamVec omddStreams;
CStreamVec pricetableStreams;
CActivateChannel mActivateChannel;
CBroadCastQueue broadcastQueue;
COutputQueue ouputQueue;
CUserMap userMap;
OrderbookLoader orderbookLoader;
IvLoader ivLoader;
FILE* input_stream = stdin;
FILE* output_stream = stdout;
int SCHED_TYPE = SCHED_FIFO;
int SCHED_PRIORITY = 99;
CS1SignalMap s1SignalMap;
CPriceTableMap pricemarkMap;
CPriceDataMap pricedataMap;





