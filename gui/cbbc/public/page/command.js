"use strict";

var ui = new Array();

$(document).ready(function() {
  initWebsocket(render);
  initUI();
});

function render(res) {
  initUI();
}

function initUI() {
  ui.container = $('#command');
}