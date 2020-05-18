const fs = require("fs");
exports.load = function()
{
    let users = {};
    try
    {
        users = require(__dirname + "/user.json");
    }
    catch(e)
    {
    }
    return users;
};

exports.store = function(users)
{
    fs.writeFileSync(__dirname + "/user.json", JSON.stringify(users));
};
