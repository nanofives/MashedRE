// FLAGS: /O2 /GS /Ob1 /G5
/* MATCH-S19 0x004c6140: raw g2s.py output (re/tools/match/g2s.py on decomp_pc.py --port --callees), body
   unchanged; globals moved to MGLOBAL. Score: see verify/matchs19_20261102/results.tsv. */
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
typedef int code();
#define CONCAT11(a,b) ((ushort)(((uint)(uchar)(a)<<8)|(uchar)(b)))
#define CONCAT12(a,b) ((uint)(((uint)(uchar)(a)<<16)|(ushort)(b)))
MGLOBAL(int, DAT_007d3ff8, 0x007d3ff8);
// MATCH: 0x004c6140 FUN_004c6140
undefined4 __thiscall __fastcall FUN_004c6140(undefined4 param_1,undefined4 param_2,char *param_3,byte param_4)

{
  char cVar1;
  undefined1 local_4 [3];
  undefined1 uStack_1;
  
  if ((param_4 == 0) || (0xf < param_4)) {
    cVar1 = '\0';
  }
  else {
    cVar1 = "0123456789abcdef"[param_4];
  }
  uStack_1 = (undefined1)((uint)param_1 >> 0x18);
  local_4 = (undefined1  [3])CONCAT12(0,CONCAT11(cVar1,0x6d));
  if (cVar1 != '\0') {
    (**(code **)(DAT_007d3ff8 + 0xd4))(param_2,local_4);
    if ((param_3 != (char *)0x0) && (*param_3 != '\0')) {
      (**(code **)(DAT_007d3ff8 + 0xd4))(param_3,local_4);
    }
  }
  return 1;
}
