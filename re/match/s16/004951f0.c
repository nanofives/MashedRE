// FLAGS: /O2 /GS /Ob1
/* MATCH-S16 0x004951f0 (h1): 100% (regs 100%), by hand. Residual: none. Stock bytes (the skip_selector boot
   patch is not in MASHED.exe.unpatched). What moved it (81.8% -> 100%): the condition through an int flag
   (ok = 1 when FUN_00499400 != 0 and DAT_007719e0 == 0, then if (!ok) { FUN_00498b60(); return 0; }). The
   direct if / else, goto and nested forms all put the failure block last. FUN_00496400 is the varargs log. */
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
MGLOBAL(undefined4, DAT_006147bc, 0x006147bc);
MGLOBAL(int, DAT_007719e0, 0x007719e0);
MGLOBAL(int, DAT_00771e58, 0x00771e58);
MGLOBALA(char, s_VertexShaderVersion____x_005cfe54, 26, 0x005cfe54);
MGLOBALA(char, s_PixelShaderVersion____x_005cfe38, 25, 0x005cfe38);
void FUN_00496400(const char *fmt, ...);
void FUN_00498b60(void);
undefined4 FUN_00498bf0(void);
undefined4 FUN_00499400(undefined4 param_1);
int FUN_004cbb60(void);
__declspec(dllimport) int __stdcall ShowCursor(int bShow);
// MATCH: 0x004951f0 FUN_004951f0
undefined4 FUN_004951f0(void)
{
  int ok = 0;
  if (FUN_00499400(DAT_006147bc) != 0 && DAT_007719e0 == 0) ok = 1;
  if (!ok) {
    FUN_00498b60();
    return 0;
  }
    if (FUN_00498bf0() != 0) {
      ShowCursor(0);
    }
    DAT_00771e58 = FUN_004cbb60();
    FUN_00496400(s_VertexShaderVersion____x_005cfe54, *(uint *)(DAT_00771e58 + 0xc4) & 0xffff);
    FUN_00496400(s_PixelShaderVersion____x_005cfe38, *(uint *)(DAT_00771e58 + 0xcc) & 0xffff);
    return 1;
}
