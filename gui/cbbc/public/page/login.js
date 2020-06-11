"use strict";

var ui = new Array();

window.onload = function() {
  initUI();
};

$(document).ready(function() {
  initUI();
  if (alertMsg.length == 0 || alertMsg=="")
    ui.alert.hide();
  else if (alertMsg.length >= 1 && ui.alert)
    ui.alert.html(alertMsg)
  else if (alertMsg.length >= 1 && !ui.alert)
    alert(alertMsg);
});

function render(res) {

}

function initUI() {
  ui.alert = $('.alert-danger');
}