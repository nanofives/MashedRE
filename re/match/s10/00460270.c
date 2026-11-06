// FLAGS: /O2 /GS /Ob1
/* MATCH-S10 0x00460270 (h5): 100% (regs 100%), by hand from 60% raw. Moves: 12-byte vec3 local read at [1],
   the .rdata operands 2.5f / 0.125f / 0.0f / 1.0f written as float literals (the global forms loaded first and
   compared reversed), clamp written as if (<0) else if (>1). relocheck OK unchecked=0, cxx ok. */
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
typedef struct { int m_chan; int m_pad; } Slot0068f640;
MGLOBAL(int, DAT_0068f4c8, 0x0068f4c8);
MGLOBAL(int, DAT_008aa254, 0x008aa254);
MGLOBAL(int, DAT_0088e670, 0x0088e670);
MGLOBAL(float, DAT_006905a8, 0x006905a8);
MGLOBALA(Slot0068f640, DAT_0068f640, 1, 0x0068f640);
void FUN_004417e0(float *p0);
void FUN_0045e0f0(int p0, float p1);
void FUN_005a66d0(int p0, int p1);
void FUN_005a6dc0(int p0, int p1, int p2, float p3);

// MATCH: 0x00460270 FUN_00460270
void FUN_00460270(void)
{
  int i;
  float v[3];

  if ((DAT_0068f4c8 == 0) || (DAT_008aa254 <= 1)) {
    FUN_004417e0(v);
    i = *(int *)(DAT_0088e670 + 0x470);
    if ((*(byte *)(DAT_0068f640[i].m_chan + 0x34) & 2) == 0) {
      FUN_005a66d0(DAT_0068f640[i].m_chan, 1);
    }
    *(float *)(DAT_0068f640[i].m_chan + 0x3c) = 22050.0f;
    if (*(int *)(DAT_0068f640[i].m_chan + 0x30) != 0) {
      FUN_005a6dc0(*(int *)(DAT_0068f640[i].m_chan + 0x30), 4, 1, 22050.0f);
    }
    DAT_006905a8 = (v[1] + 2.5f) * 0.125f;
    if (DAT_006905a8 < 0.0f) {
      DAT_006905a8 = 0.0f;
    }
    else if (DAT_006905a8 > 1.0f) {
      DAT_006905a8 = 1.0f;
    }
    FUN_0045e0f0(i, DAT_006905a8);
  }
}
