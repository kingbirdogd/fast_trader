/*
 * IvLoader.cpp
 *
 *  Created on: 2017¦~5¤ë16¤é
 *      Author: leo
 */

#include "IvLoader.h"


IvLoader::IvLoader() {
	// TODO Auto-generated constructor stub

}

IvLoader::~IvLoader() {
	// TODO Auto-generated destructor stub
}

vector<string> IvLoader::split(string str, char delimiter) {
  vector<string> internal;
  stringstream ss(str); // Turn the string into a stream.
  string tok;

  while(getline(ss, tok, delimiter)) {
    internal.push_back(tok);
  }
  return internal;
}

void IvLoader::load(const char* filename){
	ifstream file(filename);
	string str;
	while (getline(file, str))
	{
		fprintf(stderr, "Load Iv File : %s\n", str.c_str());
		vector<string> sep = split(str, ',');

		struct WarrantIv wiv;
		string str = sep[6];
		if(str.find(".HK", 0) == std::string::npos && str.find(".HSI", 0) == std::string::npos && str.find(".HSCE", 0) == std::string::npos && str.find(".HSTECH", 0) == std::string::npos){
			continue;
		}


		wiv.Code = stoul(sep[0]);
		wiv.Gearing = atof (sep[1].c_str());
		wiv.Egearing = atof (sep[2].c_str());
		wiv.Iv = atof (sep[3].c_str());
		wiv.Delta = atof (sep[4].c_str());
		wiv.Cratio = atof (sep[5].c_str());

		str = regex_replace(str, regex("\\<"), "");
		str = regex_replace(str, regex("\\>"), "");
		str = regex_replace(str, regex("\\.HK"), "");

		str = regex_replace(str, regex("\."), "");


		fprintf(stderr, "STR UCODE : %s\n", str.c_str());

		bool isStockWarrant = false;
		if(str.compare("HSI") == 0){
			wiv.UCode = 100001;
		}else if(str.compare("HSCE") == 0){
			wiv.UCode = 100002;
		}else if(str.compare("HSTECH") == 0){
			wiv.UCode = 100003;
		}else{
			wiv.UCode = atoi(str.c_str());
			isStockWarrant = true;
		}
		wiv.Wtype = sep[7];
		wiv.Issuer = sep[8];
		IvMap[wiv.Code] = wiv;

		if(isStockWarrant){

			IssuerMap[wiv.Issuer][wiv.UCode].insert(wiv.Code);

			UWarrantMap[wiv.UCode].insert(wiv.Code);


			auto umap = UMap.find(wiv.UCode);
			if(UMap.end() == umap){
				UMap[wiv.UCode] = 1;
			}else{
				umap->second++;
			}
		}

	}
	file.close();
}

unordered_set<unsigned int> IvLoader::allUnderlying(){
	unordered_set<unsigned int> vd;
	for (auto& it: UMap) {
	    // Do stuff
	    unsigned int code = it.first;
	    vd.insert(code);
	}
	return vd;
}

vector<WarrantIv> IvLoader::allWarrants(){
	vector<WarrantIv> vd;
	vd.reserve(IvMap.size());
	for(auto kv : IvMap) {
		vd.push_back(kv.second);
	}
	return vd;
}

WarrantIv IvLoader::getWarrantIv(unsigned int code){
	auto it = IvMap.find(code);
	if(IvMap.end() != it){
		return it->second;
	}
	return WarrantIv();
}

unordered_set<unsigned int> IvLoader::getWarrantByIssuer(string issuer, unsigned int code){
	unordered_set<unsigned int> vd;
	auto it = IssuerMap.find(issuer);
	if(it != IssuerMap.end()){

		auto it2 = it->second.find(code);
		if(it2 != it->second.end()){


			for (const auto &n: it2->second){
				vd.insert(IvMap[n].Code);
			}

		}
	}
	return vd;
}

unordered_set<unsigned int> IvLoader::getWarrantUCode(unsigned int code){

	unordered_set<unsigned int> vd;
	auto it = UWarrantMap.find(code);
	if(it != UWarrantMap.end()){
		return it->second;
	}
	return vd;
}

bool IvLoader::exist(unsigned int code){
	auto it = IvMap.find(code);
	if(IvMap.end() != it){
		return true;
	}
	auto it2 = UMap.find(code);
	if(UMap.end() != it2){
		return true;
	}
	return false;
}


int IvLoader::getUnderlyingCount(){
	return (int)UMap.size();
}

int IvLoader::getWarrantCount(){
	return (int)IvMap.size();
}

