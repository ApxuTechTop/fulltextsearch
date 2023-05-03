local luafts = require("libluafts")
local csv = require("src/lua/csv")

local fts = {}

local indexMeta = {
    __index = {
        accessor = function(index)
            return fts.accessor{type = index.type, path = index.path}
        end,
        add = function(index, id, text)
            luafts.add_document(index, id, text)
        end,
        write = function(index, options)
            local options = options or {}
            options.type = options.type or index.type or "text"
            options.path = options.path or index.path or error("Expected path for index")
            if options.type == "text" then
                print("start writing")
                luafts.text_index_write(index, options.path)
            end
        end
    },
    __gc = function(self) 
        luafts.free_index(self)
    end
}
local accessorMeta = {
    __index = {
        search = function(accessor, query, config)
            local config = config or accessor.configuration
            local results = luafts.search(accessor, query, config) -- это си функция
            return results
        end,
        get = function(accessor, id)
            return luafts.load_document(accessor, id)
        end
    }
}

fts.index = function(options)
    local index = {}
    index.type = options.type or "text"
    index.path = options.path
    index.configuration = options.configuration
    if options.file then
        local f = csv.open(options.file, {header = true})
        print("creating index")
        luafts.create_index(index)
        for fields in f:lines() do
            --print("add", fields.bookID, fields.title)
            luafts.add_document(index, fields.bookID, fields.title)
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