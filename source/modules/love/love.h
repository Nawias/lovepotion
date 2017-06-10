#pragma once

// Forward declare lua_State.
struct lua_State;
extern bool LUA_ERROR;

/*
	@brief returns the LOVE version :: Lua
	@return MAJOR, MINOR, REVISION, and CODENAME
*/
int loveGetVersion(lua_State * L);

int loveScan(lua_State * L);

/*
	@brief initializes LOVE, its modules, and their classes
*/
int loveInit(lua_State * L);