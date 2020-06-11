"use strict";

var ui = new Array();
var fields = {};
var commands = {};

var init_algo = [
  {type: "text", name: 'wtype', value: '购', valid: 'string'},
  {type: "text", name: 'underlyibng_code', value: '股', valid: 'ucode'},
  {type: "text", name: 'warrant_code', value: '证', valid: 'code'},
  {type: "text", name: 'buy_trriger', value: '触发买价(股)', valid: 'number'},
  {type: "text", name: 'sell_trriger', value: '触发卖价(股)', valid: 'number'},
  {type: "text", name: 'buy_price', value: '限买价(证)', valid: 'number'},
  {type: "text", name: 'sell_price', value: '限卖价(证)', valid: 'number'},
  {type: "text", name: 'bottom_price', value: '止损价', valid: 'number'},
  {type: "text", name: 'ceiling_price', value: '止盈价', valid: 'number'},
  {type: "text", name: 'auto_buy_quantity', value: '手数', valid: 'number'},
  {type: "text", name: 'action', value: '模式', valid: 'string'},
  {type: "button", name: 'start', value: '开始', valid: 'null'},
];
fields.满仓反攻_范例不能用 = [
  {type: "text", name: 'wtype', value: '购', valid: 'string'},
  {type: "text", name: 'warrant_code', value: '证', valid: 'code'},
  {type: "text", name: 'buy_price', value: '限买价(证)', valid: 'number'},
  {type: "text", name: 'bottom_price', value: '止损价', valid: 'number'},
  {type: "button", name: 'start', value: '开始', valid: 'null'}
];
fields.多方反攻_范例不能用 = [
  {type: "text", name: 'wtype', value: '购', valid: 'string'},
  {type: "text", name: 'underlyibng_code', value: '股', valid: 'ucode'},
  {type: "text", name: 'warrant_code', value: '证', valid: 'code'},
  {type: "text", name: 'buy_trriger', value: '触发买价(股)', valid: 'number'},
  {type: "text", name: 'buy_price', value: '限买价(证)', valid: 'number'},
  {type: "text", name: 'bottom_price', value: '止损价', valid: 'number'},
  {type: "button", name: 'start', value: '开始', valid: 'null'}
];
fields.自定義_001_范例不能用 = [];
fields.自定義_002_范例不能用 = [];
fields.自定義_003_范例不能用 = [];

$(document).ready(function() {
  initUI();
});

function initUI() {
  
}