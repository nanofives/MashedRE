// FLAGS: /O2 /Ob1 /G5
/* MATCH-S28 0x005c93a0: 28.9% (regs 51.5%). Compile-fail raw rewritten as a typed bit-vector ctor (struct buf/words/lg/flags, intrinsic memset, signed-char log table via MGLOBALA). Orig keeps the table sum in eax across a nested lg8/lg16/lg32 helper shape (+7 then +0x10 as two adds, no frame push of ecx, ebp holds the masked size); source shape not found. Class: b. */
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
typedef struct { unsigned int *buf; unsigned int words; unsigned char lg; unsigned char flags; } BV_s;
typedef unsigned int size_t_;
void *memset(void *, int, size_t_);
#pragma intrinsic(memset)
MGLOBALA(signed char, DAT_00617ff8, 256, 0x00617ff8);
MGLOBAL(undefined1, DAT_007de328, 0x007de328);
BV_s *FUN_005ae800(void *p0, int p1);
unsigned int *FUN_005aea00(unsigned int p0, int p1);
void FUN_005ae920(void *p0, BV_s *p1);
// MATCH: 0x005c93a0 FUN_005c93a0
BV_s *FUN_005c93a0(int param_1, BV_s *param_2, unsigned int *param_3)
{
  int cVar1;
  unsigned int uVar2;
  unsigned char bVar3;
  unsigned char bVar4;
  unsigned int uVar5;

  bVar3 = 0;
  uVar2 = ((unsigned int)param_1 + 7) & 0xfffffff8;
  if ((uVar2 & 0xffff0000) != 0) {
    if ((unsigned char)(uVar2 >> 24) != 0) {
      cVar1 = DAT_00617ff8[uVar2 >> 24] + 0x17;
    }
    else {
      cVar1 = DAT_00617ff8[uVar2 >> 16] + 0xf;
    }
  }
  else if ((uVar2 & 0xff00) != 0) {
    cVar1 = DAT_00617ff8[uVar2 >> 8] + 7;
  }
  else {
    cVar1 = DAT_00617ff8[uVar2] - 1;
  }  uVar5 = uVar2 >> 3;
  if (param_2 == (BV_s *)0) {
    param_2 = FUN_005ae800(&DAT_007de328, 0x30800);
    bVar3 = 1;
    if (param_2 == (BV_s *)0) {
      return (BV_s *)0;
    }
  }
  bVar4 = bVar3;
  if (param_3 == (unsigned int *)0) {
    param_3 = FUN_005aea00(uVar5, 0x30000);
    bVar4 = bVar3 | 2;
    if (param_3 == (unsigned int *)0) {
      if (bVar3 != 0) {
        FUN_005ae920(&DAT_007de328, param_2);
      }
      return (BV_s *)0;
    }
  }
  param_2->lg = (unsigned char)(cVar1 + 1);
  param_2->buf = param_3;
  memset(param_3, 0, uVar5);
  param_2->words = uVar5;
  param_2->flags = bVar4;
  return param_2;
}


