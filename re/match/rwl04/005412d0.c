// FLAGS: /O2 /Ob1 /G5
/* RWC-DRIVER round 2: permuter 1345 compiles reached 58.3% only through moves that change behaviour (the +0x1b4 call before its vector is filled, swapped FUN_00541b50 / FUN_004d54f0 and render-state get order, alpha read before the pass pointer is set): rejected. goto-placed constant/blend blocks 45.6%, direct parameter reads 49%: rejected. */
/* RWC-DRIVER 2026-11-11: 44.2% -> 55.6% (regs 66.8%, ops 83.7%). Rewritten from the disassembly (the raw body had Ghidra stack junk: iStack_ and unaff_ names in the render-state save/restore tail): typed pass/mesh/instance/texture structs, every IDirect3DDevice9 call __stdcall (+0x170, +0x1ac, +0x1b4 with a 4-float local, +0x118 into a counter, +0x144/+0x148 draw pair as a DRAW block macro), the pass-0 / constant-path / blend-path / multipass-tail structure as in the binary, and the dead parameter slots reused for the flag words and the saved render states as the original does (p2 = texture flag then state 10, p4 = blend flag then state 11, p3/p5/p6/p1 = states 0x89/0xe/0x1c/0x22). Residual: the original converts the alpha float with an inline fstp/fld/fistp (round to nearest, RW inline asm); C gives a __ftol call (truncates), which also changes the frame; plus block placement of the FUN_004d6e70(0,0) arm and copies of the instance pointer. */
/* h2 note: 44.2% (regs 53.9). Fixed compile errors of raw (param_2 float->int, dead param_2=fVar4 removed, ROUND->(uint)cast, FUN_00541b50 proto int). cxx FAIL: FUN_00540340 arg4 int* vs undefined4, code() arity. class (b). */
/* MATCH-RWL04 0x005412d0: raw g2s.py output (re/tools/match/g2s.py on decomp_pc.py --port --callees), body
   unchanged; globals moved to MGLOBAL. Score: see verify/matchrwl04_20261107/results.tsv. */
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
#ifdef __cplusplus
typedef int code(...);
#else
typedef int code();
#endif
typedef unsigned char uchar;
typedef struct Raster { int pad[8]; uchar cType; uchar cFlags; uchar privateFlags; uchar cFormat; } Raster;
typedef struct Tex { Raster *raster; } Tex;
typedef struct Mat { int pad0; uchar r, g, b, alpha; } Mat;
typedef struct Inst { int pad0, pad4; int indexed; int primType; } Inst;
typedef struct Mesh { int pad0, pad4; Mat *mat; int fc; int shader; int baseVertex; int numVertices; int startIndex; int primCount; } Mesh;
typedef struct Pass { int a; int pad4; float f8; int c; int pad10, pad14; } Pass;
typedef long (__stdcall *dev1_fn)(int *, int);
typedef long (__stdcall *dev1p_fn)(int *, int *);
typedef long (__stdcall *spc_fn)(int *, int, float *, int);
typedef long (__stdcall *dp_fn)(int *, int, int, int);
typedef long (__stdcall *dip_fn)(int *, int, int, int, int, int, int);
MGLOBAL(int, DAT_007dc5dc, 0x007dc5dc);
MGLOBAL(float, _DAT_005cd04c, 0x005cd04c);
MGLOBAL(int, DAT_006181d0, 0x006181d0);
MGLOBAL(int *, DAT_007d4110, 0x007d4110);
MGLOBAL(int, DAT_007dc600, 0x007dc600);
MGLOBAL(int, DAT_006181d4, 0x006181d4);
MGLOBAL(int, DAT_007dc604, 0x007dc604);
MGLOBAL(int, DAT_007d3ff8, 0x007d3ff8);
MGLOBAL(int, DAT_00623d34, 0x00623d34);
MGLOBAL(int, DAT_007dc620, 0x007dc620);
MGLOBAL(int, DAT_007dc618, 0x007dc618);
int FUN_004cfee0(Tex *p0);
void FUN_004d3b00(Raster *p0);
void FUN_004d53b0(void);
void FUN_004d5480(int p0, int p1);
void FUN_004d54d0(int p0, int *p1);
void FUN_004d54f0(int p0, int p1, int p2);
int FUN_004d6c40(int p0);
int FUN_004d6c90(int p0);
int FUN_004d6e70(Tex *p0, int p1);
void FUN_004d7100(int p0);
int FUN_004d71f0(void);
void FUN_00540340(Inst *p0, Mesh *p1, uint p2, Tex *p3);
int FUN_00541b50(Tex *p0, int p1, int p2);
#define DRAW(a, m) \
  if ((a)->indexed != 0) { \
    FUN_004d53b0(); \
    (*(dip_fn *)(*DAT_007d4110 + 0x148))(DAT_007d4110,(a)->primType,(m)->baseVertex,0,(m)->numVertices,(m)->startIndex,(m)->primCount); \
  } \
  else { \
    FUN_004d53b0(); \
    (*(dp_fn *)(*DAT_007d4110 + 0x144))(DAT_007d4110,(a)->primType,(m)->baseVertex,(m)->primCount); \
  }
// MATCH: 0x005412d0 FUN_005412d0
void FUN_005412d0(int param_1,int param_2,uint param_3,int param_4,int param_5,int param_6)
{
  Inst *a;
  Mesh *m;
  Tex *tex;
  Tex *env;
  Pass *e;
  uint alpha;
  uint col;
  float v[4];

  a = (Inst *)param_1;
  m = (Mesh *)param_2;
  tex = (Tex *)param_5;
  env = (Tex *)param_6;
  e = &(*(Pass **)(DAT_007dc5dc + (int)m->mat))[param_4];
  alpha = (int)(e->f8 * _DAT_005cd04c) & 0xff;
  if (alpha == 0 || env == 0 || env->raster == 0) {
    if (param_4 == 0) {
      FUN_00540340(a,m,param_3,tex);
    }
    return;
  }
  param_4 = 0;
  param_2 = 0;
  if (DAT_006181d0 != m->shader) {
    DAT_006181d0 = m->shader;
    (*(dev1_fn *)(*DAT_007d4110 + 0x170))(DAT_007d4110,m->shader);
  }
  if (param_4 == 0) {
    if ((param_3 & 0x84) != 0 && tex != 0) {
      FUN_004d6e70(tex,0);
      param_2 = 1;
      if ((tex->raster->cFormat & 0x20) && (env->raster->cFormat & 0x20) && tex != env) {
        FUN_004d3b00(env->raster);
      }
    }
    else {
      FUN_004d6e70(0,0);
    }
    if (e->c != 0 ||
        (m->fc == 0 && m->mat->alpha == 0xff && (param_2 == 0 || FUN_004cfee0(tex) == 0))) {
      if (FUN_004cfee0(env) != 0) {
        if (DAT_007dc600 != 0) {
          v[0] = e->f8;
          v[1] = e->f8;
          v[2] = e->f8;
          v[3] = e->f8;
          (*(spc_fn *)(*DAT_007d4110 + 0x1b4))(DAT_007d4110,0,v,1);
          if (param_2 != 0) {
            FUN_00541b50(env,1,e->a);
            if (DAT_006181d4 != DAT_007dc600) {
              DAT_006181d4 = DAT_007dc600;
              (*(dev1_fn *)(*DAT_007d4110 + 0x1ac))(DAT_007d4110,DAT_007dc600);
            }
            DRAW(a,m);
            FUN_004d6e70(0,1);
            FUN_004d54f0(1,0x18,0);
            FUN_004d54f0(1,0xb,1);
            return;
          }
          FUN_00541b50(env,0,e->a);
          if (DAT_006181d4 != DAT_007dc604) {
            DAT_006181d4 = DAT_007dc604;
            (*(dev1_fn *)(*DAT_007d4110 + 0x1ac))(DAT_007d4110,DAT_007dc604);
          }
          DRAW(a,m);
          FUN_004d6e70(0,0);
          FUN_004d54f0(0,0x18,0);
          FUN_004d54f0(0,0xb,0);
          return;
        }
      }
      else if (DAT_007dc618 != 0 ||
               (alpha == 0xff && (param_2 == 0 || DAT_007dc620 != 0 || DAT_00623d34 != 0))) {
        param_4 = 1;
      }
    }
    if (DAT_006181d4 != 0) {
      DAT_006181d4 = 0;
      (*(dev1_fn *)(*DAT_007d4110 + 0x1ac))(DAT_007d4110,0);
    }
    if (param_2 != 0) {
      FUN_004d54f0(0,1,4);
      FUN_004d54f0(0,2,2);
      FUN_004d54f0(0,3,0);
      FUN_004d54f0(0,4,4);
      FUN_004d54f0(0,5,2);
      FUN_004d54f0(0,6,0);
    }
    else {
      FUN_004d54f0(0,1,3);
      FUN_004d54f0(0,3,0);
      FUN_004d54f0(0,4,3);
      FUN_004d54f0(0,6,0);
    }
    if (param_4 != 0) {
      if (alpha == 0xff && param_2 == 0) {
        FUN_00541b50(env,0,e->a);
        FUN_004d54f0(0,1,7);
        DRAW(a,m);
        FUN_004d6e70(0,0);
        FUN_004d54f0(0,0x18,0);
        FUN_004d54f0(0,0xb,0);
        return;
      }
      FUN_00541b50(env,1,e->a);
      if (alpha != 0xff) {
        FUN_004d5480(0x3c,((alpha << 8 | alpha) << 8 | alpha) << 8 | alpha);
        FUN_004d54f0(1,1,0x19);
        FUN_004d54f0(1,0x1a,1);
        FUN_004d54f0(1,2,2);
        FUN_004d54f0(1,3,3);
      }
      else {
        FUN_004d54f0(1,1,7);
        FUN_004d54f0(1,2,2);
        FUN_004d54f0(1,3,1);
        if (DAT_00623d34 != 0) {
          param_6 = 0;
          FUN_004d53b0();
          if ((*(dev1p_fn *)(*DAT_007d4110 + 0x118))(DAT_007d4110,&param_6) < 0 || (DAT_007dc620 = 1, param_6 != 1)) {
            DAT_007dc620 = 0;
          }
          DAT_00623d34 = 0;
        }
      }
      DRAW(a,m);
      FUN_004d6e70(0,1);
      FUN_004d54f0(1,1,1);
      FUN_004d54f0(1,4,1);
      FUN_004d54f0(1,0x18,0);
      FUN_004d54f0(1,0xb,1);
      return;
    }
    DRAW(a,m);
  }
  FUN_00541b50(env,0,e->a);
  if (FUN_004d71f0() == 0) {
    FUN_004d7100(1);
  }
  (**(code **)(DAT_007d3ff8 + 0x24))(10,&param_2);
  (**(code **)(DAT_007d3ff8 + 0x24))(0xb,&param_4);
  FUN_004d6c40(5);
  FUN_004d6c90(2);
  col = alpha | ((alpha << 8 | alpha) << 8 | alpha) << 8;
  if (col != 0xffffffff) {
    FUN_004d5480(0x3c,col);
    FUN_004d54f0(0,1,4);
    FUN_004d54f0(0,2,2);
    FUN_004d54f0(0,3,3);
    FUN_004d54f0(0,4,2);
    FUN_004d54f0(0,5,2);
    FUN_004d54f0(0,6,3);
  }
  else {
    FUN_004d54f0(0,1,2);
    FUN_004d54f0(0,2,2);
    FUN_004d54f0(0,4,2);
    FUN_004d54f0(0,5,2);
  }
  FUN_004d54d0(0x89,(int *)&param_3);
  FUN_004d54d0(0xe,&param_5);
  FUN_004d54d0(0x1c,&param_6);
  FUN_004d5480(0x89,0);
  FUN_004d5480(0xe,0);
  if (param_6 != 0) {
    FUN_004d54d0(0x22,&param_1);
    FUN_004d5480(0x22,0);
  }
  DRAW(a,m);
  if (param_6 != 0) {
    FUN_004d5480(0x22,param_1);
  }
  FUN_004d5480(0xe,param_5);
  FUN_004d5480(0x89,param_3);
  if (col != 0xffffffff) {
    FUN_004d54f0(0,3,0);
    FUN_004d54f0(0,6,0);
  }
  else {
    FUN_004d54f0(0,1,4);
    FUN_004d54f0(0,4,4);
  }
  FUN_004d7100(0);
  (**(code **)(DAT_007d3ff8 + 0x20))(10,param_2);
  (**(code **)(DAT_007d3ff8 + 0x20))(0xb,param_4);
  FUN_004d54f0(0,0x18,0);
  FUN_004d54f0(0,0xb,0);
}
