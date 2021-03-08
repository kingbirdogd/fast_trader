var global = {};

$(document).ready(function() {
  global.cookies = {
    'cbbc-lang': Cookies.get('cbbc-lang'),
    'cbbc-uname': Cookies.get('cbbc-uname'),
    
    'a1-lang': Cookies.get('a1-lang'),
    'a1-uname': Cookies.get('a1-uname'),
    
    'csalgo-lang': Cookies.get('csalgo-lang'),
    'csalgo-uname': Cookies.get('csalgo-uname'),
    
    's1algoput-lang': Cookies.get('s1algoput-lang'),
    's1algoput-uname': Cookies.get('s1algoput-uname'),
    
    'semipro-lang': Cookies.get('semipro-lang'),
    'semipro-uname': Cookies.get('semipro-uname'),
  }
  global.ui = {
    'navbar-a1': $("#navbar-nav-a1"),
    'navbar-cbbc': $("#navbar-nav-cbbc"),
    'navbar-csalgo': $("#navbar-nav-csalgo"),
    'navbar-s1algoput': $("#navbar-nav-s1algoput"),
    'navbar-semipro': $("#navbar-nav-semipro"),
    
    'btn-logout-a1': $("#btn-logout-a1"),
    'btn-logout-cbbc': $("#btn-logout-cbbc"),
    'btn-logout-csalgo': $("#btn-logout-csalgo"),
    'btn-logout-s1algoput': $("#btn-logout-s1algoput"),
    'btn-logout-semipro': $("#btn-logout-semipro"),
    
    'navber-personal-info': $('.nav-personal-info'),
  };
  global.func = {};
  global.func.logout = function () {
    var href = window.location.href;
    if (href.includes('cbbc'))
      global.ui['btn-logout-cbbc'].submit();
    else if (href.includes('a1'))
      global.ui['btn-logout-a1'].submit();
    else if (href.includes('csalgo'))
      global.ui['btn-logout-csalgo'].submit();
    else if (href.includes('s1algoput'))
      global.ui['btn-logout-s1algoput'].submit();
    else if (href.includes('semipro'))
      global.ui['btn-logout-semipro'].submit();
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
    document.title += ' (CBBC)'
    global.ui['navber-personal-info'].html(global.cookies['cbbc-uname']);
    global.ui['btn-logout-cbbc'].show();
    global.ui['navbar-cbbc'].show();
  }
  else if (href.includes('a1')) {
    document.title += ' (A1)'
    global.ui['navber-personal-info'].html(global.cookies['a1-uname']);
    global.ui['btn-logout-a1'].show();
    global.ui['navbar-a1'].show();
  }
  else if (href.includes('csalgo')) {
    document.title += ' (csalgo)'
    global.ui['navber-personal-info'].html(global.cookies['csalgo-uname']);
    global.ui['btn-logout-csalgo'].show();
    global.ui['navbar-csalgo'].show();
  }
  else if (href.includes('s1algoput')) {
    document.title += ' (s1algoput)'
    global.ui['navber-personal-info'].html(global.cookies['s1algoput-uname']);
    global.ui['btn-logout-s1algoput'].show();
    global.ui['navbar-s1algoput'].show();
  }
  else if (href.includes('semipro')) {
    document.title += ' (hts)'
    global.ui['navber-personal-info'].html(global.cookies['semipro-uname']);
    global.ui['btn-logout-semipro'].show();
    global.ui['navbar-semipro'].show();
  }
  else
    console.log('initNavbar error!');
}

// 储存资料
function initDataInCookies(res) {
  var data = JSON.parse(res);
  var href = window.location.href;
  if ("user_id" in data) {
    if (href.includes('cbbc')) {
      Cookies.set('cbbc-userId', data.user_id);
      global.cookies['cbbc-userId'] = data.user_id;
    }
    else if (href.includes('a1')) {
      Cookies.set('a1-userId', data.user_id);
      global.cookies['a1-userId'] = data.user_id;
    }
    else if (href.includes('csalgo')) {
      Cookies.set('csalgo-userId', data.user_id);
      global.cookies['csalgo-userId'] = data.user_id;
    }
    else if (href.includes('s1algoput')) {
      Cookies.set('s1algoput-userId', data.user_id);
      global.cookies['s1algoput-userId'] = data.user_id;
    }
    else if (href.includes('semipro')) {
      Cookies.set('semipro-userId', data.user_id);
      global.cookies['semipro-userId'] = data.user_id;
    }
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
    else if (href.includes('csalgo'))
      Cookies.set('csalgo-lang', lang);
    else if (href.includes('s1algoput'))
      Cookies.set('s1algoput-lang', lang);
    else if (href.includes('semipro'))
      Cookies.set('semipro-lang', lang);
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