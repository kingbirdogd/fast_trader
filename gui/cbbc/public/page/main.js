var ui = new Array();

$(document).ready(function() {
  initUI();
  
  ui.item.click(function(e) { 
    var origin = $(this).data("origin");
    window.open("/"+origin, '_blank');
  });
});

function initUI() {
  ui.item = $('ul.menu li');
}