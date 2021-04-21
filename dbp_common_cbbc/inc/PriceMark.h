/*
 * PriceMark.h
 *
 *  Created on: 2017�~5��22��
 *      Author: leo
 */

#ifndef PRICEMARK_H_
#define PRICEMARK_H_

#include <map>
#include <string>
#include <stdio.h>
#include <regex>
#include <vector>
#include <sstream>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iomanip>
#include <algorithm>
#include "SpreadTable.h"

using namespace std;

#define BID_SIDE 0
#define ASK_SIDE 1
#define BULL 1
#define BEAR 2

#define PRICE_CHANGE 1
#define VOL_CHANGE 2

#define INDEX_SPREAD 100000

class PriceMark {
private:
	//unsigned int porderbookid;
	unsigned int pcode;
	int pwtype;
	unsigned long long pIssuerSize;
	map<string,string> priceMarkTable;
	map<unsigned long long,unsigned long long> pAskMark;
	map<unsigned long long,unsigned long long> pBidMark;
	SpreadTable spreadTable;

	unsigned long long bidkey = 0;
	unsigned long long askkey = 0;
	unsigned long long bidprice = 0;
	unsigned long long askprice = 0;
	unsigned long long BidIssuerQty;
	unsigned long long AskIssuerQty;
public:
	PriceMark(unsigned int code, int wtype);
	virtual ~PriceMark();
	vector<string> split(string str, char delimiter);

	bool updateBid(unsigned long long wbid, unsigned long long pwbid, unsigned long long fbid, unsigned long long pfbid);
	bool updateAsk(unsigned long long wask, unsigned long long pwask, unsigned long long fask, unsigned long long pfask);
	bool updateIBid(unsigned long long wbid, unsigned long long pwbid, unsigned long long fbid, unsigned long long pfbid);
	bool updateIAsk(unsigned long long wask, unsigned long long pwask, unsigned long long fask, unsigned long long pfask);

	string updateTableBid(string key, unsigned long long bidprice);
	string updateTableAsk(string key, unsigned long long askprice);

	unsigned long long buyIn(unsigned long long wprice);
	unsigned long long sellOut(unsigned long long wprice);

	void setSellout(unsigned long long wprice, unsigned long long uprice);

	void clearall();

	unsigned long long getIssuerIize();
	int getWType();
	string ftos(float value, int dp);

	map<unsigned long long,unsigned long long> getBidTable();
	map<unsigned long long,unsigned long long> getAskTable();

	unsigned long long getWarrantBidAskSpread(string key);
	unsigned long long getRefWarrantBid(string key);

	unsigned long long getBidKey();
	unsigned long long getAskKey();
	unsigned long long getBidPrice();
	unsigned long long getAskPrice();

	unsigned long long getIssuerBidQty();
	unsigned long long getIssuerAskQty();
	void setBidIssuerQty(unsigned long long qty);
	void setAskIssuerQty(unsigned long long qty);
	string printTable(unsigned long long ubid);
	string printTable();
	string printIndexTable();
	string printTableJson();
	string printIndexTableJson();

	unsigned long long getMaxBidAskSpread();
};



#endif /* CBBCPRICEMARK_H_ */
