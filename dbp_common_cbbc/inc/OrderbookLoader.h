/*
 * OrderbookLoader.h
 *
 *  Created on: 2017¦~5¤ë16¤é
 *      Author: leo
 */

#ifndef ORDERBOOKLOADER_H_
#define ORDERBOOKLOADER_H_

#include <string>
#include <regex>
#include <string.h>
#include <unordered_map>
#include <fstream>
#include <vector>
#include <sstream>
#include <iostream>

#include "NormalDistribution.h"


using namespace std;


class OrderbookLoader {
private:
	unordered_map<unsigned int, NormalDistribution*> NormMap;
public:
	OrderbookLoader();
	virtual ~OrderbookLoader();
	void load(const char* filename);
	vector<string> split(string str, char delimiter);
	NormalDistribution* getNormDistribution(unsigned int code);
};


#endif /* ORDERBOOKLOADER_H_ */
