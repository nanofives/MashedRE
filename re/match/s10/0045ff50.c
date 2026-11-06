// FLAGS: /O2 /GS /Ob1
/* MATCH-S10 0x0045ff50 (h5): 100% (regs 100%), by hand from 56.5% raw. Moves: channel struct (+0x30 handle,
   +0x34 flags, +0x3c freq, +0x58 Vec3 pos) behind the 8-byte slot array 0x0068f640, a Vec3 local copied by
   struct assignment into +0x58 and passed by address to 0x005a6d60, .rdata operands as float literals (22050,
   50, 74), 0x00472650 as float(float,float) called twice for the second freq (re-evaluated, not reused).
   relocheck OK unchecked=0, cxx ok. */
/* ---- MATCH-S10 preamble (same text in every re/match/s10 source; not part of any function body) ----
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
typedef struct { float x, y, z; } Vec3_45ff50;
typedef struct {
  char m_pad0[0x30];
  int m_h;              /* +0x30 */
  unsigned char m_flags; /* +0x34 */
  char m_pad35[0x3c - 0x35];
  float m_freq;         /* +0x3c */
  char m_pad40[0x58 - 0x40];
  Vec3_45ff50 m_pos;    /* +0x58 */
} Chan_45ff50;
typedef struct { Chan_45ff50 *m_chan; int m_pad; } Slot0068f640;
MGLOBAL(int, DAT_0068f4c8, 0x0068f4c8);
MGLOBAL(int, DAT_008aa254, 0x008aa254);
MGLOBAL(int, DAT_0088e670, 0x0088e670);
MGLOBALA(Slot0068f640, DAT_0068f640, 1, 0x0068f640);
void FUN_0045e0f0(int p0, float p1);
float FUN_00472650(float lo, float hi);
int FUN_004b44f0(int p0, int p1);
void FUN_005a66d0(Chan_45ff50 *p0, int p1);
void FUN_005a6d60(int p0, int p1, int p2, Vec3_45ff50 *p3);
void FUN_005a6dc0(int p0, int p1, int p2, float p3);

// MATCH: 0x0045ff50 FUN_0045ff50
void FUN_0045ff50(void)
{
  int i;
  Vec3_45ff50 v;

  if ((DAT_0068f4c8 == 0) || (DAT_008aa254 <= 1)) {
    i = *(int *)(DAT_0088e670 + 0x470);
    if ((DAT_0068f640[i].m_chan->m_flags & 2) == 0) {
      FUN_005a66d0(DAT_0068f640[i].m_chan, 1);
    }
    DAT_0068f640[i].m_chan->m_freq = 22050.0f;
    if (DAT_0068f640[i].m_chan->m_h != 0) {
      FUN_005a6dc0(DAT_0068f640[i].m_chan->m_h, 4, 1, 22050.0f);
    }
    FUN_0045e0f0(i, 0.5f);
    if (FUN_004b44f0(0, 0x1e) == 0x19) {
      i = *(int *)(FUN_004b44f0(0, 2) * 0x90 + 0x500 + DAT_0088e670);
      if ((DAT_0068f640[i].m_chan->m_flags & 2) == 0) {
        FUN_005a66d0(DAT_0068f640[i].m_chan, 1);
      }
      DAT_0068f640[i].m_chan->m_freq = FUN_00472650(0.0f, 10000.0f) + 22050.0f;
      if (DAT_0068f640[i].m_chan->m_h != 0) {
        FUN_005a6dc0(DAT_0068f640[i].m_chan->m_h, 4, 1, FUN_00472650(0.0f, 10000.0f) + 22050.0f);
      }
      FUN_0045e0f0(i, 1.0f);
      {
        float x = FUN_00472650(0.0f, 100.0f) - 50.0f;
        v.z = FUN_00472650(0.0f, 74.0f) - 74.0f;
        v.x = x;
        v.y = 0.0f;
      }
      DAT_0068f640[i].m_chan->m_pos = v;
      if (DAT_0068f640[i].m_chan->m_h != 0) {
        FUN_005a6d60(DAT_0068f640[i].m_chan->m_h, 0, 1, &v);
      }
    }
    i = *(int *)(DAT_0088e670 + 0x6b0);
    if ((DAT_0068f640[i].m_chan->m_flags & 2) == 0) {
      FUN_005a66d0(DAT_0068f640[i].m_chan, 1);
    }
    DAT_0068f640[i].m_chan->m_freq = 22050.0f;
    if (DAT_0068f640[i].m_chan->m_h != 0) {
      FUN_005a6dc0(DAT_0068f640[i].m_chan->m_h, 4, 1, 22050.0f);
    }
    FUN_0045e0f0(i, 1.0f);
    v.x = 1.32f;
    v.y = -7.7f;
    v.z = -30.7f;
    DAT_0068f640[i].m_chan->m_pos = v;
    if (DAT_0068f640[i].m_chan->m_h != 0) {
      FUN_005a6d60(DAT_0068f640[i].m_chan->m_h, 0, 1, &v);
    }
    i++;
    if ((DAT_0068f640[i].m_chan->m_flags & 2) == 0) {
      FUN_005a66d0(DAT_0068f640[i].m_chan, 1);
    }
    DAT_0068f640[i].m_chan->m_freq = 22050.0f;
    if (DAT_0068f640[i].m_chan->m_h != 0) {
      FUN_005a6dc0(DAT_0068f640[i].m_chan->m_h, 4, 1, 22050.0f);
    }
    FUN_0045e0f0(i, 1.0f);
    v.x = 11.0f;
    v.y = -5.7f;
    v.z = -61.7f;
    DAT_0068f640[i].m_chan->m_pos = v;
    if (DAT_0068f640[i].m_chan->m_h != 0) {
      FUN_005a6d60(DAT_0068f640[i].m_chan->m_h, 0, 1, &v);
    }
  }
}
