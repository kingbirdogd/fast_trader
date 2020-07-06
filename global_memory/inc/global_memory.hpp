#ifndef INC_GLOBAL_MEMORY_HPP_
#define INC_GLOBAL_MEMORY_HPP_
#include <msg.hpp>
#include <cstdio>
#include <pthread.h>
#include <unordered_map>
#include <IvLoader.h>
#include <OrderbookLoader.h>

class user;
using CUserMap = std::unordered_map<unsigned long long, user*>;
extern dbp::cpu::CpuInfo cpuInfo;
extern COmdOrderMap omdcMap;
extern COmdOrderMap omddMap;
extern COmdOrderMap ptomdcMap;
extern COmdcAdditionDefinitionsMap omdcAdditionDefinitionsMap;
extern CWarrants warrantToUnderlying;
extern CUnderlyingWarrants underlyingToWarrant;
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
extern CS1SignalMap s1SignalMap;
extern CPriceTableMap pricemarkMap;
extern CPriceDataMap pricedataMap;





#endif /* INC_GLOBAL_MEMORY_HPP_ */
