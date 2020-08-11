#ifndef INC_GLOBAL_MEMORY_HPP_
#define INC_GLOBAL_MEMORY_HPP_
#include <msg.hpp>
#include <cstdio>
#include <pthread.h>
#include <unordered_map>
#include <IvLoader.h>
#include <OrderbookLoader.h>
#include <SpreadTable.h>

class user;
using CUserMap = std::unordered_map<unsigned long long, user*>;
extern dbp::cpu::CpuInfo cpuInfo;
extern COmdOrderMap omdcMap;
extern COmdOrderMap omdcSlimMap;
extern COmdOrderMap omddMap;
extern COmdOrderMap ptomdcMap;
extern COmdOrderMap s1omdcMap;
extern COmdcAdditionDefinitionsMap omdcAdditionDefinitionsMap;
extern CWarrants warrantToUnderlying;
extern CUnderlyingWarrants underlyingToWarrant;
extern COmddCodeToNameMap codeToName;
extern COmddNameToCodeMap nameToCode;
extern COmddCodeToUnderlying codeTounderlying;
extern cnSignalMap CsSignalMap;
extern CRetranVec retranVec;
extern CStreamVec omdcStreams;
extern CStreamVec omddStreams;
extern CStreamVec pricetableStreams;
extern CStreamVec s1Streams;
extern CActivateChannel mActivateChannel;
extern CBroadCastQueue broadcastQueue;
extern COutputQueue ouputQueue;
extern CUserMap userMap;
extern FILE* input_stream;
extern FILE* output_stream;
extern OrderbookLoader orderbookLoader;
extern IvLoader ivLoader;
extern int SCHED_TYPE;
extern int SCHED_PRIORITY;
extern CS1SignalMap s1SignalMap;
extern CPriceTableMap pricemarkMap;
extern CPriceDataMap pricedataMap;
extern AlgoParameter algoParam;
extern SpreadTable spreadTable;





#endif /* INC_GLOBAL_MEMORY_HPP_ */
