function getWsMsg(type, id){
	/*if(isRecovery){
		return "";
	}*/
	
	var msg = "";
	var wtype = "";
	var ucode= "";
	var wcode = "";
	var vol = "";
	var uprice = "";
	var wprice = "";
	var bottom = "";
	var ceiling = "";
	var t_vol = "";
	var id2="";
	var idArr="";
	var code="";
	var orderno="";
	var reftype="";
	var buy_vol="";
	var sell_vol="";
	var buy_ratio=0;
	var sell_ratio=0;
	
	if(id != "" && type!="get"){
		if(type == "getbid" || type == "getask"){
			id2=id;
			id=$("#"+id).closest("tr").attr("id");
		}else if(type == "getucode"){
			id2=id+"code";
		}else if(type == "stopsell"){
			id2=id;
			id=id2.replace("_stopsell", "");
		}else if(type == "getbid_checkforcesell"){
			id2=id+"checkforcesell";
			type = "getbid";
		}
		
		idArr = id.split("_");
		//ucode= $("#"+idArr[0]+"ucode").val()*1;
		ucode= $("#"+idArr[0]+"ucode").val().toUpperCase();
		wcode= $("#"+id+"code").val()*1;
		
		updateFactor(ucode);
		
		vol = getValue($("#"+id+"vol").val())*factor_stock;
		vol = vol.toFixed(0);
		uprice= $("#"+id+"buy_ulast").val()*factor;
		if(isNaN(uprice)){
			uprice=0;
		}
		uprice = uprice.toFixed(0);
		uprice2= $("#"+id+"sell_ulast").val()*factor;
		if(isNaN(uprice2)){
			uprice2=0;
		}
		uprice2 = uprice2.toFixed(0);
		wprice= $("#"+id+"buy_last").val()*factor_stock;
		if(isNaN(wprice)){
			wprice=0;
		}
		wprice = wprice.toFixed(0);
		wprice2= $("#"+id+"sell_last").val()*factor_stock;
		if(isNaN(wprice2)){
			wprice2=0;
		}
		wprice2 = wprice2.toFixed(0);
		bottom= $("#"+id+"bottom").val()*factor_stock;
		bottom = bottom.toFixed(0);
		if(isNaN(bottom)){
			bottom=0;
		}
		ceiling= $("#"+id+"ceiling").val()*factor_stock;
		ceiling = ceiling.toFixed(0);
		if(isNaN(ceiling)){
			ceiling=0;
		}
		t_vol = getValue($("#"+id+"t_vol_i").val())*factor_stock;
		t_vol = t_vol.toFixed(0);
		wtype = $("#"+id+"wtype").val();
		orderno=$("#"+id+"orderno").val();
		
		if(id2==id+"buy_last" || id2==id+"sell_last" || id2==id+"bottom" || id2==id+"ceiling" || id2==id+"code" || id2==id+"checkforcesell"){
			code=wcode;
		}else{
			code=ucode;
		}
		
		if(!isWarrant(wcode)){
			return "";
		}
		
		/*if(ucode>0 && ucode<10000){
		}else{
			return "";
		}*/
		
		if(type != "getucode" && !isUnderlying(ucode)){
			return "";
		}
		
		buy_vol = getValue($("#"+id+"buy_vol").val())*1;
		buy_vol = buy_vol.toFixed(0);
		
		if(isRatio($("#"+id+"buy_ratio").val())){
			buy_ratio = $("#"+id+"buy_ratio").val()*1;
		}
		
		sell_vol = getValue($("#"+id+"sell_vol").val())*1;
		sell_vol = sell_vol.toFixed(0);
		
		if(isRatio($("#"+id+"sell_ratio").val())){
			sell_ratio = $("#"+id+"sell_ratio").val()*1;
		}
	
		
		if(ucode*1>0){
		}else{
			ucode = '"'+ucode+'"';
		}
	}
	/*if(type=="setbuy" && $("#"+id+"as").hasClass("off")){
		type = "auto";
	}*/
	
	if(type=="setbuy" || type=="updatebuy" || type=="setsell" || type=="updatesell" || type=="stop" || type=="stopsell" || type=="auto"){
		var action = "";
		if(type=="setbuy"){
			 action = "BUY";
		}else if(type=="updatebuy" || type=="updatesell"){
			action = "NOCHANGE";
		}else if(type=="setsell"){
			action = "SELL";
		}else if(type=="stop" || type=="stopsell"){
			action = "STOP";
		}else if(type=="auto"){
			action = "AUTO";
		}
		
		var _vol = vol;
		if(type=="setsell" || type=="updatesell" || type=="stopsell"){
			_vol = t_vol;
		}
		
		var position = ', "position": '+t_vol;
		var resetposition = '';
		if(isAddPosstion){
			resetposition = ', "resetposition": true';
			_vol = vol;
			isAddPosstion = false;
		}
		
		return '{"cmd": "set", "type":"'+wtype+'", "warrant_code": '+wcode+', "underlying_code":'+ucode+', "buy_trriger": '+uprice+', "sell_trriger": '+uprice2+', "buy_price": '+wprice+', "sell_price": '+wprice2+', "bottom_price": '+bottom+', "ceiling_price": '+ceiling+', "auto_buy_quantity": '+_vol+', "early_buy_qty": '+buy_vol+', "early_sell_qty": '+sell_vol+', "ratio_buy": '+buy_ratio+', "ratio_sell": '+sell_ratio+', "id": '+user_id+', "action": "'+action+'", "algo_name":"'+key+'", "ref": "'+id+'"'+position+resetposition+'}';
	}
	/*if(type=="setbuy"){
		//set|wtype|underlying_code|warrant_code|buy_trigger_price|sell_trigger_price|buy_price|sell_price|bottom_price|ceiling_price|auto_buy_quantity|action|ref_id 
		//set | wtype | 股 | 證 | 觸發買價(股) | 觸發賣價(股) | 限買價(證) | 限賣價(證) | 止損價 | 止盈價 | 手數
		//msg = 'set|'+wtype+'|'+ucode+'|'+wcode+'|'+uprice+'|'+uprice2+'|'+wprice+'|'+wprice2+'|'+bottom+'|'+ceiling+'|'+vol+'|BUY|'+id+'|'+buy_vol+'|'+sell_vol;
		return '{"cmd": "set", "type":"'+wtype+'", "warrant_code": '+wcode+', "underlying_code":'+ucode+', "buy_trriger": '+uprice+', "sell_trriger": '+uprice2+', "buy_price": '+wprice+', "sell_price": '+wprice2+', "bottom_price": '+bottom+', "ceiling_price": '+ceiling+', "auto_buy_quantity": '+vol+', "early_buy_qty": '+buy_vol+', "early_sell_qty": '+sell_vol+', "id": '+user_id+', "action": "BUY", "algo_name":"'+key+'", "ref": "'+id+'"}';
	}else if(type=="updatebuy"){
		//msg = 'set|'+wtype+'|'+ucode+'|'+wcode+'|'+uprice+'|'+uprice2+'|'+wprice+'|'+wprice2+'|'+bottom+'|'+ceiling+'|'+vol+'|NOCHANGE|'+id+'|'+buy_vol+'|'+sell_vol;
		return '{"cmd": "set", "type":"'+wtype+'", "warrant_code": '+wcode+', "underlying_code":'+ucode+', "buy_trriger": '+uprice+', "sell_trriger": '+uprice2+', "buy_price": '+wprice+', "sell_price": '+wprice2+', "bottom_price": '+bottom+', "ceiling_price": '+ceiling+', "auto_buy_quantity": '+vol+', "early_buy_qty": '+buy_vol+', "early_sell_qty": '+sell_vol+', "id": '+user_id+', "action": "NOCHANGE", "algo_name":"'+key+'", "ref": "'+id+'"}';
	}else if(type=="setsell"){
		//msg = 'set|'+wtype+'|'+ucode+'|'+wcode+'|'+uprice+'|'+uprice2+'|'+wprice+'|'+wprice2+'|'+bottom+'|'+ceiling+'|'+t_vol+'|SELL|'+id+'|'+buy_vol+'|'+sell_vol+'|'+t_vol;
		return '{"cmd": "set", "type":"'+wtype+'", "warrant_code": '+wcode+', "underlying_code":'+ucode+', "buy_trriger": '+uprice+', "sell_trriger": '+uprice2+', "buy_price": '+wprice+', "sell_price": '+wprice2+', "bottom_price": '+bottom+', "ceiling_price": '+ceiling+', "auto_buy_quantity": '+vol+', "early_buy_qty": '+buy_vol+', "early_sell_qty": '+sell_vol+', "id": '+user_id+', "action": "SELL", "algo_name":"'+key+'", "ref": "'+id+'"}';
	}else if(type=="updatesell"){
		//msg = 'set|'+wtype+'|'+ucode+'|'+wcode+'|'+uprice+'|'+uprice2+'|'+wprice+'|'+wprice2+'|'+bottom+'|'+ceiling+'|'+t_vol+'|NOCHANGE|'+id+'|'+buy_vol+'|'+sell_vol+'|'+t_vol;
		return '{"cmd": "set", "type":"'+wtype+'", "warrant_code": '+wcode+', "underlying_code":'+ucode+', "buy_trriger": '+uprice+', "sell_trriger": '+uprice2+', "buy_price": '+wprice+', "sell_price": '+wprice2+', "bottom_price": '+bottom+', "ceiling_price": '+ceiling+', "auto_buy_quantity": '+vol+', "early_buy_qty": '+buy_vol+', "early_sell_qty": '+sell_vol+', "id": '+user_id+', "action": "NOCHANGE", "algo_name":"'+key+'", "ref": "'+id+'"}';
	}else if(type=="stop"){
		//msg = 'set|'+wtype+'|'+ucode+'|'+wcode+'|'+uprice+'|'+uprice2+'|'+wprice+'|'+wprice2+'|'+bottom+'|'+ceiling+'|'+vol+'|STOP|'+id+'|'+buy_vol+'|'+sell_vol;
		return '{"cmd": "set", "type":"'+wtype+'", "warrant_code": '+wcode+', "underlying_code":'+ucode+', "buy_trriger": '+uprice+', "sell_trriger": '+uprice2+', "buy_price": '+wprice+', "sell_price": '+wprice2+', "bottom_price": '+bottom+', "ceiling_price": '+ceiling+', "auto_buy_quantity": '+vol+', "early_buy_qty": '+buy_vol+', "early_sell_qty": '+sell_vol+', "id": '+user_id+', "action": "STOP", "algo_name":"'+key+'", "ref": "'+id+'"}';
	}else if(type=="stopsell"){
		//msg = 'set|'+wtype+'|'+ucode+'|'+wcode+'|'+uprice+'|'+uprice2+'|'+wprice+'|'+wprice2+'|'+bottom+'|'+ceiling+'|'+t_vol+'|STOP|'+id2+'|'+buy_vol+'|'+sell_vol;
		return '{"cmd": "set", "type":"'+wtype+'", "warrant_code": '+wcode+', "underlying_code":'+ucode+', "buy_trriger": '+uprice+', "sell_trriger": '+uprice2+', "buy_price": '+wprice+', "sell_price": '+wprice2+', "bottom_price": '+bottom+', "ceiling_price": '+ceiling+', "auto_buy_quantity": '+vol+', "early_buy_qty": '+buy_vol+', "early_sell_qty": '+sell_vol+', "id": '+user_id+', "action": "STOP", "algo_name":"'+key+'", "ref": "'+id+'"}';
	}else if(type=="auto"){
		//msg = 'set|'+wtype+'|'+ucode+'|'+wcode+'|'+uprice+'|'+uprice2+'|'+wprice+'|'+wprice2+'|'+bottom+'|'+ceiling+'|'+vol+'|AUTO|'+id+'|'+buy_vol+'|'+sell_vol;
		return '{"cmd": "set", "type":"'+wtype+'", "warrant_code": '+wcode+', "underlying_code":'+ucode+', "buy_trriger": '+uprice+', "sell_trriger": '+uprice2+', "buy_price": '+wprice+', "sell_price": '+wprice2+', "bottom_price": '+bottom+', "ceiling_price": '+ceiling+', "auto_buy_quantity": '+vol+', "early_buy_qty": '+buy_vol+', "early_sell_qty": '+sell_vol+', "id": '+user_id+', "action": "AUTO", "algo_name":"'+key+'", "ref": "'+id+'"}';
	}*/else if(type=="delete"){
		//delete|underlying_code|warrant_code
		//msg = 'delete|'+ucode+'|'+wcode+'|'+id;
		//msg = 'delete|'+id;
		return '{"cmd": "delete", "id": '+user_id+', "algo_name":"'+key+'", "ref": "'+id+'"}';
	}else if(type=="forcebuy"){
		//force_buy|underlying_code|warrant_code|quantity(0 for auto_buy_quantity)
		//msg = 'force_buy|'+ucode+'|'+wcode+'|0|'+id;
		//msg = 'force_buy|0|'+id;
		return '{"cmd": "force_buy", "quantity":'+vol+', "id": '+user_id+', "algo_name":"'+key+'", "ref": "'+id+'", "price": '+wprice+'}';
	}else if(type=="checkforcesell"){
		var _price = wprice2;
		if(_price<bottom){
			_price=bottom;
		}
		if(_price<ceiling){
			_price=ceiling;
		}
		return '{"cmd": "force_sell", "quantity":'+t_vol+', "id": '+user_id+', "algo_name":"'+key+'", "ref": "'+id+'", "price": '+_price+'}';
	}else if(type=="forcesell"){
		//force_sell|underlying_code|warrant_code|quantity(0 for clean all position) 
		//msg = 'force_sell|'+ucode+'|'+wcode+'|0|'+id;
		//msg = 'force_sell|0|'+id;
		//msg = 'force_sell|'+t_vol+'|'+id;
		return '{"cmd": "force_sell", "quantity":'+t_vol+', "id": '+user_id+', "algo_name":"'+key+'", "ref": "'+id+'", "price": '+wprice2+'}';
	}else if(type=="getbid" || type=="getask"){
		/*if(code*1>0){
			//return '{"type":"get_omdc_tradable", "code": '+code+', "ref": "'+id+"-"+id2+"-"+type+'"}';
			return '{"type":"get_omdc_bestprice", "code": '+code+', "ref": "'+id+"-"+id2+"-"+type+'"}';
		}else{
			//return '{"type":"get_omdd_tradable", "code": "'+code+'", "ref": "'+id+"-"+id2+"-"+type+'"}';
			return '{"type":"get_omdd_bestprice", "code": "'+code+'", "ref": "'+id+"-"+id2+"-"+type+'"}';
		}*/
		if(code*1>0){
			return '{"cmd": "get_omdc_tradable", "code": '+code+', "algo_name":"'+key+'", "ref": "'+id+"-"+id2+"-"+type+'"}';;
		}else{
			return '{"cmd": "get_omdd_tradable", "code":  "'+code+'", "algo_name":"'+key+'", "ref": "'+id+"-"+id2+"-"+type+'"}';
		}
	}else if(type=="position"){
		//return '{"type":"algo_command", "key":"'+key+'", "command":"position"}';
		return '{"cmd": "position", "id": '+user_id+', "algo_name":"'+key+'", "ref": "position"}';
	}else if(type=="orders"){
		//return '{"type":"list_orders"}';
		return '{"cmd": "order_list", "id": '+user_id+', "algo_name":"'+key+'", "max_display":30, "ref": "orders"}';
	}else if(type=="allorders"){
		return '{"cmd": "order_list", "id": '+user_id+', "algo_name":"'+key+'", "ref": "allorders"}';
	}else if(type=="getucode"){
		//return '{"type":"get_warrant_underlying", "warrant": '+code+', "ref": "'+id+'"}';
		return '{"cmd":"get_warrent_underlying", "code": '+code+', "ref": "'+id+'"}';
	}else if(type=="get"){
		return '{"cmd": "get", "id": '+user_id+', "algo_name":"'+key+'", "ref": "'+id+'"}';
	}else if(type=="power"){
		return '{"cmd": "get_buy_power", "id": '+user_id+', "algo_name":"'+key+'", "ref": "power"}';
	}else if(type=="top_power"){
		return '{"cmd": "get_top_buy_power", "id": '+user_id+', "algo_name":"'+key+'", "ref": "top_power"}';
	}
	
	//return '{"type":"algo_command", "key":"'+key+'", "command": "'+msg+'", "refid": "'+id+'", "reftype": "'+reftype+'"}';
	return '{"type":"algo_command", "key":"'+key+'", "command": "'+msg+'"}';

}

function initWebsocket(){
	if ("WebSocket" in window){
		if(!ws){
			ws = new WebSocket("ws://"+domain+"/uid="+localStorage.getItem('uid'));
		}

		ws.onopen = function(){
			if(connectCount>10){
				initLogout();
			}
			console.log("已連接！");
			retryCount = 0;
			connectCount++;
		}
		ws.onmessage = function (evt){//console.log(evt);
			var received_msg = evt.data;
			if(received_msg=="{}"){
				initLogout();
			}
			var today = new Date().toLocaleString();
			var data = $.parseJSON(received_msg);
			var receive_time = "";
			if(data.tm != undefined){
				receive_time= "("+new Date(data.tm).toLocaleString()+")";
			}
			if(data.user_id*1>0){
				user_id = data.user_id;
			}
			console.log(today+" Receive"+receive_time+": "+received_msg);
			if(data.algo_name != undefined && key != "" && key != data.algo_name && data.cmd!="get_top_buy_power"){
			}else{
				/*if(data.action!="log" && data.action!="handleOmdcTrade" && data.action!="handleOmddTrade" && data.action!="handleOmdcOrderBook" && data.action!="remind_buy" && data.action!="remind_sell" && data.action!="list_orders"){
					console.log(today+" Receive"+receive_time+": "+received_msg);
				}*/
				/*if(data.action=="algo_command"){
					printLog(today,receive_time,received_msg);
					var command = $.parseJSON(data.command);
					var commandstr = command.command;
					var commandArr = commandstr.split("|");
					var action2 = commandArr[0];
					if(action2=="set"){
						var type = commandArr[11];
						var id = commandArr[12];
						if(id != undefined && id != ""){
							if(data.result == "success"){
								var idArr = id.split("_");
								
								$("#"+idArr[0]+"ucode").val(commandArr[2]);
								$("#"+id+"code").val(commandArr[3]);
								
								if(commandArr[4]*1>0)
									$("#"+id+"buy_ulast").val(commandArr[4]/factor);
								
								if(commandArr[5]*1>0)
									$("#"+id+"sell_ulast").val(commandArr[5]/factor);
								
								if(commandArr[6]*1>0)
									$("#"+id+"buy_last").val(commandArr[6]/factor);
								
								if(commandArr[7]*1>0)
									$("#"+id+"sell_last").val(commandArr[7]/factor);
								
								if(commandArr[8]*1>0)
									$("#"+id+"bottom").val(commandArr[8]/factor);
								
								if(commandArr[9]*1>0)
									$("#"+id+"ceiling").val(commandArr[9]/factor);
								
								if(type=="BUY"){
									$("#"+id+"vol").val(formatValue(commandArr[10]/factor));
									
									//setBuy(id);
									if($("#"+id+"force_buy").hasClass("off")){
										sendWsMsg(getWsMsg("forcebuy", id));
									}else{
										$("#"+id+"monbuy").removeClass("disable");
										$("#"+id+"force_buy").removeClass("disable off");
										$("#"+id+"monbuy").addClass("off");
									}
								}else if(type=="SELL"){
									$("#"+id+"t_vol_i").val(formatValue(commandArr[10]/factor));
								}
							}else{
								//stopBuy(id);
							}
						}
					}else if(action2=="force_buy"){
						var id = commandArr[4];
						if(data.result == "success"){
							$("#"+id+"monbuy").removeClass("off");
							$("#"+id+"force_buy").addClass("off");
						}else{
							
						}
					}else if(action2=="delete"){
						var id = commandArr[3];
						if(data.result == "success"){
							$("#"+id+"monbuy").removeClass("off");
							$("#"+id+"force_buy").removeClass("off");
						}else{
							var idArr = id.split("_");
								
							$("#"+idArr[0]+"ucode").val(data.underlying_code);
							$("#"+id+"code").val(data.warrant_code);
						}
					}
				}else */if(data.action=="load" && data.result == "success"){
					//{"action":"load","id":2,"key":"wendy_init_algo","name":"CppAlgo","param":"semi","recovery":true,"result":"success","type":"algo","tm":1566961141017} 
					if(data.key.indexOf("_init_algo")>0){
						key = data.key;
					}
				
				/*}else if(data.action=="command delete pair"){
					printLog(today,receive_time,received_msg);
					//{"action":"command delete pair","auto_buy":true,"auto_buy_quantity":0,"auto_sell":true,"bottom_price":3700000,"buy_price":4200000,"buy_trriger":23440000000,"ceiling_price":9200000,"id":1,"is_bull":true,"key":"leo_init_algo","position":0,"quantity":0,"recovery":true,"ref":"u002_1","result":"SUCCESS","sell_price":4100000,"sell_trriger":23420000000,"type":"algo semi","underlying_code":388,"warrant_code":15702,"tm":1565680830176}
					
					var id = data.ref;
					
					var type="";
					if(data.auto_buy == true && data.auto_sell == true){
						type = "auto";
					}else if(data.auto_buy == true ){
						type = "buy";
					}else if(data.auto_sell == true ){
						type = "sell";
					}
					
					if(type=="sell"){
						stopSell(id);
					}else if(type=="buy"){
						stopBuy(id);
					}else{
						stopAutoBuy(id);
					}*/
				//}else if(data.action=="command set pair"){
					//{"action":"command set pair","auto_buy":true,"auto_sell":false,"bottom_price":0,"buy_price":30000000,"buy_trriger":34220000000,"ceiling_price":0,"id":1,"is_bull":true,"key":"leo_init_algo","quantity":1000000000000,"ref":"u000_1","result":"SUCCESS","sell_price":0,"sell_trriger":0,"type":"algo semi","underlying_code":700,"warrant_code":19650,"tm":1565332140227}
				}else if(data.msg_type=="semi_algo_set"){
					//stop: {"algo_name":"leo_semi","id":1,"msg_type":"semi_algo_set","no_change":false,"pair":{"auto_buy":false,"auto_buy_id":0,"auto_buy_quantity":1000000000000,"auto_sell":false,"auto_sell_id":0,"bottom_price":10100000,"buy_price":10400000,"buy_trriger":41300000000,"early_buy_qty":0,"early_sell_qty":0,"is_bull":true,"is_buying":false,"is_selling":false,"position":0,"ref":"","sell_price":10400000,"sell_trriger":41300000000,"underlying_code":700,"underlying_symbol":"","warrant_code":29707},"ref":"u000_1","result":"SUCCESS","tm":1590388570720}}
					
					//buy: {"algo_name":"leo_semi","id":1,"msg_type":"semi_algo_set","no_change":false,"pair":{"auto_buy":true,"auto_buy_id":0,"auto_buy_quantity":1000000000000,"auto_sell":false,"auto_sell_id":0,"bottom_price":10100000,"buy_price":10400000,"buy_trriger":41320000000,"early_buy_qty":0,"early_sell_qty":0,"is_bull":true,"is_buying":false,"is_selling":false,"position":0,"ref":"","sell_price":10400000,"sell_trriger":41320000000,"underlying_code":700,"underlying_symbol":"","warrant_code":29707},"ref":"u000_1","result":"SUCCESS","tm":1590388633128}
					
					//auto: {"algo_name":"leo_semi","id":1,"msg_type":"semi_algo_set","no_change":false,"pair":{"auto_buy":true,"auto_buy_id":0,"auto_buy_quantity":1000000000000,"auto_sell":true,"auto_sell_id":0,"bottom_price":16500000,"buy_price":16800000,"buy_trriger":27460000000,"ceiling_price":16900000,"early_buy_qty":0,"early_sell_qty":0,"is_bull":true,"is_buying":false,"is_selling":false,"position":0,"ratio_buy":0,"ratio_sell":0,"ref":"u004_1","sell_price":17100000,"sell_trriger":27480000000,"underlying_code":388,"underlying_symbol":"","warrant_code":14841},"ref":"u004_1","result":"SUCCESS","tm":1590544452104} 
					
					//sell: {"algo_name":"leo_semi","id":1,"msg_type":"semi_algo_set","no_change":false,"pair":{"auto_buy":false,"auto_buy_id":0,"auto_buy_quantity":1000000000000,"auto_sell":true,"auto_sell_id":0,"bottom_price":12900000,"buy_price":13200000,"buy_trriger":42340000000,"ceiling_price":13300000,"early_buy_qty":0,"early_sell_qty":0,"is_bull":true,"is_buying":false,"is_selling":false,"position":0,"ratio_buy":0,"ratio_sell":0,"ref":"u000_1","sell_price":13300000,"sell_trriger":42360000000,"underlying_code":700,"underlying_symbol":"","warrant_code":29707},"ref":"u000_1","result":"SUCCESS","tm":1590469389450}
					
					//nochange: {"algo_name":"leo_semi","id":1,"msg_type":"semi_algo_set","no_change":true,"pair":{"auto_buy":false,"auto_buy_id":0,"auto_buy_quantity":1000000000000,"auto_sell":false,"auto_sell_id":0,"bottom_price":9800000,"buy_price":10100000,"buy_trriger":41140000000,"ceiling_price":10200000,"early_buy_qty":0,"early_sell_qty":0,"is_bull":false,"is_buying":false,"is_selling":false,"position":0,"ratio_buy":0,"ratio_sell":0,"ref":"u022_1","sell_price":10100000,"sell_trriger":41180000000,"underlying_code":700,"underlying_symbol":"","warrant_code":26399},"ref":"u022_1","result":"SUCCESS","tm":1590721663132}
					
					printLog(today,receive_time,received_msg);
					
					var id = data.ref;
					var stop_sell = false;
					if(id.indexOf("_stopsell")>0){
						id = id.replace("_stopsell","");
						stop_sell = true;
					}
					
					var type="";
					if(data.no_change == true){
						type = "update";
					}else if(data.pair.auto_buy == true && data.pair.auto_sell == true){
						type = "auto";
					}else if(data.pair.auto_buy == true ){
						type = "buy";
					}else if(data.pair.auto_sell == true ){
						type = "sell";
					}else{
						type = "stop";
					}
						
					if(data.result == "SUCCESS"){
						var idArr = id.split("_");
						var ucode = "";
						if(data.pair.underlying_symbol != ""){
							ucode = data.pair.underlying_symbol;
						}else{
							ucode = data.pair.underlying_code;
						}
						$("#"+idArr[0]+"ucode").val(ucode);
						$("#"+id+"code").val(data.pair.warrant_code);
					
						updateFactor(ucode);
								
						if(data.pair.buy_trriger*1>0){
							$("#"+id+"buy_ulast").val(data.pair.buy_trriger/factor);
						}
						
						if(data.pair.sell_trriger*1>0){
							$("#"+id+"sell_ulast").val(data.pair.sell_trriger/factor);
						}
						
						if(data.pair.buy_price*1>0){
							$("#"+id+"buy_last").val(data.pair.buy_price/factor_stock);
						}
						
						if(data.pair.sell_price*1>0){
							$("#"+id+"sell_last").val(data.pair.sell_price/factor_stock);
						}
						
						if(data.pair.bottom_price*1>0){
							$("#"+id+"bottom").val(data.pair.bottom_price/factor_stock);
						}
						
						if(data.pair.ceiling_price*1>0){
							$("#"+id+"ceiling").val(data.pair.ceiling_price/factor_stock);
						}
						
						$("#"+id+"buy_status").removeClass("bg_red");
						$("#"+id+"buy_status").html("");
						$("#"+id+"sell_status").removeClass("bg_red bg_orange bg_yellow");
						$("#"+id+"sell_status").html("");
						
						if(type!="update"){
							if(type=="auto"){
								$("#"+id+"monbuy").removeClass("off").addClass("disable");
							}else{
								$("#"+id+"as").removeClass("off").addClass("disable");
							}
							
							if(type=="buy" || type=="auto"){
								if(data.pair.auto_buy_quantity*1>0){
									$("#"+id+"vol").val(formatValue(data.pair.auto_buy_quantity/factor_stock));
								}
								$("#"+id+"buy_vol").val(formatValue(data.pair.early_buy_qty));
								$("#"+id+"sell_vol").val(formatValue(data.pair.early_sell_qty));
								
								if($("#"+id+"force_buy").hasClass("off")){
									sendWsMsg(getWsMsg("forcebuy", id));
								}else{
									if(type=="auto"){
										$("#"+id+"monbuy").removeClass("off").addClass("disable");
										$("#"+id+"as").removeClass("disable").addClass("off");
									}else{
										$("#"+id+"as").removeClass("off").addClass("disable");
										$("#"+id+"monbuy").removeClass("disable").addClass("off");
									}
							
									$("#"+id+"force_buy").removeClass("disable off");
								}
								$("#"+id+"buy_status").addClass("bg_green");
								$("#"+id+"buy_status").html("");
								$("#"+id+"ismon").val(1);
							}else if(type=="sell"){
								if(data.pair.auto_buy_quantity*1>0){
									$("#"+id+"t_vol_i").val(formatValue(data.pair.auto_buy_quantity/factor_stock));
									$("#"+id+"t_vol").val(data.pair.auto_buy_quantity/factor_stock);
								}
								
								if($("#"+id+"force_sell").hasClass("off")){
									sendWsMsg(getWsMsg("forcesell", id));
								}else{
									$("#"+id+"monsell").removeClass("disable");
									$("#"+id+"force_sell").removeClass("disable off");
									$("#"+id+"monsell").addClass("off");
									
									if($("#"+id+"click_sell").val()==1){
										sendWsMsg(getWsMsg("getbid_checkforcesell", id));
									}
								}
								$("#"+id+"sell_status").addClass("bg_green");
								$("#"+id+"sell_status").html("");
								$("#"+id+"ismon").val(2);
							}else if(type=="stop"){
								if(stop_sell || $("#"+id+"ismon").val()==2 || $("#"+id+"t_vol").val()*1>0){
									$("#"+id+"t_vol_i").val(formatValue(data.pair.position/factor_stock));
									$("#"+id+"t_vol").val(data.pair.position/factor_stock);
									$("#"+id+"monsell").removeClass("disable off");
									$("#"+id+"force_sell").removeClass("disable off");
									$("#"+id+"sell_status").removeClass("bg_green").addClass("bg_orange");
								}else{
									$("#"+id+"vol").val(formatValue(data.pair.auto_buy_quantity/factor_stock));
									$("#"+id+"monbuy").removeClass("disable off");
									$("#"+id+"force_buy").removeClass("disable off");
									$("#"+id+"buy_status").removeClass("bg_green");
									asBtn(id);
								}
								$("#"+id+"ismon").val(0);
								
								//$("#"+id+"init").val(1);
								$("#"+idArr[0]+"ucode").removeAttr('readonly');
								$("#"+id+"code").removeAttr('readonly');
							}
							if(type!="stop"){
								$("#"+idArr[0]+"ucode").attr('readonly','readonly');
								$("#"+id+"code").attr('readonly','readonly');
							}
							$("#"+id+"remove_btn").hide();
						}
					}else{
						if(type=="buy" || type=="auto"){
							stopBuy(id,"");
						}
						if(type=="sell" || type=="auto"){
							stopSell(id);
						}
					}
				//}else if(data.action=="on_order"){				 //{"action":"on_order","auto_buy":false,"auto_sell":false,"bottom_price":0,"buy_price":30000000,"buy_trriger":34220000000,"ceiling_price":0,"id":1,"is_bull":true,"key":"leo_init_algo","order":{"aon":"aon_type::non_ano","broker_id":"9712","code":19650,"exectuion_id":"","filled_quantity":1000000000000,"header":{"api_id":"api_id_flag::new_order","first_ref":0,"flag":"api_type_flag::single_reply","len":254,"order_id":1,"ref_data":"0x0000000000000100000000000000","session_id":"0x316747327A577335"},"match_price":30000000,"match_quantity":1000000000000,"match_records":[{"match_price":30000000,"match_quantity":1000000000000}],"order_ref":57,"price":30000000,"quantity":1000000000000,"reject_reason":"","remain_quantity":0,"rep_type":"report_type::order_fill","report_id":58,"side":"order_side::buy","status":"order_status::filled","transaction_tm":"20190809143201455","type":"order_type::sl"},"quantity":1000000000000,"recovery":true,"ref":"u000_1","result":"SUCCESS","sell_price":0,"sell_trriger":0,"type":"algo semi","underlying_code":700,"warrant_code":19650,"tm":1565332321455}
					//{"action":"on_order","auto_buy":false,"auto_buy_quantity":1000000000000,"auto_sell":false,"bottom_price":67000000,"buy_price":72000000,"buy_trriger":33620000000,"ceiling_price":122000000,"id":1,"is_bull":true,"key":"leo_init_algo","order":{"aon":"aon_type::non_ano","broker_id":"9712","code":14339,"exectuion_id":"","filled_quantity":1000000000000,"header":{"api_id":"api_id_flag::new_order","first_ref":0,"flag":"api_type_flag::single_reply","len":254,"order_id":3,"ref_data":"0x0000000000000300000000000000","session_id":"0x55344D47764E4E51"},"match_price":71000000,"match_quantity":1000000000000,"match_records":[{"match_price":71000000,"match_quantity":1000000000000}],"order_ref":87,"price":71000000,"quantity":1000000000000,"reject_reason":"","remain_quantity":0,"rep_type":"report_type::order_fill","report_id":88,"side":"order_side::sell","status":"order_status::filled","transaction_tm":"20190813132033472","type":"order_type::sl"},"position":0,"quantity":1000000000000,"recovery":true,"ref":"u000_1","result":"SUCCESS","sell_price":71000000,"sell_trriger":33680000000,"type":"algo semi","underlying_code":700,"warrant_code":14339,"tm":1565673633473}
				}else if(data.msg_type=="semi_algo_odr_msg"){
					printLog(today,receive_time,received_msg);	
					
					//buy: {"algo_name":"leo_semi","id":1,"msg_type":"semi_algo_odr_msg","odr":{"aon":"aon_type::non_ano","broker_id":"9712","code":58725,"exectuion_id":"","filled_quantity":1000000000000,"header":{"api_id":"api_id_flag::new_order","first_ref":0,"flag":"api_type_flag::single_reply","len":254,"order_id":1,"ref_data":"0x0000000000000100000000000000","session_id":"0x6C7A7764334A6435"},"match_price":8600000,"match_quantity":1000000000000,"match_records":[{"match_price":8600000,"match_quantity":1000000000000}],"order_ref":1,"ori_price":0,"ori_quantity":0,"price":8600000,"quantity":1000000000000,"reject_reason":"","remain_quantity":0,"rep_type":"report_type::order_fill","report_id":2,"side":"order_side::buy","status":"order_status::filled","transaction_tm":"20200527093428830","type":"order_type::sl"},"recovery":true,"ref":"u000_1","tm":1590543268831}
					
					//sell: {"algo_name":"leo_semi","auto_buy":false,"auto_buy_qty":3000000000000,"auto_sell":false,"buy_price":12800000,"buy_trigger":21200000000,"filled_price":12500000,"filled_quantity":3000000000000,"id":1,"last_price":12500000,"last_trigger_price":21200000000,"msg_type":"semi_algo_odr_msg","odr":{"aon":"aon_type::non_ano","broker_id":"9712","code":26901,"exectuion_id":"","filled_quantity":3000000000000,"header":{"api_id":"api_id_flag::new_order","first_ref":0,"flag":"api_type_flag::single_reply","len":254,"order_id":107,"ref_data":"0x0000000000006B00000000000000","session_id":"0x325545665864465A"},"match_price":12500000,"match_quantity":3000000000000,"match_records":[{"match_price":12500000,"match_quantity":3000000000000}],"order_ref":187,"ori_price":0,"ori_quantity":0,"price":12500000,"quantity":3000000000000,"reject_reason":"","remain_quantity":0,"rep_type":"report_type::order_fill","report_id":188,"side":"order_side::sell","status":"order_status::filled","transaction_tm":"20200630105639273","type":"order_type::sl"},"position":0,"reason":"","ref":"u007_1","side":"SELL","status":"filled","tm":1593485799274}
					
					//auto: {"algo_name":"leo_semi","auto_buy":false,"auto_buy_qty":3000000000000,"auto_sell":true,"buy_price":12800000,"buy_trigger":21200000000,"filled_price":12800000,"filled_quantity":3000000000000,"id":1,"last_price":12800000,"last_trigger_price":21200000000,"msg_type":"semi_algo_odr_msg","odr":{"aon":"aon_type::non_ano","broker_id":"9712","code":26901,"exectuion_id":"","filled_quantity":3000000000000,"header":{"api_id":"api_id_flag::new_order","first_ref":0,"flag":"api_type_flag::single_reply","len":254,"order_id":103,"ref_data":"0x0000000000006700000000000000","session_id":"0x325545665864465A"},"match_price":12800000,"match_quantity":3000000000000,"match_records":[{"match_price":12800000,"match_quantity":3000000000000}],"order_ref":179,"ori_price":0,"ori_quantity":0,"price":12800000,"quantity":3000000000000,"reject_reason":"","remain_quantity":0,"rep_type":"report_type::order_fill","report_id":180,"side":"order_side::buy","status":"order_status::filled","transaction_tm":"20200630105638284","type":"order_type::sl"},"position":3000000000000,"reason":"","ref":"u007_1","side":"BUY","status":"filled","tm":1593485798285}
					
					//cancel: {"algo_name":"wendy_semi","auto_buy":false,"auto_sell":false,"buy_price":6600000,"buy_trigger":2287600000,"id":2,"msg_type":"semi_algo_odr_msg","odr":{"aon":"aon_type::non_ano","broker_id":"","code":0,"exectuion_id":"","filled_quantity":0,"header":{"api_id":"api_id_flag::modify_order","first_ref":0,"flag":"api_type_flag::single_reply","len":254,"order_id":0,"ref_data":"0x0000000000000000000000000000","session_id":"0x0000000000000000"},"match_price":0,"match_quantity":0,"match_records":[],"order_ref":0,"ori_price":0,"ori_quantity":0,"price":0,"quantity":0,"reject_reason":"Exceed Buy Power","remain_quantity":0,"rep_type":"report_type::order_report","report_id":0,"side":"order_side::buy","status":"order_status::canceled","transaction_tm":"","type":"order_type::pl"},"recovery":true,"ref":"u002_1","tm":1590658240691}
					
					//reject: {"algo_name":"wendy_semi","auto_buy":true,"auto_sell":false,"buy_price":17600000,"buy_trigger":45000000000,"id":3,"msg_type":"semi_algo_odr_msg","odr":{"aon":"aon_type::non_ano","broker_id":"9712","code":29707,"exectuion_id":"","filled_quantity":0,"header":{"api_id":"api_id_flag::new_order","first_ref":0,"flag":"api_type_flag::single_reply","len":254,"order_id":166,"ref_data":"0x000000000000A600000000000000","session_id":"0x454B306146766E6B"},"match_price":0,"match_quantity":0,"match_records":[],"order_ref":161,"ori_price":0,"ori_quantity":0,"price":17700000,"quantity":18446741073709552000,"reject_reason":"Invalid order price","remain_quantity":0,"rep_type":"report_type::order_report","report_id":162,"side":"order_side::sell","status":"order_status::rejected","transaction_tm":"20200618105551848","type":"order_type::sl"},"recovery":true,"ref":"u002_1","tm":1592448951849} {"algo_name":"wendy_semi","auto_buy":true,"auto_sell":false,"buy_price":17600000,"buy_trigger":45000000000,"id":3,"msg_type":"semi_algo_odr_msg","odr":{"aon":"aon_type::non_ano","broker_id":"9712","code":29707,"exectuion_id":"","filled_quantity":0,"header":{"api_id":"api_id_flag::new_order","first_ref":0,"flag":"api_type_flag::single_reply","len":254,"order_id":163,"ref_data":"0x000000000000A300000000000000","session_id":"0x454B306146766E6B"},"match_price":0,"match_quantity":0,"match_records":[],"order_ref":155,"ori_price":0,"ori_quantity":0,"price":17700000,"quantity":3000000000000,"reject_reason":"Invalid order price","remain_quantity":0,"rep_type":"report_type::order_report","report_id":156,"side":"order_side::sell","status":"order_status::rejected","transaction_tm":"20200618105544561","type":"order_type::sl"},"recovery":true,"ref":"u002_1","tm":1592448951849}
					
					var id = data.ref;
					var idArr = id.split("_");
					var status = "";
					
					/*if(data.odr.status.indexOf("partial_fill")>1){
						status = "partial";
					}else if(data.odr.status.indexOf("filled")>1){
						status = "filled";
					}else if(data.odr.status.indexOf("reject")>1){
						status = "reject";
					}else if(data.odr.status.indexOf("cancel")>1){
						status = "cancel";
					}else if(data.odr.status.indexOf("queued")>1){
						status = "queued";
					}*/
					
					if(data.status == "partial"){
						status = "partial";
					}else if(data.status == "filled"){
						status = "filled";
					}else if(data.status == "reject"){
						status = "reject";
					}else if(data.status == "cancel"){
						status = "cancel";
					}else if(data.status == "queued"){
						status = "queued";
					}
					
					var type="";
					/*if(data.odr.side.indexOf("buy")>1){
						type = "buy";
					}else if(data.odr.side.indexOf("sell")>1){
						type = "sell";
					}*/
					if(data.side == "BUY"){
						type = "buy";
					}else if(data.side == "SELL"){
						type = "sell";
					}
					if(status != "partial" && status != "queued"){
						//if(data.result == "SUCCESS" && status == "filled"){
						if(isRecovery){
							$("#"+id+"rc_action").val(type);
						}
						if(status == "filled"){
							$("#"+id+"ismon").val(0);
							
							/*var price = data.odr.match_price;
							if(price == 0){
								price = data.odr.match_records[0].match_price;
							}*/
							if(!isRecovery){
								var ucode = $("#"+idArr[0]+"ucode").val();
								updateFactor(ucode);
							}
							
							$("#"+id+"ulast").text(data.last_trigger_price/factor);
							$("#"+id+"last").text((data.last_price/factor_stock).toFixed(3)*1);
							if(type=="buy"){
								if(!isRecovery){
									$("#audio-beep")[0].play();
									
									//var ucode = $("#"+idArr[0]+"ucode").val();
									//updateFactor(ucode);
									
									//$("#"+id+"ulast").text(data.buy_trigger/factor);
								}
								
								//$("#"+id+"last").text((price/factor_stock).toFixed(3)*1);
								
								/*$("#"+id+"vol").val(formatValue(data.odr.quantity/factor_stock));
								$("#"+id+"t_vol").val($("#"+id+"t_vol").val()*1+(data.odr.filled_quantity/factor_stock));
								$("#"+id+"t_vol_i").val(formatValue($("#"+id+"t_vol").val()*1));*/
								$("#"+id+"vol").val(formatValue(data.auto_buy_qty/factor_stock));
								$("#"+id+"t_vol").val(data.position/factor_stock);
								$("#"+id+"t_vol_i").val(formatValue($("#"+id+"t_vol").val()*1));
								
								//$("#"+id+"buy_vol").val(formatValue(data.early_buy_qty));
								//$("#"+id+"sell_vol").val(formatValue(data.early_sell_qty));
								$("#"+id+"monbuy").removeClass("off").addClass("disable");
								$("#"+id+"force_buy").removeClass("off").addClass("disable");
								
								$("#"+id+"buy_status").removeClass("bg_green bg_red");
								$("#"+id+"buy_status").html("");
								
								if(data.auto_sell == true){
									$("#"+id+"monsell").addClass("off").removeClass("disable");
									$("#"+id+"force_sell").removeClass("disable");
									$("#"+id+"as").removeClass("off").addClass("disable");
									$("#"+id+"sell_status").addClass("bg_green");
									$("#"+id+"ismon").val(2);
								}else{
									setSell(id);
									$("#"+id+"sell_status").addClass("bg_orange");
								}
								
								$("#"+id+"buy_status").removeClass("bg_green");
								//maxAmmount -= (price/factor_stock)*(data.odr.filled_quantity/factor_stock);
							}else if(type=="sell"){
								//$("#"+id+"ulast").text(data.sell_trriger/factor);
								//$("#"+id+"last").text((price/factor_stock).toFixed(3)*1);
								
								/*$("#"+id+"t_vol").val((data.odr.quantity-data.odr.filled_quantity)/factor_stock);
								$("#"+id+"t_vol_i").val(formatValue((data.odr.quantity-data.odr.filled_quantity)/factor_stock));*/
								$("#"+id+"t_vol").val(data.position/factor_stock);
								$("#"+id+"t_vol_i").val(formatValue(data.position/factor_stock));
								
								$("#"+id+"sell_status").removeClass("bg_orange bg_green bg_red bg_yellow");
								$("#"+id+"sell_status").html("");
								$("#"+id+"init").val(1);
								$("#"+id+"monsell").removeClass("off");
								$("#"+id+"force_sell").removeClass("off");
								if($("#"+id+"t_vol").val()==0){
									$("#"+id+"monsell").addClass("disable");
									$("#"+id+"force_sell").addClass("disable");
									setBuy(id);
								}else{
									$("#"+id+"ismon").val(2);
									$("#"+id+"monsell").addClass("off");
									$("#"+id+"sell_status").addClass("bg_yellow");
								}
								
								//maxAmmount += (price/factor_stock)*(data.odr.filled_quantity/factor_stock);
							}
						}else{
							if(status != ""){
								if(data.auto_sell == true && type=="sell"){
								}else{
									$("#"+id+"ismon").val(0);
									if(type=="sell"){
										stopSell(id);
									}else if(type=="buy"){
										if(data.auto_sell == true){
											stopAutoBuy(id, "2");
										}else{
											stopBuy(id,"2");
										}
									}
								}
								getWsStatus(id, data.reason, type, "bg_red", status);
							}
						}
					}
				//}else if(data.action=="get_warrant_underlying"){//{"action":"get_warrant_underlying","id":3,"ref":"u000_1","result":"success","type":"control","underlying":3690,"tm":1579225240128}
				}else if(data.cmd=="get_warrent_underlying"){//{"cmd":"get_warrent_underlying","code":29707,"id":1,"ref":"u000_1","underlying":700,"tm":1590375870089}
					var id = data.ref;
					var idArr = id.split("_");
					
					if(data.error == undefined){
						$("#"+id+"buy_status").removeClass("bg_red");
						$("#"+id+"buy_status").html("");
								
						var ucode = $("#"+idArr[0]+"ucode").val();
						if(isUnderlying(data.underlying)){
							if(ucode==""){
								$("#"+idArr[0]+"ucode").val(data.underlying);
							}else if((ucode*1>0 && ucode*1!=data.underlying*1) || (ucode*1==0 && ucode!=data.underlying)){
								getWsStatus(id, "Underlying/Warrant Pair Not Matched", "buy", "bg_red");
							}
						}
					}else{
						getWsStatus(id, "Invalid Warrant Code", "buy", "bg_red");
					}
				}else if(data.type=="recovery_end"){
					printLog(today,receive_time,received_msg);
					isRecovery = false;
					//sendWsMsg(getWsMsg("power", ""));
					loadHis();
				//}else if(data.action=="get_omdc_tradable" || data.action=="get_omdd_tradable" || data.action=="get_omdc_bestprice" || data.action=="get_omdd_bestprice"){
					//{"action":"get_omdc_bestprice","id":3,"ref":"u000_1-u000_1buy_ulast-getbid","result":"success","tradable":{"best_ask":{"m_iPrice":398800,"m_uNumberOfOrder":18,"m_uQuantity":13600},"best_bid":{"m_iPrice":398600,"m_uNumberOfOrder":19,"m_uQuantity":12400},"m_iTradePrice":398600,"m_iTradeSide":1,"m_iTradeType":0,"m_uCode":700,"m_uTm":1583463487642000000,"m_uTradeTime":1583463487000000000,"trade_side":"B"},"type":"control","tm":1583463487651}
					//{"action":"get_omdd_bestprice","id":3,"ref":"u000_1-u000_1buy_ulast-getbid","result":"success","symbol":"HSIZ0","tradable":{"best_ask":{"m_iPrice":25635,"m_uNumberOfOrder":1,"m_uQuantity":2},"best_bid":{"m_iPrice":25343,"m_uNumberOfOrder":2,"m_uQuantity":2},"m_iTradePrice":0,"m_iTradeSide":0,"m_iTradeType":0,"m_uCode":6360994,"m_uTm":1583472604445000000,"m_uTradeTime":0,"trade_side":""},"type":"control","tm":1583472641999}
				}else if(data.cmd=="get_omdc_tradable" || data.cmd=="get_omdd_tradable"){
					//{"cmd":"get_omdc_tradable","code":700,"id":1,"omdc_tradable":{"m_AccumulateBlankQuantity":0,"m_AccumulateBuyQuantity":0,"m_AccumulateSellQuantity":0,"m_Ask":[{"m_iPrice":411400,"m_uNumberOfOrder":42,"m_uQuantity":21100}],"m_Bid":[{"m_iPrice":411200,"m_uNumberOfOrder":16,"m_uQuantity":3000}],"m_Code":700,"m_LastTradePrice":411200,"m_LastTradeQuantity":200,"m_MsgTime":1590377469504018200,"m_MsgType":"OMDC_BOOK","m_PkgTime":1590377469504000000,"m_TradeSide":"SELL_SIDE","m_TradeType":0},"ref":"u000_1-u000_1buy_ulast-getbid","tm":1590377469640}
					//{"cmd":"get_omdd_tradable","code":"HSIK0","id":1,"omdd_tradable":{"m_AccumulateBlankQuantity":0,"m_AccumulateBuyQuantity":0,"m_AccumulateSellQuantity":0,"m_Ask":[{"m_iPrice":23320,"m_uNumberOfOrder":4,"m_uQuantity":6}],"m_Bid":[{"m_iPrice":23318,"m_uNumberOfOrder":7,"m_uQuantity":9}],"m_Code":72683426,"m_LastTradePrice":23320,"m_LastTradeQuantity":1,"m_MsgTime":0,"m_MsgType":"OMDD_BOOK","m_PkgTime":1590458284185000000,"m_Symbol":72683426,"m_TradeSide":"BUY_SIDE","m_TradeType":1},"ref":"u000_1-u000_1buy_ulast-getbid","tm":1590458284191}
					printLog(today,receive_time,received_msg);
					if(data.error == undefined){
						var ref = data.ref;
						var refArr = ref.split("-");
						var id = refArr[0];
						var idArr = id.split("_");
						var id2 = refArr[1];
						var type = refArr[2];
						if(id2==id+"checkforcesell" ){
							if(type == "getbid"){
								var bid = 0;
								if(data.cmd=="get_omdd_tradable"){
									bid = data.omdd_tradable.m_Bid[0].m_iPrice*1;
								}else{
									bid = data.omdc_tradable.m_Bid[0].m_iPrice/1000;
								}
								if(bid>=$("#"+id+"ceiling").val()){
									sendWsMsg(getWsMsg("checkforcesell", id));
									$("#"+id+"click_sell").val(0);
								}
							}
						}else{
							if(id2==id+"buy_last" || id2==id+"sell_last" || id2==id+"bottom" || id2==id+"ceiling"){
								$("#"+id+"code").val(data.code);
							}else{
								if(data.cmd=="get_omdd_tradable"){
									$("#"+idArr[0]+"ucode").val(data.code);
								}else{
									$("#"+idArr[0]+"ucode").val(data.code);
								}
							}
							var val = $("#"+id2).val();
							if(type == "getbid"){
								if(data.cmd=="get_omdd_tradable"){
									$("#"+id2).val(data.omdd_tradable.m_Bid[0].m_iPrice);
								}else{
									$("#"+id2).val(data.omdc_tradable.m_Bid[0].m_iPrice/1000);
								}
							}else if(type == "getask"){
								if(data.cmd=="get_omdd_tradable"){
									$("#"+id2).val(data.omdd_tradable.m_Ask[0].m_iPrice);
								}else{
									$("#"+id2).val(data.omdc_tradable.m_Ask[0].m_iPrice/1000);
								}
							}
							var val2 = $("#"+id2).val();
							
							if(val2>0){
								var sign=1;
								if(id2==id+"buy_last"){
									$("#"+id+"sell_last").val(getValBySpread(val2, $("#default_tick_out").val()*sign));
									if($("#default_tick_bottom").val()!=""){
										$("#"+id+"bottom").val(getValBySpread(val2, $("#default_tick_bottom").val()*sign));
									}else{
										$("#"+id+"bottom").val(0);
									}
									$("#"+id+"ceiling").val(getValBySpread(val2, $("#default_tick_ceiling").val()*sign));
									if(val*1!=val2*1){
										if($("#"+id+"as").hasClass("off") || $("#"+id+"t_vol").val()==0){
											setBuy(id);
										}else{
											setSell(id);
										}
									}
								}else if(id2==id+"buy_ulast"){
									$("#"+id+"sell_ulast").val(getValBySpread(val2, $("#default_sell_ulast").val()*sign));
									if(val*1!=val2*1){
										if($("#"+id+"as").hasClass("off") || $("#"+id+"t_vol").val()==0){
											setBuy(id);
										}else{
											setSell(id);
										}
									}
								}
							}
						}
					}else{
						
					}
					//{"action":"get_omdc_tradable","id":1,"ref":"u000_1-u000_1buy_ulast-getbid","result":"success","tradable":{"m_Ask":[{"m_iPrice":342400,"m_uNumberOfOrder":1,"m_uQuantity":4700},{"m_iPrice":342600,"m_uNumberOfOrder":4,"m_uQuantity":76200},{"m_iPrice":342800,"m_uNumberOfOrder":3,"m_uQuantity":29000},{"m_iPrice":343000,"m_uNumberOfOrder":12,"m_uQuantity":171600},{"m_iPrice":343200,"m_uNumberOfOrder":15,"m_uQuantity":122100},{"m_iPrice":343400,"m_uNumberOfOrder":12,"m_uQuantity":87900},{"m_iPrice":343600,"m_uNumberOfOrder":20,"m_uQuantity":157300},{"m_iPrice":343800,"m_uNumberOfOrder":19,"m_uQuantity":152300},{"m_iPrice":344000,"m_uNumberOfOrder":23,"m_uQuantity":94500},{"m_iPrice":344200,"m_uNumberOfOrder":18,"m_uQuantity":185400},{"m_iPrice":0,"m_uNumberOfOrder":0,"m_uQuantity":0}],"m_Bid":[{"m_iPrice":342200,"m_uNumberOfOrder":7,"m_uQuantity":15000},{"m_iPrice":342000,"m_uNumberOfOrder":26,"m_uQuantity":104800},{"m_iPrice":341800,"m_uNumberOfOrder":16,"m_uQuantity":49000},{"m_iPrice":341600,"m_uNumberOfOrder":25,"m_uQuantity":97600},{"m_iPrice":341400,"m_uNumberOfOrder":6,"m_uQuantity":59800},{"m_iPrice":341200,"m_uNumberOfOrder":12,"m_uQuantity":39200},{"m_iPrice":341000,"m_uNumberOfOrder":28,"m_uQuantity":58400},{"m_iPrice":340800,"m_uNumberOfOrder":10,"m_uQuantity":39500},{"m_iPrice":340600,"m_uNumberOfOrder":22,"m_uQuantity":65400},{"m_iPrice":340400,"m_uNumberOfOrder":8,"m_uQuantity":22600},{"m_iPrice":0,"m_uNumberOfOrder":0,"m_uQuantity":0}],"m_TradeRecord":[{"m_iPrice":342400,"m_uQuantity":100,"m_uSeq":13256350,"m_uTm":1565251742000000000},{"m_iPrice":342400,"m_uQuantity":900,"m_uSeq":13256351,"m_uTm":1565251742000000000},{"m_iPrice":342400,"m_uQuantity":1200,"m_uSeq":13256352,"m_uTm":1565251742000000000},{"m_iPrice":342400,"m_uQuantity":1800,"m_uSeq":13256353,"m_uTm":1565251742000000000},{"m_iPrice":342400,"m_uQuantity":3200,"m_uSeq":13256354,"m_uTm":1565251742000000000},{"m_iPrice":342400,"m_uQuantity":100,"m_uSeq":13256355,"m_uTm":1565251742000000000},{"m_iPrice":342400,"m_uQuantity":200,"m_uSeq":13256356,"m_uTm":1565251742000000000},{"m_iPrice":342400,"m_uQuantity":100,"m_uSeq":13256357,"m_uTm":1565251742000000000},{"m_iPrice":342400,"m_uQuantity":500,"m_uSeq":13256358,"m_uTm":1565251742000000000},{"m_iPrice":342400,"m_uQuantity":100,"m_uSeq":13256359,"m_uTm":1565251742000000000},{"m_iPrice":342400,"m_uQuantity":600,"m_uSeq":13256360,"m_uTm":1565251742000000000},{"m_iPrice":342400,"m_uQuantity":1200,"m_uSeq":13256361,"m_uTm":1565251742000000000},{"m_iPrice":342400,"m_uQuantity":9000,"m_uSeq":13256362,"m_uTm":1565251742000000000},{"m_iPrice":342400,"m_uQuantity":300,"m_uSeq":13256363,"m_uTm":1565251742000000000},{"m_iPrice":342400,"m_uQuantity":1000,"m_uSeq":13256344,"m_uTm":1565251742000000000},{"m_iPrice":342400,"m_uQuantity":100,"m_uSeq":13256345,"m_uTm":1565251742000000000},{"m_iPrice":342400,"m_uQuantity":100,"m_uSeq":13256346,"m_uTm":1565251742000000000},{"m_iPrice":342400,"m_uQuantity":100,"m_uSeq":13256347,"m_uTm":1565251742000000000},{"m_iPrice":342400,"m_uQuantity":600,"m_uSeq":13256348,"m_uTm":1565251742000000000},{"m_iPrice":342400,"m_uQuantity":100,"m_uSeq":13256349,"m_uTm":1565251742000000000}],"m_iTradePrice":342400,"m_iTradeSide":1,"m_iTradeType":103,"m_uCode":700,"m_uLastTradeIdx":25034,"m_uLotSize":100,"m_uOrderBookUpdateFlag":7516192767,"m_uOrderSeq":13256365,"m_uQueueIdx":0,"m_uSubscribeFlag":2305843009213694000,"m_uTm":1565251742478000000,"m_uTradeQuanty":300,"m_uTradeSeq":13256363,"m_uTradeTime":1565251742000000000,"trade_side":"B"},"type":"control","tm":1565253225385}
				//}else if(data.action=="position"){
					//{"action":"position","position":{"12240":0,"18654":0},"type":"algo semi","tm":1565837914025}
				}else if(data.msg_type=="semi_algo_odr_position"){
					//{"algo_name":"leo_semi","id":1,"msg_type":"semi_algo_odr_position","position":{"18016":0,"29707":0},"ref":"position","tm":1590477336309}
					var position = data.position;
					if(position!=null){
						$.each(position, function(index, value) {
							$("#position_table").append("<tr id='position_tr_"+index+"'><td class='code'>"+index+"</td><td class='vol'>"+formatValue(value/factor_stock)+"</td></tr>");
						});
					}
					if(maxAmmount>0){
						$("#portfolios_cash").html(addcomma(maxAmmount));
					}else{
						$("#portfolios_loan").html(addcomma(maxAmmount));
					}
				}else if(data.action=="list_orders" && data.command != undefined){
					//{"action":"list_orders","command":"{\"type\":\"list_orders\",\"id\":1}","id":1,"result":"success","type":"control","tm":1565840461799}	
					map={};
					journal_data = "";
					combination_data = "";
					combination_array = Array();
					combination_map = {};
				//}else if(data.action=="list_orders" && data.ordres != undefined){
					//{"action":"list_orders","id":1,"ordres":[{"aon":"aon_type::non_ano","broker_id":"9712","code":12240,"exectuion_id":"","filled_quantity":1000000000000,"header":{"api_id":"api_id_flag::new_order","first_ref":0,"flag":"api_type_flag::single_reply","len":254,"order_id":2,"ref_data":"0x0000000000000200000000000000","session_id":"0x376F345842576F74"},"key":"leo_init_algo","match_price":3200000,"match_quantity":1000000000000,"match_records":[{"match_price":3200000,"match_quantity":1000000000000}],"order_ref":157,"price":3200000,"quantity":1000000000000,"reject_reason":"","remain_quantity":0,"rep_type":"report_type::order_fill","report_id":158,"side":"order_side::sell","status":"order_status::filled","transaction_tm":"20190815113155434","type":"order_type::sl"},{"aon":"aon_type::non_ano","broker_id":"9712","code":12240,"exectuion_id":"","filled_quantity":1000000000000,"header":{"api_id":"api_id_flag::new_order","first_ref":0,"flag":"api_type_flag::single_reply","len":254,"order_id":1,"ref_data":"0x0000000000000100000000000000","session_id":"0x376F345842576F74"},"key":"leo_init_algo","match_price":4100000,"match_quantity":1000000000000,"match_records":[{"match_price":4100000,"match_quantity":1000000000000}],"order_ref":155,"price":4100000,"quantity":1000000000000,"reject_reason":"","remain_quantity":0,"rep_type":"report_type::order_fill","report_id":156,"side":"order_side::buy","status":"order_status::filled","transaction_tm":"20190815113153228","type":"order_type::sl"}],"result":"success","type":"orders","tm":1565840461799}
				}else if(data.ref=="allorders" && data.orders != undefined){
					map={};
					journal_data = "";
					combination_all_data = "";
					combination_all_array = Array();
					combination_all_map = {};
					
					var order = data.orders;
					var j=0;
					var combination_all_table_sum_num=0;
					var combination_all_table_profit_num=0;
					
					for(var i=0; i<order.length; i++){
						if(order[i].transaction_tm==""){
							continue;
						}
						var type = "";
						var buy = 0;
						var sell = 0;
						var buy_sum = 0;
						var sell_sum = 0;
						var buy_count = 0;
						var sell_count = 0;
						var profit = 0;
						var filled_quantity = order[i].filled_quantity/factor_stock;
						var quantity = order[i].quantity/factor_stock;
						var price = order[i].match_price/factor_stock;
						var sum = price*filled_quantity;
						var tradeStatus = "";
						if(order[i].status.indexOf("filled")>1){
							if(filled_quantity==quantity){
								tradeStatus = "全部成交";
							}else{
								tradeStatus = "部份成交";
							}
							if(order[i].side.indexOf("buy")>1){
								buy_count = 1;
							}else if(order[i].side.indexOf("sell")>1){
								sell_count = 1;
							}
							
							if(price == 0){
								price = order[i].match_records[0].match_price/factor_stock;
								sum = price*filled_quantity;
							}
						}else if(order[i].status.indexOf("reject")>1){
							tradeStatus = "拒絕";
							if(order[i].reject_reason!=""){
								tradeStatus += " ("+order[i].reject_reason+")";
							}
						}else if(order[i].status.indexOf("cancel")>1){
							tradeStatus = "取消";
						}
						if(order[i].side.indexOf("buy")>1){
							type = "買";
							buy = price;
							//buy_count = 1;
							buy_sum = sum;
							profit = sum*(-1);
						}else if(order[i].side.indexOf("sell")>1){
							type = "賣";
							sell = price;
							//sell_count = 1;
							sell_sum = sum;
							profit = sum;
						}
						
						var time = order[i].transaction_tm.substr(8, 2)+":"+order[i].transaction_tm.substr(10, 2)+":"+order[i].transaction_tm.substr(12, 2);
						var content = "<tr><td>"+time+"</td><td>"+tradeStatus+"</td><td>"+order[i].header.order_id+"</td><td>"+order[i].code+"</td><td>"+type+"</td><td>"+formatValue(quantity)+"</td><td>"+price+"</td><td>"+formatValue(filled_quantity)+"</td><td>"+order[i].order_ref+"</td></tr>";
						
						journal_data += time+","+tradeStatus+","+order[i].header.order_id+","+order[i].code+","+type+","+formatValue(quantity)+","+price+","+formatValue(filled_quantity)+","+order[i].order_ref+"\n";
						
						if(order[i].status.indexOf("filled")>1){
							if(map[order[i].code]!=1){
								map[order[i].code]=1;
								
								$("#combination_all_table").append("<tr id='combination_all_tr_"+order[i].code+"'><td class='code'>"+order[i].code+"<input type='hidden' class='buy_count' value='"+buy_count+"' /><input type='hidden' class='buy_avg_price' value='"+buy+"' /><input type='hidden' class='buy_total_price' value='"+buy_sum+"' /><input type='hidden' class='sell_count' value='"+sell_count+"' /><input type='hidden' class='sell_avg_price' value='"+sell+"' /><input type='hidden' class='sell_total_price' value='"+sell_sum+"' /><input type='hidden' class='total' value='"+sum+"' /><input type='hidden' class='profit' value='"+profit+"' /></td><td class='buy_avg_price'>"+buy+"</td><td class='buy_total_price'>"+addcomma(buy_sum.toFixed(2)*1)+"</td><td class='sell_avg_price'>"+sell+"</td><td class='sell_total_price'>"+addcomma(sell_sum.toFixed(2)*1)+"</td><td class='total'>"+addcomma(sum.toFixed(2)*1)+"</td><td class='profit'>"+formatProfit(profit.toFixed(2)*1)+"</td></tr>");
								
								combination_all_array[j] = order[i].code+","+buy+","+(buy_sum.toFixed(2)*1)+","+sell+","+(sell_sum.toFixed(2)*1)+","+(sum.toFixed(2)*1)+","+(profit.toFixed(2)*1)+"\n";
								combination_all_map[order[i].code]=j;
								j++;
							}else{
								$("#combination_all_tr_"+order[i].code+" input.total").val($("#combination_all_tr_"+order[i].code+" input.total").val()*1+sum);
								$("#combination_all_tr_"+order[i].code+" td.total").html(addcomma(($("#combination_all_tr_"+order[i].code+" input.total").val()*1).toFixed(2)*1));
								$("#combination_all_tr_"+order[i].code+" input.profit").val($("#combination_all_tr_"+order[i].code+" input.profit").val()*1+profit);
								$("#combination_all_tr_"+order[i].code+" td.profit").html(formatProfit(($("#combination_all_tr_"+order[i].code+" input.profit").val()*1).toFixed(2)*1));
								$("#combination_all_tr_"+order[i].code+" input.buy_count").val($("#combination_all_tr_"+order[i].code+" input.buy_count").val()*1+buy_count);
								$("#combination_all_tr_"+order[i].code+" input.buy_total_price").val($("#combination_all_tr_"+order[i].code+" input.buy_total_price").val()*1+buy_sum);
								$("#combination_all_tr_"+order[i].code+" td.buy_total_price").html(addcomma(($("#combination_all_tr_"+order[i].code+" input.buy_total_price").val()*1).toFixed(2)*1));
								$("#combination_all_tr_"+order[i].code+" input.buy_avg_price").val($("#combination_all_tr_"+order[i].code+" input.buy_avg_price").val()*1+buy);
								if($("#combination_all_tr_"+order[i].code+" input.buy_count").val()*1>0){
									$("#combination_all_tr_"+order[i].code+" td.buy_avg_price").html(addcomma((($("#combination_all_tr_"+order[i].code+" input.buy_avg_price").val()*1)/$("#combination_all_tr_"+order[i].code+" input.buy_count").val()*1).toFixed(4)*1));
								}
								$("#combination_all_tr_"+order[i].code+" input.sell_count").val($("#combination_all_tr_"+order[i].code+" input.sell_count").val()*1+sell_count);
								$("#combination_all_tr_"+order[i].code+" input.sell_total_price").val($("#combination_all_tr_"+order[i].code+" input.sell_total_price").val()*1+sell_sum);
								$("#combination_all_tr_"+order[i].code+" td.sell_total_price").html(addcomma(($("#combination_all_tr_"+order[i].code+" input.sell_total_price").val()*1).toFixed(2)*1));
								$("#combination_all_tr_"+order[i].code+" input.sell_avg_price").val($("#combination_all_tr_"+order[i].code+" input.sell_avg_price").val()*1+sell);
								if($("#combination_all_tr_"+order[i].code+" input.sell_count").val()*1>0){
									$("#combination_all_tr_"+order[i].code+" td.sell_avg_price").html(addcomma((($("#combination_all_tr_"+order[i].code+" input.sell_avg_price").val()*1)/$("#combination_all_tr_"+order[i].code+" input.sell_count").val()*1).toFixed(4)*1));
								}
								combination_all_array[combination_all_map[order[i].code]] = order[i].code+",";
								
								if($("#combination_all_tr_"+order[i].code+" input.buy_count").val()*1>0){
									combination_all_array[combination_all_map[order[i].code]] += ((($("#combination_all_tr_"+order[i].code+" input.buy_avg_price").val()*1)/$("#combination_all_tr_"+order[i].code+" input.buy_count").val()*1).toFixed(4)*1);
								}else{
									combination_all_array[combination_all_map[order[i].code]] += "0";
								}
								
								combination_all_array[combination_all_map[order[i].code]] += ","+(($("#combination_all_tr_"+order[i].code+" input.buy_total_price").val()*1).toFixed(2)*1)+",";
								
								if($("#combination_all_tr_"+order[i].code+" input.sell_count").val()*1>0){
									combination_all_array[combination_all_map[order[i].code]] += ((($("#combination_all_tr_"+order[i].code+" input.sell_avg_price").val()*1)/$("#combination_all_tr_"+order[i].code+" input.sell_count").val()*1).toFixed(4)*1);
								}else{
									combination_all_array[combination_all_map[order[i].code]] += "0";
								}
								
								combination_all_array[combination_all_map[order[i].code]] += ","+(($("#combination_all_tr_"+order[i].code+" input.sell_total_price").val()*1).toFixed(2)*1)+","+(($("#combination_all_tr_"+order[i].code+" input.total").val()*1).toFixed(2)*1)+","+(($("#combination_all_tr_"+order[i].code+" input.profit").val()*1).toFixed(2)*1)+"\n";
								
							}
							combination_all_table_sum_num = combination_all_table_sum_num*1+sum;
							combination_all_table_profit_num = combination_all_table_profit_num*1+profit;
						}
					}
					
					for(var i=0; i<combination_all_array.length; i++){
						combination_all_data += combination_all_array[i];
					}
					combination_all_data += "摘要,,,,,"+((combination_all_table_sum_num*1).toFixed(2)*1)+","+((combination_all_table_profit_num*1).toFixed(2)*1)+"\n";
					
					$("#download_btn2").attr('href','data:application/csv;charset=utf8,%EF%BB%BF當日成交\n時間,訂單狀態,訂單號碼,證代碼,買/賣,買入量,成交價,成交量,對盤牌號\n' + encodeURIComponent(journal_data)+'\n\n組合\n證代碼,平均買入價,買入總金額,平均賣出價,賣出總金額,總金額,盈虧\n'+ encodeURIComponent(combination_all_data));
					$("#download_btn2").get(0).click();
				}else if(data.ref=="orders" && data.orders != undefined){
					
					map={};
					journal_data = "";
					combination_data = "";
					combination_array = Array();
					combination_map = {};
					//{"algo_name":"wendy_semi","id":2,"orders":[{"algo_name":"wendy_semi","aon":"aon_type::non_ano","broker_id":"9712","code":26891,"exectuion_id":"","filled_quantity":1000000000000,"header":{"api_id":"api_id_flag::new_order","first_ref":0,"flag":"api_type_flag::single_reply","len":254,"order_id":2,"ref_data":"0x0000000000000200000000000000","session_id":"0x4B32436A416D4F68"},"match_price":12200000,"match_quantity":1000000000000,"match_records":[{"match_price":12200000,"match_quantity":1000000000000}],"order_ref":2517,"ori_price":0,"ori_quantity":0,"price":12200000,"quantity":1000000000000,"reject_reason":"","remain_quantity":0,"rep_type":"report_type::order_fill","report_id":2518,"side":"order_side::sell","status":"order_status::filled","transaction_tm":"20200528143211246","type":"order_type::sl"},{"algo_name":"wendy_semi","aon":"aon_type::non_ano","broker_id":"9712","code":26891,"exectuion_id":"","filled_quantity":1000000000000,"header":{"api_id":"api_id_flag::new_order","first_ref":0,"flag":"api_type_flag::single_reply","len":254,"order_id":1,"ref_data":"0x0000000000000100000000000000","session_id":"0x4B32436A416D4F68"},"match_price":12000000,"match_quantity":1000000000000,"match_records":[{"match_price":12000000,"match_quantity":1000000000000}],"order_ref":2515,"ori_price":0,"ori_quantity":0,"price":12000000,"quantity":1000000000000,"reject_reason":"","remain_quantity":0,"rep_type":"report_type::order_fill","report_id":2516,"side":"order_side::buy","status":"order_status::filled","transaction_tm":"20200528143210591","type":"order_type::sl"}],"ref":"orders","tm":1590647544750}
					//if(data.result == "success"){
						var order = data.orders;
						var j=0;
						for(var i=0; i<order.length; i++){
							if(order[i].transaction_tm==""){
								continue;
							}
							var type = "";
							var buy = 0;
							var sell = 0;
							var buy_sum = 0;
							var sell_sum = 0;
							var buy_count = 0;
							var sell_count = 0;
							var profit = 0;
							var filled_quantity = order[i].filled_quantity/factor_stock;
							var quantity = order[i].quantity/factor_stock;
							var price = order[i].match_price/factor_stock;
							var sum = price*filled_quantity;
							var tradeStatus = "";
							if(order[i].status.indexOf("filled")>1){
								if(filled_quantity==quantity){
									tradeStatus = "全部成交";
								}else{
									tradeStatus = "部份成交";
								}
								if(order[i].side.indexOf("buy")>1){
									buy_count = 1;
								}else if(order[i].side.indexOf("sell")>1){
									sell_count = 1;
								}
								
								if(price == 0){
									price = order[i].match_records[0].match_price/factor_stock;
									sum = price*filled_quantity;
								}
							}else if(order[i].status.indexOf("reject")>1){
								tradeStatus = "拒絕";
								if(order[i].reject_reason!=""){
									tradeStatus += " ("+order[i].reject_reason+")";
								}
							}else if(order[i].status.indexOf("cancel")>1){
								tradeStatus = "取消";
							}
							if(order[i].side.indexOf("buy")>1){
								type = "買";
								buy = price;
								//buy_count = 1;
								buy_sum = sum;
								profit = sum*(-1);
							}else if(order[i].side.indexOf("sell")>1){
								type = "賣";
								sell = price;
								//sell_count = 1;
								sell_sum = sum;
								profit = sum;
							}
							/*var time = "-";
							if(order[i].transaction_tm!=""){
								time = order[i].transaction_tm.substr(8, 2)+":"+order[i].transaction_tm.substr(10, 2)+":"+order[i].transaction_tm.substr(12, 2);
							}*/
							var time = order[i].transaction_tm.substr(8, 2)+":"+order[i].transaction_tm.substr(10, 2)+":"+order[i].transaction_tm.substr(12, 2);
							var content = "<tr><td>"+time+"</td><td>"+tradeStatus+"</td><td>"+order[i].header.order_id+"</td><td>"+order[i].code+"</td><td>"+type+"</td><td>"+formatValue(quantity)+"</td><td>"+price+"</td><td>"+formatValue(filled_quantity)+"</td><td>"+order[i].order_ref+"</td></tr>";
							$("#journal_table").append(content);
							//journal_data += time+","+tradeStatus+","+order[i].header.order_id+","+order[i].code+","+type+","+formatValue(quantity)+","+price+","+formatValue(filled_quantity)+","+order[i].order_ref+"\n";
							
							if(order[i].status.indexOf("filled")>1){
								if(map[order[i].code]!=1){
									map[order[i].code]=1;
									
									$("#combination_table").append("<tr id='combination_tr_"+order[i].code+"'><td class='code'>"+order[i].code+"<input type='hidden' class='buy_count' value='"+buy_count+"' /><input type='hidden' class='buy_avg_price' value='"+buy+"' /><input type='hidden' class='buy_total_price' value='"+buy_sum+"' /><input type='hidden' class='sell_count' value='"+sell_count+"' /><input type='hidden' class='sell_avg_price' value='"+sell+"' /><input type='hidden' class='sell_total_price' value='"+sell_sum+"' /><input type='hidden' class='total' value='"+sum+"' /><input type='hidden' class='profit' value='"+profit+"' /></td><td class='buy_avg_price'>"+buy+"</td><td class='buy_total_price'>"+addcomma(buy_sum.toFixed(2)*1)+"</td><td class='sell_avg_price'>"+sell+"</td><td class='sell_total_price'>"+addcomma(sell_sum.toFixed(2)*1)+"</td><td class='total'>"+addcomma(sum.toFixed(2)*1)+"</td><td class='profit'>"+formatProfit(profit.toFixed(2)*1)+"</td></tr>");
									
									//combination_array[j] = order[i].code+","+buy+","+(buy_sum.toFixed(2)*1)+","+sell+","+(sell_sum.toFixed(2)*1)+","+(sum.toFixed(2)*1)+","+(profit.toFixed(2)*1)+"\n";
									//combination_map[order[i].code]=j;
									j++;
								}else{
									$("#combination_tr_"+order[i].code+" input.total").val($("#combination_tr_"+order[i].code+" input.total").val()*1+sum);
									$("#combination_tr_"+order[i].code+" td.total").html(addcomma(($("#combination_tr_"+order[i].code+" input.total").val()*1).toFixed(2)*1));
									$("#combination_tr_"+order[i].code+" input.profit").val($("#combination_tr_"+order[i].code+" input.profit").val()*1+profit);
									$("#combination_tr_"+order[i].code+" td.profit").html(formatProfit(($("#combination_tr_"+order[i].code+" input.profit").val()*1).toFixed(2)*1));
									$("#combination_tr_"+order[i].code+" input.buy_count").val($("#combination_tr_"+order[i].code+" input.buy_count").val()*1+buy_count);
									$("#combination_tr_"+order[i].code+" input.buy_total_price").val($("#combination_tr_"+order[i].code+" input.buy_total_price").val()*1+buy_sum);
									$("#combination_tr_"+order[i].code+" td.buy_total_price").html(addcomma(($("#combination_tr_"+order[i].code+" input.buy_total_price").val()*1).toFixed(2)*1));
									$("#combination_tr_"+order[i].code+" input.buy_avg_price").val($("#combination_tr_"+order[i].code+" input.buy_avg_price").val()*1+buy);
									if($("#combination_tr_"+order[i].code+" input.buy_count").val()*1>0){
										$("#combination_tr_"+order[i].code+" td.buy_avg_price").html(addcomma((($("#combination_tr_"+order[i].code+" input.buy_avg_price").val()*1)/$("#combination_tr_"+order[i].code+" input.buy_count").val()*1).toFixed(4)*1));
									}
									$("#combination_tr_"+order[i].code+" input.sell_count").val($("#combination_tr_"+order[i].code+" input.sell_count").val()*1+sell_count);
									$("#combination_tr_"+order[i].code+" input.sell_total_price").val($("#combination_tr_"+order[i].code+" input.sell_total_price").val()*1+sell_sum);
									$("#combination_tr_"+order[i].code+" td.sell_total_price").html(addcomma(($("#combination_tr_"+order[i].code+" input.sell_total_price").val()*1).toFixed(2)*1));
									$("#combination_tr_"+order[i].code+" input.sell_avg_price").val($("#combination_tr_"+order[i].code+" input.sell_avg_price").val()*1+sell);
									if($("#combination_tr_"+order[i].code+" input.sell_count").val()*1>0){
										$("#combination_tr_"+order[i].code+" td.sell_avg_price").html(addcomma((($("#combination_tr_"+order[i].code+" input.sell_avg_price").val()*1)/$("#combination_tr_"+order[i].code+" input.sell_count").val()*1).toFixed(4)*1));
									}
									//combination_array[combination_map[order[i].code]] = order[i].code+","+((($("#combination_tr_"+order[i].code+" input.buy_avg_price").val()*1)/$("#combination_tr_"+order[i].code+" input.buy_count").val()*1).toFixed(4)*1)+","+(($("#combination_tr_"+order[i].code+" input.buy_total_price").val()*1).toFixed(2)*1)+","+((($("#combination_tr_"+order[i].code+" input.sell_avg_price").val()*1)/$("#combination_tr_"+order[i].code+" input.sell_count").val()*1).toFixed(4)*1)+","+(($("#combination_tr_"+order[i].code+" input.sell_total_price").val()*1).toFixed(2)*1)+","+(($("#combination_tr_"+order[i].code+" input.total").val()*1).toFixed(2)*1)+","+(($("#combination_tr_"+order[i].code+" input.profit").val()*1).toFixed(2)*1)+"\n";
								}
								$("#combination_table_sum_num").val($("#combination_table_sum_num").val()*1+sum);
								$("#combination_table_profit_num").val($("#combination_table_profit_num").val()*1+profit);
								$("#combination_table_sum").html(addcomma(($("#combination_table_sum_num").val()*1).toFixed(2)*1));
								$("#combination_table_profit").html(formatProfit(($("#combination_table_profit_num").val()*1).toFixed(2)*1));
							}
						}
						
						/*for(var i=0; i<combination_array.length; i++){
							combination_data += combination_array[i];
						}
						combination_data += "摘要,,,,,"+(($("#combination_table_sum_num").val()*1).toFixed(2)*1)+","+(($("#combination_table_profit_num").val()*1).toFixed(2)*1)+"\n";*/
					//}else{
						
					//}
				}else if(data.msg_type=="semi_algo_get"){
					//{"algo_name":"leo_semi","id":1,"msg_type":"semi_algo_get","pair":{"auto_buy":false,"auto_buy_id":102,"auto_buy_quantity":4000000000000,"auto_sell":false,"auto_sell_id":106,"bottom_price":12500000,"buy_price":12800000,"buy_trriger":21200000000,"ceiling_price":12900000,"early_buy_qty":0,"early_sell_qty":0,"is_bull":true,"is_buying":false,"is_reset_position":false,"is_selling":false,"last_price":12500000,"last_trigger_price":21200000000,"position":0,"ratio_buy":0,"ratio_sell":0,"ref":"u008_1","sell_price":12800000,"sell_trriger":21200000000,"underlying_code":9988,"underlying_symbol":"","warrant_code":26901},"ref":"u008_1","result":"SUCCESS","tm":1593486569214}
					
					var id = data.ref;
					var stop_sell = false;
					if(id.indexOf("_stopsell")>0){
						id = id.replace("_stopsell","");
						stop_sell = true;
					}
						
					if(data.result == "SUCCESS"){
						var type="";
						if(data.pair.is_buying == true){
							type = "buy";
						}else if(data.pair.is_selling == true){
							type = "sell";
						}else if(data.pair.auto_buy == true && data.pair.auto_sell == true){
							type = "auto";
						}else if(data.pair.auto_buy == true ){
							type = "buy";
						}else if(data.pair.auto_sell == true ){
							type = "sell";
						}else{
							type = "";
						}
						
						var idArr = id.split("_");
						var ucode = "";
						if(data.pair.underlying_symbol != ""){
							ucode = data.pair.underlying_symbol;
						}else{
							ucode = data.pair.underlying_code;
						}
						$("#"+idArr[0]+"ucode").val(ucode);
						$("#"+id+"code").val(data.pair.warrant_code);
					
						updateFactor(ucode);
								
						if(data.pair.buy_trriger*1>0){
							$("#"+id+"buy_ulast").val(data.pair.buy_trriger/factor);
						}
						
						if(data.pair.sell_trriger*1>0){
							$("#"+id+"sell_ulast").val(data.pair.sell_trriger/factor);
						}
						
						if(data.pair.buy_price*1>0){
							$("#"+id+"buy_last").val(data.pair.buy_price/factor_stock);
						}
						
						if(data.pair.sell_price*1>0){
							$("#"+id+"sell_last").val(data.pair.sell_price/factor_stock);
						}
						
						if(data.pair.bottom_price*1>0){
							$("#"+id+"bottom").val(data.pair.bottom_price/factor_stock);
						}
						
						if(data.pair.ceiling_price*1>0){
							$("#"+id+"ceiling").val(data.pair.ceiling_price/factor_stock);
						}
						
						/*if($("#"+id+"rc_action").val()=="buy"){
							$("#"+id+"ulast").html(data.pair.buy_trriger/factor);
						}else if($("#"+id+"rc_action").val()=="sell"){
							$("#"+id+"ulast").html(data.pair.sell_trriger/factor);
						}*/
						
						
						$("#"+id+"buy_vol").val(formatValue(data.pair.early_buy_qty));
						$("#"+id+"sell_vol").val(formatValue(data.pair.early_sell_qty));
						
						$("#"+id+"buy_ratio").val(data.pair.ratio_buy);
						$("#"+id+"sell_ratio").val(data.pair.ratio_sell);
						
						$("#"+id+"buy_status").removeClass("bg_red");
						$("#"+id+"buy_status").html("");
						$("#"+id+"sell_status").removeClass("bg_red");
						$("#"+id+"sell_status").html("");
						
						$("#"+id+"vol").val(formatValue(data.pair.auto_buy_quantity/factor_stock));
						
						$("#"+id+"t_vol").val(data.pair.position/factor_stock);
						$("#"+id+"t_vol_i").val(formatValue(data.pair.position/factor_stock));
						
						$("#"+id+"ulast").text(data.pair.last_trigger_price/factor);
						$("#"+id+"last").text((data.pair.last_price/factor_stock).toFixed(3)*1);
						
						if(data.pair.position==0){
							$("#"+id+"sell_status").removeClass("bg_orange");
							//$("#"+id+"ulast").text(data.pair.sell_trriger/factor);
							//$("#"+id+"last").text((data.pair.sell_price/factor_stock).toFixed(3)*1);
						}else{
							$("#"+id+"sell_status").addClass("bg_orange");
							//$("#"+id+"ulast").text(data.pair.buy_trriger/factor);
							//$("#"+id+"last").text((data.pair.buy_price/factor_stock).toFixed(3)*1);
						}
						
						if(type!="update"){
							if(type=="auto"){
								$("#"+id+"monbuy").removeClass("off").addClass("disable");
							}else if(type=="buy" || type=="sell"){
								$("#"+id+"as").removeClass("off").addClass("disable");
							}
							
							if(type=="buy" || type=="auto"){
								$("#"+id+"vol").val(formatValue(data.pair.auto_buy_quantity/factor_stock));
								
								if(type=="auto"){
									$("#"+id+"monbuy").removeClass("off").addClass("disable");
									$("#"+id+"as").removeClass("disable").addClass("off");
								}else if(type=="buy" || type=="sell"){
									$("#"+id+"as").removeClass("off").addClass("disable");
									$("#"+id+"monbuy").removeClass("disable").addClass("off");
								}
						
								$("#"+id+"force_buy").removeClass("disable off");
								
								$("#"+id+"buy_status").addClass("bg_green");
								$("#"+id+"buy_status").html("");
								$("#"+id+"ismon").val(1);
							}else if(type=="sell"){
								//$("#"+id+"t_vol").val(data.pair.auto_buy_quantity/factor_stock);
								//$("#"+id+"t_vol_i").val(formatValue(data.pair.auto_buy_quantity/factor_stock));
								
								$("#"+id+"monsell").removeClass("disable");
								$("#"+id+"force_sell").removeClass("disable off");
								$("#"+id+"monsell").addClass("off");
								
								$("#"+id+"sell_status").addClass("bg_green");
								$("#"+id+"sell_status").html("");
								$("#"+id+"ismon").val(2);
							}else{
								if($("#"+id+"t_vol").val()*1>0){
									$("#"+id+"monsell, #"+id+"force_sell").removeClass("disable");
									//setSell(id);
								}else{
									$("#"+id+"vol").val(formatValue(data.pair.auto_buy_quantity/factor_stock));
									$("#"+id+"monbuy, #"+id+"force_buy").removeClass("disable");
									//setBuy(id);
								}
								asBtn(id);
							}/*else if(type=="stop"){
								if(stop_sell){
									$("#"+id+"t_vol_i").val(formatValue(data.pair.auto_buy_quantity/factor_stock));
									$("#"+id+"monsell").removeClass("disable");
									$("#"+id+"force_sell").removeClass("disable off");
									$("#"+id+"monsell").addClass("off");
								}else{
									$("#"+id+"vol").val(formatValue(data.pair.auto_buy_quantity/factor_stock));
								}
								
								$("#"+id+"monbuy").removeClass("disable off");
								$("#"+id+"force_buy").removeClass("disable off");
								
								asBtn(id);
								//$("#"+id+"init").val(1);
							}*/
							/*if(type!="stop"){
								$("#"+idArr[0]+"ucode").attr('readonly','readonly');
								$("#"+id+"code").attr('readonly','readonly');
							}*/
							$("#"+id+"remove_btn").hide();
						}
					}
				}else if(data.msg_type=="semi_algo_force_buy"){
					//{"algo_name":"leo_semi","id":1,"msg_type":"semi_algo_force_buy","pair":{"auto_buy":false,"auto_buy_id":0,"auto_buy_quantity":1000000000000,"auto_sell":false,"auto_sell_id":0,"bottom_price":10700000,"buy_price":11000000,"buy_trriger":41640000000,"early_buy_qty":0,"early_sell_qty":0,"is_bull":true,"is_buying":true,"is_selling":false,"position":0,"ref":"u000_1","sell_price":11000000,"sell_trriger":41640000000,"underlying_code":700,"underlying_symbol":"","warrant_code":29707},"quantity":1000000000000,"ref":"u000_1","result":"fail new buy order not valid","tm":1590399061103}
				}else if(data.msg_type=="semi_algo_force_sell"){
					//{"algo_name":"leo_semi","id":1,"msg_type":"semi_algo_force_sell","pair":{"auto_buy":false,"auto_buy_id":0,"auto_buy_quantity":1000000000000,"auto_sell":false,"auto_sell_id":0,"bottom_price":15200000,"buy_price":15500000,"buy_trriger":42380000000,"ceiling_price":15600000,"early_buy_qty":0,"early_sell_qty":0,"is_bull":true,"is_buying":false,"is_selling":true,"position":1000000000000,"ratio_buy":0,"ratio_sell":0,"ref":"u006_1","sell_price":15700000,"sell_trriger":42400000000,"underlying_code":700,"underlying_symbol":"","warrant_code":26891},"quantity":1000000000000,"ref":"u006_1","result":"SUCCESS","tm":1590471688455}
				}else if(data.cmd=="get_buy_power"){
					//{"buy_power":999999999,"cmd":"get_buy_power","id":1,"ref":"power","tm":1590398976167}
					maxAmmount = data.buy_power*1;
					sendWsMsg(getWsMsg("top_power", ""));
					//sendWsMsg(getWsMsg("position", ""));
				}else if(data.cmd=="get_top_buy_power"){
					//{"buy_power":999999999,"cmd":"get_buy_power","id":1,"ref":"power","tm":1590398976167}
					var maxAmmountTop = Math.floor(data.buy_power*1/100000000);
					if(maxAmmount>maxAmmountTop){
						maxAmmount=maxAmmountTop;
					}
					sendWsMsg(getWsMsg("position", ""));
				}else if(data.msg_type=="semi_algo_del"){
					//{"algo_name":"leo_semi","id":1,"msg_type":"semi_algo_del","pair":{"auto_buy":false,"auto_buy_id":0,"auto_buy_quantity":1000000000000,"auto_sell":false,"auto_sell_id":0,"bottom_price":10700000,"buy_price":11000000,"buy_trriger":41640000000,"early_buy_qty":0,"early_sell_qty":0,"is_bull":true,"is_buying":true,"is_selling":false,"position":0,"ref":"u000_1","sell_price":11000000,"sell_trriger":41640000000,"underlying_code":700,"underlying_symbol":"","warrant_code":29707},"ref":"u000_1","result":"SUCCESS","tm":1590399426175}
					//{"algo_name":"leo_semi","id":1,"msg_type":"semi_algo_del","pair":{"auto_buy":false,"auto_buy_id":0,"auto_buy_quantity":1000000000000,"auto_sell":true,"auto_sell_id":0,"bottom_price":13800000,"buy_price":14100000,"buy_trriger":42720000000,"ceiling_price":14200000,"early_buy_qty":0,"early_sell_qty":0,"is_bull":true,"is_buying":false,"is_selling":false,"position":1000000000000,"ratio_buy":0,"ratio_sell":0,"ref":"u000_1","sell_price":14100000,"sell_trriger":42720000000,"underlying_code":700,"underlying_symbol":"","warrant_code":29707},"ref":"u000_1","result":"SUCCESS","tm":1590475988339}
					
					var id = data.ref;
				
					var type="";
					if(data.pair.auto_buy == true && data.pair.auto_sell == true){
						type = "auto";
					}else if(data.pair.auto_buy == true ){
						type = "buy";
					}else if(data.pair.auto_sell == true ){
						type = "sell";
					}
					
					if(type=="sell"){
						stopSell(id);
					}else if(type=="buy"){
						stopBuy(id,"");
					}else{
						stopAutoBuy(id, "");
					}
				}
			}
		}
		ws.onclose = function(){
			console.log("已關閉！");
			retryCount++;
			if(retryCount>10){
				initLogout();
			}else{
				console.log("Retry: "+retryCount);
				ws = null;
				initWebsocket(); 
			}
		};
	}else{
		alert("您的瀏覽器不支持 WebSocket!");
	}
}

function printLog(today,receive_time,received_msg){
	//console.log(today+" Receive"+receive_time+": "+received_msg);
}

function sendWsMsg(msg){
	if (!ws) {
		console.log("已關閉！");
		initLogout();
		return;
	}
	if(msg==""){
		return;
	}
	var today = new Date().toLocaleString();
	console.log(today+" Send: "+msg);
	waitForSocketConnection(ws, function() {
		ws.send(msg);
	});
}

function waitForSocketConnection(socket, callback){
	setTimeout(
		function(){
			if (socket.readyState === 1) {
				if(callback !== undefined){
					callback();
				}
				return;
			} else {
				waitForSocketConnection(socket,callback);
			}
		}, 
	5);
};


//------------------------------------old code--------------------------------------------------------------
function sendWS(msg){
	if (!ws) {
		console.log("Connection is closed");
		return;
	}
	var today = new Date().toLocaleString();
	console.log(today+" Send: "+msg);
	waitForSocketConnection(ws, function() {
		ws.send(msg);
	});
}
