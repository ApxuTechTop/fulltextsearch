local fts = require("src/lua/fts")

local configuration = {
    min = 2,
    max = 10,
    stopwords = {
        "of", "is", "the"
    }
}

print("index")
local index = fts.index{configuration = configuration, file = "books.csv", type = "text", path = "textidx"}
index:write()

print("accessor")

local accessor = fts.accessor{type = "text", path = arg[1]}
local results = accessor:search(arg[2], {min = 2, max = 10, stopwords = {"of", "is"}})
local best_score = results[1].score
for key, result in ipairs(results) do
    if result.score > best_score * (tonumber(arg[3]) or 0.5) then
        print(key, result.score, result.id, accessor:get(result.id))
    end
end