var ui = new Array();
var config = ['bull', 'bull', 'bear', 'bear'];    // 预设布局
var lang = 'csalgo-lang'
var userId = 'csalgo-userId'
var underlyingDefault = 'select'

$(document).ready(function() {
  initUI();
});

function initUI() {
  ui.container = $('#csalgo');
  ui.tablePrice = $(".price_table");
}

function initTablePrice() {
  initUI();
  var obj = ui.tablePrice.niceScroll({
    cursorcolor: '#797979',
    autohidemode: true,
    cursorwidth: 8
  });
  
  ui.tablePrice.mouseover(function(){
    ui.tablePrice.getNiceScroll().resize();
  });
}