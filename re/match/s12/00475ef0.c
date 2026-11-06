// FLAGS: /O2 /GS /Ob1
/* MATCH-S12 0x00475ef0: matched: Ghidra's size (49 B) stops before the loop tail, match.py
   reports 75.0% on that window; the whole obj symbol (57 B) is BYTE-IDENTICAL (match.py --bytes, relocations
   masked). relocheck OK. By hand (raw 75.0%): DAT_00691698 is a 0xd80-byte array, loop end DAT_00691698 + 0xd80
   compared signed (the original's cmp esi, 0x692418). Residual: none. */
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
MGLOBAL(int, DAT_00613254, 0x00613254);
MGLOBALA(unsigned char, DAT_00691698, 0xd80, 0x00691698);
void FUN_004b6520(void *p0, int p1);
// MATCH: 0x00475ef0 FUN_00475ef0
void FUN_00475ef0(void)
{
  unsigned char *p;
  p = DAT_00691698;
  do {
    FUN_004b6520(p, 0xd8);
    *(int *)(p + 0xc0) = DAT_00613254;
    p = p + 0xd8;
  } while ((int)p < (int)(DAT_00691698 + 0xd80));
}
