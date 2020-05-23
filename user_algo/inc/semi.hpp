#ifndef ALGOS_INC_SEMI_HPP_
#define ALGOS_INC_SEMI_HPP_

#include <algo.hpp>
#include <global_memory.hpp>

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
	private:
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
	public:
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
			bool is_thai,
			unsigned long long early_buy_qty,
			unsigned long long early_sell_qty,
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
			_underlying_code(underlying_code),
			_warrant_code(warrant_code),
			_ref(ref),
			_underlying_symbol(underlying_symbol),
			_is_bull(is_bull),
			_is_buying(false),
			_is_selling(false),
			_auto_buy(auto_buy),
			_auto_sell(auto_sell),
			_is_omdd(is_omdd)
		{
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
			unsigned long long price = _sell_price;
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
		bool buy(unsigned long long price = 0, bool is_auto = true, unsigned long long quantity = 0)
		{
			auto original_sell_quantity = quantity;
			auto original_sell_price = price;
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
				//TODO
				/*
				json_type json;
				json["type"] = "algo semi";
				json["action"] = "buy";
				json["key"] = _algo->key();
				json["id"] = _algo->id();
				json["is_omdd"] = _is_omdd;
				json["is_thai"] = _is_thai;
				if (_is_thai)
				{
					json["underlying_code"] = code_mapping::get_name_from_code(_underlying_code);
					json["warrant_code"] = code_mapping::get_name_from_code(_warrant_code);
				}
				else
				{
					json["underlying_code"] = _underlying_code;
					json["warrant_code"] = _warrant_code;
				}
				json["underlying_symbol"] = _underlying_symbol;
				json["price"] = price;
				json["quantity"] = quantity;
				json["original_sell_quantity"] = original_sell_quantity;
				json["original_sell_price"] = original_sell_price;
				json["is_auto"] = is_auto;
				json["is_bull"] = _is_bull;
				json["bottom_price"] = _bottom_price;
				json["ceiling_price"] = _ceiling_price;
				json["auto_buy"] = _auto_buy;
				json["auto_sell"] = _auto_sell;
				json["position"] = _position;
				json["auto_buy_quantity"] = _auto_buy_quantity;
				json["result"] = "fail BUYING";
				json["ref"] = _ref;
				json["recovery"] = true;
				send_out(json.dump());
				*/
				return false;
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
				//TODO
				/*
				json_type json;
				json["type"] = "algo semi";
				json["action"] = "buy";
				json["key"] = _algo->key();
				json["id"] = _algo->id();
				json["is_omdd"] = _is_omdd;
				json["is_thai"] = _is_thai;
				if (_is_thai)
				{
					json["underlying_code"] = code_mapping::get_name_from_code(_underlying_code);
					json["warrant_code"] = code_mapping::get_name_from_code(_warrant_code);
				}
				else
				{
					json["underlying_code"] = _underlying_code;
					json["warrant_code"] = _warrant_code;
				}
				json["underlying_symbol"] = _underlying_symbol;
				json["price"] = price;
				json["quantity"] = quantity;
				json["original_sell_quantity"] = original_sell_quantity;
				json["original_sell_price"] = original_sell_price;
				json["is_auto"] = is_auto;
				json["is_bull"] = _is_bull;
				json["bottom_price"] = _bottom_price;
				json["ceiling_price"] = _ceiling_price;
				json["auto_buy"] = _auto_buy;
				json["auto_sell"] = _auto_sell;
				json["position"] = _position;
				json["auto_buy_quantity"] = _auto_buy_quantity;
				json["result"] = "success";
				json["ref"] = _ref;
				json["recovery"] = true;
				send_out(json.dump());
				_algo->_o_map[odr.order_id] = this;
				if (is_auto)
				{
					_auto_buy_id = odr.order_id;
				}
				*/
				return true;
			}
			else
			{
				//TODO
				/*
				json_type json;
				json["type"] = "algo semi";
				json["action"] = "buy";
				json["key"] = _algo->key();
				json["id"] = _algo->id();
				json["is_omdd"] = _is_omdd;
				json["is_thai"] = _is_thai;
				if (_is_thai)
				{
					json["underlying_code"] = code_mapping::get_name_from_code(_underlying_code);
					json["warrant_code"] = code_mapping::get_name_from_code(_warrant_code);
				}
				else
				{
					json["underlying_code"] = _underlying_code;
					json["warrant_code"] = _warrant_code;
				}
				json["underlying_symbol"] = _underlying_symbol;
				json["price"] = price;
				json["quantity"] = quantity;
				json["original_sell_quantity"] = original_sell_quantity;
				json["original_sell_price"] = original_sell_price;
				json["is_auto"] = is_auto;
				json["is_bull"] = _is_bull;
				json["bottom_price"] = _bottom_price;
				json["ceiling_price"] = _ceiling_price;
				json["auto_buy"] = _auto_buy;
				json["auto_sell"] = _auto_sell;
				json["position"] = _position;
				json["auto_buy_quantity"] = _auto_buy_quantity;
				json["result"] = "fail new buy order not valid";
				json["ref"] = _ref;
				json["recovery"] = true;
				send_out(json.dump());
				*/
				return false;
			}

		}
		sell_result sell(unsigned long long price = 0, bool is_auto = true, unsigned long long quantity = 0)
		{
			auto original_sell_quantity = quantity;
			auto original_sell_price = price;
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
				//TODO
				/*
				json_type json;
				json["type"] = "algo semi";
				json["action"] = "sell";
				json["key"] = _algo->key();
				json["id"] = _algo->id();
				json["is_omdd"] = _is_omdd;
				json["is_thai"] = _is_thai;
				if (_is_thai)
				{
					json["underlying_code"] = code_mapping::get_name_from_code(_underlying_code);
					json["warrant_code"] = code_mapping::get_name_from_code(_warrant_code);
				}
				else
				{
					json["underlying_code"] = _underlying_code;
					json["warrant_code"] = _warrant_code;
				}
				json["underlying_symbol"] = _underlying_symbol;
				json["price"] = price;
				json["original_sell_quantity"] = original_sell_quantity;
				json["original_sell_price"] = original_sell_price;
				json["quantity"] = quantity;
				json["is_auto"] = is_auto;
				json["is_bull"] = _is_bull;
				json["bottom_price"] = _bottom_price;
				json["ceiling_price"] = _ceiling_price;
				json["auto_buy"] = _auto_buy;
				json["auto_sell"] = _auto_sell;
				json["position"] = _position;
				json["auto_buy_quantity"] = _auto_buy_quantity;
				json["result"] = "fail NOTHING_TO_SELL";
				json["ref"] = _ref;
				json["recovery"] = true;
				send_out(json.dump());
				*/
				return sell_result::NOTHING_TO_SELL;
			}
			else if (_is_selling)
			{
				//TODO
				/*
				json_type json;
				json["type"] = "algo semi";
				json["action"] = "sell";
				json["key"] = _algo->key();
				json["id"] = _algo->id();
				json["is_omdd"] = _is_omdd;
				json["is_thai"] = _is_thai;
				if (_is_thai)
				{
					json["underlying_code"] = code_mapping::get_name_from_code(_underlying_code);
					json["warrant_code"] = code_mapping::get_name_from_code(_warrant_code);
				}
				else
				{
					json["underlying_code"] = _underlying_code;
					json["warrant_code"] = _warrant_code;
				}
				json["underlying_symbol"] = _underlying_symbol;
				json["price"] = price;
				json["quantity"] = quantity;
				json["original_sell_quantity"] = original_sell_quantity;
				json["original_sell_price"] = original_sell_price;
				json["is_auto"] = is_auto;
				json["is_bull"] = _is_bull;
				json["bottom_price"] = _bottom_price;
				json["ceiling_price"] = _ceiling_price;
				json["auto_buy"] = _auto_buy;
				json["auto_sell"] = _auto_sell;
				json["position"] = _position;
				json["auto_buy_quantity"] = _auto_buy_quantity;
				json["result"] = "fail SELLING";
				json["ref"] = _ref;
				json["recovery"] = true;
				send_out(json.dump());
				*/
				return sell_result::SELLING;
			}
			else if (quantity > _position)
			{
				//TODO
				/*
				json_type json;
				json["type"] = "algo semi";
				json["action"] = "sell";
				json["key"] = _algo->key();
				json["id"] = _algo->id();
				json["is_omdd"] = _is_omdd;
				json["underlying_code"] = _underlying_code;
				json["warrant_code"] = _warrant_code;
				json["underlying_symbol"] = _underlying_symbol;
				json["price"] = price;
				json["quantity"] = quantity;
				json["original_sell_quantity"] = original_sell_quantity;
				json["original_sell_price"] = original_sell_price;
				json["is_auto"] = is_auto;
				json["is_bull"] = _is_bull;
				json["bottom_price"] = _bottom_price;
				json["ceiling_price"] = _ceiling_price;
				json["auto_buy"] = _auto_buy;
				json["auto_sell"] = _auto_sell;
				json["position"] = _position;
				json["auto_buy_quantity"] = _auto_buy_quantity;
				json["result"] = "fail SHORT_SELL";
				json["ref"] = _ref;
				json["recovery"] = true;
				send_out(json.dump());
				*/
				return sell_result::SHORT_SELL;
			}
			_is_selling = true;
			_auto_sell = false;
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
				//TODO
				/*
				json_type json;
				json["type"] = "algo semi";
				json["action"] = "sell";
				json["key"] = _algo->key();
				json["id"] = _algo->id();
				json["is_omdd"] = _is_omdd;
				json["underlying_code"] = _underlying_code;
				json["warrant_code"] = _warrant_code;
				json["underlying_symbol"] = _underlying_symbol;
				json["price"] = price;
				json["quantity"] = quantity;
				json["original_sell_quantity"] = original_sell_quantity;
				json["original_sell_price"] = original_sell_price;
				json["is_auto"] = is_auto;
				json["is_bull"] = _is_bull;
				json["bottom_price"] = _bottom_price;
				json["ceiling_price"] = _ceiling_price;
				json["auto_buy"] = _auto_buy;
				json["auto_sell"] = _auto_sell;
				json["position"] = _position;
				json["auto_buy_quantity"] = _auto_buy_quantity;
				json["result"] = "SUCCESS";
				json["ref"] = _ref;
				json["recovery"] = true;
				send_out(json.dump());
				*/
				_algo->_o_map[odr.order_id] = this;
				if (is_auto)
				{
					_auto_sell_id = odr.order_id;
				}
				return sell_result::SUCCESS;
			}
			else
			{
				//TODO
				/*
				json_type json;
				json["type"] = "algo semi";
				json["action"] = "sell";
				json["key"] = _algo->key();
				json["id"] = _algo->id();
				json["is_omdd"] = _is_omdd;
				json["is_thai"] = _is_thai;
				json["underlying_code"] = _underlying_code;
				json["warrant_code"] = _warrant_code;
				json["underlying_symbol"] = _underlying_symbol;
				json["price"] = price;
				json["quantity"] = quantity;
				json["original_sell_quantity"] = original_sell_quantity;
				json["original_sell_price"] = original_sell_price;
				json["is_auto"] = is_auto;
				json["is_bull"] = _is_bull;
				json["bottom_price"] = _bottom_price;
				json["ceiling_price"] = _ceiling_price;
				json["auto_buy"] = _auto_buy;
				json["auto_sell"] = _auto_sell;
				json["position"] = _position;
				json["auto_buy_quantity"] = _auto_buy_quantity;
				json["result"] = "fail new sell order not valid";
				json["ref"] = _ref;
				json["recovery"] = true;
				send_out(json.dump());
				*/
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
			if (!(_underlying_symbol[0] < '0' || _underlying_symbol[0] > '9')){
				if (0 != type && 100 != type)
					return;
			}
			if (_is_bull)
			{
				auto trade_quantity = static_cast<long long>(tradable.m_AccumulateBuyQuantity);
				diff = static_cast<long long>(tradable.m_Ask[0].m_uQuantity) - static_cast<long long>(_early_buy_qty);

				if (
						TradeSide::BUY_SIDE == side &&
						(
								(
										trade_price == _buy_trriger && trade_price == ask_price &&
										trade_quantity >= diff &&
										0 != tradable.m_Ask[0].m_uQuantity && _is_buying == false && _auto_buy == true && _is_selling == false
								)
						)
					)
				{

					if (!_auto_buy)
					{
						//TODO
						/*
						json_type json;
						json["type"] = "algo semi";
						json["action"] = "remind_buy";
						json["key"] = _algo->key();
						json["id"] = _algo->id();
						json["is_omdd"] = _is_omdd;
						json["is_thai"] = _is_thai;
						if (_is_thai)
						{
							json["underlying_code"] = code_mapping::get_name_from_code(_underlying_code);
							json["warrant_code"] = code_mapping::get_name_from_code(_warrant_code);
						}
						else
						{
							json["underlying_code"] = _underlying_code;
							json["warrant_code"] = _warrant_code;
						}
						json["underlying_symbol"] = _underlying_symbol;
						json["price"] = _buy_price;
						json["quantity"] = _auto_buy_quantity;
						json["ref"] = _ref;
						json["is_bull"] = _is_bull;
						json["bottom_price"] = _bottom_price;
						json["ceiling_price"] = _ceiling_price;
						json["auto_buy"] = _auto_buy;
						json["auto_sell"] = _auto_sell;
						json["position"] = _position;
						json["auto_buy_quantity"] = _auto_buy_quantity;
						send_out(json.dump());
						*/
					}
					else if (buy(_buy_price))
					{
						//TODO
						/*
						json_type json;
						json["type"] = "algo semi";
						json["action"] = "auto_trriger_buy";
						json["key"] = _algo->key();
						json["id"] = _algo->id();
						json["is_omdd"] = _is_omdd;
						json["is_thai"] = _is_thai;
						if (_is_thai)
						{
							json["underlying_code"] = code_mapping::get_name_from_code(_underlying_code);
							json["warrant_code"] = code_mapping::get_name_from_code(_warrant_code);
						}
						else
						{
							json["underlying_code"] = _underlying_code;
							json["warrant_code"] = _warrant_code;
						}
						json["underlying_symbol"] = _underlying_symbol;
						json["price"] = _buy_price;
						json["quantity"] = _auto_buy_quantity;
						json["is_auto"] = true;
						json["is_bull"] = _is_bull;
						json["bottom_price"] = _bottom_price;
						json["ceiling_price"] = _ceiling_price;
						json["auto_buy"] = _auto_buy;
						json["auto_sell"] = _auto_sell;
						json["position"] = _position;
						json["auto_buy_quantity"] = _auto_buy_quantity;
						json["result"] = "success";
						json["ref"] = _ref;
						send_out(json.dump());
						*/
					}
					else
					{
						//TODO:
						/*
						json_type json;
						json["type"] = "algo semi";
						json["action"] = "auto_trriger_buy";
						json["key"] = _algo->key();
						json["id"] = _algo->id();
						json["is_omdd"] = _is_omdd;
						json["is_thai"] = _is_thai;
						if (_is_thai)
						{
							json["underlying_code"] = code_mapping::get_name_from_code(_underlying_code);
							json["warrant_code"] = code_mapping::get_name_from_code(_warrant_code);
						}
						else
						{
							json["underlying_code"] = _underlying_code;
							json["warrant_code"] = _warrant_code;
						}
						json["underlying_symbol"] = _underlying_symbol;
						json["price"] = _buy_price;
						json["quantity"] = _auto_buy_quantity;
						json["is_auto"] = true;
						json["is_bull"] = _is_bull;
						json["bottom_price"] = _bottom_price;
						json["ceiling_price"] = _ceiling_price;
						json["auto_buy"] = _auto_buy;
						json["auto_sell"] = _auto_sell;
						json["position"] = _position;
						json["auto_buy_quantity"] = _auto_buy_quantity;
						json["result"] = "fail new buy order not valid";
						json["ref"] = _ref;
						send_out(json.dump());
						*/
					}
				}

				diff = static_cast<long long>(tradable.m_Bid[0].m_uQuantity)  - static_cast<long long>(_early_sell_qty);
				if (
						TradeSide::SELL_SIDE == side &&
						(
							(
									trade_price == _sell_trriger && trade_price == bid_price &&
									trade_quantity >= diff &&
									0 != tradable.m_Bid[0].m_uQuantity && _is_selling == false && _auto_sell == true && _is_buying == false
							)
						)
					)
				{
					if (_position > 0)
					{

						if (!_auto_sell)
						{
							//TODO
							/*
							json_type json;
							json["type"] = "algo semi";
							json["action"] = "remind_sell";
							json["key"] = _algo->key();
							json["id"] = _algo->id();
							json["is_omdd"] = _is_omdd;
							json["is_thai"] = _is_thai;
							if (_is_thai)
							{
								json["underlying_code"] = code_mapping::get_name_from_code(_underlying_code);
								json["warrant_code"] = code_mapping::get_name_from_code(_warrant_code);
							}
							else
							{
								json["underlying_code"] = _underlying_code;
								json["warrant_code"] = _warrant_code;
							}
							json["underlying_symbol"] = _underlying_symbol;
							json["price"] = _sell_price;
							json["quantity"] = _position;
							json["bottom_price"] = _bottom_price;
							json["ceiling_price"] = _ceiling_price;
							json["auto_buy"] = _auto_buy;
							json["auto_sell"] = _auto_sell;
							json["position"] = _position;
							json["auto_buy_quantity"] = _auto_buy_quantity;
							json["ref"] = _ref;
							json["is_bull"] = _is_bull;
							send_out(json.dump());
							*/
						}
						else
						{
							auto result = sell(_sell_price);
							//TODO
							/*
							json_type json;
							json["type"] = "algo semi";
							json["action"] = "auto_trriger_sell";
							json["key"] = _algo->key();
							json["id"] = _algo->id();
							json["is_omdd"] = _is_omdd;
							json["is_thai"] = _is_thai;
							json["underlying_code"] = _underlying_code;
							json["warrant_code"] = _warrant_code;
							json["underlying_symbol"] = _underlying_symbol;
							json["price"] = _sell_price;
							json["quantity"] = _position;
							json["is_auto"] = true;
							json["is_bull"] = _is_bull;
							json["bottom_price"] = _bottom_price;
							json["ceiling_price"] = _ceiling_price;
							json["auto_buy"] = _auto_buy;
							json["auto_sell"] = _auto_sell;
							json["position"] = _position;
							json["auto_buy_quantity"] = _auto_buy_quantity;
							if (result == sell_result::SUCCESS)
							{
								json["result"] = "success";
							}
							else if (result == sell_result::SELLING)
								json["result"] = "fail SELLING";
							else if (result == sell_result::SHORT_SELL)
								json["result"] = "fail SHORT_SELL";
							else if (result == sell_result::NEW_SELL_ODR_FAIL)
								json["result"] = "fail NEW_SELL_ODR_FAIL";
							else
								json["result"] = "fail unknow";
							json["ref"] = _ref;
							send_out(json.dump());
							*/
						}
					}
				}
			}
			else
			{

				auto trade_quantity = static_cast<long long>(tradable.m_AccumulateBuyQuantity);
				diff = static_cast<long long>(tradable.m_Bid[0].m_uQuantity) - static_cast<long long>(_early_buy_qty);

				if (
						TradeSide::SELL_SIDE == side &&
						(
								(
										trade_price == _buy_trriger && trade_price == bid_price &&
										trade_quantity >= (diff) &&
										0 != tradable.m_Bid[0].m_uQuantity && _is_buying == false && _auto_buy == true  && _is_selling == false
								)
						)
					)
				{

					if (!_auto_buy)
					{
						//TODO
						/*
						json_type json;
						json["type"] = "algo semi";
						json["action"] = "remind_buy";
						json["key"] = _algo->key();
						json["id"] = _algo->id();
						json["is_omdd"] = _is_omdd;
						json["is_thai"] = _is_thai;
						if (_is_thai)
						{
							json["underlying_code"] = code_mapping::get_name_from_code(_underlying_code);
							json["warrant_code"] = code_mapping::get_name_from_code(_warrant_code);
						}
						else
						{
							json["underlying_code"] = _underlying_code;
							json["warrant_code"] = _warrant_code;
						}
						json["underlying_symbol"] = _underlying_symbol;
						json["price"] = _buy_price;
						json["quantity"] = _auto_buy_quantity;
						json["bottom_price"] = _bottom_price;
						json["ceiling_price"] = _ceiling_price;
						json["auto_buy"] = _auto_buy;
						json["auto_sell"] = _auto_sell;
						json["position"] = _position;
						json["auto_buy_quantity"] = _auto_buy_quantity;
						json["ref"] = _ref;
						json["is_bull"] = _is_bull;
						send_out(json.dump());
						*/
					}
					else if (buy(_buy_price))
					{
						//TODO
						/*
						json_type json;
						json["type"] = "algo semi";
						json["action"] = "auto_trriger_buy";
						json["key"] = _algo->key();
						json["id"] = _algo->id();
						json["is_omdd"] = _is_omdd;
						json["is_thai"] = _is_thai;
						if (_is_thai)
						{
							json["underlying_code"] = code_mapping::get_name_from_code(_underlying_code);
							json["warrant_code"] = code_mapping::get_name_from_code(_warrant_code);
						}
						else
						{
							json["underlying_code"] = _underlying_code;
							json["warrant_code"] = _warrant_code;
						}
						json["underlying_symbol"] = _underlying_symbol;
						json["price"] = _buy_price;
						json["quantity"] = _auto_buy_quantity;
						json["bottom_price"] = _bottom_price;
						json["ceiling_price"] = _ceiling_price;
						json["auto_buy"] = _auto_buy;
						json["auto_sell"] = _auto_sell;
						json["is_auto"] = true;
						json["is_bull"] = _is_bull;
						json["position"] = _position;
						json["auto_buy_quantity"] = _auto_buy_quantity;
						json["result"] = "success";
						json["ref"] = _ref;
						send_out(json.dump());
						*/
					}
					else
					{
						//TODO
						/*
						json_type json;
						json["type"] = "algo semi";
						json["action"] = "auto_trriger_buy";
						json["key"] = _algo->key();
						json["id"] = _algo->id();
						json["is_omdd"] = _is_omdd;
						json["is_thai"] = _is_thai;
						if (_is_thai)
						{
							json["underlying_code"] = code_mapping::get_name_from_code(_underlying_code);
							json["warrant_code"] = code_mapping::get_name_from_code(_warrant_code);
						}
						else
						{
							json["underlying_code"] = _underlying_code;
							json["warrant_code"] = _warrant_code;
						}
						json["underlying_symbol"] = _underlying_symbol;
						json["price"] = _buy_price;
						json["quantity"] = _auto_buy_quantity;
						json["bottom_price"] = _bottom_price;
						json["ceiling_price"] = _ceiling_price;
						json["auto_buy"] = _auto_buy;
						json["auto_sell"] = _auto_sell;
						json["is_auto"] = true;
						json["is_bull"] = _is_bull;
						json["position"] = _position;
						json["auto_buy_quantity"] = _auto_buy_quantity;
						json["result"] = "fail new buy order not valid";
						json["ref"] = _ref;
						send_out(json.dump());
						*/
					}
				}
				diff = static_cast<long long>(tradable.m_Ask[0].m_uQuantity)  - static_cast<long long>(_early_sell_qty);

				if (
						-1 == side &&
						(
								(
										trade_price == _sell_trriger &&
										trade_quantity >= (diff) && trade_price == ask_price &&
										0 != tradable.m_Ask[0].m_uQuantity && _is_selling == false && _auto_sell == true  && _is_buying == false
								)
						)
					)
				{
					if (_position > 0)
					{

						if (!_auto_sell)
						{
							//TODO
							/*
							json_type json;
							json["type"] = "algo semi";
							json["action"] = "remind_sell";
							json["key"] = _algo->key();
							json["id"] = _algo->id();
							json["is_omdd"] = _is_omdd;
							json["is_thai"] = _is_thai;
							if (_is_thai)
							{
								json["underlying_code"] = code_mapping::get_name_from_code(_underlying_code);
								json["warrant_code"] = code_mapping::get_name_from_code(_warrant_code);
							}
							else
							{
								json["underlying_code"] = _underlying_code;
								json["warrant_code"] = _warrant_code;
							}
							json["underlying_symbol"] = _underlying_symbol;
							json["price"] = _sell_price;
							json["quantity"] = _position;
							json["bottom_price"] = _bottom_price;
							json["ceiling_price"] = _ceiling_price;
							json["auto_buy"] = _auto_buy;
							json["auto_sell"] = _auto_sell;
							json["position"] = _position;
							json["auto_buy_quantity"] = _auto_buy_quantity;
							json["ref"] = _ref;
							json["is_bull"] = _is_bull;
							send_out(json.dump());
							*/
						}
						else
						{
							auto result = sell(_sell_price);
							//TODO
							/*
							json_type json;
							json["type"] = "algo semi";
							json["action"] = "auto_trriger_sell";
							json["key"] = _algo->key();
							json["id"] = _algo->id();
							json["is_omdd"] = _is_omdd;
							json["is_thai"] = _is_thai;
							if (_is_thai)
							{
								json["underlying_code"] = code_mapping::get_name_from_code(_underlying_code);
								json["warrant_code"] = code_mapping::get_name_from_code(_warrant_code);
							}
							else
							{
								json["underlying_code"] = _underlying_code;
								json["warrant_code"] = _warrant_code;
							}
							json["underlying_symbol"] = _underlying_symbol;
							json["price"] = _sell_price;
							json["quantity"] = _position;
							json["is_auto"] = true;
							json["is_bull"] = _is_bull;
							json["bottom_price"] = _bottom_price;
							json["ceiling_price"] = _ceiling_price;
							json["auto_buy"] = _auto_buy;
							json["auto_sell"] = _auto_sell;
							json["position"] = _position;
							json["auto_buy_quantity"] = _auto_buy_quantity;
							if (result == sell_result::SUCCESS)
							{
								json["result"] = "success";
							}
							else if (result == sell_result::SELLING)
								json["result"] = "fail SELLING";
							else if (result == sell_result::SHORT_SELL)
								json["result"] = "fail SHORT_SELL";
							else if (result == sell_result::NEW_SELL_ODR_FAIL)
								json["result"] = "fail NEW_SELL_ODR_FAIL";
							else
								json["result"] = "fail unknow";
							json["ref"] = _ref;
							send_out(json.dump());
							*/
						}
					}
				}
			}
		}
		void on_book(const Tradable& tradable)
		{
			auto best_bid_price = static_cast<unsigned long long>(tradable.m_Bid[0].m_iPrice) * 100000;
			if (0 != best_bid_price && (best_bid_price == _bottom_price || best_bid_price >= _ceiling_price))
			{
				if (_position > 0)
				{
					if (!_auto_sell)
					{

						//TODO
						/*
						json_type json;
						json["type"] = "algo semi";
						json["action"] = "remind_sell";
						json["key"] = _algo->key();
						json["id"] = _algo->id();
						json["is_omdd"] = _is_omdd;
						json["is_thai"] = _is_thai;
						if (_is_thai)
						{
							json["underlying_code"] = code_mapping::get_name_from_code(_underlying_code);
							json["warrant_code"] = code_mapping::get_name_from_code(_warrant_code);
						}
						else
						{
							json["underlying_code"] = _underlying_code;
							json["warrant_code"] = _warrant_code;
						}
						json["underlying_symbol"] = _underlying_symbol;
						json["price"] = best_bid_price;
						json["quantity"] = _position;
						json["bottom_price"] = _bottom_price;
						json["ceiling_price"] = _ceiling_price;
						json["auto_buy"] = _auto_buy;
						json["auto_sell"] = _auto_sell;
						json["ref"] = _ref;
						json["is_bull"] = _is_bull;
						json["position"] = _position;
						json["auto_buy_quantity"] = _auto_buy_quantity;
						send_out(json.dump());
						*/
					}
					else
					{
						auto result = sell();
						//TODO
						/*
						json_type json;
						json["type"] = "algo semi";
						if (best_bid_price <= _bottom_price)
							json["action"] = "auto_cut_lost_sell";
						else if (best_bid_price >= _ceiling_price)
							json["action"] = "auto_ceiling_sell";
						json["key"] = _algo->key();
						json["id"] = _algo->id();
						json["is_omdd"] = _is_omdd;
						json["is_thai"] = _is_thai;
						if (_is_thai)
						{
							json["underlying_code"] = code_mapping::get_name_from_code(_underlying_code);
							json["warrant_code"] = code_mapping::get_name_from_code(_warrant_code);
						}
						else
						{
							json["underlying_code"] = _underlying_code;
							json["warrant_code"] = _warrant_code;
						}
						json["underlying_symbol"] = _underlying_symbol;
						json["price"] = best_bid_price;
						json["quantity"] = _position;
						json["is_auto"] = true;
						json["is_bull"] = _is_bull;
						json["bottom_price"] = _bottom_price;
						json["ceiling_price"] = _ceiling_price;
						json["auto_buy"] = _auto_buy;
						json["auto_sell"] = _auto_sell;
						json["position"] = _position;
						json["auto_buy_quantity"] = _auto_buy_quantity;
						if (result == sell_result::SUCCESS)
						{
							json["result"] = "success";
						}
						else if (result == sell_result::SELLING)
							json["result"] = "fail SELLING";
						else if (result == sell_result::SHORT_SELL)
							json["result"] = "fail SHORT_SELL";
						else if (result == sell_result::NEW_SELL_ODR_FAIL)
							json["result"] = "fail NEW_SELL_ODR_FAIL";
						else
							json["result"] = "fail unknow";
						json["ref"] = _ref;
						send_out(json.dump());
						*/
					}
				}
			}
		}
		void on_order(const dbp::top::enhance_order& odr)
		{
			//_algo->log_info(std::string("on_order:") + odr.to_string());
			auto status = odr.status;
			auto side = odr.side;
			if (dbp::top::order_status::rejected == status || dbp::top::order_status::canceled == status || dbp::top::order_status::deleted == status || dbp::top::order_status::filled == status)
			{
				if (dbp::top::order_side::buy == side)
				{
					_position += odr.filled_quantity;
					_is_buying = false;
					if (odr.order_id == _auto_buy_id && dbp::top::order_status::filled != status)
					{
						_auto_buy_quantity -= _position;
					}
				}
				else if (dbp::top::order_side::sell == side)
				{
					_position -= odr.filled_quantity;
					_is_selling = false;
				}
			}
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
		unsigned long long auto_buy_id() const
		{
			return _auto_buy_id;
		}
		unsigned long long auto_sell_id() const
		{
			return _auto_sell_id;
		}
		bool is_buying() const
		{
			return _is_buying;
		}
		bool is_selling() const
		{
			return _is_selling;
		}
		bool is_omdd() const
		{
			return _is_omdd;
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
	};
public:
	semi() = delete;
	semi(user& u, const std::string& name);
	semi(const algo&) = delete;
	semi(algo&&) = delete;
	semi& operator= (const algo&) = delete;
	semi& operator= (algo&&) = delete;
	virtual ~semi() = default;
	virtual void on_omdc_book(const Tradable&);
	virtual void on_omdd_book(const Tradable&);
	virtual void on_omdc_trade(const Tradable&);
	virtual void on_omdd_trade(const Tradable&);
	virtual void handler_order(const dbp::top::enhance_order&);
	virtual void handle_command(const algo_msg_base&);
	virtual algo_msg_base* json_to_msg(json& msg);
	virtual json msg_to_json(algo_msg_base* msg);
};



#endif /* ALGOS_INC_SEMI_HPP_ */
