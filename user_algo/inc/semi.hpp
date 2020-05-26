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
			_is_omdd(false)
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
			_is_omdd(is_omdd)
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
				_algo->_o_map[odr.order_id] = this;
				if (is_auto)
				{
					_auto_buy_id = odr.order_id;
				}
				return true;
			}
			else
			{
				return false;
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

			auto buyratio = static_cast<unsigned long long>((best_ask_vol- tradable.m_AccumulateBuyQuantity) / (best_bid_vol + best_ask_vol - tradable.m_AccumulateBuyQuantity - tradable.m_AccumulateSellQuantity) * 100);
			auto sellratio = static_cast<unsigned long long>((best_bid_vol-tradable.m_AccumulateSellQuantity) / (best_bid_vol + best_ask_vol - tradable.m_AccumulateBuyQuantity - tradable.m_AccumulateSellQuantity) * 100);

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
								)||(_ratio_buy > 0 && buyratio<= _ratio_buy)
						)
					)
				{

					if (_auto_buy)
					{
						buy(_buy_price);
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
							)||(_ratio_sell > 0 && sellratio<= _ratio_sell)
						)
					)
				{
					if (_position > 0)
					{
						if (_auto_sell)
						{
							sell(_sell_price);
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

					if (_auto_buy)
					{
						buy(_buy_price);
					}
				}
				diff = static_cast<long long>(tradable.m_Ask[0].m_uQuantity)  - static_cast<long long>(_early_sell_qty);

				if (
						TradeSide::SELL_SIDE == side &&
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

						if (_auto_sell)
						{
							sell(_sell_price);
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
					if (_auto_sell)
					{
						sell();
					}
				}
			}
		}
		void on_order(const dbp::top::enhance_order& odr)
		{
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
			algo_odr_msg* msg = new algo_odr_msg();
			msg->al = _algo;
			msg->algo_name = _algo->_name;
			msg->id = _algo->_u.get_id();
			msg->ref = _ref;
			msg->odr = odr;
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
	};
private:
	using pair_map = std::unordered_map<std::string, pair>;
private:
	pair_map _p_map;
private:
	struct algo_odr_msg: public algo_msg_base
	{
		dbp::top::enhance_order odr;
		algo_odr_msg():
			algo_msg_base(),
			odr()
		{
		}
		virtual nlohmann::json to_json() const
		{
			auto j = algo_msg_base::to_json();
			j["msg_type"] = "semi_algo_odr_msg";
			j["odr"] = odr.to_json();
			j["recovery"] = true;
			return j;
		}
		virtual void on_command()
		{
		}
		virtual ~algo_odr_msg() = default;
	};
	struct algo_err_msg: public algo_msg_base
	{
		std::string err;
		algo_err_msg():
			algo_msg_base(),
			err("")
		{
		}
		virtual nlohmann::json to_json() const
		{
			auto j = algo_msg_base::to_json();
			j["msg_type"] = "semi_algo_err_msg";
			j["error"] = err;
			return j;
		}
		virtual void on_command()
		{
			ouputQueue.enqueue(this);
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
		virtual ~algo_odr_position() = default;
	};
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
			j["pair"] = p.to_json();
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
				j["pair"] = p->to_json();
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
		virtual ~algo_del() = default;
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
		virtual ~algo_get() = default;
	};
	struct algo_force_buy: public algo_msg_base
	{
		pair* p;
		std::string result;
		unsigned long long quantity;
		algo_force_buy():
			algo_msg_base(),
			p(nullptr),
			result(""),
			quantity(0)
		{
		}
		virtual nlohmann::json to_json() const
		{
			auto j = algo_msg_base::to_json();
			j["msg_type"] = "semi_algo_force_buy";
			if (p)
				j["pair"] = p->to_json();
			else
				j["pair"] = nullptr;
			j["quantity"] = quantity;
			j["result"] = result;
			return j;
		}
		virtual void on_command()
		{
			auto* self = dynamic_cast<semi*>(al);
			result = self->force_buy(quantity, p, ref);
			ouputQueue.enqueue(this);
		}
		virtual ~algo_force_buy() = default;
	};
	struct algo_force_sell: public algo_msg_base
	{
		pair* p;
		std::string result;
		unsigned long long quantity;
		algo_force_sell():
			algo_msg_base(),
			p(nullptr),
			result(""),
			quantity(0)
		{
		}
		virtual nlohmann::json to_json() const
		{
			auto j = algo_msg_base::to_json();
			j["msg_type"] = "semi_algo_force_sell";
			if (p)
				j["pair"] = p->to_json();
			else
				j["pair"] = nullptr;
			j["quantity"] = quantity;
			j["result"] = result;
			return j;
		}
		virtual void on_command()
		{
			auto* self = dynamic_cast<semi*>(al);
			result = self->force_sell(quantity, p, ref);
			ouputQueue.enqueue(this);
		}
		virtual ~algo_force_sell() = default;
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
	std::string force_buy(unsigned long long quantity, pair*& pref, const std::string& ref);
	std::string force_sell(unsigned long long quantity, pair*& pref, const std::string& ref);
	void position(algo_odr_position& msg) const;
	virtual ~semi() = default;
	virtual void on_omdc_book(const Tradable&);
	virtual void on_omdd_book(const Tradable&);
	virtual void on_omdc_trade(const Tradable&);
	virtual void on_omdd_trade(const Tradable&);
	virtual void handler_order(const dbp::top::enhance_order&);
	virtual void handle_command(algo_msg_base&);
	virtual algo_msg_base* json_to_msg(json& msg);
	virtual json msg_to_json(algo_msg_base* msg);
};



#endif /* ALGOS_INC_SEMI_HPP_ */
