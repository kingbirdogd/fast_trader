/*
 * AlgoBetSmall.h
 *
 *  Created on: 2017¦~9¤ë4¤é
 *      Author: leo
 */

#ifndef ALGOBETSMALL_H_
#define ALGOBETSMALL_H_

#include "AlgoBet.h"


class AlgoBetSmall : public AlgoBet {
public:
	AlgoBetSmall();
	virtual ~AlgoBetSmall();
	virtual AlgoBet* getCurrentBet(string);
	virtual void selectBet(string);
	virtual unsigned long long fixQuantity(unsigned long long price, unsigned long long quantity);
	virtual unsigned long long fixQuantityByIssuer(unsigned long long price,unsigned long long quantity, string issuer);
};


#endif /* ALGOBETSMALL_H_ */
