// FLAGS: /O2 /GS /Ob1
/* MATCH-S07 0x0042c220 (h7): 100%. From the raw g2s output (97.1%): the thunk 0x00496900 (-> 0x00497450) returns
   int (setne cl; mov eax,ecx), not bool, which fixed test al,al vs test eax,eax. Globals as int arrays (the
   0x007f1a14 records are 16 bytes, field 0 read). Residual: none. */
/* ---- MATCH-S07 preamble (same text in every re/match/s07 source; not part of any function body) ----
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
MGLOBALA(int, DAT_0067ea10, 12, 0x0067ea10);
MGLOBALA(int, DAT_007f1a14, 16, 0x007f1a14);
int FUN_0040e470(int p0);
int FUN_00496900(int p0);
// MATCH: 0x0042c220 FUN_0042c220
int FUN_0042c220(void)
{
  int iVar1;
  int uVar2;
  int *piVar3;
  int iVar4;

  uVar2 = 0;
  iVar4 = 0;
  piVar3 = DAT_007f1a14;
  do {
    if (((FUN_0040e470(iVar4) == 1) && (iVar1 = *piVar3, -1 < iVar1)) && (iVar1 < 0xc)) {
      if (FUN_00496900(iVar1) == 0) {
        uVar2 = 1;
        DAT_0067ea10[*piVar3] = 1;
      }
    }
    piVar3 = piVar3 + 4;
    iVar4 = iVar4 + 1;
  } while ((int)piVar3 < (int)(DAT_007f1a14 + 16));
  return uVar2;
}
