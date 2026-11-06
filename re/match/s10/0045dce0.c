// FLAGS: /O2 /GS /Ob1
/* MATCH-S10 0x0045dce0 (h3): 100% (regs 100%), byte-identical. Raw converter 41.2%: the vector copy is one
   12-byte struct assignment from param_2 (a vector pointer, also passed to FUN_005a6d60). Residual: none. */
/* ---- MATCH-S10 preamble (same text in every re/match/s10 source; not part of any function body) ----
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
typedef struct {
  float x;
  float y;
  float z;
} V3_0045dce0;
MGLOBAL(int, DAT_00604570, 0x00604570);
MGLOBAL(int, DAT_0068f640, 0x0068f640);
int FUN_00492d10(void);
int FUN_005a6d60(int p0, int p1, int p2, V3_0045dce0 *p3);
// MATCH: 0x0045dce0 FUN_0045dce0
void FUN_0045dce0(int param_1,V3_0045dce0 *param_2,int param_3)
{
  int iVar2;
  int iVar1;

  if (FUN_00492d10() == 3) {
    iVar2 = (&DAT_00604570)[param_1 * 0x25] + param_3;
    iVar1 = (&DAT_0068f640)[iVar2 * 2];
    if ((*(unsigned char *)(iVar1 + 0x34) & 2) != 0) {
      *(V3_0045dce0 *)(iVar1 + 0x58) = *param_2;
      if (*(int *)((&DAT_0068f640)[iVar2 * 2] + 0x30) != 0) {
        FUN_005a6d60(*(int *)((&DAT_0068f640)[iVar2 * 2] + 0x30),0,1,param_2);
      }
    }
  }
}
