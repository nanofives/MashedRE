// FLAGS: /O2 /Ob1 /G5
/* h3 hand-match: best 23.6% (regs 34.4%), class b. Not worked beyond the raw converter output (budget went to higher-yield rows); flags changed from /GS to /O2 /Ob1 /G5 (same class of score); frame is 4 bytes short as in the other rows with a dead 2-dword error record, needs the array form. */
/* MATCH-RWL01 0x004ddfb0: raw g2s.py output (re/tools/match/g2s.py on decomp_pc.py --port --callees), body
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
typedef unsigned char undefined1;
typedef unsigned int undefined4;
typedef unsigned int uint;
typedef unsigned char byte;
undefined4 FUN_004d7ff0(undefined4 p0);
undefined4 * FUN_004d8480(undefined4 * p0);
// MATCH: 0x004ddfb0 FUN_004ddfb0
undefined4 FUN_004ddfb0(uint *param_1,uint *param_2,uint param_3)

{
  uint uVar1;
  undefined1 uVar2;
  undefined4 local_8;
  undefined4 local_4;
  
  if ((param_3 & 0x2000) != 0) {
    *param_1 = *param_2;
    return 1;
  }
  param_3 = param_3 & 0xf00;
  if (param_3 < 0x401) {
    if (param_3 == 0x400) {
      uVar1 = *param_2;
      *(undefined1 *)((int)param_1 + 3) = 0xff;
      uVar2 = (undefined1)uVar1;
      *(undefined1 *)param_1 = uVar2;
      *(undefined1 *)((int)param_1 + 1) = uVar2;
      *(undefined1 *)((int)param_1 + 2) = uVar2;
      return 1;
    }
    if (param_3 == 0x100) {
      uVar1 = *param_2;
      if ((char)(uVar1 >> 8) < '\0') {
        *(undefined1 *)((int)param_1 + 3) = 0xff;
      }
      else {
        *(undefined1 *)((int)param_1 + 3) = 0;
      }
      *(byte *)param_1 = (byte)(uVar1 >> 7) & 0xf8;
      *(char *)((int)param_1 + 2) = (char)uVar1 << 3;
      *(byte *)((int)param_1 + 1) = (byte)(uVar1 >> 2) & 0xf8;
      return 1;
    }
    if (param_3 == 0x200) {
      uVar1 = *param_2;
      *(undefined1 *)((int)param_1 + 3) = 0xff;
      *(byte *)param_1 = (byte)(uVar1 >> 8) & 0xf8;
      *(char *)((int)param_1 + 2) = (char)uVar1 << 3;
      *(byte *)((int)param_1 + 1) = (byte)(uVar1 >> 3) & 0xfc;
      return 1;
    }
    if (param_3 == 0x300) {
      uVar1 = *param_2;
      if ((uVar1 & 0xf000) == 0xf000) {
        *(undefined1 *)((int)param_1 + 3) = 0xff;
      }
      else {
        *(byte *)((int)param_1 + 3) = (byte)(uVar1 >> 8) & 0xf0;
      }
      *(byte *)param_1 = (byte)(uVar1 >> 4) & 0xf0;
      *(byte *)((int)param_1 + 2) = (byte)uVar1 << 4;
      *(byte *)((int)param_1 + 1) = (byte)uVar1 & 0xf0;
      return 1;
    }
  }
  else {
    if (param_3 == 0x500) {
      uVar1 = *param_2;
      *(char *)((int)param_1 + 2) = (char)uVar1;
      *(char *)param_1 = (char)(uVar1 >> 0x10);
      *(char *)((int)param_1 + 3) = (char)(uVar1 >> 0x18);
      *(char *)((int)param_1 + 1) = (char)(uVar1 >> 8);
      return 1;
    }
    if (param_3 == 0x600) {
      uVar1 = *param_2;
      *(undefined1 *)((int)param_1 + 3) = 0xff;
      *(char *)((int)param_1 + 2) = (char)uVar1;
      *(char *)param_1 = (char)(uVar1 >> 0x10);
      *(char *)((int)param_1 + 1) = (char)(uVar1 >> 8);
      return 1;
    }
    if (param_3 == 0xa00) {
      uVar1 = *param_2;
      *(undefined1 *)((int)param_1 + 3) = 0xff;
      *(byte *)param_1 = (byte)(uVar1 >> 7) & 0xf8;
      *(char *)((int)param_1 + 2) = (char)uVar1 << 3;
      *(byte *)((int)param_1 + 1) = (byte)(uVar1 >> 2) & 0xf8;
      return 1;
    }
  }
  local_8 = 1;
  local_4 = FUN_004d7ff0(0x8000000d);
  FUN_004d8480(&local_8);
  return 0;
}
