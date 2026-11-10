// FLAGS: /O2 /GS /Ob1
/* MATCH-S10 0x00464a50 (h8): 98.3% (regs 98.3%), by hand (raw 49.4%). 180/178: the 2 extra are the add esp /
   ret past Ghidra's size. Found: memset intrinsic for dist[10] (rep stosd x10), bubble sort with the swap
   written t = dist[i], tp = src[i + 1], tp2 = src[i] (other orders flip which element is the int copy),
   d.y / d.z as assign-then-subtract (d.y = p->y; d.y = me.y - d.y gives fld [p+4] / fsubr, as in the
   original; me.x - p->x stays plain), third loop as for + break on i >= n. Remaining: the original stores the
   0x004c3ac0 result (fstp dist[i]) before popping the two cdecl args, ours pops first. Tried: float/double/
   long double return, temp variable, do-while first loop, statement orders, permuter 287 compiles. Class (b). 
   MATCH-RACEPATH-X87A 2026-11-09: CLOSED, 587 B BYTE-IDENTICAL (insn below 100% only because the Ghidra body is
   584 B, the symbol 587 B). The z difference goes through a named double temporary (`double dz = me.z - d.z;
   d.z = (float)dz;`), the form that closed 0x00412cf0. The value is a float difference widened and narrowed
   again: unchanged. */
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
typedef struct { float x, y, z; } MatchV3_464a50;
typedef struct { char *p; float f; } MatchRec0068f640;
MGLOBAL(int, DAT_00604d88, 0x00604d88);
MGLOBAL(int, DAT_00605b68, 0x00605b68);
MGLOBALA(MatchRec0068f640, DAT_0068f640, 1, 0x0068f640);
void *memset(void *d, int c, unsigned int n);
#pragma intrinsic(memset)
int FUN_00405890(void);
int FUN_004075a0(void);
MatchV3_464a50 *FUN_00407600(int a);
MatchV3_464a50 *FUN_00425ee0(int i);
int FUN_00425ef0(void);
void FUN_004417e0(MatchV3_464a50 *out);
void FUN_0045e0f0(int p0, float p1);
float FUN_004c3ac0(MatchV3_464a50 *v);
void FUN_005a66d0(char *p, int a);
void FUN_005a6d60(int h, int a, int b, MatchV3_464a50 *v);
void FUN_005a6dc0(int h, int a, int b, float f);
// MATCH: 0x00464a50 FUN_00464a50
void FUN_00464a50(void)
{
  int other;
  int n;
  MatchV3_464a50 me;
  MatchV3_464a50 d;
  float dist[10];
  MatchV3_464a50 *src[10];
  MatchV3_464a50 *p;
  int i;
  int ch;
  int swapped;
  float t;
  MatchV3_464a50 *tp;
  MatchV3_464a50 *tp2;

  memset(dist, 0, sizeof(dist));
  n = FUN_00425ef0();
  other = FUN_004075a0();
  if (FUN_00405890() != 0) {
    other = 0;
  }
  if (other + n != 0) {
    FUN_004417e0(&me);
    for (i = 0; i < n; i++) {
      p = FUN_00425ee0(i);
      d.x = me.x - p->x;
      src[i] = p;
      d.y = p->y;
      d.y = me.y - d.y;
      d.z = p->z;
      { double dz = me.z - d.z; d.z = (float)dz; }
      dist[i] = FUN_004c3ac0(&d);
    }
    do {
      swapped = 0;
      for (i = 0; i < n - 1; i++) {
        if (dist[i + 1] < dist[i]) {
          t = dist[i];
          tp = src[i + 1];
          dist[i] = dist[i + 1];
          dist[i + 1] = t;
          tp2 = src[i];
          src[i] = tp;
          src[i + 1] = tp2;
          swapped = 1;
        }
      }
    } while (swapped == 1);
    for (i = 0; i < 3; i++) {
      if (i >= n) {
        break;
      }
      ch = DAT_00604d88 + i;
      if ((*(unsigned char *)(DAT_0068f640[ch].p + 0x34) & 2) == 0) {
        FUN_005a66d0(DAT_0068f640[ch].p, 1);
        *(float *)(DAT_0068f640[ch].p + 0x3c) = 22050.0f;
        if (*(int *)(DAT_0068f640[ch].p + 0x30) != 0) {
          FUN_005a6dc0(*(int *)(DAT_0068f640[ch].p + 0x30), 4, 1, 22050.0f);
        }
      }
      FUN_0045e0f0(ch, 1.0f);
      p = src[i];
      *(MatchV3_464a50 *)(DAT_0068f640[ch].p + 0x58) = *p;
      if (*(int *)(DAT_0068f640[ch].p + 0x30) != 0) {
        FUN_005a6d60(*(int *)(DAT_0068f640[ch].p + 0x30), 0, 1, p);
      }
    }
    if (other == 1) {
      ch = DAT_00605b68;
      if ((*(unsigned char *)(DAT_0068f640[ch].p + 0x34) & 2) == 0) {
        FUN_005a66d0(DAT_0068f640[ch].p, 1);
        *(float *)(DAT_0068f640[ch].p + 0x3c) = 22050.0f;
        if (*(int *)(DAT_0068f640[ch].p + 0x30) != 0) {
          FUN_005a6dc0(*(int *)(DAT_0068f640[ch].p + 0x30), 4, 1, 22050.0f);
        }
      }
      FUN_0045e0f0(ch, 1.0f);
      p = FUN_00407600(0);
      *(MatchV3_464a50 *)(DAT_0068f640[ch].p + 0x58) = *p;
      if (*(int *)(DAT_0068f640[ch].p + 0x30) != 0) {
        FUN_005a6d60(*(int *)(DAT_0068f640[ch].p + 0x30), 0, 1, p);
      }
    }
  }
}
