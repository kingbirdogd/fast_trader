#include <global_memory.hpp>
dbp::cpu::CpuInfo cpuInfo;
COmdOrderMap omdcMap;
COmdOrderMap omddMap;
CWarrants warrantToUnderlying;
CWarrants underlyingToWarrant;
COmddCodeToNameMap codeToName;
COmddNameToCodeMap nameToCode;
CRetranVec retranVec;
CStreamVec omdcStreams;
CStreamVec omddStreams;
CActivateChannel mActivateChannel;
CBroadCastQueue broadcastQueue;
COutputQueue ouputQueue;
CUserMap userMap;
FILE* input_stream = stdin;
FILE* output_stream = stdout;
int SCHED_TYPE = SCHED_FIFO;
int SCHED_PRIORITY = 99;




