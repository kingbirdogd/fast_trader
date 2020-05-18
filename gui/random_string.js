const crypto = require("crypto");
exports.random_string = function()
{
    var buf = crypto.randomBytes(256);
    return buf.toString("hex");
};
