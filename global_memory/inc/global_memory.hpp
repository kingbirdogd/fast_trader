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
#include <string>
#include <fstream>
#include <sys/stat.h>
#include <tools.h>
#include <omd.h>
#include <thread>
#include <pthread.h>

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
extern FILE* log_stream;
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
template <typename SideType>
class FullTickBook
{
public:
	enum OrderSide : SideType
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
	OrderMap OmddbidOrds;
	OrderMap OmddaskOrds;
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
	bool omdd_new_order(unsigned long long id, int price, unsigned int quantity, OrderSide side)
	{

		if (OrderSide::BID == side)
		{
			auto& odr = OmddbidOrds[id];
			odr.price = price;
			odr.quantity = quantity;
			odr.side = side;
			auto it = Bids.emplace(price, PriceItem()).first;
			it->second.quantity += quantity;
			++it->second.number_of_order;

			DEBUG("tm:%llu, OMDD Add Bid, AddBestBid: %d, Iterator Price: %d, Begin Bid Price: %d bidqty %llu, Begin Ask Price: %d askqty %llu\n",
							dbp::tools::srv::current(),
							price, it->first, Bids.begin()->first,Bids.begin()->second.quantity, Asks.begin()->first, Asks.begin()->second.quantity);

			return (Bids.begin() == it);
		}
		else
		{
			auto& odr = OmddaskOrds[id];
			odr.price = price;
			odr.quantity = quantity;
			odr.side = side;

			auto it = Asks.emplace(price, PriceItem()).first;
			it->second.quantity += quantity;
			++it->second.number_of_order;

			DEBUG("tm:%llu, OMDD Add Ask, AddBestAsk: %d, Iterator Price: %d, Begin Bids Price: %d bidqty %llu, Begin Ask Price: %d askqty %llu\n",
										dbp::tools::srv::current(),
										price, it->first, Bids.begin()->first, Bids.begin()->second.quantity, Asks.begin()->first, Asks.begin()->second.quantity);

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

	auto omdd_modify_order(unsigned long long id, unsigned int quantity, OrderSide side)
	{
		struct result
		{
			OrderSide side;
			bool is_top;
		};

		if (OrderSide::BID == side)
		{
			auto it = OmddbidOrds.find(id);
			if (OmddbidOrds.end() != it)
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
		}else{
			auto it = OmddaskOrds.find(id);
			if (OmddaskOrds.end() != it)
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
					it->second.quantity = quantity;
					it->second.price = price;
					auto new_price_it = Bids.emplace(price, PriceItem()).first;
					new_price_it->second.quantity += quantity;
					++new_price_it->second.number_of_order;
					return result{side, ((Bids.begin() == new_price_it) || isTop)};
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
					it->second.quantity = quantity;
					it->second.price = price;
					auto new_price_it = Asks.emplace(price, PriceItem()).first;
					new_price_it->second.quantity += quantity;
					++new_price_it->second.number_of_order;
					return result{side, ((Asks.begin() == new_price_it) || isTop)};
				}
			}
		}
		return result{OrderSide::NONE, false};
	}


	auto omdd_modify_order(unsigned long long id, unsigned int quantity, int price, OrderSide side)
	{
		struct result
		{
			OrderSide side;
			bool is_top;
		};
		if (OrderSide::BID == side)
		{
			auto it = OmddbidOrds.find(id);
			if (OmddbidOrds.end() != it)
			{
				//if (it->second.price == price)
				//{

						auto it2 = Bids.find(price);
						if (quantity > it->second.quantity)
						{
							it2->second.quantity += (quantity - it->second.quantity);
						}
						else
						{
							it2->second.quantity -= (it->second.quantity - quantity);
						}
						it->second.quantity = quantity;


						DEBUG("tm:%llu, OMDD Modify Bid, AddBestBid: %d, Iterator Price: %d, Begin Bid Price: %d bidqty %llu, Begin Ask Price: %d askqty %llu\n",
																		dbp::tools::srv::current(),
																		price, it->first, Bids.begin()->first,Bids.begin()->second.quantity, Asks.begin()->first, Asks.begin()->second.quantity);


						return result{it->second.side, (Bids.begin() == it2)};

				//}

				/*
				else
				{
					auto side = it->second.side;
					bool isTop = false;

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
						it->second.quantity = quantity;
						it->second.price = price;
						auto new_price_it = Bids.emplace(price, PriceItem()).first;
						new_price_it->second.quantity += quantity;
						++new_price_it->second.number_of_order;
						return result{side, ((Bids.begin() == new_price_it) || isTop)};

				}*/
			}
		}else{
			auto it = OmddaskOrds.find(id);
			if (OmddaskOrds.end() != it)
			{
				//if (it->second.price == price)
				//{

						auto it2 = Asks.find(price);
						if (quantity > it->second.quantity)
						{
							it2->second.quantity += (quantity - it->second.quantity);
						}
						else
						{
							it2->second.quantity -= (it->second.quantity - quantity);
						}

						DEBUG("tm:%llu, OMDD Modify Ask, AddBestAsk: %d, Iterator Price: %d, Begin Bids Price: %d bidqty %llu, Begin Ask Price: %d askqty %llu\n",
																				dbp::tools::srv::current(),
																				price, it->first, Bids.begin()->first, Bids.begin()->second.quantity, Asks.begin()->first, Asks.begin()->second.quantity);


						return result{it->second.side, (Asks.begin() == it2)};
/*
				}
				else
				{
					auto side = it->second.side;
					bool isTop = false;

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
						it->second.quantity = quantity;
						it->second.price = price;
						auto new_price_it = Asks.emplace(price, PriceItem()).first;
						new_price_it->second.quantity += quantity;
						++new_price_it->second.number_of_order;
						return result{side, ((Asks.begin() == new_price_it) || isTop)};

				}*/
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

	auto omdd_cancel_order(unsigned long long id, OrderSide side)
	{
		struct result
		{
			OrderSide side;
			bool is_top;
		};
		if (OrderSide::BID == side)
		{
			auto it = OmddbidOrds.find(id);
			if (OmddbidOrds.end() != it)
			{
				auto side = it->second.side;
				bool isTop = false;

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

					DEBUG("tm:%llu, OMDD Delete Bid, Delete Price: %d, Delete Qty: %ull\n",
							dbp::tools::srv::current(),
							it->second.price, it->second.quantity);

					OmddbidOrds.erase(it);
				return result{side, isTop};
			}
		}else{
			auto it = OmddaskOrds.find(id);
			if (OmddaskOrds.end() != it)
			{
				auto side = it->second.side;
				bool isTop = false;

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

					DEBUG("tm:%llu, OMDD Delete Ask, Delete Price: %d, Delete Qty: %ull\n",
												dbp::tools::srv::current(),
												it->second.price, it->second.quantity);

					OmddaskOrds.erase(it);
				return result{side, isTop};
			}
		}
		return result{OrderSide::NONE, false};
	}



	bool isCross()
	{
		if (Bids.empty() || Asks.empty())
			return false;
		if (Bids.begin()->first < Asks.begin()->first)
			return false;
		else
			return true;
	}




	void clear()
	{
		OmddbidOrds.clear();
		OmddaskOrds.clear();
		Ords.clear();
		Bids.clear();
		Asks.clear();
	}
};
template <typename SideType>
using FullTickBooks = std::unordered_map<unsigned int, FullTickBook<SideType>>;
using OmdcFullTickBook =  FullTickBook<unsigned short int>;
using OmddFullTickBook =  FullTickBook<unsigned char>;
extern FullTickBooks<unsigned short int> omdcFullTickBook;
extern FullTickBooks<unsigned char> omddFullTickBook;

#endif //FULLTICK

#ifdef CAPTURE
class capture
{
private:
	std::string _path;
	std::ofstream* _tradable_raw;
	std::ofstream* _tradable_text;
	std::unordered_map<unsigned short int, std::ofstream*> _channel_raw;
public:
	capture():
		_path(""),
		_tradable_raw(nullptr),
		_tradable_text(nullptr),
		_channel_raw()
	{
	}
	capture(const capture&) = default;
	capture(capture&&) = default;
	capture& operator= (const capture&) = default;
	capture& operator= (capture&&) = default;
	~capture()
	{
		if (_tradable_raw)
		{
			_tradable_raw->close();
		}
		if (_tradable_text)
		{
			_tradable_text->close();
		}
		for (auto it = _channel_raw.begin(); it != _channel_raw.end(); ++it)
		{
			it->second->close();
		}
	}
	void init(json& _json)
	{
		try
		{
			auto capture_path = _json["CAPTURE_PATH"].get<std::string>();
			auto convert_capture_path = dbp::tools::srv::replace_env(capture_path);
			dbp::tools::srv::get_YYYYMMDDHHMMSSsss();
			convert_capture_path += "/";
			convert_capture_path += std::to_string(dbp::tools::srv::get_YYYYMMDDHHMMSSsss() / 1000000000);
			struct stat st;
			if (0 != ::stat(convert_capture_path.c_str(), &st))
			{
				if (0 == ::mkdir(convert_capture_path.c_str(), 0777))
				{
					_path = convert_capture_path;
				}
			}
			else if (S_ISDIR(st.st_mode))
			{
				_path = convert_capture_path;
			}
		}
		catch(...)
		{
			std::cout << "do not have CAPTURE_PATH, no capture will do" << std::endl;
		}
		if (_path != "")
		{
			_tradable_raw = new std::ofstream(_path + "/tradable_raw.log", std::ios_base::out | std::ios_base::app | std::ios_base::binary);
			if (!(*_tradable_raw))
			{
				_tradable_raw->close();
				_tradable_raw = nullptr;
			}
			_tradable_text = new std::ofstream(_path + "/tradable_json.log", std::ios_base::out | std::ios_base::app);
			if (!(*_tradable_text))
			{
				_tradable_text->close();
				_tradable_text = nullptr;
			}
		}
	}
	void initChannel(unsigned short int channelId)
	{
		if (_path != "")
		{
			auto channelPath = _path + "/channel_";
			channelPath += std::to_string(channelId);
			channelPath += ".log";
			auto log_file = new std::ofstream(channelPath, std::ios_base::out | std::ios_base::app | std::ios_base::binary);
			if (!(*log_file))
			{
				log_file->close();
				log_file = nullptr;
			}
			else
			{
				_channel_raw[channelId] = log_file;
			}
		}
	}
	void writeChannel(dbp::omd::COmdMsgHeader* _pMsg, unsigned long long _uPkgTm, unsigned short int channelId)
	{
		auto it = _channel_raw.find(channelId);
		if (_channel_raw.end() != it)
		{
			auto log_file = it->second;
			auto tm = dbp::tools::srv::current();
			log_file->write(static_cast<const char*>(static_cast<const void*>(&tm)), sizeof(tm));
			log_file->write(static_cast<const char*>(static_cast<const void*>(&_uPkgTm)), sizeof(_uPkgTm));
			log_file->write(static_cast<const char*>(static_cast<const void*>(_pMsg)), _pMsg->m_uMsgSize);
		}
	}
	void startCapture()
	{
		if (_tradable_raw && _tradable_text)
		{
			std::thread* pThread = new std::thread
			(
				[&]
				()
				{
					using comsumer = typename CBroadCastQueue::comsumer_st;
					auto cfg = broadcastQueue.get_configure();
					comsumer md(broadcastQueue);
					cfg.x_depends_y(md, broadcastQueue);
					while (true)
					{
						Tradable msg;
						if(md.try_dequeue(msg))
						{
							if (MsgType::NONE != msg.m_MsgType)
							{
								_tradable_raw->write(static_cast<const char*>(static_cast<const void*>(&msg)), sizeof(msg));
								(*_tradable_text) << msg.to_json() << std::endl;
							}
						}
					}
				}
			);
			pthread_t iThread = pThread->native_handle();
		#ifndef __APPLE__
			cpu_set_t cpuset;
			CPU_ZERO(&cpuset);
			CPU_SET(cpuInfo.getCore(), &cpuset);
			pthread_setaffinity_np(iThread, sizeof(cpu_set_t), &cpuset);
		#endif
			struct sched_param sch;
			memset (&sch, 0, sizeof(struct sched_param));
			int iPolicy = 0;
			pthread_getschedparam(iThread, &iPolicy, &sch);
			sch.sched_priority = SCHED_PRIORITY;
			pthread_setschedparam(iThread, SCHED_TYPE, &sch);
		}
	}
};

extern capture cap;
#define INIT_CAPTURE(JSON) cap.init(JSON)
#define INIT_CAHNNEL(CHANNEL_ID) cap.initChannel(CHANNEL_ID)
#define WRITE_CHANNEL_DATA(MSG, TM, CHANNEL_ID) cap.writeChannel(MSG, TM, CHANNEL_ID)
#define START_CAPTURE() cap.startCapture()
#else
#define INIT_CAPTURE(JSON)
#define INIT_CAHNNEL(CHANNEL_ID)
#define WRITE_CHANNEL_DATA(MSG, TM, CHANNEL_ID)
#define START_CAPTURE()
#endif //CAPTURE



#endif /* INC_GLOBAL_MEMORY_HPP_ */
