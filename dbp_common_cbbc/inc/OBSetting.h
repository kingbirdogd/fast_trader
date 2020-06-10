/*
 * OBSetting.h
 *
 *  Created on: 2017¦~5¤ë11¤é
 *      Author: leo
 */



#ifndef OBSETTING_H_
#define OBSETTING_H_

#include <stdio.h>
#include <string>
#include <vector>
#include <unordered_map>

#include "AlgoEngineData.h"

using namespace std;



class OBSetting {



public:
	string algoid;
	int Parent;
	unsigned int SeqNo;
	unsigned long long OrderId;
	std::string Name;
	string Date;
	unsigned int Code;
	bool detected = false;
	bool hasPosition = false;
	float BuyPrice;
	string BuyTime;
	float SellPrice;
	string SoldTime;
	float StopLostPrice;
	int StopLostTick;
	string DetectTime;
	float DetectedAsk;
	float DetectedBid;
	int Status;
	float UBid;
	float UAsk;
	bool forceSold = false;
	unsigned int Quantity;

	unsigned int Orderbookid;
	int DetectedDBid;
	int DetectedDAsk;
	int wtype;

	unsigned long long IssuerSize;

	unsigned long long BuyIn = -1;
	unsigned long long SellOut = -1;
	unsigned long long LvLBid = -1;
	unsigned long long RefWBid = -1;
	unsigned long long RefWAsk = -1;

	unsigned long AccumTrade = 0;
	unsigned long ACount = 0;
	unsigned long BCount = 0;
	unsigned long VATotal = 0;
	unsigned long VBTotal = 0;

	int BuyInSellOutRange = 10;
	int WinTicks = 0;
	int InLvlRange = 2;
	int BidAskSpread = 1;
	int BuyTriggerQty = 0;
	int SellTriggerQty = 0;

	time_t TradeTime;

	int ActionStatus;

	OBSetting();
	virtual ~OBSetting();

	unordered_map<unsigned int, warrant*> WarrantMap;

	void addWarrantOrCbbc(warrant* wnrt);
	bool isExist(unsigned int code);
	warrant* removeWarrantOrCbbc(unsigned int);
	warrant* getRelatedWarrant(unsigned int);
	int getRelatedWarrantCount();
	float getHighestStopLostPrice();
	float getLowestStopLostPrice();
	bool hasRelatedWarrant(int);
	vector<warrant*> getRelatedWarrant();
	void setRelatedWarrantStatus(unsigned int, int);
	bool warrantStatus(unsigned int, int);
	int getWarrantStatus(unsigned int);
	bool hasWarrants();
	bool allStatus(int );
	void printAllWarrant();
	void removeAllWarrants();
};

/* namespace easywsclient */

#endif /* OBSETTING_H_ */
