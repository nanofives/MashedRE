// FLAGS: /O2 /GS /Ob1
/* MATCH-S08 0x004464c0: 100% (regs 100%) by hand (h8). Raw did not compile (Ghidra typed callee 0x00445aa0 as
   (float, int *)). Fix: the record table DAT_008964c0 as int[54] (0xd8-byte records, kind at +4), all three
   callees (0x00445aa0, 0x00441d40, 0x00442440) as cdecl (int *rec, int arg) (each call is followed by add esp, 8). */
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
MGLOBAL(int, DAT_00898994, 0x00898994);
MGLOBALA(int, DAT_008964c0, 54, 0x008964c0);
void FUN_00441d40(int *p0, int p1);
void FUN_00442440(int *p0, int p1);
void FUN_00445aa0(int *p0, int p1);
// MATCH: 0x004464c0 FUN_004464c0
void FUN_004464c0(int param_1)
{
  int i;
  int *rec;

  i = 0;
  if (0 < DAT_00898994) {
    rec = DAT_008964c0;
    do {
      if (rec[1] == 0) {
        FUN_00445aa0(rec, param_1);
      }
      if (rec[1] == 1) {
        FUN_00441d40(rec, param_1);
      }
      if (rec[1] == 2) {
        FUN_00442440(rec, param_1);
      }
      i = i + 1;
      rec = rec + 0x36;
    } while (i < DAT_00898994);
  }
}
