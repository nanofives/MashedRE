// FLAGS: /O2 /Ob1 /G5
/* MATCH-RWR04 2026-11-08: 70.3%, was 75.8% on a source that wrote puVar3[1] before puVar3 was assigned (uninitialised pointer). The store now follows the alloc, so the semantics are right and the score dropped. Remaining: param reload / lea scheduling. Class (b). */
/* MATCH-RWL05 0x00563840 (h2): hand, 75.8%, class (b): scheduling of the and/lea chain, permuter 240 s. */
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
MGLOBAL(int, DAT_007d3ff8, 0x007d3ff8);
typedef unsigned int *(*AL)(unsigned int,int);
// MATCH: 0x00563840 FUN_00563840
void FUN_00563840(int param_1,int param_2,int param_3)
{
  unsigned int uVar1;
  unsigned int uVar2;
  unsigned int *puVar3;
  unsigned int uVar4;
  unsigned int uVar5;
  unsigned int uVar6;
  unsigned int uVar7;

  uVar6 = param_1 * 0xc + 0x9bU & 0xfff0;
  uVar7 = uVar6 + 0x11 + param_1 * 2 & 0xfff0;
  uVar4 = uVar7 + 1 + param_3 * 0xc & 0xfffe;
  uVar5 = uVar4 + 5 + param_3 * 2 & 0xfffc;
  uVar1 = uVar5 + 1 + param_2 * 0xc & 0xfffe;
  uVar2 = uVar1 + param_2 * 2 & 0xffff;
  puVar3 = (*(AL *)(DAT_007d3ff8 + 0x108))(uVar2,0x30900);
  puVar3[1] = (unsigned int)(puVar3 + 0x23);
  *puVar3 = uVar2;
  puVar3[2] = (uVar6 & 0xffff) + (unsigned int)puVar3;
  puVar3[3] = (uVar7 & 0xffff) + (unsigned int)puVar3;
  puVar3[4] = (uVar4 & 0xffff) + (unsigned int)puVar3;
  puVar3[5] = (uVar5 & 0xffff) + (unsigned int)puVar3;
  puVar3[6] = (uVar1 & 0xffff) + (unsigned int)puVar3;
  *(unsigned short *)(puVar3 + 7) = (unsigned short)param_1;
  *(unsigned short *)(puVar3 + 8) = (unsigned short)param_2;
  *(unsigned short *)((int)puVar3 + 0x1e) = (unsigned short)param_3;
  puVar3[0x21] = 1;
  puVar3[0x22] = 0;
}
