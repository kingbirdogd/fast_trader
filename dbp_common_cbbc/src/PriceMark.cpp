/*
 * SimplePriceMark.cpp
 *
 *  Created on: 2017¦~5¤ë22¤é
 *      Author: leo
 */

#include "PriceMark.h"









PriceMark::PriceMark(int wtype){
	pwtype = wtype;
}

PriceMark::~PriceMark() {
	// TODO Auto-generated destructor stub
}


void PriceMark::clearall(){
	pDnBidMark.clear();
	pUpAskMark.clear();
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
				auto itp = pDnBidMark.find(wbid);
				if(itp != pDnBidMark.end()){
					/*
					if(itp->second != fprice){
						pDnBidMark[wbid] = fprice;
						bidkey = wbid;
						bidprice = fprice;
						return true;
					}else{
						return false;
					}*/
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
			}

		}
		if(wbid < pwbid && fprice < pfprice){
			if(pwbid > 0 && pfprice>0){
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
				pDnBidMark[pwbid] = pfprice;
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
				auto itp = pDnBidMark.find(wbid);
				if(itp != pDnBidMark.end()){
					/*
					if(itp->second != fprice){
						pDnBidMark[wbid] = fprice;
						bidkey = wbid;
						bidprice = fprice;
						return true;
					}else{
						return false;
					}*/
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
			}
		}
		if(wbid < pwbid && fprice > pfprice){
			if(wbid > 0 && pfprice>0){
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



				pDnBidMark[pwbid] = pfprice;
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
	return false;
}

bool PriceMark::updateAsk(unsigned long long wask, unsigned long long  pwask, unsigned long long  fprice, unsigned long long  pfprice){
	if(pwtype == BULL ){
		if(wask > pwask && fprice > pfprice){
			if(pwask > 0 && pfprice>0){

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
				pUpAskMark[pwask] = pfprice;
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

				auto itp = pUpAskMark.find(wask);
				if(itp != pUpAskMark.end()){
					/*
					if(itp->second != fprice){
						pUpAskMark[wask] = fprice;
						askkey = wask;
						askprice = fprice;
						return true;
					}else{
						return false;
					}*/
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
			}


		}
	}
	if(pwtype == BEAR ){
		if(wask > pwask && fprice < pfprice){
			if(pwask > 0 && fprice>0){
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
				pUpAskMark[pwask] = pfprice;
				askkey = pwask;
				askprice = pfprice;

				unsigned long long spread = spreadTable.getSpread("01", pfprice - 1llu);
				unsigned long long refbid = pfprice-spread;
				string ukey = to_string(refbid) +"-"+ to_string(pfprice)  ;
				updateTableAsk(ukey, pwask);

				return true;
			}
		}
		if(wask < pwask && fprice > pfprice){

			if(pfprice > 0 && wask>0){

				auto itp = pUpAskMark.find(wask);
				if(itp != pUpAskMark.end()){
					/*
					if(itp->second != fprice){
						pUpAskMark[wask] = fprice;
						askkey = wask;
						askprice = fprice;
						return true;
					}else{
						return false;
					}*/
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
			}

		}
	}
	return false;
}

unsigned long long PriceMark::buyIn(unsigned long long wprice){
	//string swask = to_string(wprice);
	unsigned long long buyInUpAsk = 0;
	auto aup = pUpAskMark.find(wprice);
	if(aup != pUpAskMark.end()){
		buyInUpAsk = pUpAskMark[wprice];
	}
	return buyInUpAsk;
}

unsigned long long PriceMark::sellOut(unsigned long long wprice){
	//string swbid = to_string(wprice);
	unsigned long long sellOutDnBid = 99999999;
	auto bdn = pDnBidMark.find(wprice);
	if(bdn != pDnBidMark.end()){
		sellOutDnBid = pDnBidMark[wprice];
	}
	return sellOutDnBid;
}

unsigned long long PriceMark::getIssuerIize(){
	return pIssuerSize;
}

map<unsigned long long,unsigned long long> PriceMark::getBidTable(){
	return pDnBidMark;
}

map<unsigned long long,unsigned long long> PriceMark::getAskTable(){
	return pUpAskMark;
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



