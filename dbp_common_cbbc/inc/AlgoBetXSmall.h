/*
 * AlgoBetXSmall.h
 *
 *  Created on: 2017¦~9¤ë4¤é
 *      Author: leo
 */

#ifndef ALGOBETXSMALL_H_
#define ALGOBETXSMALL_H_

#include "AlgoBet.h"


class AlgoBetXSmall : public AlgoBet {
public:
	AlgoBetXSmall();
	virtual ~AlgoBetXSmall();
	virtual string getBetName();
	virtual AlgoBet* getCurrentBet(string);
	virtual string selectBet(string);
	virtual unsigned long long fixQuantityBySpread(unsigned long long , unsigned long long, unsigned long long );
	virtual unsigned long long fixQuantity(unsigned long long price, unsigned long long quantity);
	virtual unsigned long long fixQuantityByIssuer(unsigned long long price,unsigned long long quantity, string issuer);
};



#endif /* ALGOBETXSMALL_H_ */
