/*
 * AlgoBetSmall.cpp
 *
 *  Created on: 2017¦~9¤ë4¤é
 *      Author: leo
 */

#include "AlgoBetX.h"
#include "AlgoBetSmall.h"
#include "AlgoBet1Lot.h"
#include "AlgoBet100.h"


AlgoBetX::AlgoBetX() {
	// TODO Auto-generated constructor stub

	//AlgoBet* algoBetNormal = dynamic_cast<AlgoBet*>(new AlgoBetNormal());
	AlgoBet* algoBetSmall = dynamic_cast<AlgoBet*>(new AlgoBetSmall());
	AlgoBet* algoBet1Lot = dynamic_cast<AlgoBet*>(new AlgoBet1Lot());
	AlgoBet* algoBet100 = dynamic_cast<AlgoBet*>(new AlgoBet100());
	//AlgoBet* algoBet60 = dynamic_cast<AlgoBet*>(new AlgoBet60());
	//AlgoBet* algoBet80 = dynamic_cast<AlgoBet*>(new AlgoBet80());
	//AlgoBet* algoBet50 = dynamic_cast<AlgoBet*>(new AlgoBet50());
	//AlgoBet* algoBetXSmall = dynamic_cast<AlgoBet*>(new AlgoBetXSmall());

	//betMap["BetNormal"] = algoBetNormal;
	betMap["BetSmall"] = algoBetSmall;
	betMap["Bet1Lot"] = algoBet1Lot;
	betMap["Bet100"] = algoBet100;
	//betMap["Bet60"] = algoBet60;
	//betMap["Bet80"] = algoBet80;
	//betMap["Bet50"] = algoBet50;
	//betMap["BetXSmall"] = algoBetXSmall;

	currentBet = algoBet1Lot;
}

AlgoBetX::~AlgoBetX() {
	// TODO Auto-generated destructor stub
}

string AlgoBetX::getBetName(){
	return currentBet->getBetName();
}

AlgoBet* AlgoBetX::getCurrentBet(string name){
	auto it = betMap.find(name);
	if(it != betMap.end()){
		return betMap[name];
	}
	return currentBet;
}

string AlgoBetX::selectBet(string name){
	auto it = betMap.find(name);
	if(it != betMap.end()){
		currentBet = betMap[name];

		return name;
	}
	return currentBet->getBetName();
}

unsigned long long AlgoBetX::fixQuantityBySpread(unsigned long long price, unsigned long long quantity, unsigned long long spread){
	return currentBet->fixQuantityBySpread(price,quantity,spread);
}

unsigned long long AlgoBetX::fixQuantity(unsigned long long price, unsigned long long quantity){
	return currentBet->fixQuantity(price,quantity);
}

unsigned long long AlgoBetX::fixQuantityByIssuer(unsigned long long price,unsigned long long quantity, string issuer){
	return currentBet->fixQuantityByIssuer(price,quantity,issuer);
}

