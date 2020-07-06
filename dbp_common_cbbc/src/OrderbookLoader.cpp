/*
 * OrderbookLoader.cpp
 *
 *  Created on: 2017¦~5¤ë16¤é
 *      Author: leo
 */

#include "OrderbookLoader.h"


OrderbookLoader::OrderbookLoader() {
	// TODO Auto-generated constructor stub

}

OrderbookLoader::~OrderbookLoader() {
	// TODO Auto-generated destructor stub
}

void OrderbookLoader::load(const char* filename){
	ifstream file(filename);
	string str;
	while (getline(file, str))
	{
		//fprintf(stderr, "Load Orderbook File : %s\n", str.c_str());
		vector<string> sep = split(str, ',');

		unsigned int Code = atoi (sep[0].c_str());
		double Mean = stod (sep[1].c_str());
		double Sd = stod (sep[2].c_str());

		//fprintf(stderr, "Load Orderbook File : %u %d %d\n", Code, Mean, Sd);

		NormalDistribution* nd = new NormalDistribution(Mean,Sd);

		NormMap[Code] = nd;

	}
	file.close();
}

vector<string> OrderbookLoader::split(string str, char delimiter) {
  vector<string> internal;
  stringstream ss(str); // Turn the string into a stream.
  string tok;

  while(getline(ss, tok, delimiter)) {
    internal.push_back(tok);
  }
  return internal;
}

NormalDistribution* OrderbookLoader::getNormDistribution(unsigned int code){
	auto it = NormMap.find(code);
	if(NormMap.end() != it){
		return it->second;
	}
	return nullptr;

}

