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

// i18n中间件
app.use(function (req, res, next) {
  var _lang = req.cookies.lang;
  var lang = (typeof _lang == 'undefined' || !_lang) ? 'en' : _lang;
  res.cookie('lang', lang, { maxAge: 900000, httpOnly: false });
  app.set('views', path.join(__dirname, 'cbbc/'+lang));
  res.setLocale(lang);
  next();
});

const old_msg = []; // 公共
var conns = {};     // WebSocket



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
	if (undefined === cookies.uid)
		return undefined;
	else
	{
		let id = sessions[cookies.uid];
		if (undefined === id)
			return undefined;
		else
			return id;
	}
};
// 验证用户中间件
var userAuth = function (req, res, next) {
  var cookies = get_cookies(req);
  var uname = cookies.uname;
  var uid = cookies.uid;
  
  // 未登入
  if(uname === undefined || uid === undefined || typeof(uname) !== "string" || typeof(uid) !== "string")
    res.redirect('/login');
  // 没有该通话
  else if(undefined === sessions[uid])
    res.redirect('/login');
  // 已登入
  else if(uname && uid)
    next();
  // 防爆点
  else
    res.redirect('/login');
};
// 清除sessions
var logout = function(req)
{
	let cookies = get_cookies(req);
	if (undefined === cookies.uid)
		return false;
	else
	{
		let id = sessions[cookies.uid];
		if (undefined !== id)
		{
			delete sessions[cookies.uid];
			return true;
		}
		else
		{
			return false;
		}
	}
};
// 返回uid给服务器端
var login = function(req, user, password)
{
	logout(req);
	if (undefined !== users[user])
	{
		var encryptor = require(__dirname + "/password.js");
		if (users[user].password === encryptor.encrypt(password))
		{
			let cookie_str = crypto.randomBytes(256).toString("hex");
			sessions[cookie_str] = users[user].id;
			return cookie_str;
		}
		else
			return "";
	}
};



/*
 * 路由器
 */
app.get('/', function (req, res, next) {
  res.redirect('/login');
});

app.get('/login', function (req, res, next) {
  res.render('login', {alert: null});
});

app.post('/login', function (req, res, next) {
  var name = req.body.user_name;
  var pw = req.body.password;
  var cookie = login(req, name, pw);
  
  // 验证用户名和密码
  if(name === undefined || pw === undefined || typeof(name) !== "string" || typeof(pw) !== "string") 
    res.render('login', {alert: "请填写用户名和密码"});
  // 没有该用户
  else if(cookie === undefined)
    res.render('login', {alert: "没有该用户"});
  // 成功登入
  else if(name && pw && cookie) {
    res.cookie('uid', cookie, { maxAge: 86400000, httpOnly: false });
    res.cookie('uname', name, { maxAge: 86400000, httpOnly: false });
    res.redirect('/index');
  }
  // 防爆点
  else
    res.render('login', {alert: "用户名和密码错误"});
});

app.get('/index', userAuth, function (req, res, next) {
  res.render('index', {});
});

app.get('/order', userAuth, function (req, res, next) {
  res.render('order', {});
});

app.get('/log', userAuth, function (req, res, next) {
  res.render('log', {});
});

app.get('/profilo', userAuth, function (req, res, next) {
  res.render('profilo', {});
});

app.get('/module/:module', userAuth, function (req, res, next) {
  res.render('module', {module: req.params.module});
});

app.get('/command', userAuth, function (req, res, next) {
  res.render('command', {});
});

app.get('/warrant', userAuth, function (req, res, next) {
  res.render('warrant', {});
});

app.get('/cbbc', userAuth, function (req, res, next) {
  res.render('cbbc', {});
});

app.get('/cbbc_uat', userAuth, function (req, res, next) {
  res.render('cbbc_uat', {});
});

app.post('/logout', function (req, res, next) {
  logout(req); // 必须清除通话
  res.redirect('/login');
});

app.get('/hts', function (req, res, next) {
  res.sendFile(__dirname+'/hts/hts.html');
});

app.get('/validate', function (req, res, next) {
  var params = querystring.parse(req.originalUrl);
  var name = params.user_name;
  var pw = params.password;
  var cookie = login(req, name, pw);
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
    delete sessions[uid];
  }
  
  res.jsonp(json);
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

app.get('/demo/save_data_to_json', function(req, res, next) {
  res.sendFile(__dirname+'/cbbc/demo/saveJsonForHTS.html');
});



/*
 * HTTP服务器端
 */
var server = http.createServer(app);
server.listen(port);



/*
 * WebSocket
 */
var wsSrv = new ws({httpServer : server});
wsSrv.on('request', (request)=>
{
	let connection = request.accept(null, request.origin);
	let id = get_uid(request.httpRequest);
  if(typeof id=='undefined')
    id = sessions[request.resource.replace('/uid=','')];
	let user_id_json = {};
	user_id_json["user_id"] = id;
	connection.send(JSON.stringify(user_id_json));
	if (undefined === id)
	{
		connection.close();
		return;
	}
	conns[id] = connection;
	for (let i in old_msg)
	{
		let old_json = old_msg[i];
		if (undefined === old_json.id || 0 === old_json.id || id === old_json.id)
			connection.send(JSON.stringify(old_json));
	}
	let recovery_end = {};
	recovery_end["type"] = "recovery_end";
	connection.send(JSON.stringify(recovery_end));
	connection.on("close", () =>
	{
		delete conns[id];
	});
	connection.on('message', (message) => 
	{
		if (message.type === 'utf8')
		{
			try
			{
				let json = JSON.parse(message.utf8Data);
				json.id = id;
				process.stdout.write(JSON.stringify(json) + "\n");
			}
			catch(e)
			{
			}
		}
	});
});

/*
 *  读取IO数据流
 */
var reader = null;
var recursiveAsyncReadLine = function() {
  console.log('Readline start');
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
      let json_msg = JSON.stringify(json);
      if (undefined === json.id || 0 === json.id)
      {
        for (let id in conns)
        {
          conns[id].send(json_msg);
        }
      }
      else
      {
        let conn = conns[json.id];
        if (undefined !== conn)
        {
          conn.send(json_msg);
        }
      }
      if (true === json["recovery"])
      {
        old_msg.push(json);
      }
    }
    catch(e)
    {
      console.error('Readline error');
      console.error(e);
    }
  });
  reader.on('close', () =>
  {
    console.error('Readline close');
    recursiveAsyncReadLine();
  });
}
recursiveAsyncReadLine();
