// FLAGS: /O2 /Ob1 /G5
/* 0x005a8060: 100.0%, 744 B BYTE-IDENTICAL (PORT-TOP-AUDIO-5A8F30-STOPS, 2026-11-16). The bit scan is the nested macro HB32(M) of
   re/match/s25/005a8f30.c (M = 1 << FUN_005aee20(0x28), re-evaluated at every use); the tail (list heads, two FUN_005aeea0
   events, FUN_005aef00 / FUN_005aef30 thread start, CloseHandle unwind) is the previous source unchanged. Previous source (40.0%)
   kept in D:/MashedWT/PORT-TOP-AUDIO-5A8F30-STOPS_data/005a8060_before.c. */
/* MATCH-S25 0x005a8060: raw g2s.py output (re/tools/match/g2s.py on decomp_pc.py --port --callees), body
   unchanged; globals moved to MGLOBAL. Score: see verify/matchs25_20261107/results.tsv. */
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
typedef unsigned char undefined1;
typedef unsigned short undefined2;
typedef unsigned int undefined4;
typedef unsigned int uint;
typedef unsigned char byte;
typedef int (__cdecl code)(void);
typedef void *HANDLE;
typedef int BOOL;
typedef long LONG;
void FUN_005a8460(void);
MGLOBAL(undefined4, DAT_007dcad8, 0x007dcad8);
void FUN_005a84d0(void);
MGLOBAL(undefined1, DAT_007dcadc, 0x007dcadc);
void FUN_005a8680(void);
MGLOBAL(undefined4, DAT_0063307c, 0x0063307c);
MGLOBAL(undefined4, DAT_007dcb4c, 0x007dcb4c);
MGLOBAL(undefined4, DAT_007dcb50, 0x007dcb50);
MGLOBAL(undefined4, DAT_007dcae0, 0x007dcae0);
MGLOBALA(signed char, DAT_00617ff8, 256, 0x00617ff8);
MGLOBAL(undefined1, DAT_005e616c, 0x005e616c);
MGLOBAL(undefined1, DAT_007dcb54, 0x007dcb54);
void FUN_005a8390(void);
void FUN_005a8560(void);
__declspec(dllimport) BOOL __stdcall CloseHandle(HANDLE p0);
undefined4 FUN_005a9e10(undefined4 p0, undefined4 p1, undefined4 p2);
int FUN_005aee20(uint p0);
uint FUN_005aeea0(undefined4 * p0, LONG p1, LONG p2);
undefined4 FUN_005aef00(undefined4 * p0, undefined4 p1, undefined4 p2, undefined4 p3);
undefined4 * FUN_005aef30(undefined4 * p0, undefined4 p1);
MGLOBAL(int, _DAT_007dcadc, 0x007dcadc);
#define SH_005a8060 (1 << FUN_005aee20(0x28))
#define M_005a8060 (((short)SH_005a8060 != 0) ? (SH_005a8060 & 0xffff) : 1)
#define HB16_005a8060(w) ((((w) & 0xff00) != 0) ? DAT_00617ff8[(w) >> 8] + 7 : DAT_00617ff8[(w)] - 1)
#define HB32_005a8060(v) ((((v) & 0xffff0000) != 0) ? HB16_005a8060((v) >> 16) + 16 : HB16_005a8060(v))
// MATCH: 0x005a8060 FUN_005a8060
void FUN_005a8060(int param_1)

{
  int iVar2;

  FUN_005a9e10(param_1,(undefined4)&DAT_005e616c,0);
  *(undefined4 *)(param_1 + 0x40) = 0xd78;
  iVar2 = HB32_005a8060(M_005a8060);
  *(uint *)(param_1 + 0x40) = *(uint *)(param_1 + 0x40) | iVar2 << 0x1c;
  *(code **)(param_1 + 0x28) = (code *)FUN_005a8390;
  *(code **)(param_1 + 0x34) = (code *)FUN_005a8460;
  *(code **)(param_1 + 0x30) = (code *)FUN_005a8560;
  *(code **)(param_1 + 0x3c) = (code *)FUN_005a8680;
  *(undefined2 *)(param_1 + 0x44) = 0xc;
  DAT_007dcad8 = (undefined4)&DAT_007dcad8;
  _DAT_007dcadc = (int)&DAT_007dcad8;
  iVar2 = FUN_005aeea0((undefined4 *)&DAT_007dcae0,1,1);
  if (iVar2 != 0) {
    iVar2 = FUN_005aeea0((undefined4 *)&DAT_007dcb50,0,1);
    if (iVar2 != 0) {
      DAT_007dcb4c = 0;
      iVar2 = FUN_005aef00((undefined4 *)&DAT_007dcb54,(undefined4)FUN_005a84d0,DAT_0063307c,0);
      if ((iVar2 != 0) && (iVar2 = (int)FUN_005aef30((undefined4 *)&DAT_007dcb54,0), iVar2 != 0)) {
        return;
      }
      CloseHandle((HANDLE)DAT_007dcb50);
    }
    CloseHandle((HANDLE)DAT_007dcae0);
  }
  return;
}
