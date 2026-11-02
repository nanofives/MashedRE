// FLAGS: /O2 /GS /Ob1
/* 0x0048a430: 100% (regs 100%, whole symbol byte-identical). By hand from raw (70%): the slot +0x48 call
   takes the object as its one argument (mov [esp+4],eax; jmp [eax+0x48] tail call). Array lengths 4 are
   the 16-byte gap between the two globals, not a measured bound. */
/* MATCH-S14 0x0048a430: raw g2s.py output (re/tools/match/g2s.py on decomp_pc.py --port --callees), body
   unchanged; globals moved to MGLOBAL. Score: see verify/matchs14_20261102/results.tsv. */
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
typedef void (*fn_obj_t)(int obj);
MGLOBALA(int, DAT_007067cc, 4, 0x007067cc);
MGLOBALA(int, DAT_007067dc, 4, 0x007067dc);
// MATCH: 0x0048a430 FUN_0048a430
void FUN_0048a430(int param_1)
{
  int obj = DAT_007067dc[param_1];
  if (obj != 0 && DAT_007067cc[param_1] != 0) {
    (*(fn_obj_t *)(obj + 0x48))(obj);
  }
}
