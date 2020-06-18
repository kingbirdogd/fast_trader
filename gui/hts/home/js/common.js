var ws = null;
var ls = false;
var default_tickin = "0";
var default_max_entry = "0";
var default_tick_bottom = "-3";
var default_tick_ceiling = "1";
var default_tick_out = "0";
var default_min_exit = "0";
var default_sell_ulast = "0";
var htsstatus = ["-", "READY BUY", "MONBUY", "READY SELL", "MONSELL", "STOCK INVALID", "FULLY EXE", "PARTIAL EXE"];
var mousedown = false;
var ucodeNum=40;
var codeNum=4;
var callNum=30;
var wsuserid = "";
var wsuser = "";
var user_id = 0;
var key = "";
var map = {};
var isLoginFail = true;
var priceUrl = "https://secure.dbpower.com.hk/hts/hts_pricetable.php";
var priceUrlIndex = "https://secure.dbpower.com.hk/hts/hts_pricetable_index.php";
//var domain = "192.168.134.38:9919"; // 192.168.134.38:9915 - deploy
var domain = location.hostname;
if(location.port!=""){
	domain += ":"+location.port;
}
var factor = 100000000;
var factor_stock = 100000000;
var factor_index = 100000;
var maxAmmount = 1000000;
var isRecovery = true;
var journal_data = "";
var combination_data = "";
var combination_array = Array();
var combination_map = {};
var retryCount = 0;
var isAddPosstion = false;

$(document).ready(function() {
	if (typeof(Storage) !== "undefined") {
		ls = true;
	} else {
		// Sorry! No Web Storage support..
	}
	
	if(ls && localStorage.getItem('uid')){
		wsuser=localStorage.getItem('uname');
		wsuserid=localStorage.getItem('uid');
		initLogin();
    }else{
		initLogout();
    }
});

function loadHis(){
	for(var i=0; i<ucodeNum; i++){
		var id = 'u'+adddigit(i);
		var id2 = id+"_1";
		sendWsMsg(getWsMsg("get", id2));
	}
}

function login(){
	if($("#username").val()!="" && $("#pw").val()!=""){
		//getIP();
		$.ajax({url: "http://"+domain+"/validate", type: "GET", data: {user_name: $("#username").val(), password: $("#pw").val()}, dataType: 'jsonp', jsonpCallback: "callback", crossDomain: true, success: function(res){
			var result = res.result;
			var uname = res.uname;
			var passowrd = res.passowrd;
			var uid = res.uid;
			if(result=='success'){
				if(ls){
					localStorage.setItem('uname', uname);
					localStorage.setItem('uid', uid);
				}
				wsuser=uname;
				wsuserid=uid;
				initLogin();
			}else if(result=='fail'){
				isLoginFail = true;
				alert("登入帳戶或密碼錯誤");
			}
		}});
	}else{
		alert("請輸入登入帳戶和密碼");
	}
}

function initLogin(){
	if(ls){
		localStorage.wsuser=wsuser;
		localStorage.wsuserid=wsuserid;
		if (localStorage[wsuser+"_default_tickin"]!=undefined) {
			default_tickin = localStorage[wsuser+"_default_tickin"];
		}
		if (localStorage[wsuser+"_default_max_entry"]!=undefined) {
			default_max_entry = localStorage[wsuser+"_default_max_entry"];
		}
		if (localStorage[wsuser+"_default_tick_bottom"]!=undefined) {
			default_tick_bottom = localStorage[wsuser+"_default_tick_bottom"];
		}
		if (localStorage[wsuser+"_default_tick_ceiling"]!=undefined) {
			default_tick_ceiling = localStorage[wsuser+"_default_tick_ceiling"];
		}
		if (localStorage[wsuser+"_default_tick_out"]!=undefined) {
			default_tick_out = localStorage[wsuser+"_default_tick_out"];
		}
		if (localStorage[wsuser+"_default_min_exit"]!=undefined) {
			default_min_exit = localStorage[wsuser+"_default_min_exit"];
		}
		if (localStorage[wsuser+"_default_sell_ulast"]!=undefined) {
			default_sell_ulast = localStorage[wsuser+"_default_sell_ulast"];
		}
	}
	
	$("#default_tickin").val(default_tickin);
	$("#default_max_entry").val(default_max_entry);
	$("#default_tick_bottom").val(default_tick_bottom);
	$("#default_tick_ceiling").val(default_tick_ceiling);
	$("#default_tick_out").val(default_tick_out);
	$("#default_sell_ulast").val(default_sell_ulast);
	
	key=wsuser+"_semi";
					
	isLoginFail = false;
	$("#default_tickin").val(default_tickin);
	$("#default_max_entry").val(default_max_entry);
	$("#default_tick_bottom").val(default_tick_bottom);
	$("#default_tick_ceiling").val(default_tick_ceiling);
	$("#default_tick_out").val(default_tick_out);
	$("#default_sell_ulast").val(default_sell_ulast);
	$("#default_min_exit").val(default_min_exit);
	$("#page").removeClass("home").addClass("inside");
	initTable();
	$("#login_area").hide();
	$("#content_area").show();
	initWebsocket();
}

function initLogout(){
	$("#page").removeClass("inside").addClass("home");
	$("#content_area").hide();
	$("#login_area").show();
	if (ws) {
		ws.close();
	}
	if(ls){
		localStorage.removeItem('uname');
		localStorage.removeItem('uid');
	}
	if(!isLoginFail){
		location.reload();
	}
}

function logout(){
	$.ajax({url: "http://"+domain+"/delete", type: "GET", data: {uid: localStorage.getItem('uid')}, dataType: 'jsonp', jsonpCallback: "callback", crossDomain: true, success: function(res){
		var result = res.result;
		var uname = res.uid;
		var sessions = res.sessions;
		if(result=='success'){
			initLogout();
		}else if(result=='fail'){
			alert("登出失敗！");
		}
	}});
}

function initTable(){
	$(".glossary").mouseover(function(){
		$('#glossary_content').show();
	});
	
	$(".glossary").mouseout(function(){
		$('#glossary_content').hide();
	});
	
	$.jatt({
		cacheData:false,
		direction:'e',
		hidden: hideTooltip
	});
	
	var wtype = '';
	var addedSeparate = false;
	for(var i=0; i<ucodeNum; i++){
		var id = 'u'+adddigit(i);
		var separate = "";
		var id2 = id+"_1";
		
		if(i<callNum){
			wtype = '<span id="'+id2+'wtype_text" class="wtype_text call">購</span><input type="hidden" name="" id="'+id2+'wtype" value="bull" class="wtype" />';
		}else{
			if(!addedSeparate){
				separate = "separate";
				addedSeparate = true;
			}
			wtype = '<span id="'+id2+'wtype_text" class="wtype_text put">沽</span><input type="hidden" name="" id="'+id2+'wtype" value="bear" class="wtype" />';
		}
		var codeContent = '';
		codeContent += '<tr id="'+id2+'" class="'+id+' '+id+'c '+separate+'">';
        codeContent += '<td class="underlying"><input name="input2" type="text" id="'+id+'ucode" value="" placeholder="輸入正股" class="ucode" /></td>';
        codeContent += '<td><input type="text" name="" id="'+id2+'code" class="code" /></td>';
        codeContent += '<td>'+wtype+'</td>';
        codeContent += '<td><input type="text" name="" id="'+id2+'vol" class="vol" /></td>';
        codeContent += '<td class="green_bg"><input type="text" name="" id="'+id2+'buy_ulast" class="buy_ulast" /></td>';
        codeContent += '<td class="green_bg">';
        codeContent += '<input type="text" name="" id="'+id2+'buy_last" class="buy_last" />';
		codeContent += '<a class="price button pricebtn tip" id="'+id2+'tip" style="cursor:pointer;" >$</a><a class="price tooltip button pricebtn tip2" id="'+id2+'tip2" rel="#tip_'+id2+'" style="display:none;cursor:pointer;">$</a>';
        codeContent += '<div id="tip_'+id2+'" class="tooltip_popup" style="display:none">';
        codeContent += '</div>';
        codeContent += '</td>';
        codeContent += '<td class="green_bg"><input type="text" name="" id="'+id2+'buy_vol" class="buy_vol" /></td>';
        codeContent += '<td class="green_bg border_right"><input type="text" name="" id="'+id2+'buy_ratio" class="buy_ratio" /></td>';
        codeContent += '<td class="red_bg"><input type="text" name="" id="'+id2+'sell_ulast" class="sell_ulast" /></td>';
        codeContent += '<td class="red_bg"><input type="text" name="" id="'+id2+'sell_last" class="sell_last" /></td>';
        codeContent += '<td class="red_bg"><input type="text" name="" id="'+id2+'sell_vol" class="sell_vol" /></td>';
        codeContent += '<td class="red_bg"><input type="text" name="" id="'+id2+'sell_ratio" class="sell_ratio" /></td>';
        codeContent += '<td class="left"><a id="'+id2+'monbuy" class="button buy monbuy disable" href="javascript:clickBuy(\''+id2+'\');">開啟</a><input type="hidden" name="" id="'+id2+'init" value="0" class="init" /><input type="hidden" name="" id="'+id2+'init1" value="0" class="init1" /><input type="hidden" name="" id="'+id2+'init2" value="0" class="init2" /><input type="hidden" name="" id="'+id2+'ismon" value="0" class="ismon" /><input type="hidden" name="" id="'+id2+'orderno" value="-1" class="orderno" /><input type="hidden" name="" id="'+id2+'error" value="0" class="error" /><input type="hidden" name="" id="'+id2+'rc_action" value="" class="rc_action" /><input type="hidden" name="" id="'+id2+'click_sell" value="" class="click_sell" /></td>';//ismon -> 1: monbuy, 2: monsell
        codeContent += '<td><a id="'+id2+'force_buy" class="button buy force_buy disable" href="javascript:clickForcebuy(\''+id2+'\');">即買</a></td>';
        //codeContent += '<td><input type="text" name="" id="'+id2+'tc" class="tc" value="'+$("#default_tick_ceiling").val()+'" style="width:35px;text-align: center;" /></td>';
        codeContent += '<td><a id="'+id2+'as" class="button as disable" href="javascript:clickAS(\''+id2+'\');">開啟</a></td>';
        codeContent += '<td id="'+id2+'buy_status" class="border_right"></td>';
        codeContent += '<td id="'+id2+'ulast">0.000</td>';
        codeContent += '<td id="'+id2+'last">0.000</td>';
        codeContent += '<td class="border_right"><input type="text" name="" id="'+id2+'t_vol_i" class="t_vol_i" style="width: 66px;" /><input type="hidden" name="" id="'+id2+'t_vol" class="t_vol" /></td>';
        codeContent += '<td><input type="text" name="" id="'+id2+'bottom" class="bottom" /></td>';
        codeContent += '<td><input type="text" name="" id="'+id2+'ceiling" class="ceiling" /></td>';
        codeContent += '<td class="left"><a id="'+id2+'monsell" class="button sell monsell disable" href="javascript:clickSell(\''+id2+'\');">開啟</a></td>';
        codeContent += '<td><a id="'+id2+'force_sell" class="button sell force_sell disable" href="javascript:clickForcesell(\''+id2+'\');">即賣</a></td>';
        codeContent += '<td id="'+id2+'sell_status" class="border_right"></td>';
        codeContent += '</tr>';
		
		$("#trade_table").append(codeContent);
		
		updatePricePopup(id2);
		
		//allOverInput(id2);
		allremoveString(id2);
		
		allBind(id2);
		
		$("#"+id+"ucode").focus(function(){
			var val = $(this).val();
			$(this).blur(function(){
				var val2 = $(this).val();
				var id=$(this).closest("tr").attr("id");
				
				if(!$("#"+id+"remove_btn").is(':hidden') && val*1!=val2*1 && isUnderlying(val2)){//console.log(id+" -- "+$("#"+id+"code").val());
					if(isWarrant($("#"+id+"code").val()*1)){
						sendWsMsg(getWsMsg("getucode", id));
					}
					setBuy(id);
				}
				$(this).unbind("blur"); 
			});
		});
	
		$("#"+id2+"code").focus(function(){
			var val = $(this).val();
			$(this).blur(function(){
				var val2 = $(this).val();
				var id=$(this).closest("tr").attr("id");
				//console.log((val*1)+" -- "+(val2*1)+"--"+(val*1!=val2*1)+" -- "+isWarrant(val2*1)+" -- "+Number.isInteger(val2*1));	
				if(!$("#"+id+"remove_btn").is(':hidden') && val*1!=val2*1 && isWarrant(val2*1)&&  Number.isInteger(val2*1)){
					sendWsMsg(getWsMsg("getucode", id));
					setBuy(id);
				}
				$(this).unbind("blur");
			});
		});
		
		$("#"+id2+"vol").focus(function(){
			var val = $(this).val();
			$(this).blur(function(){
				var val2 = $(this).val();
				if(getValue(val)!=getValue(val2)){
					var id=$(this).closest("tr").attr("id");
					setBuy(id);
				}
				$(this).unbind("blur");
			});
		});
		
		$("#"+id2+"buy_ulast").focus(function(){
			var val = $(this).val();
			$(this).blur(function(){
				var val2 = $(this).val();
				if(val*1!=val2*1 && val2*1>0){
					var id=$(this).closest("tr").attr("id");
					var sign=1;
					$("#"+id+"sell_ulast").val(getValBySpread(val2, $("#default_sell_ulast").val()*sign));
					setBuy(id);
				}else if(val*1>0 && isNaN(val2*1)){
					$(this).val(val);
				}
				$(this).unbind("blur");
			});
		});
		
		$("#"+id2+"buy_last").focus(function(){
			var val = $(this).val();
			$(this).blur(function(){
				var val2 = $(this).val();
				if(val*1!=val2*1 && val2*1>0){
					var id=$(this).closest("tr").attr("id");
					var sign=1;
					$("#"+id+"sell_last").val(getValBySpread(val2, $("#default_tick_out").val()*sign));
					if($("#default_tick_bottom").val()!=""){
						$("#"+id+"bottom").val(getValBySpread(val2, $("#default_tick_bottom").val()*sign));
					}else{
						$("#"+id+"bottom").val(0);
					}
					$("#"+id+"ceiling").val(getValBySpread(val2, $("#default_tick_ceiling").val()*sign));
					setBuy(id);
				}else if(val*1>0 && isNaN(val2*1)){
					$(this).val(val);
				}
				$(this).unbind("blur");
			});
		});
		
		$("#"+id2+"buy_vol").focus(function(){
			var val = $(this).val();
			$(this).blur(function(){
				var val2 = $(this).val();
				if(getValue(val)!=getValue(val2)){
					var id=$(this).closest("tr").attr("id");
					setBuy(id);
				}
				$(this).unbind("blur");
			});
		});
		
		$("#"+id2+"buy_ratio").focus(function(){
			var val = $(this).val();
			$(this).blur(function(){
				var val2 = $(this).val();
				if(isRatio(val2) && val!=val2){
					var id=$(this).closest("tr").attr("id");
					setBuy(id);
				}
				$(this).unbind("blur");
			});
		});
		
		$("#"+id2+"sell_ulast").focus(function(){
			var val = $(this).val();
			$(this).blur(function(){
				var val2 = $(this).val();
				if(val*1!=val2*1 && val2*1>0){
					var id=$(this).closest("tr").attr("id");
					if($("#"+id+"as").hasClass("off") || $("#"+id+"t_vol").val()==0){
						setBuy(id);
					}else{
						setSell(id);
					}
				}else if(val*1>0 && isNaN(val2*1)){
					$(this).val(val);
				}
				$(this).unbind("blur");
			});
		});
		
		$("#"+id2+"sell_last").focus(function(){
			var val = $(this).val();
			$(this).blur(function(){
				var val2 = $(this).val();
				if(val*1!=val2*1 && val2*1>0){
					var id=$(this).closest("tr").attr("id");
					if($("#"+id+"as").hasClass("off") || $("#"+id+"t_vol").val()==0){
						setBuy(id);
					}else{
						setSell(id);
					}
				}else if(val*1>0 && isNaN(val2*1)){
					$(this).val(val);
				}
				$(this).unbind("blur");
			});
		});
		
		$("#"+id2+"sell_vol").focus(function(){
			var val = $(this).val();
			$(this).blur(function(){
				var val2 = $(this).val();
				if(getValue(val)!=getValue(val2)){
					var id=$(this).closest("tr").attr("id");
					if($("#"+id+"as").hasClass("off")){
						setBuy(id);
					}else{
						setSell(id);
					}
				}
				$(this).unbind("blur");
			});
		});
		
		$("#"+id2+"sell_ratio").focus(function(){
			var val = $(this).val();
			$(this).blur(function(){
				var val2 = $(this).val();
				if(isRatio(val2) && val!=val2){
					var id=$(this).closest("tr").attr("id");
					if($("#"+id+"as").hasClass("off")){
						setBuy(id);
					}else{
						setSell(id);
					}
				}
				$(this).unbind("blur");
			});
		});
		
		$("#"+id2+"t_vol_i").focus(function(){
			var val = $(this).val();
			$(this).blur(function(){
				var val2 = $(this).val();
				if(getValue(val)!=getValue(val2)){
					if(confirm("確定變更？")){
						$("#"+id2+"t_vol").val(getValue(val2));
						isAddPosstion = true;
						var id=$(this).closest("tr").attr("id");
						setSell(id);
					}else{
						$(this).val(val);
					}
				}
				$(this).unbind("blur");
			});
		});
		
		$("#"+id2+"bottom").focus(function(){
			var val = $(this).val();
			$(this).blur(function(){
				var val2 = $(this).val();
				if(val*1!=val2*1 && val2*1>0){
					var id=$(this).closest("tr").attr("id");
					if($("#"+id+"as").hasClass("off")){
						setBuy(id);
					}else{
						setSell(id);
					}
				}else if(val*1>0 && isNaN(val2*1)){
					$(this).val(val);
				}
				$(this).unbind("blur");
			});
		});
		
		$("#"+id2+"ceiling").focus(function(){
			var val = $(this).val();
			$(this).blur(function(){
				var val2 = $(this).val();
				if(val*1!=val2*1 && val2*1>0){
					var id=$(this).closest("tr").attr("id");
					if($("#"+id+"as").hasClass("off")){
						setBuy(id);
					}else{
						setSell(id);
					}
				}else if(val*1>0 && isNaN(val2*1)){
					$(this).val(val);
				}
				$(this).unbind("blur");
			});
		});
		
		/*$("#"+id2).keydown(function(objEvent) {
			console.log(id2);
		});*/
	}
	
	/*$(window).on("mousedown", function() {
		mousedown = true;
		for(var i=0; i<ucodeNum; i++){
			var id = 'u'+adddigit(i);
			var id2 = id+"_1";
			allOverCheck(id2);
		}
	});
	
	$(window).on("mouseup", function() {
		mousedown = false;
		for(var i=0; i<ucodeNum; i++){
			var id = 'u'+adddigit(i);
			var id2 = id+"_1";
			allUnbind(id2);
		}
	});*/
	
	$("#default_tick_bottom").focus(function(){
		var val = $(this).val();
		$(this).blur(function(){
			var val2 = $(this).val();
			if(val!=val2 && !isNaN(val2)){
				save();
			}else{
				$(this).val(val);
			}
			$(this).unbind("blur");
		});
	});
	
	$("#default_tick_ceiling").focus(function(){
		var val = $(this).val();
		$(this).blur(function(){
			var val2 = $(this).val();
			if(val!=val2 && !isNaN(val2)){
				//updateSetting("tc", "tick_ceiling");
				save();
			}else{
				$(this).val(val);
			}
			$(this).unbind("blur");
		});
	});
	
	$("#default_tick_out").focus(function(){
		var val = $(this).val();
		$(this).blur(function(){
			var val2 = $(this).val();
			if(val!=val2 && !isNaN(val2)){
				save();
			}else{
				$(this).val(val);
			}
			$(this).unbind("blur");
		});
	});
	
	$("#default_sell_ulast").focus(function(){
		var val = $(this).val();
		$(this).blur(function(){
			var val2 = $(this).val();
			if(val!=val2 && !isNaN(val2)){
				save();
			}else{
				$(this).val(val);
			}
			$(this).unbind("blur");
		});
	});
	
	$(document).keydown(function(objEvent) {
		/*if (objEvent.keyCode == 9) {  //tab pressed
			objEvent.preventDefault(); // stops its action
		}*/
	});
	
	inputSelectBlur();
	
	$("#download_btn").click(function(){
		//var data = "a,b,c\n1,2,3\na,b,c\n1,2,3\na,b,c\n1,2,3\na,b,c\n1,2,3\na,b,c\n1,2,3\na,b,c\n1,2,3\na,b,c\n1,2,3\na,b,c\n1,2,3\na,b,c\n1,2,3\na,b,c\n1,2,3\na,b,c\n1,2,3\na,b,c\n1,2,3\na,b,c\n1,2,3\na,b,c\n1,2,3\na,b,c\n1,2,3\na,b,c\n1,2,3\na,b,c\n1,2,3\na,b,c\n1,2,3\na,b,c\n1,2,3\na,b,c\n1,2,3\na,b,c\n1,2,3\na,b,c\n1,2,3\na,b,c\n1,2,3\na,b,c\n1,2,3\na,b,c\n1,2,3\na,b,c\n1,2,3\na,b,c\n1,2,3\na,b,c\n1,2,3\na,b,c\n1,2,3\na,b,c\n1,2,3\na,b,c\n1,2,3\na,b,c\n1,2,3\na,b,c\n1,2,3\na,b,c\n1,2,3\na,b,c\n1,2,3\na,b,c\n1,2,3\na,b,c\n1,2,3\na,b,c\n1,2,3\na,b,c\n1,2,3\na,b,c\n1,2,3\na,b,c\n1,2,3\na,b,c\n1,2,3\na,b,c\n1,2,3\na,b,c\n1,2,3\na,b,c\n1,2,3\na,b,c\n1,2,3\na,b,c\n1,2,3\na,b,c\n1,2,3\n";
		$("#download_btn2").attr('href','data:application/csv;charset=utf8,%EF%BB%BF當日成交\n時間,訂單狀態,訂單號碼,證代碼,買/賣,買入量,成交價,成交量,對盤牌號\n' + encodeURIComponent(journal_data)+'\n\n組合\n證代碼,平均買入價,買入總金額,平均賣出價,賣出總金額,總金額,盈虧\n'+ encodeURIComponent(combination_data));
		$("#download_btn2").get(0).click();
	});
}

function updatePricePopup(id){
	var priceContent = '';
	priceContent += '<div class="tl">對價表</div>';
	priceContent += '<div align="right" class="timer"><span id="'+id+'tip_time"></span></div>';
	priceContent += '<div class="clear"></div>';
	priceContent += '<div class="table" id="'+id+'tip_table">';
	priceContent += '<table width="100%" border="0" cellspacing="0" cellpadding="0">';
	priceContent += '<thead>';
	priceContent += '<tr>';
	priceContent += '<th>股買入價</th>';
	priceContent += '<th>股賣出價</th>';
	priceContent += '<th>證買入價</th>';
	priceContent += '<th>證賣出價</th>';
	priceContent += '</tr>';
	priceContent += '</thead>';
	priceContent += '<tbody id="'+id+'tip_body">';
	priceContent += '</tbody>';
	priceContent += '</table>';
	priceContent += '</div>';
	priceContent += '<div class="table" id="'+id+'tip_table_index">';
	priceContent += '<table width="100%" border="0" cellspacing="0" cellpadding="0">';
	priceContent += '<thead>';
	priceContent += '<tr>';
	priceContent += '<th>證價</th>';
	priceContent += '<th>股賣出價</th>';
	priceContent += '<th>股買入價</th>';
	priceContent += '</tr>';
	priceContent += '</thead>';
	priceContent += '<tbody id="'+id+'tip_body_index">';
	priceContent += '</tbody>';
	priceContent += '</table>';
	priceContent += '</div>';
	
	$("#tip_"+id).html(priceContent);
	
	$("#"+id+"tip").click(function(){
		if(isWarrant($("#"+id+"code").val()*1)){
			if($("#"+id+"error").val()==0){
				getpriceTable(id);
			}
		}else{
			$("#"+id+"tip_time").html("-");
			$("#"+id+"tip_body").html("");
		}
	});
	
	$("#"+id+"tip").mouseout(function(){
		$("#"+id+"tip2").blur();
	});
	
	$("#"+id+"tip2").click(function(){
		if($("#tooltip").length>0){
			$("#"+id+"tip2").hide();
			$("#"+id+"tip").show();
		}
	});
}

function getpriceTable(id){
	var data = {code: $("#"+id+"code").val()};
	if($("#"+id+"buy_ulast").val()*1>0){
		data = {code: $("#"+id+"code").val(), ubid: getValBySpread($("#"+id+"buy_ulast").val(),-1), uask: getValBySpread($("#"+id+"buy_ulast").val(),1)};
	}
	$.ajax({url: priceUrl, type: "POST", data: data, cache: false, success: function(data) {
		if(data && data["p"] && data["p"]!=""){
			$("#"+id+"tip_time").html(data["p"]);
			var tip_body="";
			for(var i = 0; i < data.tr.length; i++) {
				var obj = data.tr[i];
				var className = "";
				if(obj["class"]){
					className = obj["class"];
				}
				tip_body+="<tr class='"+className+"'><td>"+obj.td[0]+"</td><td>"+obj.td[1]+"</td><td>"+obj.td[2]+"</td><td>"+obj.td[3]+"</td></tr>";
			}
			$("#"+id+"tip_body").html(tip_body);
			$("#"+id+"tip_table_index").hide();
			$("#"+id+"tip_table").show();
			
			$("#"+id+"tip").hide();
			$("#"+id+"tip2").show();
		}else{
			$("#"+id+"tip_time").html("");
			$("#"+id+"tip_body").html("");
			getpriceTableIndex(id);
		}
	}});
}

function getpriceTableIndex(id){
	var data = {code: $("#"+id+"code").val()};
	$.ajax({url: priceUrlIndex, type: "POST", data: data, cache: false, success: function(data) {
		if(data && data["p"] && data["p"]!=""){
			//$("#"+id+"tip_time").html(data["p"].substring(0,4)+"-"+data["p"].substring(4,6)+"-"+data["p"].substring(6,8)+" "+data["p"].substring(8,10)+":"+data["p"].substring(10,12));
			$("#"+id+"tip_time").html(new Date(data["p"]*1000).toLocaleString("sv-SE"));
			var tip_body="";
			for(var i = 0; i < data.tr.length; i++) {
				var obj = data.tr[i];
				var className = "";
				if(obj["class"]){
					className = obj["class"];
				}
				tip_body+="<tr class='"+className+"'><td>"+obj.td[0]+"</td><td>"+obj.td[2]+"</td><td>"+obj.td[1]+"</td></tr>";
			}
			$("#"+id+"tip_body_index").html(tip_body);
			$("#"+id+"tip_table").hide();
			$("#"+id+"tip_table_index").show();
		}else{
			$("#"+id+"tip_time").html("");
			$("#"+id+"tip_body_index").html("");
		}
		$("#"+id+"tip").hide();
		$("#"+id+"tip2").show();
	}});
}

function setBuy(id){ 
	var id2 = id.split("_");
	var ucode = $("#"+id2[0]+"ucode").val();
	var code = $("#"+id+"code").val()*1;
	var vol = getValue($("#"+id+"vol").val());
	var t_vol = getValue($("#"+id+"t_vol_i").val());
	
	var buy_ulast = $("#"+id+"buy_ulast").val()*1;
	var buy_last = $("#"+id+"buy_last").val()*1;
	if(isUnderlying(ucode) && isWarrant(code) && Number.isInteger(code) && vol>0 && buy_ulast>0 && buy_last>0 && t_vol*1==0){
		$("#"+id+"monsell").addClass("disable");
		$("#"+id+"force_sell").addClass("disable");
		
		if($("#"+id+"force_buy").hasClass("off") || $("#"+id+"monbuy").hasClass("off") || $("#"+id+"as").hasClass("off")){
			$("#"+id2[0]+"ucode").attr('readonly','readonly');
			$("#"+id+"code").attr('readonly','readonly');
			$("#"+id+"remove_btn").hide();
			
			$("#"+id+"monsell").addClass("disable");
			$("#"+id+"force_sell").addClass("disable");
			
			/*if($("#"+id+"force_buy").hasClass("off")){
				sendWsMsg(getWsMsg("setbuy", id));
				sendWsMsg(getWsMsg("forcebuy", id));
			}else{
				sendWsMsg(getWsMsg("setbuy", id));
			}*/
			
			if($("#"+id+"ismon").val()*1==0){
				if($("#"+id+"as").hasClass("off")){
					sendWsMsg(getWsMsg("auto", id));
				}else{
					sendWsMsg(getWsMsg("setbuy", id));
				}
			}else{
				sendWsMsg(getWsMsg("updatebuy", id));
			}
		}else{
			if($("#"+id+"init").val()*1==0){
				sendWsMsg(getWsMsg("stop", id));
			}else{
				$("#"+id+"monbuy").removeClass("disable");
				$("#"+id+"force_buy").removeClass("disable");
				$("#"+id+"init").val(1);
			}
			
			$("#"+id2[0]+"ucode").removeAttr('readonly');
			$("#"+id+"code").removeAttr('readonly');
			$("#"+id+"remove_btn").show();
		}
	}else{//console.log("aaaa t_vol:"+t_vol+" buy_last:"+buy_last+" vol:"+vol+" code:"+code+" ucode:"+ucode+" id:"+id);
		$("#"+id+"monbuy").removeClass("off");
		$("#"+id+"force_buy").removeClass("off");
		$("#"+id+"monbuy").addClass("disable");
		$("#"+id+"force_buy").addClass("disable");
		stopBuy(id, "1");
	}
	
	asBtn(id);
}

function setSell(id){
	var idArr = id.split("_");
	var ucode = $("#"+idArr[0]+"ucode").val();
	var code = $("#"+id+"code").val()*1;
	var sell_last = $("#"+id+"sell_last").val()*1;
	var sell_ulast = $("#"+id+"sell_ulast").val()*1;
	var t_vol = getValue($("#"+id+"t_vol_i").val());
	
	//console.log("setSell - "+ucode+" - "+code+" - "+sell_last+" - "+sell_ulast+" - "+t_vol);
	
	//$("#"+id+"monsell").removeClass("orange");
	if(isUnderlying(ucode) && isWarrant(code) && Number.isInteger(code) && sell_last>0 && sell_ulast>0 && t_vol!=0){
		
		$("#"+id+"monbuy").addClass("disable");
		$("#"+id+"force_buy").addClass("disable");
			
		if($("#"+id+"force_sell").hasClass("off") || $("#"+id+"monsell").hasClass("off")){
			$("#"+idArr[0]+"ucode").attr('readonly','readonly');
			$("#"+id+"code").attr('readonly','readonly');
			$("#"+id+"remove_btn").hide();
			//$("#"+id+"status").text(htsstatus[4]);
			
			$("#"+id+"monbuy").addClass("disable");
			$("#"+id+"force_buy").addClass("disable");
			
			if($("#"+id+"ismon").val()*1==0){
				sendWsMsg(getWsMsg("setsell", id));
			}else{
				sendWsMsg(getWsMsg("updatesell", id));
			}
		}else{
			$("#"+id+"monsell").removeClass("disable");
			$("#"+id+"force_sell").removeClass("disable");
			$("#"+id+"sell_status").addClass("bg_orange");
			$("#"+id+"sell_status").html("");
		}
	}else{
		$("#"+id+"monsell").removeClass("off");
		$("#"+id+"force_sell").removeClass("off");
		$("#"+id+"monsell").addClass("disable");
		$("#"+id+"force_sell").addClass("disable");
		stopSell(id);
		//$("#"+id+"status").text(htsstatus[0]);
	}
	
	asBtn(id);
}

function clickBuy(id){//class=off: clicked
	//$("#"+id+"ismon").val(0);
	if($("#"+id+"force_buy").hasClass("off")){
		sendWsMsg(getWsMsg("setbuy", id));
	}else if($("#"+id+"monbuy").hasClass("off")){
		//sendWsMsg(getWsMsg("delete", id));
		sendWsMsg(getWsMsg("stop", id));
	}else{
		$("#"+id+"monbuy").addClass("off");
		setBuy(id);
	}
}

function clickForcebuy(id){
	//$("#"+id+"ismon").val(0);
	if($("#"+id+"monbuy").hasClass("off") || $("#"+id+"as").hasClass("off")){
		sendWsMsg(getWsMsg("forcebuy", id));
	}else if($("#"+id+"force_buy").hasClass("off")){
		//sendWsMsg(getWsMsg("delete", id));
		sendWsMsg(getWsMsg("stop", id));
	}else{
		$("#"+id+"force_buy").addClass("off");
		setBuy(id);
	}
}

function clickSell(id){
	//$("#"+id+"ismon").val(0);
	if($("#"+id+"force_sell").hasClass("off")){
		sendWsMsg(getWsMsg("setsell", id));
	}else if($("#"+id+"monsell").hasClass("off") || $("#"+id+"monsell").hasClass("orange")){
		//sendWsMsg(getWsMsg("delete", id));
		sendWsMsg(getWsMsg("stopsell", id));
	}else{
		$("#"+id+"monsell").addClass("off");
		$("#"+id+"click_sell").val(1);
		setSell(id);
	}
}

function clickForcesell(id){
	//$("#"+id+"ismon").val(0);
	if($("#"+id+"monsell").hasClass("off")){
		sendWsMsg(getWsMsg("forcesell", id));
	}else if($("#"+id+"force_sell").hasClass("off")){
		//sendWsMsg(getWsMsg("delete", id));
		sendWsMsg(getWsMsg("stopsell", id));
	}else{
		$("#"+id+"force_sell").addClass("off");
		setSell(id);
	}
}

function clickAS(id){
	$("#"+id+"ismon").val(0);
	var idArr = id.split("_");
	if($("#"+id+"as").hasClass("off")){
		//sendWsMsg(getWsMsg("delete", id));
		sendWsMsg(getWsMsg("stop", id));
	}else{
		sendWsMsg(getWsMsg("auto", id));
	}
}

function isWarrant(code){
	if((code>=10000 && code<30000) || (code>=89000 && code<90000) || (code*1>=50000 && code*1<70000)){
		return true;
	}else{
		return false;
	}
	//return true;
}

function isUnderlying(ucode){
	if((ucode>0 && ucode<10000)){
		return true;
	}else if(ucode.length==5){
		var ucode2 = ucode.substring(0,3).toUpperCase();
		var fcode = ucode.substring(3,4).toUpperCase();
		var year = ucode.substring(4,5);
		if((ucode2 == "HSI" || ucode2 == "HHI") && futureCode(fcode) && yearCode(year)){
			return true;
		}
	}
	//console.log(ucode);
	return false;
}

function isIndex(ucode){
	if((ucode*1>0)){
		return false;
	}else{
		return true;
	}
}

function isRatio(val){
	if(val*1>=0 && val*1<=99 && Number.isInteger(val*1)){
		return true;
	}else{
		return false;
	}
}

function futureCode(b){
	var a = Array("F","G","H","J","K","M","N","Q","U","V","X","Z"); //1 to 12 月
	if($.inArray(b, a) !== -1){
		return true;
	}else{
		return false;
	}
}

function yearCode(b){
	var a = Array("0","1","2","3","4","5","6","7","8","9"); //2020年--> 0, 2021年--> 1
	if($.inArray(b, a) !== -1){
		return true;
	}else{
		return false;
	}
}

function updateFactor(ucode){
	if(isNaN(ucode*1) || ucode*1==0){
		factor = factor_index;
	}else{
		factor = factor_stock;
	}
	
}

function getWsStatus(id, msg, type, bg, status){//console.log("getWsStatus:"+id+"--"+msg+"--"+type+"--"+bg+"--"+status);
	var result = "";
	msg=msg.trim();
	if(msg.indexOf("Invalid Underlying Code")>-1 || msg.indexOf("Invalid Code-ucode")>-1){
		result = "股碼";
	}else if(msg.indexOf("Invalid Underlying Price")>-1){
		result = "股價";
	}else if(msg.indexOf("Invalid Warrant Code")>-1 || msg.indexOf("Invalid Code-code")>-1 || msg.indexOf("underlying code not found for warr")>-1){
		result = "證碼";
	}else if(msg.indexOf("Invalid Warrant Price")>-1 || msg.indexOf("Invalid Warrant Best Bid")>-1 || msg.indexOf("Invalid Warrant Best Ask")>-1 || msg.indexOf("Invalid Order Price")>-1 || msg.indexOf("Invalid order price")>-1){
		result = "證價";
	}else if(msg.indexOf("Invalid Quantity")>-1 || msg.indexOf("Sell Qty Exceed O/S Position")>-1){
		result = "證量";
	}else if(msg.indexOf("Invalid Order No.")>-1){
		result = "證單";
	}else if(msg.indexOf("Invalid Lot size")>-1 || msg.indexOf("Quantity is not a multiple of lot size")>-1){
		result = "手數";
	}else if(msg.indexOf("Sent To EXCH")>-1){
		result = "出盤";
	}else if(msg.indexOf("Invalid Action")>-1){
		result = "動作";
	}else if(msg.indexOf("Unexpected Error")>-1){
		result = "系統";
	}else if(msg.indexOf("Exceed Throttle Limit")>-1){
		result = "節流";
	}else if(msg.indexOf("Insufficient Fund")>-1 || msg.indexOf("Exceed Buy Power")>-1){
		result = "金額";
	}else if(msg.indexOf("Underlying/Warrant Pair Not Matched")>-1){	//證標的不符
		result = "標的";
	}
	
	if(result==""){
		if(status == "reject"){
			result = "拒絕";
		}else if(status == "cancel"){
			result = "取消";
		}
	}
	
	$("#"+id+"buy_status").removeClass("bg_red bg_green bg_orange bg_yellow");
	$("#"+id+"buy_status").html("");
	$("#"+id+"sell_status").removeClass("bg_red bg_green bg_orange bg_yellow");
	$("#"+id+"sell_status").html("");
	
	if(bg!=""){
		$("#"+id+type+"_status").addClass(bg);
	}
	//console.log("getWsStatus:"+result);
	if(result!=""){
		$("#"+id+type+"_status").html(result);
	}
}

//------------------------------------old code--------------------------------------------------------------
function updatePriceTable(id){
	var priceContent = '';
	priceContent += '<div class="tl">對價表</div>';
	priceContent += '<div align="right" class="timer"><span id="'+id+'tip_time"></span></div>';
	priceContent += '<div class="clear"></div>';
	priceContent += '<div class="table">';
	priceContent += '<table width="100%" border="0" cellspacing="0" cellpadding="0">';
	priceContent += '<thead>';
	priceContent += '<tr>';
	priceContent += '<th>股買入價</th>';
	priceContent += '<th>股賣出價</th>';
	priceContent += '<th>證買入價</th>';
	priceContent += '<th>證賣出價</th>';
	priceContent += '</tr>';
	priceContent += '</thead>';
	priceContent += '<tbody id="'+id+'tip_body">';
	priceContent += '</tbody>';
	priceContent += '</table>';
	priceContent += '</div>';
	
	$("#tip_"+id).html(priceContent);
	
	$("#"+id+"tip").click(function(){
		if(isWarrant($("#"+id+"code").val()*1)){
			if($("#"+id+"error").val()==0){
				if($("#"+id+"orderno").val()!="-1"){
					sendWS(getMsg("55", id));
				}else{
					sendWS(getMsg("54", id));
				}
			}
			/*$.ajax({url: "https://secure.dbpower.com.hk/hts/hts_pricetable.php", type: "POST", data: {code: $("#"+id+"code").val()}, cache: false, success: function(data) {
				if(data["p"]!=""){
					$("#"+id+"tip_time").html(data["p"]);
					var tip_body="";
					for(var i = 0; i < data.tr.length; i++) {
						var obj = data.tr[i];
						tip_body+="<tr><td>"+obj.td[0]+"</td><td>"+obj.td[1]+"</td><td>"+obj.td[2]+"</td><td>"+obj.td[3]+"</td></tr>";
					}
					$("#"+id+"tip_body").html(tip_body);
				}else{
					$("#"+id+"tip_time").html("");
					$("#"+id+"tip_body").html("");
				}
				$("#"+id+"tip2").focus();
			}});*/
		}else{
			$("#"+id+"tip_time").html("-");
			$("#"+id+"tip_body").html("");
		}
	});
	
	$("#"+id+"tip").mouseout(function(){
		$("#"+id+"tip2").blur();
	});
	
	$("#"+id+"tip2").click(function(){
		if($("#tooltip").length>0){
			$("#"+id+"tip2").hide();
			$("#"+id+"tip").show();
		}
	});
		
	/*$("#"+id+"tip2").mouseout(function(){
		$("#"+id+"tip2").hide();
		$("#"+id+"tip").show();
	});*/
}

function hideTooltip(){
	$(".tip2").hide();
	$(".tip").show();
}

function removeCode(id, count){
	var count1 = $("#"+id+"count1").val()*1;
	var count2 = $("#"+id+"count2").val()*1;
	
	if(count1>0){
		if($("#"+id+"_"+count+"init").val()*1>0){
			sendWS(getMsg("33", id+"_"+count));
		}
		count1--;
		$("#"+id+"count1").val(count1);
		$("#"+id+"_"+count).remove();
		
		if(count1<codeNum){
			$("#"+id+"add").show();
		}
	}
}

function buyAll(id){
	$("tr."+id+"c").each(function(){
		var id2 = $(this).attr("id");
		if(!$("#"+id2+"monbuy").hasClass("off") && !$("#"+id2+"monbuy").hasClass("disable")){
			buy(id2);
		}
	});
}

function forcebuyAll(id){
	$("tr."+id+"c").each(function(){
		var id2 = $(this).attr("id");
		if(!$("#"+id2+"force_buy").hasClass("off") && !$("#"+id2+"force_buy").hasClass("disable")){
			forcebuy(id2);
		}
	});
}

function sellAll(id){
	$("tr."+id+"c").each(function(){
		var id2 = $(this).attr("id");
		if(!$("#"+id2+"monsell").hasClass("off") && !$("#"+id2+"monsell").hasClass("disable")){
			sell(id2);
		}
	});
}

function forcesellAll(id){
	$("tr."+id+"c").each(function(){
		var id2 = $(this).attr("id");
		if(!$("#"+id2+"force_sell").hasClass("off") && !$("#"+id2+"force_sell").hasClass("disable")){
			forcesell(id2);
		}
	});
}

function stopAll(){
	//sendWS(getMsg("24", ""));
	for(var i=0; i<ucodeNum; i++){
		var id = 'u'+adddigit(i);
		var id2 = id+"_1";
		
		if($("#"+id2+"ismon").val()==1){
			sendWsMsg(getWsMsg("delete", id2));
		}else if($("#"+id2+"ismon").val()==2){
			sendWsMsg(getWsMsg("forcesell", id2));
		}
	}
}

function inputSelectBlur(){
	$("input").click(function(){
		$(this).select();
	});
	
	$("input").mouseover(function(){
		//$(this).select();
		$(this).addClass("over");
	});
	
	$("input").mouseout(function(){
		//$(this).blur();
		$(this).removeClass("over");
	});
}

function overInput(id, checkBS=0){
	$("#"+id).mouseover(function() {
		overAction(id, checkBS);
	});
	
	$("#"+id).mouseout(function() {
		$("#"+id).unbind("keypress");
	});
}

function overCheck(id, checkBS=0){
	//if ($("#"+id).is(':hover')) {
	if ($("#"+id+":hover").length > 0 || $("#"+id).hasClass("over")) {
		overAction(id, checkBS);
	}
}

function overAction(id, checkBS=0){
	$("#"+id).bind("keypress", function( event ) {
		keyAction(id, checkBS);
	});
}

function keyAction(id, checkBS=0){  //checkBS --> 1: spread+getbid/getask 	2: mon
	//if(mousedown){
		if($("#"+id).val()!="" && $("#"+id).val()*1!=0 && checkBS==1){
			//console.log("keyAction_"+id);
			if ( event.key == "q" || event.key == "Q" ) {
				$("#"+id).val(($("#"+id).val()*1+getSpread($("#"+id).val()*1+0.0001)).toFixed(3)*1);
			}else if ( event.key == "w" || event.key == "W" ) {
				$("#"+id).val(($("#"+id).val()*1-getSpread($("#"+id).val()*1-0.0001)).toFixed(3)*1);
			}
		}else{
			//$("#"+id).val($("#"+id).val()*1);
		}
		
		var id2 = $("#"+id).closest("tr").attr("id");
		var idArr = id2.split("_");
		if(isUnderlying($("#"+idArr[0]+"ucode").val()) && $("#"+id2+"code").val()*1>0 && checkBS==1){
			if ( event.key == "a" || event.key == "A" ) {
				//$("#"+id).val((getBidAsk("bid")*1).toFixed(3)*1);
				sendWsMsg(getWsMsg("getbid", id));
			}else if ( event.key == "s" || event.key == "S" ) {
				//$("#"+id).val((getBidAsk("ask")*1).toFixed(3)*1);
				sendWsMsg(getWsMsg("getask", id));
			}
		}
		
		/*if ( event.key == "e" || event.key == "E" ){
			if(($("#"+id2+"monbuy").hasClass("off") || !$("#"+id2+"monbuy").hasClass("disable")) && $("#"+id2+"t_vol").val()==0){
				clickBuy(id2);
			}else if(($("#"+id2+"monsell").hasClass("off") || !$("#"+id2+"monsell").hasClass("disable")) && $("#"+id2+"t_vol").val()>0){
				clickSell(id2);
			}
		}
		
		if ( event.key == "r" || event.key == "R" ){
			if(($("#"+id2+"force_buy").hasClass("off") || !$("#"+id2+"force_buy").hasClass("disable")) && $("#"+id2+"t_vol").val()==0){
				clickForcebuy(id2);
			}else if(($("#"+id2+"force_sell").hasClass("off") || !$("#"+id2+"force_sell").hasClass("disable")) && $("#"+id2+"t_vol").val()>0){
				clickForcesell(id2);
			}
		}
		
		if ( event.key == "t" || event.key == "T"){
			if(($("#"+id2+"as").hasClass("off") || !$("#"+id2+"as").hasClass("disable")) && $("#"+id2+"t_vol").val()==0){
				clickAS(id2);
			}
		}*/
	 //}
}

function removeString(id){
	$("#"+id).keyup(function( event ) {
		//$("#"+id).val($("#"+id).val().replace(/\D/gi,""));
		//$("#"+id).val(/[-]?\d+[.]?\d*/.exec($("#"+id).val()));
		var val = $("#"+id).val();
		var str = val.substr(val.length-1);
		//if(str == "+" || str == "-" || str == "[" || str == "]"){
		if(str == "q" || str == "w" || str == "a" || str == "s" || str == "Q" || str == "W" || str == "A" || str == "S" || str == "e" || str == "r" || str == "t" || str == "E" || str == "R" || str == "T"){
			$("#"+id).val(val.substr(0,val.length-1));
		}
	});
}

function stopAutoBuy(id, byset){//byset=2: error call (cancel, reject)
	var idArr = id.split("_");
	if($("#"+id+"ismon").val()!=0){
		sendWsMsg(getWsMsg("stop", id));
	}
	$("#"+id+"ismon").val(0);
	if(byset!="2"){
		$("#"+id+"buy_status").removeClass("bg_red bg_green bg_orange bg_yellow");
		$("#"+id+"buy_status").html("");
	}
	$("#"+id+"monbuy").removeClass("off disable");
	$("#"+id+"as").removeClass("off disable");
	$("#"+id+"force_buy").removeClass("off disable");
	$("#"+idArr[0]+"ucode").removeAttr('readonly');
	$("#"+id+"code").removeAttr('readonly');
	if( byset!="2"){
		setBuy(id);
	}
}

function stopBuy(id, byset){//byset=1: setBuy call, byset=2: error call (cancel, reject)
	var idArr = id.split("_");
	if($("#"+id+"ismon").val()!=0){
		sendWsMsg(getWsMsg("stop", id));
	}
	//$("#"+id+"ismon").val(0);
	
	if(byset!="2"){
		$("#"+id+"buy_status").removeClass("bg_red bg_green bg_orange bg_yellow");
		$("#"+id+"buy_status").html("");
	}
	
	if(isRecovery || byset!="1"){
		$("#"+id+"monbuy").removeClass("off disable");
		$("#"+id+"as").removeClass("off disable");
		$("#"+id+"force_buy").removeClass("off disable");
		$("#"+idArr[0]+"ucode").removeAttr('readonly');
	}
		
	$("#"+id+"code").removeAttr('readonly');
	$("#"+id+"remove_btn").show();
	//$("#"+id+"status").text(htsstatus[1]);
	
	if(byset!="1" && byset!="2"){
		setBuy(id);
	}else{
		asBtn(id);
	}
}

function asBtn(id){
	var idArr = id.split("_");
	var ucode = $("#"+idArr[0]+"ucode").val();
	var code = $("#"+id+"code").val()*1;
	var vol = getValue($("#"+id+"vol").val());
	var sell_last = $("#"+id+"sell_last").val()*1;
	var sell_ulast = $("#"+id+"sell_ulast").val()*1;
	var bottom = $("#"+id+"bottom").val()*1;
	var ceiling = $("#"+id+"ceiling").val()*1;
	
	if($("#"+id+"force_buy").hasClass("disable") && $("#"+id+"monbuy").hasClass("disable")){
		$("#"+id+"as").addClass("disable");
	}else if(($("#"+id+"force_buy").hasClass("off") || $("#"+id+"buy").hasClass("off")) && !$("#"+id+"as").hasClass("off")){
		$("#"+id+"as").addClass("disable");
	}else if(isUnderlying(ucode) && code>0 && vol!="" && sell_last>0 && sell_ulast>0 && ceiling>0){
		$("#"+id+"as").removeClass("disable");//console.log("asBtn");
		
		/*if($("#"+id+"as").hasClass("off")){
			sendWS(getMsg("42", "AS click (on): "+ucode+" "+code+" ("+id+")"));
		}*/
	}else{
		if($("#"+id+"as").hasClass("off")){
			$("#"+id+"as").removeClass("off");
			//sendWS(getMsg("42", "AS auto (off): "+ucode+" "+code+" ("+id+")"));
			$("#"+id+"as").addClass("disable");
		}
	}
}

function stopSell(id){
	var t_vol = getValue($("#"+id+"t_vol_i").val());
	var idArr = id.split("_");
	if($("#"+id+"ismon").val()!=0){
		sendWsMsg(getWsMsg("stopsell", id));
	}
	//$("#"+id+"ismon").val(0);
	$("#"+id+"sell_status").removeClass("bg_red bg_green bg_orange bg_yellow");
	$("#"+id+"sell_status").html("");
	if(t_vol*1>0){
		$("#"+id+"sell_status").addClass("bg_orange");
	}else{
		$("#"+idArr[0]+"ucode").removeAttr('readonly');
		$("#"+id+"code").removeAttr('readonly');
		$("#"+id+"remove_btn").show();
		sendWsMsg(getWsMsg("stopsell", id));
		//setBuy(id);
	}
	$("#"+id+"monsell").removeClass("off");
	$("#"+id+"monsell").removeClass("orange");
	$("#"+id+"force_sell").removeClass("off");
	//$("#"+id+"status").text(htsstatus[3]);
}

function getValue(val){
	if(val==""){
		return 0;
	}
	val = (""+val).trim();

	var patt = new RegExp("^[0-9]{1,9}(\.[0-9]{1,3})?$|^[0-9]{1,2}(\.[0-9]{1,3})?[mM]?$|^[0-9]{1,5}(\.[0-9]{1,3})?[kK]?$");
	var ret = patt.test(val);

	if(!ret)
	{
		return 0;
	}

	var unit = val.substring(val.length-1);
	if(unit=="k" || unit=="K"){
		val = val.substring(0, val.length-1)*1000;
	}else if(unit=="m" || unit=="M"){
		val = val.substring(0, val.length-1)*1000000;
	}
	
	return val;
}

function formatValue(val){
	if(val*1>=1000000){
		val /= 1000000;
		val += "M";
	}
	else if(val*1>=1000){
		val /= 1000;
		val += "K";
	} 
	return val;
}

function allOverInput(id){
	overInput(id+"bottom", 1);
	overInput(id+"ceiling",1);
	overInput(id+"sell_last", 1);
	overInput(id+"sell_ulast", 1);
	overInput(id+"buy_ulast", 1);
	overInput(id+"buy_last", 1);
	
	/*overInput(id+"vol", 2);
	overInput(id+"buy_vol", 2);
	overInput(id+"buy_ratio", 2);
	overInput(id+"sell_vol", 2);
	overInput(id+"sell_ratio", 2);*/
}

function allOverCheckallOverCheck(id){
	overCheck(id+"bottom", 1);
	overCheck(id+"ceiling", 1);
	overCheck(id+"sell_last", 1);
	overCheck(id+"sell_ulast", 1);
	overCheck(id+"buy_ulast", 1);
	overCheck(id+"buy_last", 1);
	
	/*overCheck(id+"vol", 2);
	overCheck(id+"buy_vol", 2);
	overCheck(id+"buy_ratio", 2);
	overCheck(id+"sell_vol", 2);
	overCheck(id+"sell_ratio", 2);*/
}

function allBind(id){
	$("#"+id+"bottom").bind("keypress", function( event ) {
		keyAction(id+"bottom", 1);
	});
	$("#"+id+"ceiling").bind("keypress", function( event ) {
		keyAction(id+"ceiling", 1);
	});
	$("#"+id+"sell_last").bind("keypress", function( event ) {
		keyAction(id+"sell_last", 1);
	});
	$("#"+id+"sell_ulast").bind("keypress", function( event ) {
		keyAction(id+"sell_ulast", 1);
	});
	$("#"+id+"buy_ulast").bind("keypress", function( event ) {
		keyAction(id+"buy_ulast", 1);
	});
	$("#"+id+"buy_last").bind("keypress", function( event ) {
		keyAction(id+"buy_last", 1);
	});
	
	/*$("#"+id+"vol").bind("keypress", function( event ) {
		keyAction(id+"vol", 2);
	});
	$("#"+id+"buy_vol").bind("keypress", function( event ) {
		keyAction(id+"buy_vol", 2);
	});
	$("#"+id+"buy_ratio").bind("keypress", function( event ) {
		keyAction(id+"buy_ratio", 2);
	});
	$("#"+id+"sell_vol").bind("keypress", function( event ) {
		keyAction(id+"sell_vol", 2);
	});
	$("#"+id+"sell_ratio").bind("keypress", function( event ) {
		keyAction(id+"sell_ratio", 2);
	});*/
}

function allUnbind(id){
	$("#"+id+"bottom").unbind("keypress");
	$("#"+id+"ceiling").unbind("keypress");
	$("#"+id+"sell_last").unbind("keypress");
	$("#"+id+"sell_ulast").unbind("keypress");
	$("#"+id+"buy_ulast").unbind("keypress");
	$("#"+id+"buy_last").unbind("keypress");
	
	/*$("#"+id+"vol").unbind("keypress");
	$("#"+id+"buy_vol").unbind("keypress");
	$("#"+id+"buy_ratio").unbind("keypress");
	$("#"+id+"sell_vol").unbind("keypress");
	$("#"+id+"sell_ratio").unbind("keypress");*/
}

function allremoveString(id){
	removeString(id+"bottom");
	removeString(id+"ceiling");
	removeString(id+"sell_last");
	removeString(id+"sell_ulast");
	removeString(id+"buy_ulast");
	removeString(id+"buy_last");
	
	/*removeString(id+"vol");
	removeString(id+"buy_vol");
	removeString(id+"buy_ratio");
	removeString(id+"sell_vol");
	removeString(id+"sell_ratio");*/
}

function adddigit(number){
	if (isNaN(number*1)){
		return number;
	}else{
		var str = '' + number;
		while (str.length  < 3) {
			str = '0' + str;
		}
		return str;
	}
}

function getSpread(val){
	var spread = 0;
	if(val*1>=0.01 && val*1<0.25){
		spread = 0.001;
	}else if(val*1>=0.25 && val*1<0.5){
		spread = 0.005;
	}else if(val*1>=0.5 && val*1<10){
		spread = 0.01;
	}else if(val*1>=10 && val*1<20){
		spread = 0.02;
	}else if(val*1>=20 && val*1<100){
		spread = 0.05;
	}else if(val*1>=100 && val*1<200){
		spread = 0.1;
	}else if(val*1>=200 && val*1<500){
		spread = 0.2;
	}else if(val*1>=500 && val*1<1000){
		spread = 0.5;
	}else if(val*1>=1000 && val*1<2000){
		spread = 1;
	}else if(val*1>=2000 && val*1<5000){
		spread = 2;
	}else if(val*1>=5000){
		spread = 5;
	}else{
		spread = 0;
	}
	return spread;
}

function getValBySpread(val, x){
	if(x*1==0 || x==""){
		return val;
	}
	
	var direction = "up";
	if(x<0){
		direction = "down";
		x = x*(-1);
	}
	
	var result = val*1;
	var spread = 0;
	for(var i=0; i<x; i++){
		//spread = getSpread(result);
		if(direction == "up"){
			result+=getSpread(result+0.0001);
		}else{
			result-=getSpread(result-0.0001);
		}
	}
	return (result.toFixed(3))*1;
}
function save(){
	if(confirm("確定變更？")){
		if(ls){
			/*for(var i=0; i<putNum; i++){
				var id = 'p'+adddigit(i);
				saveItem(id);
			}
			
			for(var i=0; i<callNum; i++){
				var id = 'c'+adddigit(i);
				saveItem(id);
			}*/
			
			localStorage[wsuser+"_default_tick_bottom"]=$("#default_tick_bottom").val();
			localStorage[wsuser+"_default_tick_ceiling"]=$("#default_tick_ceiling").val();
			localStorage[wsuser+"_default_tick_out"]=$("#default_tick_out").val();
			localStorage[wsuser+"_default_sell_ulast"]=$("#default_sell_ulast").val();
			
			$('.ismon').each(function(i, obj) {
				for(var i=0; i<ucodeNum; i++){
					var id = 'u'+adddigit(i);
					var id2 = id+"_1";
					
					if($("#"+id2+"ismon").val()==0){
						var sign=1;
						if($("#"+id2+"buy_ulast").val()*1>0){
							$("#"+id2+"sell_ulast").val(getValBySpread($("#"+id2+"buy_ulast").val(), $("#default_sell_ulast").val()*sign));
						}
						if($("#"+id2+"buy_last").val()*1>0){
							$("#"+id2+"sell_last").val(getValBySpread($("#"+id2+"buy_last").val(), $("#default_tick_out").val()*sign));
							$("#"+id2+"bottom").val(getValBySpread($("#"+id2+"buy_last").val(), $("#default_tick_bottom").val()*sign));
							$("#"+id2+"ceiling").val(getValBySpread($("#"+id2+"buy_last").val(), $("#default_tick_ceiling").val()*sign));
						}
						
						setBuy(id);
					}
				}
			});
		}
	}else{
		if(ls){
			$("#default_tick_bottom").val(localStorage[wsuser+"_default_tick_bottom"]);
			$("#default_tick_ceiling").val(localStorage[wsuser+"_default_tick_ceiling"]);
			$("#default_tick_out").val(localStorage[wsuser+"_default_tick_out"]);
			$("#default_sell_ulast").val(localStorage[wsuser+"_default_sell_ulast"]);
		}
	}
}

function saveItem(id){
	localStorage[wsuser+"_"+id+"trigger"]=$("#"+id+"trigger").val();
	localStorage[wsuser+"_"+id+"trade"]=$("#"+id+"trade").val();
	localStorage[wsuser+"_"+id+"vol"]=$("#"+id+"vol").val();
	localStorage[wsuser+"_"+id+"bottom"]=$("#"+id+"bottom").val();
	localStorage[wsuser+"_"+id+"ceiling"]=$("#"+id+"ceiling").val();
	localStorage[wsuser+"_"+id+"min_exit"]=$("#"+id+"min_exit").val();
	localStorage[wsuser+"_"+id+"out"]=$("#"+id+"out").val();
	localStorage[wsuser+"_"+id+"in"]=$("#"+id+"in").val();
	localStorage[wsuser+"_"+id+"max_entry"]=$("#"+id+"max_entry").val();
	localStorage[wsuser+"_"+id+"tc"]=$("#"+id+"tc").val();
	if($("#"+id+"as").hasClass("off")){
		localStorage[wsuser+"_"+id+"as"]=1;
	}else{
		localStorage[wsuser+"_"+id+"as"]=0;
	}
}

function load(){
	if (localStorage[wsuser+"_"+id+"_default_tick_bottom"]) {
		$("#default_tick_bottom").val(localStorage[wsuser+"_default_tick_bottom"]);
	}
	if (localStorage[wsuser+"_"+id+"_default_tick_ceiling"]) {
		$("#default_tick_ceiling").val(localStorage[wsuser+"_default_tick_ceiling"]);
	}
	if (localStorage[wsuser+"_"+id+"_default_tick_out"]) {
		$("#default_tick_out").val(localStorage[wsuser+"_default_tick_out"]);
	}
	if (localStorage[wsuser+"_"+id+"_default_sell_ulast"]) {
		$("#default_sell_ulast").val(localStorage[wsuser+"_default_sell_ulast"]);
	}
}

function loadItem(id){
	var ucode_o = $("#"+id+"trigger").val()*1;
	var wcode_o = $("#"+id+"trade").val()*1;
	var ucode_l = 0;
	var wcode_l = 0;
	
	if (localStorage[wsuser+"_"+id+"trigger"]) {
		ucode_l = localStorage[wsuser+"_"+id+"trigger"]*1;
	}
	if (localStorage[wsuser+"_"+id+"trade"]) {
		wcode_l = localStorage[wsuser+"_"+id+"trade"]*1;
	}
	
	if($("#"+id+"init").val()==0 || (ucode_o==ucode_l && wcode_o==wcode_l)){
		if (localStorage[wsuser+"_"+id+"trigger"]) {
			$("#"+id+"trigger").val(localStorage[wsuser+"_"+id+"trigger"]);
		}
		if (localStorage[wsuser+"_"+id+"trade"]) {
			$("#"+id+"trade").val(localStorage[wsuser+"_"+id+"trade"]);
		}
		if (localStorage[wsuser+"_"+id+"vol"]) {
			$("#"+id+"vol").val(localStorage[wsuser+"_"+id+"vol"]);
		}
		if (localStorage[wsuser+"_"+id+"bottom"]) {
			$("#"+id+"bottom").val(localStorage[wsuser+"_"+id+"bottom"]);
		}
		if (localStorage[wsuser+"_"+id+"ceiling"]) {
			$("#"+id+"ceiling").val(localStorage[wsuser+"_"+id+"ceiling"]);
		}
		if (localStorage[wsuser+"_"+id+"min_exit"]) {
			$("#"+id+"min_exit").val(localStorage[wsuser+"_"+id+"min_exit"]);
		}
		if (localStorage[wsuser+"_"+id+"out"]) {
			$("#"+id+"out").val(localStorage[wsuser+"_"+id+"out"]);
		}
		if (localStorage[wsuser+"_"+id+"in"]) {
			$("#"+id+"in").val(localStorage[wsuser+"_"+id+"in"]);
		}
		if (localStorage[wsuser+"_"+id+"max_entry"]) {
			$("#"+id+"max_entry").val(localStorage[wsuser+"_"+id+"max_entry"]);
		}
		if (localStorage[wsuser+"_"+id+"tc"]) {
			$("#"+id+"tc").val(localStorage[wsuser+"_"+id+"tc"]);
		}
		if (localStorage[wsuser+"_"+id+"as"] && localStorage[wsuser+"_"+id+"as"]==1) {
			$("#"+id+"as").addClass("off");
		}else{
			$("#"+id+"as").removeClass("off");
		}
		autoBuy(id, "l");
		autoSell(id, "l");
	}
}

function sendHeartbeat(){
	setInterval(function(){ 
		sendWS(getMsg("40", "")); 
	}, 60000);
}

function updateSetting(id, id2){
	if(confirm("對所有證進行變更？")){
		$("."+id).val($("#default_"+id2).val());
	}
}

function goToPage(type){
	if(type=="portfolios"){
		$("#position_table").html("");
		sendWsMsg(getWsMsg("power", ""));
		//sendWsMsg(getWsMsg("position", ""));
	}else if(type=="journal"){
		$("#journal_table").html("");
		$("#combination_table").html("");
		$("#combination_table_sum_num").val(0);
		$("#combination_table_profit_num").val(0);
		$("#combination_table_sum").html("0");
		$("#combination_table_profit").html("0");
		sendWsMsg(getWsMsg("orders", ""));
	}
	
	$("#page").addClass("journal");
	$("#content_area").hide();
	$("#"+type+"_content").show();
}

function backPage(type){
	$("#page").removeClass("journal");
	$("#"+type+"_content").hide();
	$("#content_area").show();
}

function addcomma(pmsg) {
    var news = "";

    pmsg = pmsg.toString().replace(",", "");
    pmsg = pmsg * 1;

    if (pmsg == 0) {
        return pmsg;
    }

    var sign = "";
    if (pmsg < 0) {
        sign = "-";
        pmsg = Math.abs(pmsg);
    }

    var t = pmsg.toString().split(".");

    if (t[0] !== '') {
        var xx = 0;
        for (var c = (t[0].length - 1); c >= 0; c--) {
            if ((xx % 3 == 0) && (xx > 0)) {
                news = t[0].substr(c, 1) + "," + news;
            } else {
                news = t[0].substr(c, 1) + news;
            }
            xx++;
        }
        if (typeof t[1] != 'undefined') {
            if (t[1] !== '') {
                news = news + "." + t[1];
            }
        }
    }
    return sign + news;
}
function formatProfit(val){
	var profit="";
	if(val>0){
		profit = "<span class='green'>"+addcomma(val.toFixed(4)*1)+"</span>";
	}else if(val<0){
		profit = "<span class='red'>"+addcomma(val.toFixed(4)*1)+"</span>";
	}else{
		profit = "<span class=''>"+val+"</span>";
	}
	return profit;
}