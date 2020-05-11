#ifndef INC_GLOBAL_MEMORY_HPP_
#define INC_GLOBAL_MEMORY_HPP_
#ifndef TRADABLE_BOOK_SIZE
#define TRADABLE_BOOK_SIZE 1
#endif
#include <unordered_map>
#include <dbp_cpu.h>
#include <omd.h>
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
};

enum MsgType : unsigned char
{
	NONE = 0x00,
	OMDC_BOOK = 0x01,
	OMDC_TRADE = 0x02,
	OMDD_BOOK = 0x03,
	OMDD_TRADE = 0x04,
	COMMAND = 0x05
};

enum TradeSide : char
{
	NO_SIDE = 0,
	SELL_SIDE = 1,
	BUY_SIDE = -1
};

struct Tradable
{
	unsigned long long m_PkgTime;
	unsigned long long m_MsgTime;
	unsigned long long m_LastTradeQuantity;
	unsigned int m_Code;
	int m_LastTradePrice;
	unsigned short int m_TradeType;
	MsgType m_MsgType;
	TradeSide m_TradeSide;
	OrderItem m_Bid[TRADABLE_BOOK_SIZE];
	OrderItem m_Ask[TRADABLE_BOOK_SIZE];
	Tradable():
		m_PkgTime(0),
		m_MsgTime(0),
		m_LastTradeQuantity(0),
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

typedef std::unordered_map<std::string, std::string> CActivateChannel;
typedef std::vector<CRetranProxy> CRetranVec;
typedef std::unordered_map<unsigned int, COmdOrderbook> COmdOrderMap;
typedef std::unordered_map<unsigned int, unsigned int> CWarrants;
typedef std::unordered_map<unsigned int, std::string> COmddCodeToNameMap;
typedef std::unordered_map<std::string, unsigned int> COmddNameToCodeMap;
typedef std::unordered_map<int, CDefChannel> CDefMap;
typedef std::vector<CStreamChannel> CStreamVec;
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




#endif /* INC_GLOBAL_MEMORY_HPP_ */
