#ifndef INC_GLOBAL_MEMORY_HPP_
#define INC_GLOBAL_MEMORY_HPP_
#include <msg.hpp>
#include <cstdio>
#include <pthread.h>
#include <unordered_map>
#include <unordered_set>
#include <map>
#include <IvLoader.h>
#include <OrderbookLoader.h>
#include <SpreadTable.h>

struct TcpChannelConfig
{
	std::string IP;
	unsigned short int PORT;
	bool BIND;
	TcpChannelConfig():
		IP(""),
		PORT(0),
		BIND(false)
	{
	};
};

class user;
using CUserMap = std::unordered_map<unsigned long long, user*>;
extern dbp::cpu::CpuInfo cpuInfo;
extern COmdOrderMap omdcMap;
extern COmdOrderMap stockWarrantomdcMap;
extern COmdOrderMap omdcSlimMap;
extern COmdOrderMap omddMap;
extern CTcpOrderMap tcpMap;
extern COmdOrderMap ptomdcMap;
extern COmdOrderMap s1omdcMap;
extern COmdcAdditionDefinitionsMap omdcAdditionDefinitionsMap;
extern CWarrants warrantToUnderlying;
extern CUnderlyingWarrants underlyingToWarrant;
extern COmddCodeToNameMap codeToName;
extern COmddNameToCodeMap nameToCode;
extern COmddCodeToUnderlying codeTounderlying;
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
extern TcpChannelConfig tcpConfig;

#ifdef FULLTICK
class FullTickBook
{
public:
	enum OrderSide : unsigned short int
	{
		BID = 0,
		ASK = 1,
		NONE = 2
	};
	enum OrderType : char
	{
		Market = '1',
		Limit = '2'
	};
	struct OrderItem
	{
		int price;
		unsigned int quantity;
		OrderSide side;
	};
	struct PriceItem
	{
		unsigned long long quantity;
		unsigned int number_of_order;
	};
	using OrderMap = std::unordered_map<unsigned long long, OrderItem>;
	using BidMap = std::map<int, PriceItem, std::greater<int>>;
	using AskMap = std::map<int, PriceItem, std::less<int>>;
public:
	OrderMap Ords;
	BidMap Bids;
	AskMap Asks;
public:
	bool new_order(unsigned long long id, int price, unsigned int quantity, OrderSide side)
	{
		auto& odr = Ords[id];
		odr.price = price;
		odr.quantity = quantity;
		odr.side = side;
		if (OrderSide::BID == side)
		{
			auto it = Bids.emplace(price, PriceItem()).first;
			it->second.quantity += quantity;
			++it->second.number_of_order;
			return (Bids.begin() == it);
		}
		else
		{
			auto it = Asks.emplace(price, PriceItem()).first;
			it->second.quantity += quantity;
			++it->second.number_of_order;
			return (Asks.begin() == it);
		}
	}
	auto modify_order(unsigned long long id, unsigned int quantity)
	{
		struct result
		{
			OrderSide side;
			bool is_top;
		};
		auto it = Ords.find(id);
		if (Ords.end() != it)
		{
			if (OrderSide::BID == it->second.side)
			{
				auto it2 = Bids.find(it->second.price);
				if (quantity > it->second.quantity)
				{
					it2->second.quantity += (quantity - it->second.quantity);
				}
				else
				{
					it2->second.quantity -= (it->second.quantity - quantity);
				}
				it->second.quantity = quantity;
				return result{it->second.side, (Bids.begin() == it2)};
			}
			else
			{
				auto it2 = Asks.find(it->second.price);
				if (quantity > it->second.quantity)
				{
					it2->second.quantity += (quantity - it->second.quantity);
				}
				else
				{
					it2->second.quantity -= (it->second.quantity - quantity);
				}
				it->second.quantity = quantity;
				return result{it->second.side, (Asks.begin() == it2)};
			}
		}
		return result{OrderSide::NONE, false};
	}

	auto modify_order(unsigned long long id, unsigned int quantity, int price)
	{
		struct result
		{
			OrderSide side;
			bool is_top;
		};
		auto it = Ords.find(id);
		if (Ords.end() != it)
		{
			if (it->second.price == price)
			{
				if (OrderSide::BID == it->second.side)
				{
					auto it2 = Bids.find(it->second.price);
					if (quantity > it->second.quantity)
					{
						it2->second.quantity += (quantity - it->second.quantity);
					}
					else
					{
						it2->second.quantity -= (it->second.quantity - quantity);
					}
					it->second.quantity = quantity;
					return result{it->second.side, (Bids.begin() == it2)};
				}
				else
				{
					auto it2 = Asks.find(it->second.price);
					if (quantity > it->second.quantity)
					{
						it2->second.quantity += (quantity - it->second.quantity);
					}
					else
					{
						it2->second.quantity -= (it->second.quantity - quantity);
					}
					return result{it->second.side, (Asks.begin() == it2)};
				}
			}
			else
			{
				auto side = it->second.side;
				bool isTop = false;
				if (OrderSide::BID == it->second.side)
				{
					auto it2 = Bids.find(it->second.price);
					if (Bids.begin() == it2)
					{
						isTop = true;
					}
					it2->second.quantity -= it->second.quantity;
					it2->second.number_of_order -= 1;
					if (0 == it2->second.number_of_order)
					{
						Bids.erase(it2);
					}
				}
				else
				{
					auto it2 = Asks.find(it->second.price);
					if (Asks.begin() == it2)
					{
						isTop = true;
					}
					it2->second.quantity -= it->second.quantity;
					it2->second.number_of_order -= 1;
					if (0 == it2->second.number_of_order)
					{
						Asks.erase(it2);
					}
				}
				Ords.erase(it);
				auto new_top = new_order(id, price, quantity, side);
				return result{side, new_top && isTop};
			}
		}
		return result{OrderSide::NONE, false};
	}
	auto cancel_order(unsigned long long id)
	{
		struct result
		{
			OrderSide side;
			bool is_top;
		};
		auto it = Ords.find(id);
		if (Ords.end() != it)
		{
			auto side = it->second.side;
			bool isTop = false;
			if (OrderSide::BID == it->second.side)
			{
				auto it2 = Bids.find(it->second.price);
				if (Bids.begin() == it2)
				{
					isTop = true;
				}
				it2->second.quantity -= it->second.quantity;
				it2->second.number_of_order -= 1;
				if (0 == it2->second.number_of_order)
				{
					Bids.erase(it2);
				}
			}
			else
			{
				auto it2 = Asks.find(it->second.price);
				if (Asks.begin() == it2)
				{
					isTop = true;
				}
				it2->second.quantity -= it->second.quantity;
				it2->second.number_of_order -= 1;
				if (0 == it2->second.number_of_order)
				{
					Asks.erase(it2);
				}
			}
			Ords.erase(it);
			return result{side, isTop};
		}
		return result{OrderSide::NONE, false};
	}
	void clear()
	{
		Ords.clear();
		Bids.clear();
		Asks.clear();
	}
};
using FullTickBooks = std::unordered_map<unsigned int, FullTickBook>;
using CodeChannelMap = std::unordered_map<unsigned int, unsigned short int>;
using OmddChannelSet = std::unordered_set<unsigned short int>;
extern FullTickBooks omdcFullTickBook;
extern FullTickBooks omddFullTickBook;
extern CodeChannelMap omdcChannelMap;
extern CodeChannelMap omddChannelMap;
extern OmddChannelSet omddChannelSet;

#endif //FULLTICK




#endif /* INC_GLOBAL_MEMORY_HPP_ */
