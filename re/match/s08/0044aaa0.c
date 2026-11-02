// FLAGS: /O2 /GS /Ob1
/* MATCH-S08 0x0044aaa0 (123 B): 100% (regs 100%), by hand. Raw converter 62.5%.
   What moved it: the 2-element array at 0x00684320 (stride 0x28) written as a struct array indexed by i; the
   compiler strength-reduces it to the field pointer 0x0068432c that Ghidra named. Local is float[16].
   DAT_005f96dc element count (3) is [UNCERTAIN]: only its address is used here. relocheck OK, cxx ok. */
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
typedef struct Emitter_0068 {
  int *m_00;
  int m_04;
  int m_08;
  int m_0c;
  int m_10;
  int m_14[5];
} Emitter_0068;
MGLOBALA(Emitter_0068, DAT_00684320, 2, 0x00684320);
MGLOBALA(float, DAT_005f96dc, 3, 0x005f96dc);
void FUN_0047d080(int p0, int p1);
void FUN_0047d180(int p0, float *p1);
void FUN_004b47e0(float *p0, int p1, float p2);
void FUN_004c51a0(float *p0, float *p1, int p2);
// MATCH: 0x0044aaa0 FUN_0044aaa0
void FUN_0044aaa0(void)
{
  int i;
  float m[16];

  for (i = 0; i < 2; i++) {
    FUN_0047d080(DAT_00684320[i].m_08, 1);
    FUN_0047d080(DAT_00684320[i].m_0c, 1);
    DAT_00684320[i].m_10 = 0;
    FUN_004b47e0(m, DAT_00684320[i].m_04, 0.0f);
    FUN_0047d180(DAT_00684320[i].m_0c, m);
    FUN_004c51a0(m, DAT_005f96dc, 2);
    FUN_0047d180(DAT_00684320[i].m_08, m);
  }
}
