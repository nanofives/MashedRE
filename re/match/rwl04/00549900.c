// FLAGS: /O2 /Ob1 /G5
/* MATCH-RWL04 0x00549900 h1: best 94.9% (regs 94.9%, ops 94.9%). Residue: orig second operand (float)b is 'fisub dword [esp]' on the zero-extended temp, ours 'fild qword; fsubp'. Tried unsigned/int/long/double casts and locals. Class (a). */
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
typedef struct { unsigned int a, b; float c, d; } S4;
typedef struct { S4 s; float e, f; } D6;
MGLOBAL(int, DAT_007d3ff8, 0x007d3ff8);
MGLOBAL(int, DAT_007dc720, 0x007dc720);
MGLOBAL(float, DAT_005cc320, 0x005cc320);
// MATCH: 0x00549900 FUN_00549900
int FUN_00549900(S4 *param_1)
{
  float fVar1;
  D6 *puVar2;
  
  puVar2 = (D6 *)(DAT_007dc720 + DAT_007d3ff8);
  puVar2->s = *param_1;
  fVar1 = DAT_005cc320 / (param_1->d - param_1->c);
  puVar2->f = fVar1;
  puVar2->e = ((float)param_1->a - (float)param_1->b) * fVar1;
  return 1;
}
