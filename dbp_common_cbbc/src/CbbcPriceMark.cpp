/*
 * SimplePriceMark.cpp
 *
 *  Created on: 2017¦~5¤ë22¤é
 *      Author: leo
 */

#include "CbbcPriceMark.h"


CbbcPriceMark::CbbcPriceMark(int pType, int wtype, unsigned long long spread, unsigned long long uspread){
	pwtype = wtype;
	pDefaultSpread = spread;
	pUSpread = uspread;
	ppType = pType;
}

CbbcPriceMark::~CbbcPriceMark() {
	// TODO Auto-generated destructor stub
}


void CbbcPriceMark::clearall(){

	pBidMark.clear();
	pAskMark.clear();

	pDnBidMark.clear();
	pUpAskMark.clear();

	pUpBidMark.clear();
	pDnAskMark.clear();
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


bool CbbcPriceMark::updateTableBid(unsigned long long wbid, unsigned long long pwbid, unsigned long long fbid, unsigned long long pfbid){
	if(INDEX_TYPE == ppType){
		return updateBid(wbid, pwbid, fbid, pfbid);
	}
	if(STOCK_TYPE == ppType){
		//return updateThBid(wbid, pwbid, fbid, pfbid);
		return updateBid(wbid, pwbid, fbid, pfbid);
	}
	return false;
}

bool CbbcPriceMark::updateTableAsk(unsigned long long wask, unsigned long long pwask, unsigned long long fask, unsigned long long pfask){
	if(INDEX_TYPE == ppType){
		return updateAsk(wask, pwask, fask, pfask);
	}
	if(STOCK_TYPE == ppType){
		//return updateThAsk(wask, pwask, fask, pfask);
		return updateAsk(wask, pwask, fask, pfask);
	}
	return false;
}


bool CbbcPriceMark::updateThBid(unsigned long long wbid, unsigned long long pwbid, unsigned long long fprice, unsigned long long  pfprice){

	if(pwtype == BULL){
		if(wbid > pwbid && fprice > pfprice){
			if(pwbid > 0){

				pBidMark[wbid] = fprice;
				bidkey = wbid;
				bidprice = fprice;

				return true;

			}
		}
		if(wbid < pwbid && fprice < pfprice){
			if(pwbid > 0){

				pBidMark[pwbid] = pfprice;
				bidkey = pwbid;
				bidprice = pfprice;

				return true;
			}
		}
	}
	if(pwtype == BEAR){
		if(wbid > pwbid && fprice < pfprice){
			if(pwbid > 0){

				pBidMark[wbid] = fprice;
				bidkey = wbid;
				bidprice = fprice;

				return true;
			}
		}
		if(wbid < pwbid && fprice > pfprice){
			if(pwbid > 0){

				pBidMark[pwbid] = pfprice;
				bidkey = pwbid;
				bidprice = pfprice;

				return true;
			}
		}
	}
	return false;
}

bool CbbcPriceMark::updateThAsk(unsigned long long wask, unsigned long long  pwask, unsigned long long  fprice, unsigned long long  pfprice){
	if(pwtype == BULL ){
		if(wask > pwask && fprice > pfprice){
			if(pwask > 0){

				pAskMark[pwask] = pfprice;
				askkey = pwask;
				askprice = pfprice;

				return true;
			}
		}
		if(wask < pwask && fprice < pfprice){
			if(pwask > 0){

				pAskMark[wask] = fprice;
				askkey = wask;
				askprice = fprice;

				return true;
			}
		}
	}
	if(pwtype == BEAR ){
		if(wask > pwask && fprice < pfprice){
			if(pwask > 0){

				pAskMark[pwask] = pfprice;
				askkey = pwask;
				askprice = pfprice;

				return true;
			}
		}
		if(wask < pwask && fprice > pfprice){
			if(pwask > 0){

				pAskMark[pwask] = fprice;
				askkey = pwask;
				askprice = fprice;

				return true;
			}
		}
	}
	return false;
}

//bool CbbcPriceMark::updateBid(float wbid, float pwbid, int fbid, int pfbid){
bool CbbcPriceMark::updateBid(unsigned long long wbid, unsigned long long pwbid, unsigned long long fprice, unsigned long long  pfprice){

	if(pwtype == BULL){
		if(wbid > pwbid && fprice > pfprice){
			if(pwbid > 0){

				unsigned long long spread = pDefaultSpread;
				unsigned long long pplus1 = pwbid + spread;

				bool eq1 = (fprice - pfprice) == pUSpread;
				if(pplus1 == wbid && eq1){

					pUpBidMark[wbid] = fprice;

					auto itp = pBidMark.find(wbid);
					if(itp != pBidMark.end()){
						return false;
					}

					pBidMark[wbid] = fprice;
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

				pBidMark[pwbid] = pfprice;
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

					pUpBidMark[wbid] = fprice;

					auto itp = pBidMark.find(wbid);
					if(itp != pBidMark.end()){

						return false;
					}


					pBidMark[wbid] = fprice;
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

				pBidMark[pwbid] = pfprice;
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

				pAskMark[pwask] = pfprice;
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

						//auto itp = pDnAskMark.find(wask);
						pDnAskMark[wask] = fprice;

						auto itp = pAskMark.find(wask);
						if(itp != pAskMark.end()){

							return false;
						}

						pAskMark[wask] = fprice;
						askkey = wask;
						askprice = fprice;

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

				pAskMark[pwask] = pfprice;
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

						pDnAskMark[wask] = fprice;

						auto itp = pAskMark.find(wask);
						if(itp != pAskMark.end()){

							return false;
						}
/*
						pUpAskMark[pwask] = fprice;
						askkey = pwask;
						askprice = fprice;
*/
						pAskMark[wask] = fprice;
						askkey = wask;
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


					auto itp = pBidMark.find(wbid);
					if(itp != pBidMark.end()){

						return false;
					}


					pBidMark[wbid] = fprice;
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

					auto itp = pBidMark.find(pwbid);
					if(itp != pBidMark.end()){
						if(itp->second != pfprice){

							pBidMark[pwbid] = pfprice;
							bidkey = pwbid;
							bidprice = pfprice;
							return true;
						}else{

							return false;
						}
					}

					pBidMark[pwbid] = pfprice;
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


					auto itp = pBidMark.find(wbid);
					if(itp != pBidMark.end()){

						return false;
					}


					pBidMark[wbid] = fprice;
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


					auto itp = pBidMark.find(pwbid);
					if(itp != pBidMark.end()){
						if(itp->second != pfprice){

							pBidMark[pwbid] = pfprice;
							bidkey = pwbid;
							bidprice = pfprice;
							return true;
						}else{

							return false;
						}
					}

					pBidMark[pwbid] = pfprice;
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


						auto itp = pAskMark.find(pwask);
						if(itp != pAskMark.end()){
							if(itp->second != pfprice){

								pAskMark[pwask] = pfprice;
								askkey = pwask;
								askprice = pfprice;
								return true;
							}else{

								return false;
							}
						}

						pAskMark[pwask] = pfprice;
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

						auto itp = pAskMark.find(wask);
						if(itp != pAskMark.end()){

							return false;
						}

						pAskMark[wask] = fprice;
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

						auto itp = pAskMark.find(pwask);
						if(itp != pAskMark.end()){
							if(itp->second != pfprice){

								pAskMark[pwask] = pfprice;
								askkey = pwask;
								askprice = pfprice;
								return true;
							}else{

								return false;
							}
						}

						pAskMark[pwask] = pfprice;
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

						auto itp = pAskMark.find(wask);
						if(itp != pAskMark.end()){

							return false;
						}

						pAskMark[wask] = fprice;
						askkey = wask;
						askprice = fprice;

						return true;
					}
				}
			}
		}
	}
	return false;
}

unsigned long long CbbcPriceMark::getSensitivity(){
	return sensitivity;
}

unsigned long long CbbcPriceMark::buyIn(unsigned long long wprice){

	if(sensitivity > 0){
		return dynameicBuyIn(wprice);
	}

	//string swask = to_string(wprice);
	unsigned long long buyInUpAsk = 0;
	auto aup = pAskMark.find(wprice);
	if(aup != pAskMark.end()){
		buyInUpAsk = pAskMark[wprice];
	}
	return buyInUpAsk;
}

unsigned long long CbbcPriceMark::sellOut(unsigned long long wprice){

	if(sensitivity > 0){
		return dynameicSellOut(wprice);
	}

	//string swbid = to_string(wprice);
	unsigned long long sellOutDnBid = 99999999;
	auto bdn = pBidMark.find(wprice);
	if(bdn != pBidMark.end()){
		sellOutDnBid = pBidMark[wprice];
	}
	return sellOutDnBid;
}
/*
unsigned long long CbbcPriceMark::level(unsigned long long wprice){

	if(sensitivity > 0){
		return dynameicLevel(wprice);
	}

	//string swbid = to_string(wprice);
	unsigned long long sellOutDnBid = 99999999;
	auto bdn = pBidMark.find(wprice);
	if(bdn != pBidMark.end()){
		sellOutDnBid = pBidMark[wprice];
	}
	return sellOutDnBid;
}*/

unsigned long long CbbcPriceMark::dynameicBuyIn(unsigned long long wprice){

	if(sensitivity == 0)
		return 0;

	if(pwtype == BULL ){
		if(wprice > askkey && askkey > 0){

			unsigned long long v = wprice - askkey;
			int no_of_spread = static_cast<int>(v / pDefaultSpread);

			return askprice + no_of_spread*sensitivity;


			//return askprice + sensitivity;
		}
		if(wprice < askkey && askkey > 0){

			unsigned long long v = askkey - wprice;
			int no_of_spread = static_cast<int>(v / pDefaultSpread);

			return askprice - no_of_spread*sensitivity;
			//return askprice - sensitivity;
		}
		return askprice;
	}
	if(pwtype == BEAR ){
		if(wprice > askkey && askkey > 0){

			unsigned long long v = wprice - askkey;
			int no_of_spread = static_cast<int>(v / pDefaultSpread);

			return askprice - no_of_spread*sensitivity;


			//return askprice - sensitivity;
		}
		if(wprice < askkey && askkey > 0){

			unsigned long long v = askkey - wprice;
			int no_of_spread = static_cast<int>(v / pDefaultSpread);

			return askprice + no_of_spread*sensitivity;


			//return askprice + sensitivity;
		}
		return askprice;
	}
	return 0;
}

unsigned long long CbbcPriceMark::dynameicSellOut(unsigned long long wprice){

	if(sensitivity == 0)
		return 99999999;

	if(pwtype == BULL ){
		if(wprice > bidkey && bidkey > 0){

			unsigned long long v = wprice - bidkey;
			int no_of_spread = static_cast<int>(v / pDefaultSpread);

			return bidprice + no_of_spread*sensitivity;
			//return bidprice - sensitivity;
		}
		if(wprice < bidkey && bidkey > 0){

			unsigned long long v = bidkey - wprice;
			int no_of_spread = static_cast<int>(v / pDefaultSpread);

			return bidprice - no_of_spread*sensitivity;

			//return bidprice + sensitivity;
		}
		return bidprice;
	}
	if(pwtype == BEAR ){
		if(wprice > bidkey && bidkey > 0){

			unsigned long long v = wprice - bidkey;
			int no_of_spread = static_cast<int>(v / pDefaultSpread);

			return bidprice - no_of_spread*sensitivity;


			//return bidprice - sensitivity;
		}
		if(wprice < bidkey && bidkey > 0){

			unsigned long long v = bidkey - wprice;
			int no_of_spread = static_cast<int>(v / pDefaultSpread);

			return bidprice + no_of_spread*sensitivity;


			//return bidprice + sensitivity;
		}
		return bidprice;
	}
	return 0;
}
/*
unsigned long long CbbcPriceMark::dynameicLevel(unsigned long long wprice){

	if(sensitivity == 0)
		return 99999999;

	if(pwtype == BULL ){
		if(wprice > bidkey && bidkey > 0){

			unsigned long long v = wprice - bidkey;
			int no_of_spread = static_cast<int>(v / pDefaultSpread);

			return bidprice + no_of_spread*sensitivity + pUSpread;
			//return bidprice - sensitivity;
		}
		if(wprice < bidkey && bidkey > 0){

			unsigned long long v = bidkey - wprice;
			int no_of_spread = static_cast<int>(v / pDefaultSpread);

			return bidprice - no_of_spread*sensitivity + pUSpread;

			//return bidprice + sensitivity;
		}
		return bidprice + pUSpread;
	}
	if(pwtype == BEAR ){
		if(wprice > bidkey && bidkey > 0){

			unsigned long long v = wprice - bidkey;
			int no_of_spread = static_cast<int>(v / pDefaultSpread);

			return bidprice - no_of_spread*sensitivity - pUSpread;


			//return bidprice - sensitivity;
		}
		if(wprice < bidkey && bidkey > 0){

			unsigned long long v = bidkey - wprice;
			int no_of_spread = static_cast<int>(v / pDefaultSpread);

			return bidprice + no_of_spread*sensitivity- pUSpread;


			//return bidprice + sensitivity;
		}
		return bidprice - pUSpread;
	}
	return 0;
}*/

unsigned long long CbbcPriceMark::getIssuerIize(){
	return pIssuerSize;
}

map<unsigned long long,unsigned long long> CbbcPriceMark::getBidTable(){
	return pBidMark;
}

map<unsigned long long,unsigned long long> CbbcPriceMark::getAskTable(){
	return pAskMark;
}

map<unsigned long long,unsigned long long> CbbcPriceMark::getDnBidTable(){
	return pDnBidMark;
}

map<unsigned long long,unsigned long long> CbbcPriceMark::getUpAskTable(){
	return pUpAskMark;
}

map<unsigned long long,unsigned long long> CbbcPriceMark::getUpBidTable(){
	return pUpBidMark;
}

map<unsigned long long,unsigned long long> CbbcPriceMark::getDnAskTable(){
	return pDnAskMark;
}


void CbbcPriceMark::copyTable(map<unsigned long long,unsigned long long> bid,  map<unsigned long long,unsigned long long> ask){
	pBidMark.insert(bid.begin(), bid.end());
	pAskMark.insert(ask.begin(), ask.end());
}
void CbbcPriceMark::copyUpTable(map<unsigned long long,unsigned long long> bid,  map<unsigned long long,unsigned long long> ask){
	pUpBidMark.insert(bid.begin(), bid.end());
	pUpAskMark.insert(ask.begin(), ask.end());
}
void CbbcPriceMark::copyDnTable(map<unsigned long long,unsigned long long> bid,  map<unsigned long long,unsigned long long> ask){
	pDnBidMark.insert(bid.begin(), bid.end());
	pDnAskMark.insert(ask.begin(), ask.end());
}

void CbbcPriceMark::setSensitivity(unsigned long long value){
	sensitivity = value;
}


