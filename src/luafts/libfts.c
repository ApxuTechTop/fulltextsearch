#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <libindexer-c/indexer.h>
#include <libaccessor-c/accessor.h>
#include <libsearcher-c/searcher.h>


static index_accessor get_accessor(lua_State* L, int pos) {
    index_accessor accessor;
    lua_getfield(L, pos, "type"); // accessor, query, config, accessor.type
    const char* type = lua_tolstring(L, -1, NULL);
    accessor.type = index_accessor_type_text;
    if (strcmp(type, "text") == 0) {
        accessor.type = index_accessor_type_text;
    } else if (strcmp(type, "binary") == 0) {
        accessor.type = index_accessor_type_binary;
    }
    lua_pop(L, 1); // accessor, query, config
    lua_getfield(L, pos, "path");
    const char* path = lua_tolstring(L, -1, NULL);
    accessor.path = path;
    lua_pop(L, 1);
    return accessor;
}

static Configuration get_config(lua_State* L, int pos) {
    Configuration config;
    lua_getfield(L, pos, "min");
    config.ngrammin = luaL_checkinteger(L, -1);
    lua_pop(L, 1);
    lua_getfield(L, pos, "max");
    config.ngrammax = luaL_checkinteger(L, -1);
    lua_pop(L, 1);
    lua_getfield(L, pos, "stopwords");
    lua_len(L, -1);
    config.count = (size_t)luaL_checkinteger(L, -1);
    lua_pop(L, 1);
    if (config.count > 0) {
        config.stopwords = malloc(sizeof(*config.stopwords) * config.count);
    }
    for (size_t i = 0; i < config.count; ++i) {
        lua_geti(L, -1, i + 1);
        const char* word = luaL_checkstring(L, -1);
        config.stopwords[i] = word;
        lua_pop(L, 1);
    }
    lua_pop(L, 1);
    return config;
}

static int l_search(lua_State* L) { // {path = "", type = ""}, query, config
    index_accessor accessor = get_accessor(L, 1);
    const char* query = lua_tolstring(L, 2, NULL);
    Configuration config = get_config(L, 3);
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
    free(config.stopwords);
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
    Configuration config = get_config(L, 2);
    Index cindex = create_index(&config);
    lua_pushlightuserdata(L, cindex);
    lua_setfield(L, 1, "_index");
    free(config.stopwords);
    return 0;
}

// index
static int l_free_index(lua_State* L) {
    lua_getfield(L, 1, "_index");
    Index index = (Index)lua_touserdata(L, -1);
    free_index(index);
    return 0;
}

// accessor id -> text
static int l_load_document(lua_State* L) {
    index_accessor accessor = get_accessor(L, 1);
    int doc_id = (int)lua_tointeger(L, 2);
    char* text;
    index_load_document(accessor, doc_id, &text);
    lua_pushlstring(L, text, strlen(text));
    free(text);
    return 1;
}

// index path
static int l_text_index_write(lua_State* L) {
    lua_getfield(L, 1, "_index");
    Index cindex = (Index)lua_touserdata(L, -1);
    lua_pop(L, 1);
    const char* path = luaL_checkstring(L, 2);
    text_index_write(cindex, path);
    return 0;
}

static const struct luaL_Reg luaftslib [] = {
    {"search", l_search}, // связали поле с CFunction
    {"add_document", l_add_document},
    {"free_index", l_free_index},
    {"create_index", l_create_index},
    {"load_document", l_load_document},
    {"text_index_write", l_text_index_write},
    {NULL, NULL}
};

int luaopen_libluafts(lua_State* L) { // функция которая вызывается через require
    luaL_newlib(L, luaftslib); // создали библиотеку (таблицу с функциями) (можно было сделать через таблицу, но в руководстве через библиотеку)
    return 1;
}


