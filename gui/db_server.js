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
const readline = require("readline");
const ws = require("websocket").server;
const http = require("http");
const fs = require("fs");
const mime = require(__dirname + "/mime.json");
const old_msg = [];
var random_str = require(__dirname + "/random_string.js");
const get_mime = function(url)
{
	let strs = url.split('.');
	if (0 === strs.length)
	{
		return null;
	}
	let subfix = strs[strs.length - 1].toLowerCase();
	if (undefined === mime[subfix])
	{
		return null;
	}
	return mime[subfix];
};
const now = function()
{
	return new Date().getTime();
}
var conns = {};
var handle_post = function (req, cb)
{
	let body = "";
	req.on("data", function(chunk)
	{
		body += chunk.toString();
	});
	req.on("end", function()
	{
		cb(body);
	});
};
var sessions = {};
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
var get_id = function(req)
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
			if (conns[id])
			{
				try
				{
					conns[id].close();
				}
				catch(e)
				{
				}
			}
			return true;
		}
		else
		{
			return false;
		}
	}
};
var login = function(req, user, password)
{
	logout(req);
	if (undefined !== users[user])
	{
		var encryptor = require(__dirname + "/password.js");
		if (users[user].password === encryptor.encrypt(password))
		{
			let cookie_str = random_str.random_string();
			sessions[cookie_str] = users[user].id;
			return cookie_str;
		}
		else
			return "";
	}
};
var server = http.createServer((req, res) =>
{
	let login_path = __dirname + "/login.html";
	if ("POST" === req.method)
	{
		if ("/login" === req.url)
		{
			handle_post(req, function(body)
			{
				var querystring = require('querystring');
				try
				{
					let login_data = querystring.parse(body);
					if (undefined === login_data.user_name)
					{
						fs.readFile(login_path, function(error,data)
						{
							if (error)
							{
								res.writeHead(404, {"Content-Type": "text/html"});
								res.end("Page not found!");
							}
							else
							{
								res.writeHead(200, {"Content-Type": "text/html"});
								res.end(data);
							}
						});
					}
					else if ("string" !== typeof(login_data.user_name))
					{
						fs.readFile(login_path, function(error,data)
						{
							if (error)
							{
								res.writeHead(404, {"Content-Type": "text/html"});
								res.end("Page not found!");
							}
							else
							{
								res.writeHead(200, {"Content-Type": "text/html"});
								res.end(data);
							}
						});
					}
					else if (undefined === login_data.password)
					{
						fs.readFile(login_path, function(error,data)
						{
							if (error)
							{
								res.writeHead(404, {"Content-Type": "text/html"});
								res.end("Page not found!");
							}
							else
							{
								res.writeHead(200, {"Content-Type": "text/html"});
								res.end(data);
							}
						});
					}
					else if ("string" !== typeof(login_data.password))
					{
						fs.readFile(login_path, function(error,data)
						{
							if (error)
							{
								res.writeHead(404, {"Content-Type": "text/html"});
								res.end("Page not found!");
							}
							else
							{
								res.writeHead(200, {"Content-Type": "text/html"});
								res.end(data);
							}
						});
					}
					else
					{
						let cookie_str = login(req, login_data.user_name, login_data.password);
						if ("" === cookie_str)
						{
							fs.readFile(login_path, function(error,data)
							{
								if (error)
								{
									res.writeHead(404, {"Content-Type": "text/html"});
									res.end("Page not found!");
								}
								else
								{
									res.writeHead(200, {"Content-Type": "text/html"});
									res.end(data);
								}
							});
						}
						else
						{
							res.writeHead(302,
							{
								"Content-Type": "text/html",
								"Location": "/",
								"Set-Cookie": ["uid=" + cookie_str, "uname=" + login_data.user_name]
							});
							res.end("Login Success");
						}
					}
				}
				catch(e)
				{
					fs.readFile(login_path, function(error,data)
					{
						if (error)
						{
							res.writeHead(404, {"Content-Type": "text/html"});
							res.end("Page not found!");
						}
						else
						{
							res.writeHead(200, {"Content-Type": "text/html"});
							res.end(data);
						}
					});
				}
			});
		}
		else if ("/logout" === req.url)
		{
			handle_post(req, function()
			{
				logout(req);
				res.writeHead(302,
				{
					"Content-Type": "text/html",
					"Location": "/",
					"Set-Cookie": "uid="
				});
				res.end("Logout Success");
			});
		}
		else
		{
			res.writeHead(404, {"Content-Type": "text/html"});
			res.end("Page not found!");
		}
	}
	else if ("GET" === req.method)
	{
		let path = req.url;
		if ("/" === path)
		{
			path = "/db_index.html";
		}
    if(path.indexOf("validate")>-1)
    {
      var arr = path.replace("/validate?","").split("&");
      var params = new Array();
      for (let i in arr)
      {
        var key = arr[i].split("=")[0];
        var val = arr[i].split("=")[1];
        params[key] = val;
      }
      var cookie_str = login(req, params.user_name, params.password);
      res.writeHead(200, {"Content-Type": "text/html"});
      if ("" === cookie_str || typeof(cookie_str)=='undefined')
      {
        res.end('callback ({"result":"fail", "uname":"'+params.user_name+'", "passowrd":"'+params.password+'", "uid":null})');
      }
      else
      {
        res.end('callback ({"result":"success", "uname":"'+params.user_name+'", "passowrd":"'+params.password+'", "uid":"'+cookie_str+'"})');
      }
    }
    if(path.indexOf("delete")>-1){
      var arr = path.replace("/delete?","").split("&");
      var params = new Array();
      for (let i in arr)
      {
        var key = arr[i].split("=")[0];
        var val = arr[i].split("=")[1];
        params[key] = val;
      }
      
      var arr = [];
      for (var key in sessions) {
        if (sessions.hasOwnProperty(key)) {
          arr.push(key + '=' + sessions[key]);
        }
      };
      res.writeHead(200, {"Content-Type": "text/html"});
      
      if (params.uid in sessions){
        delete sessions[params.uid];
        res.end('callback ({"result":"success", "uid":"'+params.uid+'", "session":"'+arr.join(', ')+'"})');
      }
      else{
        res.end('callback ({"result":"fail", "uid":"'+params.uid+'", "session":"'+arr.join(', ')+'"})');
      }
    }
		if ("/login.html" !== path && -1 === path.indexOf("/hts/"))
		{
			if (undefined === get_id(req))
			{
				fs.readFile(login_path, function(error,data)
				{
					if (error)
					{
						res.writeHead(404, {"Content-Type": "text/html"});
						res.end("Page not found!");
					}
					else
					{
						res.writeHead(200, {"Content-Type": "text/html"});
						res.end(data);
					}
				});
				return;
			}
		}
		let t = get_mime(path);
		if (null === t)
		{
			res.writeHead(404, {"Content-Type": "text/html"});
			res.end("Page Mime not found!");
		}
		else
		{
			let full_path  = __dirname + path;
			fs.readFile(full_path, function(error,data)
			{
				if (error)
				{
					res.writeHead(404, {"Content-Type": "text/html"});
					res.end("Page not found!");
				}
				else
				{
					res.writeHead(200, {"Content-Type": t});
					res.end(data);
				}
			});
		}
	}
	else
	{
		res.writeHead(405, {"Content-Type": "text/html"});
		res.end("method not support!");	
	}
});
server.listen(port);
var wsSrv = new ws({httpServer : server});
wsSrv.on('request', (request)=>
{
	let connection = request.accept(null, request.origin);
	let id = get_id(request.httpRequest);
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
var reader = readline.createInterface
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
		json.tm = now();
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
	}
});

