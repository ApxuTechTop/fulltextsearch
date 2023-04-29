local luafts = require("luafts")

local fts = {}

local indexMeta = {
    __index = {
        accessor = function(index)
            return fts.accessor{type = index.type, path = index.path}
        end,
        add = function()

        end
    },
    __gc = function(self) {
        luafts.free_index(self)
    }
}
local accessorMeta = {
    __index = {
        search = function(accessor, query, config)
            local config = config or accessor.configuration
            local results = luafts.search(accessor, query, config) -- это си функция
            return results
        end
    }
}

local indexTypes = {}
indexTypes["text"] = function(index)

end
indexTypes["binary"] = function(index)

end

fts.index = function(options)
    local index = {}
    index.type = options.type or "text"
    index.path = options.path or error("Expected path for index")
    index.configuration = options.configuration
    if options.file then
        local f = csv.open(options.file)
        luafts.create_index(index)
        for fields in f:lines() do
            luafts.add_document(index, fields.bookID, fields.title)
        end
        if options.path then
            if index.type == "text" then
                luafts.text_index_write(index, options.path)
            elseif index.type == "binary" then
                
            end
        end
    end

    setmetatable(index, indexMeta)
    return index
end
fts.accessor = function(options)
    local accessor = {}
    accessor.type = options.type or "text"
    accessor.path = options.path or error("Expected path for accessor")
    accessor.configuration = options.configuration

    setmetatable(accessor, accessorMeta)
    return accessor
end

return fts