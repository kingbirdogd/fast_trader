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

	unsigned long long Bestbid;
	unsigned long long Bestask;
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
	unsigned long long BuyPrice;
	string BuyTime;
	unsigned long long SellPrice;
	string SoldTime;
	float StopLostPrice;
	string DetectTime;
	unsigned int UCode;
	float RefWBid;
	float RefWAsk;
	string SellMode;
	unsigned long long Quantity;
	unsigned long long BuyQty;
	unsigned long long SellQty;

	unsigned long long BuyIn;
	unsigned long long SellOut;


	string Issuer;
	string Strategy;

	float UBuyPrice;
	float USoldPrice;

	float UBid;
	float UAsk;

	int DBuyPrice;
	int DSellPrice;
	unsigned long long DBid;
	unsigned long long DAsk;

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
