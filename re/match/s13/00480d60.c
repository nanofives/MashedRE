// FLAGS: /O2 /GS /Ob1
/* MATCH-S13 0x00480d60 (h10): 72.8% (regs 66.8%, 628/650 ins). Moved it: literals for 0x005cf218..0x005cf22c
   (-6.5f, -2/9, 2/9, -1/9, 1/9, -1/1500) and 0x005cc568/320/32c (100, 1, 0.5); record pos at +0x30 copied
   field by field from a struct member (a whole-struct copy emits add eax,0x30); cases 5-7 re-read the
   POS() vector after p += dir (the original reloads it); case 8 if/else order and p.y += 0.5f after the
   copy; FUN_0046d4a0 writes the record pointer into param_1's own slot. Raw 12.5%.
   Residual (b): tail layout. The original holds two copies of the shared tail (p += dir; imp.x = imp.z = 0;
   FUN_0055b650), one at the end of case 6 (case 0 jumps in before the dir.z fstp) and one at the end of
   case 7 (case 5 jumps in before the dir.z copy store). Ours emits one tail after the switch. Tried: every
   case with its own tail + return (no cross-jumping, 685 ins, 68.0%), gotos into case 6 / case 7 labels
   (72.2%, the suffix then merges into case 8's call), case 8 first (67.4%), permuter (978 compiles; only
   a reorder of independent loads in case 6, kept). Register choice in the case-1/2 calls also differs. */
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
typedef struct V3 { float x, y, z; } V3;
typedef struct Rec { int pad[12]; V3 pos; } Rec;
MGLOBAL(V3, DAT_006146f0, 0x006146f0);
MGLOBALA(int, DAT_006c9a78, 4, 0x006c9a78);
MGLOBALA(int, DAT_008815ac, 1, 0x008815ac);
MGLOBALA(int, DAT_00881ec8, 1, 0x00881ec8);
MGLOBALA(float, DAT_00881f68, 1, 0x00881f68);
MGLOBALA(float, DAT_00881f6c, 1, 0x00881f6c);
MGLOBALA(float, DAT_00881f70, 1, 0x00881f70);
MGLOBALA(float, DAT_00881f74, 1, 0x00881f74);
MGLOBALA(float, DAT_00881f78, 1, 0x00881f78);
MGLOBALA(float, DAT_00881f7c, 1, 0x00881f7c);
void FUN_0046d4a0(int *p0, int p1);
int FUN_0047f230(V3 *p0, int p1);
float FUN_004c3ac0(V3 *p0);
void FUN_004c3df0(V3 *p0, V3 *p1, int p2, int *p3);
void FUN_0055b650(int *p0, V3 *p1, V3 *p2);
int FUN_0057c210(int p0);
#define POS(g) (*(V3 *)(*(int *)(*(int *)(*(g) + 0x10) + 8) + (g)[1] * 0x20))
// MATCH: 0x00480d60 FUN_00480d60
void FUN_00480d60(int param_1, int param_2)
{
  int idx;
  int *g;
  float d;
  V3 dir;
  V3 p;
  V3 imp;

  idx = param_1;
  FUN_0046d4a0(&param_1, idx);
  g = *(int **)(*(int *)(FUN_0057c210(DAT_006c9a78[idx]) + 0x10) + 4);
  if (g == 0) {
    return;
  }
  switch (param_2) {
  case 0:
    p.x = ((Rec *)param_1)->pos.x;
    p.y = ((Rec *)param_1)->pos.y;
    p.z = ((Rec *)param_1)->pos.z;
    dir = POS(g);
    dir.x *= (-1.0f / 1500.0f);
    dir.y *= (-1.0f / 1500.0f);
    imp.y = 350.0f;
    dir.z *= (-1.0f / 1500.0f);
    break;
  case 1:
    p.x = ((Rec *)param_1)->pos.x;
    p.y = ((Rec *)param_1)->pos.y;
    p.z = ((Rec *)param_1)->pos.z;
    dir = POS(g);
    dir.x *= (-1.0f / 1500.0f);
    dir.y *= (-1.0f / 1500.0f);
    dir.z *= (-1.0f / 1500.0f);
    imp.x = 0.0f;
    imp.y = 350.0f;
    imp.z = 0.0f;
    p.x += dir.x;
    p.y += dir.y;
    p.z += dir.z;
    FUN_0055b650(g, &imp, &p);
    FUN_004c3df0(&dir, &DAT_006146f0, 1, &DAT_00881ec8[idx * 0x341]);
    dir.x *= (1.0f / 9.0f);
    dir.y *= (1.0f / 9.0f);
    dir.z *= (1.0f / 9.0f);
    p.x += dir.x;
    p.y += dir.y;
    p.z += dir.z;
    imp.x = 0.0f;
    imp.y = 150.0f;
    imp.z = 0.0f;
    FUN_0055b650(g, &imp, &p);
    return;
  case 2:
    p.x = ((Rec *)param_1)->pos.x;
    p.y = ((Rec *)param_1)->pos.y;
    p.z = ((Rec *)param_1)->pos.z;
    dir = POS(g);
    dir.x *= (-1.0f / 1500.0f);
    dir.y *= (-1.0f / 1500.0f);
    dir.z *= (-1.0f / 1500.0f);
    imp.x = 0.0f;
    imp.y = 350.0f;
    imp.z = 0.0f;
    p.x += dir.x;
    p.y += dir.y;
    p.z += dir.z;
    FUN_0055b650(g, &imp, &p);
    FUN_004c3df0(&dir, &DAT_006146f0, 1, &DAT_00881ec8[idx * 0x341]);
    dir.x *= (-1.0f / 9.0f);
    dir.y *= (-1.0f / 9.0f);
    dir.z *= (-1.0f / 9.0f);
    p.x += dir.x;
    p.y += dir.y;
    p.z += dir.z;
    imp.x = 0.0f;
    imp.y = 150.0f;
    imp.z = 0.0f;
    FUN_0055b650(g, &imp, &p);
    return;
  case 3:
    p.x = ((Rec *)param_1)->pos.x;
    p.y = ((Rec *)param_1)->pos.y;
    p.z = ((Rec *)param_1)->pos.z;
    dir = POS(g);
    dir.x *= (-1.0f / 1500.0f);
    dir.y *= (-1.0f / 1500.0f);
    dir.z *= (-1.0f / 1500.0f);
    p.x += dir.x;
    p.y += dir.y;
    p.z += dir.z;
    FUN_004c3df0(&dir, &DAT_006146f0, 1, &DAT_00881ec8[idx * 0x341]);
    dir.x *= (1.0f / 9.0f);
    imp.x = 0.0f;
    imp.y = 180.0f;
    imp.z = 0.0f;
    dir.y *= (1.0f / 9.0f);
    dir.z *= (1.0f / 9.0f);
    p.x += dir.x;
    p.y += dir.y;
    p.z += dir.z;
    FUN_0055b650(g, &imp, &p);
    return;
  case 4:
    p.x = ((Rec *)param_1)->pos.x;
    p.y = ((Rec *)param_1)->pos.y;
    p.z = ((Rec *)param_1)->pos.z;
    dir = POS(g);
    dir.x *= (-1.0f / 1500.0f);
    dir.y *= (-1.0f / 1500.0f);
    dir.z *= (-1.0f / 1500.0f);
    p.x += dir.x;
    p.y += dir.y;
    p.z += dir.z;
    FUN_004c3df0(&dir, &DAT_006146f0, 1, &DAT_00881ec8[idx * 0x341]);
    dir.x *= (-1.0f / 9.0f);
    imp.x = 0.0f;
    imp.y = 180.0f;
    imp.z = 0.0f;
    dir.y *= (-1.0f / 9.0f);
    dir.z *= (-1.0f / 9.0f);
    p.x += dir.x;
    p.y += dir.y;
    p.z += dir.z;
    FUN_0055b650(g, &imp, &p);
    return;
  case 5:
    p.x = ((Rec *)param_1)->pos.x;
    p.y = ((Rec *)param_1)->pos.y;
    p.z = ((Rec *)param_1)->pos.z;
    dir = POS(g);
    dir.x *= (-1.0f / 1500.0f);
    dir.y *= (-1.0f / 1500.0f);
    dir.z *= (-1.0f / 1500.0f);
    p.x += dir.x;
    p.y += dir.y;
    p.z += dir.z;
    dir = POS(g);
    dir.x = DAT_00881f74[idx * 0x341] * (2.0f / 9.0f);
    imp.y = 425.0f;
    dir.y = DAT_00881f78[idx * 0x341] * (2.0f / 9.0f);
    dir.z = DAT_00881f7c[idx * 0x341] * (2.0f / 9.0f);
    break;
  case 6:
    dir = POS(g);
    p.x = ((Rec *)param_1)->pos.x;
    p.y = ((Rec *)param_1)->pos.y;
    p.z = ((Rec *)param_1)->pos.z;
    dir.x *= (-1.0f / 1500.0f);
    dir.y *= (-1.0f / 1500.0f);
    dir.z *= (-1.0f / 1500.0f);
    p.x += dir.x;
    p.y += dir.y;
    p.z += dir.z;
    dir = POS(g);
    dir.x = DAT_00881f74[idx * 0x341] * (-2.0f / 9.0f);
    imp.y = 425.0f;
    dir.y = DAT_00881f78[idx * 0x341] * (-2.0f / 9.0f);
    dir.z = DAT_00881f7c[idx * 0x341] * (-2.0f / 9.0f);
    break;
  case 7:
    p.x = ((Rec *)param_1)->pos.x;
    p.y = ((Rec *)param_1)->pos.y;
    p.z = ((Rec *)param_1)->pos.z;
    dir = POS(g);
    dir.x *= (-1.0f / 1500.0f);
    dir.y *= (-1.0f / 1500.0f);
    dir.z *= (-1.0f / 1500.0f);
    p.x += dir.x;
    p.y += dir.y;
    p.z += dir.z;
    dir = POS(g);
    dir.x = DAT_00881f74[idx * 0x341] * (-2.0f / 9.0f);
    imp.y = 125.0f;
    dir.y = DAT_00881f78[idx * 0x341] * (-2.0f / 9.0f);
    dir.z = DAT_00881f7c[idx * 0x341] * (-2.0f / 9.0f);
    break;
  case 8:
    if (FUN_0047f230(&dir, idx) != 0) {
      d = FUN_004c3ac0(&dir);
    } else {
      d = 100.0f;
    }
    if (DAT_008815ac[idx * 0x341] > 6000 || d < 1.0f) {
      p.x = ((Rec *)param_1)->pos.x;
      p.y = ((Rec *)param_1)->pos.y;
      p.z = ((Rec *)param_1)->pos.z;
      p.y += 0.5f;
      imp.x = DAT_00881f68[idx * 0x341] * -6.5f;
      imp.y = DAT_00881f6c[idx * 0x341] * -6.5f;
      imp.z = DAT_00881f70[idx * 0x341] * -6.5f;
      FUN_0055b650(g, &imp, &p);
    }
    return;
  default:
    return;
  }
  p.x += dir.x;
  p.y += dir.y;
  imp.x = 0.0f;
  imp.z = 0.0f;
  p.z += dir.z;
  FUN_0055b650(g, &imp, &p);
}
