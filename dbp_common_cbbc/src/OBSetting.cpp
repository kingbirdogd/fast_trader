/*
 * OBSetting.cpp
 *
 *  Created on: 2017¦~5¤ë11¤é
 *      Author: leo
 */

#include "OBSetting.h"


OBSetting::OBSetting() {
	// TODO Auto-generated constructor stub

}

OBSetting::~OBSetting() {
	// TODO Auto-generated destructor stub
}

void OBSetting::addWarrantOrCbbc(warrant* wnrt){
	auto it = WarrantMap.find(wnrt->Code);
	if(WarrantMap.end() == it){
		WarrantMap[wnrt->Code] = wnrt;
	}
}

bool OBSetting::isExist(unsigned int code){
	auto it = WarrantMap.find(code);
	if(WarrantMap.end() == it){
		return false;
	}
	return true;
}

warrant* OBSetting::removeWarrantOrCbbc(unsigned int code){
	auto it = WarrantMap.find(code);
	if(WarrantMap.end() != it){
		warrant* wrnt = it->second;
		WarrantMap.erase(code);
		return wrnt;
	}
	return nullptr;
}

void OBSetting::removeAllWarrants(){
	WarrantMap.clear();
}

warrant* OBSetting::getRelatedWarrant(unsigned int code){
	auto it = WarrantMap.find(code);
	if(WarrantMap.end() != it){
		warrant* wrnt = it->second;
		return wrnt;
	}
	return nullptr;
}

int OBSetting::getRelatedWarrantCount(){
	return WarrantMap.size();
}

bool OBSetting::allStatus(int status){
	for ( auto it = WarrantMap.begin(); it != WarrantMap.end(); ++it )
	{
		warrant *w = it->second;
		if(w->Status != status){
			return false;
		}
	}
	return true;
}

bool OBSetting::hasWarrants(){
	return WarrantMap.size()>0;
}

unsigned long long OBSetting::getHighestStopLostPrice(){
	unsigned long long higheststoplost = 0;
	for ( auto it = WarrantMap.begin(); it != WarrantMap.end(); ++it )
	{
		warrant *w = it->second;
		if(w->StopLostPrice > higheststoplost){
			higheststoplost = w->StopLostPrice;
		}
	}
	return higheststoplost;
}

unsigned long long OBSetting::getLowestStopLostPrice(){
	unsigned long long loweststoplost = 9999ull * 100000000ull;
	for ( auto it = WarrantMap.begin(); it != WarrantMap.end(); ++it )
	{
		warrant *w = it->second;
		if(w->StopLostPrice < loweststoplost){
			loweststoplost = w->StopLostPrice;
		}
	}
	return loweststoplost;
}

bool OBSetting::hasRelatedWarrant(int status){
	for ( auto it = WarrantMap.begin(); it != WarrantMap.end(); ++it )
	{
		warrant *w = it->second;
		if(w->Status == status){
			return true;
		}
	}
	return false;
}

void OBSetting::printAllWarrant(){
	for ( auto it = WarrantMap.begin(); it != WarrantMap.end(); ++it )
	{
		warrant *w = it->second;
		fprintf(stderr, "Code = %u Status = %u\n", w->Code, w->Status);

	}
}


vector<warrant*> OBSetting::getRelatedWarrant(){
	vector<warrant*> vals;
	vals.reserve(WarrantMap.size());

	for (auto it = WarrantMap.begin(); it != WarrantMap.end(); ++it ){
	    vals.push_back(it->second);
	}

	return vals;
}

void OBSetting::setRelatedWarrantStatus(unsigned int code, int status){
	auto it = WarrantMap.find(code);
	if(WarrantMap.end() != it){
		(it->second)->Status = status;
	}
}

bool OBSetting::warrantStatus(unsigned int code, int status){
	auto it = WarrantMap.find(code);
	if(WarrantMap.end() != it){
		if(it->second->Status == status){
			return true;
		}
	}
	return false;
}

int OBSetting::getWarrantStatus(unsigned int code){
	auto it = WarrantMap.find(code);
	if(WarrantMap.end() != it){
		return it->second->Status;
	}
	return -1;
}

/* namespace easywsclient */
