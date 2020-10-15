#ifndef __SMFH_SRV_ORDERBOOK__
#define __SMFH_SRV_ORDERBOOK__
#include <omd.h>
#include "smfh_srv_cfg.h"
#ifdef FULLTICK
template <typename FullBook, typename Book>
inline static void ConvertFullBookToBookBid(FullBook& fullBook, Book& rOrderBook)
{
	std::memset(rOrderBook.m_Bid, 0, TRADABLE_BOOK_SIZE * sizeof(OrderItem));
	unsigned long long i = 0;
	for (auto it = fullBook.begin(); it != fullBook.end(); ++it)
	{
		rOrderBook.m_Bid[i].m_iPrice = it->first;
		rOrderBook.m_Bid[i].m_uQuantity = it->second.quantity;
		rOrderBook.m_Bid[i].m_uNumberOfOrder = it->second.number_of_order;
		++i;
		if (i == TRADABLE_BOOK_SIZE)
		{
			break;
		}
	}
	rOrderBook.m_AccumulateBuyQuantity = 0;
	rOrderBook.m_AccumulateSellQuantity = 0;
	rOrderBook.m_AccumulateBlankQuantity = 0;
}

template <typename FullBook, typename Book>
inline static void ConvertFullBookToBookAsk(FullBook& fullBook, Book& rOrderBook)
{
	std::memset(rOrderBook.m_Ask, 0, TRADABLE_BOOK_SIZE * sizeof(OrderItem));
	unsigned long long i = 0;
	for (auto it = fullBook.begin(); it != fullBook.end(); ++it)
	{
		rOrderBook.m_Ask[i].m_iPrice = it->first;
		rOrderBook.m_Ask[i].m_uQuantity = it->second.quantity;
		rOrderBook.m_Ask[i].m_uNumberOfOrder = it->second.number_of_order;
		++i;
		if (i == TRADABLE_BOOK_SIZE)
		{
			break;
		}
	}
	rOrderBook.m_AccumulateBuyQuantity = 0;
	rOrderBook.m_AccumulateSellQuantity = 0;
	rOrderBook.m_AccumulateBlankQuantity = 0;
}

#endif //FULLTICK
inline static void buildOmdcOrderBook(dbp::omd::COmdMsgHeader* _pMsg, COmdOrderbook& rOrderBook)
{
	unsigned char uNoEntries = OMD_GET_VALUE(_pMsg, 11, unsigned char);
	char* pszMsgPointer = (char*)_pMsg + 12;
	OrderItem* pOrderArray = 0;

	for (unsigned char i = 0; i < uNoEntries; ++i)
	{
		unsigned long long uAggregateQuantity = OMD_GET_VALUE(pszMsgPointer, 0, unsigned long long);
		int iPrice = OMD_GET_VALUE(pszMsgPointer, 8, int);
		unsigned int uNumberOfOrders = OMD_GET_VALUE(pszMsgPointer, 12, unsigned int);
		unsigned short int uSide = OMD_GET_VALUE(pszMsgPointer, 16, unsigned short int);
		unsigned char uPriceLevel = (unsigned int)OMD_GET_VALUE(pszMsgPointer, 18, unsigned char);
		unsigned char uUpdateAction = (unsigned int)OMD_GET_VALUE(pszMsgPointer, 19, unsigned char);
		if (0 == uSide)
		{
			pOrderArray = &(rOrderBook.m_BidOrder[0]);
		}
		else
		{
			pOrderArray = &(rOrderBook.m_AskOrder[0]);
		}
		if (74 == uUpdateAction)
		{
			memset(static_cast<void*>(pOrderArray), 0, sizeof(OrderItem) * 10);
		}
		else if (1 == uUpdateAction)
		{
			pOrderArray[uPriceLevel - 1].m_iPrice = iPrice;
			pOrderArray[uPriceLevel - 1].m_uQuantity = uAggregateQuantity;
			pOrderArray[uPriceLevel - 1].m_uNumberOfOrder = uNumberOfOrders;
		}
		else if (2 == uUpdateAction)
		{
			memmove(static_cast<void*>(pOrderArray + uPriceLevel - 1), static_cast<void*>(pOrderArray + uPriceLevel), sizeof(OrderItem) * (10 - uPriceLevel));
			pOrderArray[9].m_iPrice = 0;
			pOrderArray[9].m_uQuantity = 0;
			pOrderArray[9].m_uNumberOfOrder = 0;
		}
		else if (0 == uUpdateAction)
		{
			memmove(static_cast<void*>(pOrderArray + uPriceLevel), static_cast<void*>(pOrderArray + uPriceLevel - 1), sizeof(OrderItem) * (10 - uPriceLevel));
			pOrderArray[uPriceLevel - 1].m_iPrice = iPrice;
			pOrderArray[uPriceLevel - 1].m_uQuantity = uAggregateQuantity;
			pOrderArray[uPriceLevel - 1].m_uNumberOfOrder = uNumberOfOrders;
		}
		pszMsgPointer += 24;
	}
}


inline static unsigned char buildSlimOmdcOrderBook(dbp::omd::COmdMsgHeader* _pMsg, COmdOrderbook& rOrderBook)
{
	unsigned char uNoEntries = OMD_GET_VALUE(_pMsg, 11, unsigned char);
	char* pszMsgPointer = (char*)_pMsg + 12;
	OrderItem* pOrderArray = 0;
	unsigned char smallestlevel = 10;
	for (unsigned char i = 0; i < uNoEntries; ++i)
	{
		unsigned long long uAggregateQuantity = OMD_GET_VALUE(pszMsgPointer, 0, unsigned long long);
		int iPrice = OMD_GET_VALUE(pszMsgPointer, 8, int);
		unsigned int uNumberOfOrders = OMD_GET_VALUE(pszMsgPointer, 12, unsigned int);
		unsigned short int uSide = OMD_GET_VALUE(pszMsgPointer, 16, unsigned short int);
		unsigned char uPriceLevel = (unsigned int)OMD_GET_VALUE(pszMsgPointer, 18, unsigned char);
		unsigned char uUpdateAction = (unsigned int)OMD_GET_VALUE(pszMsgPointer, 19, unsigned char);
		if (0 == uSide)
		{
			pOrderArray = &(rOrderBook.m_BidOrder[0]);
		}
		else
		{
			pOrderArray = &(rOrderBook.m_AskOrder[0]);
		}
		if (74 == uUpdateAction)
		{
			memset(static_cast<void*>(pOrderArray), 0, sizeof(OrderItem) * 10);
		}
		else if (1 == uUpdateAction)
		{
			pOrderArray[uPriceLevel - 1].m_iPrice = iPrice;
			pOrderArray[uPriceLevel - 1].m_uQuantity = uAggregateQuantity;
			pOrderArray[uPriceLevel - 1].m_uNumberOfOrder = uNumberOfOrders;

			if(uPriceLevel < smallestlevel){
				smallestlevel = uPriceLevel;
			}

		}
		else if (2 == uUpdateAction)
		{
			memmove(static_cast<void*>(pOrderArray + uPriceLevel - 1), static_cast<void*>(pOrderArray + uPriceLevel), sizeof(OrderItem) * (10 - uPriceLevel));
			pOrderArray[9].m_iPrice = 0;
			pOrderArray[9].m_uQuantity = 0;
			pOrderArray[9].m_uNumberOfOrder = 0;

			if(uPriceLevel < smallestlevel){
				smallestlevel = uPriceLevel;
			}
		}
		else if (0 == uUpdateAction)
		{
			memmove(static_cast<void*>(pOrderArray + uPriceLevel), static_cast<void*>(pOrderArray + uPriceLevel - 1), sizeof(OrderItem) * (10 - uPriceLevel));
			pOrderArray[uPriceLevel - 1].m_iPrice = iPrice;
			pOrderArray[uPriceLevel - 1].m_uQuantity = uAggregateQuantity;
			pOrderArray[uPriceLevel - 1].m_uNumberOfOrder = uNumberOfOrders;

			if(uPriceLevel < smallestlevel){
				smallestlevel = uPriceLevel;
			}

		}
		pszMsgPointer += 24;
	}
	return smallestlevel;
}


inline static void buildOmddOrderBook(dbp::omd::COmdMsgHeader* _pMsg, COmdOrderbook& rOrderBook)
{
	unsigned char uNoEntries = OMD_GET_VALUE(_pMsg, 11, unsigned char);
	char* pszMsgPointer = (char*)_pMsg + 12;
	OrderItem* pOrderArray = 0;
	for (unsigned char i = 0; i < uNoEntries; ++i)
	{
		unsigned long long uAggregateQuantity = OMD_GET_VALUE(pszMsgPointer, 0, unsigned long long);
		int iPrice = OMD_GET_VALUE(pszMsgPointer, 8, int);
		int uNumberOfOrders = OMD_GET_VALUE(pszMsgPointer, 12, int);
		unsigned char uBidAsk = OMD_GET_VALUE(pszMsgPointer, 16, unsigned char);
		unsigned char uPriceLevel = OMD_GET_VALUE(pszMsgPointer, 18, unsigned char);
		unsigned char uAction = OMD_GET_VALUE(pszMsgPointer, 19, unsigned char);
		if (255 == uPriceLevel)
		{
			uPriceLevel = 11;
		}
		if (0 == uBidAsk)
		{
			pOrderArray = &(rOrderBook.m_BidOrder[0]);
		}
		else
		{
			pOrderArray = &(rOrderBook.m_AskOrder[0]);
		}
		if (74 == uAction)
		{
			memset(static_cast<void*>(pOrderArray), 0, sizeof(OrderItem) * 11);
		}
		else if (1 == uAction)
		{
			pOrderArray[uPriceLevel - 1].m_iPrice = iPrice;
			pOrderArray[uPriceLevel - 1].m_uQuantity = uAggregateQuantity;
			pOrderArray[uPriceLevel - 1].m_uNumberOfOrder = uNumberOfOrders;
		}
		else if (2 == uAction)
		{
			if (11 != uPriceLevel)
			{
				memmove(static_cast<void*>(pOrderArray + uPriceLevel - 1), static_cast<void*>(pOrderArray + uPriceLevel), sizeof(OrderItem) * (10 - uPriceLevel));
				pOrderArray[9].m_iPrice = 0;
				pOrderArray[9].m_uQuantity = 0;
				pOrderArray[9].m_uNumberOfOrder = 0;
			}
			else
			{
				pOrderArray[10].m_iPrice = 0;
				pOrderArray[10].m_uQuantity = 0;
				pOrderArray[10].m_uNumberOfOrder = 0;
			}
		}
		else if (0 == uAction)
		{
			if (11 != uPriceLevel)
			{
				memmove(static_cast<void*>(pOrderArray + uPriceLevel), static_cast<void*>(pOrderArray + uPriceLevel - 1), sizeof(OrderItem)* (10 - uPriceLevel));
			}
			pOrderArray[uPriceLevel - 1].m_iPrice = iPrice;
			pOrderArray[uPriceLevel - 1].m_uQuantity = uAggregateQuantity;
			pOrderArray[uPriceLevel - 1].m_uNumberOfOrder = uNumberOfOrders;
		}
		pszMsgPointer += 24;
	}
}
#endif


