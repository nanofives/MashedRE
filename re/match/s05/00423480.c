// FLAGS: /O2 /GS /Ob1
/* 0x00423480 (171 B Ghidra size; real end 0x00423533). Best 96.8% exact (regs 96.8%) = matched: the whole obj
   symbol is BYTE-IDENTICAL (match.py --bytes, 179 B, relocations masked). The two "missing" instructions
   (add esp, 0x44; jmp 0x004a2be9 = /GS cookie check tail) sit past Ghidra's size.
   Raw g2s 31.1% (cookie read as unaff_retaddr, buffers expanded). Hand form: two char[32] = "" locals
   (declared path first: 90.5% with the other order, init order and stack slots swap), sprintf
   FUN_004a2b60(name, "AI%d.AI" @0x005cd328, FUN_00426c00()), then intrinsic strcpy/strcat into the buffer
   at 0x00644110. Its size 64 is the gap to the next global seen (0x00644150), not a known extent.
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
MGLOBALA(char, DAT_00644110, 64, 0x00644110);
#ifdef __cplusplus
extern "C" char *strcpy(char *, const char *);
extern "C" char *strcat(char *, const char *);
#else
char *strcpy(char *, const char *);
char *strcat(char *, const char *);
#endif
#pragma intrinsic(strcpy, strcat)
int FUN_00426c00(void);
int FUN_004a2b60(char *dst, const char *fmt, ...);

// MATCH: 0x00423480 FUN_00423480
char *FUN_00423480(void)
{
  char path[32] = "";
  char name[32] = "";
  FUN_004a2b60(name, "AI%d.AI", FUN_00426c00());
  strcpy(DAT_00644110, path);
  strcat(DAT_00644110, name);
  return DAT_00644110;
}
