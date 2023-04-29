local fts = require("fts")




local configuration = {
    min = 2,
    max = 10,
    stopwords = {
        "of", "is", "the"
    }
}

--[=[
local index = fts.index{configuration = configuration, file = "books.csv", type = "binary", path = "binindex"}
index.write("binindex", "binary")
local index = fts.index{type = "binary", path = "binindex"}

]=]


local accessor = fts.accessor{type = "text", path = "idx"}
local results = accessor:search("harry potar", {min = 2, max = 10, stopwords = {"of", "is"}})
for key, result in ipairs(results) do
    print(key, result.score, result.id, index[result.id])
end