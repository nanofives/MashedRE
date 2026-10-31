// FLAGS: /O2 /GS /Ob1
/* MATCH-S04 h4 0x0041c090 (46 B): 100.0% exact (regs 100, ops 100), by hand. Raw converter: 91.7%.
   The callee 0x0041b7a0 takes the record in ESI (no prolog save; the caller's loop register). That convention comes
   out of plain 9176 once 0x0041b7a0 is `static`, defined first, with the record as its parameter (pilot finding 1).
   This file holds the callee 0x0041b7a0 (1037 B) with the Ghidra decompilation as body (unaff_ESI -> parameter,
   stack locals regrouped into the RwMatrix-shaped local they are: right/flags/up/at/pos, flags OR'd with 0x20003
   uninitialised as in the original). The callee itself is NOT matched: 30.8%, class (b), not in h4's list, and its
   relocheck is BAD only because its code does not line up yet. The caller does not depend on its body beyond the
   ESI convention. Residual (caller): none. */
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
MGLOBALA(int, DAT_0063cd90, 4, 0x0063cd90);
MGLOBALA(char, DAT_0063cab8, 0x2d8, 0x0063cab8);
MGLOBAL(int, DAT_0063cda0, 0x0063cda0);
void FUN_0040b930(int *p);
int FUN_0040b6d0(int p0);
int FUN_0040b890(void);
typedef struct RwMatrix0041c090 {
  float right[3]; unsigned int flags;
  float up[3];    unsigned int pad1;
  float at[3];    unsigned int pad2;
  float pos[3];   unsigned int pad3;
} RwMatrix0041c090;
int FUN_004c13e0(int frame, float *scale, int combine);
int FUN_004c1480(int frame, RwMatrix0041c090 *m, int combine);
void FUN_004c51a0(RwMatrix0041c090 *m, float *v, int combine);

// MATCH: 0x0041b7a0 FUN_0041b7a0
static void FUN_0041b7a0(int *o)
{
  uint uVar1;
  uint uVar2;
  int iVar3;
  int iVar4;
  int bVar5;
  float scale[3];
  RwMatrix0041c090 m;
  
  scale[0] = 1.0f;
  scale[1] = 1.0f;
  scale[2] = 1.0f;
  if (DAT_0063cd90[*o] == 0) {
    uVar2 = o[0x5a] & 0xfffbffff;
  }
  else {
    uVar2 = o[0x5a] | 0x40000;
  }
  o[0x5a] = uVar2;
  iVar4 = 0;
  do {
    if (o[iVar4 + 0x2c] == 0) goto LAB_0041b998;
    switch(iVar4) {
    case 0:
    case 1:
    case 2:
    case 3:
      goto switchD_0041b808_caseD_0;
    case 4:
      uVar2 = o[0x5a] & 0x40000;
      goto LAB_0041b994;
    case 5:
      uVar2 = o[0x5a];
      uVar1 = uVar2 & 0x3000000;
      goto LAB_0041b82a;
    case 6:
      if ((o[0x5a] & 0x3000000U) == 0) break;
      bVar5 = (o[0x5a] & 2U) == 0;
      goto LAB_0041b832;
    case 7:
      if ((o[0x5a] & 0x3000000U) != 0) {
        bVar5 = (o[0x5a] & 4U) == 0;
        goto LAB_0041b832;
      }
      break;
    case 8:
      if ((o[0x5a] & 0x3000000U) != 0) {
        bVar5 = (o[0x5a] & 8U) == 0;
        goto LAB_0041b832;
      }
      break;
    case 9:
      if ((o[0x5a] & 0x3000000U) != 0) {
        bVar5 = (o[0x5a] & 0x10U) == 0;
        goto LAB_0041b832;
      }
      break;
    case 10:
      if ((o[0x5a] & 0x3000000U) != 0) {
        bVar5 = (o[0x5a] & 0x20U) == 0;
        goto LAB_0041b832;
      }
      break;
    case 0xb:
      if ((o[0x5a] & 0x3000000U) != 0) {
        bVar5 = (o[0x5a] & 0x40U) == 0;
        goto LAB_0041b832;
      }
      break;
    case 0xc:
      if (((o[0x5a] & 0x3000000U) == 0) || (-1 < (char)o[0x5a])) break;
      goto switchD_0041b808_caseD_0;
    case 0xd:
      if ((o[0x5a] & 0x3000000U) != 0) {
        bVar5 = (o[0x5a] & 0x100U) == 0;
        goto LAB_0041b832;
      }
      break;
    case 0xe:
      if ((o[0x5a] & 0x3000000U) != 0) {
        bVar5 = (o[0x5a] & 0x200U) == 0;
        goto LAB_0041b832;
      }
      break;
    case 0xf:
      if ((o[0x5a] & 0x3000000U) != 0) {
        bVar5 = (o[0x5a] & 0x400U) == 0;
        goto LAB_0041b832;
      }
      break;
    case 0x10:
      if ((o[0x5a] & 0x3000000U) != 0) {
        bVar5 = (o[0x5a] & 0x800U) == 0;
        goto LAB_0041b832;
      }
      break;
    case 0x16:
      if ((o[0x5a] & 0x2000000U) != 0) {
        bVar5 = (o[0x5a] & 0x1000U) == 0;
        goto LAB_0041b832;
      }
      break;
    case 0x17:
      if ((o[0x5a] & 0x2000000U) != 0) {
        bVar5 = (o[0x5a] & 0x2000U) == 0;
        goto LAB_0041b832;
      }
      break;
    case 0x18:
      if ((o[0x5a] & 0x2000000U) != 0) {
        bVar5 = (o[0x5a] & 0x4000U) == 0;
        goto LAB_0041b832;
      }
      break;
    case 0x19:
      if (((o[0x5a] & 0x2000000U) == 0) || (-1 < (char)((uint)o[0x5a] >> 8))) break;
      uVar2 = 1;
      goto LAB_0041b994;
    case 0x1a:
      if ((o[0x5a] & 0x2000000U) != 0) {
        bVar5 = (o[0x5a] & 0x10000U) == 0;
        goto LAB_0041b832;
      }
      break;
    case 0x1b:
      if ((o[0x5a] & 0x2000000U) != 0) {
        bVar5 = (o[0x5a] & 0x20000U) == 0;
        goto LAB_0041b832;
      }
      break;
    case 0x1c:
      uVar2 = o[0x5a];
      uVar1 = uVar2 & 0x800000;
LAB_0041b82a:
      if (uVar1 != 0) {
        bVar5 = (uVar2 & 1) == 0;
LAB_0041b832:
        if (!bVar5) goto switchD_0041b808_caseD_0;
      }
      break;
    case 0x1d:
      if (((o[0x5a] & 0x800000U) == 0) || ((o[0x5a] & 2U) == 0)) break;
      uVar2 = 1;
      goto LAB_0041b994;
    case 0x1e:
      if (((o[0x5a] & 0x800000U) == 0) || ((o[0x5a] & 4U) == 0)) break;
      uVar2 = 1;
      goto LAB_0041b994;
    case 0x1f:
      if (((o[0x5a] & 0x800000U) == 0) || ((o[0x5a] & 8U) == 0)) break;
      uVar2 = 1;
      goto LAB_0041b994;
    case 0x20:
      if (((o[0x5a] & 0x800000U) == 0) || ((o[0x5a] & 0x10U) == 0)) break;
      uVar2 = 1;
      goto LAB_0041b994;
    case 0x21:
      if (((o[0x5a] & 0x800000U) == 0) || ((o[0x5a] & 0x20U) == 0)) break;
      uVar2 = 1;
      goto LAB_0041b994;
    }
    uVar2 = 0;
LAB_0041b994:
    o[iVar4 + 4] = uVar2;
LAB_0041b998:
    iVar4 = iVar4 + 1;
    if (0x27 < iVar4) {
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
      FUN_004c51a0(&m,(float *)(o + 0x54),0);
      FUN_004c1480(o[0x58],&m,0);
      FUN_004c13e0(o[0x58],scale,1);
      iVar4 = FUN_0040b6d0(*o);
      iVar3 = FUN_0040b890();
      scale[0] = (float)iVar4 / (float)iVar3;
      scale[2] = 1.0f;
      scale[1] = 1.0f;
      FUN_004c13e0(*(int *)(o[0x2e] + 4),scale,0);
      return;
    }
  } while (1);
switchD_0041b808_caseD_0:
  uVar2 = 1;
  goto LAB_0041b994;
}

// MATCH: 0x0041c090 FUN_0041c090
void FUN_0041c090(void)
{
  int i;
  FUN_0040b930(DAT_0063cd90);
  for (i = 0; i < 2; i++)
    FUN_0041b7a0((int *)(DAT_0063cab8 + i * 0x16c));
  DAT_0063cda0 = DAT_0063cda0 + 1;
}
