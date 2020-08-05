/*
 * AlgoBetSmall.h
 *
 *  Created on: 2017¦~9¤ë4¤é
 *      Author: leo
 */

#ifndef ALGOBETX_H_
#define ALGOBETX_H_

#include "AlgoBet.h"
#include <unordered_map>


class AlgoBetX : public AlgoBet {
private:
	AlgoBet* currentBet;
	unordered_map<string, AlgoBet*> betMap;
public:
	AlgoBetX();
	virtual ~AlgoBetX();
	virtual string getBetName();
	virtual AlgoBet* getCurrentBet(string);
	virtual string selectBet(string);
	virtual unsigned long long fixQuantityBySpread(unsigned long long , unsigned long long, unsigned long long );
	virtual unsigned long long fixQuantity(unsigned long long price, unsigned long long quantity);
	virtual unsigned long long fixQuantityByIssuer(unsigned long long price,unsigned long long quantity, string issuer);
};



#endif /* ALGOBETX_H_ */
