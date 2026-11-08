// FLAGS: /O2 /Ob1 /G5
/* h3 hand-match: best 68.5% (regs 74.1%), class b. Fixed realloc arity (3 args, flags hoisted) and COM calls to __stdcall: 64.6 -> 68.5. Residual: switch 0x15..0x23 compiles as a compare chain, original is byte index table + 2-entry jump table (tried explicit case labels 0x1a..0x22 into default, no change); malloc/realloc arm layout and the post-loop realloc tail also differ. */
/* MATCH-RWL01 0x004c90a0: raw g2s.py output (re/tools/match/g2s.py on decomp_pc.py --port --callees), body
   unchanged; globals moved to MGLOBAL. Score: see verify/matchrwl01_20261107/results.tsv. */
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
typedef unsigned int undefined4;
typedef unsigned int uint;
#ifdef __cplusplus
typedef int code(...);
#else
typedef int code();
#endif
#ifndef __cplusplus
#define true 1
#endif
MGLOBAL(int, DAT_007d4130, 0x007d4130);
MGLOBAL(int, DAT_00911f80, 0x00911f80);
MGLOBAL(int, DAT_007d3ff8, 0x007d3ff8);
MGLOBAL(int *, DAT_007d4108, 0x007d4108);
MGLOBAL(undefined4, DAT_007d410c, 0x007d410c);
MGLOBAL(int, DAT_007d4128, 0x007d4128);
MGLOBAL(undefined4, DAT_005d8b80, 0x005d8b80);
// MATCH: 0x004c90a0 FUN_004c90a0
void FUN_004c90a0(void)

{
  int iVar1;
  int iVar2;
  int iVar3;
  int *piVar4;
  undefined4 *puVar5;
  int iVar6;
  
  if (DAT_007d4130 == 0) {
    DAT_007d4130 = (**(code **)(DAT_007d3ff8 + 0x108))((DAT_00911f80 * 5 + 5) * 4,0x1040411);
  }
  else {
    DAT_007d4130 = (**(code **)(DAT_007d3ff8 + 0x110))(DAT_007d4130,(DAT_00911f80 * 5 + 5) * 4,0x1040411);
  }
  (*(int (__stdcall *)(int *,undefined4,int))*(int *)(*DAT_007d4108 + 0x20))(DAT_007d4108,DAT_007d410c,DAT_007d4130);
  switch(*(undefined4 *)(DAT_007d4130 + 0xc)) {
  case 0x15:
  case 0x16:
  case 0x17:
  case 0x18:
  case 0x19:
  case 0x23:
    *(undefined4 *)(DAT_007d4130 + 0x10) = 0;
    DAT_007d4128 = 1;
    break;
  case 0x1a: case 0x1b: case 0x1c: case 0x1d: case 0x1e: case 0x1f: case 0x20: case 0x21: case 0x22:
  default:
    DAT_007d4128 = 0;
  }
  puVar5 = &DAT_005d8b80;
  do {
    iVar2 = (*(int (__stdcall *)(int *,undefined4,undefined4))*(int *)(*DAT_007d4108 + 0x18))(DAT_007d4108,DAT_007d410c,*puVar5);
    iVar6 = 0;
    if (0 < iVar2) {
      do {
        (*(int (__stdcall *)(int *,undefined4,undefined4,int,int))*(int *)(*DAT_007d4108 + 0x1c))
                  (DAT_007d4108,DAT_007d410c,*puVar5,iVar6,DAT_007d4130 + DAT_007d4128 * 0x14);
        iVar3 = 1;
        if (1 < DAT_007d4128) {
          iVar1 = DAT_007d4130 + DAT_007d4128 * 0x14;
          piVar4 = (int *)(DAT_007d4130 + 0x20);
          do {
            if (((*(int *)(DAT_007d4130 + DAT_007d4128 * 0x14) == piVar4[-3]) &&
                (*(int *)(iVar1 + 4) == piVar4[-2])) && (*(int *)(iVar1 + 0xc) == *piVar4)) {
              if (iVar3 < DAT_007d4128) {
                if (*(uint *)(DAT_007d4130 + 8 + iVar3 * 0x14) < *(uint *)(iVar1 + 8)) {
                  *(uint *)(DAT_007d4130 + 8 + iVar3 * 0x14) = *(uint *)(iVar1 + 8);
                }
                goto LAB_004c920b;
              }
              break;
            }
            iVar3 = iVar3 + 1;
            piVar4 = piVar4 + 5;
          } while (iVar3 < DAT_007d4128);
        }
        *(undefined4 *)(DAT_007d4130 + 0x10 + DAT_007d4128 * 0x14) = 1;
        DAT_007d4128 = DAT_007d4128 + 1;
LAB_004c920b:
        iVar6 = iVar6 + 1;
      } while (iVar6 < iVar2);
    }
    puVar5 = puVar5 + 1;
    if (0x5d8b8b < (int)puVar5) {
      if (DAT_007d4128 < DAT_00911f80 + 1) {
        DAT_007d4130 = (**(code **)(DAT_007d3ff8 + 0x110))
                                 (DAT_007d4130,DAT_007d4128 * 0x14,0x1040411);
      }
      return;
    }
  } while( true );
}
