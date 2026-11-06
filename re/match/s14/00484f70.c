// FLAGS: /O2 /GS /Ob1
/* MATCH-S14 0x00484f70 (h2): world-object pair pass, all pairs (no grid). 68.1% (regs 70.3%, ops 91.2%).
   Raw: compile error (code* cast). Structure, compare order, callback cache and the x87 sequence match
   (sum written dz*dz + (dx*dx + dy*dy) as in 0x00484de0). Residual (b): induction variables. The original
   keeps three pointer IVs (ebp = &DAT_006dccb8[i]+4, esi = &DAT_006dccb8[j]+4, a spilled j-start pointer
   [esp+0x18]) and forms &DAT[i]/&DAT[j] with lea -4 at the hit; ours derives the j pointer from one i
   pointer at +0x10 (lea esi,[edi+0x90]). Tried: int counters, while(++j), do-while outer, nested-if
   condition, explicit a/b/bs pointer IVs (64.8-65.9%), a/b locals per pair (49.5-54.9%), permuter 113. */
/* ---- MATCH-S11 preamble (same text in every re/match/s11 source; not part of any function body) ----
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
struct WorldObj;
typedef void (__cdecl *WorldObjCb)(struct WorldObj *self, struct WorldObj *other);
typedef struct WorldCell {
  struct WorldObj *objs[100];
  int count;
  int f194[7];
  int f1b0;
  int f1b4;
} WorldCell;
typedef struct WorldObj {
  float pos[3];
  float radius;
  int owner;
  int f14;
  int f18;
  WorldObjCb cb;
  short idx;
  short pad22;
  WorldCell *cells[16];
  int ncells;
  unsigned char seen[36];
} WorldObj;
MGLOBALA(WorldObj, DAT_006dccb8, 300, 0x006dccb8);
MGLOBAL(uint, DAT_006e70cc, 0x006e70cc);
// MATCH: 0x00484f70 FUN_00484f70
void FUN_00484f70(void)
{
  uint i;
  uint j;
  WorldObjCb cb;
  float dx, dy, dz, r;

  for (i = 0; i < DAT_006e70cc; i++) {
    for (j = i + 1; j < DAT_006e70cc; j++) {
      if (DAT_006dccb8[i].owner == DAT_006dccb8[j].owner) continue;
      cb = DAT_006dccb8[i].cb;
      if (cb == 0 && DAT_006dccb8[j].cb == 0) continue;
      dx = DAT_006dccb8[i].pos[0] - DAT_006dccb8[j].pos[0];
      dy = DAT_006dccb8[i].pos[1] - DAT_006dccb8[j].pos[1];
      dz = DAT_006dccb8[i].pos[2] - DAT_006dccb8[j].pos[2];
      r = DAT_006dccb8[i].radius + DAT_006dccb8[j].radius;
      if (dz * dz + (dx * dx + dy * dy) < r * r) {
        if (cb != 0) cb(&DAT_006dccb8[i], &DAT_006dccb8[j]);
        if (DAT_006dccb8[j].cb != 0) DAT_006dccb8[j].cb(&DAT_006dccb8[j], &DAT_006dccb8[i]);
      }
    }
  }
}
