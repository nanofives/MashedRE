// FLAGS: /O2 /GS /Ob1
/* MATCH-S10 0x00460350 (h5): 100% (regs 100%), whole symbol byte-identical. By hand from 35.6% raw, same
   shape as 0x00460df0 (channel struct, Vec3 local struct copy, float literals, k++, FUN_004495d0's Vec3*
   copied into two channels). The two fade-volume expressions need the 0.0333 / -1.0 factors and the final
   addend 0.2 as globals (MGLOBAL 0x005cc324 / 0x005cc33c / 0x005cc9c0) and the 0.2 factor as a literal:
   all-literal forms are constant-folded into one factor, all-global forms get factored to (x + 1) * 0.2.
   The -1.0 global is written before the 0.0333 one (the compiler swaps the order of the two loads).
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

MGLOBALA(Slot0068f640, DAT_0068f640, 1, 0x0068f640);
float FUN_00408ad0(int p0);
MGLOBAL(float, DAT_005cc324, 0x005cc324);
MGLOBAL(float, DAT_005cc33c, 0x005cc33c);
MGLOBAL(float, DAT_005cc9c0, 0x005cc9c0);
void FUN_004417e0(Vec3_ch *p0);
Vec3_ch *FUN_004495d0(void);
void FUN_0045e0f0(int p0, float p1);
float FUN_00472650(float lo, float hi);
int FUN_004b44f0(int p0, int p1);
void FUN_005a66d0(Chan_ch *p0, int p1);
void FUN_005a6d60(int p0, int p1, int p2, Vec3_ch *p3);
void FUN_005a6dc0(int p0, int p1, int p2, float p3);

// MATCH: 0x00460350 FUN_00460350
void FUN_00460350(void)
{
  int k;
  float t;
  Vec3_ch *p;
  Vec3_ch cam;
  Vec3_ch v;

  if ((DAT_0068f4c8 == 0) || (DAT_008aa254 <= 1)) {
    t = FUN_00408ad0(0);
    FUN_004417e0(&cam);
    k = *(int *)(DAT_0088e670 + 0x470);
    if ((DAT_0068f640[k].m_chan->m_flags & 2) == 0) {
      FUN_005a66d0(DAT_0068f640[k].m_chan, 1);
    }
    DAT_0068f640[k].m_chan->m_freq = 22050.0f;
    if (DAT_0068f640[k].m_chan->m_h != 0) {
      FUN_005a6dc0(DAT_0068f640[k].m_chan->m_h, 4, 1, 22050.0f);
    }
    FUN_0045e0f0(k, 0.2f);
    if (t < 30.0f) {
      FUN_0045e0f0(k, (t - 30.0f) * DAT_005cc33c * DAT_005cc324 * 0.2f + DAT_005cc9c0);
    }
    if (t > 70.0f) {
      FUN_0045e0f0(k, (t - 70.0f) * DAT_005cc324 * 0.2f + DAT_005cc9c0);
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
    v.x = 67.0f;
    v.y = -5.0f;
    v.z = 126.0f;
    DAT_0068f640[k].m_chan->m_pos = v;
    if (DAT_0068f640[k].m_chan->m_h != 0) {
      FUN_005a6d60(DAT_0068f640[k].m_chan->m_h, 0, 1, &v);
    }
    k = *(int *)(DAT_0088e670 + 0x590);
    if ((DAT_0068f640[k].m_chan->m_flags & 2) == 0) {
      FUN_005a66d0(DAT_0068f640[k].m_chan, 1);
    }
    DAT_0068f640[k].m_chan->m_freq = 22050.0f;
    if (DAT_0068f640[k].m_chan->m_h != 0) {
      FUN_005a6dc0(DAT_0068f640[k].m_chan->m_h, 4, 1, 22050.0f);
    }
    FUN_0045e0f0(k, 1.0f);
    v.x = 61.0f;
    v.y = -5.0f;
    v.z = 115.9f;
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
    v.x = 47.5f;
    v.y = 2.0f;
    v.z = 67.3f;
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
    v.x = 43.8f;
    v.y = 2.0f;
    v.z = 63.0f;
    DAT_0068f640[k].m_chan->m_pos = v;
    if (DAT_0068f640[k].m_chan->m_h != 0) {
      FUN_005a6d60(DAT_0068f640[k].m_chan->m_h, 0, 1, &v);
    }
    FUN_0045e0f0(k, 1.0f);
    k = *(int *)(DAT_0088e670 + 0x6b0);
    if ((DAT_0068f640[k].m_chan->m_flags & 2) == 0) {
      FUN_005a66d0(DAT_0068f640[k].m_chan, 1);
    }
    DAT_0068f640[k].m_chan->m_freq = 27050.0f;
    if (DAT_0068f640[k].m_chan->m_h != 0) {
      FUN_005a6dc0(DAT_0068f640[k].m_chan->m_h, 4, 1, 27050.0f);
    }
    FUN_0045e0f0(k, 0.9f);
    v.x = 37.2f;
    v.y = -3.2f;
    v.z = 115.0f;
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
    v.x = 43.6f;
    v.y = -3.35f;
    v.z = 92.7f;
    DAT_0068f640[k].m_chan->m_pos = v;
    if (DAT_0068f640[k].m_chan->m_h != 0) {
      FUN_005a6d60(DAT_0068f640[k].m_chan->m_h, 0, 1, &v);
    }
    k++;
    if ((DAT_0068f640[k].m_chan->m_flags & 2) == 0) {
      FUN_005a66d0(DAT_0068f640[k].m_chan, 1);
    }
    DAT_0068f640[k].m_chan->m_freq = 25050.0f;
    if (DAT_0068f640[k].m_chan->m_h != 0) {
      FUN_005a6dc0(DAT_0068f640[k].m_chan->m_h, 4, 1, 25050.0f);
    }
    FUN_0045e0f0(k, 1.0f);
    v.x = 44.0f;
    v.y = -4.22f;
    v.z = 105.4f;
    DAT_0068f640[k].m_chan->m_pos = v;
    if (DAT_0068f640[k].m_chan->m_h != 0) {
      FUN_005a6d60(DAT_0068f640[k].m_chan->m_h, 0, 1, &v);
    }
    k = *(int *)(DAT_0088e670 + 0xa10);
    if ((DAT_0068f640[k].m_chan->m_flags & 2) == 0) {
      FUN_005a66d0(DAT_0068f640[k].m_chan, 1);
    }
    DAT_0068f640[k].m_chan->m_freq = 22050.0f;
    if (DAT_0068f640[k].m_chan->m_h != 0) {
      FUN_005a6dc0(DAT_0068f640[k].m_chan->m_h, 4, 1, 22050.0f);
    }
    FUN_0045e0f0(k, 1.0f);
    v.x = 1.7f;
    v.y = -0.6f;
    v.z = 119.6f;
    DAT_0068f640[k].m_chan->m_pos = v;
    if (DAT_0068f640[k].m_chan->m_h != 0) {
      FUN_005a6d60(DAT_0068f640[k].m_chan->m_h, 0, 1, &v);
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
    p = FUN_004495d0();
    DAT_0068f640[k].m_chan->m_pos = *p;
    if (DAT_0068f640[k].m_chan->m_h != 0) {
      FUN_005a6d60(DAT_0068f640[k].m_chan->m_h, 0, 1, p);
    }
    k = *(int *)(DAT_0088e670 + 0x6b0) + 1;
    if ((DAT_0068f640[k].m_chan->m_flags & 2) == 0) {
      FUN_005a66d0(DAT_0068f640[k].m_chan, 1);
    }
    DAT_0068f640[k].m_chan->m_freq = 22050.0f;
    if (DAT_0068f640[k].m_chan->m_h != 0) {
      FUN_005a6dc0(DAT_0068f640[k].m_chan->m_h, 4, 1, 22050.0f);
    }
    FUN_0045e0f0(k, 1.0f);
    DAT_0068f640[k].m_chan->m_pos = *p;
    if (DAT_0068f640[k].m_chan->m_h != 0) {
      FUN_005a6d60(DAT_0068f640[k].m_chan->m_h, 0, 1, p);
    }
    k = *(int *)(DAT_0088e670 + 0x980);
    if ((DAT_0068f640[k].m_chan->m_flags & 2) == 0) {
      FUN_005a66d0(DAT_0068f640[k].m_chan, 1);
    }
    DAT_0068f640[k].m_chan->m_freq = 22050.0f;
    if (DAT_0068f640[k].m_chan->m_h != 0) {
      FUN_005a6dc0(DAT_0068f640[k].m_chan->m_h, 4, 1, 22050.0f);
    }
    FUN_0045e0f0(k, 1.0f);
    v.x = 68.2f;
    v.y = -4.2f;
    v.z = 106.8f;
    DAT_0068f640[k].m_chan->m_pos = v;
    if (DAT_0068f640[k].m_chan->m_h != 0) {
      FUN_005a6d60(DAT_0068f640[k].m_chan->m_h, 0, 1, &v);
    }
    k = *(int *)(DAT_0088e670 + 0xa10);
    k++;
    if ((DAT_0068f640[k].m_chan->m_flags & 2) == 0) {
      FUN_005a66d0(DAT_0068f640[k].m_chan, 1);
    }
    DAT_0068f640[k].m_chan->m_freq = 22050.0f;
    if (DAT_0068f640[k].m_chan->m_h != 0) {
      FUN_005a6dc0(DAT_0068f640[k].m_chan->m_h, 4, 1, 22050.0f);
    }
    FUN_0045e0f0(k, 1.0f);
    v.x = 35.7f;
    v.y = 0.4f;
    v.z = 60.0f;
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
    v.x = 68.7f;
    v.y = 0.68f;
    v.z = 54.6f;
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
        float x = FUN_00472650(0.0f, 20.0f) - 10.0f;
        v.z = FUN_00472650(0.0f, 10.0f) + 34.0f;
        v.x = x;
        v.y = 0.0f;
      }
      DAT_0068f640[k].m_chan->m_pos = v;
      if (DAT_0068f640[k].m_chan->m_h != 0) {
        FUN_005a6d60(DAT_0068f640[k].m_chan->m_h, 0, 1, &v);
      }
    }
  }
}
