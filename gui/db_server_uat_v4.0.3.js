/*
 * IO设置端口
 */
if (3 !== process.argv.length)
{
	console.error("usage: node server <port,1-65535>");
	process.exit(-1);
}
let port = parseInt(process.argv[2]);
if (port < 1 || port > 65535)
{
	console.error("usage: node server <port,1-65535>");
	process.exit(-2);
}



/*
 * package
 */
const os = require('os');
const readline = require("readline");
const ws = require("websocket").server;
const http = require("http");
const fs = require("fs");
const read = require('fs').readFileSync;
const mime = require(__dirname + "/mime.json");
const Router  = require('router');
const finalhandler = require('finalhandler');
const compression = require('compression');
const bodyParser = require('body-parser');
const cookieParser = require('cookie-parser');
const crypto = require("crypto");
const ejs = require('ejs');
const querystring = require('querystring');
const express = require('express');
const path = require('path');
const i18n = require("i18n");
const router = Router();

i18n.configure({
  locales:['en', 'tc', 'sc'],
  defaultLocale: 'en',
  cookie: 'lang',
  queryParameter: 'lang',
  directory: __dirname + '/cbbc/locales'
});

var app = express();
app.set('view engine', 'ejs');

app.use(bodyParser.json());
app.use(bodyParser.urlencoded({ extended: false }));
app.use(cookieParser());
app.use(compression());
app.use(i18n.init);
app.use(express.static(path.join(__dirname, 'cbbc/public')));
app.use(express.static(path.join(__dirname, 'hts')));

var old_msg = [];                           // recover message
var conns = {};                             // websocket
var homedir = os.homedir();                 // path for save log



/*
 * 用户验证
 */
var sessions = {};  // cookies里的uid
var users = require(__dirname + "/user.js").load();
for (let user in users)
{
	let node = users[user];
	let id = node.id;
}


/*
 * 工具类
 */
var getDate = function() {return new Date().getFullYear().toString()+pad2(new Date().getMonth()+1)+pad2(new Date().getDate())}
var getDate2 = function() {return new Date().getFullYear().toString()+'-'+pad2(new Date().getMonth()+1)+'-'+pad2(new Date().getDate())}
var getTime = function() {return pad2(new Date().getHours())+':'+pad2(new Date().getMinutes()+1)+':'+pad2(new Date().getSeconds())}
var getUserName = function(id) { for (var name in users) { if (id && parseInt(users[name].id) == parseInt(id)) { return name } } return 'unnamed' }

var get_cookies = function (req)
{
	let cookies = {};
	let cookie_str = req.headers.cookie;
	if (undefined !== cookie_str)
	{
		let cookie_list = cookie_str.split(";");
		for (let i in cookie_list)
		{
			let cookie_pair_str = cookie_list[i];
			cookie_pair_str = cookie_pair_str.trim();
			let cookie_pair = cookie_pair_str.split("=");
			if (2 == cookie_pair.length)
			{
				cookies[cookie_pair[0]] = cookie_pair[1];
			}
		}
	}
	return cookies;
};
// 提取独立uid
var get_uid = function(req)
{
	let cookies = get_cookies(req);
  
  if (req.url.includes('cbbc'))
    var cookies1 = 'cbbc-uid'
  else if (req.url.includes('a1'))
    var cookies1 = 'a1-uid'
  else if (req.url.includes('csalgo'))
    var cookies1 = 'csalgo-uid'
  else
    var cookies1 = 'uid'
  
	if (undefined === cookies[cookies1])
		return {uid: undefined, id: undefined, origin: undefined};
	else
	{
		let session = sessions[cookies[cookies1]];
		if (undefined === session)
			return {uid: undefined, id: undefined, origin: undefined};
		else
			return {uid: cookies[cookies1], id: session.userId, origin: session.origin};
	}
};
var get_uid2 = function(req)
{
  if (undefined == req || undefined == req.path)
    return {uid: undefined, id: undefined, origin: undefined};
  else
  {
    var uid = req.path.replace('/uid=','');
    var session = sessions[uid];
		if (undefined === session || undefined === uid || uid == '' || uid.toString().length == 0 || uid == 'null')
			return {uid: undefined, id: undefined, origin: undefined};
		else
			return {uid: uid, id: session.userId, origin: session.origin};
  }
}
// 清除sessions
function wait(ms)
{
  return new Promise(r => setTimeout(r, ms));
}
var logout = async function(req)
{
  if (req.originalUrl.includes('cbbc'))
    var cookies1 = 'cbbc-uid'
  else if (req.originalUrl.includes('a1'))
    var cookies1 = 'a1-uid'
  else if (req.originalUrl.includes('csalgo'))
    var cookies1 = 'csalgo-uid'
  else
    var cookies1 = 'uid'
  
	let cookies = get_cookies(req);
	if (undefined === cookies[cookies1] || !(cookies[cookies1] in sessions))
		return false;
	else
	{
    var id = sessions[cookies[cookies1]].userId;
		if (undefined !== id)
		{
      saveLog(getUserName(id), 'delete session ('+sessions[cookies[cookies1]].origin+').');
      await wait(4000);
			delete sessions[cookies[cookies1]];
			return true;
		}
		else
		{
			return false;
		}
	}
};
var logout2 = async function(uid)
{
  if (!(uid in sessions))
    return false
  else
  {
    saveLog(getUserName(sessions[uid].userId), 'delete session ('+sessions[uid].origin+').');
    await wait(4000);
    delete sessions[uid];
    return true;
  }
};
// 返回uid给服务器端
var login = function(req, user, password, origin)
{
	if (undefined !== users[user])
	{
		var encryptor = require(__dirname + "/password.js");
		if (users[user].password === encryptor.encrypt(password))
		{
      // 安全token
			let cookie_str = crypto.randomBytes(256).toString("hex");
      // 优先级
      var curPriority = 0;
      for (var uid in sessions)
      {
        if (sessions[uid].userId == users[user].id && sessions[uid].priority >= curPriority)
          curPriority = sessions[uid].priority;
      }
      // 认证实体
			sessions[cookie_str] = {
        userId: users[user].id,
        origin: origin,
        priority: curPriority+1,
        loginTime: getDate2()+' '+getTime()
      };
			return cookie_str;
		}
		else
			return "";
	}
};
// 錯誤信息
var getErrorMsg = function(err) {return JSON.stringify({action: 'connect_reject', error: err, stime: getTime()});}
// 儲存日志 sessions log
var pad2 = function(n) { return n < 10 ? '0' + n : n }
var padno = function(n, m) { var len = n.toString().length; if(len<m) {for(var i=0; i<m-len; i++) {n+=" ";}} return n; }
var file = homedir+'/log/node_'+getDate()+'.log';
var saveLog = function(data1, data2, data3, data4) {
  var data1 = data1 !== undefined ? padno(data1, 30) : padno('', 30),
      data2 = data2 !== undefined ? padno(data2, 40) : padno('', 40),
      data3 = data3 !== undefined ? padno(data3, 40) : padno('', 40);
      data4 = data4 !== undefined ? padno(data3, 40) : padno('', 40);
  if (!fs.existsSync(file))
    fs.writeFile(file, '', (err) => {});
  var stream = fs.createWriteStream(file, {flags: 'a'});
  stream.write(getTime()+' '+data1+' '+data2+' '+data3+' '+data4+'\r');
  stream.end();
}
var file2 = homedir+'/log/node_received_'+getDate()+'.log';
var saveLog2 = function(data1) {
  if (!fs.existsSync(file2))
    fs.writeFile(file2, '', (err) => {});
  var stream2 = fs.createWriteStream(file2, {flags: 'a'});
  stream2.write(getTime()+' '+data1+'\r');
  stream2.end();
}



/* 
 * 中间件
 */
// i18n 中间件
app.use(function (req, res, next) {
  if (req.url.includes('cbbc'))
    var lang1 = 'cbbc-lang';
  else if (req.url.includes('a1'))
    var lang1 = 'a1-lang';
  else if (req.url.includes('csalgo'))
    var lang1 = 'csalgo-lang';
  else
    var lang1 = 'lang';
  
  var cookies = get_cookies(req);
  var lang2 = cookies[lang1];
  var lang3 = (typeof lang2 == 'undefined' || !lang2) ? 'en' : lang2;
  res.cookie(lang1, lang3, { maxAge: 900000, httpOnly: false });
  app.set('views', path.join(__dirname, 'cbbc/'+lang3));
  res.setLocale(lang3);
  next();
});
// 验证用户中间件
var userAuth = function (req, res, next)
{
  if (req.url.includes('cbbc'))
    var uname1 = 'cbbc-uname', uid1 = 'cbbc-uid';
  else if (req.url.includes('a1'))
    var uname1 = 'a1-uname', uid1 = 'a1-uid';
  else if (req.url.includes('csalgo'))
    var uname1 = 'csalgo-uname', uid1 = 'csalgo-uid';
  
  var cookies = get_cookies(req), uname = cookies[uname1], uid = cookies[uid1];
  if(uname === undefined || uid === undefined || typeof(uname) !== "string" || typeof(uid) !== "string")  // 未登入
    res.redirect('/');
  else if(undefined === sessions[uid])    // 没有该通话
    res.redirect('/');
  else if(uname && uid)                   // 已登入
    next();
  else                                    // 防爆点
    res.redirect('/');
};



/*
 * 路由器
 */
// 1.0 路由器cbbc
app.get('/', function (req, res, next) {
  res.render('main', {});
});

app.get('/cbbc-login', function (req, res, next) {
  // 已登入
  var cookies = get_cookies(req);
  if(cookies['cbbc-uid'] in sessions)
    res.redirect('/cbbc');
  // 未登入
  else
    res.render('cbbc-login', {alert: null});
});

app.post('/cbbc-login', function (req, res, next) {
  var name = req.body.user_name;
  var pw = req.body.password;
  var uid = login(req, name, pw, 'cbbc');
  var cookies = get_cookies(req);
  var lang = cookies['cbbc-lang'];
  var errMsg = {
    tc: {empty: '请填写用户名和密码', userNotFound: '没有该用户', wrongAcOrPs: '用户名和密码错误'},
    tc: {empty: '請填寫用戶名和密碼', userNotFound: '沒有該用戶', wrongAcOrPs: '用戶名和密碼錯誤'},
    en: {empty: 'Username or Password is not filled in.', userNotFound: 'Username is not authorized.', wrongAcOrPs: 'Username or Password is invalid.'},
  }
  // 验证用户名和密码
  if(name === undefined || pw === undefined || typeof(name) !== "string" || typeof(pw) !== "string") 
    res.render('cbbc-login', {alert: errMsg[lang].empty});
  // 没有该用户
  else if(uid === undefined)
    res.render('cbbc-login', {alert: errMsg[lang].userNotFound});
  // 成功登入
  else if(name && pw && uid) {
    res.cookie('cbbc-uid', uid, { maxAge: 86400000, httpOnly: false });
    res.cookie('cbbc-uname', name, { maxAge: 86400000, httpOnly: false });
    saveLog(name, 'login (cbbc)');
    res.redirect('/cbbc');
  }
  // 防爆点
  else
    res.render('cbbc-login', {alert: errMsg[lang].wrongAcOrPs});
});

app.get('/cbbc', userAuth, function (req, res, next) {
  res.render('cbbc', {});
});

app.post('/cbbc-logout', function (req, res, next) {
  logout(req); // 必须清除通话
  var cookies = get_cookies(req);
  saveLog(cookies['cbbc-uname'], 'logout (cbbc)');
  res.clearCookie('cbbc-uid');
  res.clearCookie('cbbc-userId');
  res.clearCookie('cbbc-uname');
  res.redirect('/');
});


// 2.0 路由器a1
app.get('/a1-login', function (req, res, next) {
  var cookies = get_cookies(req);
  if(cookies['a1-uid'] in sessions)
    res.redirect('/a1');
  else
    res.render('a1-login', {alert: null});
});

app.post('/a1-login', function (req, res, next) {
  var name = req.body.user_name;
  var pw = req.body.password;
  var uid = login(req, name, pw, 'a1');
  var cookies = get_cookies(req);
  var lang = cookies['a1-lang'];
  var errMsg = {
    tc: {empty: '请填写用户名和密码', userNotFound: '没有该用户', wrongAcOrPs: '用户名和密码错误'},
    tc: {empty: '請填寫用戶名和密碼', userNotFound: '沒有該用戶', wrongAcOrPs: '用戶名和密碼錯誤'},
    en: {empty: 'Username or Password is not filled in.', userNotFound: 'Username is not authorized.', wrongAcOrPs: 'Username or Password is invalid.'},
  }
  // 验证用户名和密码
  if(name === undefined || pw === undefined || typeof(name) !== "string" || typeof(pw) !== "string") 
    res.render('a1-login', {alert: errMsg[lang].empty});
  // 没有该用户
  else if(uid === undefined)
    res.render('a1-login', {alert: errMsg[lang].userNotFound});
  // 成功登入
  else if(name && pw && uid) {
    res.cookie('a1-uid', uid, { maxAge: 86400000, httpOnly: false });
    res.cookie('a1-uname', name, { maxAge: 86400000, httpOnly: false });
    saveLog(name, 'login (a1)');
    res.redirect('/a1');
  }
  // 防爆点
  else
    res.render('a1-login', {alert: errMsg[lang].wrongAcOrPs});
});

app.get('/a1', userAuth, function (req, res, next) {
  res.render('a1', {});
});

app.post('/a1-logout', function (req, res, next) {
  logout(req); // 必须清除通话
  var cookies = get_cookies(req);
  saveLog(cookies['a1-uname'], 'logout (a1)');
  res.clearCookie('a1-uid');
  res.clearCookie('a1-userId');
  res.clearCookie('a1-uname');
  res.redirect('/');
});

// 2.1 路由器csalgo
app.get('/csalgo-login', function (req, res, next) {
  var cookies = get_cookies(req);
  if(cookies['csalgo-uid'] in sessions)
    res.redirect('/csalgo');
  else
    res.render('csalgo-login', {alert: null});
});

app.post('/csalgo-login', function (req, res, next) {
  var name = req.body.user_name;
  var pw = req.body.password;
  var uid = login(req, name, pw, 'csalgo');
  var cookies = get_cookies(req);
  var lang = cookies['csalgo-lang'];
  var errMsg = {
    tc: {empty: '请填写用户名和密码', userNotFound: '没有该用户', wrongAcOrPs: '用户名和密码错误'},
    tc: {empty: '請填寫用戶名和密碼', userNotFound: '沒有該用戶', wrongAcOrPs: '用戶名和密碼錯誤'},
    en: {empty: 'Username or Password is not filled in.', userNotFound: 'Username is not authorized.', wrongAcOrPs: 'Username or Password is invalid.'},
  }
  // 验证用户名和密码
  if(name === undefined || pw === undefined || typeof(name) !== "string" || typeof(pw) !== "string") 
    res.render('csalgo-login', {alert: errMsg[lang].empty});
  // 没有该用户
  else if(uid === undefined)
    res.render('csalgo-login', {alert: errMsg[lang].userNotFound});
  // 成功登入
  else if(name && pw && uid) {
    res.cookie('csalgo-uid', uid, { maxAge: 86400000, httpOnly: false });
    res.cookie('csalgo-uname', name, { maxAge: 86400000, httpOnly: false });
    saveLog(name, 'login (csalgo)');
    res.redirect('/csalgo');
  }
  // 防爆点
  else
    res.render('csalgo-login', {alert: errMsg[lang].wrongAcOrPs});
});

app.get('/csalgo', userAuth, function (req, res, next) {
  res.render('csalgo', {});
});

app.post('/csalgo-logout', function (req, res, next) {
  logout(req); // 必须清除通话
  var cookies = get_cookies(req);
  saveLog(cookies['csalgo-uname'], 'logout (csalgo)');
  res.clearCookie('csalgo-uid');
  res.clearCookie('csalgo-userId');
  res.clearCookie('csalgo-uname');
  res.redirect('/');
});

// 3.0 路由器hts
app.get('/hts', function (req, res, next) {
  res.sendFile(__dirname+'/hts/hts.html');
});

app.get('/validate', function (req, res, next) {
  var params = querystring.parse(req.originalUrl);
  var name = params.user_name;
  var pw = params.password;
  var cookie = login(req, name, pw, 'hts');
  var json = {"result":null, "uname":name, "passowrd":pw, "uid":cookie};
  
  // 验证用户名和密码
  if(name === undefined || pw === undefined || typeof(name) !== "string" || typeof(pw) !== "string")
    json.result = "fail";
  // 没有该用户
  else if(cookie === "") 
    json.result = "fail";
  // 成功登入
  else if(name && pw && cookie)
    json.result = "success";
  // 防爆点
  else
    json.result = "fail";
  saveLog(name, 'login (hts)');
  res.jsonp(json);
});

app.get('/delete', function (req, res, next) {
  var params = querystring.parse(req.originalUrl);
  var uid = params.uid;
  var json = {"result":"fail", "uid":uid, "sessionId":null};
  
  // 必须清除通话
  if (uid in sessions) {
    json.result = "success";
    json.sessionId = sessions[uid];
    saveLog(getUserName(sessions[uid].userId), 'logout (hts)');
    logout2(uid);
  }
  res.jsonp(json);
});

app.post('/validate-uid', function (req, res, next) {
  var uid = req.body.uid;
  if (!uid)
    return res.jsonp({result: 'fail', error: 'param error', param: {uid: 'string'}});
  
  if (uid in sessions)
    res.jsonp({result: 'success', error: ''});
  else if (!(uid in sessions))
    res.jsonp({result: 'fail', error: 'uid is not validated.'});
});

app.post('/saveData', function(req, res, next) {
  var type = req.body.type,
      name = req.body.name,
      data = req.body.data;
  if (!type || !name || !data)
    return res.jsonp({result: 'fail', error: 'param error', param: {type: 'string', name: 'string', data: 'json'}});
  
  // 1.0 位址
  if (type=='hts')
    var location = path.join(__dirname, 'hts/data/');
  else if (type=='cbbc')
    var location = path.join(__dirname, 'cbbc/public/data/');
  else
    return res.jsonp({result: 'fail', error: 'type is not found'});
  
  try {
    // 2.0 儲存
    var path1 = location+name+'_data.json';
    fs.writeFile(path1, data, (err) => {
      // 2.1 錯誤
      if (err) return res.jsonp({result: 'fail', error: err});
      // 2.2 成功
      return res.jsonp({result: 'success'});
    });
  } catch (err) {
    // 3.0 錯誤
    return res.jsonp({result: 'fail', error:err});
  }
});

app.post('/loadData', function(req, res, next) {
  var type = req.body.type,
      name = req.body.name;
  if (!type || !name)
    return res.jsonp({result: 'fail', error: 'param error', param: {type: 'string', name: 'string'}});
  
  // 1.0 位址
  if (type=='hts')
    var location = path.join(__dirname, 'hts/data/');
  else if (type=='cbbc')
    var location = path.join(__dirname, 'cbbc/public/data/');
  else
    return res.jsonp({result: 'fail', error: 'type is not found'});
  
  // 2.0 文件存在
  var path1 = location+name+'_data.json'
  if (!fs.existsSync(path1)) {
    return res.jsonp({result: 'fail', error: 'file is not exist'});
  } else {
    try {
      // 3.0 儲存
      fs.readFile(path1, 'utf8', function (err, data) {
        // 3.1 錯誤
        if (err) return res.jsonp({result: 'fail', error: err});
        // 3.2 成功
        return res.jsonp({result: 'success', data: data});
      });
    } catch (err) {
      // 4.0 錯誤
      return res.jsonp({result: 'fail', error:err});
    }
  }
});

app.post('/deleteData', function(req, res, next) {
  var type = req.body.type,
      name = req.body.name;
  if (!type || !name)
    return res.jsonp({result: 'fail', error: 'param error', param: {type: 'string', name: 'string'}});
  
  // 1.0 位址
  if (type=='hts')
    var location = path.join(__dirname, 'hts/data/');
  else if (type=='cbbc')
    var location = path.join(__dirname, 'cbbc/public/data/');
  else
    return res.jsonp({result: 'fail', error: 'type is not found'});
  
  // 2.0 文件存在
  var path1 = location+name+'_data.json'
  if (!fs.existsSync(path1)) {
    return res.jsonp({result: 'fail', error: 'file is not exist'});
  } else {
    try {
      // 3.0 儲存
      fs.unlink(path1, function (err) {
        // 3.1 錯誤
        if (err) return res.jsonp({result: 'fail', error: err});
        // 3.2 成功
        return res.jsonp({result: 'success'});
      });
    } catch (err) {
      // 4.0 錯誤
      return res.jsonp({result: 'fail', error:err});
    }
  }
});

// 4.0 路由器demo
var ac1 = 'fast_trader', pw1 = '123a';
app.post('/status', function(req, res, next) {
  var ac = req.body.ac,
      pw = req.body.pw;
  // 1.0 驗證身份
  if(!ac || !pw)
    return res.jsonp({result: 'fail', error: 'param error', param: {ac: 'string', pw: 'string'}});
  else if(ac != ac1 || pw != pw1)
    return res.jsonp({result: 'fail', error: 'ac or pw error'});
  // 2.0 websocket列表
  var conns_copy = {};
  for (var id in conns)
    conns_copy[id] = {};
  for (var id in conns)
    for (var origin in conns[id])
        conns_copy[id][origin] = getUserName(id)+' websocket'
  // 3.0 sessions
  // 4.0 log日志
  var log = (fs.existsSync(file)) 
    ? fs.readFileSync(file, 'utf-8').replace(/  +/g, ' ').split("\r").reverse().slice(1)
    : [];
  // 5.0 错误
  var error = []
  for (var errMsg of log)
    if(errMsg.includes('error'))
      error.push(errMsg);
  // 6.0 數據組
  if (log.length>40) log.length = 40;
  if (error.length>40) error.length = 40;
  var dict1 = {conns: conns_copy, sessions: sessions, log: log, error: error};
  return res.jsonp(dict1);
});

app.post('/delete-uid', function(req, res, next) {
  var ac = req.body.ac,
      pw = req.body.pw,
      uid = req.body.uid;
  if(!ac || !pw || !uid)
    return res.jsonp({result: 'fail', error: 'param error', param: {ac: 'string', pw: 'string', uid: 'string'}});
  else if(ac != ac1 || pw != pw1)
    return res.jsonp({result: 'fail', error: 'ac or pw error'});
  else if (!(uid in sessions))
    return res.jsonp({result: 'fail', error: 'uid not found'});
  else {
    delete sessions[uid];
    return res.jsonp({result: 'success'});
  }
});

app.get('/demo/save_data_to_json', function(req, res, next) {
  res.sendFile(__dirname+'/cbbc/demo/saveJsonForHTS.html');
});

app.get('/demo/session_panel', function(req, res, next) {
  res.sendFile(__dirname+'/cbbc/demo/sessionPanel.html');
});



/*
 * HTTP服务器端
 */
var server = http.createServer(app);
server.listen(port);
saveLog('Node start', 'db_server_uat');



/*
 * 0.0 廣播絡該用戶的所有子頻道
 * 1.0 分流频道
 */
var broadcast = function(json, id)
{
  let json_msg = JSON.stringify(json);
  // 0.1 廣播 頻道a
  if (id in conns && 'cbbc' in conns[id])
    conns[id].cbbc.send(json_msg);
  else if (!json_msg.includes('get_algo_names'))
    old_msg.push(json);
  
  // 0.2 廣播 頻道b
  if (id in conns && 'hts' in conns[id])
    conns[id].hts.send(json_msg);
  // else if (!json_msg.includes('get_algo_names'))
  //   old_msg.push(json);
  
  // 0.3 廣播 頻道c
  if (id in conns && 'a1' in conns[id])
    conns[id].a1.send(json_msg);
  // else if (!json_msg.includes('get_algo_names'))
  //  old_msg.push(json);

  // 0.4 廣播 頻道d
  if (id in conns && 'csalgo' in conns[id])
    conns[id].csalgo.send(json_msg);
}
var sendMsg = function(json)
{
  var id = json.id;
  let json_msg = JSON.stringify(json);
  saveLog2(json_msg);
  // 1.1 檢驗algo
  if ( (json_msg.includes('get_algo_names') && 'algos' in json) || ('cmd' in json && json.cmd.includes('get_algo_names')) )
  {
    broadcast(json, id);
    if ((!json_msg.includes('bear')) && (id in conns) && ('cbbc' in conns[id]))
      conns[id].cbbc.send(getErrorMsg('No Supported (bear) Algo.'));
    if ((!json_msg.includes('semi')) && (id in conns) && ('hts' in conns[id]))
      conns[id].hts.send(getErrorMsg('No Supported (semi) Algo.'));
    if ((!json_msg.includes('csalgo')) && (id in conns) && ('csalgo' in conns[id]))
      conns[id].csalgo.send(getErrorMsg('No Supported (csalgo) Algo.'));
    if ((!json_msg.includes('s1')) && (id in conns) && ('a1' in conns[id]))
      conns[id].a1.send(getErrorMsg('No Supported (s1) Algo.'));
  }
  // 1.2 頻道a
  else if ( (json_msg.includes('bear') || (('algo_name' in json) && json.algo_name.includes('bear'))) && ((id in conns) && ('cbbc' in conns[id])) )
  {
    conns[id].cbbc.send(json_msg);
  }
  // 1.3 頻道b
  else if ( (json_msg.includes('semi') || (('algo_name' in json) && json.algo_name.includes('semi'))) && ((id in conns) && ('hts' in conns[id])) )
  {
    conns[id].hts.send(json_msg);
  }
  // 1.5 頻道d
  else if ( (json_msg.includes('csalgo') || (('algo_name' in json) && json.algo_name.includes('csalgo'))) && ((id in conns) && ('csalgo' in conns[id])) )
  {
    conns[id].csalgo.send(json_msg);
  }
  // 1.4 頻道c
  else if ( (json_msg.includes('s1') || (('algo_name' in json) && json.algo_name.includes('s1'))) && ((id in conns) && ('a1' in conns[id])) )
  {
    conns[id].a1.send(json_msg);
  }
  // 1.6 沒有頻道a
  else if ( json_msg.includes('bear') || (('algo_name' in json) && json.algo_name.includes('bear')) )
  {
    old_msg.push(json);
  }
  // 1.7 沒有頻道b
  else if ( json_msg.includes('semi') || (('algo_name' in json) && json.algo_name.includes('semi')) )
  {
    // old_msg.push(json);
  }
  // 1.8 沒有頻道c
  else if ( json_msg.includes('s1') || (('algo_name' in json) && json.algo_name.includes('s1')) )
  {
    // old_msg.push(json);
  }
  // 1.9 沒有頻道d
  else if ( json_msg.includes('csalgo') || (('algo_name' in json) && json.algo_name.includes('csalgo')) )
  {
    // old_msg.push(json);
  }
  // 1.10 廣播
  else if (id in conns)
  {
    broadcast(json, id);
    saveLog(getUserName(id), 'ws conflict send.', 'except error 018: '+json_msg);
  }
  // 1.11 錯誤
  else
  {
    saveLog(getUserName(id), 'ws cannot send.', 'except error 019: catch error');
  }
}



/*
 * WebSocket
 */
var wsSrv = new ws({httpServer : server});
wsSrv.on('request', (request)=>
{
  // 0.0 抓取優先級
  var getPriority = function(uid)
  {
    if(!(uid in sessions))
      return {maxPriority: undefined, curPriority: undefined}
    var maxPriority = 0, curPriority = sessions[uid].priority;
    for(var uid2 in sessions)
    {
      if (sessions[uid2].userId == sessions[uid].userId && 
          sessions[uid2].origin == sessions[uid].origin && 
          sessions[uid2].priority > maxPriority) {
        maxPriority = sessions[uid2].priority
      }
    }
    return {maxPriority: maxPriority, curPriority: curPriority}
  }
  // 0.1 寫數據流IO
  var writeIO = function(message, id)
  {
    try
    {
      if (typeof message == 'object' && 'utf8Data' in message)
        var json = JSON.parse(message.utf8Data);
      else
        var json = JSON.parse(message);
      json.id = id;
      process.stdout.write(JSON.stringify(json) + "\n");
    } 
    catch(e)
    {
      saveLog(getUserName(id), 'write IO error.', JSON.stringify(message), JSON.stringify(e));
    }
  }
  
	let connection = request.accept(null, request.origin);
  var msg_get_algo_names = JSON.stringify({cmd: 'get_algo_names', ref: 'algo_name_test'});
  // 1.0 抓取uid (登入時的安全識別標簽)
  var path = request.resourceURL.path;
  if (path.indexOf('uid=') > -1)
    var {uid, id, origin} = get_uid2(request.resourceURL);
  else
    var {uid, id, origin} = get_uid(request.httpRequest);
  saveLog(getUserName(id), 'ws ('+origin+') request.', path);
  // 2.0 檢查uid，防止被滲透
  if (undefined === id)
  {
    connection.send(getErrorMsg('Authentication Token Failure.'));
		connection.close();
    saveLog(getUserName(id), 'ws ('+origin+') cannot start.', 'except error 001: uid not found.');
		return;
  }
  // 3.1 檢查sessions, 防止被滲透
  var isSessionExist = false;
  for (var uid in sessions)
    if (sessions[uid].userId == id)
      isSessionExist = true;
  if (isSessionExist == false)
  {
    connection.send(getErrorMsg('Authentication Token Failure.'));
		connection.close();
    saveLog(getUserName(id), 'ws ('+origin+') cannot start.', 'except error 002: session id not found.');
		return;
  }
  // 3.0 已在ws通訊層
  if (id in conns) {
    // 3.1 尋找该userId的最高優先級
    var {maxPriority, curPriority} = getPriority(uid);
    if (origin in conns[id])
    {
      // 3.2 不存在ws通訊層
      if (curPriority == 0 || curPriority == undefined)
      {
        saveLog(getUserName(id), 'ws ('+origin+') cannot start.', 'except error 003: uid not found.');
      }
      // 3.3 如不是最高優先級，拒絕其訪問ws通信层
      else if (curPriority < maxPriority)
      {
        conns[id][origin].send(getErrorMsg('Duplicate Login Account.'));
        saveLog(getUserName(id), 'ws ('+origin+') cannot start & reject.', 'ws duplicate & reject, priority '+curPriority+'<'+maxPriority+' denied.');
      }
      // 3.4 是最高優先級，可訪問ws通信层
      else if (curPriority == maxPriority)
      {
        conns[id][origin].send(getErrorMsg('Duplicate Login Account.'));
        saveLog(getUserName(id), 'ws ('+origin+') reject.', 'priority='+curPriority);
        writeIO(msg_get_algo_names, id);
        conns[id][origin] = connection;
        saveLog(getUserName(id), 'ws ('+origin+') start.', 'priority='+curPriority);
      }
      else
      {
        saveLog(getUserName(id), 'ws ('+origin+') cannot start.', 'except error 004: current priority not found.');
      }
    }
    // 3.5 不在ws通訊層，建立持久層連接
    else if (!('origin' in conns[id]))
    {
      writeIO(msg_get_algo_names, id);
      conns[id][origin] = connection
      saveLog(getUserName(id), 'ws ('+origin+') start.', 'init ws instance.');
    }
    else {
      saveLog(getUserName(id), 'ws ('+origin+') cannot start.', 'except error 005: origin not found.');
    }
  }
  // 4.0 不在ws通訊層，建立持久層連接
  else if (!(id in conns))
  {
    writeIO(msg_get_algo_names, id);
    conns[id] = {};
    conns[id][origin] = connection
    saveLog(getUserName(id), 'ws ('+origin+') start.', 'init ws instance.');
  }
  // 5.0 傳送信息
	let user_id_json = {user_id: id};
	connection.send(JSON.stringify(user_id_json));
	for (let i in old_msg)
	{
		let old_json = old_msg[i];
		if (undefined === old_json.id || 0 === old_json.id || id === old_json.id)
      sendMsg(old_json);  // connection.send(JSON.stringify(old_json));
	}
	connection.send(JSON.stringify({type: 'recovery_end'}));
	connection.on("close", () =>
	{
    // 6.0 刪除ws通訊層
    if (uid in sessions)
    {
      // 6.1 尋找该userId的最高優先級
      var {maxPriority, curPriority} = getPriority(uid);
      // 6.2 不存在ws通訊層
      if (curPriority == 0 || curPriority == undefined)
      {
        connection.send(getErrorMsg('Authentication Token Failure.'));
        saveLog(getUserName(id), 'ws ('+origin+') cannot close.', 'except error 006: uid not found.');
      }
      // 6.3.1 该频道存在
      if (sessions[uid].origin == origin)
      {
        // 6.3.2  如不是最高優先級，拒絕其訪問ws通信层
        if (curPriority < maxPriority)
        {
          connection.send(getErrorMsg('Authentication Token Failure.'));
          saveLog(getUserName(id), 'ws ('+origin+') cannot close.', 'priority '+curPriority+'<'+maxPriority+' denied.');
        }
        // 6.3.3 是最高優先級，可訪問ws通信层
        else if (curPriority = maxPriority)
        {
          // 6.3.4 uid不存在ws層
          if (!(id in conns))
          {
            connection.send(getErrorMsg('Authentication Token Failure.'));
            saveLog(getUserName(id), 'ws ('+origin+') cannot close.', 'except error 007: id not found.');
          }
          // 6.3.5 origin不存在ws層
          else if (!(origin in conns[id]))
          {
            connection.send(getErrorMsg('Authentication Token Failure.'));
            saveLog(getUserName(id), 'ws ('+origin+') cannot close.', 'except error 008: origin not found.');
          }
          // 6.3.6 刪除ws層col
          else
          {
            delete conns[id][origin];
            saveLog(getUserName(id), 'ws ('+origin+') close.', 'priority='+curPriority);
            // 6.3.7 刪除ws層key
            if (Object.keys(conns[id]).length === 0)
            {
              delete conns[id];
              saveLog(getUserName(id), 'ws (column) close.', 'priority='+curPriority);
            }
          }
        }
      }
      // 6.4 錯誤
      else
      {
        connection.send(getErrorMsg('Authentication Token Failure.'));
        saveLog(getUserName(id), 'ws ('+origin+') cannot close.', 'except error 009: origin not found.');
      }
    }
    // 6.5 錯誤
    else
    {
      connection.send(getErrorMsg('Authentication Token Failure.'));
      saveLog(getUserName(id), 'ws ('+origin+') cannot close.', 'except error 010: uid not found.');
    }
	});
	connection.on('message', (message) => 
	{
    // 7.0 傳送
		if (message.type === 'utf8')
		{
			try
			{
        // 7.0 防止滲透
        if (!(uid in sessions))
        {
          connection.send(getErrorMsg('Authentication Token Failure.'));
          connection.close();
          saveLog(getUserName(id), 'ws ('+origin+') cannot send.', 'except error 011: uid not found. uid='+uid);
        }
        // 7.1 尋找该userId的最高優先級
        var {maxPriority, curPriority} = getPriority(uid);
        // 7.2 不存在ws通訊層
        if (curPriority == 0 || curPriority == undefined)
        {
          connection.send(getErrorMsg('Authentication Token Failure.'));
          saveLog(getUserName(id), 'ws ('+origin+') cannot send.', 'except error 012: uid not found. uid='+uid);
        }
        // 7.3 如不是最高優先級，拒絕其訪問ws通信层
        else if (curPriority < maxPriority)
        {
          connection.send(getErrorMsg('Authentication Token Failure.'));
          saveLog(getUserName(id), 'ws ('+origin+') cannot send.', origin+' priority '+curPriority+'<'+maxPriority+' denied.');
        }
        // 7.4 是最高優先級，可訪問ws通信层
        else if (curPriority == maxPriority)
        {
          writeIO(message, id);
        }
        // 7.5 錯誤
        else
        {
          connection.send(getErrorMsg('Authentication Token Failure.'));
          saveLog(getUserName(id), 'ws ('+origin+') cannot send.', 'except error 013: priority not found.');
        }
			}
			catch(e)
			{
        saveLog(getUserName(id), 'ws ('+origin+') cannot send.', 'except error 014: catch error');
			}
		}
	});
});
// 8.0 heartbeat
try
{
  setInterval(function(){
    if (parseInt(new Date().getSeconds())%10==0)
      for (var id in conns)
        for (var origin in conns[id])
          conns[id][origin].send(JSON.stringify({action: 'connect_alive', origin: origin, stime: getTime()}));
  }, 1000);
}
catch(e)
{
  saveLog(getUserName(id), 'ws cannot send.', 'except error 015: heartbeat');
}



/*
 *  读取IO数据流
 */
var reader = null;
var recursiveAsyncReadLine = function()
{
  saveLog('Readline start');
  reader = readline.createInterface
  (
    {
      input: process.stdin
    }
  );
  reader.on("line", (line) =>
  {
    try
    {
      let json = JSON.parse(line);
      json.tm = new Date().getTime();
      // 1.0 公共广播信息
      if (undefined === json.id || 0 === json.id)
      {
        for (let id in conns)
        {
          json.id = id;
          sendMsg(json);
        }
      }
      // 2.0 个别用户信息
      else
      {
        sendMsg(json);
      }
      // 3.0 重新連接時，重發所有數據
      if (true === json["recovery"])
      {
        old_msg.push(json);
      }
    }
    catch(e)
    {
      saveLog('Readline error', JSON.stringify(e));
      console.error(e);
    }
  });
  reader.on('close', () =>
  {
    saveLog('Readline close');
    recursiveAsyncReadLine();
  });
}
recursiveAsyncReadLine();
