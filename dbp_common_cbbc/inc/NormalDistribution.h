/*
 * NormalDistribution.h
 *
 *  Created on: 2017¦~5¤ë16¤é
 *      Author: leo
 */

#ifndef NORMALDISTRIBUTION_H_
#define NORMALDISTRIBUTION_H_

#include <math.h>


using namespace std;


class NormalDistribution {
private:
	double SQRT2 = sqrt(2.0);
	double Mean;
	double standardDeviation;
	double erfInv(const double x);
public:
	NormalDistribution();
	NormalDistribution(double mean, double sd);
	virtual ~NormalDistribution();
	double cumulativeProbability(double x);
	double inverseCumulativeProbability(double p);
};


#endif /* NORMALDISTRIBUTION_H_ */
