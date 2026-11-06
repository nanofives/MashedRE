// FLAGS: /O2 /GS /Ob1
/* MATCH-S13 0x0047b8d0: raw g2s.py output (re/tools/match/g2s.py on decomp_pc.py --port --callees), body
   unchanged; globals moved to MGLOBAL. Score: see verify/matchs13_20261102/results.tsv. */
/* ---- MATCH-S13 preamble (same text in every re/match/s13 source; not part of any function body) ----
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
MGLOBAL(uint, DAT_00616038, 0x00616038);
MGLOBAL(undefined1, DAT_005cf010, 0x005cf010);
bool FUN_0047b8a0(undefined4 p0, undefined4 p1, int * p2);
void FUN_004b6520(undefined4 p0, undefined4 p1);
void __chkstk(void);
undefined4 thunk_FUN_005507b0(char * p0, char * p1);
undefined4 thunk_FUN_00550910(int p0);
uint thunk_FUN_00550950(undefined4 p0, uint p1, int p2, int p3);
undefined4 thunk_FUN_005509b0(int p0, undefined4 p1, int p2);
undefined4 thunk_FUN_00550bc0(int p0);
// MATCH: 0x0047b8d0 FUN_0047b8d0
/* WARNING: Function: __security_check_cookie replaced with injection: security_check_cookie */
/* [C2 2026-06-02] Two-parameter function: `param_1` (file-name pointer, passed straight to fopen),
   `param_2` (forwarded to FUN_0047b8a0)
 */

void FUN_0047b8d0(undefined4 param_1,undefined4 param_2)

{
  undefined4 uVar1;
  undefined4 uVar2;
  uint unaff_retaddr;
  undefined1 local_8004 [32768];
  uint local_4;
  
  local_4 = DAT_00616038 ^ unaff_retaddr;
  FUN_004b6520(local_8004,0x8000);
  uVar1 = thunk_FUN_005507b0(param_1,&DAT_005cf010);
  thunk_FUN_005509b0(uVar1,0,2);
  uVar2 = thunk_FUN_00550bc0(uVar1);
  thunk_FUN_005509b0(uVar1,0,0);
  uVar2 = thunk_FUN_00550950(local_8004,1,uVar2,uVar1);
  thunk_FUN_00550910(uVar1);
  FUN_0047b8a0(local_8004,uVar2,param_2);
  return;
}
