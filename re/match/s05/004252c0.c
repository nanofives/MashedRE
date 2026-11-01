// FLAGS: /O2 /GS /Ob1
/* MATCH-S05 0x004252c0 (1889 B per Ghidra; the obj symbol runs to the ret at 0x00425a31), h9 by hand: 87.7%
   (regs 89.7%). Raw 31.9%. Shape: 4-car record table 0x008994c0 (stride 0x138, same struct as 0x004248b0) as one
   MGLOBALA; slot table 0x007f1038 (0x4c) via DAT_007f1a14[c].slot; the 0x007e9de0 event queue as one struct (S02
   0x0040b290); every .rdata float is a literal (0, 360, 330, 350, -1, 1, 57.2958, 180, 90, 60); acos is the CRT
   stack entry FUN_004a3384. Moved it: the spin check written twice (each arm owns its while loop and test, the
   compiler tail-merges the test), dot terms y,x,z, the facing dot with the first FUN_00426cc0 result in a temp
   and a partial sum, rec assigned before slot (permuter, 856 compiles).
   Residual (b): (1) facing dot: y/z terms emit fld [eax+k]; fmul local in the original, ours fld local; fmul [eax+k]
   (operand order in source does not move it); (2) final loop: the original keeps i+0xff00 in edi and &arrA[i] in
   ebx and rematerialises i with lea per call, ours CSEs i into edi and spills the arrA pointer (tried an id loop,
   a separate id counter, a pointer over arrA, nested/continue conditions, declaration order); (3) one ecx/edx swap
   in the outer loop step. relocheck reports offsets shifted by the residual only. */
/* ---- MATCH-S05 preamble (same text in every re/match/s05 source; not part of any function body) ----
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
typedef struct { float x; float y; float z; } V3;
typedef struct { uint count; int type[256]; int a[256]; int b[256]; int c[256][8]; } EvQ;
typedef struct { int slot; int pad[3]; } CarSlotIdx;
typedef struct { char b0; char b1; char pad2[3]; char b5; char pad6[0x46]; } Slot4c;
typedef struct {
  int f000;          /* +0x000 */
  int pad004;        /* +0x004 */
  int score;         /* +0x008 */
  int evt;           /* +0x00c */
  int timer;         /* +0x010 */
  char pad014[0x88]; /* +0x014 */
  int f09c;          /* +0x09c */
  int f0a0;          /* +0x0a0 */
  int f0a4;          /* +0x0a4 */
  int f0a8;          /* +0x0a8 */
  int f0ac;          /* +0x0ac */
  char pad0b0[0x50]; /* +0x0b0 */
  float ang;         /* +0x100 */
  int st;            /* +0x104 */
  int f108;          /* +0x108 */
  int hit[4];        /* +0x10c */
  char pad11c[0x10]; /* +0x11c */
  int killer;        /* +0x12c */
  int ktime;         /* +0x130 */
  int f134;          /* +0x134 */
} CarRec;
MGLOBALA(CarRec, DAT_008994c0, 4, 0x008994c0);
MGLOBALA(Slot4c, DAT_007f1038, 8, 0x007f1038);
MGLOBALA(CarSlotIdx, DAT_007f1a14, 4, 0x007f1a14);
MGLOBAL(EvQ, DAT_007e9de0, 0x007e9de0);
MGLOBAL(int, DAT_007f0ff8, 0x007f0ff8);
MGLOBAL(int, DAT_007f1030, 0x007f1030);
int FUN_00407a40(int car);
int FUN_0040e350(void);
int FUN_0040e370(int car);
int FUN_0040e470(int car);
void FUN_00415860(int car);
float FUN_004233e0(float x, float z);
V3 *FUN_00426cc0(int id);
int FUN_0046c7b0(int car);
void FUN_0046cbb0(int car, int *a, int *b);
void FUN_0046cc10(int *out, int car);
void FUN_0046d510(V3 *out, int car);
double FUN_004a3384(double x);
// MATCH: 0x004252c0 FUN_004252c0
void FUN_004252c0(int param_1)
{
  int i;
  int c;
  int j;
  int m;
  int state;
  float ang;
  float d;
  float dot;
  V3 p1;
  V3 p2;
  int arrA[4];
  int arrB[4];
  Slot4c *slot;
  CarRec *rec;
  V3 *pv;

  for (i = 0; i < 4; i++) {
    FUN_0046cbb0(i, &arrA[i], &arrB[i]);
  }
  if (FUN_0040e350() != 7) {
    for (c = 0; c < 4; c++) {
      rec = &DAT_008994c0[c];
      slot = &DAT_007f1038[DAT_007f1a14[c].slot];
      if (FUN_0040e370(c) && FUN_0046c7b0(c) == 1) {
        FUN_0046cc10(&state, c);
        if (state == 4) {
          FUN_0046d510(&p1, c);
          ang = FUN_004233e0(p1.x, p1.z);
          if (slot->b0 != 0) {
            if (rec->st == 1) {
              d = ang - rec->ang;
              while (d < 0.0f) d += 360.0f;
              if (d > 330.0f && d < 350.0f) {
                rec->score += 200;
                rec->st = 0;
                rec->evt = 1;
                rec->timer = 3000;
              }
            } else {
              rec->ang = ang;
              rec->st = 1;
            }
          } else if (slot->b1 != 0) {
            if (rec->st == 2) {
              d = rec->ang - ang;
              while (d < 0.0f) d += 360.0f;
              if (d > 330.0f && d < 350.0f) {
                rec->score += 200;
                rec->st = 0;
                rec->evt = 1;
                rec->timer = 3000;
              }
            } else {
              rec->ang = ang;
              rec->st = 2;
            }
          } else {
            rec->st = 0;
          }
        }
        if (slot->b5 != 0) {
          FUN_0046d510(&p1, c);
          pv = FUN_00426cc0(FUN_00407a40(c));
          dot = p1.x * pv->x + FUN_00426cc0(FUN_00407a40(c))->y * p1.y;
          if (dot + FUN_00426cc0(FUN_00407a40(c))->z * p1.z < 0.0f) {
            rec->f108 += param_1;
            if (rec->f108 > 9000) {
              rec->score += 200;
              rec->evt = 2;
              rec->timer = 3000;
              rec->f108 = 0;
            }
          } else {
            rec->f108 = 0;
          }
        }
        for (j = 0; j < 4; j++) {
          if (c != j && FUN_0040e370(j) && FUN_0046c7b0(j) == 1 && rec->hit[j] == DAT_007f0ff8) {
            rec->f09c++;
            if (state == 4 && rec->f134 == 1) {
              DAT_008994c0[j].f0a0++;
              m = FUN_0040e350();
              if (m != 1 && m != 2 && m != 3 && m != 4 && m != 5) {
                DAT_007e9de0.type[DAT_007e9de0.count] = 0x3ff03;
                DAT_007e9de0.a[DAT_007e9de0.count] = DAT_007f1030;
                DAT_007e9de0.b[DAT_007e9de0.count] = j + 0xff00;
                DAT_007e9de0.c[DAT_007e9de0.count][0] = c + 0xff00;
                DAT_007e9de0.count++;
                if (DAT_007e9de0.count >= 256) {
                  DAT_007e9de0.count = 0;
                }
              }
              if (FUN_0040e470(j) == 1) {
                FUN_00415860(c);
              }
              FUN_0046d510(&p1, c);
              FUN_0046d510(&p2, j);
              dot = p1.y * p2.y + p1.x * p2.x + p1.z * p2.z;
              if (dot < -1.0f) dot = -1.0f;
              else if (dot > 1.0f) dot = 1.0f;
              d = (float)FUN_004a3384(dot) * 57.2958f;
              if (d < 0.0f) d = -d;
              while (d > 180.0f) d -= 180.0f;
              if (d > 90.0f) d = 180.0f - d;
              if (d > 60.0f) {
                m = FUN_0040e350();
                if (m != 1 && m != 2 && m != 3 && m != 4 && m != 5) {
                  DAT_007e9de0.type[DAT_007e9de0.count] = 0x3ff04;
                  DAT_007e9de0.a[DAT_007e9de0.count] = DAT_007f1030;
                  DAT_007e9de0.b[DAT_007e9de0.count] = j + 0xff00;
                  DAT_007e9de0.c[DAT_007e9de0.count][0] = c + 0xff00;
                  DAT_007e9de0.count++;
                  if (DAT_007e9de0.count >= 256) {
                    DAT_007e9de0.count = 0;
                  }
                }
              }
            }
          }
        }
      }
    }
  }
  for (i = 0; i < 4; i++) {
    rec = &DAT_008994c0[i];
    if (rec->f0ac == 0 && FUN_0040e370(i) && (arrA[i] != 0 || FUN_0046c7b0(i) != 1)) {
      m = FUN_0040e350();
      if (m != 1 && m != 2 && m != 3 && m != 4 && m != 5) {
        DAT_007e9de0.type[DAT_007e9de0.count] = 5;
        DAT_007e9de0.a[DAT_007e9de0.count] = DAT_007f1030;
        DAT_007e9de0.b[DAT_007e9de0.count] = i + 0xff00;
        DAT_007e9de0.count++;
        if (DAT_007e9de0.count >= 256) {
          DAT_007e9de0.count = 0;
        }
      }
      if (rec->killer >= 1000) {
        if (DAT_007f0ff8 - rec->ktime < 4500) {
          m = FUN_0040e350();
          if (m != 1 && m != 2 && m != 3 && m != 4 && m != 5) {
            DAT_007e9de0.type[DAT_007e9de0.count] = 0x3ff01;
            DAT_007e9de0.a[DAT_007e9de0.count] = DAT_007f1030;
            DAT_007e9de0.b[DAT_007e9de0.count] = rec->killer - 1000 + 0xff00;
            DAT_007e9de0.c[DAT_007e9de0.count][0] = i + 0xff00;
            DAT_007e9de0.count++;
            if (DAT_007e9de0.count >= 256) {
              DAT_007e9de0.count = 0;
            }
          }
          DAT_008994c0[rec->killer - 1000].f0a4++;
          rec->killer = 0;
        } else {
          rec->f0a8++;
          m = FUN_0040e350();
          if (m != 1 && m != 2 && m != 3 && m != 4 && m != 5) {
            DAT_007e9de0.type[DAT_007e9de0.count] = 0x3ff02;
            DAT_007e9de0.a[DAT_007e9de0.count] = DAT_007f1030;
            DAT_007e9de0.b[DAT_007e9de0.count] = i + 0xff00;
            DAT_007e9de0.count++;
            if (DAT_007e9de0.count >= 256) {
              DAT_007e9de0.count = 0;
            }
          }
        }
      } else {
        rec->f0a8++;
        m = FUN_0040e350();
        if (m != 1 && m != 2 && m != 3 && m != 4 && m != 5) {
          DAT_007e9de0.type[DAT_007e9de0.count] = 0x3ff02;
          DAT_007e9de0.a[DAT_007e9de0.count] = DAT_007f1030;
          DAT_007e9de0.b[DAT_007e9de0.count] = i + 0xff00;
          DAT_007e9de0.count++;
          if (DAT_007e9de0.count >= 256) {
            DAT_007e9de0.count = 0;
          }
        }
      }
      rec->f0ac = 1;
    }
  }
}

