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
public:
	s1algo() = delete;
	s1algo(user& u, const std::string& name);
	s1algo(const algo&) = delete;
	s1algo(algo&&) = delete;
	s1algo& operator= (const algo&) = delete;
	s1algo& operator= (algo&&) = delete;

	virtual vector<warrant*> getSelectedWarrantFromMarketByIssuer(std::string issuercode, unsigned int underlying, unsigned long long ubid, unsigned long long uask);

	virtual bool doWarrantAction(std::string action, unsigned char side, unsigned long long price, unsigned long long quantity);

	virtual void on_omdc_book(const Tradable&);
	virtual void on_omdd_book(const Tradable&);
	virtual void on_omdc_trade(const Tradable&);
	virtual void on_omdd_trade(const Tradable&);
	virtual void handler_order(const dbp::top::enhance_order&);

	virtual void handle_command(algo_msg_base&);
	virtual algo_msg_base* json_to_msg(json& msg);
	virtual std::string get_lib_name();

	virtual void Log(std::string msg);
};



#endif /* USER_ALGO_INC_S1ALGO_HPP_ */
