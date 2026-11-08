// FLAGS: /O2 /Ob1 /G5
/* MATCH-S23 0x00534920: 100% (regs 100%). RenderWare-style plugin attach: RwEngineRegisterPlugin-shaped
   FUN_004c2d90(0x10, 0x10c, ctor FUN_00534990, dtor FUN_00534950), offset kept in 0x007dc578, returns offset >= 0.
   Residual: none. */
/* ---- MATCH-S11 preamble (same text in every re/match/s11 source; not part of any function body) ----
   C under the DDK (match.py), C++ under VS2022 when the port includes this file (MATCH_PORT). */
#ifndef MATCH_PREAMBLE
#define MATCH_PREAMBLE
typedef unsigned char undefined; typedef unsigned char undefined1; typedef unsigned short undefined2;
typedef unsigned int undefined3; typedef unsigned int undefined4; typedef unsigned __int64 undefined8;
typedef unsigned int uint; typedef unsigned short ushort; typedef unsigned char uchar; typedef unsigned char byte;
typedef unsigned long ulong; typedef __int64 longlong; typedef unsigned __int64 ulonglong; typedef signed char sbyte;
typedef unsigned short word; typedef unsigned int dword; typedef long double float10;
#ifndef __cplusplus
typedef unsigned char bool;
#endif
#ifdef __cplusplus
/* port: a global is a reference to its original address (the exe maps the original data range) */
#define MGLOBAL(T, n, a) static T &n = *(T *)(a)
#define MGLOBALA(T, n, N, a) static T (&n)[N] = *(T (*)[N])(a)
#else
#define MGLOBAL(T, n, a) extern T n
#define MGLOBALA(T, n, N, a) extern T n[N]
#endif
#endif
/* ---- end preamble ---- */
typedef void *(*RwPluginObjectConstructor)(void *object, int offsetInObject, int sizeInObject);
typedef void *(*RwPluginObjectDestructor)(void *object, int offsetInObject, int sizeInObject);

MGLOBAL(int, DAT_007dc578, 0x007dc578);

int FUN_004c2d90(int size, unsigned pluginID, RwPluginObjectConstructor ctor, RwPluginObjectDestructor dtor);
void *FUN_00534990(void *object, int offsetInObject, int sizeInObject);
void *FUN_00534950(void *object, int offsetInObject, int sizeInObject);

// MATCH: 0x00534920 FUN_00534920
int FUN_00534920(void)
{
    DAT_007dc578 = FUN_004c2d90(0x10, 0x10c, FUN_00534990, FUN_00534950);
    return DAT_007dc578 >= 0;
}
