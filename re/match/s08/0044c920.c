// FLAGS: /O2 /GS /Ob1
/* MATCH-S08 0x0044c920 (121 B): 100% (regs 100%), by hand. Raw converter 70.5%.
   3-element array of 0x58-byte slots at 0x00896120 (the record of 0x0044c5f0 / 0x0044c740; only its size
   matters here). What moved it: unsigned int parameter (fild + 2^32 fix-up from 0x005cc94c is the compiler's
   uint->float), 0x005ce18c as the literal 0.02f, and a plain i / j = i + 1 pair loop. relocheck OK, cxx ok. */
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
typedef struct Slot_c920 {
  int m_00[22];
} Slot_c920;
MGLOBALA(Slot_c920, DAT_00896120, 3, 0x00896120);
void FUN_0044c5f0(Slot_c920 *s, float dt);
void FUN_0044c740(Slot_c920 *a, Slot_c920 *b);
// MATCH: 0x0044c920 FUN_0044c920
void FUN_0044c920(uint param_1)
{
  float dt;
  int i;
  int j;

  dt = (float)param_1 * 0.02f;
  for (i = 0; i < 3; i++) {
    for (j = i + 1; j < 3; j++) {
      FUN_0044c740(&DAT_00896120[i], &DAT_00896120[j]);
    }
  }
  for (i = 0; i < 3; i++) {
    FUN_0044c5f0(&DAT_00896120[i], dt);
  }
}
