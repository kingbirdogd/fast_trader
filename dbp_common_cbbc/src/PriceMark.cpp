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



//bool PriceMark::updateBid(float wbid, float pwbid, int fbid, int pfbid){
bool PriceMark::updateBid(unsigned long long wbid, unsigned long long pwbid, unsigned long long fprice, unsigned long long  pfprice){

	if(pwtype == BULL){
		if(wbid > pwbid && fprice > pfprice){
			if(pwbid > 0){

				auto itp = pDnBidMark.find(wbid);
				if(itp != pDnBidMark.end()){
					if(itp->second != fprice){
						//fprintf(stderr, "BULL BID MARK CHANGE \n");
						pDnBidMark[wbid] = fprice;
						bidkey = wbid;
						bidprice = fprice;
						return true;
					}else{
						//fprintf(stderr, "BULL BID MARK NO CHANGE \n");
						return false;
					}
				}

				//fprintf(stderr, "BULL BID MARK NEW ENTRY \n");
				pDnBidMark[wbid] = fprice;
				bidkey = wbid;
				bidprice = fprice;

				return true;
			}

		}
		if(wbid < pwbid && fprice < pfprice){
			if(pwbid > 0){
				auto itp = pDnBidMark.find(pwbid);
				if(itp != pDnBidMark.end()){
					if(itp->second != pfprice){
						//fprintf(stderr, "BULL BID MARK CHANGE \n");
						pDnBidMark[pwbid] = pfprice;
						bidkey = pwbid;
						bidprice = pfprice;
						return true;
					}else{
						//fprintf(stderr, "BULL BID MARK NO CHANGE \n");
						return false;
					}
				}

				//fprintf(stderr, "BULL BID MARK NEW ENTRY \n");
				pDnBidMark[pwbid] = pfprice;
				bidkey = pwbid;
				bidprice = pfprice;
				//algoActionInterface->showLog("Bid Table");
				//printTable(pDnBidMark);
				return true;
			}
		}
	}
	if(pwtype == BEAR){
		if(wbid > pwbid && fprice < pfprice){
			if(pwbid > 0){
				auto itp = pDnBidMark.find(wbid);
				if(itp != pDnBidMark.end()){
					if(itp->second != fprice){
						//fprintf(stderr, "BEAR BID MARK CHANGE \n");
						pDnBidMark[wbid] = fprice;
						bidkey = wbid;
						bidprice = fprice;
						return true;
					}else{
						//fprintf(stderr, "BEAR BID MARK NO CHANGE \n");
						return false;
					}
				}
				//fprintf(stderr, "BULL BID MARK NEW ENTRY \n");

				pDnBidMark[wbid] = fprice;
				bidkey = wbid;
				bidprice = fprice;
				//algoActionInterface->showLog("Bid Table");
				//printTable(pDnBidMark);
				return true;
			}
		}
		if(wbid < pwbid && fprice > pfprice){
			if(pwbid > 0){
				auto itp = pDnBidMark.find(pwbid);
				if(itp != pDnBidMark.end()){
					if(itp->second != pfprice){
						//fprintf(stderr, "BEAR BID MARK CHANGE \n");
						pDnBidMark[pwbid] = pfprice;
						bidkey = pwbid;
						bidprice = pfprice;
						return true;
					}else{
						//fprintf(stderr, "BEAR BID MARK NO CHANGE \n");
						return false;
					}
				}
				//fprintf(stderr, "BEAR BID MARK NEW ENTRY \n");
				pDnBidMark[pwbid] = pfprice;
				bidkey = pwbid;
				bidprice = pfprice;
				//algoActionInterface->showLog("Bid Table");
				//printTable(pDnBidMark);
				return true;
			}
		}
	}
	return false;
}

bool PriceMark::updateAsk(unsigned long long wask, unsigned long long  pwask, unsigned long long  fprice, unsigned long long  pfprice){
	if(pwtype == BULL ){
		if(wask > pwask && fprice > pfprice){
			if(pwask > 0){

				auto itp = pUpAskMark.find(pwask);
				if(itp != pUpAskMark.end()){
					if(itp->second != pfprice){
						//fprintf(stderr, "BULL ASK MARK CHANGE \n");
						pUpAskMark[pwask] = pfprice;
						askkey = pwask;
						askprice = pfprice;
						return true;
					}else{
						//fprintf(stderr, "BULL ASK MARK NO CHANGE \n");
						return false;
					}
				}
				//fprintf(stderr, "BULL ASK MARK NEW ENTRY \n");

				pUpAskMark[pwask] = pfprice;
				askkey = pwask;
				askprice = pfprice;
				//algoActionInterface->showLog("Ask Table");
				//printTable(pUpAskMark);
				return true;
			}
		}
		if(wask < pwask && fprice < pfprice){
			if(pwask > 0){
				if(fprice > 0){

					auto itp = pUpAskMark.find(wask);
					if(itp != pUpAskMark.end()){
						if(itp->second != fprice){
							//fprintf(stderr, "BULL ASK MARK CHANGE \n");
							pUpAskMark[wask] = fprice;
							askkey = wask;
							askprice = fprice;
							return true;
						}else{
							//fprintf(stderr, "BULL ASK MARK NO CHANGE \n");
							return false;
						}
					}
					//fprintf(stderr, "BULL ASK MARK NEW ENTRY \n");
					pUpAskMark[wask] = fprice;
					askkey = wask;
					askprice = pfprice;
					//algoActionInterface->showLog("Ask Table");
					//printTable(pUpAskMark);
					return true;
				}
			}

		}
	}
	if(pwtype == BEAR ){
		if(wask > pwask && fprice < pfprice){
			if(pwask > 0){
				auto itp = pUpAskMark.find(pwask);
				if(itp != pUpAskMark.end()){
					if(itp->second != pfprice){
						//fprintf(stderr, "BEAR ASK MARK CHANGE \n");
						pUpAskMark[pwask] = pfprice;
						askkey = pwask;
						askprice = pfprice;
						return true;
					}else{
						//fprintf(stderr, "BEAR ASK MARK NO CHANGE \n");
						return false;
					}
				}
				//fprintf(stderr, "BEAR ASK MARK NEW ENTRY \n");
				pUpAskMark[pwask] = pfprice;
				askkey = pwask;
				askprice = pfprice;
				//algoActionInterface->showLog("Ask Table");
				//printTable(pUpAskMark);
				return true;
			}
		}
		if(wask < pwask && fprice > pfprice){
			if(pwask > 0){

				if(fprice > 0){

					auto itp = pUpAskMark.find(wask);
					if(itp != pUpAskMark.end()){
						if(itp->second != fprice){
							//fprintf(stderr, "BEAR ASK MARK CHANGE \n");
							pUpAskMark[wask] = fprice;
							askkey = wask;
							askprice = fprice;
							return true;
						}else{
							//fprintf(stderr, "BEAR ASK MARK NO CHANGE \n");
							return false;
						}
					}
					//fprintf(stderr, "BEAR ASK MARK NEW ENTRY \n");
					pUpAskMark[wask] = fprice;
					askkey = pwask;
					askprice = fprice;
					//algoActionInterface->showLog("Ask Table");
					//printTable(pUpAskMark);
					return true;
				}
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



