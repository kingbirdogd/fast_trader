#ifndef ALGOS_INC_SEMI_HPP_
#define ALGOS_INC_SEMI_HPP_

#include <algo.hpp>

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
