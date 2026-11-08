// FLAGS: /O2 /Ob1 /G5
/* MATCH-S21 0x004d5190 (h1, by hand): best 79.0% exact, regs 79.0% (raw 24.8%). Allocates a slot record out of a
   pool: clears the 10-dword output, RWERROR 0x29 / 0x28 when the counts at +0x2c / +0x20 exceed 32, RWERROR 0x2a and
   return 0 when +0x2c >= the plugin limit (RwGlobals + DAT_00911ae0 + 0x38), else fills n slots with -1, zeroes a
   12-byte triple and bumps pool->count. Moves: varargs _rwerror + RwError struct, real out/pool structs, ok=1 before
   the clear loop, ok=0 placements found by the permuter (only the semantics-preserving ones kept; 54.3 -> 79.0).
   Residual: `ok` is spilled to a stack slot in ours, kept in ebp in the original (with ebp reused as the constant 1
   for the first pluginID). Tried: register on ok, ok init positions (4), memset intrinsic vs loop, pointer vs
   index fill, permuter (1036 compiles). Class (b). C++ ok. */
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
typedef struct { int pluginID; int errorCode; } FUN_004d5190_err;
typedef struct { int pad; int count; char *mem; } FUN_004d5190_pool;
typedef struct {
  int src;
  uint n;
  int *slots;
  int pad0c[4];
  int *trip;
  int a;
  int b;
} FUN_004d5190_out;
MGLOBAL(char *, DAT_007d3ff8, 0x007d3ff8);
MGLOBAL(int, DAT_00911ae0, 0x00911ae0);
int FUN_004d7ff0(int code, ...);
FUN_004d5190_err *FUN_004d8480(FUN_004d5190_err *err);
// MATCH: 0x004d5190 FUN_004d5190
int FUN_004d5190(FUN_004d5190_pool *param_1,FUN_004d5190_out *param_2,int param_3)

{
  FUN_004d5190_err err;
  uint n;
  uint i;
  int ok;
  int *p;

  n = *(uint *)(param_3 + 0x2c);
  ok = 1;
  for (i = 0; i < 10; i++) {
    ((int *)param_2)[i] = 0;
  }
  if (n > 0x20) {
    err.pluginID = 1;
    err.errorCode = FUN_004d7ff0(0x29);
    ok = 0;
    FUN_004d8480(&err);
  }
  if (*(uint *)(param_3 + 0x20) > 0x20) {
    ok = 0;
    err.pluginID = 1;
    err.errorCode = FUN_004d7ff0(0x28);
    FUN_004d8480(&err);
  }
  if (n >= *(uint *)(DAT_007d3ff8 + DAT_00911ae0 + 0x38)) {
    err.errorCode = FUN_004d7ff0(0x2a);
    FUN_004d8480(&err);
    err.pluginID = 1;
    return 0;
  }
  if (ok) {
    p = (int *)(param_1->mem + param_1->count * 0x80 +
                *(uint *)(DAT_007d3ff8 + DAT_00911ae0 + 0x38) * 0x28);
    param_2->slots = p;
    param_2->n = n;
    for (i = 0; i < param_2->n; i++) {
      *p++ = -1;
    }
    p = (int *)(param_1->mem +
                (param_1->count + *(uint *)(DAT_007d3ff8 + DAT_00911ae0 + 0x38) * 14) * 12);
    p[0] = 0;
    p[1] = 0;
    p[2] = 0;
    param_2->trip = p;
    param_2->a = 0;
    param_2->b = 0;
    param_2->src = param_3;
    param_1->count++;
  }
  return ok;
}
