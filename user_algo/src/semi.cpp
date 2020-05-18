#include <semi.hpp>


semi::semi(user& u, const std::string& name):
	algo(u, name)
{
}

void semi::on_omdc_book(const Tradable&)
{
}

void semi::on_omdd_book(const Tradable&)
{
}

void semi::on_omdc_trade(const Tradable&)
{
}

void semi::on_omdd_trade(const Tradable&)
{
}

void semi::handler_order(const dbp::top::enhance_order&)
{
}

void semi::handle_command(const algo_msg_base&)
{
}

algo_msg_base* semi::json_to_msg(json&)
{
	return nullptr;
}

json semi::msg_to_json(algo_msg_base*)
{
	return json();
}

