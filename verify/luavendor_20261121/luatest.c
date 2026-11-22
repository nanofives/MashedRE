/* LUA-VENDOR link/run smoke test: the API surface the game's callers use (0x004b7330 lua_open,
   0x004c0510 lua_mathlibopen-shape, 0x004b7a70 lua_dobuffer, 0x004b7200/0x004b7250 register,
   0x004b6fc0 lua_gettop, 0x004b7090 lua_tonumber, 0x004b7480 lua_close). */
#include <stdio.h>
#include "lua.h"
#include "lualib.h"

static double acc;
static int cb(lua_State *L) {
    int n = lua_gettop(L), i;
    for (i = 1; i <= n; i++) acc += lua_tonumber(L, i);
    return 0;
}

int main(void) {
    static const char s[] = "local a = 1/3 x = sin(0.5) + a * 7 cb(x, 2, -0.125, floor(9.75), mod(17, 5)) ";
    lua_State *L = lua_open(0);
    int r;
    lua_mathlibopen(L);
    lua_register(L, "cb", cb);
    r = lua_dobuffer(L, s, sizeof(s) - 1, "t");
    lua_getglobal(L, "x");
    printf("dobuffer=%d acc=%.17g x=%.17g top=%d\n", r, acc, lua_tonumber(L, -1), lua_gettop(L));
    lua_close(L);
    return r;
}
