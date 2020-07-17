/*
 * SelectedWarrant.h
 *
 *  Created on: 2017¦~5¤ë17¤é
 *      Author: leo
 */


#ifndef SELECTEDWARRANT_H_
#define SELECTEDWARRANT_H_

#include <string>
#include <string.h>
#include <iostream>

using namespace std;


class SelectedWarrant {
public:
	SelectedWarrant();
	virtual ~SelectedWarrant();
	bool isAccept(float uspread, float delta, float cratio, float bidaskspread, float ticks);
};


#endif /* SELECTEDWARRANT_H_ */
