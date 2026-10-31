// FLAGS: /O2 /GS /Ob1
/* 0x0040de30 FUN_0040de30 (229 B): 100.0% exact, regs 100.0%.
   Raw g2s was 30.2%. Hand form: RenderWare-shaped structs (camera +4 frame, +0x14 projection, +0x68 view
   window; frame +0x10 modelling matrix, copied by struct assignment = rep movsd), local identity matrix with
   flags |= 0x20003, view window {0.6f, 0.45f}. Statement-order stores gave 84.9%; the original store order is
   that of two chained assignments (rightmost stored first). Residual: none. */
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
typedef struct V3 { float x, y, z; } V3;
typedef struct V2 { float x, y; } V2;
typedef struct RwMatrix { V3 right; uint flags; V3 up; uint pad1; V3 at; uint pad2; V3 pos; uint pad3; } RwMatrix;
typedef struct RwFrame { char obj[0x10]; RwMatrix modelling; } RwFrame;
typedef struct RwCamera { char pad0[4]; RwFrame *parent; char pad8[0xc]; int projectionType; char pad18[0x50];
  V2 viewWindow; } RwCamera;
MGLOBAL(RwMatrix, DAT_0063b928, 0x0063b928);
MGLOBAL(int, DAT_0063b9a8, 0x0063b9a8);
MGLOBAL(V2, DAT_0063b9ac, 0x0063b9ac);

RwFrame *FUN_004c1480(RwFrame *f, RwMatrix *m, int combine);
RwCamera *FUN_004c1c80(RwCamera *c, V2 *vw);
RwCamera *FUN_004c1c10(RwCamera *c, int proj);
void FUN_00426670(RwCamera *c);

// MATCH: 0x0040de30 FUN_0040de30
void FUN_0040de30(RwCamera *param_1)
{
  RwFrame *frame;
  V2 vw;
  RwMatrix m;

  frame = param_1->parent;
  vw.x = 0.6f;
  vw.y = 0.45f;
  DAT_0063b928 = frame->modelling;
  DAT_0063b9ac = param_1->viewWindow;
  DAT_0063b9a8 = param_1->projectionType;
  m.right.x = m.up.y = m.at.z = 1.0f;
  m.pos.x = m.pos.y = m.pos.z = m.up.z = m.at.x = m.at.y = m.right.y = m.right.z = m.up.x = 0.0f;
  m.flags |= 0x20003;
  FUN_004c1480(frame, &m, 0);
  FUN_004c1c80(param_1, &vw);
  FUN_004c1c10(param_1, 2);
  FUN_00426670(param_1);
}
