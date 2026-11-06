// FLAGS: /O2 /GS /Ob1
/* MATCH-S09 0x00456eb0: 100% by hand (h5), relocheck OK. Raw 33.8% (in_EAX). static with one pointer parameter (a
   64-byte matrix, position at +0x30) so cl passes it in EAX (pilot finding 1). The real caller 0x004577f0
   (mov eax, [esp+4]; jmp 0x00456eb0) is not in this file, so a stand-in caller of the same shape sits below it (not
   the real TU). 64 records of 0x68 bytes at 0x00688810 with a used-bit map at 0x0068a300. The `jne` after the imul
   is the r == 0 test of a pointer set only on the break path. The bit is set for i even when no slot is free
   (i == 64, byte 0x0068a308), as the original does. */
/* ---- MATCH-S09 preamble (same text in every re/match/s09 source; not part of any function body) ----
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
typedef struct V3_00456eb0 { float x, y, z; } V3_00456eb0;
typedef struct Mat00456eb0 {
  V3_00456eb0 right; int flags;
  V3_00456eb0 up; int pad1;
  V3_00456eb0 at; int pad2;
  V3_00456eb0 pos; int pad3;
} Mat00456eb0;
typedef struct Rec00456eb0 {
  float m_0;
  int m_4;
  V3_00456eb0 m_8;
  float m_14;
  Mat00456eb0 m_18;
  int m_58;
  float m_5c;
  int m_60;
  int m_64;
} Rec00456eb0;
MGLOBALA(Rec00456eb0, DAT_00688810, 64, 0x00688810);
MGLOBALA(unsigned char, DAT_0068a300, 8, 0x0068a300);

// MATCH: 0x00456eb0 FUN_00456eb0
static void FUN_00456eb0(Mat00456eb0 *m)
{
  Rec00456eb0 *best;
  Rec00456eb0 *r;
  int i;

  best = DAT_00688810;
  r = 0;
  for (i = 0; i < 64; i++) {
    if (DAT_00688810[i].m_0 > best->m_0) {
      best = &DAT_00688810[i];
    }
    if ((DAT_0068a300[i >> 3] & (1 << (i & 7))) == 0) {
      r = &DAT_00688810[i];
      break;
    }
  }
  if (r == 0) {
    r = best;
  }
  DAT_0068a300[i >> 3] |= (unsigned char)(1 << (i & 7));
  r->m_60 = 0;
  r->m_0 = 0;
  r->m_5c = -1.0f;
  r->m_58 = 0;
  r->m_14 = 0.25f;
  r->m_8 = m->pos;
  r->m_18 = *m;
}

/* stand-in caller (not the real TU): the same shape as 0x004577f0 (mov eax, [esp+4]; jmp 0x00456eb0) */
void FUN_00456eb0_caller(Mat00456eb0 *m)
{
  FUN_00456eb0(m);
}
