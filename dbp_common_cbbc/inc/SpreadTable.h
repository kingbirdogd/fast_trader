/*
 * SpreadTable.h
 *
 *  Created on: 2017¦~5¤ë15¤é
 *      Author: leo
 */

#ifndef SPREADTABLE_H_
#define SPREADTABLE_H_


#include <stdio.h>
#include <string>
#include <vector>

using namespace std;



class SpreadTable {
public:
	SpreadTable();
	virtual ~SpreadTable(){};
	unsigned long long getSpread(string spreadTableCode, unsigned long long price);
};



#endif /* SPREADTABLE_H_ */
