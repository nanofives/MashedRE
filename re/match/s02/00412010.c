// FLAGS: /O2 /GS /Ob1
/* 0x00412010: RpClumpDestroy(rec[i].obj->+0x3c) for the 6 0x2c-byte records at DAT_0063bb38
   (0x0063bb38..0x0063bc40), then destroys and clears DAT_0063bc40 if set. Array-of-struct loop: 100%, 61 B BYTE-IDENTICAL.
   Residual: none. */
/* ---- MATCH-S02 preamble (same text in every re/match/s02 source; not part of any function body) ----
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
typedef struct S_0063bb38 {
  int m_obj;          /* +0x00: base of an object whose +0x3c is a clump */
  undefined1 pad[0x28];
} S_0063bb38;

MGLOBALA(S_0063bb38, DAT_0063bb38, 6, 0x0063bb38);
MGLOBAL(int, DAT_0063bc40, 0x0063bc40);

int RpClumpDestroy(int param_1);

// MATCH: 0x00412010 FUN_00412010
void FUN_00412010(void)
{
  int i;

  for (i = 0; i < 6; i++) {
    RpClumpDestroy(*(int *)(DAT_0063bb38[i].m_obj + 0x3c));
  }
  if (DAT_0063bc40 != 0) {
    RpClumpDestroy(DAT_0063bc40);
    DAT_0063bc40 = 0;
  }
}
