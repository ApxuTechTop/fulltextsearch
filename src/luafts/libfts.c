#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>
#include <stdlib.h>
#include <string.h>
#include <libindexer-c/indexer.h>
#include <libaccessor-c/accessor.h>
#include <libsearcher-c/searcher.h>


static int l_search(lua_State* L) { // {path = "", type = ""}, query, config
    index_accessor accessor;
    const char* query;
    Configuration config;
    lua_getfield(L, 1, "type"); // accessor, query, config, accessor.type
    const char* type = lua_tolstring(L, -1, NULL);
    if (strcmp(type, "text")) {
        accessor.type = index_accessor_type_text;
    } else if (strcmp(type, "binary")) {
        accessor.type = index_accessor_type_binary;
    }
    lua_pop(L, 1); // accessor, query, config
    lua_getfield(L, 1, "path");
    const char* path = lua_tolstring(L, -1, NULL);
    accessor.path = path;
    lua_pop(L, 1);
    query = lua_tolstring(L, 2, NULL);
    lua_getfield(L, 3, "min");
    lua_getfield(L, 3, "max");
    lua_getfield(L, 3, "stopwords");
    config.ngrammin = lua_tointeger(L, -3);
    config.ngrammax = lua_tointeger(L, -2);
    lua_len(L, -1);
    config.count = lua_tointeger(L, -1);
    lua_pop(L, 1);
    for (size_t i = 0; i < config.count; ++i) {
        lua_geti(L, -1, i + 1);
        config.stopwords[i] = lua_tolstring(L, -1, NULL);
        lua_pop(L, 1);
    }
    lua_pop(L, 3);

    vector_wrap vw = accessor_search(accessor, query, &config);
    lua_createtable(L, vw.count, 0);
    for (size_t i = 0; i < vw.count; ++i) {
        lua_newtable(L);
        lua_pushnumber(L, ((Result*)vw.data)[i].score);
        lua_setfield(L, -2, "score");
        lua_pushinteger(L, ((Result*)vw.data)[i].document_id);
        lua_setfield(L, -2, "id");
        lua_seti(L, -2, i + 1);
    }
    free(vw.data);
    free(vw.vector_ptr);
    return 1;
}

// index, id, text
static int l_add_document(lua_State* L) {
    int id = lua_tointeger(L, 2);
    const char* text = lua_tolstring(L, 3, NULL);
    lua_getfield(L, 1, "_index");
    void* index = lua_touserdata(L, -1);
    index_add_document(index, id, text);
    return 0;
}

// index
static int l_create_index(lua_State* L) {
    lua_getfield(L, 1, "configuration");
    Configuration config;
    lua_getfield(L, 2, "min");
    config.ngrammin = lua_tointeger(L, -1);
    lua_getfield(L, 2, "max");
    config.ngrammax = lua_tointeger(L, -1);
    lua_getfield(L, 2, "stopwords");
    int n = luaL_len(L, -1);
    config.count = (size_t)n;
    for (int i = 0; i < n; ++i) {
        lua_rawgeti(L, -1, i + 1);
        const char* word = lua_tolstring(L, -1, NULL);
        config.stopwords[i] = word;
    }
    Index cindex = create_index(&config);
    lua_pushlightuserdata(L, cindex);
    lua_setfield(L, 1, "_index");
    return 0;
}

// index
static int l_free_index(lua_State* L) {
    lua_getfield(L, 1, "_index");
    Index index = (Index)lua_touserdata(L, -1);
    free_index(index);
    return 0;
}

static const struct luaL_Reg luaftslib [] = {
    {"search", l_search}, // связали поле с CFunction
    {"add_document", l_add_document},
    {"free_index", l_free_index},
    {"create_index", l_create_index},
    {NULL, NULL}
};

int luaopen_luafts(lua_State* L) { // функция которая вызывается через require
    luaL_newlib(L, luaftslib); // создали библиотеку (таблицу с функциями) (можно было сделать через таблицу, но в руководстве через библиотеку)
    return 1;
}


