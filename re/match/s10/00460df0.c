// FLAGS: /O2 /GS /Ob1
/* MATCH-S10 0x00460df0 (h5): 100% (regs 100%), whole symbol byte-identical. By hand from 42.4% raw, same
   shape as 0x0045ff50/0x00461650: channel struct behind the 8-byte slot array 0x0068f640, one Vec3 local
   struct-copied into +0x58 and passed by address, float literals for the .rdata constants, k++ for the
   adjacent channels, the random x held in a temporary before z is computed. relocheck OK unchecked=0, cxx ok. */
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
typedef struct { float x, y, z; } Vec3_ch;
typedef struct {
  char m_pad0[0x30];
  int m_h;               /* +0x30 */
  unsigned char m_flags; /* +0x34 */
  char m_pad35[0x3c - 0x35];
  float m_freq;          /* +0x3c */
  char m_pad40[0x58 - 0x40];
  Vec3_ch m_pos;         /* +0x58 */
} Chan_ch;
typedef struct { Chan_ch *m_chan; int m_pad; } Slot0068f640;
MGLOBAL(int, DAT_0068f4c8, 0x0068f4c8);
MGLOBAL(int, DAT_008aa254, 0x008aa254);
MGLOBAL(int, DAT_0088e670, 0x0088e670);
MGLOBAL(float, DAT_006905ac, 0x006905ac);
MGLOBALA(Slot0068f640, DAT_0068f640, 1, 0x0068f640);
void FUN_004417e0(Vec3_ch *p0);
void FUN_0045e0f0(int p0, float p1);
float FUN_00472650(float lo, float hi);
int FUN_004b44f0(int p0, int p1);
void FUN_005a66d0(Chan_ch *p0, int p1);
void FUN_005a6d60(int p0, int p1, int p2, Vec3_ch *p3);
void FUN_005a6dc0(int p0, int p1, int p2, float p3);

// MATCH: 0x00460df0 FUN_00460df0
void FUN_00460df0(void)
{
  int k;
  Vec3_ch cam;
  Vec3_ch v;

  if ((DAT_0068f4c8 == 0) || (DAT_008aa254 <= 1)) {
    FUN_004417e0(&cam);
    DAT_006905ac = DAT_006905ac + 0.001f;
    if (DAT_006905ac > 3.14f) {
      DAT_006905ac = 0.0f;
    }
    k = *(int *)(DAT_0088e670 + 0x590);
    if ((DAT_0068f640[k].m_chan->m_flags & 2) == 0) {
      FUN_005a66d0(DAT_0068f640[k].m_chan, 1);
    }
    DAT_0068f640[k].m_chan->m_freq = 22050.0f;
    if (DAT_0068f640[k].m_chan->m_h != 0) {
      FUN_005a6dc0(DAT_0068f640[k].m_chan->m_h, 4, 1, 22050.0f);
    }
    FUN_0045e0f0(k, 0.2f);
    k = *(int *)(DAT_0088e670 + 0x470);
    if ((DAT_0068f640[k].m_chan->m_flags & 2) == 0) {
      FUN_005a66d0(DAT_0068f640[k].m_chan, 1);
    }
    DAT_0068f640[k].m_chan->m_freq = 22050.0f;
    if (DAT_0068f640[k].m_chan->m_h != 0) {
      FUN_005a6dc0(DAT_0068f640[k].m_chan->m_h, 4, 1, 22050.0f);
    }
    FUN_0045e0f0(k, 1.0f);
    v.x = 36.7f;
    v.y = 2.6f;
    v.z = -13.1f;
    DAT_0068f640[k].m_chan->m_pos = v;
    if (DAT_0068f640[k].m_chan->m_h != 0) {
      FUN_005a6d60(DAT_0068f640[k].m_chan->m_h, 0, 1, &v);
    }
    k++;
    if ((DAT_0068f640[k].m_chan->m_flags & 2) == 0) {
      FUN_005a66d0(DAT_0068f640[k].m_chan, 1);
    }
    DAT_0068f640[k].m_chan->m_freq = 22050.0f;
    if (DAT_0068f640[k].m_chan->m_h != 0) {
      FUN_005a6dc0(DAT_0068f640[k].m_chan->m_h, 4, 1, 22050.0f);
    }
    FUN_0045e0f0(k, 1.0f);
    v.x = 36.7f;
    v.y = 2.6f;
    v.z = -19.1f;
    DAT_0068f640[k].m_chan->m_pos = v;
    if (DAT_0068f640[k].m_chan->m_h != 0) {
      FUN_005a6d60(DAT_0068f640[k].m_chan->m_h, 0, 1, &v);
    }
    k++;
    if ((DAT_0068f640[k].m_chan->m_flags & 2) == 0) {
      FUN_005a66d0(DAT_0068f640[k].m_chan, 1);
    }
    DAT_0068f640[k].m_chan->m_freq = 22050.0f;
    if (DAT_0068f640[k].m_chan->m_h != 0) {
      FUN_005a6dc0(DAT_0068f640[k].m_chan->m_h, 4, 1, 22050.0f);
    }
    FUN_0045e0f0(k, 1.0f);
    v.x = 36.7f;
    v.y = 2.6f;
    v.z = -26.1f;
    DAT_0068f640[k].m_chan->m_pos = v;
    if (DAT_0068f640[k].m_chan->m_h != 0) {
      FUN_005a6d60(DAT_0068f640[k].m_chan->m_h, 0, 1, &v);
    }
    k = *(int *)(DAT_0088e670 + 0x500);
    if ((DAT_0068f640[k].m_chan->m_flags & 2) == 0) {
      FUN_005a66d0(DAT_0068f640[k].m_chan, 1);
    }
    DAT_0068f640[k].m_chan->m_freq = 22050.0f;
    if (DAT_0068f640[k].m_chan->m_h != 0) {
      FUN_005a6dc0(DAT_0068f640[k].m_chan->m_h, 4, 1, 22050.0f);
    }
    FUN_0045e0f0(k, 1.0f);
    v.x = 85.6f;
    v.y = 0.3f;
    v.z = 21.97f;
    DAT_0068f640[k].m_chan->m_pos = v;
    if (DAT_0068f640[k].m_chan->m_h != 0) {
      FUN_005a6d60(DAT_0068f640[k].m_chan->m_h, 0, 1, &v);
    }
    k = *(int *)(DAT_0088e670 + 0x6b0);
    if ((DAT_0068f640[k].m_chan->m_flags & 2) == 0) {
      FUN_005a66d0(DAT_0068f640[k].m_chan, 1);
    }
    DAT_0068f640[k].m_chan->m_freq = 22050.0f;
    if (DAT_0068f640[k].m_chan->m_h != 0) {
      FUN_005a6dc0(DAT_0068f640[k].m_chan->m_h, 4, 1, 22050.0f);
    }
    FUN_0045e0f0(k, 1.0f);
    v.x = 96.6f;
    v.y = 0.8f;
    v.z = -22.8f;
    DAT_0068f640[k].m_chan->m_pos = v;
    if (DAT_0068f640[k].m_chan->m_h != 0) {
      FUN_005a6d60(DAT_0068f640[k].m_chan->m_h, 0, 1, &v);
    }
    k = *(int *)(DAT_0088e670 + 0x740);
    if ((DAT_0068f640[k].m_chan->m_flags & 2) == 0) {
      FUN_005a66d0(DAT_0068f640[k].m_chan, 1);
    }
    DAT_0068f640[k].m_chan->m_freq = 22050.0f;
    if (DAT_0068f640[k].m_chan->m_h != 0) {
      FUN_005a6dc0(DAT_0068f640[k].m_chan->m_h, 4, 1, 22050.0f);
    }
    FUN_0045e0f0(k, 1.0f);
    v.x = -1.06f;
    v.y = 0.6f;
    v.z = -29.32f;
    DAT_0068f640[k].m_chan->m_pos = v;
    if (DAT_0068f640[k].m_chan->m_h != 0) {
      FUN_005a6d60(DAT_0068f640[k].m_chan->m_h, 0, 1, &v);
    }
    k = *(int *)(DAT_0088e670 + 0x620);
    if ((DAT_0068f640[k].m_chan->m_flags & 2) == 0) {
      FUN_005a66d0(DAT_0068f640[k].m_chan, 1);
    }
    DAT_0068f640[k].m_chan->m_freq = 22050.0f;
    if (DAT_0068f640[k].m_chan->m_h != 0) {
      FUN_005a6dc0(DAT_0068f640[k].m_chan->m_h, 4, 1, 22050.0f);
    }
    FUN_0045e0f0(k, 1.0f);
    v.x = 36.7f;
    v.y = 3.3f;
    v.z = 42.5f;
    DAT_0068f640[k].m_chan->m_pos = v;
    if (DAT_0068f640[k].m_chan->m_h != 0) {
      FUN_005a6d60(DAT_0068f640[k].m_chan->m_h, 0, 1, &v);
    }
    k++;
    if ((DAT_0068f640[k].m_chan->m_flags & 2) == 0) {
      FUN_005a66d0(DAT_0068f640[k].m_chan, 1);
    }
    DAT_0068f640[k].m_chan->m_freq = 23050.0f;
    if (DAT_0068f640[k].m_chan->m_h != 0) {
      FUN_005a6dc0(DAT_0068f640[k].m_chan->m_h, 4, 1, 23050.0f);
    }
    FUN_0045e0f0(k, 1.0f);
    v.x = 96.7f;
    v.y = 1.3f;
    v.z = 14.2f;
    DAT_0068f640[k].m_chan->m_pos = v;
    if (DAT_0068f640[k].m_chan->m_h != 0) {
      FUN_005a6d60(DAT_0068f640[k].m_chan->m_h, 0, 1, &v);
    }
    if (FUN_004b44f0(0, 100) == 0x19) {
      k = *(int *)(FUN_004b44f0(0, 1) * 0x90 + 2000 + DAT_0088e670);
      if ((DAT_0068f640[k].m_chan->m_flags & 2) == 0) {
        FUN_005a66d0(DAT_0068f640[k].m_chan, 1);
      }
      DAT_0068f640[k].m_chan->m_freq = 22050.0f;
      if (DAT_0068f640[k].m_chan->m_h != 0) {
        FUN_005a6dc0(DAT_0068f640[k].m_chan->m_h, 4, 1, 22050.0f);
      }
      FUN_0045e0f0(k, 1.0f);
      {
        float x = FUN_00472650(0.0f, 17.0f) + 49.0f;
        v.z = FUN_00472650(0.0f, 70.0f) - 12.0f;
        v.x = x;
        v.y = 0.0f;
      }
      DAT_0068f640[k].m_chan->m_pos = v;
      if (DAT_0068f640[k].m_chan->m_h != 0) {
        FUN_005a6d60(DAT_0068f640[k].m_chan->m_h, 0, 1, &v);
      }
    }
    k = *(int *)(DAT_0088e670 + 0x8f0);
    if ((DAT_0068f640[k].m_chan->m_flags & 2) == 0) {
      FUN_005a66d0(DAT_0068f640[k].m_chan, 1);
    }
    DAT_0068f640[k].m_chan->m_freq = 22050.0f;
    if (DAT_0068f640[k].m_chan->m_h != 0) {
      FUN_005a6dc0(DAT_0068f640[k].m_chan->m_h, 4, 1, 22050.0f);
    }
    FUN_0045e0f0(k, 1.0f);
    v.x = 63.6f;
    v.y = 0.5f;
    v.z = -28.0f;
    DAT_0068f640[k].m_chan->m_pos = v;
    if (DAT_0068f640[k].m_chan->m_h != 0) {
      FUN_005a6d60(DAT_0068f640[k].m_chan->m_h, 0, 1, &v);
    }
    k++;
    if ((DAT_0068f640[k].m_chan->m_flags & 2) == 0) {
      FUN_005a66d0(DAT_0068f640[k].m_chan, 1);
    }
    DAT_0068f640[k].m_chan->m_freq = 22050.0f;
    if (DAT_0068f640[k].m_chan->m_h != 0) {
      FUN_005a6dc0(DAT_0068f640[k].m_chan->m_h, 4, 1, 22050.0f);
    }
    FUN_0045e0f0(k, 1.0f);
    v.x = 77.0f;
    v.y = 0.5f;
    v.z = -28.0f;
    DAT_0068f640[k].m_chan->m_pos = v;
    if (DAT_0068f640[k].m_chan->m_h != 0) {
      FUN_005a6d60(DAT_0068f640[k].m_chan->m_h, 0, 1, &v);
    }
  }
}
