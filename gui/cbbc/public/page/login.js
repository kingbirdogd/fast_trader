"use strict";

var ui = new Array();

window.onload = function() {
  initUI();
};

$(document).ready(function() {
  initUI();

  //
  if (alertMsg.length == 0 || alertMsg=="")
    ui.alert.hide();
  else if (alertMsg.length >= 1 && ui.alert)
    ui.alert.html(alertMsg)
  else if (alertMsg.length >= 1 && !ui.alert)
    alert(alertMsg);
  
  //
  if (origin)
    ui.slOrigin.val(origin);
  
  //
  isShowSubInfo();
  
  //
  ui.slOrigin.change(function() {
    isShowSubInfo();
    
    ui.alert.html('');
    ui.alert.hide();
  });
  
  //
  ui.btnSubmit.click(function() {
    var tvUser = ui.tvUser.val();
    var tvPw = ui.tvPw.val();
    var slOrigin = ui.slOrigin.val();
    
    var hosts = ['semipro', 'cbbc', 'a1', 'csalgo', 's1algoput'];
    if (hosts.includes(slOrigin)) {
      ui.formLogin.attr('action', '/'+slOrigin+'-login');
      ui.formLogin.submit();
    }
    else if (slOrigin=='semi') {
      $.ajax({
        type: 'GET',
        url: 'validate',
        dataType: 'jsonp',
        jsonp: 'callback', 
        data: {user_name: tvUser, password: tvPw},
        success: async function(data) {
          if (data.result == 'fail') {
            ui.alert.html('Username is not authorized.');
            ui.alert.show();
          }
          else if (data.result == 'success') {
            localStorage.setItem("wsuserid", data.uid);
            localStorage.setItem("wsuser", data.uname);
            localStorage.setItem("uid", data.uid);
            localStorage.setItem("uname", data.uname);
            window.location.href = './hts';
          }
        }
      });
    }
  });
});

function render(res) {
  
}

function isShowSubInfo() {
  if (false && ui.slOrigin.val() == 'semipro')
    ui.divSubInfo.show();
  else
    ui.divSubInfo.hide();
}

function initUI() {
  ui.alert = $('.alert-danger');
  ui.tvUser = $('#user_name');
  ui.tvPw = $('#password');
  ui.slOrigin = $('#origin');
  ui.btnSubmit = $('#submit');
  ui.formLogin = $('#form-login');
  ui.divSubInfo = $('.sub-info');
}