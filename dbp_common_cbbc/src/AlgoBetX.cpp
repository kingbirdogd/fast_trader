/*
 * AlgoBetSmall.cpp
 *
 *  Created on: 2017¦~9¤ë4¤é
 *      Author: leo
 */

#include "AlgoBetX.h"
#include "AlgoBetNormal.h"
#include "AlgoBetSmall.h"
#include "AlgoBetXSmall.h"
#include "AlgoBet60.h"
#include "AlgoBet80.h"
#include "AlgoBet50.h"


AlgoBetX::AlgoBetX() {
	// TODO Auto-generated constructor stub

	//AlgoBet* algoBetNormal = dynamic_cast<AlgoBet*>(new AlgoBetNormal());
	AlgoBet* algoBetSmall = dynamic_cast<AlgoBet*>(new AlgoBetSmall());
	//AlgoBet* algoBet60 = dynamic_cast<AlgoBet*>(new AlgoBet60());
	//AlgoBet* algoBet80 = dynamic_cast<AlgoBet*>(new AlgoBet80());
	//AlgoBet* algoBet50 = dynamic_cast<AlgoBet*>(new AlgoBet50());
	//AlgoBet* algoBetXSmall = dynamic_cast<AlgoBet*>(new AlgoBetXSmall());

	//betMap["BetNormal"] = algoBetNormal;
	betMap["BetSmall"] = algoBetSmall;
	//betMap["Bet60"] = algoBet60;
	//betMap["Bet80"] = algoBet80;
	//betMap["Bet50"] = algoBet50;
	//betMap["BetXSmall"] = algoBetXSmall;

	currentBet = algoBetSmall;
}

AlgoBetX::~AlgoBetX() {
	// TODO Auto-generated destructor stub
}

AlgoBet* AlgoBetX::getCurrentBet(string name){
	auto it = betMap.find(name);
	if(it != betMap.end()){
		return betMap[name];
	}
	return currentBet;
}

void AlgoBetX::selectBet(string name){
	auto it = betMap.find(name);
	if(it != betMap.end()){
		currentBet = betMap[name];
	}
}

unsigned long long AlgoBetX::fixQuantity(unsigned long long price, unsigned long long quantity){
	return currentBet->fixQuantity(price,quantity);
}

unsigned long long AlgoBetX::fixQuantityByIssuer(unsigned long long price,unsigned long long quantity, string issuer){
	return currentBet->fixQuantityByIssuer(price,quantity,issuer);
}

