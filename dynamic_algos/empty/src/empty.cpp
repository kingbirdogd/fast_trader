#include <empty.hpp>

#include <vector>

//std::unordered_map<unsigned int, priceinfo*> bear::uprice_map;



empty::empty(user& u, const std::string& name):
	algo(u, name)
{

}

void empty::on_omdc_book(const Tradable& tradable)
{


/*
	//Get Best Bid / Ask data
	auto best_bid_price = static_cast<unsigned long long>(tradable.m_Bid[0].m_iPrice) * 100000;
	auto best_ask_price = static_cast<unsigned long long>(tradable.m_Ask[0].m_iPrice) * 100000;
	auto best_bid_qty = tradable.m_Bid[0].m_uQuantity;
	auto best_ask_qty = tradable.m_Ask[0].m_uQuantity;


*/

}


void empty::on_omdc_trade(const Tradable& tradable)
{

	/*
	 * Trade Information
	 * auto side = tradable.m_TradeSide;
		auto bid_price = static_cast<unsigned long long>(tradable.m_Bid[0].m_iPrice) * 100000;
		auto ask_price = static_cast<unsigned long long>(tradable.m_Ask[0].m_iPrice) * 100000;
		auto trade_price = static_cast<unsigned long long>(tradable.m_LastTradePrice) * 100000;
		auto trade_qty = static_cast<unsigned long long>(tradable.m_LastTradeQuantity);
		auto best_bid_vol = static_cast<unsigned long long>(tradable.m_Bid[0].m_uQuantity);
		auto best_ask_vol = static_cast<unsigned long long>(tradable.m_Ask[0].m_uQuantity);
		auto trade_sell_quantity = static_cast<unsigned long long>(tradable.m_AccumulateSellQuantity);
		auto trade_buy_quantity = static_cast<unsigned long long>(tradable.m_AccumulateBuyQuantity);
		auto untradeqty = static_cast<unsigned long long>(tradable.m_UnTradedQuantity);
	 *
	 */




	/* Place Order
	 *
	 * auto odr = this->_u.new_order(
			this,
			quantity,
			price,
			w->Code,
			side,
			dbp::top::order_type::sl,
			dbp::top::aon_type::non_ano,
			dbp::top::ignore_price_type::ignore,
			0);
	 *
	 *
	 */
}


void empty::on_omdd_book(const Tradable& )
{

}

void empty::on_omdd_trade(const Tradable& )
{

}


void empty::handler_order(const dbp::top::enhance_order& )
{
	/*
	 * Handle Order Response
	auto status = odr.status;
	auto side = odr.side;

	auto it = order_map.find(odr.order_id);
	if(order_map.end() != it)
	{
		unsigned int ucode = it->second;

		auto itObs = obMap.find(ucode);
		if(itObs == obMap.end()){
			Log("Code = " + to_string(ucode) + "OBS not Found");
			return;
		}

		OBSetting* obs = itObs->second;
		unsigned int code = odr.code;
		if (dbp::top::order_status::rejected == status || dbp::top::order_status::canceled == status || dbp::top::order_status::deleted == status || dbp::top::order_status::filled == status)
		{
			if (dbp::top::order_side::buy == side)
			{
				if (dbp::top::order_status::filled == status)
				{

				}
				if (dbp::top::order_status::canceled == status || dbp::top::order_status::rejected == status)
				{

				}
			}
			else if (dbp::top::order_side::sell == side)
			{
				if (dbp::top::order_status::filled == status)
				{

				}
				if (dbp::top::order_status::canceled == status || dbp::top::order_status::rejected == status)
				{

				}
			}
		}

	}*/
}

void empty::Log(string msg){
	logger->Log(string(DateUtil::getCurrentTime()) + " " + msg);
}

void empty::handle_command(algo_msg_base& msg)
{
	msg.on_command();
}

algo_msg_base* empty::json_to_msg(json& json)
{

	try
	{
		auto cmd = json["cmd"].get<std::string>();
		auto ref = json["ref"].get<std::string>();
		if (cmd == "aaa"){

			Log(json.dump());
			auto msg = algo_err_msg_pool.get_obj();
			msg->al = this;
			msg->algo_name = _name;
			msg->id = _u.get_id();
			msg->ref = ref;
			msg->action = "json_to_msg";
			msg->result = "FAIL";
			msg->reason = std::string("invalid command");
			return msg;
		}
		else
		{

			Log(json.dump());
			auto msg = algo_err_msg_pool.get_obj();
			msg->al = this;
			msg->algo_name = _name;
			msg->id = _u.get_id();
			msg->ref = ref;
			msg->action = "json_to_msg";
			msg->result = "FAIL";
			msg->reason = std::string("invalid command");
			return msg;
		}
	}
	catch(const std::exception& e)
	{
		auto msg = algo_err_msg_pool.get_obj();
		msg->al = this;
		msg->algo_name = _name;
		msg->id = _u.get_id();
		msg->ref = "unknown";
		msg->action = "json_to_msg";
		msg->result = "FAIL";
		msg->reason = std::string("Exception");

		return msg;
	}
}

std::string empty::get_lib_name()
{
	return "empty";
}

void empty::on_tcp_book(const Tradable&)
{
}

void empty::on_tcp_trade(const Tradable&)
{
}

rapid_ring::spmc_ring_buffer_object_pool<empty::algo_err_msg, 8192> empty::algo_err_msg_pool;

