"use strict";

var ui = new Array();
var cur_algo = null;
var command_list_module = '{"type":"list_module"}';
var command_list_algo = '{"type":"list_algo"}';

window.onload = function() {
  window.location.replace("/cbbc");
};

$(document).ready(function() {
  /*initWebsocket(render);
  sendWebsocket(command_list_algo);
  sendWebsocket(command_list_module);
  initUI();*/
  
  // 指令
  ui.btnSend.click(function(){
    let command = ui.tvCommand.val().replace(/\s/g,'');
    
    if(!cur_algo || !command){
      alert("Please select algo and fill in command.");
      return 0;
    }
    
    if(command.indexOf('{')>-1)
      var _command = command;
    else if(cur_algo && command)
      var _command = '{"type":"algo_command", "key": "'+cur_algo+'", "command": "'+command+'"}';
    console.log(_command);
    sendWebsocket(_command);
    ui.tvCommand.val("");                 // 清空
    ui.tvCommand.attr("placeholder", ""); // 清空
    cur_algo = null;
  });
  
  // 开始
  ui.tableModule.on("click", ".start_algo", function(){
    var module = $(this).data("module");
    var key = $('#'+module+'_key').val().replace(/\s/g,'');
    var param = $('#'+module+'_param').val().replace(/\s/g,'');
    
    if(!key){
      alert("Please fill in key.");
      return 0;
    }

    var command = '{"type":"start_algo","module":"'+module+'","param":"'+param+'","key":"'+key+'"}';
    sendWebsocket(command);
    $('#'+module+'_key').val("");     // 清空
    $('#'+module+'_param').val("");   // 清空
    sendWebsocket(command_list_module);
  });
  
  // 删除
  ui.tableAlgo.on("click", ".stop_algo", function(){
    var key = $(this).data("key");
    sendWebsocket('{"type":"stop_algo","key":"'+key+'"}');
  });
  // 停止
  ui.tableAlgo.on("click", ".pause_algo", function(){
    var key = $(this).data("key");
    console.log('pause: '+key);
  });
  // 停止
  ui.tableAlgo.on("click", ".resume_algo", function(){
    var key = $(this).data("key");
    console.log('resume: '+key);
  });
  // 選擇
  ui.tableAlgo.on("click", ".select_algo", function(){
    cur_algo = $(this).data("key");
    ui.tvCommand.attr("placeholder", cur_algo);
  });
  
});

function initUI(){
  ui.tvCommand = $(".tv-command");
  ui.btnSend = $(".btn-send");
  ui.tableAlgo = $(".table-algo tbody");
  ui.tableCommand = $(".table-command tbody");
  ui.tableModule = $(".table-module tbody");
}

function render(res) {
  initUI();
  var json = JSON.parse(res);
  if(json.action=='algo') // 策略
    render_algo(json);
  else if(json.action=='load' || json.action=='unload') // 开始策略 & 停止策略
    sendWebsocket(command_list_algo);
  else if(json.action=='algo_command') // 指令
    render_command(json);
  else if(json.action=='module') // 库
    render_module(json);
  else if(json.action=='reload') // 载入库
    sendWebsocket(command_list_module);
}

function render_algo(json) {
  var data = json.data;
  var id = json.id;
  var type = json.type;
  var tm = json.tm;
  var html = null;
  
  if(!data)
    html += '<tr><td>Algo</td><td>No Algo</td></tr>';
  else if (typeof data !== 'undefined'){
    for(var i=0; i<data.length; i++){
      var key = data[i];
      var no = i+1;
      html += 
      '<tr><td>'+no+'</td>'+
      '<td>'+key+'</td>'+
      '<td><button type="button" class="stop_algo btn btn-sm btn-danger" data-key="'+key+'" >Stop</button>'+
      '<button type="button" class="pause_algo btn btn-sm btn-primary" data-key="'+key+'" >Pause</button>'+
      '<button type="button" class="resume_algo btn btn-sm btn-info" data-key="'+key+'" >Resume</button>'+
      '<button type="button" class="select_algo btn btn-sm btn-warning" style="display: none;" data-key="'+key+'" >Select</button>'+
      '</td></tr>';
    }
  }
  ui.tableAlgo.html(html);
}

function render_module(json) {
  var action = json.action;
  var data = json.data;
  var id = json.id;
  var type = json.type;
  var tm = json.tm;
  var html = null;
  
  if(data){
    for(var i=0; i<data.length; i++){
      var key = data[i];
      var val = '';
      html += 
      '<tr><td>'+key+'</td>'+
      '<td>'+
      '<div class="form-inline"">'+
      '<div class="input-group"> <input class="form-control form-control-sm" type="text" id="'+key+'_key" value="" placeholder="key"/> </div> &nbsp'+
      '<div class="input-group"> <input class="form-control form-control-sm" type="text" id="'+key+'_param" value="" placeholder="param"/> </div> &nbsp'+
      '<div class="input-group"> <button type="button" class="start_algo btn btn-sm btn-success" data-module="'+key+'">Start</button> </div>'+
      '</div></td></tr>';
    }
    ui.tableModule.html(html);
  }
}

function render_command(json) {
  var action = json.action;
  var command = JSON.parse(json.command);
  var key = command.key;
  var _command = command.command;
  var result = json.result;
  var tm = moment(json.tm).format("YYYY-MM-DD HH:mm:ss");
  html = "<tr> <td>"+tm+"</td> <td>"+key+"</td><td>"+_command.replace(/#/g, ', ')+"</td> <td class='text-success'>"+result+"</td></tr>";
  ui.tableCommand.prepend(html);
}