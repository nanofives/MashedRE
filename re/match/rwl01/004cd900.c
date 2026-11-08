// FLAGS: /O2 /Ob1 /G5
/* MATCH-RWL01 0x004cd900: 97.2% regs 97.2%.  class (a/b) Fixed float arg (FUN_004cf160(1.0f), was pushed as converted int) 96.5 -> 97.2. Only diff: original keeps the first failure return inline (jne; xor eax,eax; pop ebx; ret) while ours tail-merges it into a later failure block (3 insns).. */
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
#ifdef __cplusplus
typedef int code(...);
#else
typedef int code();
#endif
MGLOBAL(undefined4, DAT_006182d0, 0x006182d0);
MGLOBAL(undefined4, DAT_006182cc, 0x006182cc);
MGLOBAL(int, DAT_007d4628, 0x007d4628);
MGLOBAL(undefined4, DAT_006182b4, 0x006182b4);
MGLOBAL(int, DAT_007d3ff8, 0x007d3ff8);
MGLOBAL(undefined4, DAT_006182d8, 0x006182d8);
MGLOBAL(int, DAT_007d462c, 0x007d462c);
MGLOBAL(undefined1, DAT_007d4604, 0x007d4604);
MGLOBAL(undefined1, DAT_007d4630, 0x007d4630);
undefined4 FUN_004cc9f0(int p0);
undefined4 FUN_004cf160(float p0);
uint * thunk_FUN_004cc820(int p0, uint p1, uint p2, int p3, uint * p4, uint p5);
// MATCH: 0x004cd900 FUN_004cd900

undefined4 FUN_004cd900(undefined4 param_1,int param_2)

{
  undefined1 *puVar1;
  undefined4 uVar2;
  
  DAT_007d4628 = param_2;
  uVar2 = (undefined4)thunk_FUN_004cc820(DAT_006182b4,DAT_006182cc,4,DAT_006182d0,(uint *)&DAT_007d4630,0x40018);
  *(undefined4 *)(DAT_007d3ff8 + DAT_007d4628) = uVar2;
  if (*(int *)(DAT_007d3ff8 + DAT_007d4628) == 0) {
    return 0;
  }
  uVar2 = (undefined4)thunk_FUN_004cc820(0x34,DAT_006182cc,4,DAT_006182d8,(uint *)&DAT_007d4604,0x40406);
  *(undefined4 *)(DAT_007d3ff8 + 0x218 + DAT_007d4628) = uVar2;
  if (*(int *)(DAT_007d3ff8 + 0x218 + DAT_007d4628) == 0) {
    FUN_004cc9f0(*(undefined4 *)(DAT_007d3ff8 + DAT_007d4628));
    *(undefined4 *)(DAT_007d3ff8 + DAT_007d4628) = 0;
    return 0;
  }
  *(undefined4 *)(DAT_007d3ff8 + 8 + DAT_007d4628) = 0x100;
  uVar2 = (**(code **)(DAT_007d3ff8 + 0x108))
                    (*(undefined4 *)(DAT_007d3ff8 + 8 + DAT_007d4628),0x1040406);
  *(undefined4 *)(DAT_007d3ff8 + 4 + DAT_007d4628) = uVar2;
  puVar1 = *(undefined1 **)(DAT_007d3ff8 + 4 + DAT_007d4628);
  if (puVar1 == (undefined1 *)0x0) {
    FUN_004cc9f0(*(undefined4 *)(DAT_007d3ff8 + 0x218 + DAT_007d4628));
    *(undefined4 *)(DAT_007d3ff8 + 0x218 + DAT_007d4628) = 0;
    FUN_004cc9f0(*(undefined4 *)(DAT_007d3ff8 + DAT_007d4628));
    *(undefined4 *)(DAT_007d3ff8 + DAT_007d4628) = 0;
    return 0;
  }
  *puVar1 = 0;
  DAT_007d462c = DAT_007d462c + 1;
  FUN_004cf160(1.0f);
  *(undefined4 *)(DAT_007d3ff8 + 0x21c + DAT_007d4628) = 0;
  *(undefined4 *)(DAT_007d3ff8 + 0x214 + DAT_007d4628) = 0x100;
  uVar2 = (**(code **)(DAT_007d3ff8 + 0x108))
                    (*(undefined4 *)(DAT_007d3ff8 + 0x214 + DAT_007d4628),0x1040018);
  *(undefined4 *)(DAT_007d3ff8 + 0x210 + DAT_007d4628) = uVar2;
  if (*(int *)(DAT_007d3ff8 + 0x210 + DAT_007d4628) == 0) {
    (**(code **)(DAT_007d3ff8 + 0x10c))(*(undefined4 *)(DAT_007d3ff8 + 4 + DAT_007d4628));
    *(undefined4 *)(DAT_007d3ff8 + 4 + DAT_007d4628) = 0;
    *(undefined4 *)(DAT_007d3ff8 + 8 + DAT_007d4628) = 0;
    FUN_004cc9f0(*(undefined4 *)(DAT_007d3ff8 + 0x218 + DAT_007d4628));
    *(undefined4 *)(DAT_007d3ff8 + 0x218 + DAT_007d4628) = 0;
    FUN_004cc9f0(*(undefined4 *)(DAT_007d3ff8 + DAT_007d4628));
    *(undefined4 *)(DAT_007d3ff8 + DAT_007d4628) = 0;
    return 0;
  }
  return param_1;
}
