// FLAGS: /O2 /GS /Ob1
/* MATCH-S05 0x00422ac0 (42 B): 100% exact (regs 100, ops 100) by hand. Raw g2s 61.5%.
   A 16-byte struct copy into a 0xf40-stride record array (field at 0x006412e8; same stride as the 0x006403e8
   records of 0x00422a80). Element-wise int copies, a V4 cast copy and this struct-array form all give 100%.
   Residual: none. */
/* ---- MATCH-S05 preamble (same text in every re/match/s05 source; not part of any function body) ----
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
typedef struct { int a, b, c, d; } V4_ac0;
typedef struct { V4_ac0 v; char pad[0xf30]; } Rec_ac0;   /* stride 0xf40 */
MGLOBALA(Rec_ac0, DAT_006412e8, 4, 0x006412e8);
// MATCH: 0x00422ac0 FUN_00422ac0
void FUN_00422ac0(int param_1, V4_ac0 *param_2)
{
  DAT_006412e8[param_1].v = *param_2;
}
