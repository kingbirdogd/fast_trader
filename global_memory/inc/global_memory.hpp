#ifndef INC_GLOBAL_MEMORY_HPP_
#define INC_GLOBAL_MEMORY_HPP_
#include <msg.hpp>
#include <cstdio>
#include <pthread.h>
#include <unordered_map>

class user;
using CUserMap = std::unordered_map<unsigned long long, user*>;
extern dbp::cpu::CpuInfo cpuInfo;
extern COmdOrderMap omdcMap;
extern COmdOrderMap omddMap;
extern CWarrants warrantToUnderlying;
extern CWarrants underlyingToWarrant;
extern COmddCodeToNameMap codeToName;
extern COmddNameToCodeMap nameToCode;
extern COmddCodeToUnderlying codeTounderlying;
extern CRetranVec retranVec;
extern CStreamVec omdcStreams;
extern CStreamVec omddStreams;
extern CActivateChannel mActivateChannel;
extern CBroadCastQueue broadcastQueue;
extern COutputQueue ouputQueue;
extern CUserMap userMap;
extern FILE* input_stream;
extern FILE* output_stream;
extern int SCHED_TYPE;
extern int SCHED_PRIORITY;





#endif /* INC_GLOBAL_MEMORY_HPP_ */
