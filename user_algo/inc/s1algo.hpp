#ifndef USER_ALGO_INC_S1ALGO_HPP_
#define USER_ALGO_INC_S1ALGO_HPP_
#include <msg.hpp>
#include <user.hpp>
#include <json.hpp>
#include <string>
#include <vector>
#include <algo.hpp>
#include <DateUtil.h>
#include <unordered_map>
#include <OBSetting.h>
#include <global_memory.hpp>
#include <AlgoEngineData.h>
#include <SelectedWarrant.h>
#include <AlgoBetX.h>

#define MaxBuyNoWarrant 1

#define MARKET_NOTREADY 0
#define MARKET_START 1
#define MARKET_PAUSE 2

#define BUY 1
#define SELL 2


class s1algo : public algo
{
public:
	std::unordered_map<unsigned long long, unsigned int> order_map;
	unordered_map<unsigned int, OBSetting*> obMap;
	unordered_set<std::string> selectedIssuer;
	SelectedWarrant CSelectedWarrant;
	int MarketStatus;
	AlgoBetX algoBet;
	time_t forceSoldTime;
	time_t undetectedTime;
	time_t soldendTime;

private:
	struct algo_order_msg: public algo_msg_base
	{
		unsigned long long orderid;
		unsigned int warrant_code;
		unsigned int ucode;
		std::string action;
		std::string side;
		unsigned long long order_price;
		unsigned long long order_quantity;
		unsigned long long filled_price;
		unsigned long long filled_quantity;
		unsigned long long stoplost;
		std::string status;
		std::string transaction_time;
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
			j["ucode"] = ucode;
			j["action"] = "order";
			j["side"] = side;
			j["order_price"] = order_price;
			j["order_quantity"] = order_quantity;
			if(status == "filled" || status == "Partial filled"){
				j["filled_price"] = filled_price;
				j["filled_quantity"] = filled_quantity;
			}
			j["stoplost"] = stoplost;
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
	struct algo_portfolio_msg: public algo_msg_base
	{
		unsigned long long orderid;
		unsigned int warrant_code;
		unsigned int ucode;
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
			j["ucode"] = ucode;
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
	struct algo_signal_msg: public algo_msg_base
	{
		unsigned int code;
		unsigned long long detect_ask;
		bool selected;
		unordered_set<warrant*> detectedlist;

		algo_signal_msg():
			algo_msg_base()
		{
		}
		virtual nlohmann::json to_json() const
		{
			auto j = algo_msg_base::to_json();
			j["action"] = "signal";
			j["code"] = code;
			j["detected_ask"] = detect_ask;
			if(selected){
				j["detected"] = true;
			}else{
				j["detected"] = false;
			}

			if(detectedlist.size() > 0){
				j["detectedlist"] = nlohmann::json::array();
				for(auto f : detectedlist) {
					json jw;
					jw["code"] = f->Code;
					jw["name"] = f->Name;
					jw["ask"] = f->RefWAsk;
					j["detectedlist"].push_back(jw);
				}
			}
			return j;
		}
		virtual void on_command()
		{
			ouputQueue.enqueue(this);
		}
		virtual void release()
		{
			if(detectedlist.size() > 0){
				detectedlist.clear();
			}
			algo_signal_msg_pool.release_obj(this);
		}
		virtual ~algo_signal_msg() = default;
	};
	struct algo_stoplost_msg: public algo_msg_base
	{
		unsigned int code;
		unsigned long long stoplost;
		unsigned long long wbid;

		algo_stoplost_msg():
			algo_msg_base()
		{
		}
		virtual nlohmann::json to_json() const
		{
			auto j = algo_msg_base::to_json();
			j["action"] = "stoplost";
			j["code"] = code;
			j["stoplost"] = stoplost;
			j["wbid"] = wbid;
			return j;
		}
		virtual void on_command()
		{
			ouputQueue.enqueue(this);
		}
		virtual void release()
		{
			algo_stoplost_msg_pool.release_obj(this);
		}
		virtual ~algo_stoplost_msg() = default;
	};
	struct algo_marketstatus_msg: public algo_msg_base
	{
		int prevmarketstatus;
		int currmarketstatus;
		std::string action;

		algo_marketstatus_msg():
			algo_msg_base(),
			prevmarketstatus(MARKET_NOTREADY),
			currmarketstatus(MARKET_NOTREADY),
			action("")
		{
		}
		virtual nlohmann::json to_json() const
		{
			auto j = algo_msg_base::to_json();
			j["action_type"] = action;
			j["previous_action_status"] = prevmarketstatus;
			j["new_action_status"] = currmarketstatus;
			j["recovery"] = true;
			return j;
		}
		virtual void on_command()
		{
			auto* self = dynamic_cast<s1algo*>(al);

			prevmarketstatus = self->MarketStatus;
			if(action == "start"){
				self->MarketStatus = MARKET_START;
			}
			if(action == "pause"){
				self->MarketStatus = MARKET_PAUSE;
			}
			currmarketstatus = self->MarketStatus;

			ouputQueue.enqueue(this);

		}
		virtual void release()
		{
			algo_marketstatus_msg_pool.release_obj(this);
		}
		virtual ~algo_marketstatus_msg() = default;
	};
	struct algo_setbet_msg: public algo_msg_base
	{
		std::string betsize;

		algo_setbet_msg():
			algo_msg_base()
		{
		}
		virtual nlohmann::json to_json() const
		{
			auto j = algo_msg_base::to_json();
			j["betsize"] = betsize;
			j["recovery"] = true;
			return j;
		}
		virtual void on_command()
		{
			auto* self = dynamic_cast<s1algo*>(al);

			self->setBetsize(betsize);
			ouputQueue.enqueue(this);

		}
		virtual void release()
		{
			algo_setbet_msg_pool.release_obj(this);
		}
		virtual ~algo_setbet_msg() = default;
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
public:
	s1algo() = delete;
	s1algo(user& u, const std::string& name);
	s1algo(const algo&) = delete;
	s1algo(algo&&) = delete;
	s1algo& operator= (const algo&) = delete;
	s1algo& operator= (algo&&) = delete;

	virtual vector<warrant*> getSelectedWarrantFromMarketByIssuer(std::string issuercode, unsigned int underlying, unsigned long long ubid, unsigned long long uask);

	virtual bool doWarrantAction(warrant* w, dbp::top::order_side side, unsigned long long price, unsigned long long quantity);

	virtual void on_omdc_book(const Tradable&);
	virtual void on_omdd_book(const Tradable&);
	virtual void on_omdc_trade(const Tradable&);
	virtual void on_omdd_trade(const Tradable&);
	virtual void handler_order(const dbp::top::enhance_order&);

	virtual void handle_command(algo_msg_base&);
	virtual algo_msg_base* json_to_msg(json& msg);
	virtual std::string get_lib_name();

	virtual void Log(std::string msg);
	virtual void setBetsize(std::string betsize);
	virtual unsigned long long getBestBid(unsigned int code);
public:
	static rapid_ring::spmc_ring_buffer_object_pool<algo_err_msg, 8192> algo_err_msg_pool;
	static rapid_ring::spsc_ring_buffer_object_pool<algo_marketstatus_msg, 8192> algo_marketstatus_msg_pool;
	static rapid_ring::spsc_ring_buffer_object_pool<algo_setbet_msg, 8192> algo_setbet_msg_pool;
	static rapid_ring::spsc_ring_buffer_object_pool<algo_order_msg, 8192> algo_order_msg_pool;
	static rapid_ring::spmc_ring_buffer_object_pool<algo_portfolio_msg, 8192> algo_portfolio_msg_pool;
	static rapid_ring::spmc_ring_buffer_object_pool<algo_signal_msg, 8192> algo_signal_msg_pool;
	static rapid_ring::spmc_ring_buffer_object_pool<algo_stoplost_msg, 8192> algo_stoplost_msg_pool;
};





#endif /* USER_ALGO_INC_S1ALGO_HPP_ */
