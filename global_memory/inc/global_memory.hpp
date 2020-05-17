#ifndef INC_GLOBAL_MEMORY_HPP_
#define INC_GLOBAL_MEMORY_HPP_
#include <msg.hpp>

typedef std::unordered_map<std::string, std::string> CActivateChannel;
typedef std::vector<CRetranProxy> CRetranVec;
typedef std::unordered_map<unsigned int, COmdOrderbook> COmdOrderMap;
typedef std::unordered_map<unsigned int, unsigned int> CWarrants;
typedef std::unordered_map<unsigned int, std::string> COmddCodeToNameMap;
typedef std::unordered_map<std::string, unsigned int> COmddNameToCodeMap;
typedef std::unordered_map<int, CDefChannel> CDefMap;
typedef std::vector<CStreamChannel> CStreamVec;
using CBroadCastQueue = rapid_ring::mp_ring_buffer_disruptor<Tradable, 2048000>;
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




#endif /* INC_GLOBAL_MEMORY_HPP_ */
