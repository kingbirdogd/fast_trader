/*
 * IvLoader.h
 *
 *  Created on: 2017¦~5¤ë16¤é
 *      Author: leo
 */

#ifndef IVLOADER_H_
#define IVLOADER_H_

#include <string>
#include <regex>
#include <string.h>
#include <unordered_map>
#include <fstream>
#include <vector>
#include <sstream>
#include <iostream>
#include <unordered_set>

using namespace std;




struct WarrantIv{
	unsigned int Code;
	float Gearing;
	float Egearing;
	float Iv;
	float Delta;
	float Cratio;
	string Wtype;
	unsigned int UCode;
	string Issuer;
};

class IvLoader {
private:
	unordered_map<unsigned int, WarrantIv> IvMap;
	unordered_map<unsigned int, unsigned int> UMap;
	unordered_map<string, unordered_map<unsigned int, std::unordered_set<unsigned int>>> IssuerMap;
public:
	IvLoader();
	virtual ~IvLoader();
	void load(const char* filename);
	vector<string> split(string str, char delimiter);
	vector<WarrantIv> allWarrants();
	WarrantIv getWarrantIv(unsigned int);
	int getUnderlyingCount();
	int getWarrantCount();
	unordered_set<unsigned int> getWarrantByIssuer(string ,unsigned int);
};


#endif /* IVLOADER_H_ */
