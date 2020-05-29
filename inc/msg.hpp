#ifndef INC_MSG_HPP_
#define INC_MSG_HPP_

#ifndef TRADABLE_BOOK_SIZE
#define TRADABLE_BOOK_SIZE 1
#endif
#include <unordered_map>
#include <dbp_cpu.h>
#include <sync_queue.hpp>
#include <omd.h>
#include <json.hpp>
#include <rapid_ring/ring_buffer_disruptor.hpp>
struct OrderItem
{
	unsigned long long m_uQuantity;
	unsigned int m_uNumberOfOrder;
	int m_iPrice;
	OrderItem():
		m_uQuantity(0),
		m_uNumberOfOrder(0),
		m_iPrice(0)
	{
	}
	~OrderItem() = default;
	nlohmann::json to_json() const
	{
		nlohmann::json json;
		json["m_uQuantity"] = m_uQuantity;
		json["m_uNumberOfOrder"] = m_uNumberOfOrder;
		json["m_iPrice"] = m_iPrice;
		return json;
	}
};

enum MsgType : unsigned char
{
	NONE = 0x00,
	OMDC_BOOK = 0x01,
	OMDC_TRADE = 0x02,
	OMDD_BOOK = 0x03,
	OMDD_TRADE = 0x04,
	COMMAND = 0x05,
	ORDER_LIST = 0x06
};

enum TradeSide : char
{
	NO_SIDE = 0,
	SELL_SIDE = 1,
	BUY_SIDE = -1
};

struct Tradable
{
#ifndef NOT_MEASURE
	unsigned long long m_PkgTime;
	unsigned long long m_MsgTime;
#endif
	unsigned long long m_LastTradeQuantity;
	unsigned long long m_AccumulateBuyQuantity;
	unsigned long long m_AccumulateSellQuantity;
	unsigned long long m_AccumulateBlankQuantity;
	unsigned int m_Code;
	int m_LastTradePrice;
	unsigned short int m_TradeType;
	MsgType m_MsgType;
	TradeSide m_TradeSide;
	OrderItem m_Bid[TRADABLE_BOOK_SIZE];
	OrderItem m_Ask[TRADABLE_BOOK_SIZE];
	Tradable():
#ifndef NOT_MEASURE
		m_PkgTime(0),
		m_MsgTime(0),
#endif
		m_LastTradeQuantity(0),
		m_AccumulateBuyQuantity(0),
		m_AccumulateSellQuantity(0),
		m_AccumulateBlankQuantity(0),
		m_Code(0),
		m_LastTradePrice(0),
		m_TradeType(0),
		m_MsgType(MsgType::NONE),
		m_TradeSide(TradeSide::NO_SIDE),
		m_Bid{OrderItem()},
		m_Ask{OrderItem()}
	{
	}
	~Tradable() = default;
	nlohmann::json to_json()
	{
		nlohmann::json j;
#ifndef NOT_MEASURE
		j["m_PkgTime"] = m_PkgTime;
		j["m_MsgTime"] = m_MsgTime;
#endif
		j["m_LastTradeQuantity"] = m_LastTradeQuantity;
		j["m_AccumulateBuyQuantity"] = m_AccumulateBuyQuantity;
		j["m_AccumulateSellQuantity"] = m_AccumulateSellQuantity;
		j["m_AccumulateBlankQuantity"] = m_AccumulateBlankQuantity;
		j["m_Code"] = m_Code;
		j["m_LastTradePrice"] = m_LastTradePrice;
		j["m_TradeType"] = m_TradeType;
		if (MsgType::NONE == m_MsgType)
		{
			j["m_MsgType"] = "NONE";
		}
		else if (MsgType::OMDC_BOOK == m_MsgType)
		{
			j["m_MsgType"] = "OMDC_BOOK";
		}
		else if (MsgType::OMDC_TRADE == m_MsgType)
		{
			j["m_MsgType"] = "OMDC_TRADE";
		}
		else if (MsgType::OMDD_BOOK == m_MsgType)
		{
			j["m_MsgType"] = "OMDD_BOOK";
		}
		else if (MsgType::OMDD_TRADE == m_MsgType)
		{
			j["m_MsgType"] = "OMDD_TRADE";
		}
		else
		{
			j["m_MsgType"] = "COMMAND";
		}
		if (TradeSide::NO_SIDE == m_TradeSide)
		{
			j["m_TradeSide"] = "NO_SIDE";
		}
		else if (TradeSide::SELL_SIDE == m_TradeSide)
		{
			j["m_TradeSide"] = "SELL_SIDE";
		}
		else
		{
			j["m_TradeSide"] = "BUY_SIDE";
		}
		j["m_Bid"] = nlohmann::json::array();
		j["m_Ask"] = nlohmann::json::array();
		for (std::size_t i = 0; i < TRADABLE_BOOK_SIZE; ++i)
		{
			j["m_Bid"].push_back(m_Bid[i].to_json());
			j["m_Ask"].push_back(m_Ask[i].to_json());
		}
		return j;
	}
};


struct COmdOrderbook: public Tradable
{
	OrderItem m_BidOrder[11];
	OrderItem m_AskOrder[11];
	COmdOrderbook():
		Tradable(),
		m_BidOrder{OrderItem()},
		m_AskOrder{OrderItem()}
	{
	}
	~COmdOrderbook() = default;
};
class CStreamChannel
{
public:
	int m_iHot;
	int m_iRefresh;
	int m_iEpoll;
	unsigned long long m_uChannelIdx;
	std::size_t m_uRetranProxyIdx;
	unsigned short int m_uChannelId;
public:
	CStreamChannel():
		m_iHot(0),
		m_iRefresh(0),
		m_iEpoll(0),
		m_uChannelIdx(0),
		m_uRetranProxyIdx(0),
		m_uChannelId(0)
	{
	}
	~CStreamChannel()
	{
	}
};
class CDefChannel
{
public:
	enum EStatus
	{
		NO_READY = 0,
		READY = 1,
		FINISHED = 2
	};
public:
	EStatus m_Status;
	unsigned int m_uSeq;
	bool m_bAgain;
	CDefChannel():
		m_Status(CDefChannel::NO_READY),
		m_uSeq(0),
		m_bAgain(false)
	{
	}
	~CDefChannel()
	{
	}
};
class CRetranProxy
{
public:
	std::string m_strIp;
	unsigned short int m_uPort;
public:
	CRetranProxy():
		m_strIp(""),
		m_uPort(0)
	{
	}
	~CRetranProxy()
	{
	}
};

class algo;

struct algo_msg_base
{
	unsigned long long id;
	std::string algo_name;
	std::string ref;
	algo* al;
	algo_msg_base():
		id(0),
		algo_name(""),
		ref(""),
		al(nullptr)
	{
	}
	virtual ~algo_msg_base() = default;
	virtual nlohmann::json to_json() const
	{
		nlohmann::json j;
		j["id"] = id;
		j["algo_name"] = algo_name;
		j["ref"] = ref;
		return j;
	}
	virtual void on_command() = 0;
	virtual void release() = 0;
};

typedef std::unordered_map<std::string, std::string> CActivateChannel;
typedef std::vector<CRetranProxy> CRetranVec;
typedef std::unordered_map<unsigned int, COmdOrderbook> COmdOrderMap;
typedef std::unordered_map<unsigned int, unsigned int> CWarrants;
typedef std::unordered_map<unsigned int, std::string> COmddCodeToNameMap;
typedef std::unordered_map<std::string, unsigned int> COmddNameToCodeMap;
typedef std::unordered_map<int, CDefChannel> CDefMap;
typedef std::vector<CStreamChannel> CStreamVec;
using CBroadCastQueue = rapid_ring::mp_ring_buffer_disruptor<Tradable, 2048000>;
using COutputQueue = container::sync_queue<algo_msg_base*>;




#endif /* INC_MSG_HPP_ */
