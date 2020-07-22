/*
 * SpreadTable.cpp
 *
 *  Created on: 2017¦~5¤ë15¤é
 *      Author: leo
 */

#include "SpreadTable.h"

const unsigned long long spreadTable01[11][3] =
{
	{1000000ull,25000000ull,100000ull},
	{25000000ull,50000000ull,500000ull},
	{50000000ull,1000000000ull,1000000ull},
	{1000000000ull,2000000000ull,2000000ull},
	{2000000000ull,10000000000ull,5000000ull},
	{10000000000ull,20000000000ull,10000000ull},
	{20000000000ull,50000000000ull,20000000ull},
	{50000000000ull,100000000000ull,50000000ull},
	{100000000000ull,200000000000ull,100000000ull},
	{200000000000ull,500000000000ull,200000000ull},
	{500000000000ull,999500000000ull,500000000ull}
};
const unsigned long long spreadTable05[10][3] =
{
	{1000000ull,100000000ull,100000ull},
	{100000000ull,500000000ull,200000ull},
	{500000000ull,1000000000ull,500000ull},
	{1000000000ull,2000000000ull,1000000ull},
	{2000000000ull,10000000000ull,2000000ull},
	{10000000000ull,20000000000ull,5000000ull},
	{20000000000ull,50000000000ull,10000000ull},
	{50000000000ull,100000000000ull,20000000ull},
	{100000000000ull,200000000000ull,50000000ull},
	{200000000000ull,999900000000ull,100000000ull}
};

SpreadTable::SpreadTable() {
	// TODO Auto-generated constructor stub


/*

*/
	//SpreadMap["01"] = spreadTable01;
	//SpreadMap["02"] = spreadTable02;
	//SpreadMap["03"] = spreadTable03;
}

unsigned long long SpreadTable::getSpread(string spreadTableCode, unsigned long long price) {

	if(spreadTableCode.compare("01") == 0 || spreadTableCode.compare("04") == 0){
		for(int i=0; i<11; i++){
			if(price > spreadTable01[i][0] && price <= spreadTable01[i][1]){
				//fprintf(stderr, "%4.5f %s Range %4.5f - %4.5f  Spread = %4.5f \n", price, spreadTableCode.c_str(), spreadTable01[i][1], spreadTable01[i][2], spreadTable01[i][3]);
				return spreadTable01[i][2];
			}
		}
	}
	if(spreadTableCode.compare("02") == 0){
		return 12500000ull;
	}
	if(spreadTableCode.compare("03") == 0){
		return 5000000ull;
	}
	if(spreadTableCode.compare("05") == 0){
		for(int i=0; i<10; i++){
			if(price > spreadTable05[i][0] && price <= spreadTable05[i][1]){
				//fprintf(stderr, "%4.5f %s Range %4.5f - %4.5f  Spread = %4.5f \n", price, spreadTableCode.c_str(), spreadTable01[i][1], spreadTable01[i][2], spreadTable01[i][3]);
				return spreadTable05[i][2];
			}
		}
	}
	//fprintf(stderr, "%4.5f %s Spread = 0 \n", price, spreadTableCode.c_str());
	return 0;
}


