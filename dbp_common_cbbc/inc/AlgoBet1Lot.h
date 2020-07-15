/*
 * AlgoBetSmall.h
 *
 *  Created on: 2017¦~9¤ë4¤é
 *      Author: leo
 */

#ifndef ALGOBET1LOT_H_
#define ALGOBET1LOT_H_

#include "AlgoBet.h"


class AlgoBet1Lot : public AlgoBet {
public:
	AlgoBet1Lot();
	virtual ~AlgoBet1Lot();
	virtual AlgoBet* getCurrentBet(string);
	virtual void selectBet(string);
	virtual unsigned long long fixQuantity(unsigned long long price, unsigned long long quantity);
	virtual unsigned long long fixQuantityByIssuer(unsigned long long price,unsigned long long quantity, string issuer);
};


#endif /* ALGOBET1LOT_H_ */
