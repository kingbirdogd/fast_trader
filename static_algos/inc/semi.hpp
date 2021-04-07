#ifndef ALGOS_INC_SEMI_HPP_
#define ALGOS_INC_SEMI_HPP_

#include <tools.h>
#include <global_memory.hpp>
#include <ctime>
#include <rapid_ring/ring_buffer_object_poll.hpp>
#include <DateUtil.h>
#include <algo.hpp>
#include "ThreadLogger.h"

class semi : public algo
{
private:
	class pair;
private:
	using order_map = std::unordered_map<unsigned long long, pair*>;
	using md_map = std::unordered_map<unsigned int, std::unordered_set<pair*>>;
	friend class pair;
private:
	order_map _o_map;
	md_map _u_map;
	md_map _w_map;
private:
	class pair
	{
	public:
		semi* _algo;
		unsigned long long _buy_trriger;
		unsigned long long _sell_trriger;
		unsigned long long _buy_price;
		unsigned long long _sell_price;
		unsigned long long _bottom_price;
		unsigned long long _ceiling_price;
		unsigned long long _auto_buy_quantity;
		unsigned long long _position;
		unsigned long long _auto_buy_id;
		unsigned long long _auto_sell_id;
		unsigned long long _early_buy_qty;
		unsigned long long _early_sell_qty;
		unsigned long long _ratio_buy;
		unsigned long long _ratio_sell;
		unsigned int _underlying_code;
		unsigned int _warrant_code;
		std::string _ref;
		std::string _underlying_symbol;
		bool _is_bull;
		bool _is_buying;
		bool _is_selling;
		bool _auto_buy;
		bool _auto_sell;
		bool _is_omdd;
		bool _is_reset_position;
		unsigned long long wbestbid;
		unsigned long long wbestask;
		unsigned long long _last_trigger_price;
		unsigned long long _last_price;
		unsigned long long _odrid;
		dbp::top::order_type _sell_type;
	public:
		enum class sell_result: unsigned long long
		{
			SUCCESS = 0,
			NOTHING_TO_SELL = 1,
			BUYING = 2,
			SELLING = 3,
			SHORT_SELL = 4,
			NEW_SELL_ODR_FAIL = 5
		};
		enum class buy_result: unsigned long long
		{
			SUCCESS = 0,
			BUYING = 1,
			NOT_READY = 2,
			EXCEED_BUY_POWER = 3,
			UNKNOWN = 4
		};
	public:
		pair
		(
		):
			_algo(nullptr),
			_buy_trriger(0),
			_sell_trriger(0),
			_buy_price(0),
			_sell_price(0),
			_bottom_price(0),
			_ceiling_price(0),
			_auto_buy_quantity(0),
			_position(0),
			_auto_buy_id(0),
			_auto_sell_id(0),
			_early_buy_qty(0),
			_early_sell_qty(0),
			_ratio_buy(0),
			_ratio_sell(0),
			_underlying_code(0),
			_warrant_code(0),
			_ref(""),
			_underlying_symbol(""),
			_is_bull(false),
			_is_buying(false),
			_is_selling(false),
			_auto_buy(false),
			_auto_sell(false),
			_is_omdd(false),
			_is_reset_position(false),
			wbestbid(0),
			wbestask(0),
			_last_trigger_price(0),
			_last_price(0),
			_odrid(0),
			_sell_type(dbp::top::order_type::sl)
		{
		}
		pair
		(
			semi* algo,
			unsigned long long buy_trriger,
			unsigned long long sell_trriger,
			unsigned long long buy_price,
			unsigned long long sell_price,
			unsigned long long bottom_price,
			unsigned long long ceiling_price,
			unsigned long long auto_buy_quantity,
			unsigned int underlying_code,
			unsigned int warrant_code,
			std::string ref,
			std::string underlying_symbol,
			bool is_bull,
			bool auto_buy,
			bool auto_sell,
			bool is_omdd,
			unsigned long long early_buy_qty,
			unsigned long long early_sell_qty,
			unsigned long long ratio_buy,
			unsigned long long ratio_sell,
			bool is_reset_position,
			unsigned long long position = 0
		):
			_algo(algo),
			_buy_trriger(buy_trriger),
			_sell_trriger(sell_trriger),
			_buy_price(buy_price),
			_sell_price(sell_price),
			_bottom_price(bottom_price),
			_ceiling_price(ceiling_price),
			_auto_buy_quantity(auto_buy_quantity),
			_position(position),
			_auto_buy_id(0),
			_auto_sell_id(0),
			_early_buy_qty(early_buy_qty),
			_early_sell_qty(early_sell_qty),
			_ratio_buy(ratio_buy),
			_ratio_sell(ratio_sell),
			_underlying_code(underlying_code),
			_warrant_code(warrant_code),
			_ref(ref),
			_underlying_symbol(underlying_symbol),
			_is_bull(is_bull),
			_is_buying(false),
			_is_selling(false),
			_auto_buy(auto_buy),
			_auto_sell(auto_sell),
			_is_omdd(is_omdd),
			_is_reset_position(is_reset_position),
			wbestbid(0),
			wbestask(0),
			_last_trigger_price(0),
			_last_price(0),
			_odrid(0),
			_sell_type(dbp::top::order_type::sl)
		{
		}
		json to_json() const
		{
			json j;
			j["buy_trriger"] = _buy_trriger;
			j["sell_trriger"] = _sell_trriger;
			j["buy_price"] = _buy_price;
			j["sell_price"] = _sell_price;
			j["bottom_price"] = _bottom_price;
			j["ceiling_price"] = _ceiling_price;
			j["auto_buy_quantity"] = _auto_buy_quantity;
			j["position"] = _position;
			j["auto_buy_id"] = _auto_buy_id;
			j["auto_sell_id"] = _auto_sell_id;
			j["early_buy_qty"] = _early_buy_qty;
			j["early_sell_qty"] = _early_sell_qty;
			j["ratio_buy"] = _ratio_buy;
			j["ratio_sell"] = _ratio_sell;
			j["underlying_code"] = _underlying_code;
			j["warrant_code"] = _warrant_code;
			j["ref"] = _ref;
			j["underlying_symbol"] = _underlying_symbol;
			j["is_bull"] = _is_bull;
			j["is_buying"] = _is_buying;
			j["is_selling"] = _is_selling;
			j["auto_buy"] = _auto_buy;
			j["auto_sell"] = _auto_sell;
			j["is_reset_position"] = _is_reset_position;
			j["last_trigger_price"] = _last_trigger_price;
			j["last_price"] = _last_price;
			j["sell_type"] = _sell_type;
			return j;
		}
		json to_minor_json() const
		{
			json j;
			j["auto_buy"] = _auto_buy;
			j["auto_sell"] = _auto_sell;
			return j;
		}

		~pair() = default;
		pair(const pair&) = default;
		pair(pair&&) = default;
		pair& operator= (const pair&) = default;
		pair& operator= (pair&&) = default;
	public:
		unsigned long long default_buy_price()
		{
			unsigned long long price = _buy_price;
			auto it = stockWarrantomdcMap.find(_warrant_code);
			if (stockWarrantomdcMap.end() != it)
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
			unsigned long long price = _sell_price;
			auto it = stockWarrantomdcMap.find(_warrant_code);
			if (stockWarrantomdcMap.end() != it)
			{
				auto& tradable = it->second;
				if (0 != tradable.m_Bid[0].m_iPrice)
				{
					price = static_cast<unsigned long long>(tradable.m_Bid[0].m_iPrice) * 100000;
				}
			}
			return price;
		}
		bool cancelorder()
		{
			_algo->Log("Code = " + to_string(_warrant_code) + " Action=Cancel Order,  Auto Sell ID=" + to_string(_auto_sell_id));


			if(_is_selling == true && _sell_type ==  dbp::top::order_type::pl && _auto_sell_id > 0){
				auto result = _algo->_u.cancel_order(_auto_sell_id);
				return result;
			}
			return false;
		}
		buy_result buy(unsigned long long price = 0, bool is_auto = true, unsigned long long quantity = 0)
		{
			if (is_auto)
			{
			}
			if (0 == quantity)
			{
				quantity = _auto_buy_quantity;
			}
			if (0 == price)
			{
				price = default_buy_price();
			}
			if (_is_buying)
			{
				return buy_result::BUYING;
			}

			_is_buying = true;
			_auto_buy = false;

			auto odr = _algo->_u.new_order(
					_algo,
					quantity,
					price,
					_warrant_code,
					dbp::top::order_side::buy,
					dbp::top::order_type::sl,
					dbp::top::aon_type::non_ano,
					dbp::top::ignore_price_type::ignore,
					0);
			if (odr.is_valid())
			{
				_algo->_o_map[odr.order_id] = this;
				if (is_auto)
				{
					_auto_buy_id = odr.order_id;
				}
				return buy_result::SUCCESS;
			}
			else
			{
				_is_buying = false;
				_auto_sell = false;

				on_order(odr);

				if(std::strcmp(odr.reject_reason,"Exceed Buy Power") == 0){
					return buy_result::EXCEED_BUY_POWER;
				}

				if(std::strcmp(odr.reject_reason,"Not Ready") == 0 ){
					return buy_result::NOT_READY;
				}

				return buy_result::UNKNOWN;
			}

		}
		sell_result sell(unsigned long long price = 0, bool is_auto = true, unsigned long long quantity = 0)
		{
			if (is_auto)
			{
			}
			if (0 == quantity)
			{
				quantity = _position;
			}
			if (0 == price)
			{
				price = default_sell_price();
			}
			if (0 == quantity)
			{
				return sell_result::NOTHING_TO_SELL;
			}
			else if (_is_selling)
			{
				return sell_result::SELLING;
			}
			else if (quantity > _position)
			{
				return sell_result::SHORT_SELL;
			}
			_is_selling = true;
			_sell_type = dbp::top::order_type::sl;
			//_auto_sell = false;
			auto odr = _algo->_u.new_order(
					_algo,
					quantity,
					price,
					_warrant_code,
					dbp::top::order_side::sell,
					dbp::top::order_type::sl,
					dbp::top::aon_type::non_ano,
					dbp::top::ignore_price_type::ignore,
					0);
			if (odr.is_valid())
			{
				_algo->_o_map[odr.order_id] = this;
				if (is_auto)
				{
					_auto_sell_id = odr.order_id;
				}
				return sell_result::SUCCESS;
			}
			else
			{
				return sell_result::NEW_SELL_ODR_FAIL;
			}
		}
		sell_result sell_limit(unsigned long long price = 0, bool is_auto = true, unsigned long long quantity = 0)
		{


			_algo->Log("Code = " + to_string(_warrant_code) + " Action=Do Sell Limit");

			if (is_auto)
			{
			}
			if (0 == quantity)
			{
				quantity = _position;
			}
			if (0 == price)
			{
				price = default_sell_price();
			}
			if (0 == quantity)
			{
				return sell_result::NOTHING_TO_SELL;
			}
			else if (_is_selling)
			{
				return sell_result::SELLING;
			}
			else if (quantity > _position)
			{
				return sell_result::SHORT_SELL;
			}

			_is_selling = true;
			_sell_type = dbp::top::order_type::pl;
			//_auto_sell = false;
			auto odr = _algo->_u.new_order(
					_algo,
					quantity,
					price,
					_warrant_code,
					dbp::top::order_side::sell,
					dbp::top::order_type::pl,
					dbp::top::aon_type::non_ano,
					dbp::top::ignore_price_type::ignore,
					0);


			_algo->Log("Code = " + to_string(_warrant_code) + " Action=Do Sell Limit , internal Order id = " + to_string(odr.order_id));

			if (odr.is_valid())
			{
				_algo->_o_map[odr.order_id] = this;
				if (is_auto)
				{
					_auto_sell_id = odr.order_id;
				}
				if(_sell_type == dbp::top::order_type::pl)
				{
					_auto_sell_id = odr.order_id;
				}
				return sell_result::SUCCESS;
			}
			else
			{
				return sell_result::NEW_SELL_ODR_FAIL;
			}
		}
		void on_trade(const Tradable& tradable)
		{
			auto type = tradable.m_TradeType;
			auto side = tradable.m_TradeSide;
			long long diff = 0;
			auto bid_price = static_cast<unsigned long long>(tradable.m_Bid[0].m_iPrice) * 100000;
			auto ask_price = static_cast<unsigned long long>(tradable.m_Ask[0].m_iPrice) * 100000;
			auto trade_price = static_cast<unsigned long long>(tradable.m_LastTradePrice) * 100000;



			auto best_bid_vol = static_cast<unsigned long long>(tradable.m_Bid[0].m_uQuantity);
			auto best_ask_vol = static_cast<unsigned long long>(tradable.m_Ask[0].m_uQuantity);


			if (_position > 0)
			{
				if (_auto_sell)
				{
					if(wbestbid == 0){
						wbestbid = default_sell_price();
					}
					if (0 != wbestbid && (wbestbid == _bottom_price || (wbestbid >= _ceiling_price  && _ceiling_price > 0)))
					{

						if(sell(wbestbid) == sell_result::SUCCESS){
							#ifndef NOT_MEASURE
								auto msg = algo_latency_pool.get_obj();
								msg->al = _algo;
								msg->algo_name = _algo->_name;
								msg->id = _algo->_u.get_id();
								msg->ref = _ref;
								msg->pkg_tm = tradable.m_PkgTime;
								msg->m_tm = tradable.m_MsgTime;
								msg->o_tm = dbp::tools::srv::current();
								ouputQueue.enqueue(msg);
							#endif
						}
					}

				}
			}




			if (!(_underlying_symbol[0] < '0' || _underlying_symbol[0] > '9')){
				if (0 != type && 100 != type)
					return;
			}
			if (_is_bull)
			{




				auto buyratio = static_cast<unsigned long long>(((best_ask_vol- tradable.m_AccumulateBuyQuantity+1)*100) / (best_bid_vol + best_ask_vol - tradable.m_AccumulateBuyQuantity - tradable.m_AccumulateSellQuantity + 1));
				auto sellratio = static_cast<unsigned long long>(((best_bid_vol-tradable.m_AccumulateSellQuantity+1)*100) / (best_bid_vol + best_ask_vol - tradable.m_AccumulateBuyQuantity - tradable.m_AccumulateSellQuantity + 1));


				auto trade_quantity = static_cast<long long>(tradable.m_AccumulateBuyQuantity);
				diff = static_cast<long long>(tradable.m_Ask[0].m_uQuantity) - static_cast<long long>(_early_buy_qty);

/*
				fprintf(stderr, "info Code BULL: %u : %llu - Bid:%llu | Ask:%llu - %llu \n", _warrant_code, best_bid_vol, bid_price, ask_price, best_ask_vol);
				fprintf(stderr, "info Code: %u : Trade Price : %llu Trade Vol = %llu    Trade Side: %d \n", _warrant_code,trade_price,tradable.m_LastTradeQuantity, side);
				fprintf(stderr, "info Code: %u :  m_AccumulateSellQuantity: %llu -- m_AccumulateBuyQuantity : %llu  \n", _warrant_code,tradable.m_AccumulateSellQuantity, tradable.m_AccumulateBuyQuantity);
				fprintf(stderr, "info Code: %u : Ratio Buy : %llu  Ratio Sell: %llu \n",_warrant_code, buyratio, sellratio);
				fprintf(stderr, "info Code: %u : _ratio_buy : %llu  _ratio_sell: %llu \n",_warrant_code, _ratio_buy, _ratio_sell);
*/

				if (
						TradeSide::BUY_SIDE == side &&
						(
								(
										trade_price == _buy_trriger && trade_price == ask_price &&
										trade_quantity >= diff &&
										0 != tradable.m_Ask[0].m_uQuantity && _is_buying == false && _auto_buy == true && _is_selling == false
								)||(_ratio_buy > 0 && buyratio<= _ratio_buy && trade_price == _buy_trriger && trade_price == ask_price && _is_buying == false && _auto_buy == true && _is_selling == false )
						)
					)
				{

					if (_auto_buy)
					{
						_algo->Log("Code = " + to_string(_warrant_code) + " Action=BUY Price=" + to_string(_buy_price) + " Quantity=" + to_string(_auto_buy_quantity));
						if(buy(_buy_price) == buy_result::SUCCESS){
#ifndef NOT_MEASURE
							auto msg = algo_latency_pool.get_obj();
							msg->al = _algo;
							msg->algo_name = _algo->_name;
							msg->id = _algo->_u.get_id();
							msg->ref = _ref;
							msg->pkg_tm = tradable.m_PkgTime;
							msg->m_tm = tradable.m_MsgTime;
							msg->o_tm = dbp::tools::srv::current();
							ouputQueue.enqueue(msg);

							fprintf(stderr, "info Code BULL: %u : %llu - Bid:%llu | Ask:%llu - %llu \n", _warrant_code, best_bid_vol, bid_price, ask_price, best_ask_vol);
							fprintf(stderr, "info Code: %u : Trade Price : %llu Trade Vol = %llu    Trade Side: %d \n", _warrant_code,trade_price,tradable.m_LastTradeQuantity, side);
							fprintf(stderr, "info Code: %u :  m_AccumulateSellQuantity: %llu -- m_AccumulateBuyQuantity : %llu  \n", _warrant_code,tradable.m_AccumulateSellQuantity, tradable.m_AccumulateBuyQuantity);
							fprintf(stderr, "info Code: %u : Ratio Buy : %llu  Ratio Sell: %llu \n",_warrant_code, buyratio, sellratio);
							fprintf(stderr, "info Code: %u : _ratio_buy : %llu  _ratio_sell: %llu \n",_warrant_code, _ratio_buy, _ratio_sell);

#endif
						}
					}
				}

				trade_quantity = static_cast<long long>(tradable.m_AccumulateSellQuantity);
				diff = static_cast<long long>(tradable.m_Bid[0].m_uQuantity)  - static_cast<long long>(_early_sell_qty);
				if (
						TradeSide::SELL_SIDE == side &&
						(
							(
									trade_price == _sell_trriger && trade_price == bid_price &&
									trade_quantity >= diff &&
									0 != tradable.m_Bid[0].m_uQuantity && _is_selling == false && _auto_sell == true && _is_buying == false
							)||(_ratio_sell > 0 && sellratio<= _ratio_sell && trade_price == _sell_trriger && trade_price == bid_price && _is_selling == false && _auto_sell == true && _is_buying == false)
						)
					)
				{
					if (_position > 0)
					{
						if (_auto_sell)
						{
							_algo->Log("Code = " + to_string(_warrant_code) + " Action=SELL Price=" + to_string(_sell_price) + " Position=" + to_string(_position));
							if(sell(_sell_price) == sell_result::SUCCESS){
#ifndef NOT_MEASURE

								fprintf(stderr, "info Code BULL: %u : %llu - Bid:%llu | Ask:%llu - %llu \n", _warrant_code, best_bid_vol, bid_price, ask_price, best_ask_vol);
								fprintf(stderr, "info Code: %u : Trade Price : %llu Trade Vol = %llu    Trade Side: %d \n", _warrant_code,trade_price,tradable.m_LastTradeQuantity, side);
								fprintf(stderr, "info Code: %u :  m_AccumulateSellQuantity: %llu -- m_AccumulateBuyQuantity : %llu  \n", _warrant_code,tradable.m_AccumulateSellQuantity, tradable.m_AccumulateBuyQuantity);
								fprintf(stderr, "info Code: %u : Ratio Buy : %llu  Ratio Sell: %llu \n",_warrant_code, buyratio, sellratio);
								fprintf(stderr, "info Code: %u : _ratio_buy : %llu  _ratio_sell: %llu \n",_warrant_code, _ratio_buy, _ratio_sell);


								auto msg = algo_latency_pool.get_obj();
								msg->al = _algo;
								msg->algo_name = _algo->_name;
								msg->id = _algo->_u.get_id();
								msg->ref = _ref;
								msg->pkg_tm = tradable.m_PkgTime;
								msg->m_tm = tradable.m_MsgTime;
								msg->o_tm = dbp::tools::srv::current();
								ouputQueue.enqueue(msg);
#endif
							}
						}
					}
				}
			}
			else
			{
				//auto buyratio = static_cast<unsigned long long>((best_bid_vol- tradable.m_AccumulateSellQuantity) / (best_bid_vol + best_ask_vol - tradable.m_AccumulateBuyQuantity - tradable.m_AccumulateSellQuantity) * 100);
				//auto sellratio = static_cast<unsigned long long>((best_ask_vol-tradable.m_AccumulateBuyQuantity) / (best_bid_vol + best_ask_vol - tradable.m_AccumulateBuyQuantity - tradable.m_AccumulateSellQuantity) * 100);

				auto buyratio = static_cast<unsigned long long>(((best_bid_vol- tradable.m_AccumulateSellQuantity+1)*100) / (best_bid_vol + best_ask_vol - tradable.m_AccumulateBuyQuantity - tradable.m_AccumulateSellQuantity+1));
				auto sellratio = static_cast<unsigned long long>(((best_ask_vol-tradable.m_AccumulateBuyQuantity+1)*100) / (best_bid_vol + best_ask_vol - tradable.m_AccumulateBuyQuantity - tradable.m_AccumulateSellQuantity+1));



				auto trade_quantity = static_cast<long long>(tradable.m_AccumulateSellQuantity);
				diff = static_cast<long long>(tradable.m_Bid[0].m_uQuantity) - static_cast<long long>(_early_buy_qty);

				/*
				fprintf(stderr, "info Code: %u : %llu - Bid:%llu | Ask:%llu - %llu \n", _underlying_code, best_bid_vol, bid_price, ask_price, best_ask_vol);
				fprintf(stderr, "info Code: %u : Trade Price : %llu  Trade Side: %d \n",_underlying_code, trade_price, side);
				fprintf(stderr, "info Code: %u :  m_AccumulateSellQuantity: %llu -- m_AccumulateBuyQuantity : %llu  \n", _warrant_code,tradable.m_AccumulateSellQuantity, tradable.m_AccumulateBuyQuantity);
				fprintf(stderr, "info Code: %u : Ratio Buy : %llu  Ratio Sell: %llu \n", _underlying_code,buyratio, sellratio);
				fprintf(stderr, "info Code: %u : _ratio_buy : %llu  _ratio_sell: %llu \n",_underlying_code, _ratio_buy, _ratio_sell);
*/

				if (
						TradeSide::SELL_SIDE == side &&
						(
								(
										trade_price == _buy_trriger && trade_price == bid_price &&
										trade_quantity >= (diff) &&
										0 != tradable.m_Bid[0].m_uQuantity && _is_buying == false && _auto_buy == true  && _is_selling == false
								)||(_ratio_buy > 0 && buyratio <= _ratio_buy && trade_price == _buy_trriger && trade_price == bid_price && _is_buying == false && _auto_buy == true  && _is_selling == false)
						)
					)
				{

					if (_auto_buy)
					{
						if(buy(_buy_price) == buy_result::SUCCESS){
#ifndef NOT_MEASURE
							fprintf(stderr, "info Code: %u : %llu - Bid:%llu | Ask:%llu - %llu \n", _underlying_code, best_bid_vol, bid_price, ask_price, best_ask_vol);
							fprintf(stderr, "info Code: %u : Trade Price : %llu  Trade Side: %d \n",_underlying_code, trade_price, side);
							fprintf(stderr, "info Code: %u :  m_AccumulateSellQuantity: %llu -- m_AccumulateBuyQuantity : %llu  \n", _warrant_code,tradable.m_AccumulateSellQuantity, tradable.m_AccumulateBuyQuantity);
							fprintf(stderr, "info Code: %u : Ratio Buy : %llu  Ratio Sell: %llu \n", _underlying_code,buyratio, sellratio);
							fprintf(stderr, "info Code: %u : _ratio_buy : %llu  _ratio_sell: %llu \n",_underlying_code, _ratio_buy, _ratio_sell);



							auto msg = algo_latency_pool.get_obj();
							msg->al = _algo;
							msg->algo_name = _algo->_name;
							msg->id = _algo->_u.get_id();
							msg->ref = _ref;
							msg->pkg_tm = tradable.m_PkgTime;
							msg->m_tm = tradable.m_MsgTime;
							msg->o_tm = dbp::tools::srv::current();
							ouputQueue.enqueue(msg);
#endif
						}
					}
				}
				trade_quantity = static_cast<long long>(tradable.m_AccumulateBuyQuantity);
				diff = static_cast<long long>(tradable.m_Ask[0].m_uQuantity)  - static_cast<long long>(_early_sell_qty);

				if (
						TradeSide::BUY_SIDE == side &&
						(
								(
										trade_price == _sell_trriger &&
										trade_quantity >= (diff) && trade_price == ask_price &&
										0 != tradable.m_Ask[0].m_uQuantity && _is_selling == false && _auto_sell == true  && _is_buying == false
								)||(_ratio_sell > 0 && sellratio<= _ratio_sell && trade_price == _sell_trriger && trade_price == ask_price  && _is_selling == false && _auto_sell == true  && _is_buying == false)
						)
					)
				{
					if (_position > 0)
					{

						if (_auto_sell)
						{
							if(sell(_sell_price) == sell_result::SUCCESS){
#ifndef NOT_MEASURE

								fprintf(stderr, "info Code: %u : %llu - Bid:%llu | Ask:%llu - %llu \n", _underlying_code, best_bid_vol, bid_price, ask_price, best_ask_vol);
								fprintf(stderr, "info Code: %u : Trade Price : %llu  Trade Side: %d \n",_underlying_code, trade_price, side);
								fprintf(stderr, "info Code: %u :  m_AccumulateSellQuantity: %llu -- m_AccumulateBuyQuantity : %llu  \n", _warrant_code,tradable.m_AccumulateSellQuantity, tradable.m_AccumulateBuyQuantity);
								fprintf(stderr, "info Code: %u : Ratio Buy : %llu  Ratio Sell: %llu \n", _underlying_code,buyratio, sellratio);
								fprintf(stderr, "info Code: %u : _ratio_buy : %llu  _ratio_sell: %llu \n",_underlying_code, _ratio_buy, _ratio_sell);


							auto msg = algo_latency_pool.get_obj();
							msg->al = _algo;
							msg->algo_name = _algo->_name;
							msg->id = _algo->_u.get_id();
							msg->ref = _ref;
							msg->pkg_tm = tradable.m_PkgTime;
							msg->m_tm = tradable.m_MsgTime;
							msg->o_tm = dbp::tools::srv::current();
							ouputQueue.enqueue(msg);
#endif
							}
						}
					}
				}
			}
		}
		void on_book(const Tradable& tradable)
		{
			auto best_bid_price = static_cast<unsigned long long>(tradable.m_Bid[0].m_iPrice) * 100000;
			auto best_ask_price = static_cast<unsigned long long>(tradable.m_Ask[0].m_iPrice) * 100000;
			wbestbid = best_bid_price;
			wbestask = best_ask_price;

			if (_position > 0){
				if (_auto_sell)
				{
					auto best_bid_price = static_cast<unsigned long long>(tradable.m_Bid[0].m_iPrice) * 100000;
					if (0 != best_bid_price && (best_bid_price == _bottom_price || (best_bid_price >= _ceiling_price && _buy_price >= _ceiling_price )))
					{

						if(sell(best_bid_price) == sell_result::SUCCESS){
							#ifndef NOT_MEASURE
								auto msg = algo_latency_pool.get_obj();
								msg->al = _algo;
								msg->algo_name = _algo->_name;
								msg->id = _algo->_u.get_id();
								msg->ref = _ref;
								msg->pkg_tm = tradable.m_PkgTime;
								msg->m_tm = tradable.m_MsgTime;
								msg->o_tm = dbp::tools::srv::current();
								ouputQueue.enqueue(msg);
							#endif
						}
					}
				}
			}
		}

		void on_order(const dbp::top::enhance_order& odr)
		{
			auto status = odr.status;
			auto side = odr.side;
			std::string strstatus = "";
			std::string strside = "";
			std::string strreason = "";

			if(dbp::top::order_status::queued == status){
				if(dbp::top::order_side::sell == side && dbp::top::order_type::pl == odr.type ){
					_odrid = odr.order_ref;
					strstatus = "queued";
				}
			}

			if (dbp::top::order_status::rejected == status || dbp::top::order_status::canceled == status || dbp::top::order_status::deleted == status || dbp::top::order_status::filled == status)
			{

				//fprintf(stderr, "info Code: %u : matched price : %llu  matched quantity : %llu filled quantity : %llu Order Status : %u Side : %u \n",_warrant_code, odr.match_price, odr.match_quantity, odr.filled_quantity, static_cast<unsigned int>(status), static_cast<unsigned char>(side));



				auto itPf = _algo->portfolioMap.find(odr.code);
				if(itPf == _algo->portfolioMap.end()){
					_algo->portfolioMap[odr.code] = new portfolio();
					_algo->portfolioMap[odr.code]->code = odr.code;
					_algo->portfolioMap[odr.code]->averagebuy = 0;
					_algo->portfolioMap[odr.code]->buyturnover = 0;
					_algo->portfolioMap[odr.code]->buyvolume = 0;
					_algo->portfolioMap[odr.code]->averagesell = 0;
					_algo->portfolioMap[odr.code]->sellturnover = 0;
					_algo->portfolioMap[odr.code]->sellvolume = 0;
					_algo->portfolioMap[odr.code]->profit = 0;
				}

				unsigned long long filledprice =  odr.match_price;
				if(filledprice == 0){
					for(unsigned int i=0; i<odr.match_records.size(); i++){
						dbp::top::match_record mr = odr.match_records[i];
						_algo->Log(">>> Match Price = " + to_string(mr.match_price) + " Match Qty = " + to_string(mr.match_quantity));
						if(mr.match_price > 0){
							filledprice = mr.match_price;
							break;
						}
					}
				}


				_algo->Log("Trade Code = " + to_string(_warrant_code) + " Match Price = " + to_string(filledprice) + " Match Quantity = " + to_string(odr.match_quantity) + "Status = " + to_string(static_cast<unsigned int>(status)) + " Side = " +  to_string(static_cast<unsigned char>(side)));

				if (dbp::top::order_side::buy == side)
				{

					strside = "BUY";
					if (dbp::top::order_status::filled == status){
						_position += odr.filled_quantity;
						strstatus = "filled";
						_last_trigger_price = _buy_trriger;
						_last_price = odr.match_price;
						_algo->_Profit -= odr.filled_quantity *  odr.match_price;

						portfolio* pf = _algo->portfolioMap[odr.code];
						//long long newaveragebuyprice = (pf->buyturnover + odr.filled_quantity * odr.match_price )/(odr.filled_quantity + pf->buyvolume);

						pf->buyvolume += odr.filled_quantity/100000ull;
						pf->buyturnover += odr.filled_quantity/100000ull * filledprice/100000ull;
						pf->profit -= odr.filled_quantity/100000ull * filledprice/100000ull;

						//pf->buyturnover += odr.filled_quantity/100000ull * odr.match_price/100000ull;
						//pf->profit -= odr.filled_quantity/100000ull * odr.match_price/100000ull;

						pf->averagebuy = static_cast<long long>(pf->buyturnover / pf->buyvolume) ;


					}
					/*
					if (odr.order_id == _auto_buy_id && dbp::top::order_status::filled != status)
					{
						_auto_buy_quantity -= _position;
					}
					*/
					if (dbp::top::order_status::canceled == status || dbp::top::order_status::rejected == status ){
						_auto_sell = false;
						strstatus = "cancel";
						strreason = std::string(odr.reject_reason);
					}

					_is_buying = false;
				}
				else if (dbp::top::order_side::sell == side)
				{
					strside = "SELL";
					_is_selling = false;

					if (dbp::top::order_status::filled == status){
						strstatus = "filled";
						if(odr.filled_quantity > 0){
							_position -= odr.filled_quantity;
							if(odr.filled_quantity == odr.quantity){
								_auto_sell = false;
							}
							_last_trigger_price = _sell_trriger;
							//_last_price = odr.match_price;
							_last_price = filledprice;

							//_algo->_Profit += odr.filled_quantity * odr.match_price;
							_algo->_Profit += odr.filled_quantity * filledprice;

							portfolio* pf = _algo->portfolioMap[odr.code];


							//long long newaveragesellprice = (pf->sellturnover + odr.filled_quantity * odr.match_price )/(odr.filled_quantity + pf->sellvolume);

							pf->sellvolume += odr.filled_quantity/100000ull;
							//pf->sellturnover += odr.filled_quantity/100000ull * odr.match_price/100000ull;
							//pf->profit += odr.filled_quantity/100000ull * odr.match_price/100000ull;

							pf->sellturnover += odr.filled_quantity/100000ull * filledprice/100000ull;
							pf->profit += odr.filled_quantity/100000ull * filledprice/100000ull;

							pf->averagesell = static_cast<long long>(pf->sellturnover / pf->sellvolume);

						}
					}else{
						strstatus = "cancel";
						strreason = std::string(odr.reject_reason);
					}
				}
			}

			algo_odr_msg* msg = algo_odr_msg_pool.get_obj();
			msg->al = _algo;
			msg->algo_name = _algo->_name;
			msg->id = _algo->_u.get_id();
			msg->ref = _ref;
			msg->odr = odr;
			auto& p = msg->p;
			p._auto_buy = _auto_buy;
			p._auto_sell = _auto_sell;
			p._buy_price = _buy_price;
			p._buy_trriger = _buy_trriger;
			p._auto_buy_quantity = _auto_buy_quantity;
			p._position = _position;
			msg->filled_price = odr.match_price;
			msg->filled_quantity = odr.filled_quantity;

			msg->last_trigger_price = _last_trigger_price;
			msg->last_price = _last_price;

			msg->status = strstatus;
			msg->side = strside;
			msg->reason = strreason;
			ouputQueue.enqueue(msg);
		}
		unsigned int underlying_code() const
		{
			return _underlying_code;
		}
		const std::string& underlying_symbol() const
		{
			return _underlying_symbol;
		}
		unsigned int warrant_code() const
		{
			return _warrant_code;
		}
		bool is_bull() const
		{
			return _is_bull;
		}
		unsigned long long buy_trriger() const
		{
			return _buy_trriger;
		}
		unsigned long long sell_trriger() const
		{
			return _sell_trriger;
		}
		unsigned long long buy_price() const
		{
			return _buy_price;
		}
		unsigned long long sell_price() const
		{
			return _sell_price;
		}
		unsigned long long bottom_price() const
		{
			return _bottom_price;
		}
		unsigned long long ceiling_price() const
		{
			return _ceiling_price;
		}
		unsigned long long auto_buy_quantity() const
		{
			return _auto_buy_quantity;
		}
		unsigned long long position() const
		{
			return _position;
		}
		unsigned long long early_buy_qty() const
		{
			return _early_buy_qty;
		}
		unsigned long long early_sell_qty() const
		{
			return _early_sell_qty;
		}
		unsigned long long ratio_buy() const
		{
			return _ratio_buy;
		}
		unsigned long long ratio_sell() const
		{
			return _ratio_sell;
		}
		bool auto_buy() const
		{
			return _auto_buy;
		}
		bool auto_sell() const
		{
			return _auto_sell;
		}
		const std::string& ref()
		{
			return _ref;
		}
		bool is_reset_position() const
		{
			return _is_reset_position;
		}
		unsigned long long auto_buy_id() const
		{
			return _auto_buy_id;
		}
		unsigned long long auto_sell_id() const
		{
			return _auto_sell_id;
		}
		unsigned long long last_trigger_price() const
		{
			return _last_trigger_price;
		}
		unsigned long long last_price() const
		{
			return _last_price;
		}
		bool is_buying() const
		{
			return _is_buying;
		}
		bool is_selling() const
		{
			return _is_selling;
		}
		void set_ref(const std::string& ref)
		{
			_ref = ref;
		}
		void set_position(unsigned long long position)
		{
			_position = position;
		}
		void set_auto_buy_id(unsigned long long auto_buy_id)
		{
			_auto_buy_id = auto_buy_id;
		}
		void set_auto_sell_id(unsigned long long auto_sell_id)
		{
			_auto_sell_id = auto_sell_id;
		}
		void set_is_buying (bool is_buying)
		{
			_is_buying = is_buying;
		}
		void set_is_selling (bool is_selling)
		{
			_is_selling = is_selling;
		}
		bool is_omdd() const
		{
			return _is_omdd;
		}
		void set_early_buy_qty(unsigned long long qty)
		{
			_early_buy_qty = qty;
		}
		void set_early_sell_qty(unsigned long long qty)
		{
			_early_sell_qty = qty;
		}
		void set_auto_buy(bool auto_buy)
		{
			_auto_buy = auto_buy;
		}
		void set_auto_sell(bool auto_sell)
		{
			_auto_sell = auto_sell;
		}
		void set_last_trigger_price(unsigned long long price)
		{
			_last_trigger_price= price;
		}
		void set_last_price(unsigned long long price)
		{
			_last_price = price;
		}
	};
private:
	using pair_map = std::unordered_map<std::string, pair>;
private:
	pair_map _p_map;
	ThreadLogger* logger;
private:
	struct algo_odr_msg: public algo_msg_base
	{
		dbp::top::enhance_order odr;
		pair p;

		unsigned long long filled_price;
		unsigned long long filled_quantity;
		std::string status;
		std::string side;
		std::string reason;

		unsigned long long last_trigger_price;
		unsigned long long last_price;

		algo_odr_msg():
			algo_msg_base(),
			odr(),
			p(),
			filled_price(0),
			filled_quantity(0),
			status(""),
			side(""),
			reason(""),
			last_trigger_price(0),
			last_price(0)
		{
		}
		virtual nlohmann::json to_json() const
		{
			auto j = algo_msg_base::to_json();
			j["msg_type"] = "semi_algo_odr_msg";
			//j["odr"] = odr.to_json();
			j["auto_buy"] = p._auto_buy;
			j["auto_sell"] = p._auto_sell;
			j["buy_trigger"] = p._buy_trriger;
			j["buy_price"] = p._buy_price;
			j["auto_buy_qty"] = p._auto_buy_quantity;
			j["position"] = p._position;

			j["status"] = status;
			j["filled_price"] = filled_price;
			j["filled_quantity"] = filled_quantity;
			j["last_trigger_price"] = last_trigger_price;
			j["last_price"] = last_price;
			j["side"] = side;
			j["reason"] = reason;

			//j["recovery"] = true;
			return j;
		}
		virtual void on_command()
		{
		}
		virtual void release()
		{
			algo_odr_msg_pool.release_obj(this);
		}
		virtual ~algo_odr_msg() = default;
	};
	struct algo_err_msg: public algo_msg_base
	{
		std::string err;
		std::string action;
		algo_err_msg():
			algo_msg_base(),
			err("")
		{
		}
		virtual nlohmann::json to_json() const
		{
			auto j = algo_msg_base::to_json();
			j["msg_type"] = "semi_algo_err_msg";
			j["action"] = action;
			j["error"] = err;
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
	struct algo_odr_position: public algo_msg_base
	{
		std::unordered_map<std::string, unsigned long long> position;
		algo_odr_position():
			algo_msg_base(),
			position()
		{
		}
		virtual nlohmann::json to_json() const
		{
			auto j = algo_msg_base::to_json();
			j["msg_type"] = "semi_algo_odr_position";
			j["position"] = nlohmann::json();
			for (const auto& it : position)
			{
				j["position"][it.first] = it.second;
			}
			return j;
		}
		virtual void on_command()
		{
			auto* self = dynamic_cast<semi*>(al);
			self->position(*this);
			ouputQueue.enqueue(this);
		}
		virtual void release()
		{
			algo_odr_position_pool.release_obj(this);
		}
		virtual ~algo_odr_position() = default;
	};
	struct algo_getprofit_msg: public algo_msg_base
	{
		long long profit;
		vector<json> codev;
		algo_getprofit_msg():
			algo_msg_base(),
			profit(0)
		{
		}
		virtual nlohmann::json to_json() const
		{
			auto j = algo_msg_base::to_json();
			j["msg_type"] = "algo_getprofit_msg";
			j["portfolio"] = nlohmann::json::array();
			for(unsigned int i=0; i<codev.size(); i++){
				j["portfolio"].push_back(codev[i]);
			}

			return j;
		}
		virtual void on_command()
		{
			auto* self = dynamic_cast<semi*>(al);

			for (auto& it: self->portfolioMap) {
			    // Do stuff
				portfolio* p = it.second;
				json jw;
				jw["code"] = it.first;
				jw["averagebuy"] = p->averagebuy;
				jw["averagesell"] = p->averagesell;
				jw["buyturnover"] = p->buyturnover;
				jw["sellturnover"] = p->sellturnover;
				jw["buyvolume"] = p->buyvolume;
				jw["sellvolume"] = p->sellvolume;
				jw["profit"] = p->profit;
				codev.push_back(jw);
			}
			ouputQueue.enqueue(this);
		}
		virtual void release()
		{
			algo_getprofit_msg_pool.release_obj(this);
		}
		virtual ~algo_getprofit_msg() = default;
	};
#ifndef NOT_MEASURE
	struct algo_latency: public algo_msg_base
	{
		unsigned long long pkg_tm;
		unsigned long long m_tm;
		unsigned long long o_tm;
		algo_latency():
			algo_msg_base(),
			pkg_tm(0),
			m_tm(0),
			o_tm(0)
		{
		}
		virtual nlohmann::json to_json() const
		{
			auto j = algo_msg_base::to_json();
			j["msg_type"] = "semi_algo_latency";
			j["hkex_to_order_latency"] = pkg_tm - m_tm;
			j["tick_to_order_latency"] = o_tm - m_tm;

			return j;
		}
		virtual void on_command()
		{
			//ouputQueue.enqueue(this);
		}
		virtual void release()
		{
			algo_latency_pool.release_obj(this);
		}
		virtual ~algo_latency() = default;
	};
#endif
	struct algo_set: public algo_msg_base
	{
		pair p;
		std::string result;
		bool no_change;
		algo_set():
		algo_msg_base(),
		p(),
		result(""),
		no_change()
		{
		}
		virtual nlohmann::json to_json() const
		{
			auto j = algo_msg_base::to_json();
			j["msg_type"] = "semi_algo_set";
			j["pair"] = p.to_minor_json();
			j["result"] = result;
			j["no_change"] = no_change;
			return j;
		}
		virtual void on_command()
		{
			auto* self = dynamic_cast<semi*>(al);
			auto p2 = p;
			result = self->set_pair(std::move(p2), no_change);
			ouputQueue.enqueue(this);
		}
		virtual void release()
		{
			algo_set_pool.release_obj(this);
		}
		virtual ~algo_set() = default;
	};
	struct algo_del: public algo_msg_base
	{
		pair* p;
		std::string result;
		algo_del():
			algo_msg_base(),
			p(nullptr),
			result("")
		{
		}
		virtual nlohmann::json to_json() const
		{
			auto j = algo_msg_base::to_json();
			j["msg_type"] = "semi_algo_del";
			if (p)
				j["pair"] = p->to_minor_json();
			else
				j["pair"] = nullptr;
			j["result"] = result;
			return j;
		}
		virtual void on_command()
		{
			auto* self = dynamic_cast<semi*>(al);
			result = self->delete_pair(ref, p);
			ouputQueue.enqueue(this);
		}
		virtual void release()
		{
			algo_del_pool.release_obj(this);
		}
		virtual ~algo_del() = default;
	};
	struct algo_cancel: public algo_msg_base
	{
		pair* p;
		std::string result;
		algo_cancel():
			algo_msg_base(),
			p(nullptr),
			result("")
		{
		}
		virtual nlohmann::json to_json() const
		{
			auto j = algo_msg_base::to_json();
			j["msg_type"] = "semi_algo_cancel";
			if (p)
				j["pair"] = p->to_minor_json();
			else
				j["pair"] = nullptr;
			j["result"] = result;
			return j;
		}
		virtual void on_command()
		{
			auto* self = dynamic_cast<semi*>(al);
			result = self->cancel_order(ref, p);
			ouputQueue.enqueue(this);
		}
		virtual void release()
		{
			algo_cancel_pool.release_obj(this);
		}
		virtual ~algo_cancel() = default;
	};
	struct algo_get: public algo_msg_base
	{
		pair* p;
		std::string result;
		algo_get():
			algo_msg_base(),
			p(nullptr),
			result("")
		{
		}
		virtual nlohmann::json to_json() const
		{
			auto j = algo_msg_base::to_json();
			j["msg_type"] = "semi_algo_get";
			if (p)
				j["pair"] = p->to_json();
			else
				j["pair"] = nullptr;
			j["result"] = result;
			return j;
		}
		virtual void on_command()
		{
			auto* self = dynamic_cast<semi*>(al);
			result = self->get_pair(ref, p);
			ouputQueue.enqueue(this);
		}
		virtual void release()
		{
			algo_get_pool.release_obj(this);
		}
		virtual ~algo_get() = default;
	};
	struct algo_force_buy: public algo_msg_base
	{
		pair* p;
		std::string result;
		unsigned long long price;
		unsigned long long quantity;
		algo_force_buy():
			algo_msg_base(),
			p(nullptr),
			result(""),
			price(0),
			quantity(0)
		{
		}
		virtual nlohmann::json to_json() const
		{
			auto j = algo_msg_base::to_json();
			j["msg_type"] = "semi_algo_force_buy";
			if (p)
				j["pair"] = p->to_minor_json();
			else
				j["pair"] = nullptr;
			j["price"] = price;
			j["quantity"] = quantity;
			j["result"] = result;
			return j;
		}
		virtual void on_command()
		{
			auto* self = dynamic_cast<semi*>(al);
			result = self->force_buy(price, quantity, p, ref);
			if(result != "Exceed Buy Power" && result != "Not Ready"){
				ouputQueue.enqueue(this);
			}

		}
		virtual void release()
		{
			algo_force_buy_pool.release_obj(this);
		}
		virtual ~algo_force_buy() = default;
	};
	struct algo_force_sell: public algo_msg_base
	{
		pair* p;
		std::string result;
		unsigned long long price;
		unsigned long long quantity;
		algo_force_sell():
			algo_msg_base(),
			p(nullptr),
			result(""),
			price(0),
			quantity(0)
		{
		}
		virtual nlohmann::json to_json() const
		{
			auto j = algo_msg_base::to_json();
			j["msg_type"] = "semi_algo_force_sell";
			if (p)
				j["pair"] = p->to_minor_json();
			else
				j["pair"] = nullptr;
			j["price"] = price;
			j["quantity"] = quantity;
			j["result"] = result;
			return j;
		}
		virtual void on_command()
		{
			auto* self = dynamic_cast<semi*>(al);
			result = self->force_sell(price, quantity, p, ref);
			ouputQueue.enqueue(this);
		}
		virtual void release()
		{
			algo_force_sell_pool.release_obj(this);
		}
		virtual ~algo_force_sell() = default;
	};
	struct algo_limit_sell: public algo_msg_base
	{
		pair* p;
		std::string result;
		unsigned long long price;
		unsigned long long quantity;
		algo_limit_sell():
			algo_msg_base(),
			p(nullptr),
			result(""),
			price(0),
			quantity(0)
		{
		}
		virtual nlohmann::json to_json() const
		{
			auto j = algo_msg_base::to_json();
			j["msg_type"] = "semi_algo_limit_sell";
			if (p)
				j["pair"] = p->to_minor_json();
			else
				j["pair"] = nullptr;
			j["price"] = price;
			j["quantity"] = quantity;
			j["result"] = result;
			return j;
		}
		virtual void on_command()
		{
			auto* self = dynamic_cast<semi*>(al);
			result = self->limit_sell(price, quantity, p, ref);
			ouputQueue.enqueue(this);
		}
		virtual void release()
		{
			algo_limit_sell_pool.release_obj(this);
		}
		virtual ~algo_limit_sell() = default;
	};
public:
	semi() = delete;
	semi(user& u, const std::string& name);
	semi(const algo&) = delete;
	semi(algo&&) = delete;
	semi& operator= (const algo&) = delete;
	semi& operator= (algo&&) = delete;
	std::string set_pair(pair&& p, bool no_change);
	std::string delete_pair(const std::string& ref, pair*& pref);
	std::string get_pair(const std::string& ref, pair*& pref);
	std::string cancel_order(const std::string& ref, pair*& pref);
	std::string force_buy(unsigned long long price, unsigned long long quantity, pair*& pref, const std::string& ref);
	std::string force_sell(unsigned long long price, unsigned long long quantity, pair*& pref, const std::string& ref);
	std::string limit_sell(unsigned long long price, unsigned long long quantity, pair*& pref, const std::string& ref);
	void position(algo_odr_position& msg) const;
	virtual ~semi() = default;
	virtual void on_omdc_book(const Tradable&);
	virtual void on_omdd_book(const Tradable&);
	virtual void on_tcp_book(const Tradable&);
	virtual void on_omdc_trade(const Tradable&);
	virtual void on_omdd_trade(const Tradable&);
	virtual void on_tcp_trade(const Tradable&);
	virtual void handler_order(const dbp::top::enhance_order&);
	virtual void handle_command(algo_msg_base&);
	virtual algo_msg_base* json_to_msg(json& msg);
	virtual std::string get_lib_name();
	virtual void Log(std::string msg);
public:
	static rapid_ring::spmc_ring_buffer_object_pool<algo_odr_msg, 8192> algo_odr_msg_pool;
	static rapid_ring::spmc_ring_buffer_object_pool<algo_err_msg, 8192> algo_err_msg_pool;
	static rapid_ring::spsc_ring_buffer_object_pool<algo_odr_position, 8192> algo_odr_position_pool;
#ifndef NOT_MEASURE
	static rapid_ring::spmc_ring_buffer_object_pool<algo_latency, 8192> algo_latency_pool;
#endif
	static rapid_ring::spsc_ring_buffer_object_pool<algo_set, 8192> algo_set_pool;
	static rapid_ring::spsc_ring_buffer_object_pool<algo_del, 8192> algo_del_pool;
	static rapid_ring::spsc_ring_buffer_object_pool<algo_cancel, 8192> algo_cancel_pool;
	static rapid_ring::spsc_ring_buffer_object_pool<algo_get, 8192> algo_get_pool;
	static rapid_ring::spsc_ring_buffer_object_pool<algo_force_buy, 8192> algo_force_buy_pool;
	static rapid_ring::spsc_ring_buffer_object_pool<algo_force_sell, 8192> algo_force_sell_pool;
	static rapid_ring::spsc_ring_buffer_object_pool<algo_limit_sell, 8192> algo_limit_sell_pool;
	static rapid_ring::spsc_ring_buffer_object_pool<algo_getprofit_msg, 8192> algo_getprofit_msg_pool;
};



#endif /* ALGOS_INC_SEMI_HPP_ */
