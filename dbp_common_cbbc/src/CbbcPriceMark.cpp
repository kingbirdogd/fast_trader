/*
 * SimplePriceMark.cpp
 *
 *  Created on: 2017¦~5¤ë22¤é
 *      Author: leo
 */

#include "CbbcPriceMark.h"









CbbcPriceMark::CbbcPriceMark(int wtype, unsigned long long spread, unsigned long long uspread){
	pwtype = wtype;
	pDefaultSpread = spread;
	pUSpread = uspread;
}

CbbcPriceMark::~CbbcPriceMark() {
	// TODO Auto-generated destructor stub
}


void CbbcPriceMark::clearall(){
	pDnBidMark.clear();
	pUpAskMark.clear();
}

string CbbcPriceMark::ftos(float value, int dp){
	stringstream ss;
	ss << fixed << setprecision(dp) << value;
	string mystring = ss.str();
	return mystring;
}

vector<string> CbbcPriceMark::split(string str, char delimiter) {
  vector<string> internal;
  stringstream ss(str); // Turn the string into a stream.
  string tok;

  while(getline(ss, tok, delimiter)) {
    internal.push_back(tok);
  }
  return internal;
}

int CbbcPriceMark::getWType(){
	return pwtype;
}

unsigned long long CbbcPriceMark::getBidKey(){
	return bidkey;
}
unsigned long long CbbcPriceMark::getAskKey(){
	return askkey;
}
unsigned long long CbbcPriceMark::getBidPrice(){
	return bidprice;
}
unsigned long long CbbcPriceMark::getAskPrice(){
	return askprice;
}



//bool CbbcPriceMark::updateBid(float wbid, float pwbid, int fbid, int pfbid){
bool CbbcPriceMark::updateBid(unsigned long long wbid, unsigned long long pwbid, unsigned long long fprice, unsigned long long  pfprice){

	if(pwtype == BULL){
		if(wbid > pwbid && fprice > pfprice){
			if(pwbid > 0){

				unsigned long long spread = pDefaultSpread;
				unsigned long long pplus1 = wbid + spread;

				bool eq1 = (fprice - pfprice) == pUSpread;
				if(pplus1 == wbid && eq1){

					auto itp = pDnBidMark.find(wbid);
					if(itp != pDnBidMark.end()){
						return false;
					}

					pDnBidMark[wbid] = fprice;
					bidkey = wbid;
					bidprice = fprice;

					return true;
				}
			}
		}
		if(wbid < pwbid && fprice < pfprice){
			if(pwbid > 0){
				/*
				unsigned long long spread = pDefaultSpread;

				unsigned long long pplus1 = wbid + spread;

				bool eq1 = (pfprice - fprice) == pUSpread;
				if(pplus1 == pwbid && eq1){

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

					return true;
				}
				*/
				pDnBidMark[pwbid] = pfprice;
				bidkey = pwbid;
				bidprice = pfprice;
				return true;
			}
		}
	}
	if(pwtype == BEAR){
		if(wbid > pwbid && fprice < pfprice){
			if(pwbid > 0){

				unsigned long long spread = pDefaultSpread;

				unsigned long long pplus1 = pwbid + spread;

				bool eq1 = (pfprice - fprice) == pUSpread;
				if(pplus1 == wbid && eq1){

					auto itp = pDnBidMark.find(wbid);
					if(itp != pDnBidMark.end()){

						return false;
					}


					pDnBidMark[wbid] = fprice;
					bidkey = wbid;
					bidprice = fprice;

					return true;
				}
			}
		}
		if(wbid < pwbid && fprice > pfprice){
			if(pwbid > 0){
				/*
				unsigned long long spread = pDefaultSpread;
				unsigned long long pplus1 = wbid + spread;

				bool eq1 = (fprice - pfprice) == pUSpread;
				if(pplus1 == pwbid && eq1){


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

					return true;
				}*/
				pDnBidMark[pwbid] = pfprice;
				bidkey = pwbid;
				bidprice = pfprice;

				return true;
			}
		}
	}
	return false;
}

bool CbbcPriceMark::updateAsk(unsigned long long wask, unsigned long long  pwask, unsigned long long  fprice, unsigned long long  pfprice){
	if(pwtype == BULL ){
		if(wask > pwask && fprice > pfprice){
			if(pwask > 0){
				/*
				unsigned long long spread = pDefaultSpread;
				unsigned long long pplus1 = pwask + spread;

				bool eq1 = (fprice - pfprice) == pUSpread;
				if(pplus1 == wask && eq1){

					if(pwask > 0){


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

						return true;
					}
				}*/
				pUpAskMark[pwask] = pfprice;
				askkey = pwask;
				askprice = pfprice;

				return true;
			}
		}
		if(wask < pwask && fprice < pfprice){
			if(pwask > 0){
				unsigned long long spread = pDefaultSpread;
				unsigned long long pplus1 = wask + spread;

				bool eq1 = (pfprice - fprice) == pUSpread;
				if(pplus1 == pwask && eq1){

					if(fprice > 0){

						auto itp = pUpAskMark.find(wask);
						if(itp != pUpAskMark.end()){

							return false;
						}

						pUpAskMark[wask] = fprice;
						askkey = wask;
						askprice = pfprice;

						return true;
					}
				}
			}
		}
	}
	if(pwtype == BEAR ){
		if(wask > pwask && fprice < pfprice){
			if(pwask > 0){
				/*
				unsigned long long spread = pDefaultSpread;
				unsigned long long pplus1 = pwask + spread;

				bool eq1 = (pfprice - fprice) == pUSpread;
				if(pplus1 == wask && eq1){

					if(pwask > 0){

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

						return true;
					}
				}*/
				pUpAskMark[pwask] = pfprice;
				askkey = pwask;
				askprice = pfprice;

				return true;
			}
		}
		if(wask < pwask && fprice > pfprice){
			if(pwask > 0){
				unsigned long long spread = pDefaultSpread;
				unsigned long long pplus1 = wask + spread;

				bool eq1 = (fprice - pfprice) == pUSpread;
				if(pplus1 == pwask && eq1){

					if(fprice > 0){

						auto itp = pUpAskMark.find(wask);
						if(itp != pUpAskMark.end()){

							return false;
						}

						pUpAskMark[wask] = fprice;
						askkey = pwask;
						askprice = fprice;

						return true;
					}
				}
			}
		}
	}
	return false;
}


bool CbbcPriceMark::updateBid(unsigned long long wbid, unsigned long long pwbid, unsigned long long fprice, unsigned long long  pfprice, bool ignoreF){

	if(pwtype == BULL){
		if(wbid > pwbid && fprice > pfprice){
			if(pwbid > 0){

				unsigned long long spread = pDefaultSpread;
				unsigned long long pplus1 = wbid + spread;

				bool eq1 = ((fprice - pfprice) == pUSpread)|ignoreF;
				if(pplus1 == wbid && eq1){

					auto itp = pDnBidMark.find(wbid);
					if(itp != pDnBidMark.end()){

						return false;
					}


					pDnBidMark[wbid] = fprice;
					bidkey = wbid;
					bidprice = fprice;

					return true;
				}
			}
		}
		if(wbid < pwbid && fprice < pfprice){
			if(pwbid > 0){

				unsigned long long spread = pDefaultSpread;

				unsigned long long pplus1 = wbid + spread;

				bool eq1 = ((pfprice - fprice) == pUSpread)|ignoreF;
				if(pplus1 == pwbid && eq1){

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

					return true;
				}
			}
		}
	}
	if(pwtype == BEAR){
		if(wbid > pwbid && fprice < pfprice){
			if(pwbid > 0){

				unsigned long long spread = pDefaultSpread;

				unsigned long long pplus1 = pwbid + spread;

				bool eq1 = ((pfprice - fprice) == pUSpread)|ignoreF;
				if(pplus1 == wbid && eq1){


					auto itp = pDnBidMark.find(wbid);
					if(itp != pDnBidMark.end()){

						return false;
					}


					pDnBidMark[wbid] = fprice;
					bidkey = wbid;
					bidprice = fprice;

					return true;
				}
			}
		}
		if(wbid < pwbid && fprice > pfprice){
			if(pwbid > 0){
				//float spread = spreadTable.getSpread("01", wbid);
				unsigned long long spread = pDefaultSpread;
				unsigned long long pplus1 = wbid + spread;

				bool eq1 = ((fprice - pfprice) == pUSpread)|ignoreF;
				if(pplus1 == pwbid && eq1){


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

					return true;
				}
			}
		}
	}
	return false;
}

bool CbbcPriceMark::updateAsk(unsigned long long wask, unsigned long long  pwask, unsigned long long  fprice, unsigned long long  pfprice, bool ignoreF){
	if(pwtype == BULL ){
		if(wask > pwask && fprice > pfprice){
			if(pwask > 0){
				unsigned long long spread = pDefaultSpread;
				unsigned long long pplus1 = pwask + spread;

				bool eq1 = ((fprice - pfprice) == pUSpread)|ignoreF;
				if(pplus1 == wask && eq1){

					if(pwask > 0){


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

						return true;
					}
				}
			}
		}
		if(wask < pwask && fprice < pfprice){
			if(pwask > 0){
				unsigned long long spread = pDefaultSpread;
				unsigned long long pplus1 = wask + spread;

				bool eq1 = ((pfprice - fprice) == pUSpread)|ignoreF;
				if(pplus1 == pwask && eq1){

					if(fprice > 0){

						auto itp = pUpAskMark.find(wask);
						if(itp != pUpAskMark.end()){

							return false;
						}

						pUpAskMark[wask] = fprice;
						askkey = wask;
						askprice = pfprice;

						return true;
					}
				}
			}
		}
	}
	if(pwtype == BEAR ){
		if(wask > pwask && fprice < pfprice){
			if(pwask > 0){
				unsigned long long spread = pDefaultSpread;
				unsigned long long pplus1 = pwask + spread;

				bool eq1 = ((pfprice - fprice) == pUSpread)|ignoreF;
				if(pplus1 == wask && eq1){

					if(pwask > 0){

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

						return true;
					}
				}
			}
		}
		if(wask < pwask && fprice > pfprice){
			if(pwask > 0){
				unsigned long long spread = pDefaultSpread;
				unsigned long long pplus1 = wask + spread;

				bool eq1 = ((fprice - pfprice) == pUSpread)|ignoreF;
				if(pplus1 == pwask && eq1){

					if(fprice > 0){

						auto itp = pUpAskMark.find(wask);
						if(itp != pUpAskMark.end()){

							return false;
						}

						pUpAskMark[wask] = fprice;
						askkey = pwask;
						askprice = fprice;

						return true;
					}
				}
			}
		}
	}
	return false;
}

unsigned long long CbbcPriceMark::buyIn(unsigned long long wprice){
	//string swask = to_string(wprice);
	unsigned long long buyInUpAsk = 0;
	auto aup = pUpAskMark.find(wprice);
	if(aup != pUpAskMark.end()){
		buyInUpAsk = pUpAskMark[wprice];
	}
	return buyInUpAsk;
}

unsigned long long CbbcPriceMark::sellOut(unsigned long long wprice){
	//string swbid = to_string(wprice);
	unsigned long long sellOutDnBid = 99999999;
	auto bdn = pDnBidMark.find(wprice);
	if(bdn != pDnBidMark.end()){
		sellOutDnBid = pDnBidMark[wprice];
	}
	return sellOutDnBid;
}

unsigned long long CbbcPriceMark::getIssuerIize(){
	return pIssuerSize;
}

map<unsigned long long,unsigned long long> CbbcPriceMark::getBidTable(){
	return pDnBidMark;
}

map<unsigned long long,unsigned long long> CbbcPriceMark::getAskTable(){
	return pUpAskMark;
}



