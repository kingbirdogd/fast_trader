#ifndef USER_ALGO_INC_S1ALGO_HPP_
#define USER_ALGO_INC_S1ALGO_HPP_
#include <msg.hpp>
#include <user.hpp>
#include <json.hpp>
#include <string>
#include <vector>
#include <tools.h>
#include <algo.hpp>
#include <DateUtil.h>
#include <unordered_map>
#include <OBSetting.h>
#include <global_memory.hpp>
#include <AlgoEngineData.h>
#include <SelectedWarrant.h>
#include <AlgoBetX.h>
#include "ThreadLogger.h"

//#define MaxBuyNoWarrant 2

#define MARKET_NOTREADY 0
#define MARKET_START 1
#define MARKET_PAUSE 2

#define BUY 1
#define SELL 2

inline static unsigned long long calWeightedPrice(unsigned long long bid1, unsigned long long bid2, unsigned long long bid3,
		unsigned long long bidqty1, unsigned long long bidqty2, unsigned long long bidqty3,
		unsigned long long ask1, unsigned long long ask2, unsigned long long ask3,
		unsigned long long askqty1, unsigned long long askqty2, unsigned long long askqty3){
	unsigned long long result = (bid1*askqty1 + bid2*askqty2 + bid3*askqty3 + ask1*bidqty1 + ask2*bidqty2 + ask3*bidqty3 + 1)/
			(bidqty1+bidqty2+bidqty3+askqty1+askqty2+askqty3+1);
	return result;
}


class s1algo : public algo
{
public:
	std::unordered_map<unsigned long long, unsigned int> order_map;
	unordered_map<unsigned int, OBSetting*> obMap;
	unordered_set<std::string> selectedIssuer;
	unordered_set<unsigned int> unselectedUCode;
	unordered_set<unsigned int> availableUCode;
	unordered_set<unsigned int> unSelectedWarrant;
	SelectedWarrant CSelectedWarrant;
	int MarketStatus;
	AlgoBetX algoBet;
	time_t forceSoldTime;
	time_t undetectedTime;
	time_t soldendTime;
	std::unordered_map<unsigned int, priceinfo*> warrantPriceMap;
	ThreadLogger* logger;
	int MaxBuyNoWarrant;

	time_t lastReadyTime;
	int signalCount = 0;

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
		unsigned long long wbid;
		std::string status;
		std::string transaction_time;
		std::string reason;


#ifndef NOT_MEASURE
		unsigned long long pkg_tm;
		unsigned long long m_tm;
		unsigned long long t_tm;
		unsigned long long o_tm;
#endif

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
			j["wbid"] = wbid;
			j["status"] = status;
			j["transaction_time"] = string(transaction_time);
			j["reason"] = string(reason);

#ifndef NOT_MEASURE
			j["hkex_to_order_latency"] = pkg_tm - m_tm;
			j["tick_to_order_latency"] = t_tm - m_tm;
			j["order_to_resp_latency"] = o_tm - t_tm;
#endif

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
		unordered_set<std::string> detectedlist;

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
					jw["warrantdesc"] = f;
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
			detectedlist.clear();
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
	struct algo_winsell_msg: public algo_msg_base
	{
		unsigned int ucode;
		unsigned int wcode;
		string action;
		bool result;

		algo_winsell_msg():
			algo_msg_base()
		{
		}
		virtual nlohmann::json to_json() const
		{
			auto j = algo_msg_base::to_json();
			j["action"] = "winsell";
			j["code"] = wcode;
			j["setaction"] = action;
			if(result){
				j["result"] = "SUCCESS";
			}else{
				j["result"] = "FAIL";
			}
			return j;
		}
		virtual void on_command()
		{
			auto* self = dynamic_cast<s1algo*>(al);
			result = self->setWinSell(action,ucode, wcode);

			ouputQueue.enqueue(this);
		}
		virtual void release()
		{
			algo_winsell_msg_pool.release_obj(this);
		}
		virtual ~algo_winsell_msg() = default;
	};
	struct algo_winlvlsell_msg: public algo_msg_base
	{
		unsigned int ucode;
		unsigned int wcode;
		string action;
		bool result;

		algo_winlvlsell_msg():
			algo_msg_base()
		{
		}
		virtual nlohmann::json to_json() const
		{
			auto j = algo_msg_base::to_json();
			j["action"] = "winlvlsell";
			j["code"] = wcode;
			j["setaction"] = action;
			if(result){
				j["result"] = "SUCCESS";
			}else{
				j["result"] = "FAIL";
			}
			return j;
		}
		virtual void on_command()
		{
			auto* self = dynamic_cast<s1algo*>(al);
			result = self->setWinLvlSell(action,ucode, wcode);

			ouputQueue.enqueue(this);
		}
		virtual void release()
		{
			algo_winlvlsell_msg_pool.release_obj(this);
		}
		virtual ~algo_winlvlsell_msg() = default;
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
			j["action"] = "marketstatus";
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
		std::string selectedbet;

		algo_setbet_msg():
			algo_msg_base()
		{
		}
		virtual nlohmann::json to_json() const
		{
			auto j = algo_msg_base::to_json();
			j["action"] = "betsize";
			j["betsize"] = betsize;
			j["selectedbet"] = selectedbet;
			if(selectedbet != betsize){
				j["result"] = "FAIL";
			}else{
				j["result"] = "SUCCESS";
				j["recovery"] = true;
			}

			return j;
		}
		virtual void on_command()
		{
			auto* self = dynamic_cast<s1algo*>(al);

			selectedbet = self->setBetsize(betsize);
			ouputQueue.enqueue(this);

		}
		virtual void release()
		{
			algo_setbet_msg_pool.release_obj(this);
		}
		virtual ~algo_setbet_msg() = default;
	};
	struct algo_issueraction_msg: public algo_msg_base
	{
		std::string issuer;
		std::string action;
		bool result;

		algo_issueraction_msg():
			algo_msg_base()
		{
		}
		virtual nlohmann::json to_json() const
		{
			auto j = algo_msg_base::to_json();
			j["action"] = "selectissuer";
			j["selectaction"] = action;
			j["issuer"] = issuer;
			if(result){
				j["result"] = "SUCCESS";
				j["recovery"] = true;
			}else{
				j["result"] = "FAIL";
				j["reason"] = "Invalid Status";
			}
			return j;
		}
		virtual void on_command()
		{
			auto* self = dynamic_cast<s1algo*>(al);

			result = self->setSelectedIssuer(action, issuer);

			ouputQueue.enqueue(this);

		}
		virtual void release()
		{
			algo_issueraction_msg_pool.release_obj(this);
		}
		virtual ~algo_issueraction_msg() = default;
	};
	struct algo_underlyingaction_msg: public algo_msg_base
	{
		unsigned int ucode;
		std::string action;
		bool result;

		algo_underlyingaction_msg():
			algo_msg_base()
		{
		}
		virtual nlohmann::json to_json() const
		{
			auto j = algo_msg_base::to_json();
			j["action"] = "selectunderlying";
			j["selectaction"] = action;
			j["ucode"] = ucode;
			if(result){
				j["result"] = "SUCCESS";
				j["recovery"] = true;
			}else{
				j["result"] = "FAIL";
				j["reason"] = "Invalid Status";
			}
			return j;
		}
		virtual void on_command()
		{
			auto* self = dynamic_cast<s1algo*>(al);

			result = self->setSelectedUnderlying(action, ucode);

			ouputQueue.enqueue(this);

		}
		virtual void release()
		{
			algo_underlyingaction_msg_pool.release_obj(this);
		}
		virtual ~algo_underlyingaction_msg() = default;
	};

	struct algo_warrantaction_msg: public algo_msg_base
	{
		unsigned int code;
		std::string action;
		bool result;

		algo_warrantaction_msg():
			algo_msg_base()
		{
		}
		virtual nlohmann::json to_json() const
		{
			auto j = algo_msg_base::to_json();
			j["action"] = "selectwarrant";
			j["selectaction"] = action;
			j["code"] = code;
			if(result){
				j["result"] = "SUCCESS";
			}else{
				j["result"] = "FAIL";
				j["reason"] = "Invalid Status";
			}
			return j;
		}
		virtual void on_command()
		{
			auto* self = dynamic_cast<csalgo*>(al);

			result = self->setSelectedWarrant(action, code);

			ouputQueue.enqueue(this);

		}
		virtual void release()
		{
			algo_warrantaction_msg_pool.release_obj(this);
		}
		virtual ~algo_warrantaction_msg() = default;
	};
	struct algo_force_sell: public algo_msg_base
	{
		unsigned int code;
		unsigned int ucode;
		bool result;
		unsigned long long price;
		algo_force_sell():
			algo_msg_base()
		{
		}
		virtual nlohmann::json to_json() const
		{
			auto j = algo_msg_base::to_json();
			j["action"] = "force_sell";
			j["code"] = code;
			j["ucode"] = ucode;
			j["price"] = price;
			if(result){
				j["result"] = "SUCCESS";
			}else{
				j["result"] = "FAIL";
				j["reason"] = result;
			}
			return j;
		}
		virtual void on_command()
		{
			auto* self = dynamic_cast<s1algo*>(al);
			result = self->force_sell(ucode, code, price);
			ouputQueue.enqueue(this);
		}
		virtual void release()
		{
			algo_force_sell_pool.release_obj(this);
		}
		virtual ~algo_force_sell() = default;
	};
	struct algo_warrantprice_msg: public algo_msg_base
	{
		unsigned int warrant_code;
		std::string side;
		//unsigned long long wkey;
		unsigned long long wprice;
		algo_warrantprice_msg():
			algo_msg_base(),
			warrant_code(0),
			side(""),
			wprice(0)
		{
		}
		virtual nlohmann::json to_json() const
		{
			auto j = algo_msg_base::to_json();
			j["action"] = "wprice";
			j["warrant_code"] = warrant_code;
			j["side"] = side;
			//j["wkey"] = wkey;
			j["price"] = wprice;
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
	struct algo_issuerlist_msg: public algo_msg_base
	{
		std::string issuers;
		algo_issuerlist_msg():
			algo_msg_base()
		{
		}
		virtual nlohmann::json to_json() const
		{
			auto j = algo_msg_base::to_json();
			j["action"] = "issuerlist";
			j["issuers"] = issuers;
			return j;
		}
		virtual void on_command()
		{
			auto* self = dynamic_cast<s1algo*>(al);
			int i=0;
			for(auto f : self->selectedIssuer) {
				string iss = f;
				if(i>0){
					issuers += "," + iss;
				}else{
					issuers = iss;
				}
				i++;
			}
			ouputQueue.enqueue(this);
		}
		virtual void release()
		{
			algo_issuerlist_msg_pool.release_obj(this);
		}
		virtual ~algo_issuerlist_msg() = default;
	};
	struct algo_underlyinglist_msg: public algo_msg_base
	{
		std::string ucodes;
		algo_underlyinglist_msg():
			algo_msg_base()
		{
		}
		virtual nlohmann::json to_json() const
		{
			auto j = algo_msg_base::to_json();
			j["action"] = "underlyinglist";
			j["ucodes"] = ucodes;
			return j;
		}
		virtual void on_command()
		{
			auto* self = dynamic_cast<s1algo*>(al);
			int i=0;
			for(auto f : self->availableUCode) {
				unsigned int iss = f;
				if(i>0){
					ucodes += "," + to_string(iss);
				}else{
					ucodes = to_string(iss);
				}
				i++;
			}
			ouputQueue.enqueue(this);
		}
		virtual void release()
		{
			algo_underlyinglist_msg_pool.release_obj(this);
		}
		virtual ~algo_underlyinglist_msg() = default;
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
	virtual vector<warrant*> getWinpriceWarrantFromMarketByIssuer(std::string issuercode, unsigned int underlying, unsigned long long ubid, unsigned long long uask);

	virtual bool doWarrantAction(warrant* w, dbp::top::order_side side, unsigned long long price, unsigned long long quantity);

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
	virtual unsigned long long issuerSize80(unsigned long long size);
	virtual string setBetsize(std::string betsize);

	virtual bool setWinSell(std::string action, unsigned int ucode, unsigned int code);
	virtual bool setWinLvlSell(std::string action, unsigned int ucode, unsigned int code);
	virtual bool setSelectedIssuer(std::string action, std::string issuer);
	virtual bool setSelectedUnderlying(std::string action, unsigned int ucode);
	virtual unsigned long long getBestBid(unsigned int code);
	virtual void forcesold();
	virtual bool checkPrice(unsigned int code, unsigned long long ubid, unsigned long long uask);
	virtual bool force_sell(unsigned int ucode, unsigned int code, unsigned long long price);
	virtual bool setSelectedWarrant(std::string action, unsigned int code);

public:
	static rapid_ring::spmc_ring_buffer_object_pool<algo_err_msg, 8192> algo_err_msg_pool;
	static rapid_ring::spsc_ring_buffer_object_pool<algo_marketstatus_msg, 8192> algo_marketstatus_msg_pool;
	static rapid_ring::spsc_ring_buffer_object_pool<algo_setbet_msg, 8192> algo_setbet_msg_pool;
	static rapid_ring::spsc_ring_buffer_object_pool<algo_issueraction_msg, 8192> algo_issueraction_msg_pool;
	static rapid_ring::spsc_ring_buffer_object_pool<algo_underlyingaction_msg, 8192> algo_underlyingaction_msg_pool;
	static rapid_ring::spsc_ring_buffer_object_pool<algo_warrantaction_msg, 8192> algo_warrantaction_msg_pool;
	static rapid_ring::spmc_ring_buffer_object_pool<algo_order_msg, 8192> algo_order_msg_pool;
	static rapid_ring::spmc_ring_buffer_object_pool<algo_portfolio_msg, 8192> algo_portfolio_msg_pool;
	static rapid_ring::spmc_ring_buffer_object_pool<algo_signal_msg, 8192> algo_signal_msg_pool;
	static rapid_ring::spmc_ring_buffer_object_pool<algo_stoplost_msg, 8192> algo_stoplost_msg_pool;
	static rapid_ring::spmc_ring_buffer_object_pool<algo_winsell_msg, 8192> algo_winsell_msg_pool;
	static rapid_ring::spmc_ring_buffer_object_pool<algo_winlvlsell_msg, 8192> algo_winlvlsell_msg_pool;
	static rapid_ring::spsc_ring_buffer_object_pool<algo_force_sell, 8192> algo_force_sell_pool;
	static rapid_ring::spmc_ring_buffer_object_pool<algo_warrantprice_msg, 8192> algo_warrantprice_msg_pool;
	static rapid_ring::spmc_ring_buffer_object_pool<algo_issuerlist_msg, 8192> algo_issuerlist_msg_pool;
	static rapid_ring::spmc_ring_buffer_object_pool<algo_underlyinglist_msg, 8192> algo_underlyinglist_msg_pool;
};





#endif /* USER_ALGO_INC_S1ALGO_HPP_ */
