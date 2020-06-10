/*
 * SpreadTable.cpp
 *
 *  Created on: 2017¦~5¤ë15¤é
 *      Author: leo
 */

#include "SpreadTable.h"



float spreadTable01[11][4] = {
		{0.01f,0.0f,0.25f,0.001f},
		{0.01f,0.25f,0.5f,0.005f},
		{0.01f,0.5f,10.0f,0.01f},
		{0.01f,10.0f,20.0f,0.02f},
		{0.01f,20.0f,100.0f,0.05f},
		{0.01f,100.0f,200.0f,0.1f},
		{0.01f,200.0f,500.0f,0.2f},
		{0.01f,500.0f,1000.0f,0.5f},
		{0.01f,1000.0f,2000.0f,1.0f},
		{0.01f,2000.0f,5000.0f,2.0f},
		{0.01f,5000.0f,9995.0f,5.0f}
};


SpreadTable::SpreadTable() {
	// TODO Auto-generated constructor stub


/*

*/
	//SpreadMap["01"] = spreadTable01;
	//SpreadMap["02"] = spreadTable02;
	//SpreadMap["03"] = spreadTable03;
}

float SpreadTable::getSpread(string spreadTableCode, float price) {

	if(spreadTableCode.compare("01") == 0){
		for(int i=0; i<11; i++){
			if(price > spreadTable01[i][1] && price <= spreadTable01[i][2]){
				//fprintf(stderr, "%4.5f %s Range %4.5f - %4.5f  Spread = %4.5f \n", price, spreadTableCode.c_str(), spreadTable01[i][1], spreadTable01[i][2], spreadTable01[i][3]);
				return spreadTable01[i][3];
			}
		}
	}
	if(spreadTableCode.compare("02") == 0){
		return 0.125f;
	}
	if(spreadTableCode.compare("03") == 0){
		return 0.05f;
	}
	//fprintf(stderr, "%4.5f %s Spread = 0 \n", price, spreadTableCode.c_str());
	return 0;
}


