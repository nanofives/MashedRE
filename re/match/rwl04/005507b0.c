// FLAGS: /O2 /Ob1 /G5
/* MATCH-RWR04 2026-11-08: 100% (regs 100%), relocheck OK, C++ check OK, was 82.0%. Same error-block shape as 0x00550670. Supersedes the header below. */
/* MATCH-RWL04 0x005507b0: raw g2s.py output (re/tools/match/g2s.py on decomp_pc.py --port --callees), body
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
MGLOBAL(undefined1, DAT_00617378, 0x00617378);
MGLOBAL(undefined1, DAT_00623dd8, 0x00623dd8);
MGLOBAL(undefined1, DAT_00623ddc, 0x00623ddc);
MGLOBAL(undefined1, DAT_00623de0, 0x00623de0);
MGLOBAL(undefined1, DAT_00623de4, 0x00623de4);
undefined4 FUN_00550580(undefined4 *p0, char *p1, uint p2, undefined4 p3, undefined4 p4);
char * _strncpy(char * p0, char * p1, size_t p2);
char * _strstr(char * p0, char * p1);
// MATCH: 0x005507b0 FUN_005507b0

undefined4 FUN_005507b0(char *param_1,char *param_2)

{
  int iVar3;
  undefined4 uVar4;
  uint uVar5;
  uint uVar6;
  undefined4 *puVar7;
  uint uVar8;
  int bVar9;
  char cStack_64[100];
  
  uVar6 = 0;
  bVar9 = 0;
  uVar5 = 0;
  if (_strstr(param_2,(char *)&DAT_00623de4) != (char *)0x0) {
    bVar9 = 1;
  }
  if (_strstr(param_2,(char *)&DAT_00623de0) != (char *)0x0) {
    uVar5 = 1;
  }
  if (_strstr(param_2,(char *)&DAT_00617378) != (char *)0x0) {
    uVar6 = 1;
    if (bVar9) {
      uVar6 = 0xb;
    }
  }
  if (_strstr(param_2,(char *)&DAT_00623ddc) != (char *)0x0) {
    uVar6 = uVar6 | 6;
    if (bVar9) {
      uVar6 = uVar6 | 1;
    }
  }
  if (_strstr(param_2,(char *)&DAT_00623dd8) != (char *)0x0) {
    uVar6 = uVar6 | 10;
    if (bVar9) {
      uVar6 = uVar6 | 1;
    }
    if (uVar5 != 0) {
      uVar6 = uVar6 | 4;
    }
  }
  if (DAT_007dc75c != 0) {
    param_2 = (char *)(**(PFN_len *)(DAT_007d3ff8 + 0xf4))(param_1);
    uVar8 = 0;
    if ((uint)param_2 > 0) {
      do {
        if (param_1[uVar8] == ':') {
          _strncpy(cStack_64,param_1,uVar8 + 1);
          cStack_64[uVar8 + 1] = 0;
          for (puVar7 = DAT_007dc754; puVar7 != (undefined4 *)0x0; puVar7 = (undefined4 *)*puVar7) {
            iVar3 = (**(PFN_cmp *)(DAT_007d3ff8 + 0xf0))(cStack_64,puVar7[3]);
            if (iVar3 == 0) {
              goto LAB_005508ee;
            }
          }
        }
        uVar8 = uVar8 + 1;
      } while (uVar8 < (uint)param_2);
    }
    puVar7 = DAT_007dc76c;
    if (DAT_007dc76c == (undefined4 *)0x0) {
      if (DAT_007dc768 != 0) {
        (*DAT_007dc768)(6);
      }
      return 0;
    }
LAB_005508ee:
    if (puVar7 == (undefined4 *)0x0) {
      return 0;
    }
    uVar4 = FUN_00550580(puVar7,param_1,uVar6,0,0);
    return uVar4;
  }
  if (DAT_007dc768 != 0) {
    (*DAT_007dc768)(6);
  }
  return 0;
}
