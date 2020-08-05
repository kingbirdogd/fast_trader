/*
 * AlgoBet100.cpp
 *
 *  Created on: 2017¦~9¤ë4¤é
 *      Author: leo
 */

#include "AlgoBet100.h"



AlgoBet100::AlgoBet100() {
	// TODO Auto-generated constructor stub

}

AlgoBet100::~AlgoBet100() {
	// TODO Auto-generated destructor stub
}

string AlgoBet100::getBetName(){
	return "Bet100";
}

AlgoBet* AlgoBet100::getCurrentBet(string){
	return this;
}

string AlgoBet100::selectBet(string){
	return "Bet100";
}

unsigned long long AlgoBet100::fixQuantity(unsigned long long, unsigned long long quantity){
	return quantity;
}

unsigned long long AlgoBet100::fixQuantityByIssuer(unsigned long long ,unsigned long long quantity, string ){
	return quantity;
}

unsigned long long AlgoBet100::fixQuantityBySpread(unsigned long long, unsigned long long quantity, unsigned long long spread){

	int noofspread = static_cast<int>(spread / 100000ull);

	unsigned long long qty = static_cast<unsigned long long>(100000/noofspread);

	unsigned long long betsize = static_cast<unsigned long long>((int)(qty/quantity + 0.1))*quantity;

	if(betsize < quantity){
		betsize = quantity;
	}
	return betsize;
}


