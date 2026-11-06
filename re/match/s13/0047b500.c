// FLAGS: /O2 /GS /Ob1
/* 0x0047b500 (Lua C function, appends a KtcSub to KtcRec.sub[], returns its global index a*8+j): 100%
   (regs 100%), relocheck OK. From raw 56.4%: KtcRec/KtcSub struct (sub[] at +0x148, stride 0x45c), r and s
   pointers, lua_pushnumber(L, (double)(j + a*8)). Residual: none. */
/* ---- MATCH-S13 preamble (same text in every re/match/s13 source; not part of any function body) ----
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
typedef struct lua_State lua_State;
typedef struct KtcSub {
  int f00;
  int f04;
  int f08;
  int ids[8];          /* +0x00c */
  int count;           /* +0x02c */
  int f30;             /* +0x030 */
  int f34;             /* +0x034 */
  int data[8][0x20];   /* +0x038 */
  int counts[8];       /* +0x438 */
  int f458;            /* +0x458 */
} KtcSub;
typedef struct KtcEnt {
  int f00;
  int f04;
  int f08;
  int f0c;
  int f10;
  int f14;
  int f18;
} KtcEnt;
typedef struct KtcRec {
  char name[0x20];
  int ids[8];          /* +0x020 */
  int count;           /* +0x040 */
  int a[0x20];         /* +0x044 */
  int b[0x20];         /* +0x0c4 */
  int total;           /* +0x144 */
  KtcSub sub[8];       /* +0x148 */
  int nsub;            /* +0x2428 */
  KtcEnt ent[8];       /* +0x242c */
  int nent;            /* +0x250c */
} KtcRec;
typedef struct T110 {
  int base;
  int f04;
  int n;
  int pad0c[(0x110 - 0x0c) / 4];
} T110;
MGLOBALA(KtcRec, DAT_0086ece0, 8, 0x0086ece0);
int FUN_004b6fc0(lua_State *L);
double FUN_004b7090(lua_State *L, int idx);
void FUN_004b7140(lua_State *L, double n);
// MATCH: 0x0047b500 FUN_0047b500
int FUN_0047b500(lua_State *L)
{
  int n;
  int a, b, c;
  int j;
  KtcRec *r;
  KtcSub *s;
  n = FUN_004b6fc0(L);
  a = (int)FUN_004b7090(L, n - 2);
  b = (int)FUN_004b7090(L, n - 1);
  c = (int)FUN_004b7090(L, n);
  r = &DAT_0086ece0[a];
  j = r->nsub;
  s = &r->sub[j];
  s->f30 = b;
  s->f34 = 1 - c;
  s->f458 = 0;
  r->nsub++;
  FUN_004b7140(L, (double)(j + a * 8));
  return 1;
}
