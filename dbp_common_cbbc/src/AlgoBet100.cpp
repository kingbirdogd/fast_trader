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

unsigned long long AlgoBet100::fixQuantity(unsigned long long price, unsigned long long quantity){
	unsigned long long betsize = quantity;
		if(price >= 400000000){
			betsize = 2500;
		}
		if(price >= 100000000 && price < 400000000){
			betsize = 10000;
		}
		if(price >= 50000000 && price < 100000000){
			betsize = 14000;
		}
		if(price >= 25000000 && price < 50000000){
			betsize = 20000;
		}
		if(price < 25000000){
			betsize = 100000;
		}

		betsize = static_cast<unsigned long long>((int)(betsize/quantity + 0.1))*quantity;

		if(betsize < quantity){
			betsize = quantity;
		}

		return betsize;
}

unsigned long long AlgoBet100::fixQuantityByIssuer(unsigned long long price,unsigned long long quantity, string ){
	unsigned long long betsize = quantity;
		if(price >= 400000000){
			betsize = 2500;
		}
		if(price >= 100000000 && price < 400000000){
			betsize = 10000;
		}
		if(price >= 50000000 && price < 100000000){
			betsize = 14000;
		}
		if(price >= 25000000 && price < 50000000){
			betsize = 20000;
		}
		if(price < 25000000){
			betsize = 100000;
		}

		betsize = static_cast<unsigned long long>((int)(betsize/quantity + 0.1))*quantity;

		if(betsize < quantity){
			betsize = quantity;
		}

		return betsize;
}

unsigned long long AlgoBet100::fixQuantityBySpread(unsigned long long price, unsigned long long quantity, unsigned long long ){
	/*
	int noofspread = static_cast<int>(spread / 100000ull);

	unsigned long long qty = static_cast<unsigned long long>(100000/noofspread);

	unsigned long long betsize = static_cast<unsigned long long>((int)(qty/quantity + 0.1))*quantity;

	if(betsize < quantity){
		betsize = quantity;
	}
	return betsize;
	*/
	unsigned long long betsize = quantity;
	if(price >= 400000000){
		betsize = 2500;
	}
	if(price >= 100000000 && price < 400000000){
		betsize = 10000;
	}
	if(price >= 50000000 && price < 100000000){
		betsize = 14000;
	}
	if(price >= 25000000 && price < 50000000){
		betsize = 20000;
	}
	if(price < 25000000){
		betsize = 100000;
	}

	betsize = static_cast<unsigned long long>((int)(betsize/quantity + 0.1))*quantity;

	if(betsize < quantity){
		betsize = quantity;
	}

	return betsize;
}


