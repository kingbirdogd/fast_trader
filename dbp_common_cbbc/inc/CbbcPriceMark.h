/*
 * SimplePriceMark.h
 *
 *  Created on: 2017�~5��22��
 *      Author: leo
 */

#ifndef CBBCPRICEMARK_H_
#define CBBCPRICEMARK_H_

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

#define INDEX_TYPE 1
#define STOCK_TYPE 2



class CbbcPriceMark {
private:
	//unsigned int porderbookid;
	//unsigned int pcode;
	int ppType;
	int pwtype;
	unsigned long long pIssuerSize;


	map<unsigned long long,unsigned long long> pAskMark;
	map<unsigned long long,unsigned long long> pBidMark;


	map<unsigned long long,unsigned long long> pUpAskMark;
	map<unsigned long long,unsigned long long> pDnBidMark;


	map<unsigned long long,unsigned long long> pDnAskMark;
	map<unsigned long long,unsigned long long> pUpBidMark;



	SpreadTable spreadTable;
	unsigned long long pDefaultSpread;
	unsigned long long pUSpread;

	unsigned long long bidkey = 0;
	unsigned long long askkey = 0;
	unsigned long long bidprice = 0;
	unsigned long long askprice = 0;

	unsigned long long sensitivity = 0;

public:
	CbbcPriceMark(int ptype, int wtype, unsigned long long spread, unsigned long long uspread);
	virtual ~CbbcPriceMark();
	vector<string> split(string str, char delimiter);



	bool updateTableBid(unsigned long long wbid, unsigned long long pwbid, unsigned long long fbid, unsigned long long pfbid);
	bool updateTableAsk(unsigned long long wask, unsigned long long pwask, unsigned long long fask, unsigned long long pfask);


	bool updateThBid(unsigned long long wbid, unsigned long long pwbid, unsigned long long fbid, unsigned long long pfbid);
	bool updateThAsk(unsigned long long wask, unsigned long long pwask, unsigned long long fask, unsigned long long pfask);

	bool updateBid(unsigned long long wbid, unsigned long long pwbid, unsigned long long fbid, unsigned long long pfbid);
	bool updateAsk(unsigned long long wask, unsigned long long pwask, unsigned long long fask, unsigned long long pfask);
	bool updateBid(unsigned long long wbid, unsigned long long pwbid, unsigned long long fbid, unsigned long long pfbid, bool ignoref);
	bool updateAsk(unsigned long long wask, unsigned long long pwask, unsigned long long fask, unsigned long long pfask, bool ignoref);

	unsigned long long buyIn(unsigned long long wprice);
	unsigned long long sellOut(unsigned long long wprice);
	unsigned long long leveldiff(unsigned long long buyprice);

	unsigned long long getSensitivity();

	unsigned long long dynameicBuyIn(unsigned long long wprice);
	unsigned long long dynameicSellOut(unsigned long long wprice);
	//unsigned long long dynameicLevel(unsigned long long wprice);

	void clearall();

	unsigned long long getIssuerIize();
	int getWType();
	string ftos(float value, int dp);


	map<unsigned long long,unsigned long long> getBidTable();
	map<unsigned long long,unsigned long long> getAskTable();
	map<unsigned long long,unsigned long long> getUpBidTable();
	map<unsigned long long,unsigned long long> getUpAskTable();
	map<unsigned long long,unsigned long long> getDnBidTable();
	map<unsigned long long,unsigned long long> getDnAskTable();

	unsigned long long getBidKey();
	unsigned long long getAskKey();
	unsigned long long getBidPrice();
	unsigned long long getAskPrice();

	void copyTable(map<unsigned long long,unsigned long long> bid,  map<unsigned long long,unsigned long long> ask);
	void copyUpTable(map<unsigned long long,unsigned long long> bid,  map<unsigned long long,unsigned long long> ask);
	void copyDnTable(map<unsigned long long,unsigned long long> bid,  map<unsigned long long,unsigned long long> ask);

	void setSensitivity(unsigned long long value);
};



#endif /* CBBCPRICEMARK_H_ */
