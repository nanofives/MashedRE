// FLAGS: /O2 /GS /Ob1
/* MATCH-S10 0x00461650 (h5): 100% byte-identical whole symbol (match.py --bytes: 0 differing unmasked bytes);
   instruction score 98.8% only because Ghidra's size (2095) stops before the epilogue. By hand from 47.7% raw.
   Moves: channel struct + Vec3 (as 0x0045ff50), Vec3 global 0x0068fcc0 copied by struct assignment, float
   literals for the .rdata constants, `k = G[0x980]; k += FUN_004b44f0(0, 1);` (load before the call), and
   DAT_0068f4c4 as a file-static global (pilot finding 2): with a plain extern the `= 0` store is scheduled
   before the flag loads. The file-static is under #ifndef __cplusplus, MGLOBAL under #else.
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
typedef struct { float x, y, z; } Vec3_461650;
typedef struct {
  char m_pad0[0x30];
  int m_h;               /* +0x30 */
  unsigned char m_flags; /* +0x34 */
  char m_pad35[0x3c - 0x35];
  float m_freq;          /* +0x3c */
  char m_pad40[0x58 - 0x40];
  Vec3_461650 m_pos;     /* +0x58 */
} Chan_461650;
typedef struct { Chan_461650 *m_chan; int m_pad; } Slot0068f640;
MGLOBAL(int, DAT_0068f4c8, 0x0068f4c8);
MGLOBAL(int, DAT_008aa254, 0x008aa254);
MGLOBAL(int, DAT_0088e670, 0x0088e670);
MGLOBAL(Vec3_461650, DAT_0068fcc0, 0x0068fcc0);
MGLOBAL(float, DAT_0069048c, 0x0069048c);
MGLOBAL(float, DAT_0068f630, 0x0068f630);
#ifndef __cplusplus
static int DAT_0068f4c4;
#else
MGLOBAL(int, DAT_0068f4c4, 0x0068f4c4);
#endif
MGLOBALA(Slot0068f640, DAT_0068f640, 1, 0x0068f640);
void FUN_004417e0(Vec3_461650 *p0);
int FUN_0044dfe0(void);
void FUN_0044dff0(int p0, Vec3_461650 *p1);
int FUN_0044e020(int p0);
float FUN_0044e050(int p0);
float FUN_0044e070(int p0);
void FUN_0045e0f0(int p0, float p1);
float FUN_00472650(float lo, float hi);
int FUN_004b44f0(int p0, int p1);
void FUN_005a66d0(Chan_461650 *p0, int p1);
void FUN_005a6d60(int p0, int p1, int p2, Vec3_461650 *p3);
void FUN_005a6dc0(int p0, int p1, int p2, float p3);

// MATCH: 0x00461650 FUN_00461650
void FUN_00461650(void)
{
  int n;
  int count;
  int i;
  int k;
  float d;
  float f;
  Vec3_461650 cam;
  Vec3_461650 v;

  if ((DAT_0068f4c8 == 0) || (DAT_008aa254 <= 1)) {
    n = 0;
    count = FUN_0044dfe0();
    FUN_004417e0(&cam);
    for (i = 0; i < 8; i++) {
      FUN_0045e0f0(*(int *)(DAT_0088e670 + 0x470) + i, 0.0f);
      FUN_0045e0f0(*(int *)(DAT_0088e670 + 0x500) + i, 0.0f);
    }
    for (i = 0; i < count; i++) {
      FUN_0044dff0(i, &DAT_0068fcc0);
      d = DAT_0068fcc0.z - cam.z;
      if (d > 0.0f) {
        if (!(d < 10.0f)) continue;
      }
      else if (!(d > -10.0f)) continue;
      if (n < 8) {
        DAT_0069048c = FUN_0044e050(i);
        DAT_0068f630 = FUN_0044e070(i);
        k = *(int *)(DAT_0088e670 + 0x470) + n;
        if ((DAT_0068f640[k].m_chan->m_flags & 2) == 0) {
          FUN_005a66d0(DAT_0068f640[k].m_chan, 1);
        }
        f = DAT_0068f630 * DAT_0069048c * 15000.0f + 22050.0f;
        DAT_0068f640[k].m_chan->m_freq = f;
        if (DAT_0068f640[k].m_chan->m_h != 0) {
          FUN_005a6dc0(DAT_0068f640[k].m_chan->m_h, 4, 1, f);
        }
        DAT_0068f640[k].m_chan->m_pos = DAT_0068fcc0;
        if (DAT_0068f640[k].m_chan->m_h != 0) {
          FUN_005a6d60(DAT_0068f640[k].m_chan->m_h, 0, 1, &DAT_0068fcc0);
        }
        FUN_0045e0f0(k, 1.0f);
        if (FUN_0044e020(i) != 0) {
          k = *(int *)(DAT_0088e670 + 0x620);
          if ((DAT_0068f640[k].m_chan->m_flags & 2) == 0) {
            FUN_005a66d0(DAT_0068f640[k].m_chan, 1);
          }
          DAT_0068f640[k].m_chan->m_freq = 22050.0f;
          if (DAT_0068f640[k].m_chan->m_h != 0) {
            FUN_005a6dc0(DAT_0068f640[k].m_chan->m_h, 4, 1, 22050.0f);
          }
          DAT_0068f640[k].m_chan->m_pos = DAT_0068fcc0;
          if (DAT_0068f640[k].m_chan->m_h != 0) {
            FUN_005a6d60(DAT_0068f640[k].m_chan->m_h, 0, 1, &DAT_0068fcc0);
          }
          FUN_0045e0f0(k, 1.0f);
          if ((DAT_0068f4c4 == 0) && (FUN_004b44f0(0, 1) == 1)) {
            k = *(int *)(FUN_004b44f0(0, 2) * 0x90 + 0x6b0 + DAT_0088e670);
            if ((DAT_0068f640[k].m_chan->m_flags & 2) == 0) {
              FUN_005a66d0(DAT_0068f640[k].m_chan, 1);
            }
            DAT_0068f640[k].m_chan->m_freq = FUN_00472650(0.0f, 1000.0f) + 22050.0f;
            if (DAT_0068f640[k].m_chan->m_h != 0) {
              FUN_005a6dc0(DAT_0068f640[k].m_chan->m_h, 4, 1, FUN_00472650(0.0f, 1000.0f) + 22050.0f);
            }
            DAT_0068f640[k].m_chan->m_pos = DAT_0068fcc0;
            if (DAT_0068f640[k].m_chan->m_h != 0) {
              FUN_005a6d60(DAT_0068f640[k].m_chan->m_h, 0, 1, &DAT_0068fcc0);
            }
            FUN_0045e0f0(k, 1.0f);
          }
          k = *(int *)(DAT_0088e670 + 0x500) + n;
          if ((DAT_0068f640[k].m_chan->m_flags & 2) == 0) {
            FUN_005a66d0(DAT_0068f640[k].m_chan, 1);
          }
          DAT_0068f640[k].m_chan->m_freq = FUN_00472650(0.0f, 3000.0f) + 15050.0f;
          if (DAT_0068f640[k].m_chan->m_h != 0) {
            FUN_005a6dc0(DAT_0068f640[k].m_chan->m_h, 4, 1, FUN_00472650(0.0f, 3000.0f) + 15050.0f);
          }
          DAT_0068f640[k].m_chan->m_pos = DAT_0068fcc0;
          if (DAT_0068f640[k].m_chan->m_h != 0) {
            FUN_005a6d60(DAT_0068f640[k].m_chan->m_h, 0, 1, &DAT_0068fcc0);
          }
          FUN_0045e0f0(k, 1.0f);
        }
        n++;
      }
    }
    DAT_0068f4c4 = 0;
    if ((DAT_0068f640[*(int *)(DAT_0088e670 + 0x620)].m_chan->m_flags & 2) != 0) {
      DAT_0068f4c4++;
    }
    if ((DAT_0068f640[*(int *)(DAT_0088e670 + 0x6b0)].m_chan->m_flags & 2) != 0) {
      DAT_0068f4c4++;
    }
    if ((DAT_0068f640[*(int *)(DAT_0088e670 + 0x740)].m_chan->m_flags & 2) != 0) {
      DAT_0068f4c4++;
    }
    k = *(int *)(DAT_0088e670 + 0x860);
    if ((DAT_0068f640[k].m_chan->m_flags & 2) == 0) {
      FUN_005a66d0(DAT_0068f640[k].m_chan, 1);
    }
    DAT_0068f640[k].m_chan->m_freq = 22050.0f;
    if (DAT_0068f640[k].m_chan->m_h != 0) {
      FUN_005a6dc0(DAT_0068f640[k].m_chan->m_h, 4, 1, 22050.0f);
    }
    FUN_0045e0f0(k, 1.0f);
    v.x = 0.7f;
    v.y = 3.8f;
    v.z = -112.1f;
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
    v.x = -0.15f;
    v.y = 1.7f;
    v.z = 137.1f;
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
    v.x = 0.7f;
    v.y = 3.8f;
    v.z = -112.1f;
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
    v.x = -0.15f;
    v.y = 1.7f;
    v.z = 137.1f;
    DAT_0068f640[k].m_chan->m_pos = v;
    if (DAT_0068f640[k].m_chan->m_h != 0) {
      FUN_005a6d60(DAT_0068f640[k].m_chan->m_h, 0, 1, &v);
    }
    if (FUN_004b44f0(0, 0x14) == 10) {
      k = *(int *)(DAT_0088e670 + 0x980);
      k += FUN_004b44f0(0, 1);
      if ((DAT_0068f640[k].m_chan->m_flags & 2) == 0) {
        FUN_005a66d0(DAT_0068f640[k].m_chan, 1);
      }
      DAT_0068f640[k].m_chan->m_freq = 22050.0f;
      if (DAT_0068f640[k].m_chan->m_h != 0) {
        FUN_005a6dc0(DAT_0068f640[k].m_chan->m_h, 4, 1, 22050.0f);
      }
      FUN_0045e0f0(k, 1.0f);
      v.x = 0.7f;
      v.y = 3.8f;
      v.z = (float)FUN_004b44f0(0, 1) * 200.0f - 112.1f;
      DAT_0068f640[k].m_chan->m_pos = v;
      if (DAT_0068f640[k].m_chan->m_h != 0) {
        FUN_005a6d60(DAT_0068f640[k].m_chan->m_h, 0, 1, &v);
      }
    }
  }
}
