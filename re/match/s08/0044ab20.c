// FLAGS: /O2 /GS /Ob1
/* MATCH-S08 0x0044ab20 (144 B): 100% (regs 100%), by hand. Raw converter 66.1%.
   What moved it: the 2-element struct array at 0x00684320 (stride 0x28, same as 0x0044aaa0) and reading the
   handle through e->m_00 at every use instead of a cached local. With a cached local the compiler picked the
   field pointer &m_0c as the induction base (85.2%); repeated e->m_00 makes the element start the base.
   FUN_0055dec0 / FUN_0055e300 are cdecl, 1 argument (add esp,4 after each). relocheck OK, cxx ok. */
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
MGLOBAL(int, DAT_006ce274, 0x006ce274);
void FUN_0047d080(int p0, int p1);
void FUN_0047d100(int p0, int p1);
int FUN_0055dec0(int p0);
int FUN_0055e300(int *p0);
// MATCH: 0x0044ab20 FUN_0044ab20
void FUN_0044ab20(void)
{
  Emitter_0068 *e;
  int i;
  int r;

  r = FUN_0055dec0(DAT_006ce274);
  for (i = 0; i < 2; i++) {
    e = &DAT_00684320[i];
    FUN_0047d080(e->m_08, 1);
    FUN_0047d100(e->m_08, 1);
    FUN_0047d080(e->m_0c, 1);
    FUN_0047d100(e->m_0c, 1);
    if (r == *e->m_00 &&
        (uint)*(ushort *)(e->m_00 + 1) < *(uint *)(r + 0x14) &&
        (*(uint *)(*(int *)(r + 0x6c) + (*(ushort *)(e->m_00 + 1) >> 5) * 4) & (1 << (*(ushort *)(e->m_00 + 1) & 0x1f))) != 0) {
      FUN_0055e300(e->m_00);
    }
  }
}
