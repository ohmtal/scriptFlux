#define _DEFAULT_SOURCE //enable posix
#include <stdio.h>
#include <string.h> // str..
#include <stdlib.h> // free
#include "lua.h"
#include "lauxlib.h" // luaL_checknumber

static int fluxFunc_Hello(lua_State* L) {
   const char* name = luaL_optstring(L, 1, "nobody");
   printf("\n\n ~~~ Hello %s ~~~~ \n\n", name);
   return 0; // nothing to return
}

static int fluxFunc_add(lua_State* L) {

    if (lua_gettop(L) < 2) {
        return luaL_error(L, "Error we need 2 parameters: %d", lua_gettop(L));
    }

    double a = luaL_checknumber(L, 1);
    double b = luaL_checknumber(L, 2);

    double result = a + b;

    lua_pushnumber(L, result);

    return 1;
}
// -----------------------------------------------------------------------------
// A Struct:
//
// TODO p.x = 4
/*
 p = Point2.new(2.2,3.3);
 p:print();
 p:add(4);
 p:print();

 collectgarbage("collect");
*/
// -----------------------------------------------------------------------------
typedef struct {
    double x;
    double y;
    char* name; // for destructor example
} Point2;

#define LUA_POINT2_METATABLE "Flux.Point2"

// Constructor
static int vector_new(lua_State* L) {
    double x = luaL_optnumber(L, 1, 0.0);
    double y = luaL_optnumber(L, 2, 0.0);
    const char* name = luaL_optstring(L, 3, "unknown");

    // reserve space
    Point2* p = (Point2*)lua_newuserdatauv(L, sizeof(Point2), 0);

    // set
    p->x = x;
    p->y = y;

    p->name = (char*)malloc(strlen(name) + 1);
    if (p->name != NULL) {
        strcpy(p->name, name);
    }

    // setuo meta table
    luaL_getmetatable(L, LUA_POINT2_METATABLE);
    lua_setmetatable(L, -2); // Link

    return 1;
}


// garbage collection
static int Point2_gc(lua_State* L) {
    Point2* p = (Point2*)luaL_checkudata(L, 1, LUA_POINT2_METATABLE);

    printf("DEBUG: Point2 release: '%s'.\n", p->name);

    if (p->name != NULL) {
        free(p->name);
        p->name = NULL;
    }
    return 0;
}

// ---------- access fields / methods -------------
// this is called when we use the DOT like print(p.x); or p.print()
static int Point2_meta_index(lua_State* L) {
    //param 1 is the object
    Point2* p = (Point2*)luaL_checkudata(L, 1, LUA_POINT2_METATABLE);

    // param 2 is the field
    const char* key = luaL_checkstring(L, 2);

    if (strcmp(key, "x") == 0) {
        lua_pushnumber(L, p->x);
        return 1;
    }
    if (strcmp(key, "y") == 0) {
        lua_pushnumber(L, p->y);
        return 1;
    }

    if (strcmp(key, "name") == 0) {
        lua_pushstring(L, p->name);
        return 1;
    }

    // fallback to methods
    luaL_getmetatable(L, LUA_POINT2_METATABLE);
    lua_pushstring(L, key);
    lua_gettable(L, -2);

    return 1;
}


// -------- methods -------

static int Point2_print(lua_State* L) {
    Point2* p = (Point2*)luaL_checkudata(L, 1, LUA_POINT2_METATABLE);

    printf("Point2(X: %.2f, Y: %.2f, Text:%s)\n", p->x, p->y, p->name);
    return 0;
}

static int Point2_add(lua_State* L) {
    Point2* p1 = (Point2*)luaL_checkudata(L, 1, LUA_POINT2_METATABLE);
    double amount = luaL_checknumber(L, 2);

    // modifix value
    p1->x += amount;
    p1->y += amount;

    return 0;
}

// -------- Opererator + -------

static int Point2_meta_add(lua_State* L) {
    Point2* v1 = (Point2*)luaL_checkudata(L, 1, LUA_POINT2_METATABLE);
    Point2* v2 = (Point2*)luaL_checkudata(L, 2, LUA_POINT2_METATABLE);

    // add result to stack at 0
    Point2* result = (Point2*)lua_newuserdatauv(L, sizeof(Point2), 0);

    result->x = v1->x + v2->x;
    result->y = v1->y + v2->y;
    result->name = (char*)malloc(16);
    if (result->name != NULL) {
        strcpy(result->name, "debug did +");
    }

    // set result on -2
    luaL_getmetatable(L, LUA_POINT2_METATABLE);
    lua_setmetatable(L, -2);

    return 1;
}

// --------

// Methods of Point2
static const struct luaL_Reg Point2_methods[] = {
    {"print", Point2_print},
    {"add", Point2_add},
    {NULL, NULL}
};

// static constructor factory
static const struct luaL_Reg Point2_factory[] = {
    {"new", vector_new},
    {NULL, NULL}
};

void register_Point2_object(lua_State* L) {
    luaL_newmetatable(L, LUA_POINT2_METATABLE);

    // Methods
    luaL_setfuncs(L, Point2_methods, 0);

    // Destructor
    lua_pushcfunction(L, Point2_gc);
    lua_setfield(L, -2, "__gc");

    // Operator
    lua_pushcfunction(L, Point2_meta_add);
    lua_setfield(L, -2, "__add");

    // look what we got when a "dot" is set
    lua_pushcfunction(L, Point2_meta_index);
    lua_setfield(L, -2, "__index");

    // Factory
    luaL_newlib(L, Point2_factory);
    lua_setglobal(L, "Point2");
}

// -----------------------------------------------------------------------------
// Bind
// -----------------------------------------------------------------------------
static const struct luaL_Reg flux_library[] = {
    {"add", fluxFunc_add},
    {NULL, NULL}
};

void flux_Bind(lua_State* L) {

    lua_register(L, "hello", fluxFunc_Hello);
    // // lua_register(L, "fluxAdd", fluxFunc_add);

    // as lib :)
    luaL_newlib(L, flux_library);
    lua_setglobal(L, "flux");

    // my point2
    register_Point2_object(L);
}
