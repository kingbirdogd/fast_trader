#ifndef __SMFH_SRV_S1SIGNAL__
#define __SMFH_SRV_S1SIGNAL__
#include <tools.h>
#include <global_memory.hpp>
#include "smfh_srv_orderbook.h"
#include "AlgoEngineData.h"
#ifndef NOT_MEASURE
inline static void handleS1Signal(dbp::omd::COmdMsgHeader* _pMsg, unsigned long long _uPkgTm)
#else
inline static void handleS1Signal(dbp::omd::COmdMsgHeader* _pMsg, unsigned long long)
#endif
{
	unsigned int uSecurityCode = OMD_GET_VALUE(_pMsg, 4, unsigned int);
	auto its = s1SignalMap.find(uSecurityCode);
	if(s1SignalMap.end() == its)
		return;

	auto it = s1omdcMap.find(uSecurityCode);
	if (s1omdcMap.end() == it)
	{
		return;
	}

	if(uSecurityCode > 10000)
		return;

	COmdOrderbook& rOrderBook = it->second;
#ifndef NOT_MEASURE
	rOrderBook.m_PkgTime = _uPkgTm;
	rOrderBook.m_MsgTime = dbp::tools::srv::current();
#endif
	if (53 == _pMsg->m_uMsgType)
	{
		rOrderBook.m_MsgType = MsgType::OMDC_BOOK;
		//buildOmdcOrderBook(_pMsg, rOrderBook);
		unsigned char updatelvl = buildSlimOmdcOrderBook(_pMsg, rOrderBook);

		if(updatelvl <= 3){

			OrderItem m_Bid[3];
			OrderItem m_Ask[3];

			std::memcpy(m_Bid, rOrderBook.m_BidOrder, 3 * sizeof(OrderItem));
			std::memcpy(m_Ask, rOrderBook.m_AskOrder, 3 * sizeof(OrderItem));

			//auto best_bid_price = static_cast<unsigned long long>(rOrderBook.m_Bid[0].m_iPrice) * 100000;
			//auto best_bid_qty = static_cast<unsigned long long>(rOrderBook.m_Bid[0].m_uQuantity);

			auto best_ask_price_1 = static_cast<unsigned long long>(m_Ask[0].m_iPrice);
			auto best_ask_price_2 = static_cast<unsigned long long>(m_Ask[1].m_iPrice);
			auto best_ask_price_3 = static_cast<unsigned long long>(m_Ask[2].m_iPrice);
			auto best_ask_qty1 = static_cast<unsigned long long>(m_Ask[0].m_uQuantity);
			auto best_ask_qty2 = static_cast<unsigned long long>(m_Ask[1].m_uQuantity);
			auto best_ask_qty3 = static_cast<unsigned long long>(m_Ask[2].m_uQuantity);

			auto best_bid_price_1 = static_cast<unsigned long long>(m_Bid[0].m_iPrice);
			auto best_bid_price_2 = static_cast<unsigned long long>(m_Bid[1].m_iPrice);
			auto best_bid_price_3 = static_cast<unsigned long long>(m_Bid[2].m_iPrice);
			auto best_bid_qty1 = static_cast<unsigned long long>(m_Bid[0].m_uQuantity);
			auto best_bid_qty2 = static_cast<unsigned long long>(m_Bid[1].m_uQuantity);
			auto best_bid_qty3 = static_cast<unsigned long long>(m_Bid[2].m_uQuantity);



			bool bidcon = ((best_bid_price_1-best_bid_price_2) - (best_bid_price_2-best_bid_price_3)) == 0;
			bool askcon = ((best_ask_price_3-best_ask_price_2) - (best_ask_price_2-best_ask_price_1)) == 0;


			s1signal* s1s = s1SignalMap[uSecurityCode];

			unsigned long long mid = static_cast<unsigned long long>((best_ask_price_1 + best_bid_price_1)/2)*100000;
			unsigned long long wp = calWeightedPrice(
					best_bid_price_1,best_bid_price_2,best_bid_price_3,
					best_bid_qty1, best_bid_qty2, best_bid_qty3,
					best_ask_price_1,best_ask_price_2,best_ask_price_3,
					best_ask_qty1, best_ask_qty2, best_ask_qty3
					);
			wp = wp * 100000;

			auto best_ask_price1 = static_cast<unsigned long long>(m_Ask[0].m_iPrice) * 100000;
			auto best_bid_price1 = static_cast<unsigned long long>(m_Bid[0].m_iPrice) * 100000;

			if(!s1s->hasSignal){
				bool isThick = best_ask_qty1 > s1s->Thick;
				bool isThin1 = best_ask_qty2 < s1s->Thin;
				bool isThin2 = best_ask_qty3 < s1s->Thin;

				if(isThick&&isThin1&&isThin2){

					//flush_printf("Code = %u A1 = %llu A2 = %llu A3 = %llu \n", uSecurityCode, best_ask_qty1, best_ask_qty2, best_ask_qty3);


					s1s->DetectAsk = best_ask_price1;
					//s1s->m_SignalTime = dbp::tools::srv::current();
					//s1s->SignalType = 0;
					s1s->hasSignal = true;

					//flush_printf("tm:%llu, s1signal = True \n", dbp::tools::srv::current());
				}
			}else{
				if(best_ask_price1 != s1s->DetectAsk){
					s1s->hasSignal = false;
				}
				bool isThin1 = best_ask_qty2 < s1s->Thin;
				bool isThin2 = best_ask_qty3 < s1s->Thin;
				if(!isThin1 || !isThin2){
					s1s->hasSignal = false;
				}
			}
			s1s->MidPrice = mid;
			s1s->WeightedPrice = wp;
			s1s->Bidcon = bidcon;
			s1s->Askcon = askcon;


			s1signal* s1sp = s1SignalPutMap[uSecurityCode];

			if(!s1sp->hasSignal){
				bool isThick = best_bid_qty1 > s1sp->Thick;
				bool isThin1 = best_bid_qty2 < s1sp->Thin;
				bool isThin2 = best_bid_qty3 < s1sp->Thin;

				if(isThick&&isThin1&&isThin2){

					//flush_printf("Code = %u A1 = %llu A2 = %llu A3 = %llu \n", uSecurityCode, best_ask_qty1, best_ask_qty2, best_ask_qty3);


					s1sp->DetectBid = best_bid_price1;
					//s1s->m_SignalTime = dbp::tools::srv::current();
					//s1s->SignalType = 0;
					s1sp->hasSignal = true;

					//flush_printf("tm:%llu, s1signal = True \n", dbp::tools::srv::current());
				}
			}else{
				if(best_bid_price1 != s1sp->DetectBid){
					s1sp->hasSignal = false;
				}
				bool isThin1 = best_bid_qty2 < s1sp->Thin;
				bool isThin2 = best_bid_qty3 < s1sp->Thin;
				if(!isThin1 || !isThin2){
					s1sp->hasSignal = false;
				}
			}
			s1sp->MidPrice = mid;
			s1sp->WeightedPrice = wp;
			s1sp->Bidcon = bidcon;
			s1sp->Askcon = askcon;
		}

	}
}
#endif


