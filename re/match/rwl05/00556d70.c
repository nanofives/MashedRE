// FLAGS: /O2 /Ob1 /G5
/* MATCH-RWL05 0x00556d70 (h2): hand, 100%: typed byte/uint arrays for the two stack locals. */
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
MGLOBAL(unsigned int, DAT_00912a00, 0x00912a00);
MGLOBAL(int, DAT_007d3ff8, 0x007d3ff8);
typedef unsigned int *(*AL)(unsigned int,int);
unsigned int FUN_004c5a60(unsigned int p0);
float * FUN_00556e90(unsigned int *p0, unsigned char *p1, unsigned char *p2, unsigned char *p3, unsigned char *p4);
int FUN_00557110(unsigned int *p0, unsigned int *p1, unsigned int *p2, unsigned int *p3, unsigned int *p4);
// MATCH: 0x00556d70 FUN_00556d70
unsigned int * FUN_00556d70(void)
{
  unsigned int *puVar1;
  unsigned int *puVar2;
  int iVar3;
  unsigned char c[4];
  unsigned int v[2];

  puVar1 = (*(AL *)(DAT_007d3ff8 + 0x118))(DAT_00912a00,0x301a2);
  puVar2 = 0;
  if (puVar1 != 0) {
    puVar2 = puVar1;
    for (iVar3 = 0x1e; iVar3 != 0; iVar3 = iVar3 + -1) {
      *puVar2 = 0;
      puVar2 = puVar2 + 1;
    }
    c[0] = 0xff;
    c[1] = 0xff;
    c[2] = 0xff;
    c[3] = 0xff;
    FUN_00556e90(puVar1,c,c,c,c);
    v[0] = 0;
    v[1] = 0;
    FUN_00557110(puVar1,v,v,v,v);
    puVar1[0x19] = puVar1[0x19] & 0xfffffffd;
    if (puVar1[0x1a] != 0) {
      FUN_004c5a60(puVar1[0x1a]);
    }
    puVar1[0x1a] = 0;
    puVar1[0x1c] = 0x3f000000;
    puVar1[0x1d] = 1;
    puVar2 = puVar1;
  }
  return puVar2;
}
