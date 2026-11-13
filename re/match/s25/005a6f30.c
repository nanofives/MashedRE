// FLAGS: /O2 /Ob1 /G5
/* PORT-TOP-AUDIO-RWA 2026-11-13: body rewritten by hand from the original disassembly at 0x005a6f30..0x005a71e5 (the
   raw g2s text below did not compile as C++ and dropped stores). L-126 provisional body (re/match/PROVISIONAL.tsv),
   knob path only, never C4 by this status. Float arguments are passed as their dword bit pattern: the original
   pushes them with fld dword / fstp dword [esp] (one stack dword), the same bytes the callee reads. */
/* MATCH-S25 h3 NOTE: permuter best 65.4% (regs 70.9%, ops 80.2%, 223/237 ins), raw converter start. NOT port-ready: the VS2022 C++ check FAILS (callee prototypes of FUN_005a6dc0/005a6df0 are 4-arg vararg-style calls with float args passed by value via fld/fstp, FUN_005a6e10/005a7460 types, DAT_007dca18 is a list head pointer, ROUND() is an unresolved converter macro that the DDK compiles as an implicit call, and the goto LAB_005a71bf block drops the puVar5[8]/[0xb] stores). Residual (b): needs a hand rewrite of the float-arg calls and the loop; budget spent. */
/* MATCH-S25 0x005a6f30: raw g2s.py output (re/tools/match/g2s.py on decomp_pc.py --port --callees), body
   unchanged; globals moved to MGLOBAL. Score: see verify/matchs25_20261107/results.tsv. */
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
typedef unsigned char undefined1;
typedef unsigned int undefined4;
typedef unsigned int uint;
typedef unsigned char byte;
MGLOBAL(int, DAT_007dca34, 0x007dca34);
MGLOBAL(uint, DAT_007dca44, 0x007dca44);
MGLOBAL(uint, DAT_007dca48, 0x007dca48);
MGLOBAL(uint, DAT_007dca4c, 0x007dca4c);
MGLOBAL(uint, DAT_00632f08, 0x00632f08);
MGLOBAL(int, DAT_007dca18, 0x007dca18);
MGLOBAL(int, DAT_007dca40, 0x007dca40);
MGLOBAL(int, DAT_007dca38, 0x007dca38);
MGLOBAL(int, DAT_007dca50, 0x007dca50);
MGLOBAL(float, DAT_005cc558, 0x005cc558);
uint FUN_005a6d60(undefined4 * p0, int p1, undefined4 p2, undefined4 * p3);
int FUN_005a6dc0(int p0, undefined4 p1, undefined4 p2, undefined4 p3);
uint FUN_005a6df0(int p0, undefined4 p1, undefined4 p2, undefined4 p3);
undefined4 FUN_005a6e10(undefined4 p0, undefined4 p1, undefined4 p2, uint * p3);
void FUN_005a7460(int * p0);
uint FUN_005acda0(int p0, uint p1);
#define F32BITS(x) (*(undefined4 *)&(x))
// MATCH: 0x005a6f30 FUN_005a6f30
/* [C2 2026-06-01] Audio mixer flush / per-frame voice-allocation tick. `void FUN_005a6f30(void)`.
   Gated on the mixer object DAT_007dca34. Node list head DAT_007dca18 (circular, sentinel = &DAT_007dca18). */
void FUN_005a6f30(void)
{
  int *node;
  int *next;
  int obj;
  int used;
  uint len;
  uint pos;
  float f;

  if (DAT_007dca34 == 0) {
    return;
  }
  obj = (int)FUN_005a6e10(DAT_007dca34, 1, 2, 0);
  if (obj != 0) {
    DAT_007dca44 = *(uint *)(obj + 0x28);
    DAT_007dca48 = *(uint *)(obj + 0x2c);
    DAT_007dca4c = *(uint *)(obj + 0x30);
    DAT_00632f08 = *(uint *)(obj + 0x64);
  }
  node = (int *)DAT_007dca18;
  used = 0;
  if (node == &DAT_007dca18) {
    return;
  }
  do {
    obj = node[2];
    next = (int *)node[0];
    if (used < DAT_007dca40 - DAT_007dca38) {
      if ((node[3] & 0x10) != 0) {
        FUN_005a7460(node);
      } else if (obj != 0) {
        if ((node[3] & 8) != 0) {
          node[8] = node[9];
        } else {
          node[8] = (int)FUN_005a6e10(obj, 7, 2, 0);
        }
        FUN_005a6dc0(obj, 6, 1, 0);
        FUN_005a6df0(obj, 9, 1, 0);
        node[0xb] = DAT_007dca50;
        node[2] = 0;
      }
      if ((node[3] & 1) != 0 && (node[3] & 8) == 0) {
        len = FUN_005acda0(node[10] + 0x10, *(uint *)(node[10] + 0x18));
        /* 0x005a7044: fild qword (zero-extended dword) / fmul dword [node+0x14] / fmul dword [0x5cc558] / fstp dword,
           then the inline truncation (fnstcw / or ah,0xc / fldcw / fistp) */
        f = (float)(longlong)(ulonglong)(uint)(DAT_007dca50 - node[0xb]);
        f = f * *(float *)&node[5];
        f = f * DAT_005cc558;
        pos = (uint)node[8] + (uint)(int)f;
        node[8] = (int)pos;
        if (pos >= len) {
          if ((*(uchar *)&node[3] & 4) != 0) {
            do {
              pos = pos - len;
            } while (pos >= len);
            node[8] = (int)pos;
            node[0xb] = DAT_007dca50;
            goto next_node;
          }
          FUN_005a7460(node);
          node[2] = 0;
        }
        node[0xb] = DAT_007dca50;
      }
    } else if (obj == 0) {
      obj = (int)FUN_005a6e10(DAT_007dca34, 4, 2, 0);
      node[2] = obj;
      if (obj != 0) {
        FUN_005a6df0(obj, 9, 1, (undefined4)node[10]);
        FUN_005a6dc0(obj, 4, 1, (undefined4)node[5]);
        FUN_005a6dc0(obj, 5, 1, (undefined4)node[6]);
        FUN_005a6dc0(obj, 8, 1, (uint)node[3] >> 2 & 1);
        if ((*(uchar *)&node[3] & 8) == 0) {
          FUN_005a6dc0(obj, 7, 1, (undefined4)node[8]);
        }
        FUN_005a6dc0(obj, 10, 1, (undefined4)node[7]);
        FUN_005a6d60((undefined4 *)obj, 0, 1, (undefined4 *)(node + 0xc));
        FUN_005a6d60((undefined4 *)obj, 1, 1, (undefined4 *)(node + 0xf));
        FUN_005a6dc0(obj, 2, 1, (undefined4)node[0x12]);
        FUN_005a6dc0(obj, 3, 1, (undefined4)node[0x13]);
        FUN_005a6dc0(obj, 0xb, 1, (undefined4)node[0x18]);
        FUN_005a6dc0(obj, 6, 1, 1);
      }
    }
next_node:
    used = used + node[4];
    node = next;
  } while (node != &DAT_007dca18);
}
