/*
 * SelectedWarrant.cpp
 *
 *  Created on: 2017¦~5¤ë17¤é
 *      Author: leo
 */

#include <stdio.h>
#include "SelectedWarrant.h"


SelectedWarrant::SelectedWarrant() {
	// TODO Auto-generated constructor stub

}

SelectedWarrant::~SelectedWarrant() {
	// TODO Auto-generated destructor stub
}

bool SelectedWarrant::isAccept(float uspread, float delta, float cratio, float bidaskspread, float ticks){
	//if(bidaskspread <= 0)
	//		return false;
	float result = uspread * delta / cratio;
	//result = (float)((int)(result * 1000 + 0.1))/1000.0f;
	return (result * ticks > bidaskspread);
}



bool SelectedWarrant::isSpreadAccept(int noofspread, unsigned long long price){
	if(price >= 50000000 && price < 1000000000){
		if(noofspread>0 && noofspread <= 1){
			return true;
		}
	}
	if(price >= 25000000 && price < 50000000){
		if(noofspread>0 && noofspread <= 1){
			return true;
		}
	}
	if(price < 25000000){
		if(noofspread>0 && noofspread <= 3){
			return true;
		}
	}
	return false;
}

unsigned long long SelectedWarrant::estimateWarrantPrice(float uspread, float delta, float cratio){
	float result = uspread * delta / cratio;
	unsigned long long refprice = static_cast<unsigned long long>(result * 1000) * 100000;
	return refprice;
}



