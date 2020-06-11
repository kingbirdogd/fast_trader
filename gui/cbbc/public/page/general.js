var global = new Array();
global.modules = [];

$(document).ready(function() {
  global.lang = Cookies.get('lang');
  initBtnStopServer();
  changeLanguage();
});

// 关机按钮
function initBtnStopServer() {
  var btn = $('#btn-stop-server');
  // 管理员才能显示按钮
  var uname = Cookies.get('uname');
  if(uname=='admin')
    btn.show();
  else
    btn.hide();
  
  // 关机动作
  btn.click(function() {
    var command = '{"type":"stop"}';
    sendWebsocket(command);
  });
}

// 导航栏
function initNavbar() {
  // 头像
  var personal = $('.nav-personal-info');
  var uname = Cookies.get('uname');
  personal.html(uname);
  
  // 导航栏Tab
  // sendWebsocket('{"type":"list_algo"}');
  
  // 
  $("#navbarContent .navbar-nav").find(".active").removeClass("active");
  var link = window.location.href.split('/')[3];
  var navbar = $('#navbarContent .navbar-nav [data-nav="'+link+'"]');
  navbar.addClass("active");
}

// 导航栏Tab
function renderNavTab(res) {
  var tab = $('.nav-dropdown-module');
  var data = JSON.parse(res);
  if (data.action=='algo' && "data" in data && data.data.length>0) {
    var html = '';
    for (var i=0; i<data.data.length; i++) {
      var module = data.data[i];
      html += '<a class="dropdown-item" href="/module/'+module+'">'+module+'</a>';
      // 公用
      if(!global.modules.includes(module))
        global.modules.push(module)
    }
    tab.html(html);
  }
}

// 储存资料
function initDataInCookies(res) {
  var data = JSON.parse(res);
  if ("user_id" in data) {
    Cookies.set("userId", data.user_id);
  }
}

// 选择语言
function changeLanguage() {
  var btn = $('.btn-lang');
  btn.click(function(e) {
    var lang = $(this).data("lang");
    Cookies.set('lang', lang);
    location.reload();
  });
}

function formatNumber(x) {
  var unit = 100000000; // 单位
  return parseFloat(x)/unit;
}

function formatPrice(x, point) {
  point = (typeof point=='undefined') ? 2 : point;
  var _x = parseFloat(x).toFixed(point);
  if (isNaN(x) || typeof x == 'undefined')
    return '/'
  return _x.toString().replace(/\B(?=(\d{3})+(?!\d))/g, ",");
}

function require(href) {
  var $d = $.Deferred();
  // style
  if(href.includes('.css')) {
    var $link = $('<link/>', {
       rel: 'stylesheet',
       type: 'text/css',
       href: href
    }).appendTo('head');
  }
  // javascript
  else if(href.includes('.js')) {
    var $link = $('<link/>', {
       rel: 'javascript',
       type: 'text/babel',
       href: href
    }).appendTo('head');
  }
  $d.resolve($link);
  return $d.promise();
}