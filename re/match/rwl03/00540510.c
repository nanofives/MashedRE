// FLAGS: /O2 /Ob1 /G5
/* MATCH-RWL03 0x00540510: raw g2s.py output (re/tools/match/g2s.py on decomp_pc.py --port --callees), body
   unchanged; globals moved to MGLOBAL. Score: see verify/matchrwl03_20261108/results.tsv. */
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
typedef unsigned int undefined4;
typedef int code();
MGLOBAL(int, DAT_006181d4, 0x006181d4);
MGLOBAL(int *, DAT_007d4110, 0x007d4110);
MGLOBAL(int, DAT_006181d0, 0x006181d0);
void FUN_004d53b0(void);
typedef int (__stdcall *SC2)(int *, int);
typedef int (__stdcall *SC4)(int *, int, int, int);
typedef int (__stdcall *SC7)(int *, int, int, int, int, int, int);
// MATCH: 0x00540510 FUN_00540510
void FUN_00540510(int param_1,int param_2)
{
  if (DAT_006181d4 != 0) {
    DAT_006181d4 = 0;
    ((SC2)*(int *)(*DAT_007d4110 + 0x1ac))(DAT_007d4110,0);
  }
  if (DAT_006181d0 != *(int *)(param_2 + 0x10)) {
    DAT_006181d0 = *(int *)(param_2 + 0x10);
    ((SC2)*(int *)(*DAT_007d4110 + 0x170))(DAT_007d4110,*(int *)(param_2 + 0x10));
  }
  if (*(int *)(param_1 + 8) != 0) {
    FUN_004d53b0();
    ((SC7)*(int *)(*DAT_007d4110 + 0x148))
              (DAT_007d4110,*(int *)(param_1 + 0xc),*(int *)(param_2 + 0x14),0,
               *(int *)(param_2 + 0x18),*(int *)(param_2 + 0x1c),
               *(int *)(param_2 + 0x20));
    return;
  }
  FUN_004d53b0();
  ((SC4)*(int *)(*DAT_007d4110 + 0x144))
            (DAT_007d4110,*(int *)(param_1 + 0xc),*(int *)(param_2 + 0x14),
             *(int *)(param_2 + 0x20));
}
