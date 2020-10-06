#ifndef CPP_ALGOS_DBP_BEAR_INC_BEAR_HPP_
#define CPP_ALGOS_DBP_BEAR_INC_BEAR_HPP_

#include <string>
#include <unordered_map>
#include <unordered_set>
#include <DateUtil.h>
#include <time.h>
#include <tools.h>
#include <algo.hpp>
#include <global_memory.hpp>
#include <CbbcPriceMark.h>
#include <OBSetting.h>
#include <AlgoEngineData.h>
#include <rapid_ring/ring_buffer_object_poll.hpp>
#include "ThreadLogger.h"


class bear : public algo
{
private:
	class pair;
private:
	std::unordered_map<unsigned int, priceinfo*> uprice_map;
	std::unordered_map<std::string, priceinfo*> rprice_map;
	using order_map = std::unordered_map<unsigned long long, pair*>;
	using mdw_map = std::unordered_map<unsigned int, pair*>;
	using w_ref_map = std::unordered_map<unsigned int, std::string>;
	using md_map = std::unordered_map<unsigned int, std::unordered_set<pair*>>;
	using rmd_map = std::unordered_map<std::string, std::unordered_set<pair*>>;
	using inout_map = std::map<unsigned long long, std::unordered_set<pair*>>;
	friend class pair;
private:
	order_map _o_map;
	md_map _u_map;
	rmd_map _ru_map;
	mdw_map _w_map;
	md_map _u_position_map;
	w_ref_map _w_ref_map;

	mutable inout_map buyin_map;
	mutable inout_map sellout_map;
	ThreadLogger* logger;
private:
	class pair
	{
	public:
		bear* _algo;
		std::string _Symbol;
		unsigned int _Underlying_code;
		unsigned long long _IssuerSize;
		unsigned long long _Quantity;
		unsigned long long _SPREAD;
		OBSetting* _OBSetting;
		priceinfo* _PriceInfo;
		priceinfo* _PriceInfoU;
		CbbcPriceMark* _CbbcPriceMark;
		unsigned int _warrant_code;
		int _Wtype = -1;
		int _Utype = -1;
		std::string _Ref;
		int _Win_Tick = 0;
		int _Stop_Lost = 0;
		int _Action_Status = -1;
		int _Status = STATUS_READY;
		int _INOUT = 0;
		int _LVLRANGE = 0;
		int _PTRANGE = 0;

 
	public:
		pair
		(
		):
			_algo(nullptr),
			_Symbol(""),
			_Underlying_code(0),
			_IssuerSize(0),
			_Quantity(0),
			_SPREAD(0),
			_OBSetting(nullptr),
			_PriceInfo(nullptr),
			_PriceInfoU(nullptr),
			_CbbcPriceMark(nullptr),
			_warrant_code(0),
			_Wtype(0),
			_Utype(0),
			_Ref("")
		{
		}
		pair
		(
			bear* algo,
			std::string Symbol,
			unsigned int Underlying_code,
			unsigned long long IssuerSize,
			unsigned long long Quantity,
			unsigned long long Spread,
			OBSetting* obs,
			priceinfo* pe,
			priceinfo* peu,
			CbbcPriceMark* pricemark,
			unsigned int warrant_code,
			int wtype,
			int utype,
			std::string Ref
		):
			_algo(algo),
			_Symbol(Symbol),
			_Underlying_code(Underlying_code),
			_IssuerSize(IssuerSize),
			_Quantity(Quantity),
			_SPREAD(Spread),
			_OBSetting(obs),
			_PriceInfo(pe),
			_PriceInfoU(peu),
			_CbbcPriceMark(pricemark),
			_warrant_code(warrant_code),
			_Wtype(wtype),
			_Utype(utype),
			_Ref(Ref)
		{
		}
		json to_json() const
		{
			json j;
			j["symbol"] = _Symbol;
			j["underlying_code"] = _Underlying_code;
			j["warrant_code"] = _warrant_code;
			j["issuer_size"] = _IssuerSize;
			j["buy_quantity"] = _Quantity;
			j["spread"] = _SPREAD;
			j["wtype"] = _Wtype;
			return j;
		}
		~pair() = default;
		pair(const pair&) = default;
		pair(pair&&) = default;
		pair& operator= (const pair&) = default;
		pair& operator= (pair&&) = default;
	public:
	public:
		void init(){
			_Status = STATUS_READY;
			_OBSetting->BuyIn = 0;
			_OBSetting->SellOut = 99999999;
		}
		unsigned long long default_buy_price()
		{
			unsigned long long price = 0;
			auto it = omdcMap.find(_warrant_code);
			if (omdcMap.end() != it)
			{
				auto& tradable = it->second;
				if (0 != tradable.m_Ask[0].m_iPrice)
				{
					price = static_cast<unsigned long long>(tradable.m_Ask[0].m_iPrice) * 100000;
				}
			}
			return price;
		}
		unsigned long long default_sell_price()
		{
			unsigned long long price = 0;
			auto it = omdcMap.find(_warrant_code);
			if (omdcMap.end() != it)
			{
				auto& tradable = it->second;
				if (0 != tradable.m_Bid[0].m_iPrice)
				{
					price = static_cast<unsigned long long>(tradable.m_Bid[0].m_iPrice) * 100000;
				}
			}
			return price;
		}
		OBSetting* getOBS(){
			return _OBSetting;
		}
		priceinfo* getPE(){
			return _PriceInfo;
		}
		std::string getRef(){
			return _Ref;
		}
		unsigned long long getSellOut()
		{
			return _OBSetting->SellOut;
		}
		unsigned long long getBuyIn()
		{
			return _OBSetting->BuyIn;
		}
		unsigned long long getLvlBid()
		{
			return _OBSetting->LvLBid;
		}
		void on_bear_trade(const Tradable& tradable)
		{

			time_t currentTime = DateUtil::getCurrentSystemTime();

			auto trade_price = static_cast<unsigned long long>(tradable.m_LastTradePrice) * 100000;

			Log(std::string(" CODE = ") + std::to_string(_warrant_code) +  " on_trade" + " Status = " + std::to_string(_Status) + " Bestbid = " + std::to_string(_PriceInfo->Bestbid)  + " Bestask = " + std::to_string(_PriceInfo->Bestask) + " Trade price = " +  std::to_string(trade_price) );

			if(BUY_ORDER == tradable.m_TradeSide){

				if(_Status == STATUS_DONE && (currentTime > (_OBSetting->TradeTime + 1))){
					_Status = STATUS_READY;
				}

				if(_Status == STATUS_READY && _Action_Status == STAGE_START){

					if(_OBSetting->SellOut != 99999999 && _OBSetting->BuyIn != 0 && trade_price == _OBSetting->BuyIn){
						Log(std::string(" CODE = ") + std::to_string(_warrant_code) +  " _INOUT=" + std::to_string(_INOUT) +  " _LVLRANGE=" + std::to_string(_LVLRANGE) +  " _PTRANGE=" + std::to_string(_PTRANGE) );
						Log(std::string(" CODE = ") + std::to_string(_warrant_code) +  " BuyIn=" + std::to_string(_OBSetting->BuyIn) +  " SellOut=" + std::to_string(_OBSetting->SellOut) +  " LvlBid=" + std::to_string(_OBSetting->LvLBid) );
						if(_INOUT > 0){
							unsigned long long cal_inout = _OBSetting->SellOut - _OBSetting->BuyIn;
							unsigned long long mak_inout =  static_cast<unsigned long long>(_INOUT) * 100000;
							Log(std::string(" CODE = ") + std::to_string(_warrant_code) +  " InoutRange" + " cal_inout = " + std::to_string(cal_inout) +  + " cal_inout = " + std::to_string(mak_inout)  );
							if(cal_inout < mak_inout || cal_inout > 99999999){
								Log(std::string(" CODE = ") + std::to_string(_warrant_code) +  " InoutRange Not Pass");

								auto msg = algo_validate_msg_pool.get_obj();
								msg->al = _algo;
								msg->algo_name = _algo->_name;
								msg->id = _algo->_u.get_id();
								msg->ref = _Ref;
								msg->warrant_code = _warrant_code;
								msg->side = "ASK";
								msg->wprice = _PriceInfo->Bestask;
								msg->buyin = _OBSetting->BuyIn;
								msg->sellout = _OBSetting->SellOut;
								msg->action = "inoutrule";
								ouputQueue.enqueue(msg);

								return;
							}
						}
						if(_LVLRANGE > 0){
							unsigned long long cal_lvlrange = abs(static_cast<long long>(_OBSetting->LvLBid) - static_cast<long long>(_OBSetting->BuyIn));
							unsigned long long mak_lvlrange =  static_cast<unsigned long long>(_LVLRANGE) * 100000;
							Log(std::string(" CODE = ") + std::to_string(_warrant_code) +  " _LVLRANGE" + " cal_lvlrange = " + std::to_string(cal_lvlrange) +  + " mak_lvlrange = " + std::to_string(mak_lvlrange)  );
							if(cal_lvlrange >  mak_lvlrange || cal_lvlrange > 99999999){
								Log(std::string(" CODE = ") + std::to_string(_warrant_code) +  " LVLRANGE Not Pass");

								auto msg = algo_validate_msg_pool.get_obj();
								msg->al = _algo;
								msg->algo_name = _algo->_name;
								msg->id = _algo->_u.get_id();
								msg->ref = _Ref;
								msg->warrant_code = _warrant_code;
								msg->side = "ASK";
								msg->wprice = _PriceInfo->Bestask;
								msg->buyin = _OBSetting->BuyIn;
								msg->lvlbid = _OBSetting->LvLBid;
								msg->action = "lvlrangerule";
								ouputQueue.enqueue(msg);

								return;
							}
						}
						if(_PTRANGE > 0){
							unsigned long long prev_buyin = _CbbcPriceMark->buyIn(_PriceInfo->Bestask - _SPREAD);

							Log(std::string(" CODE = ") + std::to_string(_warrant_code) +  " Previous BuyIn=" + std::to_string(prev_buyin));

							long long cal_ptrange = static_cast<long long>(prev_buyin) - static_cast<long long>(_OBSetting->BuyIn) ;
							//unsigned long long cal_ptrange = _OBSetting->BuyIn - prev_buyin;
							long long mak_ptrange =  static_cast<long long>(_PTRANGE) * 100000;
							Log(std::string(" CODE = ") + std::to_string(_warrant_code) +  " _PTRANGE" + " cal_ptrange = " + std::to_string(cal_ptrange) +  + " mak_ptrange = " + std::to_string(mak_ptrange)  );
							if(cal_ptrange <  mak_ptrange  && _OBSetting->BuyIn != 99999999 && prev_buyin != 99999999){
								Log(std::string(" CODE = ") + std::to_string(_warrant_code) +  " PTRANGE Not Pass");

								auto msg = algo_validate_msg_pool.get_obj();
								msg->al = _algo;
								msg->algo_name = _algo->_name;
								msg->id = _algo->_u.get_id();
								msg->ref = _Ref;
								msg->warrant_code = _warrant_code;
								msg->side = "ASK";
								msg->wprice = _PriceInfo->Bestask;
								msg->buyin = _OBSetting->BuyIn;
								msg->pbuyin = prev_buyin;
								msg->action = "ptrangerule";
								ouputQueue.enqueue(msg);

								return;
							}
						}

						unsigned long long refSpread = _SPREAD;
						unsigned long long b = _PriceInfo->Bestbid;
						unsigned long long a = _PriceInfo->Bestask;
						unsigned long long b1 = b + refSpread;

						//bool fallback =  _PriceInfo->PBestask > _PriceInfo->Bestask;


						bool within1spread = (b1 - a) == 0;

						//if(within1spread && _OBSetting->SellOut != 99999999 && _OBSetting->BuyIn && fallback){
						if(within1spread && _OBSetting->SellOut != 99999999 && _OBSetting->BuyIn){

							Log(DateUtil::getCurrentTime() + std::string(" CODE = ") + std::to_string(_warrant_code) +  " Do Buy " );

							warrant* newWarrant = new warrant;
							newWarrant->Date = DateUtil::getToday();
							newWarrant->Code = _warrant_code;
							newWarrant->Status = STATUS_PENDING;
							newWarrant->BuyPrice = _PriceInfo->Bestask;
							newWarrant->Quantity = 0;
							newWarrant->UCode = _Underlying_code;
							newWarrant->DAsk = _PriceInfoU->FBestask;
							newWarrant->BuyQty = _Quantity;
							newWarrant->Name = std::to_string(_warrant_code);
							newWarrant->BuyIn = _OBSetting->BuyIn;
							newWarrant->SellOut = _OBSetting->SellOut;
							newWarrant->LvlBid = _OBSetting->LvLBid;

							_OBSetting->addWarrantOrCbbc(newWarrant);

							doBuy(newWarrant);
						}
					}
				}
			}

			if(SELL_ORDER == tradable.m_TradeSide){
				//if(_Status == STATUS_AVAILABLE && (_Action_Status == STAGE_START || _Win_Tick > 0) ){
				if(_Status == STATUS_AVAILABLE && _Action_Status == STAGE_START && trade_price == _OBSetting->SellOut){

					unsigned long long refSpread = _SPREAD;
					unsigned long long b = _PriceInfo->Bestbid;
					unsigned long long a = _PriceInfo->Bestask;
					unsigned long long b1 = b + refSpread;

					bool within1spread = (b1 - a) == 0;


					warrant* newWarrant = _OBSetting->getRelatedWarrant(_warrant_code);

					if(within1spread){

						Log(DateUtil::getCurrentTime() + std::string(" CODE = ") + std::to_string(_warrant_code) +  " Normal Do Sell " );

							newWarrant->DBid = trade_price;

							newWarrant->Status = STATUS_SELLING;
							newWarrant->SellPrice = _PriceInfo->Bestbid;
							newWarrant->SellQty = newWarrant->Quantity;
							newWarrant->SellOut = _OBSetting->SellOut;

							_Status = STATUS_SELLING;

							doSell(newWarrant);
						//}
					}else{
						Log(std::string(" CODE = ") + std::to_string(_warrant_code) +  " Spread Width > 1 =  " + std::to_string(b) + " | " + std::to_string(a) );
					}
				}
			}

		}

		void on_bull_trade(const Tradable& tradable)
		{


			time_t currentTime = DateUtil::getCurrentSystemTime();

			auto trade_price = static_cast<unsigned long long>(tradable.m_LastTradePrice) * 100000;

			Log(std::string(" CODE = ") + std::to_string(_warrant_code) +  " on_trade" + " Status = " + std::to_string(_Status) + " Bestbid = " + std::to_string(_PriceInfo->Bestbid)  + " Bestask = " + std::to_string(_PriceInfo->Bestask));

			if(SELL_ORDER == tradable.m_TradeSide ){

				if(_Status == STATUS_DONE && (currentTime > (_OBSetting->TradeTime + 3))){
					_Status = STATUS_READY;
				}

				if(_Status == STATUS_READY && _Action_Status == STAGE_START){

					if(_OBSetting->SellOut != 99999999 && _OBSetting->BuyIn != 0){
						Log(std::string(" CODE = ") + std::to_string(_warrant_code) +  " _INOUT=" + std::to_string(_INOUT) +  " _LVLRANGE=" + std::to_string(_LVLRANGE) +  " _PTRANGE=" + std::to_string(_PTRANGE) );
						Log(std::string(" CODE = ") + std::to_string(_warrant_code) +  " BuyIn=" + std::to_string(_OBSetting->BuyIn) +  " SellOut=" + std::to_string(_OBSetting->SellOut) +  " LvlBid=" + std::to_string(_OBSetting->LvLBid) );
						if(_INOUT > 0){
							unsigned long long cal_inout = _OBSetting->BuyIn - _OBSetting->SellOut;
							unsigned long long mak_inout =  static_cast<unsigned long long>(_INOUT) * 100000;
							Log(std::string(" CODE = ") + std::to_string(_warrant_code) +  " InoutRange" + " cal_inout = " + std::to_string(cal_inout) +  + " mak_inout = " + std::to_string(mak_inout)  );
							if(cal_inout < mak_inout || cal_inout > 99999999){
								Log(std::string(" CODE = ") + std::to_string(_warrant_code) +  " InoutRange Not Pass");

								auto msg = algo_validate_msg_pool.get_obj();
								msg->al = _algo;
								msg->algo_name = _algo->_name;
								msg->id = _algo->_u.get_id();
								msg->ref = _Ref;
								msg->warrant_code = _warrant_code;
								msg->side = "ASK";
								msg->wprice = _PriceInfo->Bestask;
								msg->buyin = _OBSetting->BuyIn;
								msg->sellout = _OBSetting->SellOut;
								msg->action = "inoutrule";
								ouputQueue.enqueue(msg);

								return;
							}
						}
						if(_LVLRANGE > 0){

							unsigned long long cal_lvlrange = abs(static_cast<long long>(_OBSetting->LvLBid) - static_cast<long long>(_OBSetting->BuyIn));
							unsigned long long mak_lvlrange =  static_cast<unsigned long long>(_LVLRANGE) * 100000;

							Log(std::string(" CODE = ") + std::to_string(_warrant_code) +  " _LVLRANGE" + " cal_lvlrange = " + std::to_string(cal_lvlrange) +  + " mak_lvlrange = " + std::to_string(mak_lvlrange)  );
							if(cal_lvlrange > mak_lvlrange || cal_lvlrange > 99999999){
								Log(std::string(" CODE = ") + std::to_string(_warrant_code) +  " _LVLRANGE Not Pass");

								auto msg = algo_validate_msg_pool.get_obj();
								msg->al = _algo;
								msg->algo_name = _algo->_name;
								msg->id = _algo->_u.get_id();
								msg->ref = _Ref;
								msg->warrant_code = _warrant_code;
								msg->side = "ASK";
								msg->wprice = _PriceInfo->Bestask;
								msg->buyin = _OBSetting->BuyIn;
								msg->lvlbid = _OBSetting->LvLBid;
								msg->action = "lvlrangerule";
								ouputQueue.enqueue(msg);

								return;
							}
						}
						if(_PTRANGE > 0){
							unsigned long long prev_buyin = _CbbcPriceMark->buyIn(_PriceInfo->Bestask - _SPREAD);
							long long cal_ptrange = static_cast<long long>(_OBSetting->BuyIn) - static_cast<long long>(prev_buyin);
							long long mak_ptrange =  static_cast<long long>(_PTRANGE) * 100000;

							Log(std::string(" CODE = ") + std::to_string(_warrant_code) +  " Previous BuyIn=" + std::to_string(prev_buyin));

							Log(std::string(" CODE = ") + std::to_string(_warrant_code) +  " _PTRANGE" + " cal_ptrange = " + std::to_string(cal_ptrange) +  + " mak_ptrange = " + std::to_string(mak_ptrange)  );
							//if(cal_ptrange >= mak_ptrange || cal_ptrange>99999999){
							if(cal_ptrange <  mak_ptrange  && _OBSetting->BuyIn != 99999999 && prev_buyin != 99999999){
								Log(std::string(" CODE = ") + std::to_string(_warrant_code) +  " _PTRANGE Not Pass");

								auto msg = algo_validate_msg_pool.get_obj();
								msg->al = _algo;
								msg->algo_name = _algo->_name;
								msg->id = _algo->_u.get_id();
								msg->ref = _Ref;
								msg->warrant_code = _warrant_code;
								msg->side = "ASK";
								msg->wprice = _PriceInfo->Bestask;
								msg->buyin = _OBSetting->BuyIn;
								msg->pbuyin = prev_buyin;
								msg->action = "ptrangerule";
								ouputQueue.enqueue(msg);

								return;
							}
						}



						unsigned long long refSpread = _SPREAD;
						unsigned long long b = _PriceInfo->Bestbid;
						unsigned long long a = _PriceInfo->Bestask;
						unsigned long long b1 = b + refSpread;
						//bool fallback =  _PriceInfo->PBestask > _PriceInfo->Bestask;

						bool within1spread = (b1 - a) == 0;

						//if(within1spread && _OBSetting->SellOut != 99999999 && _OBSetting->BuyIn != 0 && fallback){
						if(within1spread && _OBSetting->SellOut != 99999999 && _OBSetting->BuyIn != 0){



							Log(DateUtil::getCurrentTime() + std::string(" CODE = ") + std::to_string(_warrant_code) +  " Do Buy" );

							warrant* newWarrant = new warrant;
							newWarrant->Date = DateUtil::getToday();
							newWarrant->Code = _warrant_code;
							newWarrant->Status = STATUS_PENDING;
							newWarrant->BuyPrice = _PriceInfo->Bestask;
							newWarrant->Quantity = 0;
							newWarrant->UCode = _Underlying_code;
							newWarrant->DAsk = _PriceInfoU->FBestask;
							newWarrant->BuyQty = _Quantity;
							newWarrant->Name = std::to_string(_warrant_code);
							newWarrant->BuyIn = _OBSetting->BuyIn;
							newWarrant->SellOut = _OBSetting->SellOut;
							newWarrant->LvlBid = _OBSetting->LvLBid;

							_OBSetting->addWarrantOrCbbc(newWarrant);

							doBuy(newWarrant);
						}
					}
				}
			}

			if(BUY_ORDER == tradable.m_TradeSide){
				//if(_Status == STATUS_AVAILABLE && (_Action_Status == STAGE_START || _Win_Tick > 0) ){
				if(_Status == STATUS_AVAILABLE && _Action_Status == STAGE_START  && trade_price == _OBSetting->SellOut){

					unsigned long long refSpread = _SPREAD;
					unsigned long long b = _PriceInfo->Bestbid;
					unsigned long long a = _PriceInfo->Bestask;
					unsigned long long b1 = b + refSpread;

					bool within1spread = (b1 - a) == 0;

					//if(within1spread || _OBSetting->BidAskSpread == 0){
					warrant* newWarrant = _OBSetting->getRelatedWarrant(_warrant_code);
					//long long diff = static_cast<long long>(_PriceInfo->Bestbid) - static_cast<long long>(newWarrant->BuyPrice);

					//if(within1spread || diff >= 0){
					if(within1spread){

						Log(DateUtil::getCurrentTime() + std::string(" CODE = ") + std::to_string(_warrant_code) +  " Normal Do Sell " );

						//warrant* newWarrant = _OBSetting->getRelatedWarrant(_warrant_code);

						//long long diff = static_cast<unsigned long long>(newWarrant->BuyPrice) - static_cast<unsigned long long>(_PriceInfo->Bestbid);
/*
						bool atgtsl = false;
						if(_Stop_Lost > 0 && diff>0){
							int noofticks =  diff/_SPREAD;
							if(noofticks >= _Stop_Lost ){
								atgtsl = true;
							}
						}
						if(_Stop_Lost > 0 && diff==0){
							atgtsl = true;
						}

						if(atgtsl || _Stop_Lost == 0){
						*/
							newWarrant->DBid = trade_price;

							newWarrant->Status = STATUS_SELLING;
							newWarrant->SellPrice = _PriceInfo->Bestbid;
							newWarrant->SellQty = newWarrant->Quantity;
							newWarrant->SellOut = _OBSetting->SellOut;

							_Status = STATUS_SELLING;

							doSell(newWarrant);
						//}
					}else{
						Log(std::string(" CODE = ") + std::to_string(_warrant_code) +  " Spread Width > 1 =  " + std::to_string(b) + " | " + std::to_string(a) );
					}
				}
			}

		}

		void on_tcp_book(const Tradable& tradable){
			std::string code = tradable.m_TcpCode;
			if(code != _Symbol){

			}

		}

		void on_omdd_book(const Tradable& tradable){
			unsigned int code = tradable.m_Code;

			if(code != _Underlying_code)
				return;

			//auto best_bid_qty = tradable.m_Bid[0].m_uQuantity;
//			auto best_bid_price = static_cast<unsigned long long>(tradable.m_Bid[0].m_iPrice) * 100000;

			//auto best_ask_qty = tradable.m_Ask[0].m_uQuantity;
//			auto best_ask_price = static_cast<unsigned long long>(tradable.m_Ask[0].m_iPrice) * 100000;

			priceinfo* uprice = _algo->uprice_map[_Underlying_code];

			_PriceInfoU->FBestbid =  uprice->FBestbid;
			_PriceInfoU->PFBestbid =  uprice->PFBestbid;
			_PriceInfoU->FBestask =  uprice->FBestask;
			_PriceInfoU->PFBestask =  uprice->PFBestask;





			//if(_Status == STATUS_AVAILABLE  && _Action_Status == STAGE_START && (_Stop_Lost > 0 || _Win_Tick >= 0)) {

		//	warrant* warrant = _OBSetting->getRelatedWarrant(_warrant_code);
		//	if(warrant == nullptr)
		//		return;

			if(_Status == STATUS_AVAILABLE  && _Action_Status == STAGE_START && _Win_Tick >= 0 ) {

				unsigned long long wbestbid = default_sell_price();
				warrant* warrant = _OBSetting->getRelatedWarrant(_warrant_code);
				if(warrant == nullptr)
					return;

/*
				if(_Stop_Lost > 0 && _PriceInfo->Bestbid > 0){

					warrant* warrant = _OBSetting->getRelatedWarrant(_warrant_code);

					long long diff = static_cast<long long>(warrant->BuyPrice) - static_cast<long long>(_PriceInfo->Bestbid);
					long long rwinPrice =  static_cast<long long>(_SPREAD * _Stop_Lost);
					if(diff >= rwinPrice){
						Log(std::string(" CODE = ") + std::to_string(code) + " SEll Stoplost > 0 " + to_string(_PriceInfo->Bestbid) );

						warrant->SellPrice = _PriceInfo->Bestbid;
						warrant->Status = STATUS_SELLING;
						warrant->SellQty = warrant->Quantity;

						if(_OBSetting->SellOut == 99999999){
							warrant->SellOut = _PriceInfoU->FBestbid;
						}else{
							warrant->SellOut = _OBSetting->SellOut;
						}
						_Status = STATUS_SELLING;
						doSell(warrant);
					}
				}
*/
				if(_Win_Tick >= 0 && wbestbid>0 && warrant->BuyPrice > 0){

					unsigned long long refSpread = _SPREAD;
					unsigned long long rwinPrice = (unsigned long long) (warrant->BuyPrice + _Win_Tick * refSpread);

					unsigned long long bp = warrant->BuyPrice;

					if(_Win_Tick == 0){
						if(wbestbid >= bp){
							Log(std::string(" CODE = ") + std::to_string(_warrant_code) + " SEll Win Tick = 0 " + to_string(wbestbid) + " Warrant Buy Price = " + to_string(warrant->BuyPrice) );
							warrant->SellPrice = wbestbid;
							warrant->Status = STATUS_SELLING;
							warrant->SellQty = warrant->Quantity;

							if(_OBSetting->SellOut == 99999999){
								warrant->SellOut = _PriceInfoU->FBestbid;
							}else{
								warrant->SellOut = _OBSetting->SellOut;
							}
							_Status = STATUS_SELLING;
							doSell(warrant);
						}
					}else{

						//if(_PriceInfo->Bestbid >= rwinPrice){
						if(wbestbid >= rwinPrice  ){
							Log(std::string(" CODE = ") + std::to_string(_warrant_code) + " SEll Win Tick > 0 " + to_string(wbestbid) + " Warrant Buy Price = " + to_string(warrant->BuyPrice) );
							warrant->SellPrice = wbestbid;
							warrant->SellQty = warrant->Quantity;
							warrant->Status = STATUS_SELLING;
							if(_OBSetting->SellOut == 99999999){
								warrant->SellOut = _PriceInfoU->FBestbid;
							}else{
								warrant->SellOut = _OBSetting->SellOut;
							}
							_Status = STATUS_SELLING;

							doSell(warrant);

						}
					}
				}
			}

/*
			if(_Status == STATUS_AVAILABLE  && _Action_Status == STAGE_START){
				if(_Wtype == 2){
					if(_PriceInfoU->LFBestask < best_ask_price &&  best_ask_price == _OBSetting->SellOut ){
						Log(std::string("Quick Sell  CODE = ") + std::to_string(_warrant_code) + " PFBestAsk = " + std::to_string(_PriceInfoU->PFBestask) +  + " FBestAsk = " + std::to_string(_PriceInfoU->FBestask));
						warrant* warrant = _OBSetting->getRelatedWarrant(_warrant_code);
						warrant->SellPrice = _PriceInfo->Bestbid;
						warrant->Status = STATUS_SELLING;
						warrant->SellQty = warrant->Quantity;

						if(_OBSetting->SellOut == 99999999){
							warrant->SellOut = best_ask_price;
						}else{
							warrant->SellOut = _OBSetting->SellOut;
						}
						_Status = STATUS_SELLING;
						doSell(warrant);
					}

				}else{
					if(_PriceInfoU->PFBestbid > best_bid_price &&  best_bid_price == _OBSetting->SellOut ){
						Log(std::string("Quick Sell CODE = ") + std::to_string(_warrant_code) + " PFBestBid = " + std::to_string(_PriceInfoU->PFBestbid) +  + " FBestBid = " + std::to_string(_PriceInfoU->FBestbid));
						warrant* warrant = _OBSetting->getRelatedWarrant(_warrant_code);
						warrant->SellPrice = _PriceInfo->Bestbid;
						warrant->Status = STATUS_SELLING;
						warrant->SellQty = warrant->Quantity;

						if(_OBSetting->SellOut == 99999999){
							warrant->SellOut = best_bid_price;
						}else{
							warrant->SellOut = _OBSetting->SellOut;
						}
						_Status = STATUS_SELLING;
						doSell(warrant);
					}

				}
			}
*/
		}

		void on_bull_book(const Tradable& tradable)
		{



			unsigned int code = tradable.m_Code;

			auto best_bid_qty = tradable.m_Bid[0].m_uQuantity;
			auto best_bid_price = static_cast<unsigned long long>(tradable.m_Bid[0].m_iPrice) * 100000;

			auto best_ask_qty = tradable.m_Ask[0].m_uQuantity;
			auto best_ask_price = static_cast<unsigned long long>(tradable.m_Ask[0].m_iPrice) * 100000;


			//_algo->log_info(std::string(" CODE = ") + std::to_string(code) );


			if(code == _Underlying_code){

/*

				if(_PriceInfoU->FBestbid != best_bid_price){
					_PriceInfoU->PFBestbid = _PriceInfoU->FBestbid;
					_PriceInfoU->FBestbid = best_bid_price;
				}
				if(_PriceInfoU->FBestask != best_ask_price ){
					_PriceInfoU->PFBestask = _PriceInfoU->FBestask;
					_PriceInfoU->FBestask = best_ask_price;
				}

				if(_Status == STATUS_AVAILABLE  && _Action_Status == STAGE_START){

					if(_PriceInfoU->PFBestbid > _PriceInfoU->FBestbid &&  _PriceInfoU->FBestbid == _OBSetting->SellOut ){
						Log(DateUtil::getCurrentTime() + std::string("Quick Sell CODE = ") + std::to_string(_warrant_code) + " PFBestBid = " + std::to_string(_PriceInfoU->PFBestbid) +  + " FBestBid = " + std::to_string(_PriceInfoU->FBestbid));
						warrant* warrant = _OBSetting->getRelatedWarrant(_warrant_code);
						warrant->SellPrice = _PriceInfo->Bestbid;
						warrant->Status = STATUS_SELLING;
						warrant->SellQty = warrant->Quantity;

						if(_OBSetting->SellOut == 99999999){
							warrant->SellOut = _PriceInfoU->FBestbid;
						}else{
							warrant->SellOut = _OBSetting->SellOut;
						}
						_Status = STATUS_SELLING;
						doSell(warrant);
					}
				}

				_algo->log_info(std::string(" UCODE ") + std::to_string(code) +
				std::string(" CODE ") + std::to_string(_warrant_code) +
				std::string(" bid = ") + std::to_string(best_bid_price) +
				std::string(" ask = ") + std::to_string(best_ask_price) +
				std::string(" best_bid_qty = ") + std::to_string(best_bid_qty) +
				std::string(" best_ask_qty = ") + std::to_string(best_ask_qty)
								);
*/


				return;
			}else if(code == _warrant_code){


				priceinfo* uprice = _algo->uprice_map[_Underlying_code];


				_PriceInfoU->FBestbid =  uprice->FBestbid;
				_PriceInfoU->PFBestbid =  uprice->PFBestbid;
				_PriceInfoU->FBestask =  uprice->FBestask;
				_PriceInfoU->PFBestask =  uprice->PFBestask;

				_PriceInfoU->BidSeq =  uprice->BidSeq;
				_PriceInfoU->AskSeq =  uprice->AskSeq;

/*
				_PriceInfoU->FBestbid =  uprice->TBestbid;
				_PriceInfoU->PFBestbid =  uprice->PTBestbid;
				_PriceInfoU->FBestask =  uprice->TBestask;
				_PriceInfoU->PFBestask =  uprice->PTBestask;
*/
				/*
				Log(std::string(" WCODE ") + std::to_string(code) +
				std::string(" CODE ") + std::to_string(_warrant_code) +
				std::string(" bid = ") + std::to_string(best_bid_price) +
				std::string(" ask = ") + std::to_string(best_ask_price) +
				std::string(" p bid = ") + std::to_string(_PriceInfo->Bestbid) +
				std::string(" p ask = ") + std::to_string(_PriceInfo->Bestask) +
				std::string(" Issuer Size = ") + std::to_string(_IssuerSize) +
				std::string(" best_bid_qty = ") + std::to_string(best_bid_qty) +
				std::string(" best_ask_qty = ") + std::to_string(best_ask_qty) +
				std::string(" FBestbid = ") + std::to_string(_PriceInfoU->FBestbid) +
				std::string(" FBestask = ") + std::to_string(_PriceInfoU->FBestask) +
				std::string(" PFBestbid = ") + std::to_string(_PriceInfoU->PFBestbid) +
				std::string(" PFBestask = ") + std::to_string(_PriceInfoU->PFBestask)
								);
*/

				unsigned long buyin = _CbbcPriceMark->buyIn(best_ask_price);
				unsigned long long sellout = _CbbcPriceMark->sellOut(best_bid_price);
				unsigned long long lvlBid = _CbbcPriceMark->sellOut(best_ask_price);

				_OBSetting->BuyIn = buyin;
				_OBSetting->SellOut = sellout;
				_OBSetting->LvLBid = lvlBid;

				if(_Status == STATUS_AVAILABLE  && _Action_Status == STAGE_START && _PriceInfo->LBestbid != best_bid_price && best_bid_price > 0 && best_bid_price > _PriceInfo->LBestbid){
					warrant* warrant = _OBSetting->getRelatedWarrant(_warrant_code);
					if(warrant->BuyPrice > _PriceInfo->LBestbid && warrant->BuyPrice > best_bid_price){
						Log(std::string(" CODE = ") + std::to_string(code) + " SEll on_bull_book Raise back sell " );
						warrant->SellPrice = best_bid_price;
						warrant->Status = STATUS_SELLING;
						warrant->SellQty = warrant->Quantity;

						if(_OBSetting->SellOut == 99999999){
							warrant->SellOut = _PriceInfoU->FBestbid;
						}else{
							warrant->SellOut = _OBSetting->SellOut;
						}
						_Status = STATUS_SELLING;
						doSell(warrant);
					}
				}

				if(_Status == STATUS_AVAILABLE  && _Action_Status == STAGE_START && _Stop_Lost > 0 && best_bid_qty >= _IssuerSize){
					warrant* warrant = _OBSetting->getRelatedWarrant(_warrant_code);

					long long diff = static_cast<long long>(warrant->BuyPrice) - static_cast<long long>(best_bid_price);
					long long rwinPrice =  static_cast<long long>(_SPREAD * _Stop_Lost);
					if(diff >= rwinPrice ){
						Log(std::string(" CODE = ") + std::to_string(code) + " SEll Stoplost > 0   Buy Price = " +   to_string(best_bid_price) + " --- Best Bid" +  to_string(best_bid_price)  + " Diff = " + to_string(diff) + "rwinPrice = " + to_string(rwinPrice) );

						warrant->SellPrice = best_bid_price;
						warrant->Status = STATUS_SELLING;
						warrant->SellQty = warrant->Quantity;

						if(_OBSetting->SellOut == 99999999){
							warrant->SellOut = _PriceInfoU->FBestbid;
						}else{
							warrant->SellOut = _OBSetting->SellOut;
						}
						_Status = STATUS_SELLING;
						doSell(warrant);
					}
				}

				if(_Status == STATUS_AVAILABLE  && _Action_Status == STAGE_START && _Win_Tick >= 0){

					warrant* warrant = _OBSetting->getRelatedWarrant(_warrant_code);

					unsigned long long refSpread = _SPREAD;
					unsigned long long rwinPrice = (unsigned long long) (warrant->BuyPrice + _Win_Tick * refSpread);

					unsigned long long bp = warrant->BuyPrice;

					if(_Win_Tick == 0){
						if(best_bid_price >= bp && bp > 0){
							Log(std::string(" CODE = ") + std::to_string(code) + " on bull book SEll Win Tick = 0 " + to_string(best_bid_price)  + " Warrant Buy Price = " + to_string(warrant->BuyPrice)  );
							warrant->SellPrice = best_bid_price;
							warrant->Status = STATUS_SELLING;
							warrant->SellQty = warrant->Quantity;

							if(_OBSetting->SellOut == 99999999){
								warrant->SellOut = _PriceInfoU->FBestbid;
							}else{
								warrant->SellOut = _OBSetting->SellOut;
							}
							_Status = STATUS_SELLING;
							doSell(warrant);
						}
					}else{

						if(best_bid_price >= rwinPrice && bp > 0){
							Log(std::string(" CODE = ") + std::to_string(code) + " on bull book SEll Win Tick > 0 " + to_string(best_bid_price)  + " Warrant Buy Price = " + to_string(warrant->BuyPrice)  );
							warrant->SellPrice = best_bid_price;
							warrant->SellQty = warrant->Quantity;
							warrant->Status = STATUS_SELLING;
							if(_OBSetting->SellOut == 99999999){
								warrant->SellOut = _PriceInfoU->FBestbid;
							}else{
								warrant->SellOut = _OBSetting->SellOut;
							}
							_Status = STATUS_SELLING;

							doSell(warrant);

						}
					}
				}



				//if(_PriceInfo->Bestbid != best_bid_price && best_bid_price > 0 && best_bid_qty >= _IssuerSize){
				if(_PriceInfo->Bestbid != best_bid_price && best_bid_price > 0){
					if(best_bid_qty >= _IssuerSize){

						if(_PriceInfo->LastBidSeq != _PriceInfoU->BidSeq){
							_PriceInfo->LastBidSeq = _PriceInfoU->BidSeq;
							//_algo->log_info(std::string(" WCODE ") + std::to_string(code) + " DO Mark BID");
							bool hasUpdate = _CbbcPriceMark->updateBid(best_bid_price, _PriceInfo->Bestbid, _PriceInfoU->FBestbid, _PriceInfoU->PFBestbid);
							if(hasUpdate){
								unsigned long long bkey = _CbbcPriceMark->getBidKey();
								unsigned long long bprice = _CbbcPriceMark->getBidPrice();
								if(bkey > 0 && bprice>0){

								auto msg = algo_pricetable_msg_pool.get_obj();
								msg->al = _algo;
								msg->algo_name = _algo->_name;
								msg->id = _algo->_u.get_id();
								msg->ref = _Ref;
								msg->warrant_code = _warrant_code;
								msg->side = "BID";
								msg->wkey = bkey;
								msg->fprice = bprice;
								ouputQueue.enqueue(msg);
								}
							}

							sellout = _CbbcPriceMark->sellOut(best_bid_price);
							lvlBid = _CbbcPriceMark->sellOut(best_ask_price);

							if(sellout != _OBSetting->SellOut && _Action_Status != STAGE_STOP){

								_OBSetting->SellOut = sellout;
								_OBSetting->LvLBid = lvlBid;
								_OBSetting->RefWBid = best_bid_price;
								_OBSetting->RefWAsk = best_ask_price;

							}
						}

					}

					if(best_bid_qty >= _IssuerSize){
						Log(std::string(" CODE = ") + std::to_string(code) + " Bid Change from " + to_string(_PriceInfo->Bestbid) + " to " + to_string(best_bid_price));
						_PriceInfo->PBestbid = _PriceInfo->Bestbid;
						_PriceInfo->Bestbid = best_bid_price;
					}


					auto msg = algo_warrantprice_msg_pool.get_obj();
					msg->al = _algo;
					msg->algo_name = _algo->_name;
					msg->id = _algo->_u.get_id();
					msg->ref = _Ref;
					msg->warrant_code = _warrant_code;
					msg->side = "BID";
					msg->wprice = best_bid_price;
					ouputQueue.enqueue(msg);
					//_algo->log_info(std::string(" CODE = ") + std::to_string(code) + " SellOut = " + to_string(sellout) );
				}

				//if(_PriceInfo->Bestask != best_ask_price && best_ask_price > 0 && best_ask_qty >= _IssuerSize){
				if(_PriceInfo->Bestask != best_ask_price && best_ask_price > 0){
					//_algo->log_info(std::string(" WCODE ") + std::to_string(code) + " DO Mark ASK");
					if(best_ask_qty >= _IssuerSize){

						if(_PriceInfo->LastAskSeq != _PriceInfoU->AskSeq){

							_PriceInfo->LastAskSeq = _PriceInfoU->AskSeq;
							bool hasUpdate = _CbbcPriceMark->updateAsk(best_ask_price, _PriceInfo->Bestask, _PriceInfoU->FBestask, _PriceInfoU->PFBestask);
							if(hasUpdate){
								unsigned long long bkey = _CbbcPriceMark->getAskKey();
								unsigned long long bprice = _CbbcPriceMark->getAskPrice();
								if(bkey > 0 && bprice>0){


								auto msg = algo_pricetable_msg_pool.get_obj();
								msg->al = _algo;
								msg->algo_name = _algo->_name;
								msg->id = _algo->_u.get_id();
								msg->ref = _Ref;
								msg->warrant_code = _warrant_code;
								msg->side = "ASK";
								msg->wkey = bkey;
								msg->fprice = bprice;
								ouputQueue.enqueue(msg);

								}
							}

							buyin = _CbbcPriceMark->buyIn(best_ask_price);
							lvlBid = _CbbcPriceMark->sellOut(best_ask_price);

							_OBSetting->RefWAsk = best_ask_price;

							if(buyin != _OBSetting->BuyIn && _Action_Status != STAGE_STOP){
								_OBSetting->BuyIn = buyin;
								_OBSetting->LvLBid = lvlBid;
							}
						}

					}
					if(best_ask_qty >= _IssuerSize){

						Log(std::string(" CODE = ") + std::to_string(code) + " Ask Change from " + to_string(_PriceInfo->Bestask) + " to " + to_string(best_ask_price));

						_PriceInfo->PBestask = _PriceInfo->Bestask;
						_PriceInfo->Bestask = best_ask_price;


					}

					auto msg = algo_warrantprice_msg_pool.get_obj();
					msg->al = _algo;
					msg->algo_name = _algo->_name;
					msg->id = _algo->_u.get_id();
					msg->ref = _Ref;
					msg->warrant_code = _warrant_code;
					msg->side = "ASK";
					msg->wprice = best_ask_price;
					ouputQueue.enqueue(msg);
					//_algo->log_info(std::string(" CODE = ") + std::to_string(code) + " buyim = " + to_string(buyin) );
				}


				if(best_bid_qty >= _IssuerSize){
					_PriceInfo->LBestbid = best_bid_price;
				}
				if(best_ask_qty >= _IssuerSize){
					_PriceInfo->LBestask = best_ask_price;
				}
			}
		}

		void on_bear_book(const Tradable& tradable)
		{



			unsigned int code = tradable.m_Code;

			auto best_bid_qty = tradable.m_Bid[0].m_uQuantity;
			auto best_bid_price = static_cast<unsigned long long>(tradable.m_Bid[0].m_iPrice) * 100000;

			auto best_ask_qty = tradable.m_Ask[0].m_uQuantity;
			auto best_ask_price = static_cast<unsigned long long>(tradable.m_Ask[0].m_iPrice) * 100000;


			//_algo->log_info(std::string(" CODE = ") + std::to_string(code) );


			if(code == _Underlying_code){
/*
				if(_PriceInfoU->FBestbid != best_bid_price){
					_PriceInfoU->PFBestbid = _PriceInfoU->FBestbid;
					_PriceInfoU->FBestbid = best_bid_price;
				}
				if(_PriceInfoU->FBestask != best_ask_price ){
					_PriceInfoU->PFBestask = _PriceInfoU->FBestask;
					_PriceInfoU->FBestask = best_ask_price;
				}

				if(_Status == STATUS_AVAILABLE  && _Action_Status == STAGE_START){

					if(_PriceInfoU->LFBestask < best_ask_price &&  best_ask_price == _OBSetting->SellOut ){
						Log(DateUtil::getCurrentTime() + std::string("Quick Sell  CODE = ") + std::to_string(_warrant_code) + " PFBestAsk = " + std::to_string(_PriceInfoU->PFBestask) +  + " FBestAsk = " + std::to_string(_PriceInfoU->FBestask));
						warrant* warrant = _OBSetting->getRelatedWarrant(_warrant_code);
						warrant->SellPrice = _PriceInfo->Bestbid;
						warrant->Status = STATUS_SELLING;
						warrant->SellQty = warrant->Quantity;

						if(_OBSetting->SellOut == 99999999){
							warrant->SellOut = _PriceInfoU->FBestbid;
						}else{
							warrant->SellOut = _OBSetting->SellOut;
						}
						_Status = STATUS_SELLING;
						doSell(warrant);
					}
				}

				_PriceInfoU->LFBestbid = best_bid_price;
				_PriceInfoU->LFBestask = best_ask_price;


				_algo->log_info(std::string(" UCODE ") + std::to_string(code) +
				std::string(" CODE ") + std::to_string(_warrant_code) +
				std::string(" bid = ") + std::to_string(best_bid_price) +
				std::string(" ask = ") + std::to_string(best_ask_price) +
				std::string(" best_bid_qty = ") + std::to_string(best_bid_qty) +
				std::string(" best_ask_qty = ") + std::to_string(best_ask_qty)
								);
*/


				return;
			}else if(code == _warrant_code){


				priceinfo* uprice = _algo->uprice_map[_Underlying_code];

				_PriceInfoU->FBestbid =  uprice->FBestbid;
				_PriceInfoU->PFBestbid =  uprice->PFBestbid;
				_PriceInfoU->FBestask =  uprice->FBestask;
				_PriceInfoU->PFBestask =  uprice->PFBestask;

				/*
				_PriceInfoU->FBestbid =  uprice->TBestbid;
				_PriceInfoU->PFBestbid =  uprice->PTBestbid;
				_PriceInfoU->FBestask =  uprice->TBestask;
				_PriceInfoU->PFBestask =  uprice->PTBestask;
				*/
				_PriceInfoU->BidSeq =  uprice->BidSeq;
				_PriceInfoU->AskSeq =  uprice->AskSeq;


				/*
				Log(std::string(" WCODE ") + std::to_string(code) +
				std::string(" CODE ") + std::to_string(_warrant_code) +
				std::string(" bid = ") + std::to_string(best_bid_price) +
				std::string(" ask = ") + std::to_string(best_ask_price) +
				std::string(" p bid = ") + std::to_string(_PriceInfo->Bestbid) +
				std::string(" p ask = ") + std::to_string(_PriceInfo->Bestask) +
				std::string(" Issuer Size = ") + std::to_string(_IssuerSize) +
				std::string(" best_bid_qty = ") + std::to_string(best_bid_qty) +
				std::string(" best_ask_qty = ") + std::to_string(best_ask_qty) +
				std::string(" FBestbid = ") + std::to_string(_PriceInfoU->FBestbid) +
				std::string(" FBestask = ") + std::to_string(_PriceInfoU->FBestask) +
				std::string(" PFBestbid = ") + std::to_string(_PriceInfoU->PFBestbid) +
				std::string(" PFBestask = ") + std::to_string(_PriceInfoU->PFBestask)
								);
*/
				unsigned long buyin = _CbbcPriceMark->buyIn(best_ask_price);
				unsigned long long sellout = _CbbcPriceMark->sellOut(best_bid_price);
				unsigned long long lvlBid = _CbbcPriceMark->sellOut(best_ask_price);

				_OBSetting->BuyIn = buyin;
				_OBSetting->SellOut = sellout;
				_OBSetting->LvLBid = lvlBid;


				if(_Status == STATUS_AVAILABLE  && _Action_Status == STAGE_START && _PriceInfo->LBestbid != best_bid_price && best_bid_price > 0 && best_bid_price > _PriceInfo->LBestbid){
					warrant* warrant = _OBSetting->getRelatedWarrant(_warrant_code);
					if(warrant->BuyPrice > _PriceInfo->LBestbid && warrant->BuyPrice > best_bid_price){

						Log(std::string(" CODE = ") + std::to_string(code) + " SEll on_bear_book Raise back sell " );

						warrant->SellPrice = best_bid_price;
						warrant->Status = STATUS_SELLING;
						warrant->SellQty = warrant->Quantity;

						if(_OBSetting->SellOut == 99999999){
							warrant->SellOut = _PriceInfoU->FBestbid;
						}else{
							warrant->SellOut = _OBSetting->SellOut;
						}
						_Status = STATUS_SELLING;
						doSell(warrant);
					}
				}


				if(_Status == STATUS_AVAILABLE  && _Action_Status == STAGE_START && _Stop_Lost > 0 && best_bid_qty >= _IssuerSize){
					warrant* warrant = _OBSetting->getRelatedWarrant(_warrant_code);

					long long diff = static_cast<long long>(warrant->BuyPrice) - static_cast<long long>(best_bid_price);
					long long rwinPrice =  static_cast<long long>(_SPREAD * _Stop_Lost);
					if(diff >= rwinPrice){
						Log(std::string(" CODE = ") + std::to_string(code) + " SEll Stoplost > 0   Buy Price = " +   to_string(best_bid_price) + " --- Best Bid" +  to_string(best_bid_price)  + " Diff = " + to_string(diff) + "rwinPrice = " + to_string(rwinPrice) );

						warrant->SellPrice = best_bid_price;
						warrant->Status = STATUS_SELLING;
						warrant->SellQty = warrant->Quantity;

						if(_OBSetting->SellOut == 99999999){
							warrant->SellOut = _PriceInfoU->FBestbid;
						}else{
							warrant->SellOut = _OBSetting->SellOut;
						}
						_Status = STATUS_SELLING;
						doSell(warrant);
					}
				}


				if(_Status == STATUS_AVAILABLE  && _Action_Status == STAGE_START && _Win_Tick >= 0){

					warrant* warrant = _OBSetting->getRelatedWarrant(_warrant_code);

					unsigned long long refSpread = _SPREAD;
					unsigned long long rwinPrice = (unsigned long long) (warrant->BuyPrice + _Win_Tick * refSpread);

					unsigned long long bp = warrant->BuyPrice;

					if(_Win_Tick == 0){
						if(best_bid_price >= bp && bp > 0){
							Log(std::string(" CODE = ") + std::to_string(code) + " on bear book Sell Win Tick = 0 " + to_string(best_bid_price) + " Warrant Buy Price = " + to_string(warrant->BuyPrice)  );
							warrant->SellPrice = best_bid_price;
							warrant->SellQty = warrant->Quantity;
							warrant->Status = STATUS_SELLING;
							if(_OBSetting->SellOut == 99999999){
								warrant->SellOut = _PriceInfoU->FBestbid;
							}else{
								warrant->SellOut = _OBSetting->SellOut;
							}
							_Status = STATUS_SELLING;
							doSell(warrant);

						}
					}else{

						if(best_bid_price >= rwinPrice && bp > 0){

							Log(std::string(" CODE = ") + std::to_string(code) + " on bear book Sell Win Tick > 0 " + to_string(best_bid_price) + " Warrant Buy Price = " + to_string(warrant->BuyPrice)  );

							warrant->SellPrice = best_bid_price;
							warrant->SellQty = warrant->Quantity;
							warrant->Status = STATUS_SELLING;
							if(_OBSetting->SellOut == 99999999){
								warrant->SellOut = _PriceInfoU->FBestbid;
							}else{
								warrant->SellOut = _OBSetting->SellOut;
							}
							_Status = STATUS_SELLING;

							doSell(warrant);

						}
					}
				}



				//if(_PriceInfo->Bestbid != best_bid_price && best_bid_price > 0 && best_bid_qty >= _IssuerSize){
				if(_PriceInfo->Bestbid != best_bid_price && best_bid_price > 0){
					//_algo->log_info(std::string(" WCODE ") + std::to_string(code) + " DO Mark BID");
					if(best_bid_qty >= _IssuerSize){

						if(_PriceInfo->LastBidSeq != _PriceInfoU->BidSeq){
							_PriceInfo->LastBidSeq = _PriceInfoU->BidSeq;

							bool hasUpdate = _CbbcPriceMark->updateBid(best_bid_price, _PriceInfo->Bestbid, _PriceInfoU->FBestask, _PriceInfoU->PFBestask);
							if(hasUpdate){
								unsigned long long bkey = _CbbcPriceMark->getBidKey();
								unsigned long long bprice = _CbbcPriceMark->getBidPrice();
								if(bkey > 0 && bprice>0){

									auto msg = algo_pricetable_msg_pool.get_obj();
									msg->al = _algo;
									msg->algo_name = _algo->_name;
									msg->id = _algo->_u.get_id();
									msg->ref = _Ref;
									msg->warrant_code = _warrant_code;
									msg->side = "BID";
									msg->wkey = bkey;
									msg->fprice = bprice;
									ouputQueue.enqueue(msg);

								}
							}

							sellout = _CbbcPriceMark->sellOut(best_bid_price);
							lvlBid = _CbbcPriceMark->sellOut(best_ask_price);

							if(sellout != _OBSetting->SellOut && _Action_Status != STAGE_STOP){

								_OBSetting->SellOut = sellout;
								_OBSetting->LvLBid = lvlBid;

							}
						}

					}
					if(best_bid_qty >= _IssuerSize){

						Log(std::string(" CODE = ") + std::to_string(code) + " Bid Change from " + to_string(_PriceInfo->Bestbid) + " to " + to_string(best_bid_price));

						_PriceInfo->PBestbid = _PriceInfo->Bestbid;
						_PriceInfo->Bestbid = best_bid_price;
					}

					auto msg = algo_warrantprice_msg_pool.get_obj();
					msg->al = _algo;
					msg->algo_name = _algo->_name;
					msg->id = _algo->_u.get_id();
					msg->ref = _Ref;
					msg->warrant_code = _warrant_code;
					msg->side = "BID";
					msg->wprice = best_bid_price;
					ouputQueue.enqueue(msg);

					//_algo->log_info(std::string(" CODE = ") + std::to_string(code) + " SellOut = " + to_string(sellout) );
				}

				//if(_PriceInfo->Bestask != best_ask_price && best_ask_price > 0 && best_ask_qty >= _IssuerSize){
				if(_PriceInfo->Bestask != best_ask_price && best_ask_price > 0){
					//_algo->log_info(std::string(" WCODE ") + std::to_string(code) + " DO Mark ASK");
					if(best_ask_qty >= _IssuerSize){

						if(_PriceInfo->LastAskSeq != _PriceInfoU->AskSeq){
							_PriceInfo->LastAskSeq = _PriceInfoU->AskSeq;

							bool hasUpdate = _CbbcPriceMark->updateAsk(best_ask_price, _PriceInfo->Bestask, _PriceInfoU->FBestbid, _PriceInfoU->PFBestbid);
							if(hasUpdate){
								unsigned long long bkey = _CbbcPriceMark->getAskKey();
								unsigned long long bprice = _CbbcPriceMark->getAskPrice();
								if(bkey > 0 && bprice>0){

									auto msg = algo_pricetable_msg_pool.get_obj();
									msg->al = _algo;
									msg->algo_name = _algo->_name;
									msg->id = _algo->_u.get_id();
									msg->ref = _Ref;
									msg->warrant_code = _warrant_code;
									msg->side = "ASK";
									msg->wkey = bkey;
									msg->fprice = bprice;
									ouputQueue.enqueue(msg);

								}
							}

							buyin = _CbbcPriceMark->buyIn(best_ask_price);
							lvlBid = _CbbcPriceMark->sellOut(best_ask_price);

							if(buyin != _OBSetting->BuyIn && _Action_Status != STAGE_STOP){
								_OBSetting->BuyIn = buyin;
								_OBSetting->LvLBid = lvlBid;
							}

						}
					}

					if(best_ask_qty >= _IssuerSize){

						Log(std::string(" CODE = ") + std::to_string(code) + " Ask Change from " + to_string(_PriceInfo->Bestask) + " to " + to_string(best_ask_price));

						_PriceInfo->PBestask = _PriceInfo->Bestask;
						_PriceInfo->Bestask = best_ask_price;
					}

					auto msg = algo_warrantprice_msg_pool.get_obj();
					msg->al = _algo;
					msg->algo_name = _algo->_name;
					msg->id = _algo->_u.get_id();
					msg->ref = _Ref;
					msg->warrant_code = _warrant_code;
					msg->side = "ASK";
					msg->wprice = best_ask_price;
					ouputQueue.enqueue(msg);

					//_algo->log_info(std::string(" CODE = ") + std::to_string(code) + " buyim = " + to_string(buyin) );
				}

				if(best_bid_qty >= _IssuerSize){
				_PriceInfo->LBestbid = best_bid_price;
				}
				if(best_ask_qty >= _IssuerSize){
				_PriceInfo->LBestask = best_ask_price;
				}
			}
		}

		bool doBuy(warrant* w)
		{
			//if(!_OBSetting->hasRelatedWarrant(_warrant_code)){
			//	_OBSetting->addWarrantOrCbbc(w);
			//}
			_Status = STATUS_PENDING;

			auto odr = _algo->_u.new_order(
					_algo,w->BuyQty,
					w->BuyPrice,
					w->Code,
					dbp::top::order_side::buy,
					dbp::top::order_type::sl,
					dbp::top::aon_type::non_ano,
					dbp::top::ignore_price_type::ignore,
					0);
			if (odr.is_valid()){
				_algo->_o_map[odr.order_id] = this;
				return true;
			}

			return false;
		}

		bool doSell(warrant* w)
		{
			//_OBSetting->addWarrantOrCbbc(w);
			_Status = STATUS_SELLING;

			auto odr = _algo->_u.new_order(
					_algo,w->SellQty,
					w->SellPrice,
					w->Code,
					dbp::top::order_side::sell,
					dbp::top::order_type::sl,
					dbp::top::aon_type::non_ano,
					dbp::top::ignore_price_type::ignore,
					0);
			if (odr.is_valid()){
				_algo->_o_map[odr.order_id] = this;
				return true;
			}

			return false;
		}

		bool doSellLevel()
		{
			if(_Status != STATUS_AVAILABLE)
				return false;

			if(_Action_Status != STAGE_START)
				return false;

			unsigned long long wbestbid = default_sell_price();

			warrant* newWarrant = _OBSetting->getRelatedWarrant(_warrant_code);

			//if(wbestbid == newWarrant->BuyPrice && newWarrant->BuyPrice > 0){
			if(newWarrant->BuyPrice > 0 && wbestbid>0){

				Log(DateUtil::getCurrentTime() + std::string(" CODE = ") + std::to_string(_warrant_code) +  " Normal Do Level Sell " );
				newWarrant->Status = STATUS_SELLING;
				//newWarrant->SellPrice = _PriceInfo->Bestbid;
				newWarrant->SellPrice = wbestbid;
				newWarrant->SellQty = newWarrant->Quantity;
				newWarrant->SellOut = _OBSetting->SellOut;

				_Status = STATUS_SELLING;

				doSell(newWarrant);
			}

			return false;
		}

		void on_order(const dbp::top::enhance_order& odr)
		{
			//Log(std::string("on_order:") + odr.to_string());
			auto status = odr.status;
			auto side = odr.side;
			if (dbp::top::order_status::rejected == status || dbp::top::order_status::canceled == status || dbp::top::order_status::deleted == status || dbp::top::order_status::filled == status)
			{
				if (dbp::top::order_side::buy == side)
				{
					if (dbp::top::order_status::filled == status)
					{
						warrant* obsw = _OBSetting->getRelatedWarrant(_warrant_code);
						obsw->BuyPrice = odr.match_price;
						obsw->BuyTime = string(odr.transaction_tm);
						obsw->Quantity += odr.filled_quantity;
						obsw->OrderId = odr.order_id;


						obsw->Status = STATUS_AVAILABLE;
						obsw->hasPosition = true;

						_OBSetting->hasPosition = true;
						_Status = STATUS_AVAILABLE;


						auto msg = algo_order_msg_pool.get_obj();
						msg->al = _algo;
						msg->algo_name = _algo->_name;
						msg->id = _algo->_u.get_id();
						msg->ref = _Ref;
						msg->orderid = odr.order_id;
						msg->warrant_code = _warrant_code;
						msg->side = "BUY";
						msg->filled_price = odr.match_price;
						msg->filled_quantity = odr.filled_quantity;
						msg->order_price = obsw->BuyPrice;
						msg->order_quantity = obsw->BuyQty;
						msg->buyin = obsw->BuyIn;
						msg->sellout = obsw->SellOut;
						msg->lvlbid = obsw->LvlBid;
						msg->status = "filled";
						msg->transaction_time = obsw->BuyTime;
						ouputQueue.enqueue(msg);

					}
					if (dbp::top::order_status::canceled == status || dbp::top::order_status::rejected == status)
					{
						if(_Status == STATUS_PENDING){
							warrant* warrant = _OBSetting->removeWarrantOrCbbc(_warrant_code);
							warrant->BuyTime = string(odr.transaction_tm);

							auto msg = algo_order_msg_pool.get_obj();
							msg->al = _algo;
							msg->algo_name = _algo->_name;
							msg->id = _algo->_u.get_id();
							msg->ref = _Ref;
							msg->orderid = odr.order_id;
							msg->warrant_code = _warrant_code;
							msg->side = "BUY";
							msg->order_price = warrant->BuyPrice;
							msg->order_quantity = warrant->BuyQty;
							msg->transaction_time = warrant->BuyTime;
							msg->buyin = warrant->BuyIn;
							msg->status = "cancel";
							msg->reason = string(odr.reject_reason);
							ouputQueue.enqueue(msg);

							_OBSetting->TradeTime = DateUtil::getCurrentSystemTime();
							_Status = STATUS_DONE;
						}
					}
				}

				else if (dbp::top::order_side::sell == side)
				{
					if (dbp::top::order_status::filled == status)
					{
						warrant* obsw = _OBSetting->removeWarrantOrCbbc(_warrant_code);

						obsw->Status = STATUS_SOLD;
						obsw->SoldTime = string(odr.transaction_tm);
						obsw->SellPrice = odr.match_price;

						obsw->Strategy = "Bear Algo";

						if(obsw->Quantity == odr.filled_quantity){

							auto msg = algo_order_msg_pool.get_obj();
							msg->al = _algo;
							msg->algo_name = _algo->_name;
							msg->id = _algo->_u.get_id();
							msg->ref = _Ref;
							msg->orderid = odr.order_id;
							msg->warrant_code = _warrant_code;
							msg->side = "SELL";
							msg->filled_price = odr.match_price;
							msg->filled_quantity = odr.filled_quantity;
							msg->order_price = obsw->SellPrice;
							msg->order_quantity = obsw->SellQty;
							msg->transaction_time = obsw->SoldTime;
							msg->sellout = obsw->SellOut;
							msg->status = "filled";
							ouputQueue.enqueue(msg);


							auto pmsg = algo_portfolio_msg_pool.get_obj();
							pmsg->al = _algo;
							pmsg->algo_name = _algo->_name;
							pmsg->id = _algo->_u.get_id();
							pmsg->ref = _Ref;
							pmsg->warrant_code = _warrant_code;
							pmsg->buy_price = obsw->BuyPrice;
							pmsg->sell_price = obsw->SellPrice;
							pmsg->quantity = obsw->Quantity;
							pmsg->buytime = obsw->BuyTime;
							pmsg->selltime= obsw->SoldTime;
							ouputQueue.enqueue(pmsg);


							_OBSetting->TradeTime = DateUtil::getCurrentSystemTime();
							_OBSetting->hasPosition = false;

							_Status = STATUS_DONE;
						}else{


							auto msg = algo_order_msg_pool.get_obj();
							msg->al = _algo;
							msg->algo_name = _algo->_name;
							msg->id = _algo->_u.get_id();
							msg->ref = _Ref;
							msg->orderid = odr.order_id;
							msg->warrant_code = _warrant_code;
							msg->side = "SELL";
							msg->filled_price = odr.match_price;
							msg->filled_quantity = odr.filled_quantity;
							msg->order_price = obsw->SellPrice;
							msg->order_quantity = obsw->SellQty;
							msg->transaction_time = obsw->SoldTime;
							msg->sellout = obsw->SellOut;
							msg->status = "Partial filled";
							ouputQueue.enqueue(msg);

							auto pmsg = algo_portfolio_msg_pool.get_obj();
							pmsg->al = _algo;
							pmsg->algo_name = _algo->_name;
							pmsg->id = _algo->_u.get_id();
							pmsg->ref = _Ref;
							pmsg->warrant_code = _warrant_code;
							pmsg->buy_price = obsw->BuyPrice;
							pmsg->sell_price = obsw->SellPrice;
							pmsg->quantity = odr.filled_quantity;
							pmsg->buytime = obsw->BuyTime;
							pmsg->selltime= obsw->SoldTime;
							ouputQueue.enqueue(pmsg);

							obsw->Quantity = obsw->Quantity - odr.filled_quantity;
							obsw->SoldTime = "";
							obsw->SellPrice = 0;
							obsw->Status = STATUS_AVAILABLE;

							_OBSetting->addWarrantOrCbbc(obsw);


							_Status = STATUS_AVAILABLE;

						}

					}
					if (dbp::top::order_status::canceled == status || dbp::top::order_status::rejected == status)
					{
						warrant* warrant = _OBSetting->getRelatedWarrant(_warrant_code);

						auto msg = algo_order_msg_pool.get_obj();
						msg->al = _algo;
						msg->algo_name = _algo->_name;
						msg->id = _algo->_u.get_id();
						msg->ref = _Ref;
						msg->orderid = odr.order_id;
						msg->warrant_code = _warrant_code;
						msg->side = "SELL";
						msg->order_price = warrant->SellPrice;
						msg->order_quantity = warrant->Quantity;
						msg->transaction_time =  string(odr.transaction_tm);
						msg->sellout = warrant->SellOut;
						msg->status = "cancel";
						msg->reason = string(odr.reject_reason);
						ouputQueue.enqueue(msg);

						if(_Status == STATUS_SELLING){
							_OBSetting->setRelatedWarrantStatus(_warrant_code, STATUS_AVAILABLE);
							_Status = STATUS_AVAILABLE;
						}
					}
				}
			}
		}
		const std::string& commodity_symbol()
		{
			return _Symbol;
		}
		unsigned int warrant_code() const
		{
			return _warrant_code;
		}
		unsigned int underlying_code() const
		{
			return _Underlying_code;
		}
		unsigned long long getIssuerSize()
		{
			return _IssuerSize;
		}
		unsigned long long getBuyQuantity()
		{
			return _Quantity;
		}
		int getWtype() const
		{
			return _Wtype;
		}
		int getUtype() const
		{
			return _Utype;
		}
		const std::string& ref()
		{
			return _Ref;
		}
		int action_status() const
		{
			return _Action_Status;
		}
		void set_ref(const std::string& ref)
		{
			_Ref = ref;
		}

		bool set_Position(unsigned long long price, unsigned long long qty)
		{
			warrant* newWarrant = new warrant;
			newWarrant->Date = DateUtil::getToday();
			newWarrant->Code = _warrant_code;
			newWarrant->Status = STATUS_READY;
			newWarrant->BuyPrice = price;
			newWarrant->BuyQty = qty;
			newWarrant->Quantity = qty;
			newWarrant->Name = std::to_string(_warrant_code);

			if(!_OBSetting->isExist(_warrant_code)){
				_OBSetting->addWarrantOrCbbc(newWarrant);
				_Status = STATUS_AVAILABLE;

				auto msg = algo_positionorder_msg_pool.get_obj();
				msg->al = _algo;
				msg->algo_name = _algo->_name;
				msg->id = _algo->_u.get_id();
				msg->ref = _Ref;
				msg->action = "previous order";
				msg->warrant_code = _warrant_code;
				msg->order_price = price;
				msg->order_quantity = qty;
				msg->filled_price = price;
				msg->filled_quantity = qty;
				msg->status = "filled";
				ouputQueue.enqueue(msg);

				return true;
			}else{
				if(qty == 0){


					warrant* w = _OBSetting->removeWarrantOrCbbc(_warrant_code);
					w->Quantity = 0;
					_OBSetting->hasPosition = false;

					auto msg = algo_positionorder_msg_pool.get_obj();
					msg->al = _algo;
					msg->algo_name = _algo->_name;
					msg->id = _algo->_u.get_id();
					msg->ref = _Ref;
					msg->action = "delete order";
					msg->warrant_code = _warrant_code;
					msg->order_price = 0;
					msg->order_quantity = 0;
					msg->filled_price = 0;
					msg->filled_quantity = 0;
					msg->status = "delete";
					ouputQueue.enqueue(msg);

					_Status = STATUS_DONE;

					return true;
				}

				return false;
			}

		}
		void set_IssuerSize (unsigned long long issuer_size)
		{
			_IssuerSize = issuer_size;
			_OBSetting->IssuerSize = _IssuerSize;
		}
		void set_BuyQuantity (unsigned long long buy_quantity)
		{
			_Quantity = buy_quantity;
			_OBSetting->Quantity = _Quantity;
		}
		void set_Symbol (std::string symbol){
			_Symbol = symbol;
		}
		void set_ActionStatus(int status){
			_Action_Status = status;
		}
		void set_WinTick (int win_tick)
		{
			_Win_Tick = win_tick;
			_OBSetting->WinTicks = _Win_Tick;
		}
		void set_StopLost (int stop_lost)
		{
			_Stop_Lost = stop_lost;
			_OBSetting->StopLostTick = _Stop_Lost;
		}
		void set_Spread(unsigned long long spread)
		{
			_SPREAD = spread;
		}
		unsigned long long getSpread()
		{
			return _SPREAD;
		}

		int inoutrange()
		{
			return _INOUT;
		}
		int lvlrange()
		{
			return _LVLRANGE;
		}
		int ptrange()
		{
			return _PTRANGE;
		}
		void set_InOut (int inout)
		{
			_INOUT = inout;
		}
		void set_LvlRange (int lvlrange)
		{
			_LVLRANGE = lvlrange;
		}
		void set_PtRange (int ptrange)
		{
			_PTRANGE = ptrange;
		}
		bool has_position(){
			return _OBSetting->hasPosition;
		}

		unsigned long long getPosition(){
			if(_OBSetting->hasPosition){
				warrant* w = _OBSetting->getRelatedWarrant(_warrant_code);
				return w->Quantity;
			}
			return 0;
		}

		std::map<unsigned long long, unsigned long long> getBidTable(){
			return _CbbcPriceMark->getBidTable();
		}

		std::map<unsigned long long, unsigned long long> getAskTable(){
			return _CbbcPriceMark->getAskTable();
		}

		int status(){
			return _Status;
		}

		void Log(string msg){
			_algo->Log(msg);
			//fprintf(stderr, "%s %s \n",DateUtil::getCurrentTime(), msg.c_str());
			//logger->Log(string(DateUtil::getCurrentTime()) + " " + msg);
		}
	};
private:
	using pair_map = std::unordered_map<std::string, pair>;
private:
	mutable pair_map _p_map;
private:
	struct action_resp
	{
		std::string result;
		std::string reason;
		int previous_action;
		int action;
		bool recovery;
	};
	struct algo_set_msg: public algo_msg_base
	{
		pair p;
		std::string msg_type;
		std::string result;
		std::string reason;

		algo_set_msg():
			algo_msg_base(),
			p(),
			msg_type(""),
			result(""),
			reason("")
		{
		}
		virtual nlohmann::json to_json() const
		{
			auto j = algo_msg_base::to_json();
			j["msg_type"] = "algo_set_msg";
			j["pair"] = p.to_json();
			if(result == "SUCCESS")
			{
				j["result"] = "SUCCESS";
				j["recovery"] = true;
			}else{
				j["result"] = "FAIL";
				j["reason"] = result;
			}
			return j;
		}
		virtual void on_command()
		{
			//fprintf(stderr, "%s \n", "algo msg set oncommand");
			auto* self = dynamic_cast<bear*>(al);
			auto p2 = p;
			std::string r = self->set_pair(std::move(p2));
			result = r;
			//fprintf(stderr, "algo msg set oncommand result = %s \n", result.c_str());
			ouputQueue.enqueue(this);
		}
		virtual void release()
		{
			algo_set_msg_pool.release_obj(this);
		}
		virtual ~algo_set_msg() = default;
	};
	struct algo_action_msg: public algo_msg_base
	{
		unsigned int code;
		std::string action;
		std::string result;
		std::string reason;
		action_resp _resp;

		algo_action_msg():
			algo_msg_base(),
			code(0),
			action(""),
			result(""),
			reason("")
		{
		}
		virtual nlohmann::json to_json() const
		{
			auto j = algo_msg_base::to_json();
			if(_resp.recovery){
				j["action_type"] = action;
				j["previous_action_status"] = _resp.previous_action;
				j["new_action_status"] = _resp.action;
				j["result"] = result;
				j["recovery"] = true;
			}else{
				j["action_type"] = action;
				j["reason"] = reason;
				j["result"] = result;
			}
			return j;
		}
		virtual void on_command()
		{
			auto* self = dynamic_cast<bear*>(al);

			action_resp resp;
			if(action == "start"){
				resp = self->set_start(code, ref);
			}
			if(action == "pause"){
				resp = self->set_pause(code, ref);
			}
			if(action == "stop"){
				resp = self->set_stop(code, ref);
			}
			_resp = resp;

			result = _resp.result;
			reason = _resp.reason;
			ouputQueue.enqueue(this);

		}
		virtual void release()
		{
			algo_action_msg_pool.release_obj(this);
		}
		virtual ~algo_action_msg() = default;
	};
	struct algo_err_msg: public algo_msg_base
	{
		std::string action;
		std::string result;
		std::string reason;
		algo_err_msg():
			algo_msg_base(),
			action(""),
			result(""),
			reason("")
		{
		}
		virtual nlohmann::json to_json() const
		{
			auto j = algo_msg_base::to_json();
			j["action"] = action;
			j["result"] = result;
			j["reason"] = reason;
			return j;
		}
		virtual void on_command()
		{
			ouputQueue.enqueue(this);
		}
		virtual void release()
		{
			algo_err_msg_pool.release_obj(this);
		}
		virtual ~algo_err_msg() = default;
	};
	struct algo_force_buy: public algo_msg_base
	{
		unsigned int code;
		std::string result;
		unsigned long long price;
		unsigned long long quantity;
		algo_force_buy():
			algo_msg_base(),
			code(0),
			result(""),
			price(0),
			quantity(0)
		{
		}
		virtual nlohmann::json to_json() const
		{
			auto j = algo_msg_base::to_json();
			j["msg_type"] = "cbbc_algo_force_buy";
			j["warrant_code"] = code;
			j["price"] = price;
			j["quantity"] = quantity;

			if(result == "SUCCESS"){
				j["result"] = result;
			}else{
				j["result"] = "FAIL";
				j["reason"] = result;
			}
			return j;
		}
		virtual void on_command()
		{
			auto* self = dynamic_cast<bear*>(al);
			result = self->force_buy(price, quantity,  ref);
			//if(result != "Exceed Buy Power" && result != "Not Ready"){
				ouputQueue.enqueue(this);
			//}
		}
		virtual void release()
		{
			algo_force_buy_pool.release_obj(this);
		}
		virtual ~algo_force_buy() = default;
	};
	struct algo_force_sell: public algo_msg_base
	{
		unsigned int code;
		std::string result;
		unsigned long long price;
		unsigned long long quantity;
		algo_force_sell():
			algo_msg_base(),
			code(0),
			result(""),
			price(0),
			quantity(0)
		{
		}
		virtual nlohmann::json to_json() const
		{
			auto j = algo_msg_base::to_json();
			j["msg_type"] = "cbbc_algo_force_sell";
			j["warrant_code"] = code;
			j["price"] = price;
			j["quantity"] = quantity;
			if(result == "SUCCESS"){
				j["result"] = "SUCCESS";
			}else{
				j["result"] = "FAIL";
				j["reason"] = result;
			}
			return j;
		}
		virtual void on_command()
		{
			auto* self = dynamic_cast<bear*>(al);
			result = self->force_sell(price, quantity,ref);
			ouputQueue.enqueue(this);
		}
		virtual void release()
		{
			algo_force_sell_pool.release_obj(this);
		}
		virtual ~algo_force_sell() = default;
	};
	struct algo_setposition: public algo_msg_base
	{
		unsigned int code;
		unsigned long long price;
		unsigned long long quantity;
		std::string result;
		algo_setposition():
			algo_msg_base(),
			code(0),
			price(0),
			quantity(0),
			result("")
		{
		}
		virtual nlohmann::json to_json() const
		{
			auto j = algo_msg_base::to_json();
			j["type"] = "set position";
			j["warrant_code"] = code;
			j["price"] = price;
			j["quantity"] = quantity;
			j["result"] = result;
			return j;
		}
		virtual void on_command()
		{
			auto* self = dynamic_cast<bear*>(al);
			result = self->set_position(price, quantity, ref);
			ouputQueue.enqueue(this);
		}
		virtual void release()
		{
			algo_setposition_pool.release_obj(this);
		}
		virtual ~algo_setposition() = default;
	};
	struct algo_loadpricetable: public algo_msg_base
	{
		unsigned int code;
		std::string result;
		algo_loadpricetable():
			algo_msg_base(),
			code(0),
			result("")
		{
		}
		virtual nlohmann::json to_json() const
		{
			auto j = algo_msg_base::to_json();
			j["msg_type"] = "algo_loadpricetable";
			j["warrant_code"] = code;
			j["result"] = result;
			return j;
		}
		virtual void on_command()
		{
			auto* self = dynamic_cast<bear*>(al);
			result = self->load_pricetable(code, ref);

			ouputQueue.enqueue(this);


		}
		virtual void release()
		{
			algo_loadpricetable_pool.release_obj(this);
		}
		virtual ~algo_loadpricetable() = default;
	};
	struct algo_param_msg: public algo_msg_base
	{
		unsigned int code;
		std::string type;
		std::string value;
		std::string result;
		algo_param_msg():
			algo_msg_base(),
			code(0),
			type(""),
			value(""),
			result("")
		{
		}
		virtual nlohmann::json to_json() const
		{
			auto j = algo_msg_base::to_json();
			j["msg_type"] = "algo_param_msg";
			j["warrant_code"] = code;
			j["type"] = type;
			j["value"] = value;
			if(result == "SUCCESS"){
				j["result"] = result;
				j["recovery"] = true;
			}else{
				j["result"] = "FAIL";
				j["reason"] = result;
			}
			return j;
		}
		virtual void on_command()
		{
			auto* self = dynamic_cast<bear*>(al);
			result = self->set_param(code, type, value, ref);

			ouputQueue.enqueue(this);


		}
		virtual void release()
		{
			algo_param_msg_pool.release_obj(this);
		}
		virtual ~algo_param_msg() = default;
	};
	struct algo_pricetable_msg: public algo_msg_base
	{
		unsigned int warrant_code;
		std::string side;
		unsigned long long wkey;
		unsigned long long fprice;
		algo_pricetable_msg():
			algo_msg_base(),
			warrant_code(0),
			side(""),
			wkey(0),
			fprice(0)
		{
		}
		virtual nlohmann::json to_json() const
		{
			auto j = algo_msg_base::to_json();
			j["msg_type"] = "pricetable";
			j["warrant_code"] = warrant_code;
			j["side"] = side;
			j["wkey"] = wkey;
			j["fprice"] = fprice;
			return j;
		}
		virtual void on_command()
		{
			ouputQueue.enqueue(this);
		}
		virtual void release()
		{
			algo_pricetable_msg_pool.release_obj(this);
		}
		virtual ~algo_pricetable_msg() = default;
	};
	struct algo_warrantprice_msg: public algo_msg_base
	{
		unsigned int warrant_code;
		std::string side;
		unsigned long long wkey;
		unsigned long long wprice;
		algo_warrantprice_msg():
			algo_msg_base(),
			warrant_code(0),
			side(""),
			wkey(0),
			wprice(0)
		{
		}
		virtual nlohmann::json to_json() const
		{
			auto j = algo_msg_base::to_json();
			j["msg_type"] = "wprice";
			j["warrant_code"] = warrant_code;
			j["side"] = side;
			j["wkey"] = wkey;
			j["wprice"] = wprice;
			return j;
		}
		virtual void on_command()
		{
			ouputQueue.enqueue(this);
		}
		virtual void release()
		{
			algo_warrantprice_msg_pool.release_obj(this);
		}
		virtual ~algo_warrantprice_msg() = default;
	};
	struct algo_validate_msg: public algo_msg_base
	{
		unsigned int warrant_code;
		std::string side;
		unsigned long long wprice;
		unsigned long long buyin;
		unsigned long long pbuyin;
		unsigned long long lvlbid;
		unsigned long long sellout;
		std::string action;

		algo_validate_msg():
			algo_msg_base(),
			warrant_code(0),
			side(""),
			wprice(0),
			buyin(0),
			pbuyin(0),
			lvlbid(0),
			sellout(0)
		{
		}
		virtual nlohmann::json to_json() const
		{
			auto j = algo_msg_base::to_json();
			j["msg_type"] = "wprice";
			j["warrant_code"] = warrant_code;
			j["side"] = side;
			j["wprice"] = wprice;
			j["action"] = action;
			if(action == "inoutrule")
			{
				j["buyin"] = buyin;
				j["sellout"] = sellout;
			}else if(action == "lvlrangerule"){
				j["buyin"] = buyin;
				j["lvlbid"] = lvlbid;
			}else if(action == "ptrangerule"){
				j["buyin"] = buyin;
				j["pbuyin"] = pbuyin;
			}
			return j;
		}
		virtual void on_command()
		{
			ouputQueue.enqueue(this);
		}
		virtual void release()
		{
			algo_validate_msg_pool.release_obj(this);
		}
		virtual ~algo_validate_msg() = default;
	};
	struct algo_order_msg: public algo_msg_base
	{
		unsigned long long orderid;
		unsigned int warrant_code;
		std::string action;
		std::string side;
		unsigned long long order_price;
		unsigned long long order_quantity;
		unsigned long long filled_price;
		unsigned long long filled_quantity;
		std::string status;
		std::string transaction_time;
		unsigned long long sellout;
		unsigned long long buyin;
		unsigned long long lvlbid;
		std::string reason;

		algo_order_msg():
			algo_msg_base()
		{
		}
		virtual nlohmann::json to_json() const
		{
			auto j = algo_msg_base::to_json();
			j["orderid"] = orderid;
			j["warrant_code"] = warrant_code;
			j["action"] = "order";
			j["side"] = side;
			if(side == "BUY"){
				j["buyin"] = buyin;
				j["sellout"] = sellout;
				j["lvlbid"] = lvlbid;
			}else if(side == "SELL"){
				j["sellout"] = sellout;
			}
			j["order_price"] = order_price;
			j["order_quantity"] = order_quantity;
			if(status == "filled" || status == "Partial filled"){
				j["filled_price"] = filled_price;
				j["filled_quantity"] = filled_quantity;
			}
			j["status"] = status;
			j["transaction_time"] = string(transaction_time);

			j["reason"] = string(reason);
			j["recovery"] = true;
			return j;
		}
		virtual void on_command()
		{
			ouputQueue.enqueue(this);
		}
		virtual void release()
		{
			algo_order_msg_pool.release_obj(this);
		}
		virtual ~algo_order_msg() = default;
	};
	struct algo_positionorder_msg: public algo_msg_base
	{
		unsigned long long orderid;
		unsigned int warrant_code;
		std::string action;
		unsigned long long order_price;
		unsigned long long order_quantity;
		unsigned long long filled_price;
		unsigned long long filled_quantity;
		std::string status;

		algo_positionorder_msg():
			algo_msg_base()
		{
		}
		virtual nlohmann::json to_json() const
		{
			auto j = algo_msg_base::to_json();
			j["warrant_code"] = warrant_code;
			j["action"] = action;
			j["order_price"] = order_price;
			j["order_quantity"] = order_quantity;
			j["filled_price"] = filled_price;
			j["filled_quantity"] = filled_quantity;
			j["status"] = status;
			j["recovery"] = true;
			return j;
		}
		virtual void on_command()
		{
			ouputQueue.enqueue(this);
		}
		virtual void release()
		{
			algo_positionorder_msg_pool.release_obj(this);
		}
		virtual ~algo_positionorder_msg() = default;
	};
	struct algo_portfolio_msg: public algo_msg_base
	{
		unsigned long long orderid;
		unsigned int warrant_code;
		unsigned long long buy_price;
		unsigned long long sell_price;
		unsigned long long quantity;
		std::string buytime;
		std::string selltime;

		algo_portfolio_msg():
			algo_msg_base()
		{
		}
		virtual nlohmann::json to_json() const
		{
			auto j = algo_msg_base::to_json();
			j["action"] = "portfolio";
			j["warrant_code"] = warrant_code;
			j["buy_price"] = buy_price;
			j["buytime"] = buytime;
			j["sell_price"] = sell_price;
			j["sellime"] = selltime;
			j["quantity"] = quantity;
			j["recovery"] = true;
			return j;
		}
		virtual void on_command()
		{
			ouputQueue.enqueue(this);
		}
		virtual void release()
		{
			algo_portfolio_msg_pool.release_obj(this);
		}
		virtual ~algo_portfolio_msg() = default;
	};

public:
	bear() = delete;
	bear(user& u, const std::string& name);
	bear(const algo&) = delete;
	bear(algo&&) = delete;
	bear& operator= (const algo&) = delete;
	bear& operator= (algo&&) = delete;
public:
	virtual void on_omdc_book(const Tradable&);
	virtual void on_omdd_book(const Tradable&);
	virtual void on_tcp_book(const Tradable&);
	virtual void on_omdc_trade(const Tradable&);
	virtual void on_omdd_trade(const Tradable&);
	virtual void on_tcp_trade(const Tradable&);
	virtual void handler_order(const dbp::top::enhance_order&);
	std::string set_pair(pair&& p);
	action_resp set_pause(unsigned int code, const std::string& ref);
	action_resp set_stop(unsigned int code, const std::string& ref);
	action_resp set_start(unsigned int code, const std::string& ref);
	std::string set_param(unsigned int code, const std::string& type, const std::string& value, const std::string& ref);
	std::string load_pricetable(unsigned int code, const std::string& ref);
	std::string force_buy(unsigned long long price, unsigned long long quantity, const std::string& ref);
	std::string force_sell(unsigned long long price, unsigned long long quantity, const std::string& ref);
	std::string set_position(unsigned long long price, unsigned long long quantity, const std::string& ref);
	virtual void Log(std::string msg);
	virtual void handle_command(algo_msg_base&);
	virtual algo_msg_base* json_to_msg(json& msg);
	virtual std::string get_lib_name();
public:
	static rapid_ring::spmc_ring_buffer_object_pool<algo_err_msg, 8192> algo_err_msg_pool;
	static rapid_ring::spsc_ring_buffer_object_pool<algo_param_msg, 8192> algo_param_msg_pool;
	static rapid_ring::spsc_ring_buffer_object_pool<algo_loadpricetable, 8192> algo_loadpricetable_pool;
	static rapid_ring::spsc_ring_buffer_object_pool<algo_positionorder_msg, 8192> algo_positionorder_msg_pool;
	static rapid_ring::spsc_ring_buffer_object_pool<algo_order_msg, 8192> algo_order_msg_pool;
	static rapid_ring::spmc_ring_buffer_object_pool<algo_validate_msg, 8192> algo_validate_msg_pool;
	static rapid_ring::spmc_ring_buffer_object_pool<algo_portfolio_msg, 8192> algo_portfolio_msg_pool;
	static rapid_ring::spmc_ring_buffer_object_pool<algo_warrantprice_msg, 8192> algo_warrantprice_msg_pool;
	static rapid_ring::spmc_ring_buffer_object_pool<algo_pricetable_msg, 8192> algo_pricetable_msg_pool;
	static rapid_ring::spsc_ring_buffer_object_pool<algo_set_msg, 8192> algo_set_msg_pool;
	static rapid_ring::spsc_ring_buffer_object_pool<algo_action_msg, 8192> algo_action_msg_pool;
	static rapid_ring::spsc_ring_buffer_object_pool<algo_setposition, 8192> algo_setposition_pool;
	static rapid_ring::spsc_ring_buffer_object_pool<algo_force_buy, 8192> algo_force_buy_pool;
	static rapid_ring::spsc_ring_buffer_object_pool<algo_force_sell, 8192> algo_force_sell_pool;

};



#endif /* CPP_ALGOS_DBP_BEAR_INC_BEAR_HPP_ */
