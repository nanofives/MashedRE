// FLAGS: /O2 /GS /Ob1
/* MATCH-S08 0x0044c740 (259 B): 100% (regs 100%), by hand. Raw converter 26.3%.
   Two 0x58-byte slots (same record as 0x0044c5f0; only +0x08..+0x14 are used here). What moved it: the
   hi/lo pointer pair set in both branches, the radius sum computed before the distance, every .rdata float as
   a literal (0x005d757c = 0.0f, 0x005cc320 = 1.0f, 0x005cd50c = -0.5f, 0x005cc32c = 0.5f). Leaf, no globals.
   relocheck OK, cxx ok. */
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
typedef struct Slot_c740 {
  int m_00;
  int m_04;
  float m_pos;     /* +0x08 */
  float m_vel;     /* +0x0c */
  float m_10;
  float m_14;
} Slot_c740;
// MATCH: 0x0044c740 FUN_0044c740
void FUN_0044c740(Slot_c740 *a, Slot_c740 *b)
{
  Slot_c740 *lo;
  Slot_c740 *hi;
  float r;
  float d;
  float p;
  float v;

  if (a->m_pos > b->m_pos) {
    hi = a;
    lo = b;
  } else {
    hi = b;
    lo = a;
  }
  r = lo->m_10 + hi->m_14;
  d = hi->m_pos - lo->m_pos;
  if (d < 0.0f) {
    d = -d;
  }
  if (d < r) {
    p = hi->m_pos - hi->m_vel;
    hi->m_pos = p;
    if (p >= 1.0f - hi->m_10 || p <= hi->m_14) {
      hi->m_pos = p - (hi->m_vel + hi->m_vel);
      hi->m_vel = hi->m_vel * -0.5f;
    }
    p = lo->m_pos - lo->m_vel;
    lo->m_pos = p;
    if (p >= 1.0f - lo->m_10 || p <= lo->m_14) {
      lo->m_pos = p - (lo->m_vel + lo->m_vel);
      lo->m_vel = lo->m_vel * -0.5f;
    }
    v = lo->m_vel;
    lo->m_vel = lo->m_vel * -0.5f;
    lo->m_vel = hi->m_vel * 0.5f + lo->m_vel;
    hi->m_vel = v * 0.5f - hi->m_vel * 0.5f;
  }
}
