/*
 * SimplePriceMark.cpp
 *
 *  Created on: 2017¦~5¤ë22¤é
 *      Author: leo
 */

#include "PriceMark.h"









PriceMark::PriceMark(unsigned int code, int wtype){
	pcode = code;
	pwtype = wtype;
}

PriceMark::~PriceMark() {
	// TODO Auto-generated destructor stub
}


void PriceMark::clearall(){
	pBidMark.clear();
	pAskMark.clear();
}

string PriceMark::ftos(float value, int dp){
	stringstream ss;
	ss << fixed << setprecision(dp) << value;
	string mystring = ss.str();
	return mystring;
}

vector<string> PriceMark::split(string str, char delimiter) {
  vector<string> internal;
  stringstream ss(str); // Turn the string into a stream.
  string tok;

  while(getline(ss, tok, delimiter)) {
    internal.push_back(tok);
  }
  return internal;
}

int PriceMark::getWType(){
	return pwtype;
}

unsigned long long PriceMark::getBidKey(){
	return bidkey;
}
unsigned long long PriceMark::getAskKey(){
	return askkey;
}
unsigned long long PriceMark::getBidPrice(){
	return bidprice;
}
unsigned long long PriceMark::getAskPrice(){
	return askprice;
}

string PriceMark::updateTableBid(string key, unsigned long long bestbidprice){
	auto it = priceMarkTable.find(key);
	if(priceMarkTable.end() == it){
		string bestvalue = to_string(bestbidprice) + "-#";
		priceMarkTable[key] = bestvalue;
		return bestvalue;
	}else{
		string bestvalue = it->second;
		vector<string> bidask = split(bestvalue, '-');
		string newbestvalue = "";
		if(bidask[0].compare("#") == 0){
			newbestvalue = to_string(bestbidprice) + "-" + bidask[1];
		}else{
			if(bidask[1].compare(to_string(bestbidprice)) != 0){
				newbestvalue = to_string(bestbidprice) + "-" + bidask[1];
			}else{
				newbestvalue = bestvalue;
			}
		}
		priceMarkTable[key] = newbestvalue;
		return newbestvalue;
	}
}

string PriceMark::updateTableAsk(string key, unsigned long long bestaskprice){
	auto it = priceMarkTable.find(key);
	if(priceMarkTable.end() == it){
		string bestvalue = "#-" + to_string(bestaskprice);
		priceMarkTable[key] = bestvalue;
		return bestvalue;
	}else{
		string bestvalue = it->second;
		vector<string> bidask = split(bestvalue, '-');
		string newbestvalue = "";
		if(bidask[1].compare("#") == 0){
			newbestvalue = bidask[0] + "-" + to_string(bestaskprice);
		}else{
			if(bidask[0].compare(to_string(bestaskprice)) != 0){
				newbestvalue = bidask[0] + "-" + to_string(bestaskprice);
			}else{
				newbestvalue = bestvalue;
			}
		}
		//string newbestvalue = bidask[0] + "-" + ftos(bestaskprice,3);
		priceMarkTable[key] = newbestvalue;
		return newbestvalue;
	}
}

unsigned long long PriceMark::getWarrantBidAskSpread(string key){
	auto it = priceMarkTable.find(key);
	if(priceMarkTable.end() == it){
		return 999999ull;
	}
	string value = it->second;
	vector<string> bidask = split(value, '-');

	if(bidask[0].compare("#") == 0 || bidask[1].compare("#") == 0 ){
		return 999999ull;
	}

	unsigned long long wbid = std::stoull(bidask[0]);
	unsigned long long wask = std::stoull(bidask[1]);

	if(wbid > wask){
		return 999999ull;
	}

	return wask - wbid;
}

unsigned long long PriceMark::getRefWarrantBid(string key){
	auto it = priceMarkTable.find(key);
	if(priceMarkTable.end() != it){
		string value = it->second;
		vector<string> bidask = split(value, '-');
		if(bidask[0].compare("#") != 0){
			unsigned long long bid = std::stoull(bidask[0].c_str());
			return bid;
		}
		return 0;
	}
	return 0;
}


//bool PriceMark::updateBid(float wbid, float pwbid, int fbid, int pfbid){
bool PriceMark::updateBid(unsigned long long wbid, unsigned long long pwbid, unsigned long long fprice, unsigned long long  pfprice){

	if(pwtype == BULL){
		if(wbid > pwbid && fprice > pfprice){
			if(pwbid > 0 && pfprice>0){

				//unsigned long long spread = spreadTable.getSpread("01", pfprice + 1llu);
				//unsigned long long refprice = pfprice+spread;
				//bool eq1 = (refprice == fprice);
				//if(eq1){

					pUpBidMark[wbid] = fprice;

					auto itp = pBidMark.find(wbid);
					if(itp != pBidMark.end()){
						return false;
					}
					pBidMark[wbid] = fprice;

					if(bidkey != wbid && bidprice != fprice && bidkey > 0 && bidprice > 0){
						long long diff = abs(static_cast<long long>(fprice) -  static_cast<long long>(bidprice));

						auto itdiff = pSpreadCountMap.find(diff);
						if(itdiff == pSpreadCountMap.end()){
							pSpreadCountMap[diff] = 1;
						}else{
							//int c =  pSpreadCountMap[diff];
							pSpreadCountMap[diff]++;
						}

					}

					bidkey = wbid;
					bidprice = fprice;



					unsigned long long spread = spreadTable.getSpread("01", fprice + 1llu);
					unsigned long long refask = fprice+spread;
					string ukey = to_string(fprice) +"-"+ to_string(refask);
					updateTableBid(ukey, wbid);

					return true;
				//}


				/*
				auto itp = pDnBidMark.find(wbid);
				if(itp != pDnBidMark.end()){
					return false;
				}
				pDnBidMark[wbid] = fprice;
				bidkey = wbid;
				bidprice = fprice;

				unsigned long long spread = spreadTable.getSpread("01", fprice + 1llu);
				unsigned long long refask = fprice+spread;
				string ukey = to_string(fprice) +"-"+ to_string(refask);
				updateTableBid(ukey, wbid);

				return true;
				*/
				return false;
			}

		}
		if(wbid < pwbid && fprice < pfprice){
			if(pwbid > 0 && pfprice>0){
				/*
				auto itp = pDnBidMark.find(pwbid);
				if(itp != pDnBidMark.end()){
					if(itp->second != pfprice){
						pDnBidMark[pwbid] = pfprice;
						bidkey = pwbid;
						bidprice = pfprice;
						return true;
					}else{
						return false;
					}
				}*/

				pDnBidMark[pwbid] = pfprice;

				pBidMark[pwbid] = pfprice;

				if(bidkey != pwbid && bidprice != pfprice && bidkey > 0 && bidprice > 0){
					long long diff = abs(static_cast<long long>(pfprice) - static_cast<long long>(bidprice));

					auto itdiff = pSpreadCountMap.find(diff);
					if(itdiff == pSpreadCountMap.end()){
						pSpreadCountMap[diff] = 1;
					}else{
						//int c =  pSpreadCountMap[diff];
						pSpreadCountMap[diff]++;
					}

				}


				bidkey = pwbid;
				bidprice = pfprice;

				unsigned long long spread = spreadTable.getSpread("01", pfprice + 1llu);
				unsigned long long refask = pfprice+spread;
				string ukey = to_string(pfprice) +"-"+ to_string(refask);
				updateTableBid(ukey, pwbid);
				return true;
			}
		}
	}
	if(pwtype == BEAR){
		if(wbid > pwbid && fprice < pfprice){
			if(pwbid > 0 && fprice>0){

				//unsigned long long spread = spreadTable.getSpread("01", pfprice - 1llu);
				//unsigned long long refprice = pfprice-spread;
				//bool eq1 = (refprice == fprice);
				//if(eq1){

					pUpBidMark[pwbid] = pfprice;

					auto itp = pBidMark.find(wbid);
					if(itp != pBidMark.end()){
						return false;
					}

					if(bidkey != wbid && bidprice != fprice && bidkey > 0 && bidprice > 0){
						//unsigned long long diff = pfprice - bidprice;

						long long diff = abs(static_cast<long long>(fprice) - static_cast<long long>(bidprice));

						auto itdiff = pSpreadCountMap.find(diff);
						if(itdiff == pSpreadCountMap.end()){
							pSpreadCountMap[diff] = 1;
						}else{
							//int c =  pSpreadCountMap[diff];
							pSpreadCountMap[diff]++;
						}

					}



					pBidMark[wbid] = fprice;
					bidkey = wbid;
					bidprice = fprice;

					unsigned long long spread = spreadTable.getSpread("01", fprice - 1llu);
					unsigned long long refbid = fprice-spread;
					string ukey =  to_string(refbid)  +"-"+ to_string(fprice);
					updateTableBid(ukey, wbid);

					return true;
				//}


				/*
				auto itp = pDnBidMark.find(wbid);
				if(itp != pDnBidMark.end()){

					if(itp->second != fprice){
						pDnBidMark[wbid] = fprice;
						bidkey = wbid;
						bidprice = fprice;
						return true;
					}else{
						return false;
					}
					return false;
				}
				pDnBidMark[wbid] = fprice;
				bidkey = wbid;
				bidprice = fprice;

				unsigned long long spread = spreadTable.getSpread("01", fprice + 1llu);
				unsigned long long refask = fprice+spread;
				string ukey = to_string(fprice) +"-"+ to_string(refask);
				updateTableBid(ukey, wbid);

				return true;
				*/
				return false;
			}
		}
		if(wbid < pwbid && fprice > pfprice){
			if(wbid > 0 && pfprice>0){
				/*
				auto itp = pDnBidMark.find(pwbid);
				if(itp != pDnBidMark.end()){
					if(itp->second != pfprice){
						pDnBidMark[pwbid] = pfprice;
						bidkey = pwbid;
						bidprice = pfprice;
						return true;
					}else{
						return false;
					}
				}
			*/

				if(bidkey != pwbid && bidprice != pfprice && bidkey > 0 && bidprice > 0){
					//unsigned long long diff = pfprice - bidprice;

					long long diff = abs(static_cast<long long>(pfprice) - static_cast<long long>(bidprice));

					auto itdiff = pSpreadCountMap.find(diff);
					if(itdiff == pSpreadCountMap.end()){
						pSpreadCountMap[diff] = 1;
					}else{
						//int c =  pSpreadCountMap[diff];
						pSpreadCountMap[diff]++;
					}

				}


				pDnBidMark[pwbid] = pfprice;

				pBidMark[pwbid] = pfprice;
				bidkey = pwbid;
				bidprice = pfprice;

				unsigned long long spread = spreadTable.getSpread("01", pfprice - 1llu);
				unsigned long long refbid = pfprice-spread;
				string ukey =   to_string(refbid) +"-"+ to_string(pfprice);
				updateTableBid(ukey, pwbid);

				return true;
			}
		}
	}
	return false;
}

bool PriceMark::updateAsk(unsigned long long wask, unsigned long long  pwask, unsigned long long  fprice, unsigned long long  pfprice){
	if(pwtype == BULL ){
		if(wask > pwask && fprice > pfprice){
			if(pwask > 0 && pfprice>0){

				/*
				auto itp = pUpAskMark.find(pwask);
				if(itp != pUpAskMark.end()){
					if(itp->second != pfprice){
						pUpAskMark[pwask] = pfprice;
						askkey = pwask;
						askprice = pfprice;
						return true;
					}else{
						return false;
					}
				}
				*/
				if(askkey != pwask && askprice != pfprice && askkey > 0 && askprice > 0){
					//unsigned long long diff = pfprice - bidprice;

					long long diff = abs(static_cast<long long>(pfprice) - static_cast<long long>(askkey));

					auto itdiff = pSpreadCountMap.find(diff);
					if(itdiff == pSpreadCountMap.end()){
						pSpreadCountMap[diff] = 1;
					}else{
						//int c =  pSpreadCountMap[diff];
						pSpreadCountMap[diff]++;
					}

				}



				pUpAskMark[pwask] = pfprice;


				pAskMark[pwask] = pfprice;
				askkey = pwask;
				askprice = pfprice;

				unsigned long long spread = spreadTable.getSpread("01", pfprice - 1llu);
				unsigned long long refbid = pfprice-spread;
				string ukey = to_string(refbid) +"-"+ to_string(pfprice)  ;
				updateTableAsk(ukey, pwask);

				return true;
			}
		}
		if(wask < pwask && fprice < pfprice){
			if(fprice > 0 && wask>0){


				//unsigned long long spread = spreadTable.getSpread("01", pfprice - 1llu);
				//unsigned long long refprice = pfprice-spread;
				//bool eq1 = (refprice == fprice);
				//if(eq1){

					pDnAskMark[wask] = fprice;

					auto itp = pAskMark.find(wask);
					if(itp != pAskMark.end()){
						return false;
					}

					if(askkey != wask && askprice != fprice && askkey > 0 && askprice > 0){
						//unsigned long long diff = pfprice - bidprice;

						long long diff = abs(static_cast<long long>(fprice) - static_cast<long long>(askkey));

						auto itdiff = pSpreadCountMap.find(diff);
						if(itdiff == pSpreadCountMap.end()){
							pSpreadCountMap[diff] = 1;
						}else{
							//int c =  pSpreadCountMap[diff];
							pSpreadCountMap[diff]++;
						}

					}

					pAskMark[wask] = fprice;
					askkey = wask;
					askprice = fprice;

					unsigned long long spread = spreadTable.getSpread("01", fprice - 1llu);
					unsigned long long refbid = fprice-spread;
					string ukey = to_string(refbid) +"-"+ to_string(fprice)  ;
					updateTableAsk(ukey, wask);

					return true;
				//}

				/*
				auto itp = pUpAskMark.find(wask);
				if(itp != pUpAskMark.end()){
					return false;
				}
				pUpAskMark[wask] = fprice;
				askkey = wask;
				askprice = fprice;

				unsigned long long spread = spreadTable.getSpread("01", fprice - 1llu);
				unsigned long long refbid = fprice-spread;
				string ukey = to_string(refbid) +"-"+ to_string(fprice)  ;
				updateTableAsk(ukey, wask);

				return true;
				*/

					//return false;
			}


		}
	}
	if(pwtype == BEAR ){
		if(wask > pwask && fprice < pfprice){
			if(pwask > 0 && fprice>0){
				/*
				auto itp = pUpAskMark.find(pwask);
				if(itp != pUpAskMark.end()){
					if(itp->second != pfprice){
						pUpAskMark[pwask] = pfprice;
						askkey = pwask;
						askprice = pfprice;

						return true;
					}else{
						return false;
					}
				}*/

				if(askkey != pwask && askprice != pfprice && askkey > 0 && askprice > 0){
					//unsigned long long diff = pfprice - bidprice;

					long long diff = abs(static_cast<long long>(pfprice) - static_cast<long long>(askkey));

					auto itdiff = pSpreadCountMap.find(diff);
					if(itdiff == pSpreadCountMap.end()){
						pSpreadCountMap[diff] = 1;
					}else{
						//int c =  pSpreadCountMap[diff];
						pSpreadCountMap[diff]++;
					}

				}


				pUpAskMark[pwask] = pfprice;

				pAskMark[pwask] = pfprice;
				askkey = pwask;
				askprice = pfprice;

				unsigned long long spread = spreadTable.getSpread("01", pfprice + 1llu);
				unsigned long long refask = pfprice+spread;
				string ukey =   to_string(pfprice) +"-"+ to_string(refask) ;
				updateTableAsk(ukey, pwask);

				return true;
			}
		}
		if(wask < pwask && fprice > pfprice){

			if(pfprice > 0 && wask>0){
				//unsigned long long spread = spreadTable.getSpread("01", pfprice + 1llu);
				//unsigned long long refprice = pfprice+spread;
				//bool eq1 = (refprice == fprice);
				//if(eq1){

					pDnAskMark[wask] = fprice;

					auto itp = pAskMark.find(wask);
					if(itp != pAskMark.end()){
						return false;
					}

					if(askkey != wask && askprice != fprice && askkey > 0 && askprice > 0){
						//unsigned long long diff = pfprice - bidprice;

						long long diff = abs(static_cast<long long>(fprice) - static_cast<long long>(askkey));

						auto itdiff = pSpreadCountMap.find(diff);
						if(itdiff == pSpreadCountMap.end()){
							pSpreadCountMap[diff] = 1;
						}else{
							//int c =  pSpreadCountMap[diff];
							pSpreadCountMap[diff]++;
						}

					}


					pAskMark[wask] = fprice;
					askkey = wask;
					askprice = fprice;

					unsigned long long spread = spreadTable.getSpread("01", fprice + 1llu);
					unsigned long long refask = fprice+spread;
					string ukey =   to_string(fprice) +"-"+ to_string(refask) ;
					updateTableAsk(ukey, wask);

					return true;
				//}

				/*
				auto itp = pUpAskMark.find(wask);
				if(itp != pUpAskMark.end()){

					if(itp->second != fprice){
						pUpAskMark[wask] = fprice;
						askkey = wask;
						askprice = fprice;
						return true;
					}else{
						return false;
					}
					return false;
				}
				pUpAskMark[wask] = fprice;
				askkey = pwask;
				askprice = fprice;

				unsigned long long spread = spreadTable.getSpread("01", fprice - 1llu);
				unsigned long long refbid = fprice-spread;
				string ukey = to_string(refbid) +"-"+ to_string(fprice)  ;
				updateTableAsk(ukey, wask);

				return true;
				*/

					//return false;
			}

		}
	}
	return false;
}


bool PriceMark::updateIBid(unsigned long long wbid, unsigned long long pwbid, unsigned long long fprice, unsigned long long  pfprice){

	if(pwtype == BULL){
		if(wbid > pwbid && fprice > pfprice){
			if(pwbid > 0 && pfprice>0){

				//unsigned long long spread = spreadTable.getSpread("01", pfprice + 1llu);
				//unsigned long long refprice = pfprice+INDEX_SPREAD;
				//bool eq1 = (refprice == fprice);
				//if(eq1){

					pUpBidMark[wbid] = fprice;

					auto itp = pBidMark.find(wbid);
					if(itp != pBidMark.end()){
						return false;
					}
					pBidMark[wbid] = fprice;
					bidkey = wbid;
					bidprice = fprice;

					return true;
				//}
				//return false;
			}

		}
		if(wbid < pwbid && fprice < pfprice){
			if(pwbid > 0 && pfprice>0){


				pDnBidMark[pwbid] = pfprice;

				pBidMark[pwbid] = pfprice;
				bidkey = pwbid;
				bidprice = pfprice;

				return true;
			}
		}
	}
	if(pwtype == BEAR){
		if(wbid > pwbid && fprice < pfprice){
			if(pwbid > 0 && fprice>0){

				//unsigned long long spread = spreadTable.getSpread("01", pfprice - 1llu);
				//unsigned long long refprice = pfprice-INDEX_SPREAD;
				//bool eq1 = (refprice == fprice);
				//if(eq1){

					pUpBidMark[wbid] = fprice;

					auto itp = pBidMark.find(wbid);
					if(itp != pBidMark.end()){
						return false;
					}
					pBidMark[wbid] = fprice;
					bidkey = wbid;
					bidprice = fprice;

					return true;
				//}

				//return false;
			}
		}
		if(wbid < pwbid && fprice > pfprice){
			if(wbid > 0 && pfprice>0){

				pDnBidMark[pwbid] = pfprice;

				pBidMark[pwbid] = pfprice;
				bidkey = pwbid;
				bidprice = pfprice;

				return true;
			}
		}
	}
	return false;
}

bool PriceMark::updateIAsk(unsigned long long wask, unsigned long long  pwask, unsigned long long  fprice, unsigned long long  pfprice){
	if(pwtype == BULL ){
		if(wask > pwask && fprice > pfprice){
			if(pwask > 0 && pfprice>0){

				pUpAskMark[pwask] = pfprice;

				pAskMark[pwask] = pfprice;
				askkey = pwask;
				askprice = pfprice;

				return true;
			}
		}
		if(wask < pwask && fprice < pfprice){
			if(fprice > 0 && wask>0){


				//unsigned long long spread = spreadTable.getSpread("01", pfprice - 1llu);
				//unsigned long long refprice = pfprice-INDEX_SPREAD;
				//bool eq1 = (refprice == fprice);
				//if(eq1){


					pDnAskMark[wask] = fprice;

					auto itp = pAskMark.find(wask);
					if(itp != pAskMark.end()){
						return false;
					}
					pAskMark[wask] = fprice;
					askkey = wask;
					askprice = fprice;

					return true;
				//}

				//return false;
			}


		}
	}
	if(pwtype == BEAR ){
		if(wask > pwask && fprice < pfprice){
			if(pwask > 0 && fprice>0){


				pUpAskMark[pwask] = pfprice;

				pAskMark[pwask] = pfprice;
				askkey = pwask;
				askprice = pfprice;

				return true;
			}
		}
		if(wask < pwask && fprice > pfprice){

			if(pfprice > 0 && wask>0){
				//unsigned long long spread = spreadTable.getSpread("01", pfprice + 1llu);
				//unsigned long long refprice = pfprice+INDEX_SPREAD;
				//bool eq1 = (refprice == fprice);
				//if(eq1){

					pDnAskMark[wask] = fprice;

					auto itp = pAskMark.find(wask);
					if(itp != pAskMark.end()){
						return false;
					}

					pAskMark[wask] = fprice;
					askkey = wask;
					askprice = fprice;

					return true;
				//}
				//return false;
			}

		}
	}
	return false;
}

unsigned long long PriceMark::buyIn(unsigned long long wprice){
	//string swask = to_string(wprice);
	unsigned long long buyInUpAsk = 0;
	auto aup = pAskMark.find(wprice);
	if(aup != pAskMark.end()){
		buyInUpAsk = pAskMark[wprice];
	}
	return buyInUpAsk;
}

unsigned long long PriceMark::sellOut(unsigned long long wprice){
	//string swbid = to_string(wprice);
	unsigned long long sellOutDnBid = 99999999;
	auto bdn = pBidMark.find(wprice);
	if(bdn != pBidMark.end()){
		sellOutDnBid = pBidMark[wprice];
	}
	return sellOutDnBid;
}

void PriceMark::setSellout(unsigned long long wprice, unsigned long long uprice){
	auto bdn = pBidMark.find(wprice);
	if(bdn != pBidMark.end())
		return;

	pBidMark[wprice] = uprice;
}

unsigned long long PriceMark::getIssuerIize(){
	return pIssuerSize;
}

map<unsigned long long,unsigned long long> PriceMark::getBidTable(){
	return pBidMark;
}

map<unsigned long long,unsigned long long> PriceMark::getAskTable(){
	return pAskMark;
}

map<unsigned long long,unsigned long long> PriceMark::getUpBidTable(){
	return pUpBidMark;
}
map<unsigned long long,unsigned long long> PriceMark::getUpAskTable(){
	return pUpAskMark;
}
map<unsigned long long,unsigned long long> PriceMark::getDnBidTable(){
	return pDnBidMark;
}
map<unsigned long long,unsigned long long> PriceMark::getDnAskTable(){
	return pDnAskMark;
}

unsigned long long PriceMark::getIssuerBidQty(){
	return BidIssuerQty;
}
unsigned long long PriceMark::getIssuerAskQty(){
	return BidIssuerQty;
}
void PriceMark::setBidIssuerQty(unsigned long long qty){
	BidIssuerQty = qty;
}
void PriceMark::setAskIssuerQty(unsigned long long qty){
	AskIssuerQty = qty;
}

string PriceMark::printTable(unsigned long long ubid){

	unsigned long long spread = spreadTable.getSpread("01", ubid - 1llu);

	unsigned long long refbid = ubid - 4*spread;

	unsigned long long aspread = spreadTable.getSpread("01", refbid + 1llu);

	unsigned long long refask = refbid+aspread;
	string ukey = to_string(refbid) +"-"+ to_string(refask);

	string data = "Warrant Code = " + to_string(pcode) + " Price Table\n";
	int count = 0;
	for (auto it = priceMarkTable.begin(); it != priceMarkTable.end(); ++it ){
		string key = it->first;
		string value = it->second;
		if(key.compare(ukey) >= 0){
			data += key + "=" + value + "\n";
			count++;
		}
		if(count > 12)
			break;
	}
	return data;
}

string PriceMark::printTable(){

	string data = "Warrant Code = " + to_string(pcode) + " Price Table\n";
	int count = 0;
	for (auto it = priceMarkTable.begin(); it != priceMarkTable.end(); ++it ){
		string key = it->first;
		string value = it->second;

		data += key + "=" + value + "\n";
	}
	return data;
}

string PriceMark::printIndexTable(){

	string data = "Warrant Code = " + to_string(pcode) + " Price Table\n";

	map<unsigned long long,std::string> pTableMap;

	for (auto it = pBidMark.begin(); it != pBidMark.end(); ++it ){
			unsigned long long key = it->first;
			unsigned long long value = it->second;
			pTableMap[key] = to_string(value) + "-#";
	}

	for (auto it = pAskMark.begin(); it != pAskMark.end(); ++it ){
			unsigned long long key = it->first;
			unsigned long long value = it->second;
			auto itf = pTableMap.find(key);
			if(itf == pTableMap.end()){
				pTableMap[key] = "#-" + to_string(value);
			}else{
				std:string svalue = pTableMap[key];
				vector<string> bidask = split(svalue, '-');

				pTableMap[key] = bidask[0] +  "-" + to_string(value);
			}

	}

	int count = 0;
	for (auto it2 = pTableMap.begin(); it2 != pTableMap.end(); ++it2 ){
		unsigned long long key = it2->first;
		string value = it2->second;

		data += to_string(key) + "=" + value + "\n";
	}
	return data;
}


string PriceMark::printIndexTable1(){

	string data = "Warrant Code = " + to_string(pcode) + " Price Table 1\n";

	map<unsigned long long,std::string> pTableMap;

	for (auto it = pDnBidMark.begin(); it != pDnBidMark.end(); ++it ){
			unsigned long long key = it->first;
			unsigned long long value = it->second;
			pTableMap[key] = to_string(value) + "-#";
	}

	for (auto it = pUpAskMark.begin(); it != pUpAskMark.end(); ++it ){
			unsigned long long key = it->first;
			unsigned long long value = it->second;
			auto itf = pTableMap.find(key);
			if(itf == pTableMap.end()){
				pTableMap[key] = "#-" + to_string(value);
			}else{
				std:string svalue = pTableMap[key];
				vector<string> bidask = split(svalue, '-');

				pTableMap[key] = bidask[0] +  "-" + to_string(value);
			}

	}

	int count = 0;
	for (auto it2 = pTableMap.begin(); it2 != pTableMap.end(); ++it2 ){
		unsigned long long key = it2->first;
		string value = it2->second;

		data += to_string(key) + "=" + value + "\n";
	}
	return data;
}

string PriceMark::printIndexTable2(){

	string data = "Warrant Code = " + to_string(pcode) + " Price Table 2\n";

	map<unsigned long long,std::string> pTableMap;

	for (auto it = pUpBidMark.begin(); it != pUpBidMark.end(); ++it ){
			unsigned long long key = it->first;
			unsigned long long value = it->second;
			pTableMap[key] = to_string(value) + "-#";
	}

	for (auto it = pDnAskMark.begin(); it != pDnAskMark.end(); ++it ){
			unsigned long long key = it->first;
			unsigned long long value = it->second;
			auto itf = pTableMap.find(key);
			if(itf == pTableMap.end()){
				pTableMap[key] = "#-" + to_string(value);
			}else{
				std:string svalue = pTableMap[key];
				vector<string> bidask = split(svalue, '-');

				pTableMap[key] = bidask[0] +  "-" + to_string(value);
			}

	}

	int count = 0;
	for (auto it2 = pTableMap.begin(); it2 != pTableMap.end(); ++it2 ){
		unsigned long long key = it2->first;
		string value = it2->second;

		data += to_string(key) + "=" + value + "\n";
	}
	return data;
}


string PriceMark::printTableJson(){

	string data = "{\"table\": \"" + to_string(pcode) + "\",";

	data += "\"rows\":[";
	int count = 0;
	for (auto it = priceMarkTable.begin(); it != priceMarkTable.end(); ++it ){
		string key = it->first;
		string value = it->second;

		if(count == 0)
			data += "{\"key\":\""+key+ "\",\"value\":\"" + value + "\"}";
		else
			data += ",{\"key\":\""+key+ "\",\"value\":\"" + value + "\"}";
		count++;
	}
	data += "]";
	data += "}";
	return data;
}


string PriceMark::printIndexTableJson(){

	string data = "{\"table\": \"" + to_string(pcode) + "\",";

	map<unsigned long long,std::string> pTableMap;

	for (auto it = pBidMark.begin(); it != pBidMark.end(); ++it ){
			unsigned long long key = it->first;
			unsigned long long value = it->second;
			pTableMap[key] = to_string(value) + "-#";
	}

	for (auto it = pAskMark.begin(); it != pAskMark.end(); ++it ){
			unsigned long long key = it->first;
			unsigned long long value = it->second;
			auto itf = pTableMap.find(key);
			if(itf == pTableMap.end()){
				pTableMap[key] = "#-" + to_string(value);
			}else{
				std:string svalue = pTableMap[key];
				vector<string> bidask = split(svalue, '-');

				pTableMap[key] = bidask[0] +  "-" + to_string(value);
			}

	}

	int count = 0;
	for (auto it2 = pTableMap.begin(); it2 != pTableMap.end(); ++it2 ){
		unsigned long long key = it2->first;
		string value = it2->second;

		//data += to_string(key) + "=" + value + "\n";

		if(count == 0)
			data += "{\"key\":\""+to_string(key)+ "\",\"value\":\"" + value + "\"}";
		else
			data += ",{\"key\":\""+to_string(key)+ "\",\"value\":\"" + value + "\"}";
		count++;

	}

	data += "]";
	data += "}";

	return data;
}


unsigned long long PriceMark::getMaxBidAskSpread(){

	unsigned long long maxBidAskSpread = 0;

	for (auto it = priceMarkTable.begin(); it != priceMarkTable.end(); ++it ){
		string value = it->second;

		vector<string> bidask = split(value, '-');
		if(bidask[0].compare("#") != 0 && bidask[1].compare("#") != 0 ){
			unsigned long long bidw = std::stoull(bidask[0].c_str());
			unsigned long long askw = std::stoull(bidask[1].c_str());

			if(askw > bidw){
				unsigned long long diff = (askw - bidw);
				if(diff > maxBidAskSpread)
					maxBidAskSpread = diff;
			}else{
				unsigned long long diff = (bidw - askw);
				if(diff > maxBidAskSpread)
					maxBidAskSpread = diff;
			}
		}
	}
	return maxBidAskSpread;
}


