// FLAGS: /O2 /GS /Ob1
/* MATCH-S09 0x00451ad0 (h1): 100% (regs 100%). Raw converter 40.2% (Ghidra lost the arguments of 0x00451990/0x0041f1e0/0x00481750): typed struct, callee arities from the pushes (0x00451990 3, 0x0041f1e0 3, 0x00481750 3, 0x0046be10 4, 0x00480720 1), the four test-and-fix blocks written out, printf at 0x004a2cbd by its FUN_ name. The and esp,-8 frame came out with no extra source change. Residual: none. */
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
typedef unsigned int undefined4;
typedef unsigned char undefined1;
typedef struct { float x, y, z; } V3_00451ad0;
typedef struct { float r[12]; V3_00451ad0 pos; float pw; } M16_00451ad0;
typedef struct {
  char a0[0xa8];
  V3_00451ad0 va8;        /* 0xa8 */
  int fb4[4];             /* 0xb4 */
} S_00451ad0;
MGLOBAL(undefined1, DAT_005f9788, 0x005f9788);
MGLOBALA(int, DAT_0088ff50, 4, 0x0088ff50);
MGLOBALA(char, s_movement_is___f___f___f__005ce368, 26, 0x005ce368);
int FUN_004a2cbd(char *p0, ...);
int FUN_0040e340(void);
void FUN_0041f1e0(int p0, int p1, M16_00451ad0 *p2);
undefined4 FUN_00451990(S_00451ad0 *p0, M16_00451ad0 *p1, M16_00451ad0 *p2);
undefined4 FUN_0046be10(void *p0, int p1, int p2, int p3);
undefined4 FUN_0046bf50(V3_00451ad0 *p0, int p1);
undefined4 FUN_0046d4a0(M16_00451ad0 **p0, int p1);
undefined4 FUN_00480720(int p0);
void FUN_00481750(int p0, V3_00451ad0 *p1, V3_00451ad0 *p2);
// MATCH: 0x00451ad0 FUN_00451ad0
void FUN_00451ad0(S_00451ad0 *param_1)
{
  M16_00451ad0 *src;
  int res;
  int n;
  V3_00451ad0 v;
  M16_00451ad0 m;
  int i;

  n = FUN_0040e340();
  v.x = 0.0f;
  v.y = -75.0f;
  v.z = 0.0f;
  for (i = 0; i < n; i++) {
    FUN_0046d4a0(&src,i);
    res = FUN_00451990(param_1,src,src);
    if (res != 0) {
      FUN_0046be10(&DAT_005f9788,i,0,0);
      DAT_0088ff50[i] = 1;
    }
    else {
      DAT_0088ff50[i] = 0;
      if (param_1->fb4[i] != 0) {
        FUN_004a2cbd(s_movement_is___f___f___f__005ce368,(double)param_1->va8.x,
                     (double)param_1->va8.y,(double)param_1->va8.z);
        FUN_0046bf50(&param_1->va8,i);
        FUN_0041f1e0(i,0,&m);
        if (FUN_00451990(param_1,&m,&m) == 0) {
          FUN_00481750(i,&v,&m.pos);
        }
        FUN_0041f1e0(i,1,&m);
        if (FUN_00451990(param_1,&m,&m) == 0) {
          FUN_00481750(i,&v,&m.pos);
        }
        FUN_0041f1e0(i,2,&m);
        if (FUN_00451990(param_1,&m,&m) == 0) {
          FUN_00481750(i,&v,&m.pos);
        }
        FUN_0041f1e0(i,3,&m);
        if (FUN_00451990(param_1,&m,&m) == 0) {
          FUN_00481750(i,&v,&m.pos);
        }
        FUN_00480720(i);
      }
      FUN_0046be10(&DAT_005f9788,i,0,1);
    }
    param_1->fb4[i] = res;
  }
}
