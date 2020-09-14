/*
 * AlgoEngine.h
 *
 *  Created on: 2017¦~5¤ë16¤é
 *      Author: leo
 */

#ifndef ALGOENGINE_H_
#define ALGOENGINE_H_

#include <string>

#define STAGE_START 1
#define STAGE_STOP 0
#define STAGE_PAUSE 2

#define CHANGE_UP 1
#define CHANGE_DN 0

#define STATUS_NEW -1
#define STATUS_READY 1
#define STATUS_PENDING 2
#define STATUS_AVAILABLE 5
#define STATUS_SELLING 6
#define STATUS_SOLD 7
#define STATUS_ERROR -11
#define STATUS_REJECTED -10
#define STATUS_DONE 8
#define STATUS_TRADE_END 100

#define BUY_ORDER 1
#define SELL_ORDER -1

#define BULL 1
#define BEAR 2

using namespace std;

struct priceinfo{

	unsigned int UCode;

	unsigned long long Bestbid;
	unsigned long long Bestask;
	unsigned long long BidQty;
	unsigned long long AskQty;
	unsigned long long PBestbid;
	unsigned long long PBestask;

	unsigned long long FBestask;
	unsigned long long FBestbid;
	unsigned long long PFBestbid;
	unsigned long long PFBestask;


	unsigned long long TBestask;
	unsigned long long TBestbid;
	unsigned long long PTBestbid;
	unsigned long long PTBestask;


	unsigned long long LFBestbid;
	unsigned long long LFBestask;

	unsigned long long LBestbid;
	unsigned long long LBestask;

	int BuyIn;
	int SellOut;

	int BidChange;
	int AskChange;

	long BidSeq;
	long AskSeq;
	long LastBidSeq;
	long LastAskSeq;
};

struct warrant{
	int Status;
	int Parent;
	int SeqNo;
	unsigned long long OrderId;
	string Name;
	string Date;
	unsigned int Code;
	bool hasPosition;
	float Egearing;
	unsigned long long BuyQuantity;
	unsigned long long BuyPrice;
	string BuyTime;
	unsigned long long SellPrice;
	string SoldTime;
	unsigned long long StopLostPrice;
	string DetectTime;
	unsigned int UCode;
	unsigned long long RefWBid;
	unsigned long long RefWAsk;
	string SellMode;
	unsigned long long Quantity;
	unsigned long long BuyQty;
	unsigned long long SellQty;

	unsigned long long BuyIn;
	unsigned long long SellOut;
	unsigned long long LvlBid;

#ifndef NOT_MEASURE
	unsigned long long pkg_tm;
	unsigned long long m_tm;
	unsigned long long t_tm;
	unsigned long long o_tm;
#endif

	string Issuer;
	string Strategy;

	unsigned long long UBuyPrice;
	unsigned long long USoldPrice;

	unsigned long long UBid;
	unsigned long long UAsk;

	int DBuyPrice;
	int DSellPrice;
	unsigned long long DBid;
	unsigned long long DAsk;

	bool isWinSell;
	bool isWinOrLvlSell;

	int NoOfTrade;
};

struct product{
	string algoid;
	unsigned int code;
	unsigned long long issuerSize;
	unsigned long long quantity;
	string symbol;
	int status;
};

#endif /* ALGOENGINE_H_ */
