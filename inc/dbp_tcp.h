#ifndef __DBP_TCP__
#define __DBP_TCP__
#include <dbp_shm.h>
namespace dbp
{
	namespace tcp
	{
		const static unsigned long long maskFlagOmdcOrderBook = 0xF000000000000000;
		const static unsigned long long maskFlagOmddOrderBook = 0xF000000000000000;
		const static unsigned long long FlagOmdcOrderBook = 0x0;
		const static unsigned long long FlagOmddOrderBook = 0x2000000000000000;
		const static unsigned long long FlagOmdcTrade = 0x1000000000000000;
		const static unsigned long long FlagOmddTrade = 0x3000000000000000;
		const static unsigned long long FlagBrokerQueue = 0x4000000000000000;
		const static unsigned long long FlagInvalidMsg = 0xF000000000000000;
		const static unsigned long long FlagInvalidCode = 0xE000000000000000;
		const static unsigned long long FlagNotReady = 0xC000000000000000;
		#pragma pack(push, 1)
		struct SubscribeMessage
		{
			unsigned long long m_uFlag;
			unsigned int m_uCode;
			unsigned short m_commoditycode;
			unsigned short m_instrumentcode;
			unsigned short m_expirydate;
			unsigned short m_strike;
			bool m_bSubscribeFlag;
			unsigned char m_uFiller[3];

			SubscribeMessage():
				m_uFlag(0),
				m_uCode(0),
				m_commoditycode(0),
				m_instrumentcode(0),
				m_expirydate(0),
				m_strike(0),
				m_bSubscribeFlag(false),
				m_uFiller{0}
			{
			}
			~SubscribeMessage()
			{
			}
			void invalidMsg()
			{
				m_uFlag = FlagInvalidMsg;
			}
			void invalidCode()
			{
				m_uFlag = FlagInvalidCode;
			}
			void notReady()
			{
				m_uFlag = FlagNotReady;
			}
			bool is_invalidMsg()
			{
				return (m_uFlag == FlagInvalidMsg);
			}
			bool is_invalidCode()
			{
				return (m_uFlag == FlagInvalidCode);
			}
			bool is_notReady()
			{
				return (m_uFlag == FlagNotReady);
			}
		};
		struct SubscribeMessageReturn
		{
			SubscribeMessage m_SubscribeKey;
			unsigned int m_uQueueIdx;
			unsigned int m_uTradableIdx;
			SubscribeMessageReturn():
				m_SubscribeKey(),
				m_uQueueIdx(0),
				m_uTradableIdx(0)
			{
			}
			~SubscribeMessageReturn()
			{
			}
		};
		#pragma pack(pop)
	}
}
#endif


