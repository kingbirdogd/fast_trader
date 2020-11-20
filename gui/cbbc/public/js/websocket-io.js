var domain = location.host;       // 选择连接的服务器
var href = window.location.href;
if (href.includes('cbbc'))
  domain += '/origin=cbbc'
else if (href.includes('s1algoput'))
  domain += '/origin=s1algoput'
else if (href.includes('a1'))
  domain += '/origin=a1'
else if (href.includes('csalgo'))
  domain += '/origin=csalgo'
else
  domain += '/origin=unknown'
var ws = null;
var numConnect = 0;

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
    }
    
    ws.onopen = function(){
      numConnect = 0;
      console.log("已连接！");
    }
    ws.onmessage = function (evt){
      if (typeof callback === "function") 
        callback(evt.data);
      else
        console.log('callback不是回调!');
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
      numConnect += 1;
      // 超過10次重連
      if (numConnect == 10)
        global.func.logout();
    }
  }, 5);
};