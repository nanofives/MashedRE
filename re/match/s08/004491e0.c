// FLAGS: /O2 /GS /Ob1
/* MATCH-S08 0x004491e0: 100% (regs 100%) by hand (h9). Raw 62.3%. Three local vectors (0,0,0), (1,0,0),
   (0,1,0), the matrix pointer is frame + 0x10 and its +0x30 is a struct member (keeps edi = base + 0x10),
   a pointer-walking for loop over the 4 slots at param_1 + 0x100f0, 180.0f literal angle. */
/* ---- MATCH-S08 preamble (same text in every re/match/s08 source; not part of any function body) ----
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
typedef struct { float x, y, z; } MatchV3_004491e0;
typedef struct {
  MatchV3_004491e0 right; int flags;
  MatchV3_004491e0 up; int pad1;
  MatchV3_004491e0 at; int pad2;
  MatchV3_004491e0 pos; int pad3;
} MatchMat_004491e0;
int FUN_004840d0(unsigned int p0);
int FUN_004c1340(int p0, MatchV3_004491e0 *p1, int p2);
int FUN_004c1520(int p0, MatchV3_004491e0 *p1, float p2, int p3);
void FUN_004c15c0(int p0);
// MATCH: 0x004491e0 FUN_004491e0
void FUN_004491e0(int param_1)
{
  MatchMat_004491e0 *mat;
  MatchV3_004491e0 zero;
  MatchV3_004491e0 axis1;
  MatchV3_004491e0 axis2;
  int *slot;
  int i;
  int m;

  mat = (MatchMat_004491e0 *)(*(int *)(FUN_004840d0(0) + 4) + 0x10);
  zero.x = 0.0f;
  zero.y = 0.0f;
  zero.z = 0.0f;
  axis1.x = 1.0f;
  axis1.y = 0.0f;
  axis1.z = 0.0f;
  axis2.x = 0.0f;
  axis2.y = 1.0f;
  axis2.z = 0.0f;
  slot = (int *)(param_1 + 0x100f0);
  for (i = 0; i < 4; i++, slot++) {
    if (*slot != 0) {
      m = *(int *)(*slot + 4);
      FUN_004c15c0(m);
      FUN_004c1340(m, &zero, 0);
      FUN_004c1520(m, &axis1, 180.0f, 1);
      FUN_004c1520(m, &axis2, 180.0f, 2);
      FUN_004c1340(m, &mat->pos, 2);
    }
  }
}
