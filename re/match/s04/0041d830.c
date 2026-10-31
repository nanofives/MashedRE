// FLAGS: /O2 /GS /Ob1
/* MATCH-S04 h4 0x0041d830 (59 B): 100.0% exact (regs 100, ops 100), by hand. Raw converter: 93.3%.
   Twin of 0x0041c090: callee 0x0041ce00 takes the record in ESI; `static`, defined first, record as parameter.
   The `lea ecx,[ecx]` before the loop is the compiler's loop alignment and comes out by itself.
   This file holds the callee 0x0041ce00 (1388 B) with the Ghidra decompilation as body (unaff_ESI -> parameter,
   RwMatrix-shaped local, the scale triple, DAT_0063d558 as unsigned (the 2^32 fix-up at 0x0041d319), sin as the
   intrinsic for the fsin at 0x0041d325; Ghidra's two extra args on the last FUN_004c13e0 call dropped).
   The callee itself is NOT matched: 38.5%, class (b), not in h4's list; its relocheck is BAD only because its code
   does not line up yet. Residual (caller): none. */
/* ---- MATCH-S04 preamble (same text in every re/match/s04 source; not part of any function body) ----
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
typedef struct RwMatrix0041d830 {
  float right[3]; unsigned int flags;
  float up[3];    unsigned int pad1;
  float at[3];    unsigned int pad2;
  float pos[3];   unsigned int pad3;
} RwMatrix0041d830;
typedef float Vec3_0041d830[3];
MGLOBALA(int, DAT_0063d278, 4, 0x0063d278);
MGLOBAL(int, DAT_0063d288, 0x0063d288);
MGLOBALA(char, DAT_0063d298, 0x2c0, 0x0063d298);
MGLOBAL(unsigned int, DAT_0063d558, 0x0063d558);
MGLOBALA(Vec3_0041d830, DAT_005f33c4, 2, 0x005f33c4);
MGLOBAL(float, DAT_005cd118, 0x005cd118);
MGLOBAL(float, DAT_005cc9c0, 0x005cc9c0);
MGLOBAL(float, DAT_005cc8f0, 0x005cc8f0);
MGLOBAL(float, DAT_005cc320, 0x005cc320);
double sin(double);
#pragma intrinsic(sin)
void FUN_0040b930(int *p);
void FUN_0040b540(int *p);
int FUN_0040b420(int p0);
int FUN_0040b6d0(int p0);
int FUN_0040b890(void);
int FUN_004c13e0(int frame, float *scale, int combine);
int FUN_004c1480(int frame, RwMatrix0041d830 *m, int combine);
void FUN_004c51a0(RwMatrix0041d830 *m, float *v, int combine);

// MATCH: 0x0041ce00 FUN_0041ce00
static void FUN_0041ce00(int *o)
{
  uint uVar1;
  undefined4 uVar2;
  uint uVar3;
  int iVar4;
  int iVar5;
  int bVar6;
  float fVar8;
  float scale[3];
  RwMatrix0041d830 m;
  
  scale[0] = 1.0f;
  scale[1] = 1.0f;
  scale[2] = 1.0f;
  if (DAT_0063d278[*o] == 0) {
    uVar3 = o[0x57] & 0xfffbffff;
  }
  else {
    uVar3 = o[0x57] | 0x40000;
  }
  o[0x57] = uVar3;
  uVar2 = FUN_0040b420(*o);
  uVar3 = o[0x57] & 0x83ffffff;
  o[0x57] = uVar3;
  switch(uVar2) {
  case 0:
    uVar3 = uVar3 | 0x10000000;
    break;
  case 1:
    uVar3 = uVar3 | 0x8000000;
    break;
  case 2:
    uVar3 = uVar3 | 0x4000000;
    break;
  case 0xfffffffe:
    uVar3 = uVar3 | 0x40000000;
    break;
  case 0xffffffff:
    uVar3 = uVar3 | 0x20000000;
    break;
  default:
    goto switchD_0041ce65_default;
  }
  o[0x57] = uVar3;
switchD_0041ce65_default:
  iVar4 = 0;
  do {
    if (o[iVar4 + 0x2c] == 0) goto LAB_0041d048;
    switch(iVar4) {
    case 0:
    case 1:
    case 2:
    case 3:
      goto switchD_0041ceb8_caseD_0;
    case 4:
      uVar3 = o[0x57] & 0x40000;
      goto LAB_0041d044;
    case 5:
      uVar3 = o[0x57];
      uVar1 = uVar3 & 0x3000000;
      goto LAB_0041ceda;
    case 6:
      if ((o[0x57] & 0x3000000U) == 0) break;
      bVar6 = (o[0x57] & 2U) == 0;
      goto LAB_0041cee2;
    case 7:
      if ((o[0x57] & 0x3000000U) != 0) {
        bVar6 = (o[0x57] & 4U) == 0;
        goto LAB_0041cee2;
      }
      break;
    case 8:
      if ((o[0x57] & 0x3000000U) != 0) {
        bVar6 = (o[0x57] & 8U) == 0;
        goto LAB_0041cee2;
      }
      break;
    case 9:
      if ((o[0x57] & 0x3000000U) != 0) {
        bVar6 = (o[0x57] & 0x10U) == 0;
        goto LAB_0041cee2;
      }
      break;
    case 10:
      if ((o[0x57] & 0x3000000U) != 0) {
        bVar6 = (o[0x57] & 0x20U) == 0;
        goto LAB_0041cee2;
      }
      break;
    case 0xb:
      if ((o[0x57] & 0x3000000U) != 0) {
        bVar6 = (o[0x57] & 0x40U) == 0;
        goto LAB_0041cee2;
      }
      break;
    case 0xc:
      if (((o[0x57] & 0x3000000U) == 0) || (-1 < (char)o[0x57])) break;
      goto switchD_0041ceb8_caseD_0;
    case 0xd:
      if ((o[0x57] & 0x3000000U) != 0) {
        bVar6 = (o[0x57] & 0x100U) == 0;
        goto LAB_0041cee2;
      }
      break;
    case 0xe:
      if ((o[0x57] & 0x3000000U) != 0) {
        bVar6 = (o[0x57] & 0x200U) == 0;
        goto LAB_0041cee2;
      }
      break;
    case 0xf:
      if ((o[0x57] & 0x3000000U) != 0) {
        bVar6 = (o[0x57] & 0x400U) == 0;
        goto LAB_0041cee2;
      }
      break;
    case 0x10:
      if ((o[0x57] & 0x3000000U) != 0) {
        bVar6 = (o[0x57] & 0x800U) == 0;
        goto LAB_0041cee2;
      }
      break;
    case 0x11:
      uVar3 = o[0x57] & 0x4000000;
      goto LAB_0041d044;
    case 0x12:
      uVar3 = o[0x57] & 0x8000000;
      goto LAB_0041d044;
    case 0x13:
      uVar3 = o[0x57] & 0x10000000;
      goto LAB_0041d044;
    case 0x14:
      uVar3 = o[0x57] & 0x20000000;
      goto LAB_0041d044;
    case 0x15:
      uVar3 = o[0x57] & 0x40000000;
      goto LAB_0041d044;
    case 0x16:
      if ((o[0x57] & 0x2000000U) != 0) {
        bVar6 = (o[0x57] & 0x1000U) == 0;
        goto LAB_0041cee2;
      }
      break;
    case 0x17:
      if ((o[0x57] & 0x2000000U) != 0) {
        bVar6 = (o[0x57] & 0x2000U) == 0;
        goto LAB_0041cee2;
      }
      break;
    case 0x18:
      if ((o[0x57] & 0x2000000U) != 0) {
        bVar6 = (o[0x57] & 0x4000U) == 0;
        goto LAB_0041cee2;
      }
      break;
    case 0x19:
      if (((o[0x57] & 0x2000000U) == 0) || (-1 < (char)((uint)o[0x57] >> 8))) break;
      uVar3 = 1;
      goto LAB_0041d044;
    case 0x1a:
      if ((o[0x57] & 0x2000000U) != 0) {
        bVar6 = (o[0x57] & 0x10000U) == 0;
        goto LAB_0041cee2;
      }
      break;
    case 0x1b:
      if ((o[0x57] & 0x2000000U) != 0) {
        bVar6 = (o[0x57] & 0x20000U) == 0;
        goto LAB_0041cee2;
      }
      break;
    case 0x1c:
      uVar3 = o[0x57];
      uVar1 = uVar3 & 0x800000;
LAB_0041ceda:
      if (uVar1 != 0) {
        bVar6 = (uVar3 & 1) == 0;
LAB_0041cee2:
        if (!bVar6) goto switchD_0041ceb8_caseD_0;
      }
      break;
    case 0x1d:
      if (((o[0x57] & 0x800000U) == 0) || ((o[0x57] & 2U) == 0)) break;
      uVar3 = 1;
      goto LAB_0041d044;
    case 0x1e:
      if (((o[0x57] & 0x800000U) == 0) || ((o[0x57] & 4U) == 0)) break;
      uVar3 = 1;
      goto LAB_0041d044;
    case 0x1f:
      if (((o[0x57] & 0x800000U) == 0) || ((o[0x57] & 8U) == 0)) break;
      uVar3 = 1;
      goto LAB_0041d044;
    case 0x20:
      if (((o[0x57] & 0x800000U) == 0) || ((o[0x57] & 0x10U) == 0)) break;
      uVar3 = 1;
      goto LAB_0041d044;
    case 0x21:
      if (((o[0x57] & 0x800000U) == 0) || ((o[0x57] & 0x20U) == 0)) break;
      uVar3 = 1;
      goto LAB_0041d044;
    }
    uVar3 = 0;
LAB_0041d044:
    o[iVar4 + 4] = uVar3;
LAB_0041d048:
    iVar4 = iVar4 + 1;
  } while (iVar4 < 0x28);
  iVar4 = 0;
  iVar5 = 1;
  if (0 < o[3]) {
    do {
      if (o[iVar4] == DAT_0063d288) {
        iVar5 = 0;
        break;
      }
      iVar4 = iVar4 + 1;
    } while (iVar4 < o[3]);
  }
  m.flags = m.flags | 0x20003;
  m.at[2] = 1.0f;
  m.up[1] = 1.0f;
  m.right[0] = 1.0f;
  m.up[0] = 0.0f;
  m.right[2] = 0.0f;
  m.right[1] = 0.0f;
  m.at[1] = 0.0f;
  m.at[0] = 0.0f;
  m.up[2] = 0.0f;
  m.pos[2] = 0.0f;
  m.pos[1] = 0.0f;
  m.pos[0] = 0.0f;
  FUN_004c51a0(&m,DAT_005f33c4[iVar5],0);
  FUN_004c1480(o[0x55],&m,0);
  scale[0] = scale[0] * DAT_005cd118;
  scale[1] = scale[1] * DAT_005cd118;
  scale[2] = scale[2] * DAT_005cd118;
  FUN_004c13e0(o[0x55],scale,1);
  iVar4 = FUN_0040b6d0(*o);
  fVar8 = (float)iVar4;
  iVar4 = FUN_0040b890();
  scale[0] = fVar8 / (float)iVar4;
  scale[2] = 1.0f;
  scale[1] = 1.0f;
  FUN_004c13e0(*(int *)(o[0x2e] + 4),scale,0);
  if ((o[0x57] & 0x40000U) == 0) {
    scale[1] = 1.0f;
    scale[0] = 1.0f;
  }
  else {
    fVar8 = (float)sin((float)DAT_0063d558 * DAT_005cc9c0) * DAT_005cc8f0 + DAT_005cc320;
    scale[1] = fVar8;
    scale[0] = fVar8;
  }
  scale[2] = 1.0f;
  FUN_004c13e0(*(int *)(o[0x30] + 4),scale,0);
  return;
switchD_0041ceb8_caseD_0:
  uVar3 = 1;
  goto LAB_0041d044;
}

// MATCH: 0x0041d830 FUN_0041d830
void FUN_0041d830(void)
{
  int i;
  FUN_0040b930(DAT_0063d278);
  FUN_0040b540(&DAT_0063d288);
  for (i = 0; i < 2; i++)
    FUN_0041ce00((int *)(DAT_0063d298 + i * 0x160));
  DAT_0063d558 = DAT_0063d558 + 1;
}
