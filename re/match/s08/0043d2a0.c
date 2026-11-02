// FLAGS: /O2 /GS /Ob1
/* MATCH-S08 0x0043d2a0: 54.0% (regs 64.7%), raw g2s did not compile (C2708). By hand. The file holds the register-arg static callees FUN_0042ac00 (edx; 76.2% by Ghidra size, but our 21 instructions equal the original listing through 0x0042ac43, Ghidra size stops before the loop tail), FUN_0042ac50 (eax,ecx; 100%) and FUN_0042ad90 (ebx,edx,edi; 100%): defining them static before the caller gives the original ebx/edx/edi arg registers. Menu-level stack 0x0067ed78 (0x40-byte MenuLevel) and the 0x34-byte MenuItem array at 0x00898ac0 are typed. Tried: switch/if order of the mode branches (mode!=0 first matches the block order, +4%), item index form instead of a pointer (38%, worse: the original strength-reduces to a pointer biased to item+0x18). Residual (b)+(c): FUN_00432b30 (1327 B) takes mode in eax and nItems in esi plus two stack args, a register convention that needs its body in this file (not reconstructed in budget; declared cdecl here), and the register allocation of the loop (s1/s2 in ebp/ebx, zero in ebx) follows from it. */
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
typedef struct {
  int *def;
  int id;
  int f8;
  int pad[12];
  int count;
} MenuLevel;
typedef struct {
  int type;
  int f4;
  int f8;
  uchar rgba[4];
  int f10;
  float scale;
  float x;
  float y;
  int idx;
  int f24;
  int s1;
  int s2;
  int *def;
} MenuItem;
MGLOBAL(int, DAT_007f0eec, 0x007f0eec);
MGLOBAL(int, DAT_007f0ef0, 0x007f0ef0);
MGLOBAL(int, DAT_007f0ef4, 0x007f0ef4);
MGLOBAL(int, DAT_007f0ef8, 0x007f0ef8);
MGLOBAL(int, DAT_007f0f00, 0x007f0f00);
MGLOBAL(int, DAT_007f0f04, 0x007f0f04);
MGLOBAL(int, DAT_007f0f08, 0x007f0f08);
MGLOBAL(int, DAT_007f0f0c, 0x007f0f0c);
MGLOBAL(int, DAT_007f0f10, 0x007f0f10);
MGLOBAL(int, DAT_007f0f14, 0x007f0f14);
MGLOBAL(int, DAT_007f0f18, 0x007f0f18);
MGLOBAL(int, DAT_007f0f1c, 0x007f0f1c);
MGLOBAL(int, DAT_007f0f20, 0x007f0f20);
MGLOBAL(int, DAT_007f0f24, 0x007f0f24);
MGLOBAL(int, DAT_007f0f30, 0x007f0f30);
MGLOBAL(int, DAT_007f0f34, 0x007f0f34);
MGLOBAL(int, DAT_007f0f38, 0x007f0f38);
MGLOBAL(int, DAT_007f0f3c, 0x007f0f3c);
MGLOBAL(int, DAT_007f0f44, 0x007f0f44);
MGLOBAL(int, DAT_007f0f48, 0x007f0f48);
MGLOBAL(int, DAT_007f0f4c, 0x007f0f4c);
MGLOBAL(int, DAT_007f0f50, 0x007f0f50);
MGLOBAL(int, DAT_008990d8, 0x008990d8);
MGLOBALA(int, DAT_007f105c, 0x1d0, 0x007f105c);
MGLOBALA(int, DAT_00899160, 16, 0x00899160);
MGLOBAL(int, DAT_0067ea70, 0x0067ea70);
MGLOBAL(int, DAT_0067e9f8, 0x0067e9f8);
MGLOBALA(MenuLevel, DAT_0067ed78, 16, 0x0067ed78);
MGLOBALA(int *, PTR_DAT_005f7638, 64, 0x005f7638);
MGLOBALA(MenuItem, DAT_00898ac0, 32, 0x00898ac0);
MGLOBALA(int, DAT_005f72a0, 52, 0x005f72a0);
MGLOBALA(int, DAT_005f7370, 52, 0x005f7370);
MGLOBAL(int, DAT_0067ece0, 0x0067ece0);
MGLOBAL(int, DAT_0067e844, 0x0067e844);
int FUN_0040ad20(void);
void FUN_0040b810(void);
void FUN_00414120(void);
void FUN_00422b30(void);
void FUN_0042d3e0(void);
void FUN_00431d90(void);
void FUN_00431f30(int menu);
void FUN_00432800(int level);
void FUN_00472640(int a);

// MATCH: 0x0042ac00 FUN_0042ac00
static int FUN_0042ac00(int *p)
{
  int n = 0;
  if (p == 0) {
    return 0;
  }
  for (;;) {
    while (*p != (int)0xff060000) {
      if (*p == (int)0xff070000) {
        return n;
      }
      p++;
    }
    if (*p == (int)0xff070000) {
      return n;
    }
    n++;
    p++;
  }
}

// MATCH: 0x0042ac50 FUN_0042ac50
static int FUN_0042ac50(unsigned int n, int spacing)
{
  int y;
  if ((n & 1) == 0) {
    y = 0xf0 - spacing / 2;
  } else {
    y = 0xf0;
  }
  return y - ((n - 1) >> 1) * spacing;
}

// MATCH: 0x0042ad90 FUN_0042ad90
static int FUN_0042ad90(int key, int *p, int idx)
{
  int i;
  int k;
  if (p == 0) {
    return -1;
  }
  k = 0;
  for (i = 0; p[i] != (int)0xff070000; i++) {
    if (p[i] == key) {
      if (idx == k) {
        return p[i + 1];
      }
      k++;
    }
  }
  return -1;
}
void FUN_00432b30(int mode, int *pn, int s, int menu);

// MATCH: 0x0043d2a0 FUN_0043d2a0
void FUN_0043d2a0(int menu, int mode)
{
  int *newDef;
  int *oldDef;
  int lv;
  int sel;
  int nItems;
  int i;
  int y30;
  int y28;
  int nNew;
  int nOld;
  short s1;
  short s2;
  float base;
  MenuItem *it;
  int *src;
  int *dst;

  switch (mode) {
  case 0:
    FUN_00431f30(menu);
    break;
  case 1:
    if (DAT_0067e9f8 - 2 >= 0) {
      FUN_00431f30(DAT_0067ed78[DAT_0067e9f8 - 2].id);
    }
    break;
  case 2:
    FUN_00431d90();
    break;
  }
  FUN_00472640(0xff);
  if (menu == 0x1a) {
    DAT_007f0ef4 = DAT_007f0eec;
    DAT_007f0ef8 = DAT_007f0ef0;
  } else if (menu == 0x13) {
    DAT_007f0f14 = DAT_007f0f00;
    DAT_007f0f24 = DAT_007f0f10;
    DAT_007f0f20 = DAT_007f0f0c;
    DAT_007f0f1c = DAT_007f0f08;
    DAT_007f0f18 = DAT_007f0f04;
  } else if (menu == 0x20) {
    DAT_008990d8 = FUN_0040ad20();
  } else if (menu == 0x1c) {
    dst = DAT_00899160;
    src = DAT_007f105c;
    do {
      *dst = *src;
      src += 0x13;
      dst++;
    } while (src < (int *)0x7f13ec);
  } else {
    if (menu == 0x1f || menu == 0x21) {
      DAT_007f0f44 = DAT_007f0f30;
      DAT_007f0f48 = DAT_007f0f34;
      DAT_007f0f4c = DAT_007f0f38;
      DAT_007f0f50 = DAT_007f0f3c;
    }
    if (menu == 1) {
      FUN_00414120();
      FUN_00422b30();
      FUN_0040b810();
      DAT_0067ea70 = 1;
    }
  }
  FUN_0042d3e0();
  sel = 0;
  nItems = 0;
  if (mode != 0) {
    if (mode == 2) {
      lv = DAT_0067e9f8;
      newDef = 0;
      oldDef = DAT_0067ed78[lv - 1].def;
    } else {
      DAT_0067e9f8--;
      lv = DAT_0067e9f8;
      if (lv <= 0) {
        return;
      }
      newDef = DAT_0067ed78[lv - 1].def;
      oldDef = DAT_0067ed78[lv].def;
    }
  } else {
    lv = DAT_0067e9f8;
    DAT_0067ed78[lv].def = PTR_DAT_005f7638[menu];
    DAT_0067ed78[lv].id = menu;
    DAT_0067ed78[lv].f8 = 0;
    FUN_00432800(lv);
    lv = DAT_0067e9f8;
    newDef = DAT_0067ed78[lv].def;
    if (lv != 0) {
      oldDef = DAT_0067ed78[lv - 1].def;
    } else {
      oldDef = 0;
    }
  }
  s1 = (short)FUN_0042ad90(0xff000000, newDef, 0);
  s2 = (short)FUN_0042ad90(0xff000000, oldDef, 0);
  if (s1 != -1 || s2 != -1) {
    DAT_00898ac0[0].rgba[0] = 0xff;
    DAT_00898ac0[0].rgba[1] = 0xff;
    DAT_00898ac0[0].rgba[2] = 0xff;
    DAT_00898ac0[0].type = 0xff000000;
    DAT_00898ac0[0].x = 64.0f;
    DAT_00898ac0[0].y = 48.0f;
    DAT_00898ac0[0].scale = 0.6f;
    DAT_00898ac0[0].f24 = 0;
    DAT_00898ac0[0].s1 = s1;
    DAT_00898ac0[0].s2 = s2;
    if (s2 == -1) {
      DAT_00898ac0[0].f4 = 1;
      DAT_00898ac0[0].f10 = 0;
      nItems = 1;
    } else {
      DAT_00898ac0[0].f10 = 0x1ff;
      DAT_00898ac0[0].f4 = 2;
      if (s1 != -1) {
        DAT_00898ac0[0].f4 = 0;
      }
      nItems = 1;
    }
  }
  if (mode != 0) {
    nOld = FUN_0042ac00(oldDef);
    nNew = FUN_0042ac00(newDef);
  } else {
    nNew = FUN_0042ac00(newDef);
    nOld = FUN_0042ac00(oldDef);
  }
  if (DAT_0067e9f8 == 0) {
    if (DAT_0067ed78[0].def == DAT_005f7370) {
      base = (float)FUN_0042ac50(nNew, 0x1c);
    } else {
      base = (float)FUN_0042ac50(nNew, 0x1e);
    }
  } else if (DAT_0067ed78[lv].def == DAT_005f7370) {
    base = (float)FUN_0042ac50(nOld, 0x1c);
  } else {
    base = (float)FUN_0042ac50(nOld, 0x1e);
  }
  i = 0;
  y28 = 0;
  y30 = 0;
  it = &DAT_00898ac0[nItems];
  for (;;) {
    s1 = (short)FUN_0042ad90(0xff040000, newDef, i);
    s2 = (short)FUN_0042ad90(0xff040000, oldDef, i);
    if (s1 == -1) {
      if (s2 == -1) {
        break;
      }
    } else {
      sel++;
    }
    it->def = DAT_0067ed78[lv].def;
    it->type = 0xff040000;
    it->x = 64.0f;
    it->rgba[0] = 0xff;
    it->rgba[1] = 0xff;
    it->rgba[2] = 0xff;
    it->rgba[3] = 0xff;
    it->scale = 0.8f;
    if ((it->def == DAT_005f72a0 || it->def == DAT_005f7370 ||
         (DAT_0067ed78[lv - 1].def == DAT_005f72a0 && mode == 2)) && y30 != 0) {
      it->scale = 0.58f;
    }
    it->f24 = 0;
    it->f4 = 0;
    it->y = (float)y30 + base;
    if (it->def == DAT_005f7370) {
      it->y = (float)y28 + base - 58.0f;
    }
    if (it->def == DAT_005f72a0) {
      it->y = it->y - 58.0f;
    }
    it->idx = i;
    it->s1 = s1;
    it->s2 = s2;
    it->f10 = 0x1ff;
    if (DAT_0067e9f8 == 0) {
      it->f4 = 1;
      it->f10 = 0;
    }
    nItems++;
    it++;
    y30 += 0x1e;
    i++;
    y28 += 0x1c;
  }
  if (mode == 0) {
    DAT_0067ed78[lv].count = sel;
  } else {
    sel = DAT_0067ed78[lv - 1].count;
  }
  DAT_0067ece0 = sel;
  FUN_00432b30(mode, &nItems, (short)FUN_0042ad90(0xff080000, newDef, 0), menu);
  if (mode == 0) {
    DAT_0067e9f8++;
  }
  DAT_0067e844 = mode;
}
