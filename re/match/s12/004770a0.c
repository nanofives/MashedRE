// FLAGS: /O2 /GS /Ob1
/* 0x004770a0 (31 B): 90.0% (regs 100%, ops 100%). Tail call through the vtable-like slot +0x48 with the loaded
   object passed as the one stack argument (the original rewrites [esp+4] and jmps). Only residue: the plugin-offset
   address is [ecx+eax] in ours, [eax+ecx] in the original (SIB base/index swap). Tried: p + off, off + p, char* /
   uchar* / int base, array index form, struct ext, unsigned/char* global, file-static global, early return, permuter
   (86 compiles, no gain). Class (b). */
/* MATCH-S12 0x004770a0: raw g2s.py output (re/tools/match/g2s.py on decomp_pc.py --port --callees), body
   unchanged; globals moved to MGLOBAL. Score: see verify/matchs12_20261102/results.tsv. */
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
typedef void (*code)(int);
MGLOBAL(int, DAT_007dc57c, 0x007dc57c);
// MATCH: 0x004770a0 FUN_004770a0
void FUN_004770a0(int param_1)
{
  int iVar1 = *(int *)(param_1 + 4);
  if (*(int *)(*(int *)(iVar1 + DAT_007dc57c) + 4) != 0) {
    (*(code *)(iVar1 + 0x48))(iVar1);
  }
}
