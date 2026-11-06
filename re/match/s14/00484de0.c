// FLAGS: /O2 /GS /Ob1
/* MATCH-S14 0x00484de0 (h2): world-object pair pass over the grid cells. 99.2% (regs 99.2%, ops 99.2%).
   Raw: compile error (code* cast). Moved: record/cell structs, obj = &DAT_006dccb8[i] inside an indexed loop
   (a pointer IV gave a second IV at +0x10, 52%), callback cached in a local, sum written dz*dz + (dx*dx + dy*dy)
   (flat sums get reordered, 95.4%). Residual (b): the spill `mov [esp+0x18],eax` of the cell counter n is
   scheduled before `je` instead of after `lea ecx,[edi+eax*4+0x24]`; tried continue/if-nesting, pc-first,
   unsigned n, local order, index form (71-75%), permuter 195 variants. */
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
// MATCH: 0x00484de0 FUN_00484de0
void FUN_00484de0(void)
{
  uint i;
  WorldObj *obj;
  WorldObj *other;
  WorldCell *cell;
  WorldCell **pc;
  int n;
  int j;
  int idx;
  float dx, dy, dz, r;
  WorldObjCb cb;

  for (i = 0; i < DAT_006e70cc; i++) {
    obj = &DAT_006dccb8[i];
    n = obj->ncells;
    if (n == 0) continue;
    pc = &obj->cells[n];
    do {
      cell = *--pc;
      if (cell->f1b0 == 1 && cell->f1b4 == 0) {
        for (j = 0; j < cell->count; j++) {
          other = cell->objs[j];
          idx = other->idx;
          if (idx <= (int)i) continue;
          if (obj->owner == other->owner) continue;
          cb = obj->cb;
          if (cb == 0 && other->cb == 0) continue;
          if (obj->seen[idx >> 3] & (1 << (idx & 7))) continue;
          dx = obj->pos[0] - other->pos[0];
          dy = obj->pos[1] - other->pos[1];
          dz = obj->pos[2] - other->pos[2];
          r = obj->radius + other->radius;
          if (dz * dz + (dx * dx + dy * dy) < r * r) {
            if (cb != 0) cb(obj, other);
            if (other->cb != 0) other->cb(other, obj);
            obj->seen[other->idx >> 3] |= (unsigned char)(1 << (other->idx & 7));
            other->seen[obj->idx >> 3] |= (unsigned char)(1 << (obj->idx & 7));
          }
        }
      }
    } while (--n != 0);
  }
}
