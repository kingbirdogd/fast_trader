#include <top_tcp_client.hpp>
#include <iostream>

int main(void)
{
	bool running = true;
	top_tcp_client cli(1,"192.168.134.38", 35101, "P990098", "pass");
	cli.set_on_login([&](const dbp::top::login_response& response)
	{
		if (dbp::top::logon_response::success == response.response)
		{
			std::cout << "login success:" << response.to_string() << std::endl;
			auto report = cli.new_order(50000000000ULL, 8595000000ULL, 1, dbp::top::order_side::buy);
			std::cout << "new order_id:" << report.to_string() << std::endl;
			if (!report)
			{
				std::cout << "new order fail:" << report.to_string() << std::endl;
				running = false;
			}
		}
		else
		{
			std::cout << "login fail:" << response.to_string() << std::endl;
			running = false;
		}
	});
	cli.set_on_order([&](const dbp::top::enhance_order& report)
	{
		if (dbp::top::report_type::order_report == report.rep_type)
		{
			if (dbp::top::order_status::rejected == report.status)
			{
				std::cout << "order report new reject:" << report.to_string() << ", order_id:"<< report.order_id << ", status:" <<(int)report.status << std::endl;
				running = false;
				return;
			}
			std::cout << "order report new success::" << report.to_string() << ", order_id:"<< report.order_id << ", status:" <<(int)report.status << std::endl;
			if (!cli.modify_order(report.order_id, 50000000000ULL, 9100000000ULL))
			{
				std::cout << "modify order fail" << std::endl;
				running = false;
			}
		}
		else if (dbp::top::report_type::order_modify_approve == report.rep_type)
		{
			if (dbp::top::order_status::rejected == report.status)
			{
				std::cout << "order report  modify reject:" << report.to_string() << ", order_id:"<< report.order_id << ", status:" <<(int)report.status << std::endl;
				running = false;
				return;
			}
			std::cout << "order report modify success:" << report.to_string() << ", order_id:"<< report.order_id<< ", status:" <<(int)report.status << std::endl;
			if (!cli.cancel_order(report.order_id))
			{
				std::cout << "cancel order fail" << std::endl;
				running = false;
			}
		}
		else if (dbp::top::report_type::order_modify_reject == report.rep_type)
		{
			if (dbp::top::order_status::rejected == report.status)
			{
				std::cout << "order report  cancel reject:" << report.to_string() << ", order_id:"<< report.order_id << ", status:" <<(int)report.status << std::endl;
				running = false;
				return;
			}
			std::cout << "order report modify reject:" << report.to_string() << ", order_id:"<< report.order_id << ", status:" <<(int)report.status << std::endl;
			if (!cli.cancel_order(report.order_id))
			{
				std::cout << "cancel order fail" << std::endl;
				running = false;
			}
		}
		else if (dbp::top::report_type::order_cancel_approve == report.rep_type)
		{
			if (dbp::top::order_status::rejected == report.status)
			{
				std::cout << "order report  cancel reject:" << report.to_string() << ", order_id:"<< report.order_id << ", status:" <<(int)report.status << std::endl;
				running = false;
				return;
			}
			std::cout << "order report cancel success:" << report.to_string() << ", order_id:"<< report.order_id << ", status:" <<(int)report.status << std::endl;
			running = false;
		}
		else if (dbp::top::report_type::order_cancel_reject == report.rep_type)
		{
			if (dbp::top::order_status::rejected == report.status)
			{
				std::cout << "order report  cancel reject:" << report.to_string() << ", order_id:"<< report.order_id << ", status:" <<(int)report.status << std::endl;
				running = false;
				return;
			}
			std::cout << "order report cancel reject:" << report.to_string() << ", order_id:"<< report.order_id << ", status:" <<(int)report.status << std::endl;
			running = false;
		}
		else if (dbp::top::report_type::order_fill == report.rep_type)
		{
			std::cout << "order report filled:" << report.to_string() << ", order_id:"<< report.order_id << ", status:" <<(int)report.status << std::endl;
			running = false;
		}
		else
		{
			if (0 == (int)report.status)
			{
				return;
			}
			std::cout << "order report unknow type:" << report.to_string() << ", order_id:"<< report.order_id << ", report_type:" << (int)report.rep_type << std::endl;
			running = false;
		}
	});
	while (running)
	{
		cli.run();
	}
	return 0;
}




