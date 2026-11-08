// FLAGS: /O2 /Ob1 /G5
/* MATCH-S26 h1 0x005afa00 (751 B). Best 74.7% (regs 76.8%), residual class (b). was a compile error; rewritten as switch(kind){1,2,3,default} over a DL{prev,next,data} list local, FUN_005a9ff0(0,FUN_005ad6a0(g),&head), unsigned counts; orig keeps separate inline pop/pop/pop/pop/add esp/ret copies for early returns (35 more ins) where cl merges them; same early-ret layout class as 0x005addd0/0x005ad910. */
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
typedef struct DL { struct DL *prev; struct DL *next; int data; } DL;
MGLOBAL(unsigned char, DAT_005e6cf0, 0x005e6cf0);
MGLOBAL(unsigned char, DAT_005e6d50, 0x005e6d50);
void FUN_004522d0(int p0);
int FUN_005a9e40(int p0, int p1);
int FUN_005a9ff0(int p0, int p1, DL *p2);
int FUN_005aa060(int p0, unsigned char *p1, int *p2);
int FUN_005aa560(int p0, int p1, int p2, int p3, int p4, int p5);
int FUN_005ac9e0(int p0, int p1);
int FUN_005ad6a0(unsigned char *p0);
void FUN_005ade90(DL *p0);
int FUN_005ae170(int p0, int p1, int p2);
void *FUN_005aea00(unsigned int p0, int p1);
int FUN_005b73b0(int p0);
MGLOBAL(int, DAT_007d3ff8, 0x007d3ff8);
// MATCH: 0x005afa00 FUN_005afa00
int FUN_005afa00(int param_1,int *param_2)
{
  int found = 0;
  int local_10;
  int *piVar1 = param_2;
  int *w;
  int iVar9;
  int r;
  DL head;
  DL *n;
  unsigned int *arr;
  unsigned int i;
  int data;

  switch (param_2[1]) {
  case 1:
    head.prev = &head;
    head.next = &head;
    head.data = 0;
    iVar9 = FUN_005a9ff0(0, FUN_005ad6a0(&DAT_005e6cf0), &head);
    if (iVar9 == 0) return 0;
    iVar9 = param_2[0];
    r = FUN_005b73b0(iVar9);
    if (r == iVar9) {
      FUN_005ade90(&head);
      return 0;
    }
    for (n = head.next; n != &head; n = n->next) {
      if (found != 0) break;
      data = n->data;
      arr = *(unsigned int **)(data + 0x54);
      for (i = 0; i < arr[2]; i++) {
        if (FUN_005ae170(((int *)arr[1])[i], r, (*(int (**)(int))(DAT_007d3ff8 + 0xf4))(((int *)arr[1])[i])) == 0) {
          found = data;
          break;
        }
      }
    }
    break;
  case 2:
    if (param_2[2] == 0) return 0;
    head.prev = &head;
    head.next = &head;
    head.data = 0;
    iVar9 = FUN_005a9ff0(0, FUN_005ad6a0(&DAT_005e6cf0), &head);
    if (iVar9 == 0) return 0;
    for (n = head.next; n != &head; n = n->next) {
      data = n->data;
      arr = *(unsigned int **)(data + 0x54);
      for (i = 0; i < arr[4]; i++) {
        if (FUN_005ac9e0(((int *)arr[3])[i], piVar1[2]) != 0) {
          found = data;
          break;
        }
      }
    }
    break;
  case 3:
    found = FUN_005aa060(0, &DAT_005e6d50, &local_10);
    goto common;
  default:
    return 0;
  }
  FUN_005ade90(&head);
common:
  if (found == 0) return 0;
  w = (int *)FUN_005aea00(*(unsigned short *)(found + 0x44), 0x1080e);
  if (w == 0) return 0;
  iVar9 = FUN_005a9e40(found, (int)w);
  if (iVar9 == 0) {
    FUN_004522d0((int)w);
    return 0;
  }
  w[0] = 0;
  w[1] = 1;
  w[2] = 0;
  w[4] = param_1 + 0x28;
  w[0xb] = piVar1[0];
  w[0xc] = piVar1[1];
  w[10] = piVar1[2];
  w[0xd] = piVar1[3];
  w[0xe] = piVar1[4];
  w[0x10] = piVar1[6];
  w[0x11] = piVar1[5];
  *(int *)(param_1 + 0x10) = piVar1[7];
  if (piVar1[8] == 0) {
    w[0x13] = 0;
  } else {
    w[0x13] = piVar1[7];
  }
  w[0xf] = 0;
  w[0x12] = 0;
  w[0x15] = 0;
  w[0x14] = 0;
  iVar9 = FUN_005aa560(found, 0, 0, (int)w, *(int *)(param_1 + 0x50), *(int *)(param_1 + 0x58));
  if (iVar9 == 0) {
    FUN_004522d0((int)w);
    return 0;
  }
  {
    int uVar6 = *(int *)(param_1 + 0x18);
    int *pl = (int *)(param_1 + 0x18);
    int *puVar2 = (int *)(iVar9 + 0x2c);
    *(int **)(iVar9 + 0x30) = pl;
    *puVar2 = uVar6;
    *(int **)(*pl + 4) = puVar2;
    *pl = (int)puVar2;
    *(int *)(param_1 + 0x14) = iVar9;
  }
  FUN_004522d0((int)w);
  return 1;
}
