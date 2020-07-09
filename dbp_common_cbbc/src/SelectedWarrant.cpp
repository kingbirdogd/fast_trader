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



