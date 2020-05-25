const error_str = "Usage: node set_user.js user_name=... password=... is_master=<true|false>";
const encryptor = require(__dirname + "/password.js");
var user = require(__dirname + "/user.js");
var users = user.load();
function decode_field(str)
{
	let arrs = str.split("=");
	if (2 !== arrs.length)
		return null;
	else
	{
		let rt = 
		{
			"key": arrs[0],
			"value": arrs[1]
		};
		return rt;
	}
}
function decode_top(str)
{
	let arrs = str.split("|");
	if (4 !== arrs.length)
		return null;
	else
	{
		let rt = 
		{
			"IP": arrs[0],
			"PORT": arrs[1],
			"USER": arrs[2],
			"PASS": arrs[3]
		};
		return rt;
	}
}
let user_name = null;
let password = null;
let is_master = false;
let top = null;
for (let i = 2; i < process.argv.length; ++i)
{
	let obj = decode_field(process.argv[i]);
	if (null === obj)
	{
		console.error(error_str);
		process.exit(-1);
	}
	if ("user_name" === obj.key)
	{
		user_name = obj.value;
	}
	else if ("password" === obj.key)
	{
		password = obj.value;
	}
	else if ("is_master" === obj.key)
	{
		if ("true" === obj.value)
			is_master = true;
		else if ("false" === obj.value)
			is_master = false;
		else
		{
			console.error(error_str);
			process.exit(-1);
		}
	}
	else
	{
		console.error(error_str);
		process.exit(-1);
	}
}
if (null === user_name)
{
	console.error(error_str);
	process.exit(-1);
}
if (null === password)
{
	console.error(error_str);
	process.exit(-1);
}
let node = users[user_name];
if (undefined === node)
{
	let id = 1;
	for (let name in users)
	{
		let new_id = users[name].id + 1;
		if (new_id > id)
		{
			id = new_id;
		}
	}
	users[user_name] = {};
	node = users[user_name];
	node.id = id;
}
if (is_master)
{
	node.id = 0;
}
node.password = encryptor.encrypt(password);
user.store(users);

