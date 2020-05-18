#ifndef INC_GLOBAL_MEMORY_HPP_
#define INC_GLOBAL_MEMORY_HPP_
#include <user.hpp>
#include <unordered_map>

using CUserMap = std::unordered_map<unsigned long long, user*>;


extern dbp::cpu::CpuInfo cpuInfo;
extern COmdOrderMap omdcMap;
extern COmdOrderMap omddMap;
extern CWarrants warrantToUnderlying;
extern CWarrants underlyingToWarrant;
extern COmddCodeToNameMap codeToName;
extern COmddNameToCodeMap nameToCode;
extern CRetranVec retranVec;
extern CStreamVec omdcStreams;
extern CStreamVec omddStreams;
extern CActivateChannel mActivateChannel;
extern CBroadCastQueue broadcastQueue;
extern CUserMap userMap;




#endif /* INC_GLOBAL_MEMORY_HPP_ */
