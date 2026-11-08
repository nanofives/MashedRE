// FLAGS: /O2 /Ob1 /G5
/* MATCH-S21 0x004d4a20 (h3): best 99.4% (regs 99.4%, ops 100%). Raw 15.0%. Copy of an object with count (+4) and an array of
   0x28-byte records (+8): record struct copy, four fields zeroed, data block reallocated through RwGlobals slot
   0x108 (size, 0x30409) and memcpy'd; error {1, FUN_004d7ff0(0x80000013, size)} to FUN_004d8480; then the 0x80
   blocks and 0xc triples carved after RwGlobals+[0x00911ae0]+0x38 records. What moved it: 'size > 0' (jbe),
   a local copy of param_1 plus the error struct declared inside the first loop body (frame 8, base spill
   shares its slot, address operand order matches), j++ inside the last store. Residual: one swap in the last
   loop (add eax,0xc before add edi,0x28 in the original). Tried p += 3 / p = &p[3] / a 3-int struct ++,
   j++ placement, do/while, permuter 300 s x3. Class (b). */
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
typedef struct Blk4a20 { int v[32]; } Blk4a20;
typedef struct Rec4a20 { int f0; int f4; Blk4a20 *blk; int c; int d; int e; int f; int *tri; void *data; unsigned int size; } Rec4a20;
typedef struct Obj4a20 { int f0; unsigned int count; Rec4a20 *recs; } Obj4a20;
typedef struct Err4a20 { int type; int code; } Err4a20;
typedef void *(*Fn108)(unsigned int, int);
MGLOBAL(int, DAT_007d3ff8, 0x007d3ff8);
MGLOBAL(int, DAT_00911ae0, 0x00911ae0);
int FUN_004d7ff0(int p0, ...);
Err4a20 *FUN_004d8480(Err4a20 *p0);
void *memcpy(void *, const void *, unsigned int);
#pragma intrinsic(memcpy)
// MATCH: 0x004d4a20 FUN_004d4a20
int FUN_004d4a20(Obj4a20 *param_1, Obj4a20 *param_2)
{
  int i;
  unsigned int j;
  char *base;
  int *p;
  Obj4a20 *dst = param_1;
  if (dst != param_2) {
    for (i = param_2->count - 1; i >= 0; i--) {
      Err4a20 err;
      dst->recs[i] = param_2->recs[i];
      dst->recs[i].c = 0;
      dst->recs[i].d = 0;
      dst->recs[i].e = 0;
      dst->recs[i].f = 0;
      if (dst->recs[i].size > 0) {
        dst->recs[i].data = (*(Fn108 *)(DAT_007d3ff8 + 0x108))(dst->recs[i].size, 0x30409);
        if (dst->recs[i].data == 0) {
          err.type = 1;
          err.code = FUN_004d7ff0(0x80000013, dst->recs[i].size);
          FUN_004d8480(&err);
          return 0;
        }
        memcpy(dst->recs[i].data, param_2->recs[i].data, dst->recs[i].size);
      }
    }
    dst->count = param_2->count;
  }
  base = (char *)(dst->recs + *(int *)(DAT_007d3ff8 + DAT_00911ae0 + 0x38));
  for (i = param_2->count - 1; i >= 0; i--) {
    dst->recs[i].blk = (Blk4a20 *)base + i;
    if (param_2->recs[i].blk) {
      *dst->recs[i].blk = *param_2->recs[i].blk;
    }
  }
  p = (int *)((Blk4a20 *)base + *(int *)(DAT_007d3ff8 + DAT_00911ae0 + 0x38));
  for (j = 0; j < param_2->count; ) {
    p[0] = 0;
    p[1] = 0;
    p[2] = 0;
    dst->recs[j++].tri = p;
    p += 3;
  }
  return 1;
}
