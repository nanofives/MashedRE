// FLAGS: /O2 /GS /Ob1
/* MATCH-S13 0x00481780 (h10): 100.0% (regs 100%). Moved it: 0x005cf230 is the float literal 1/360
   (0x3b360b61), written as (1.0f / 360.0f) so 9176 emits fld [p]; fmul [const] (a global was hoisted/
   reordered, 53.7%); the destination address expression written twice (load and store) instead of one
   pointer variable fixed the copy registers (68.3% -> 100%). Residual: none. */
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
typedef struct V3 { float x, y, z; } V3;
MGLOBALA(int, DAT_006c9a78, 4, 0x006c9a78);
int FUN_0057c210(int p0);
// MATCH: 0x00481780 FUN_00481780
void FUN_00481780(V3 *param_1, int param_2)
{
  int *h;
  V3 v;
  h = *(int **)(*(int *)(FUN_0057c210(DAT_006c9a78[param_2]) + 0x10) + 4);
  v = *(V3 *)(h[1] * 0x20 + *(int *)(*(int *)(*h + 0x10) + 8));
  v.x += param_1->x * (1.0f / 360.0f);
  v.y += param_1->y * (1.0f / 360.0f);
  v.z += param_1->z * (1.0f / 360.0f);
  *(V3 *)(h[1] * 0x20 + *(int *)(*(int *)(*h + 0x10) + 8)) = v;
}
