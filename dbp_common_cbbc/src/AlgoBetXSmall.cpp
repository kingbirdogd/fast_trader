/*
 * AlgoBetSmall.cpp
 *
 *  Created on: 2017¦~9¤ë4¤é
 *      Author: leo
 */

#include "AlgoBetXSmall.h"


AlgoBetXSmall::AlgoBetXSmall() {
	// TODO Auto-generated constructor stub

}

AlgoBetXSmall::~AlgoBetXSmall() {
	// TODO Auto-generated destructor stub
}

AlgoBet* AlgoBetXSmall::getCurrentBet(string){

	return this;
}

string AlgoBetXSmall::getBetName(){
	return "BetXSmall";
}

string AlgoBetXSmall::selectBet(string){
	return "BetXSmall";
}

unsigned int AlgoBetXSmall::fixQuantity(unsigned long long price,unsigned long long quantity){
	if(price >= 400000000){
			return 500;
	}
	if(price >= 100000000 && price < 400000000){
		return 1000;
	}
	if(price >= 50000000 && price < 100000000){
		return 2000;
	}
	if(price >= 25000000 && price < 50000000){
		return 8000;
	}
	if(price < 25000000){
		return 20000;
	}
	return quantity;
}

unsigned int AlgoBetXSmall::fixQuantityByIssuer(unsigned long long price,unsigned long long quantity, string ){
	if(price >= 400000000){
			return 500;
	}
	if(price >= 100000000 && price < 400000000){
		return 1000;
	}
	if(price >= 50000000 && price < 100000000){
		return 2000;
	}
	if(price >= 25000000 && price < 50000000){
		return 8000;
	}
	if(price < 25000000){
		return 20000;
	}
	return quantity;
}

unsigned long long AlgoBetXSmall::fixQuantityBySpread(unsigned long long price, unsigned long long quantity, unsigned long long ){

	unsigned long long betsize = quantity;
	if(price >= 400000000){
		betsize = 500;
	}
	if(price >= 100000000 && price < 400000000){
		betsize = 1000;
	}
	if(price >= 50000000 && price < 100000000){
		betsize = 2000;
	}
	if(price >= 25000000 && price < 50000000){
		betsize = 8000;
	}
	if(price < 25000000){
		betsize = 20000;
	}

	betsize = static_cast<unsigned long long>((int)(betsize/quantity + 0.1))*quantity;

	if(betsize < quantity){
		betsize = quantity;
	}

	return betsize;
}
