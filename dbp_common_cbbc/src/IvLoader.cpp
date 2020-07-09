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
		//fprintf(stderr, "Load Iv File : %s\n", str.c_str());
		vector<string> sep = split(str, ',');

		struct WarrantIv wiv;
		string str = sep[6];
		if(str.find(".HK", 0) == std::string::npos){
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
		wiv.UCode = atoi(str.c_str());
		wiv.Wtype = sep[7];
		wiv.Issuer = sep[8];
		IvMap[wiv.Code] = wiv;

		IssuerMap[wiv.Issuer][wiv.UCode].insert(wiv.Code);

		auto umap = UMap.find(wiv.UCode);
		if(UMap.end() == umap){
			UMap[wiv.UCode] = 1;
		}else{
			umap->second++;
		}

	}
	file.close();
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




int IvLoader::getUnderlyingCount(){
	return (int)UMap.size();
}

int IvLoader::getWarrantCount(){
	return (int)IvMap.size();
}

