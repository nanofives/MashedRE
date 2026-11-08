// FLAGS: /O2 /Ob1 /G5
/* MATCH-RWL04 0x00550670: raw g2s.py output (re/tools/match/g2s.py on decomp_pc.py --port --callees), body
   unchanged; globals moved to MGLOBAL. Score: see verify/matchrwl04_20261107/results.tsv. */
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
typedef unsigned char undefined;
typedef unsigned char undefined1;
typedef unsigned int undefined4;
typedef unsigned int uint;
typedef uint (*PFN_len)(char *);
typedef int (*PFN_cmp)(char *, undefined4);
typedef void (*PFN_a)(int);
typedef unsigned int size_t;
MGLOBAL(int, DAT_007dc75c, 0x007dc75c);
MGLOBAL(int, DAT_007d3ff8, 0x007d3ff8);
MGLOBAL(undefined4 *, DAT_007dc754, 0x007dc754);
MGLOBAL(undefined4 *, DAT_007dc76c, 0x007dc76c);
MGLOBAL(PFN_a, DAT_007dc768, 0x007dc768);
undefined4 FUN_00550580(undefined4 *p0, char *p1, undefined4 p2, undefined4 p3, undefined4 p4);
char * _strncpy(char * p0, char * p1, size_t p2);
// MATCH: 0x00550670 FUN_00550670
undefined4 FUN_00550670(char *param_1,undefined4 param_2,undefined4 param_3,undefined4 param_4)

{
  uint uVar1;
  int iVar2;
  undefined4 uVar3;
  undefined4 *puVar4;
  uint uVar5;
  char cStack_64[100];
  
  if (DAT_007dc75c != 0) {
    uVar1 = (**(PFN_len *)(DAT_007d3ff8 + 0xf4))(param_1);
    uVar5 = 0;
    if (uVar1 > 0) {
      do {
        if (param_1[uVar5] == ':') {
          _strncpy(cStack_64,param_1,uVar5 + 1);
          cStack_64[uVar5 + 1] = 0;
          for (puVar4 = DAT_007dc754; puVar4 != (undefined4 *)0x0; puVar4 = (undefined4 *)*puVar4) {
            iVar2 = (**(PFN_cmp *)(DAT_007d3ff8 + 0xf0))(cStack_64,puVar4[3]);
            if (iVar2 == 0) {
              goto LAB_00550711;
            }
          }
        }
        uVar5 = uVar5 + 1;
      } while (uVar5 < uVar1);
    }
    puVar4 = DAT_007dc76c;
    if (DAT_007dc76c == (undefined4 *)0x0) {
      if (DAT_007dc768 != 0) {
        (*DAT_007dc768)(6);
      }
      return 0;
    }
LAB_00550711:
    if (puVar4 == (undefined4 *)0x0) {
      return 0;
    }
    uVar3 = FUN_00550580(puVar4,param_1,param_2,param_3,param_4);
    return uVar3;
  }
  if (DAT_007dc768 != 0) {
    (*DAT_007dc768)(6);
  }
  return 0;
}
