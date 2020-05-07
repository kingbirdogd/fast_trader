#ifndef __DBP_SHM__
#define __DBP_SHM__
#include <vector>
#include <spin.h>
#include <array>
#define SET_BIT(DST, SRC) (DST) |= (SRC)
#define UNSET_BIT(DST, SRC) (DST) &= (~(SRC))
#define SET_BID_PRICE_FIRST_CHANGE(DST) (DST) |= (dbp::shm::FlagBPBase)
#define SET_BID_QUANTITY_FIRST_CHANGE(DST) (DST) |= (dbp::shm::FlagBQBase)
#define SET_BID_NUMBER_FIRST_CHANGE(DST) (DST) |= (dbp::shm::FlagBNBase)
#define SET_ASK_PRICE_FIRST_CHANGE(DST) (DST) |= (dbp::shm::FlagAPBase)
#define SET_ASK_QUANTITY_FIRST_CHANGE(DST) (DST) |= (dbp::shm::FlagAQBase)
#define SET_ASK_NUMBER_FIRST_CHANGE(DST) (DST) |= (dbp::shm::FlagANBase)
#define SET_BID_PRICE_CHANGE(DST, POSITION) (DST) |= (dbp::shm::FlagBPBase << (POSITION * 3))
#define SET_BID_QUANTITY_CHANGE(DST, POSITION) (DST) |= (dbp::shm::FlagBQBase << (POSITION * 3))
#define SET_BID_NUMBER_CHANGE(DST, POSITION) (DST) |= (dbp::shm::FlagBNBase << (POSITION * 3))
#define SET_ASK_PRICE_CHANGE(DST, POSITION) (DST) |= (dbp::shm::FlagAPBase << (POSITION * 3))
#define SET_ASK_QUANTITY_CHANGE(DST, POSITION) (DST) |= (dbp::shm::FlagAQBase << (POSITION * 3))
#define SET_ASK_NUMBER_CHANGE(DST, POSITION) (DST) |= (dbp::shm::FlagANBase << (POSITION * 3))
#define CHECK_FIRST_BID_AND_SET(DST, LEFT, RIGHT) \
	if (((LEFT).m_iPrice) != ((RIGHT).m_iPrice)) \
	{\
		SET_BID_PRICE_FIRST_CHANGE(DST);\
	}\
	if (((LEFT).m_uQuantity) != ((RIGHT).m_uQuantity)) \
	{\
		SET_BID_QUANTITY_FIRST_CHANGE(DST);\
	}\
	if (((LEFT).m_uNumberOfOrder) != ((RIGHT).m_uNumberOfOrder)) \
	{\
		SET_BID_NUMBER_FIRST_CHANGE(DST);\
	}\
	(LEFT) = (RIGHT)
#define CHECK_FIRST_ASK_AND_SET(DST, LEFT, RIGHT) \
	if (((LEFT).m_iPrice) != ((RIGHT).m_iPrice)) \
	{\
		SET_ASK_PRICE_FIRST_CHANGE(DST);\
	}\
	if (((LEFT).m_uQuantity) != ((RIGHT).m_uQuantity)) \
	{\
		SET_ASK_QUANTITY_FIRST_CHANGE(DST);\
	}\
	if (((LEFT).m_uNumberOfOrder) != ((RIGHT).m_uNumberOfOrder)) \
	{\
		SET_ASK_NUMBER_FIRST_CHANGE(DST);\
	}\
	(LEFT) = (RIGHT)
#define CHECK_BID_AND_SET(DST, LEFT, RIGHT, POSITION) \
	if (((LEFT).m_iPrice) != ((RIGHT).m_iPrice)) \
	{\
		SET_BID_PRICE_CHANGE(DST, POSITION);\
	}\
	if (((LEFT).m_uQuantity) != ((RIGHT).m_uQuantity)) \
	{\
		SET_BID_QUANTITY_CHANGE(DST, POSITION);\
	}\
	if (((LEFT).m_uNumberOfOrder) != ((RIGHT).m_uNumberOfOrder)) \
	{\
		SET_BID_NUMBER_CHANGE(DST, POSITION);\
	}\
	(LEFT) = (RIGHT)
#define CHECK_ASK_AND_SET(DST, LEFT, RIGHT, POSITION) \
	if (((LEFT).m_iPrice) != ((RIGHT).m_iPrice)) \
	{\
		SET_ASK_PRICE_CHANGE(DST, POSITION);\
	}\
	if (((LEFT).m_uQuantity) != ((RIGHT).m_uQuantity)) \
	{\
		SET_ASK_QUANTITY_CHANGE(DST, POSITION);\
	}\
	if (((LEFT).m_uNumberOfOrder) != ((RIGHT).m_uNumberOfOrder)) \
	{\
		SET_ASK_NUMBER_CHANGE(DST, POSITION);\
	}\
	(LEFT) = (RIGHT)
#define CHECK_BID_REST_AND_SET(DST, LEFT, RIGHT) \
	for (unsigned int i = 1; i < 10; ++i)\
	{\
		CHECK_BID_AND_SET(DST, ((LEFT)[i]), ((RIGHT)[i]), i);\
	}
#define CHECK_ASK_REST_AND_SET(DST, LEFT, RIGHT) \
	for (unsigned int i = 1; i < 10; ++i)\
	{\
		CHECK_ASK_AND_SET(DST, ((LEFT)[i]), ((RIGHT)[i]), i);\
	}
//#define CHECK_TRADE_FLAG(TRADABLE) (0 != ((TRADABLE).m_uOrderBookUpdateFlag & dbp::shm::FlagTradeMask))

namespace dbp
{
	namespace shm
	{
		const static unsigned long long FlagBPBase = 0x1;
		const static unsigned long long FlagBQBase = 0x2;
		const static unsigned long long FlagBNBase = 0x4;
		const static unsigned long long FlagAPBase = FlagBPBase << 30;
		const static unsigned long long FlagAQBase = FlagBQBase << 30;
		const static unsigned long long FlagANBase = FlagBNBase << 30;
		const static unsigned long long FlagOrderBookMask = 0xFFFFFFFFFFFFFFF;
		const static unsigned long long FlagTradeMask = 0x1000000000000000;
		#pragma pack(push, 1)
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
			~OrderItem()
			{
			}
		};
		struct TradeItem
		{
			OrderItem m_Bid;
			OrderItem m_Ask;
			unsigned long long m_uTm;
			unsigned long long m_uQuantity;
			unsigned long long m_uAccumulatedQuantity;
			unsigned int m_uSeq;
			unsigned int m_uTradeCnt;
			unsigned int m_iTradeType;
			int m_iTradeSide;
			int m_iPrice;
			TradeItem():
				m_Bid(),
				m_Ask(),
				m_uTm(0),
				m_uQuantity(0),
				m_uAccumulatedQuantity(0),
				m_uSeq(0),
				m_uTradeCnt(0),
				m_iTradeType(0),
				m_iTradeSide(0),
				m_iPrice(0)
			{
			}
			~TradeItem()
			{
			}
		};
		struct BrokerPosition
		{
			int m_iBrokerId;
			int m_iBorkerPosition;
			int m_iBrokerOrder;
			BrokerPosition():
				m_iBrokerId(-1),
				m_iBorkerPosition(0),
				m_iBrokerOrder(0)
			{
			}
			~BrokerPosition()
			{
			}
		};
		struct BrokerQueue
		{
			unsigned short int m_uItem;
			char m_cType;
			char m_cFiller;
			BrokerQueue():
				m_uItem(0),
				m_cType(0),
				m_cFiller(0)
			{
			}
			~BrokerQueue()
			{
			}
		};
		struct Tradable
		{
		private:
			using OrderItems = std::array<OrderItem, 11>;
			using TradeItems = std::array<TradeItem, 256>;
		public:
			OrderItems m_Bid;
			OrderItems m_Ask;
			TradeItems m_TradeRecord;
			spin::spin_shared_mutex m_Mutex;
			unsigned long long m_uTm;
			unsigned long long m_uTradeTime;
			unsigned long long m_uSubscribeFlag;
			unsigned long long m_uOrderBookUpdateFlag;
			unsigned long long m_uLastTradeIdx;
			unsigned long long m_uTradeQuanty;
			unsigned long long m_uAccumulateBuyQuantity;
			unsigned long long m_uAccumulateSellQuantity;
			unsigned long long m_uAccumulateBlankQuantity;
			unsigned int m_uQueueIdx;
			unsigned int m_uTradeSeq;
			unsigned int m_uOrderSeq;
			unsigned int m_uTradeCnt;
			unsigned int m_uOrderCnt;
			unsigned int m_uCode;
			unsigned int m_uLotSize;
			int m_iTradePrice;
			unsigned int m_iTradeType;
			int m_iTradeSide;
			Tradable():
				m_Bid{OrderItem()},
				m_Ask{OrderItem()},
				m_TradeRecord{TradeItem()},
				m_Mutex(),
				m_uTm(0),
				m_uTradeTime(0),
				m_uSubscribeFlag(0),
				m_uOrderBookUpdateFlag(0),
				m_uLastTradeIdx(0),
				m_uTradeQuanty(0),
				m_uAccumulateBuyQuantity(0),
				m_uAccumulateSellQuantity(0),
				m_uAccumulateBlankQuantity(0),
				m_uQueueIdx(0xFFFFFFFF),
				m_uTradeSeq(0),
				m_uOrderSeq(0),
				m_uTradeCnt(0),
				m_uOrderCnt(0),
				m_uCode(0),
				m_uLotSize(0),
				m_iTradePrice(0),
				m_iTradeType(0),
				m_iTradeSide(0)
			{
			}
			~Tradable()
			{
			}
		};
		struct TradablePosition
		{
			spin::spin_shared_mutex m_Mutex;
			unsigned long long m_uOrderBookTimeStamp;
			unsigned long long m_uBrokerQueueTimeStmap;
			BrokerPosition m_LPBestBid;
			BrokerPosition m_LPBestAsk;
			BrokerPosition m_CISBestBid;
			BrokerPosition m_CISBestAsk;
			unsigned int m_uSbuscribeFlag;
			unsigned int m_uFirstCome;
			unsigned int m_uCode;
			unsigned int m_uOrderBookSeq;
			unsigned int m_uBrokerQueueSeq;
			unsigned int m_uLastSeq;
			int m_iBestBid;
			int m_iBestAsk;
			BrokerQueue m_BidBQArray[40];
			BrokerQueue m_AskBQArray[40];
			TradablePosition():
				m_Mutex(),
				m_uOrderBookTimeStamp(0),
				m_uBrokerQueueTimeStmap(0),
				m_LPBestBid(),
				m_LPBestAsk(),
				m_CISBestBid(),
				m_CISBestAsk(),
				m_uSbuscribeFlag(0),
				m_uFirstCome(0),
				m_uCode(0),
				m_uOrderBookSeq(0),
				m_uBrokerQueueSeq(0),
				m_uLastSeq(0),
				m_iBestBid(0),
				m_iBestAsk(0),
				m_BidBQArray{BrokerQueue()},
				m_AskBQArray{BrokerQueue()}
			{
			}
			~TradablePosition()
			{
			}
		};
		struct OmdcTradable
		{
			Tradable m_Tradable;
			TradablePosition m_Position;
			OmdcTradable():
				m_Tradable(),
				m_Position()
			{
			}
			~OmdcTradable()
			{
			}
		};
		struct TradableQueueItem
		{
			unsigned long long m_uEventFlag;
			unsigned long long m_uCode;
			spin::spin_shared_mutex m_Mutex;
			unsigned int m_uTSequence;
			unsigned int m_uOSequence;
			unsigned int m_uTradeIdx;
			TradableQueueItem():
				m_uEventFlag(0),
				m_uCode(0),
				m_Mutex(),
				m_uTSequence(0),
				m_uOSequence(0),
				m_uTradeIdx(0)
			{
			}
			~TradableQueueItem()
			{
			}
		};
		struct PositionQueueItem
		{
			unsigned long long m_uCode;
			spin::spin_shared_mutex m_Mutex;
			unsigned int m_uOrderBookSequence;
			unsigned int m_uBrokerQueueSequence;
			PositionQueueItem():
				m_uCode(0),
				m_Mutex(),
				m_uOrderBookSequence(0),
				m_uBrokerQueueSequence(0)
			{
			}
			~PositionQueueItem()
			{
			}
		};
		struct QueueHeader
		{
			std::atomic<unsigned long long> m_uIdx;
			unsigned long long m_uQueueSize;
			QueueHeader():
				m_uIdx(0),
				m_uQueueSize(0)
			{
			}
			~QueueHeader()
			{
			}
		};
		struct MemoryHeader
		{
			unsigned int m_uOmdcTradableCnt;
			unsigned int m_uOmddTradableCnt;
			unsigned int m_uOmdcChannelCnt; 
			unsigned int m_uOmddChannelCnt; 
			MemoryHeader():
				m_uOmdcTradableCnt(0),
				m_uOmddTradableCnt(0),
				m_uOmdcChannelCnt(0),
				m_uOmddChannelCnt(0)
			{
			}
			~MemoryHeader()
			{
			}
		};
		#pragma pack(pop)
		struct MemoryRef
		{
			MemoryHeader* m_pHeader;
			QueueHeader* m_pBrokerPositionQueueHeader;
			OmdcTradable* m_pOmdcTradable;
			Tradable* m_pOmddTradable;
			PositionQueueItem* m_pPositionQueue;
			std::vector<QueueHeader*> m_vecOmdcQueueHeader;
			std::vector<QueueHeader*> m_vecOmddQueueHeader;
			std::vector<TradableQueueItem*> m_vecOmdcQueue;
			std::vector<TradableQueueItem*> m_vecOmddQueue;
			MemoryRef():
				m_pHeader(nullptr),
				m_pBrokerPositionQueueHeader(nullptr),
				m_pOmdcTradable(nullptr),
				m_pOmddTradable(nullptr),
				m_pPositionQueue(nullptr),
				m_vecOmdcQueueHeader(),
				m_vecOmddQueueHeader(),
				m_vecOmdcQueue(),
				m_vecOmddQueue()
			{
			}
			~MemoryRef()
			{
			}
		};
	}
}
#endif


