#include <vector>
#include <s1algo.hpp>

//std::unordered_map<unsigned int, priceinfo*> bear::uprice_map;

s1algo::s1algo(user& u, const std::string& name):
	algo(u, name)
{
	selectedIssuer.insert("MB");
}

void s1algo::on_omdc_book(const Tradable& tradable)
{

	auto best_bid_price = static_cast<unsigned long long>(tradable.m_Bid[0].m_iPrice) * 100000;
	auto best_ask_price = static_cast<unsigned long long>(tradable.m_Ask[0].m_iPrice) * 100000;

	auto it = s1SignalMap.find(tradable.m_Code);
	if(it != s1SignalMap.end()){

		OBSetting* obs = obMap[tradable.m_Code];
		if(it->second->hasSignal){
			if(obs->detected){
				if(best_ask_price != obs->DetectedAsk){
					obs->Status = STATUS_NEW;
					obs->detected = false;
				}
			}
			else
			{
				if(best_ask_price == it->second->DetectAsk){
					obs->DetectedAsk = best_ask_price;

					vector<warrant*> selectedWarrant = getSelectedWarrantFromMarketByIssuer("MB",tradable.m_Code, best_bid_price,best_ask_price );
					if(selectedWarrant.size() == 0)
						return;
					for(unsigned int i=0; i<selectedWarrant.size(); i++){
						warrant* w = selectedWarrant[i];

						w->Status = STATUS_READY;
						obs->addWarrantOrCbbc(w);
					}
					obs->Status = STATUS_READY;
					obs->detected = true;

					Log("Code = " + to_string(tradable.m_Code) + " Has Signal");
				}
			}
		}
		else
		{
			if(obs->detected ){
				obs->removeAllWarrants();
				obs->Status = STATUS_NEW;
				obs->detected = false;

				Log("Code = " + to_string(tradable.m_Code) + " Reset Signal");
			}
		}
	}

}

bool myfunction (warrant* i,warrant* j) {
	return i->Egearing > j->Egearing;
}

vector<warrant*> s1algo::getSelectedWarrantFromMarketByIssuer(std::string issuer, unsigned int underlying, unsigned long long ubid, unsigned long long uask)
{

	unsigned long long uspread =  uask - ubid;

	vector<warrant*> selectedWarrant;
	unordered_set<unsigned int> warrantVector = ivLoader.getWarrantByIssuer(issuer,underlying);
	for (const auto &n: warrantVector) {
		auto it = omdcMap.find(n);
		if(it != omdcMap.end()){
			auto wbest_bid_price = static_cast<unsigned long long>(it->second.m_Bid[0].m_iPrice) * 100000;
			auto wbest_ask_price = static_cast<unsigned long long>(it->second.m_Ask[0].m_iPrice) * 100000;

			if(wbest_ask_price < 4000000){
				continue;
			}

			if(wbest_bid_price == 0 || wbest_ask_price == 0)
				continue;

			unsigned long long wspread = wbest_ask_price - wbest_bid_price;

			WarrantIv wiv = ivLoader.getWarrantIv(n);

			if(wspread <= 0){
				continue;
			}

			if(wiv.Code == 0){
				continue;
			}
			if(wiv.Egearing < 4){
				continue;
			}

			bool accept = CSelectedWarrant.isAccept(uspread, wiv.Delta, wiv.Cratio, wspread, 2);

			if(accept){
				warrant* newWarrant = new warrant;
				newWarrant->Date = DateUtil::getToday();
				newWarrant->Code = n;
				newWarrant->Status = STATUS_READY;
				newWarrant->Egearing = wiv.Egearing;
				newWarrant->UCode = underlying;
				newWarrant->RefWBid = wbest_bid_price;
				newWarrant->RefWAsk = wbest_ask_price;
				newWarrant->Quantity = 0;
				newWarrant->Issuer = wiv.Issuer;
				newWarrant->Status = STATUS_READY;

				selectedWarrant.push_back(newWarrant);
			}
		}
	}

	if(selectedWarrant.size() > (unsigned int)MaxBuyNoWarrant){

		std::sort (selectedWarrant.begin(), selectedWarrant.end(), myfunction);

		selectedWarrant.erase(selectedWarrant.begin()+MaxBuyNoWarrant, selectedWarrant.end());
	}
	return selectedWarrant;
}

void s1algo::on_omdc_trade(const Tradable& )
{


}

void s1algo::on_omdd_book(const Tradable& )
{

}

void s1algo::on_omdd_trade(const Tradable& )
{

}


void s1algo::handler_order(const dbp::top::enhance_order& )
{

}

void s1algo::Log(string msg){
	fprintf(stderr, "%s %s \n",DateUtil::getCurrentTime(), msg.c_str());
}

void s1algo::handle_command(algo_msg_base& msg)
{
	msg.on_command();
}

algo_msg_base* s1algo::json_to_msg(json& )
{
	return nullptr;
}

std::string s1algo::get_lib_name()
{
	return "s1algo";
}


