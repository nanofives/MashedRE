// FLAGS: /O2 /GS /Ob1
/* MATCH-S08 0x00449b30: 100% (regs 100%) by hand (h9). Raw 75.0%. The local pair is two floats copied from the
   float global 0x005f96d4 (an int pair gives one register instead of the eax/ecx copy). "smoke" is the .rdata
   literal 0x005cca08. The call to FUN_00449880 is the tail jmp. */
/* ---- MATCH-S08 preamble (same text in every re/match/s08 source; not part of any function body) ----
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
typedef struct { float a, b; } MatchPair_00449b30;
MGLOBAL(float, DAT_005f96d4, 0x005f96d4);
MGLOBALA(unsigned char, DAT_00683288, 0x1040, 0x00683288);
int FUN_0040bb30(const char *p0);
void FUN_00449880(void);
void FUN_00476ae0(void *p0, MatchPair_00449b30 *p1);
void FUN_00476c10(void *p0, void *p1);
void FUN_004770c0(void *p0, int p1, int p2, int p3);
void FUN_004b6520(void *p0, int p1);
// MATCH: 0x00449b30 FUN_00449b30
void FUN_00449b30(void)
{
  MatchPair_00449b30 pr;

  pr.a = DAT_005f96d4;
  pr.b = DAT_005f96d4;
  FUN_004b6520(DAT_00683288, 0x1040);
  FUN_004770c0(DAT_00683288, 0x895, 0x100, FUN_0040bb30("smoke"));
  FUN_00476ae0(DAT_00683288, &pr);
  FUN_00476c10(DAT_00683288, 0);
  FUN_00449880();
}
