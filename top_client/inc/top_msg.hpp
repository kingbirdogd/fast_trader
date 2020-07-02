#ifndef DBP_TRADER_INC_DBP_EMS_TOP_MSG_HPP_
#define DBP_TRADER_INC_DBP_EMS_TOP_MSG_HPP_
#include <cstring>
#include <string>
#include <vector>
#include <picosha2.hpp>
#include <json.hpp>
#include <hex.hpp>
namespace dbp
{
	namespace top
	{
		using json_type = nlohmann::json;
		#pragma pack(push, 1)
		enum class api_type_flag : unsigned char
		{
			request = 0x54,
			single_reply = 0x55,
			multi_reply = 0x56,
			push = 0x57,
			error = 0x58
		};
		inline std::string to_string(const api_type_flag& v)
		{
			switch (v)
			{
				case api_type_flag::request:
					return "api_type_flag::request";
				case api_type_flag::single_reply:
					return "api_type_flag::single_reply";
				case api_type_flag::multi_reply:
					return "api_type_flag::multi_reply";
				case api_type_flag::push:
					return "api_type_flag::push";
				case api_type_flag::error:
					return "api_type_flag::error";
				default:
					return "api_type_flag::unknow";

			}
		}
		enum class api_id_flag : unsigned int
		{
			login = 81102101,
			change_pass = 81102102,
			new_order = 81101101,
			modify_order = 81101102,
			cancel_order = 81101103,
			odr_push = 81101999,
			order_report = 81101104,
			all_order_report = 81101105,
			all_filled_order_report = 81101106,
			all_open_order_report = 81101109,
			cash_balance = 81101107,
			instrument_balance = 81101108,
			buy_power = 81101110,
			heart_beat = 81102103,
			local_time = 81102104
		};
		inline std::string to_string(const api_id_flag& v)
		{
			switch (v)
			{
				case api_id_flag::login:
					return "api_id_flag::login";
				case api_id_flag::change_pass:
					return "api_id_flag::change_pass";
				case api_id_flag::new_order:
					return "api_id_flag::new_order";
				case api_id_flag::modify_order:
					return "api_id_flag::modify_order";
				case api_id_flag::cancel_order:
					return "api_id_flag::cancel_order";
				case api_id_flag::order_report:
					return "api_id_flag::order_report";
				case api_id_flag::all_order_report:
					return "api_id_flag::all_order_report";
				case api_id_flag::all_filled_order_report:
					return "api_id_flag::all_filled_order_report";
				case api_id_flag::all_open_order_report:
					return "api_id_flag::all_open_order_report";
				case api_id_flag::cash_balance:
					return "api_id_flag::cash_balance";
				case api_id_flag::instrument_balance:
					return "api_id_flag::instrument_balance";
				case api_id_flag::buy_power:
					return "api_id_flag::buy_power";
				case api_id_flag::heart_beat:
					return "api_id_flag::heart_beat";
				case api_id_flag::local_time:
					return "api_id_flag::local_time";
				default:
					return "api_id_flag::unknow";

			}
		}
		enum class report_type: unsigned char
		{
			order_report = 0x00,
			order_fill = 0x01,
			order_modify_approve = 0x02,
			order_modify_reject = 0x03,
			order_cancel_approve = 0x04,
			order_cancel_reject = 0x05,
			unsolicited_order_cancel = 0x06,
			trade_cancel = 0x07
		};
		inline std::string to_string(const report_type& v)
		{
			switch (v)
			{
				case report_type::order_report:
					return "report_type::order_report";
				case report_type::order_fill:
					return "report_type::order_fill";
				case report_type::order_modify_approve:
					return "report_type::order_modify_approve";
				case report_type::order_modify_reject:
					return "report_type::order_modify_reject";
				case report_type::order_cancel_approve:
					return "report_type::order_cancel_approve";
				case report_type::order_cancel_reject:
					return "report_type::order_cancel_reject";
				case report_type::unsolicited_order_cancel:
					return "report_type::unsolicited_order_cancel";
				case report_type::trade_cancel:
					return "report_type::trade_cancel";
				default:
					return "report_type::unknow";
			}
		}
		enum class order_status : unsigned int
		{
			queued = 301,
			rejected = 401,
			canceled = 402,
			deleted = 403,
			partial_fill = 601,
			filled = 801,
			unknow = 999
		};
		inline std::string to_string(const order_status& v)
		{
			switch (v)
			{
				case order_status::queued:
					return "order_status::queued";
				case order_status::rejected:
					return "order_status::rejected";
				case order_status::canceled:
					return "order_status::canceled";
				case order_status::deleted:
					return "order_status::deleted";
				case order_status::partial_fill:
					return "order_status::partial_fill";
				case order_status::filled:
					return "order_status::filled";
				case order_status::unknow:
					return "order_status::unknow";
				default:
					return "order_status::unknow";
			}
		}
		enum class error_code : unsigned int
		{
			unauthorized = 61001,
			invalid_session = 61002,
			insufficient_fund = 81001,
			insufficient_instrument = 81002,
			order_not_found = 81003,
			market_closed = 81004,
			out_of_throttle = 81005,
			system_not_ready = 81006
		};
		inline std::string to_string(const error_code& v)
		{
			switch (v)
			{
				case error_code::unauthorized:
					return "error_code::unauthorized";
				case error_code::invalid_session:
					return "error_code::invalid_session";
				case error_code::insufficient_fund:
					return "error_code::insufficient_fund";
				case error_code::insufficient_instrument:
					return "error_code::insufficient_instrument";
				case error_code::order_not_found:
					return "error_code::order_not_found";
				case error_code::market_closed:
					return "error_code::market_closed";
				case error_code::out_of_throttle:
					return "error_code::out_of_throttle";
				case error_code::system_not_ready:
					return "error_code::system_not_ready";
				default:
					return "error_code::unknow";
			}
		}
		enum class logon_response : unsigned int
		{
			success = 0,
			wrong_pass = 1,
			expired = 2,
			locked = 3,
			unknow = 99
		};
		inline std::string to_string(const logon_response& v)
		{
			switch (v)
			{
				case logon_response::success:
					return "error_code::success";
				case logon_response::wrong_pass:
					return "error_code::wrong_pass";
				case logon_response::expired:
					return "error_code::expired";
				case logon_response::locked:
					return "error_code::locked";
				case logon_response::unknow:
					return "error_code::unknow";
				default:
					return "error_code::unknow";
			}
		}
		enum class order_side : unsigned char
		{
			buy = 0x1,
			sell = 0x2,
			short_sell = 0x5
		};
		inline std::string to_string(const order_side& v)
		{
			switch (v)
			{
				case order_side::buy:
					return "order_side::buy";
				case order_side::sell:
					return "order_side::sell";
				case order_side::short_sell:
					return "order_side::short_sell";
				default:
					return "order_side::unknow";
			}
		}
		enum class order_type : unsigned char
		{
			al = 0x41,
			ao = 0x4F,
			sl = 0x53,
			el = 0x45,
			pl = 0x50
		};
		inline std::string to_string(const order_type& v)
		{
			switch (v)
			{
				case order_type::al:
					return "order_type::al";
				case order_type::ao:
					return "order_type::ao";
				case order_type::sl:
					return "order_type::sl";
				case order_type::el:
					return "order_type::el";
				case order_type::pl:
					return "order_type::pl";
				default:
					return "order_type::unknow";
			}
		}
		enum class aon_type : unsigned char
		{
			non_ano = 0x00,
			ano = 0x01
		};
		inline std::string to_string(const aon_type& v)
		{
			switch (v)
			{
				case aon_type::non_ano:
					return "aon_type::non_ano";
				case aon_type::ano:
					return "aon_type::ano";
				default:
					return "aon_type::unknow";
			}
		}
		enum class ignore_price_type : unsigned char
		{
			non_ignore = 0x00,
			ignore = 0x01
		};
		inline std::string to_string(const ignore_price_type& v)
		{
			switch (v)
			{
				case ignore_price_type::non_ignore:
					return "ignore_price_type::non_ignore";
				case ignore_price_type::ignore:
					return "ignore_price_type::ignore";
				default:
					return "ignore_price_type::unknow";
			}
		}
		struct header
		{
			unsigned char filler;
			unsigned int len;
			api_type_flag flag;
			api_id_flag api_id;
			unsigned char session_id[8];
			unsigned char filler6[6];
			unsigned long long order_id;
			json_type to_json() const
			{
				json_type json;
				json["len"] = len;
				json["flag"] = dbp::top::to_string(flag);
				json["first_ref"] = (char)(filler6[0]);
				json["api_id"] = dbp::top::to_string(api_id);
				json["order_id"] = order_id;
				json["session_id"] = get_hex(session_id, 8);
				json["ref_data"] = get_hex(filler6, 14);
				return json;
			}
			std::string to_string() const
			{
				return to_json().dump();
			}
			header():
				filler(0x01),
				len(0),
				flag(api_type_flag::request),
				api_id(api_id_flag::login),
				session_id{0},
				filler6{' ', ' ', ' ', ' ', ' ', ' '},
				order_id(0)
			{
			}
			header(const std::size_t size, const api_type_flag& _type_flag,
					const api_id_flag& _api_id,
					const unsigned char* _session_id,
					const unsigned long long& _order_id):
				filler(0x01),
				len(size),
				flag(_type_flag),
				api_id(_api_id),
				session_id{0},
				filler6{0},
				order_id(_order_id)
			{
				if (nullptr != _session_id)
					std::memcpy(session_id, _session_id, sizeof(session_id));
				else
					std::memset(session_id, 0, sizeof(session_id));
			}
			bool operator < (const header& h) const
			{
				return order_id < h.order_id;
			}
		};
		struct error_reply : public header
		{
			error_code code;
			json_type to_json() const
			{
				const header& h = *this;
				json_type json;
				json["header"] = h.to_json();
				json["code"] = dbp::top::to_string(code);
				return json;
			}
			std::string to_string() const
			{
				return to_json().dump();
			}
			error_reply(error_code _code):
				header
				(
					sizeof(error_reply),
					api_type_flag::error,
					api_id_flag::login,
					nullptr,
					0xFFFFFFFFFFFFFFFF
				),
				code(_code)
			{
			}

		};
		struct login_request: public header
		{
			char user_name[50];
			char password[200];
			json_type to_json() const
			{
				const header& h = *this;
				json_type json;
				json["header"] = h.to_json();
				json["user_name"] = user_name;
				json["password"] = password;
				return json;
			}
			std::string to_string() const
			{
				return to_json().dump();
			}
			login_request(const char* _user_name, const char* _passwrod):
				header
				(
					sizeof(login_request),
					api_type_flag::request,
					api_id_flag::login,
					nullptr,
					0xFFFFFFFFFFFFFFFF
				),
				user_name{0},
				password{0}
			{
				std::strncpy(user_name, _user_name, sizeof(user_name));
				std::string sha256hex;
				std::string pass = _passwrod;
				picosha2::hash256_hex_string(pass, sha256hex);
				std::strncpy(password, sha256hex.c_str(), sizeof(password));
			}
		};
		struct login_response: public header
		{
			logon_response response;
			login_response():
				header
				(
					sizeof(login_response),
					api_type_flag::single_reply,
					api_id_flag::login,
					nullptr,
					0xFFFFFFFFFFFFFFFF
				),
				response(logon_response::unknow)
			{
			}
			json_type to_json() const
			{
				const header& h = *this;
				json_type json;
				json["header"] = h.to_json();
				json["response"] = dbp::top::to_string(response);
				return json;
			}
			std::string to_string() const
			{
				return to_json().dump();
			}
		};
		struct new_order_request: public header
		{
			unsigned long long quantity;
			unsigned long long price;
			unsigned int code;
			order_side side;
			order_type type;
			aon_type aon;
			ignore_price_type ignore;
			unsigned int broker_id;
			json_type to_json() const
			{
				const header& h = *this;
				json_type json;
				json["header"] = h.to_json();
				json["quantity"] = quantity;
				json["price"] = price;
				json["code"] = code;
				json["side"] = dbp::top::to_string(side);
				json["type"] = dbp::top::to_string(type);
				json["ano"] = dbp::top::to_string(aon);
				json["ignore"] = dbp::top::to_string(ignore);
				json["broker_id"] = broker_id;
				return json;
			}
			std::string to_string() const
			{
				return to_json().dump();
			}
			new_order_request(const unsigned char* _session_id,
					unsigned long long client_order_id,
					unsigned long long _quantity,
					unsigned long long _price,
					unsigned int _code,
					order_side _side,
					order_type _type,
					aon_type _aon,
					ignore_price_type _ignore,
					unsigned int _broker_id):
				header
				(
					sizeof(new_order_request),
					api_type_flag::request,
					api_id_flag::new_order,
					_session_id,
					client_order_id
				),
				quantity(_quantity),
				price(_price),
				code(_code),
				side(_side),
				type(_type),
				aon(_aon),
				ignore(_ignore),
				broker_id(_broker_id)
			{
			}
		};
		struct modify_order_request: public header
		{
			unsigned int order_ref;
			unsigned long long quantity;
			unsigned long long price;
			json_type to_json() const
			{
				const header& h = *this;
				json_type json;
				json["header"] = h.to_json();
				json["order_ref"] = order_ref;
				json["quantity"] = quantity;
				json["price"] = price;
				return json;
			}
			std::string to_string() const
			{
				return to_json().dump();
			}
			modify_order_request(const unsigned char* _session_id,
				unsigned long long client_order_id,
				unsigned int _order_ref,
				unsigned long long _quantity,
				unsigned long long _price):
				header
				(
					sizeof(modify_order_request),
					api_type_flag::request,
					api_id_flag::modify_order,
					_session_id,
					client_order_id
				),
				order_ref(_order_ref),
				quantity(_quantity),
				price(_price)
			{
			}
		};
		struct cancel_order_request: public header
		{
			unsigned int order_ref;
			json_type to_json() const
			{
				const header& h = *this;
				json_type json;
				json["header"] = h.to_json();
				json["order_ref"] = order_ref;
				return json;
			}
			std::string to_string() const
			{
				return to_json().dump();
			}
			cancel_order_request(const unsigned char* _session_id,
				unsigned long long client_order_id,
				unsigned int _order_ref):
				header
				(
					sizeof(cancel_order_request),
					api_type_flag::request,
					api_id_flag::cancel_order,
					_session_id,
					client_order_id
				),
				order_ref(_order_ref)
			{
			}
		};
		struct order_report : public header
		{
			unsigned int order_ref;
			order_status status;
			unsigned long long quantity;
			unsigned long long price;
			unsigned long long remain_quantity;
			unsigned long long filled_quantity;
			unsigned long long match_quantity;
			unsigned long long match_price;
			unsigned int code;
			order_side side;
			order_type type;
			aon_type aon;
			report_type rep_type;
			char broker_id[12];
			char transaction_tm[25];
			char exectuion_id[21];
			char reject_reason[96];
			unsigned int report_id;
			order_report():
				header
				(
					sizeof(order_report),
					api_type_flag::single_reply,
					api_id_flag::modify_order,
					nullptr,
					0
				),
				order_ref(0),
				status(order_status::queued),
				quantity(0),
				price(0),
				remain_quantity(0),
				filled_quantity(0),
				match_quantity(0),
				match_price(0),
				code(0),
				side(order_side::buy),
				type(order_type::pl),
				aon(aon_type::non_ano),
				rep_type(report_type::order_report),
				broker_id{0},
				transaction_tm{0},
				exectuion_id{0},
				reject_reason{0},
				report_id{0}
			{
			}
			std::string get_broker_id()
			{
				return broker_id;
			}
			std::string get_transaction_tm()
			{
				return transaction_tm;
			}
			std::string get_exectuion_id()
			{
				return exectuion_id;
			}
			std::string get_reject_reason()
			{
				return reject_reason;
			}
			json_type to_json() const
			{
				const header& h = *this;
				json_type json;
				json["header"] = h.to_json();
				json["order_ref"] = order_ref;
				json["status"] = dbp::top::to_string(status);
				json["quantity"] = quantity;
				json["price"] = price;
				json["remain_quantity"] = remain_quantity;
				json["filled_quantity"] = filled_quantity;
				json["match_quantity"] = match_quantity;
				json["match_price"] = match_price;
				json["code"] = code;
				json["side"] = dbp::top::to_string(side);
				json["type"] = dbp::top::to_string(type);
				json["aon"] = dbp::top::to_string(aon);
				json["rep_type"] = dbp::top::to_string(rep_type);
				json["broker_id"] = broker_id;
				json["transaction_tm"] = transaction_tm;
				json["exectuion_id"] = exectuion_id;
				json["reject_reason"] = reject_reason;
				json["report_id"] = report_id;
				return json;
			}
			std::string to_string() const
			{
				std::string rt = to_json().dump();
				return rt;
			}
			inline bool is_valid()
			{
				return (0 != order_id);
			}
			inline bool is_on_fly()
			{
				return (0 == order_ref);
			}
			inline operator bool ()
			{
				return is_valid();
			}
		};
		struct buy_power_request: public header
		{
			json_type to_json() const
			{
				const header& h = *this;
				json_type json;
				json["header"] = h.to_json();
				return json;
			}
			std::string to_string() const
			{
				return to_json().dump();
			}
			buy_power_request(const unsigned char* _session_id,
					unsigned long long client_order_id):
				header
				(
					sizeof(buy_power_request),
					api_type_flag::request,
					api_id_flag::buy_power,
					_session_id,
					client_order_id
				)
			{
			}
		};
		struct buy_power_response : public header
		{
			long long ledger;
			long long special_limit;
			long long buying_power;
			char currency[4];
			buy_power_response():
				header
				(
					sizeof(buy_power_response),
					api_type_flag::single_reply,
					api_id_flag::buy_power,
					nullptr,
					0
				),
				ledger(0),
				special_limit(0),
				buying_power(0),
				currency{0}
			{
			}
			json_type to_json() const
			{
				const header& h = *this;
				json_type json;
				json["header"] = h.to_json();
				json["ledger"] = ledger;
				json["special_limit"] = special_limit;
				json["buying_power"] = buying_power;
				json["currency"] = currency;
				return json;
			}
			std::string to_string() const
			{
				std::string rt = to_json().dump();
				return rt;
			}
		};
		#pragma pack(pop)
		struct match_record
		{
			unsigned long long match_quantity;
			unsigned long long match_price;
			match_record():
				match_quantity(0),
				match_price(0)
			{
			}
			match_record(unsigned long long quantity, unsigned long long price):
				match_quantity(quantity),
				match_price(price)
			{
			}
			json_type to_json() const
			{
				json_type json;
				json["match_quantity"] = match_quantity;
				json["match_price"] = match_price;
				return json;
			}
			std::string to_string() const
			{
				return to_json().dump();
			}
		};
		struct enhance_order : public order_report
		{
			unsigned long long ori_quantity;
			unsigned long long ori_price;
			std::vector<match_record> match_records;
			enhance_order():
				order_report(),
				ori_quantity(0),
				ori_price(0),
				match_records()
			{
			}
			enhance_order(const order_report& report):
				order_report(),
				ori_quantity(0),
				ori_price(0),
				match_records()
			{
				order_report& self = *this;
				self = report;
				if (report_type::order_fill == report.rep_type)
				{
					match_records.push_back(match_record(report.match_quantity, report.match_price));
				}
			}
			enhance_order& operator= (const order_report& report)
			{
				order_report& self = *this;
				self = report;
				if (report_type::order_fill == report.rep_type)
				{
					match_records.push_back(match_record(report.match_quantity, report.match_price));
				}
				return *this;
			}
			json_type to_json() const
			{
				const order_report& self = *this;
				json_type json = self.to_json();
				json["ori_quantity"] = ori_quantity;
				json["ori_price"] = ori_price;
				json["match_records"] = json_type::array();
				for (std::size_t i = 0; i < match_records.size(); ++i)
				{
					json["match_records"].push_back(match_records[i].to_json());
				}
				return json;
			}
			std::string to_string() const
			{
				return to_json().dump();
			}
		};
	}
}



#endif /* DBP_TRADER_INC_DBP_EMS_TOP_MSG_HPP_ */
