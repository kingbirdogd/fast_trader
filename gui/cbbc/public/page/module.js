"use strict";

var ui = new Array();
var html = null;

$(document).ready(function() {
  var command = '{"type":"list_orders"}';
  initUI();
  sendWebsocket(command);
  initWebsocket(render);
});

function render(res) {
  initUI();
  var data = JSON.parse(res);
  // 有交易
  if (data.action=='list_orders' && "ordres" in data && data.ordres.length>0 && module==data.ordres[0].key) {
    var orders = data.ordres;
    for (var i in orders) {
      var code = orders[i].code;
      var key = orders[i].key;
      var price = formatNumber(orders[i].match_price);
      var quantity = formatNumber(orders[i].match_quantity);
      var status = orders[i].status.replace("order_status::", "");
      var reject_reason = orders[i].reject_reason;
      var side = orders[i].side;
      var _tm = orders[i].transaction_tm;
      var tm = _tm.substring(0, 4)+'-'+_tm.substring(4, 6)+'-'+_tm.substring(6, 8)+' '+
                _tm.substring(8, 10)+':'+_tm.substring(10, 12)+':'+_tm.substring(12, 14);
      
      // 操作
      if (side.toLowerCase().indexOf('sell')>=0) {
        var side = global.buy;
        var css_side = 'text-success';
        var total = +(price*quantity);
      }else{
        var side = global.sell;
        var css_side = 'text-danger';
        var total = -(price*quantity);
      }
      
      // 成交额
      if(status=='filled' && total>=0) {
        var total = '+'+formatPrice(total);
        var css_total = 'text-success';
      }
      else if(status=='filled' && total<0) {
        var total = formatPrice(total);
        var css_total = 'text-danger';
      }
      else if(status=='rejected') {
        var total = global.fail;
        var css_total = 'text-info';
      }
      else {
        var total = global.error;
        var css_total = 'text-warning';
      }
      
      // 状态
      if(status.toLowerCase().indexOf('filled')>=0)
        var rejectReason = '';
      else
        var rejectReason = ', '+reject_reason;
      
      // 渲染
      html += 
      '<tr>'+
      '<td>'+code+'</td>'+
      '<td class="'+css_side+'"><strong>'+side+'</strong></td>'+
      '<td>'+price+'</td>'+
      '<td>'+quantity+'</td>'+
      '<td>'+status+''+rejectReason+'</td>'+
      '<td>'+tm+'</td>'+
      '<td class="'+css_total+'"><strong>'+total+'</strong></td>'+
      '</tr>';
    }
    ui.tbody.html(html);
  }
}

function initUI() {
  ui.container = $('#module');
  ui.tbody = $('#module table tbody');
}