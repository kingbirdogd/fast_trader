/*
 * AlgoBetSmall.cpp
 *
 *  Created on: 2017¦~9¤ë4¤é
 *      Author: leo
 */

#include "AlgoBetSmall.h"



AlgoBetSmall::AlgoBetSmall() {
	// TODO Auto-generated constructor stub

}

AlgoBetSmall::~AlgoBetSmall() {
	// TODO Auto-generated destructor stub
}

AlgoBet* AlgoBetSmall::getCurrentBet(string){
	return this;
}

void AlgoBetSmall::selectBet(string){

}

unsigned long long AlgoBetSmall::fixQuantity(unsigned long long price, unsigned long long quantity){
	if(price >= 400000000){
		return 1500;
	}
	if(price >= 100000000 && price < 400000000){
		return 6000;
	}
	if(price >= 50000000 && price < 100000000){
		return 8000;
	}
	if(price >= 25000000 && price < 50000000){
		return 16000;
	}
	if(price < 25000000){
		return 60000;
	}
	return quantity;
}

unsigned long long AlgoBetSmall::fixQuantityByIssuer(unsigned long long price,unsigned long long quantity, string ){
	if(price >= 400000000){
			return 1500;
	}
	if(price >= 100000000 && price < 400000000){
		return 6000;
	}
	if(price >= 50000000 && price < 100000000){
		return 8000;
	}
	if(price >= 25000000 && price < 50000000){
		return 16000;
	}
	if(price < 25000000){
		return 60000;
	}
	return quantity;
}


