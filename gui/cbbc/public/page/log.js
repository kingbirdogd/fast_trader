"use strict";

var ui = new Array();

$(document).ready(function() {
  hljs.initHighlightingOnLoad();
  var command = '{"type":"list_orders"}';
  initWebsocket(render);
  sendWebsocket(command);
  initUI();
});

function render(res) {
  initUI();
  ui.textarea.append(res + ",\n");
  // 清理
  if(ui.textarea.html().length > 1024*200)
    ui.textarea.empty();
  // 到最底
  // ui.textarea.scrollTop(ui.textarea[0].scrollHeight);
}

function initUI() {
  ui.container = $('#log');
  ui.textarea = $('#log .textarea-log');
}