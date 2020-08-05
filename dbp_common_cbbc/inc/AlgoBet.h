/*
 * AlgoBet.h
 *
 *  Created on: 2017¦~9¤ë4¤é
 *      Author: leo
 */

#ifndef ALGOBET_H_
#define ALGOBET_H_

#include <string>
#include <string.h>
#include <iostream>

using namespace std;


class AlgoBet{
	public:
		virtual string getBetName(){return "";}
		virtual AlgoBet* getCurrentBet(string){return nullptr;}
		virtual string selectBet(string){}
		virtual unsigned long long fixQuantityBySpread(unsigned long long , unsigned long long, unsigned long long ){return 0;}
		virtual unsigned long long fixQuantity(unsigned long long , unsigned long long ){return 0;}
		virtual unsigned long long fixQuantityByIssuer(unsigned long long ,unsigned long long , string ){return 0;}
		unsigned long long scaleprice(float value)
		{
			unsigned long long svalue = static_cast<unsigned long long>(value*100000000ull);
			return svalue;
		}
};


#endif /* ALGOBET_H_ */
