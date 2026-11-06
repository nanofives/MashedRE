// FLAGS: /O2 /GS /Ob1 /G5
/* MATCH-S19 0x004c6140 (h6): 100.0%, regs 100.0%, relocheck OK, cxx ok. Mip-level texture name suffix: char[3] 'm' + hex digit from the 0x005d8b68 digit string (level 1..15, else 0) + NUL, appended with the RwGlobals+0xd4 string-cat slot to the name and, when non-empty, the mask name. Ghidra's __thiscall/__fastcall prototype was wrong: plain cdecl (char *, char *, unsigned char). */
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
typedef char *(*RwStrcatFn)(char *, const char *);
MGLOBAL(int, DAT_007d3ff8, 0x007d3ff8);
MGLOBALA(char, s_0123456789abcdef_005d8b68, 17, 0x005d8b68);
// MATCH: 0x004c6140 FUN_004c6140
int FUN_004c6140(char *name, char *maskName, unsigned char mipLevel)
{
  char suffix[3];
  suffix[0] = 'm';
  if (mipLevel > 0 && mipLevel < 16) {
    suffix[1] = s_0123456789abcdef_005d8b68[mipLevel];
  } else {
    suffix[1] = 0;
  }
  suffix[2] = 0;
  if (suffix[1]) {
    (*(RwStrcatFn *)(DAT_007d3ff8 + 0xd4))(name, suffix);
    if (maskName && *maskName) {
      (*(RwStrcatFn *)(DAT_007d3ff8 + 0xd4))(maskName, suffix);
    }
  }
  return 1;
}
