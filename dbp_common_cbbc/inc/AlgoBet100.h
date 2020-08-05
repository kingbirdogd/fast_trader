/*
 * AlgoBetSmall.h
 *
 *  Created on: 2017¦~9¤ë4¤é
 *      Author: leo
 */

#ifndef ALGOBET100_H_
#define ALGOBET100_H_

#include "AlgoBet.h"


class AlgoBet100 : public AlgoBet {
public:
	AlgoBet100();
	virtual ~AlgoBet100();
	virtual string getBetName();
	virtual AlgoBet* getCurrentBet(string);
	virtual string selectBet(string);
	virtual unsigned long long fixQuantityBySpread(unsigned long long , unsigned long long, unsigned long long );
	virtual unsigned long long fixQuantity(unsigned long long price, unsigned long long quantity);
	virtual unsigned long long fixQuantityByIssuer(unsigned long long price,unsigned long long quantity, string issuer);
};


#endif /* ALGOBET100_H_ */
