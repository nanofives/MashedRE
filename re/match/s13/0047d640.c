// FLAGS: /O2 /GS /Ob1
/* MATCH-S13 h5 0x0047d640 (2181 B, body to 0x0047deef): best 45.7% exact (regs 69.3%, ops 92.6%), 527/514 ins.
   Raw g2s did not compile; written from the decomp. C++ check ok. Class (b).
   Structure that is right: event ring 0x007e9de0 as ONE struct (count + type/stamp/who/rec/data arrays), which is
   what makes every store reload the count; 0x007f1030 loaded before the first store; the four emissions are
   one macro (type 6, who + 0xfeff, colour, 7 dwords from clumps[1..], 16 dwords from local54[5..]); colour is the
   val register for 0xff010101 and a literal for the others; the inner dispatch is
   `if (val != 0xff0000ff) { if (val == 0xffff0080) .. else .. } else ..` (gives the original block order C3,
   default, C2; a switch or a plain if-chain do not, regs 56%); 32-entry id/flag scan unrolls 8x4 as in the original;
   ring count unsigned (jb). Tried with no effect: case order, entry-array base forms (compiler bases the
   ushort entry pointer on the else-branch address, +0x32 here vs +0x30 in the original), local declaration order,
   b4/b8/bc/c0 read into locals first (the original loads all four before the compares; ours folds them),
   state copied to a local, score += vs =. Permuter (689 compiles) reached 60.1% only by moving the
   FUN_0040e350 call above FUN_0046d4a0 (a call-order change, rejected).
   Residual (b): register allocation of the per-node loop (original: esi = track, edi = car, edx = state,
   ecx = who, ebx = entry pointer then val; ours keeps the constant 1 in esi and car in edx, track on the stack),
   which shifts most of the 527 instructions; stack slot order for car/li and mode/unlinked/cnt also differs. */
/* ---- MATCH-S13 preamble (same text in every re/match/s13 source; not part of any function body) ----
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
typedef struct { int a; int w[7]; } Rec20_d640;
typedef struct {
  unsigned int count;        /* 0x007e9de0 */
  int type[256];             /* 0x007e9de4 */
  int stamp[256];            /* 0x007ea1e4 */
  int who[256];              /* 0x007ea5e4 */
  Rec20_d640 rec[256];       /* 0x007ea9e4 */
  int data[256][16];         /* 0x007ec9e4 */
} EvRing_d640;
typedef struct {
  int f0;
  int active;                /* +0x04 */
  int f8;
  int score;                 /* +0x0c */
  int f10;
  int f14;                   /* +0x14 */
  char pad18[0xbfc - 0x18];
  int ids[32];               /* +0xbfc */
  int on[32];                /* +0xc7c */
  char padcfc[0xd04 - 0xcfc];
} Car_d640;
typedef struct {
  unsigned short f0;
  unsigned short hi;         /* +0x02 */
  char pad4[0x22];
  unsigned short lo;         /* +0x26 */
} Ent_d640;                  /* stride 0x28, first at node+0x08 */
typedef struct Node_d640 {
  char pad0[0xac];
  unsigned int n;            /* +0xac */
  int fb0;
  int b4;                    /* +0xb4 */
  int b8;                    /* +0xb8 */
  int bc;                    /* +0xbc */
  int c0;                    /* +0xc0 */
  char padc4[0xdc - 0xc4];
  struct Node_d640 *next;    /* +0xdc */
} Node_d640;
typedef struct { int f0; struct Node_d640 *head; char pad[0x20]; } List_d640;   /* stride 0x28 */
typedef struct { char pad[0xc8]; unsigned int nlists; char padcc[8]; List_d640 *lists; } Owner_d640;
typedef struct { char pad[0x10]; int **tab; } Track_d640;

MGLOBAL(int, DAT_007f1008, 0x007f1008);
MGLOBAL(int, DAT_007f1030, 0x007f1030);
MGLOBAL(EvRing_d640, DAT_007e9de0, 0x007e9de0);
MGLOBALA(int, DAT_006c9a78, 4, 0x006c9a78);
MGLOBALA(Car_d640, DAT_008815a0, 4, 0x008815a0);
int FUN_0040e350(void);
Track_d640 *FUN_00426070(void);
int FUN_0046cbb0(unsigned int i, int *a, int *b);
int FUN_0046d4a0(int **out, int i);
int FUN_0057c210(int a);

#define EMIT_D640(color) \
  DAT_007e9de0.type[DAT_007e9de0.count] = 6; \
  DAT_007e9de0.stamp[DAT_007e9de0.count] = DAT_007f1030; \
  DAT_007e9de0.who[DAT_007e9de0.count] = who + 0xfeff; \
  DAT_007e9de0.rec[DAT_007e9de0.count].a = (color); \
  DAT_007e9de0.rec[DAT_007e9de0.count].w[0] = clumps[1]; \
  DAT_007e9de0.rec[DAT_007e9de0.count].w[1] = clumps[2]; \
  DAT_007e9de0.rec[DAT_007e9de0.count].w[2] = clumps[3]; \
  DAT_007e9de0.rec[DAT_007e9de0.count].w[3] = clumps[4]; \
  DAT_007e9de0.rec[DAT_007e9de0.count].w[4] = clumps[5]; \
  DAT_007e9de0.rec[DAT_007e9de0.count].w[5] = clumps[6]; \
  DAT_007e9de0.rec[DAT_007e9de0.count].w[6] = clumps[7]; \
  for (k = 0; k < 16; k++) \
    DAT_007e9de0.data[DAT_007e9de0.count][k] = local54[5 + k]; \
  DAT_007e9de0.count++; \
  if (DAT_007e9de0.count >= 0x100) \
    DAT_007e9de0.count = 0;

// MATCH: 0x0047d640 FUN_0047d640
void FUN_0047d640(Owner_d640 *param_1)
{
  Node_d640 *node;
  Node_d640 **link;
  Car_d640 *car;
  Track_d640 *track;
  int *pos;
  unsigned int li, cnt;
  unsigned int u;
  int b4, b8, bc, c0;
  int i, k, t, key, val, who, mode, done, unlinked, state;
  unsigned int idx;
  int clumps[8];
  int local54[21];

  local54[1] = 0;
  local54[2] = 0;
  local54[3] = 0;
  local54[4] = 0;
  for (u = 0; u < 4; u++)
    clumps[u] = FUN_0057c210(DAT_006c9a78[u]);
  track = FUN_00426070();
  for (li = 0; li < param_1->nlists; li++) {
    link = &param_1->lists[li].head;
    for (node = *link; node != 0; node = node->next) {
      b4 = node->b4;
      b8 = node->b8;
      bc = node->bc;
      c0 = node->c0;
      done = 0;
      if ((short)b8 == -1) {
        mode = 1;
        key = bc;
      } else if ((short)c0 == -1) {
        mode = 2;
        key = b4;
      } else {
        link = &node->next;
        continue;
      }
      who = 0;
      unlinked = 0;
      for (i = 0; i < 4; i++) {
        if (DAT_008815a0[i].active != 0 && key == clumps[i]) {
          who = i + 1;
          car = &DAT_008815a0[i];
          if (DAT_008815a0[i].f14 == 0)
            unlinked = 1;
          FUN_0046cbb0(i, &state, local54);
        }
      }
      for (cnt = 0; cnt < node->n; cnt++) {
        if (done)
          break;
        if (who != 0 && local54[who] == 0) {
          car->score += DAT_007f1008 * 2;
          local54[who] = 1;
        }
        if (mode == 1)
          idx = ((Ent_d640 *)((char *)node + 8))[cnt].lo;
        else
          idx = ((Ent_d640 *)((char *)node + 8))[cnt + 1].hi;
        val = track->tab[idx][1];
        if (val == (int)0xff010101) {
          *link = node->next;
          done = 1;
          if (who == 0)
            continue;
          if (state == 0) {
            t = FUN_0040e350();
            if (t != 1 && t != 2 && t != 3 && t != 4 && t != 5) {
              EMIT_D640(val)
            }
          }
        } else {
          if (who == 0)
            continue;
          if (val != (int)0xff0000ff) {
            if (val == (int)0xffff0080) {
            if (state == 0 || state == 1) {
              FUN_0046d4a0(&pos, who - 1);
              local54[5] = pos[12];
              local54[6] = pos[13];
              local54[7] = pos[14];
              t = FUN_0040e350();
              if (t != 1 && t != 2 && t != 3 && t != 4 && t != 5) {
                EMIT_D640(0xffff0080)
              }
            }
            } else {
            if (state == 1 && local54[0] > 0x32) {
              t = FUN_0040e350();
              if (t != 1 && t != 2 && t != 3 && t != 4 && t != 5) {
                EMIT_D640(0xff0000ff)
              }
            }
            }
          } else {
            if (state == 0 || state == 1) {
              t = FUN_0040e350();
              if (t != 1 && t != 2 && t != 3 && t != 4 && t != 5) {
                EMIT_D640(0xff0000ff)
              }
            }
          }
        }
        for (k = 0; k < 32; k++) {
          if (car->on[k] != 0 && val == car->ids[k]) {
            *link = node->next;
            done = 1;
          }
        }
      }
      if (done)
        continue;
      if (unlinked)
        *link = node->next;
      else
        link = &node->next;
    }
  }
}
