const crypto = require("crypto");
exports.encrypt = function(password)
{
    return crypto.createHash("sha1").update(password).digest("hex");
};
