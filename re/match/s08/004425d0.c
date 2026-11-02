// FLAGS: /O2 /GS /Ob1
/* MATCH-S08 0x004425d0: BYTE-IDENTICAL by hand (raw 80%). match.py reports 80% only because Ghidra's size (36)
   stops before the DAT_0089898c store and ret; match.py --bytes: 45 B whole symbol, 0 differing unmasked bytes.
   Array of 32 records of 0xd8 bytes at 0x008964c0, first dword cleared by an index loop. */
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
typedef struct { int m0; char pad[0xd4]; } Cam_004425d0;
MGLOBALA(Cam_004425d0, DAT_008964c0, 32, 0x008964c0);
MGLOBAL(int, DAT_0089898c, 0x0089898c);
// MATCH: 0x004425d0 FUN_004425d0
void FUN_004425d0(void)
{
  int i;

  for (i = 0; i < 32; i++) {
    DAT_008964c0[i].m0 = 0;
  }
  DAT_0089898c = 0;
}
