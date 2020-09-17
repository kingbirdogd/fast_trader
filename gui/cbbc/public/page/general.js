var global = {};

$(document).ready(function() {
  global.cookies = {
    'cbbc-lang': Cookies.get('cbbc-lang'),
    'cbbc-uname': Cookies.get('cbbc-uname'),
    'a1-lang': Cookies.get('a1-lang'),
    'a1-uname': Cookies.get('a1-uname'),
    's1csalgo-lang': Cookies.get('s1csalgo-lang'),
    's1csalgo-uname': Cookies.get('s1csalgo-uname'),
  }
  global.ui = {
    'navbar-a1': $("#navbar-nav-a1"),
    'navbar-cbbc': $("#navbar-nav-cbbc"),
    'navbar-s1csalgo': $("#navbar-nav-s1csalgo"),
    'btn-logout-a1': $("#btn-logout-a1"),
    'btn-logout-cbbc': $("#btn-logout-cbbc"),
    'btn-logout-s1csalgo': $("#btn-logout-s1csalgo"),
    'navber-personal-info': $('.nav-personal-info'),
  };
  global.func = {};
  global.func.logout = function () {
    var href = window.location.href;
    if (href.includes('cbbc'))
      global.ui['btn-logout-cbbc'].submit();
    else if (href.includes('a1'))
      global.ui['btn-logout-a1'].submit();
    else if (href.includes('s1csalgo'))
      global.ui['btn-logout-s1csalgo'].submit();
  }
  
  initNavbar();
  changeLanguage();
});

// 导航栏
function initNavbar() {
  var href = window.location.href;
  // 高亮
  $("#navbarContent .navbar-nav").find(".active").removeClass("active");
  var link = href.split('/')[3];
  var navbar = $('#navbarContent .navbar-nav [data-nav="'+link+'"]');
  navbar.addClass("active");
  
  // 子頁面 & 登出接鈕
  if (href.includes('cbbc')) {
    // 标题
    document.title += ' (CBBC)'
    // 用戶名
    global.ui['navber-personal-info'].html(global.cookies['cbbc-uname']);
    // 顯示
    global.ui['btn-logout-cbbc'].show();
    global.ui['navbar-cbbc'].show();
    // 隱藏
    global.ui['btn-logout-a1'].hide();
    global.ui['navbar-a1'].hide();
    
    global.ui['btn-logout-s1csalgo'].hide();
    global.ui['navbar-s1csalgo'].hide();
  }
  else if (href.includes('a1')) {
    // 标题
    document.title += ' (A1)'
    // 用戶名
    global.ui['navber-personal-info'].html(global.cookies['a1-uname']);
    // 顯示
    global.ui['btn-logout-a1'].show();
    global.ui['navbar-a1'].show();
    // 隱藏
    global.ui['btn-logout-cbbc'].hide();
    global.ui['navbar-cbbc'].hide();
    
    global.ui['btn-logout-s1csalgo'].hide();
    global.ui['navbar-s1csalgo'].hide();
  }
  else if (href.includes('s1csalgo')) {
    // 标题
    document.title += ' (s1csalgo)'
    // 用戶名
    global.ui['navber-personal-info'].html(global.cookies['s1csalgo-uname']);
    // 顯示
    global.ui['btn-logout-s1csalgo'].show();
    global.ui['navbar-s1csalgo'].show();
    // 隱藏
    global.ui['btn-logout-a1'].hide();
    global.ui['navbar-a1'].hide();
    
    global.ui['btn-logout-cbbc'].hide();
    global.ui['navbar-cbbc'].hide();
  }
  else
    console.log('initNavbar error!');
}

// 储存资料
function initDataInCookies(res) {
  var data = JSON.parse(res);
  var href = window.location.href;
  if ("user_id" in data) {
    if (href.includes('cbbc'))
      Cookies.set('cbbc-userId', data.user_id);
    else if (href.includes('a1'))
      Cookies.set('a1-userId', data.user_id);
    else if (href.includes('s1csalgo'))
      Cookies.set('s1csalgo-userId', data.user_id);
  }
}

// 选择语言
function changeLanguage() {
  var btn = $('.btn-lang');
  btn.click(function(e) {
    var lang = $(this).data("lang");
    var href = window.location.href;
    
    if (href.includes('cbbc'))
      Cookies.set('cbbc-lang', lang);
    else if (href.includes('a1'))
      Cookies.set('a1-lang', lang);
    else if (href.includes('s1csalgo'))
      Cookies.set('s1csalgo-lang', lang);
    else
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

function formatCode(num, size) {
  var s = num+"";
  while (s.length < size) s = "0" + s;
  return s;
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