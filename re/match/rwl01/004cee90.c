// FLAGS: /O2 /Ob1 /G5
/* MATCH-RWL01 0x004cee90: best 66.7% (regs 80%), class (b). The four stream-read words are one int[4] (one read of 16 bytes), early return on a failed read. The original keeps the header words in callee-saved regs across the pool alloc (the FUN_004cdca0 alloc is inlined there); an __inline static copy of the alloc made it worse (40.7%) and was dropped. */
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
typedef unsigned char byte;
#ifdef __cplusplus
typedef int code(...);
#else
typedef int code();
#endif
MGLOBAL(int, DAT_007d4628, 0x007d4628);
MGLOBAL(int, DAT_007d3ff8, 0x007d3ff8);
MGLOBAL(undefined4, DAT_006182b4, 0x006182b4);
uint FUN_004cbd30(int p0, int p1, uint p2);
undefined4 * FUN_004cc050(int p0, int p1);
undefined4 FUN_004cc5e0(undefined4 p0, int p1, undefined4 * p2, uint * p3);
uint * FUN_004cdd60(uint * p0);
int FUN_004d8000(int p0, undefined4 * p1);
// MATCH: 0x004cee90 FUN_004cee90
undefined4 * FUN_004cee90(int param_1)

{
  undefined4 uVar1;
  int iVar2;
  undefined4 *puVar3;
  int iVar4;
  undefined1 local_18 [4];
  undefined1 local_14 [4];
  int hdr[4];
  
  uVar1 = param_1;
  iVar2 = FUN_004cc5e0(param_1,1,(undefined4 *)local_14,(uint *)local_18);
  if (iVar2 == 0) {
    return (undefined4 *)0x0;
  }
  FUN_004cbd30(param_1,(int)hdr,0x10);
  iVar2 = hdr[1];
  puVar3 = (undefined4 *)
           (**(code **)(DAT_007d3ff8 + 0x118))
                     (*(undefined4 *)(DAT_007d4628 + DAT_007d3ff8),0x30018);
  if (puVar3 == (undefined4 *)0x0) {
    puVar3 = (undefined4 *)0x0;
  }
  else {
    puVar3[2] = iVar2;
    puVar3[3] = hdr[2];
    puVar3[1] = hdr[0];
    puVar3[5] = 0;
    puVar3[6] = 0;
    *puVar3 = 0;
    FUN_004d8000((int)&DAT_006182b4,puVar3);
  }
  FUN_004cdd60(puVar3);
  iVar2 = puVar3[4];
  if (iVar2 == hdr[3]) {
    FUN_004cbd30(param_1,puVar3[5],puVar3[2] * iVar2);
  }
  else {
    iVar4 = 0;
    param_1 = 0;
    if (0 < hdr[1]) {
      do {
        if (iVar2 > hdr[3]) {
          FUN_004cbd30(uVar1,puVar3[5] + iVar4,hdr[3]);
        }
        else {
          FUN_004cbd30(uVar1,puVar3[5] + iVar4,iVar2);
          FUN_004cc050(uVar1,hdr[3] - iVar2);
        }
        param_1 = param_1 + 1;
        iVar4 = iVar4 + iVar2;
      } while (param_1 < hdr[1]);
    }
  }
  if ((int)puVar3[3] <= 8) {
    FUN_004cbd30(uVar1,puVar3[6],(1 << ((byte)0x1f & puVar3[3])) << 2);
  }
  return puVar3;
}
