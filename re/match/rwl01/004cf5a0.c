// TOOLCHAIN rw9044
// FLAGS: /O2 /Ob1 /G5
/* MATCH-RWL01 0x004cf5a0: best 59.4% (regs 70%, ops 79%), class (b). Header words as one uint[4] (flags, error word, size, type) so the frame is 0x110 as in the original, FUN_004d8810 returns int. The original reads byte [esp+0x12] into ebx before the calls and branches on it (test bl,1 then push 0/1); byte-access and ternary spellings of the 0x10000 test scored lower (55%). */
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
MGLOBAL(undefined4, DAT_00618138, 0x00618138);
undefined4 FUN_004c5800(undefined4 p0);
undefined4 FUN_004c5820(void);
undefined4 FUN_004c5830(undefined4 p0);
undefined4 FUN_004c5850(void);
int FUN_004c5cb0(undefined1 * p0, undefined1 * p1);
uint FUN_004cbd30(undefined4 p0, undefined4 * p1, uint p2);
undefined4 FUN_004cc5e0(undefined4 p0, int p1, undefined4 * p2, uint * p3);
undefined4 FUN_004d7ff0(undefined4 p0);
undefined4 * FUN_004d8480(undefined4 * p0);
int FUN_004d8810(undefined1 * p0, undefined4 p1);
int FUN_004e1b60(undefined4 * p0, int p1, int p2);
undefined4 FUN_004e1df0(undefined4 * p0, int p1);
// MATCH: 0x004cf5a0 FUN_004cf5a0
int FUN_004cf5a0(undefined4 param_1)

{
  uint uVar1;
  int iVar2;
  undefined4 uVar3;
  undefined4 uVar4;
  int iVar5;
  uint uVar6;
  uint hdr[4];
  undefined1 local_100 [128];
  undefined1 local_80 [128];
  
  iVar2 = FUN_004cc5e0(param_1,1,&hdr[2],&hdr[3]);
  if (iVar2 != 0) {
    if ((hdr[3] < 0x35000) || (0x37002 < hdr[3])) {
      hdr[0] = 1;
      hdr[1] = FUN_004d7ff0(0x80000004);
      FUN_004d8480((undefined4 *)hdr);
    }
    else {
      hdr[0] = 0;
      iVar2 = FUN_004cbd30(param_1,(undefined4 *)hdr,hdr[2]);
      if (iVar2 == hdr[2]) {
        uVar6 = hdr[0] & 0xff;
        if ((hdr[0] & 0xf000) == 0) {
          hdr[0] = hdr[0] | (hdr[0] & 0xf00) << 4;
        }
        uVar1 = hdr[0];
        uVar3 = FUN_004c5820();
        uVar4 = FUN_004c5850();
        if ((((uVar6 == 3) || (uVar6 == 4)) || (uVar6 == 5)) || (uVar6 == 6)) {
          FUN_004c5800(1);
          FUN_004c5830((uVar1 & 0x10000) == 0);
        }
        else {
          FUN_004c5800(0);
          FUN_004c5830(0);
        }
        iVar2 = FUN_004d8810(local_80,param_1);
        if (iVar2 == 0) {
          FUN_004c5800(uVar3);
          FUN_004c5830(uVar4);
          return 0;
        }
        iVar2 = FUN_004d8810(local_100,param_1);
        if (iVar2 == 0) {
          FUN_004c5800(uVar3);
          FUN_004c5830(uVar4);
          return 0;
        }
        iVar2 = FUN_004c5cb0(local_80,local_100);
        if (iVar2 != 0) {
          FUN_004c5800(uVar3);
          FUN_004c5830(uVar4);
          if (*(int *)(iVar2 + 0x54) == 1) {
            *(uint *)(iVar2 + 0x50) = hdr[0] & 0xffff;
            iVar5 = FUN_004e1b60((undefined4 *)&DAT_00618138,param_1,iVar2);
            if (iVar5 == 0) {
              return 0;
            }
          }
          else {
            iVar5 = FUN_004e1df0((undefined4 *)&DAT_00618138,param_1);
            if (iVar5 == 0) {
              return 0;
            }
          }
          return iVar2;
        }
        FUN_004e1df0((undefined4 *)&DAT_00618138,param_1);
        FUN_004c5800(uVar3);
        FUN_004c5830(uVar4);
        return 0;
      }
    }
  }
  return 0;
}
