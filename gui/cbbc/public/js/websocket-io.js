var domain = location.host; // 选择连接的服务器
var ws = null;

function sendWebsocket(data){
  if(!ws){
    console.log("WebSocket还没有建立！不能传送数据！");
    return false;
  }
  
  waitForSocketConnection(ws, function() {
    ws.send(data);
    console.log(JSON.parse(data));
  });
}

function initWebsocket(callback){
  if ("WebSocket" in window){
    if(!ws || ws.readyState == 3){
      ws = new WebSocket("ws://"+domain);
      console.log('建立WebSocket!');
      // 导航栏Tab
      initNavbar();
    }
    
    ws.onopen = function(){
      console.log("已连接！");
    }
    ws.onmessage = function (evt){
      if (typeof callback === "function") 
        callback(evt.data);
      else
        console.log('callback不是回调!');
      // 导航栏Tab
      renderNavTab(evt.data);
      // 储存资料
      initDataInCookies(evt.data);
    }
    ws.onclose = function(){
      console.log("已关闭！");
      initWebsocket(callback);
      console.log("重新连接！");
    };

  }else{
    alert("您的浏览器不支持 WebSocket!");
  }
}

function waitForSocketConnection(socket, callback){
  setTimeout(function(){
    if (socket.readyState === 1) {
      if(callback !== undefined){
        callback();
      }
      return;
    } else {
      waitForSocketConnection(socket,callback);
    }
  }, 1);
};