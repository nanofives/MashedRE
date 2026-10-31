// FLAGS: /O2 /GS /Ob1
/* 0x0040e180 (423 B Ghidra size; whole obj symbol 433 B): BYTE-IDENTICAL (match.py --bytes, relocations masked).
   Instruction score 95.1% only because Ghidra size stops 10 B before the real end (second *param_2 store tail).
   Raw Ghidra 42.6%. Steps: RwMatrix pos at +0x30, first car pos as three float scalars, second car pos as a
   field-by-field RwV3d b (gives the 0x3c frame and the spilled b.z), "if (bestA == -1) bestA = 0" (xor eax,eax
   store), init order bestB, best, bestA. FUN_004c3ac0 = RwV3dLength-shaped (float return). No residual. */
/* ---- MATCH-S02 preamble (same text in every re/match/s02 source; not part of any function body) ----
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
typedef struct { float x; float y; float z; } RwV3d;
typedef struct {
  RwV3d right; uint flags; RwV3d up; uint pad1; RwV3d at; uint pad2; RwV3d pos; uint pad3;
} RwMatrix;
MGLOBAL(char *, PTR_PTR_005f2770, 0x005f2770);
int FUN_0046c7b0(int p0);
int FUN_0046cbb0(int p0, int *p1, int *p2);
int FUN_0046d4a0(RwMatrix **p0, int p1);
float FUN_004c3ac0(RwV3d *p0);

// MATCH: 0x0040e180 FUN_0040e180
void FUN_0040e180(int *param_1, int *param_2)
{
  int bestA;
  int bestB;
  float best;
  int i;
  int j;
  int flag;
  int other;
  RwMatrix *m;
  float ax;
  float ay;
  float az;
  RwV3d b;
  RwV3d d;
  float dist;

  bestB = -1;
  best = 0.0f;
  bestA = -1;
  for (i = 0; i < 4; i++) {
    if (*(int *)(PTR_PTR_005f2770 + 0x34 + i * 4) != 0 && FUN_0046c7b0(i) == 1) {
      FUN_0046cbb0(i, &flag, &other);
      if (flag == 0) {
        FUN_0046d4a0(&m, i);
        ax = m->pos.x;
        ay = m->pos.y;
        az = m->pos.z;
        for (j = 0; j < 4; j++) {
          if (*(int *)(PTR_PTR_005f2770 + 0x34 + j * 4) != 0 && FUN_0046c7b0(j) == 1) {
            FUN_0046cbb0(j, &flag, &other);
            if (flag == 0) {
              FUN_0046d4a0(&m, j);
              b.x = m->pos.x;
              b.y = m->pos.y;
              b.z = m->pos.z;
              d.x = ax - b.x;
              d.y = ay - b.y;
              d.z = az - b.z;
              dist = FUN_004c3ac0(&d);
              if (dist >= best) {
                best = dist;
                bestB = j;
                bestA = i;
              }
            }
          }
        }
      }
    }
  }
  if (bestB == -1) bestB = bestA;
  if (bestA == -1) bestA = bestB;
  if (bestB == -1) bestB = 0;
  if (bestA == -1) bestA = 0;
  *param_1 = bestB;
  *param_2 = bestA;
}
