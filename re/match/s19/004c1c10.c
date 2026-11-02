// FLAGS: /O2 /GS /Ob1
/* MATCH-S19 0x004c1c10 (h2): 100%, regs 100%. Hand: RwError-style {type,code} local, range test 0<t<=2. Residual: none. */
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
MGLOBALA(char, s_Invalid_projection_type_specifie_00617fb8, 1, 0x00617fb8);
typedef struct { int type; int code; } RwError_t;
int FUN_004c0e50(int p0);
void FUN_004c1a70(int p0);
int FUN_004d7ff0(int p0, ...);
RwError_t *FUN_004d8480(RwError_t *p0);
// MATCH: 0x004c1c10 FUN_004c1c10
int FUN_004c1c10(int param_1, int param_2)
{
  RwError_t err;
  if ((param_2 > 0) && (param_2 <= 2)) {
    *(int *)(param_1 + 0x14) = param_2;
    if (*(int *)(param_1 + 4) != 0) {
      FUN_004c0e50(*(int *)(param_1 + 4));
    }
    FUN_004c1a70(param_1);
    return param_1;
  }
  err.type = 1;
  err.code = FUN_004d7ff0(0x80000003, s_Invalid_projection_type_specifie_00617fb8);
  FUN_004d8480(&err);
  return 0;
}
