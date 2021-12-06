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
#define STAGE_NEW -1

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

#define HSI_SYMBOL 1
#define NQ_SYMBOL 2
#define ST_SYMBOL 3



inline static unsigned long long calWeightedPrice(unsigned long long bid1, unsigned long long bid2, unsigned long long bid3,
		unsigned long long bidqty1, unsigned long long bidqty2, unsigned long long bidqty3,
		unsigned long long ask1, unsigned long long ask2, unsigned long long ask3,
		unsigned long long askqty1, unsigned long long askqty2, unsigned long long askqty3){
	unsigned long long result = (bid1*askqty1 + bid2*askqty2 + bid3*askqty3 + ask1*bidqty1 + ask2*bidqty2 + ask3*bidqty3 + 1)/
			(bidqty1+bidqty2+bidqty3+askqty1+askqty2+askqty3+1);
	return result;
}


using namespace std;


struct strategy1{

	bool bullbuy;
	bool bullsell;
	bool bearbuy;
	bool bearsell;

	unsigned long long bullr1s;
	unsigned long long bullr1e;
	unsigned long long bullr2s;
	unsigned long long bullr2e;
	unsigned long long bullr3s;
	unsigned long long bullr3e;

	unsigned long long bearr1s;
	unsigned long long bearr1e;
	unsigned long long bearr2s;
	unsigned long long bearr2e;
	unsigned long long bearr3s;
	unsigned long long bearr3e;

	bool isReady;
	bool isEnable;
};

struct priceinfo{

	unsigned int Code;
	unsigned int UCode;
	std::string Issuer;
	unsigned long long Lotsize;

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

	unsigned long long IBestbid;
	unsigned long long IBestask;
	unsigned long long PIBestbid;
	unsigned long long PIBestask;

	unsigned long long TBestask;
	unsigned long long TBestbid;
	unsigned long long PTBestbid;
	unsigned long long PTBestask;


	unsigned long long LFBestbid;
	unsigned long long LFBestask;

	unsigned long long LBestbid;
	unsigned long long LBestask;

	long long DiffBid;
	//unsigned long long BidSeq;
	//unsigned long long LastBidSeq;

	int BuyIn;
	int SellOut;

	int BidChange;
	int AskChange;

	long BidSeq;
	long AskSeq;
	long TBidSeq;
	long TAskSeq;
	long LastBidSeq;
	long LastAskSeq;
	long LastTBidSeq;
	long LastTAskSeq;

	unsigned long long LastTradePrice;
	int LastTradeSide;
	unsigned long long LastTradeQty;

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

	unsigned long long DetectedAsk;

#ifndef NOT_MEASURE
	unsigned long long pkg_tm;
	unsigned long long m_tm;
	unsigned long long t_tm;
	unsigned long long o_tm;
#endif

	time_t o_buytime;
	time_t o_leveltime;
	time_t o_soldtime;
	time_t o_wintime;


	int wincount;
	int lvlcount;

	unsigned long long o_lvlfuture;
	unsigned long long o_winfuture;


	//string StockName;
	string Issuer;
	//string Wtype;
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
	bool isLvl;
	bool isPause;

	int NoOfTrade;
};

struct aorder{
	int orderid;
	unsigned int warrant_code;
	unsigned int ucode;
	string side;
	string wtype;
	string issuer;
	string wname;
	string uname;
	unsigned long long buyin;
	unsigned long long sellout;
	unsigned long long lvlbid;
	unsigned long long order_price;
	unsigned long long order_quantity;
	unsigned long long filled_price;
	unsigned long long filled_quantity;
	string status;
	string transaction_time;
	int leveltime;
	int wintime;
	int lvlcount;
	int wincount;
	unsigned long long lvlprice;
	unsigned long long winprice;
	string reason;
};

struct portfolio{
	int id;
	unsigned long long code;
	string issuer;
	string wtype;
	unsigned long long avgBuy;
	unsigned long long avgSell;
	unsigned long long totalQty;
	long win;
	long loss;
	long draw;
	long execution;
	int win1;
	int win2;
	int win3;
	int lost1;
	int lost2;
	int lost3;
	string lasttradetime;
	unsigned long long buyturnover;
	unsigned long long sellturnover;
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
