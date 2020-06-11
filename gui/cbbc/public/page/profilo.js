"use strict";

var ui = new Array();

$(document).ready(function() {
  initWebsocket(render);
  initUI();
});

function render(res) {
  initUI();
  var data = JSON.parse(res);
  // 已成功交易
  if(data.action=='on_order' && data.order.status.toLowerCase().indexOf('filled')>=0) {
    renderTable(data); 
    renderInfo(data);
  }
}

function initUI() {
  ui.container = $('#profilo');
  ui.tbody = $('#profilo table tbody');
  ui.cost = $('.cost');
  ui.benefit = $('.benefit');
  ui.roi = $('.roi');
}

var orders = new Array();
function renderTable(data) {
  var key = data.key;
  var ref = data.ref;
  var tm = moment(data.tm).format("YYYY-MM-DD HH:mm:ss");
  
  var order = data.order;
  var code = order.code;
  var price = formatNumber(order.match_price);
  var quantity = formatNumber(order.match_quantity);
  var total = price*quantity;
  var side = order.side.replace("order_side::", "");
  
  // 初始化
  if(!(ref in orders))
    orders[ref] = {buy: new Array(), sell: new Array(), avg: {buy:0, sell:0}};
  
  // 多空
  var c = {price:price, quantity:quantity, total:total};
  if(side.toLowerCase().indexOf('buy')>=0)
    orders[ref].buy.push(c);
  if(side.toLowerCase().indexOf('sell')>=0)
    orders[ref].sell.push(c);
  
  // 平均
  for (var key in orders) {
    var total = null;
    for(var i in orders[key].buy)
      total += orders[key].buy[i].total;
    orders[key].avg.buy = total/orders[key].buy.length;
    
    var total = null;
    for(var j in orders[key].sell)
      total += orders[key].sell[j].total;
    orders[key].avg.sell = total/orders[key].sell.length;
    
    // 总收益
    var yields = orders[key].avg.sell - orders[key].avg.buy;
    if (!isNaN(yields))
      orders[key].avg.yields = yields;
  }
  
  // 渲染
  var html = null;
  for (var key in orders) {
    if(orders[key].avg.yields>0)
      var css_yields = 'text-success';
    else if(orders[key].avg.yields<0)
      var css_yields = 'text-danger';
    else
      var css_yields = 'text-info';
    
    html += 
    '<tr>'+
    '<td>'+key+'</td>'+
    '<td>'+formatPrice(orders[key].avg.buy)+'</td>'+
    '<td>'+formatPrice(orders[key].avg.sell)+'</td>'+
    '<td class="'+css_yields+'">'+formatPrice(orders[key].avg.yields)+'</td>'+
    '</tr>';
  }
  ui.tbody.html(html);
}

var cost=0, benefit=0, roi=0;
function renderInfo(data) {
  var order = data.order;
  var price = formatNumber(order.match_price);
  var quantity = formatNumber(order.match_quantity);
  var total = price*quantity;
  var side = order.side.replace("order_side::", "");
  
  if(side.toLowerCase().indexOf('buy')>=0)
    cost += total;
  if(side.toLowerCase().indexOf('sell')>=0)
    benefit += total;
  roi = (benefit-cost)/cost*100;
  
  // 总成本
  ui.cost.html(formatPrice(cost));

  // 总收益
  ui.benefit.html(formatPrice(benefit-cost));
  ui.benefit.removeClass("text-success text-danger");
  var style = (benefit-cost>0) ? 'text-success' : 'text-danger';
  ui.benefit.addClass(style);
  
  // 收益率
  ui.roi.html(formatPrice(roi)+'%');
  ui.roi.removeClass("text-success text-danger");
  var style = (roi>0) ? 'text-success' : 'text-danger';
  ui.roi.addClass(style);
}