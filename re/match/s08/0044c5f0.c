// FLAGS: /O2 /GS /Ob1
/* MATCH-S08 0x0044c5f0 (328 B): 100% (regs 100%), by hand. Raw converter 18.6%.
   Slot is the 0x58-byte record of 0x0044c4f0 / 0x0044c490 (position +0x08, velocity +0x0c, limits +0x10/+0x14).
   What moved it: every .rdata float as a literal (0x005cc320 = 1.0f, 0x005cd50c = -0.5f, 0x005d757c = 0.0f,
   0x005cc558 = 0.001f, 0x005cc9b4 = 0.99f), the bounce velocity through a local (nv), and FUN_00545d30
   returning float with the result discarded (fstp st(0)). Field m_4c length 3 is [UNCERTAIN]: only its address
   is used. relocheck OK, cxx ok. */
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
typedef struct Slot_c5f0 {
  int m_00;
  int m_04;
  float m_pos;     /* +0x08 */
  float m_vel;     /* +0x0c */
  float m_10;
  float m_14;
  int m_18[9];
  int m_3c;
  int m_40[3];
  float m_4c[3];
} Slot_c5f0;
void FUN_0044c490(int param_1);
void FUN_0047d180(int p0, float *p1);
int FUN_004c1480(int p0, float *p1, int p2);
void FUN_004c51a0(float *p0, float *p1, int p2);
float FUN_00545d30(int p0, int p1, float p2, float *p3, float *p4);
// MATCH: 0x0044c5f0 FUN_0044c5f0
void FUN_0044c5f0(Slot_c5f0 *s, float dt)
{
  float pos;
  float lim;
  float nv;
  float up[3];
  float m[16];

  up[0] = 0.0f;
  up[1] = 1.0f;
  up[2] = 0.0f;
  FUN_00545d30(s->m_00, 10, s->m_pos, up, m);
  FUN_004c1480(*(int *)(s->m_04 + 4), m, 0);
  FUN_004c51a0(m, s->m_4c, 2);
  FUN_0047d180(s->m_3c, m);
  pos = dt * s->m_vel + s->m_pos;
  s->m_pos = pos;
  lim = 1.0f - s->m_10;
  if (pos >= lim || pos <= s->m_14) {
    s->m_pos = pos - (s->m_vel + s->m_vel);
    nv = s->m_vel * -0.5f;
    s->m_vel = nv;
    if (s->m_pos >= lim) {
      if (nv > 0.0f) {
        s->m_vel = -nv;
      }
      s->m_pos = lim - 0.001f;
    }
    if (s->m_pos <= s->m_14) {
      if (s->m_vel < 0.0f) {
        s->m_vel = -s->m_vel;
      }
      s->m_pos = s->m_14 + 0.001f;
    }
  }
  s->m_vel = s->m_vel * 0.99f;
  FUN_0044c490((int)s);
}
