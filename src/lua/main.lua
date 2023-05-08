local fts = require("src/lua/fts")

local configuration = {
    min = 3,
    max = 10,
    stopwords = {
        "of", "is", "the"
    }
}

local index = fts.index{configuration = configuration, file = arg[3], type = "text", path = arg[1]}
if (arg[3]) then
    index:write()
end


local accessor = fts.accessor{type = "text", path = arg[1]}
local results = accessor:search(arg[2], {min = 2, max = 10, stopwords = {"of", "is"}})
local best_score = results[1].score
for key, result in ipairs(results) do
    if result.score > best_score * (tonumber(arg[4]) or 0.5) then
        print(key, result.score, result.id, accessor:get(result.id))
    end
end