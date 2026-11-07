// FLAGS: /O2 /Ob1 /G5
/* MATCH-S26 0x005ad5f0 (h3): 100.0% exact (regs 100.0%). count read as (uint)(ushort) into a temp, loop counter a separate copy (cnt = n after iVar2 = 0); /G5 gives xor eax,eax;mov ax; unsigned > 0. */
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
typedef unsigned short ushort;
typedef unsigned char byte;
MGLOBAL(undefined1, DAT_007dd684, 0x007dd684);
void FUN_004522d0(undefined4 p0);
undefined4 FUN_005ae030(int p0);
void FUN_005ae920(undefined1 *p0, int p1);
// MATCH: 0x005ad5f0 FUN_005ad5f0
undefined4 FUN_005ad5f0(int param_1)
{
  void (*cb)(int);
  uint n;
  uint cnt;
  int iVar2;
  byte b;

  cb = *(void (**)(int))(param_1 + 0x1c);
  if (cb != 0) {
    cb(param_1);
  }
  FUN_005ae030(param_1);
  n = (uint)*(ushort *)(param_1 + 0x14);
  if (n > 0) {
    iVar2 = 0;
    cnt = n;
    do {
      FUN_005ae030(*(int *)(param_1 + 0x10) + iVar2);
      iVar2 = iVar2 + 0x14;
      cnt = cnt - 1;
    } while (cnt != 0);
  }
  n = (uint)*(ushort *)(param_1 + 0x16);
  if (n > 0) {
    iVar2 = 0;
    cnt = n;
    do {
      FUN_005ae030(*(int *)(param_1 + 0xc) + iVar2);
      iVar2 = iVar2 + 0x14;
      cnt = cnt - 1;
    } while (cnt != 0);
  }
  b = *(byte *)(param_1 + 0x18);
  if ((b & 1) == 0) {
    FUN_004522d0(*(undefined4 *)(param_1 + 0x10));
  }
  **(undefined4 **)(param_1 + 0x24) = *(undefined4 *)(param_1 + 0x20);
  *(undefined4 *)(*(int *)(param_1 + 0x20) + 4) = *(undefined4 *)(param_1 + 0x24);
  b = *(byte *)(param_1 + 0x18);
  if ((b & 2) == 0) {
    FUN_005ae920(&DAT_007dd684,param_1);
  }
  return 1;
}
