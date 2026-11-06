// FLAGS: /O2 /GS /Ob1
/* MATCH-S12 0x00476a80: raw g2s.py output (re/tools/match/g2s.py on decomp_pc.py --port --callees), body
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
typedef unsigned char undefined1;
typedef unsigned int undefined4;
MGLOBAL(undefined4, DAT_00692534, 0x00692534);
MGLOBAL(undefined4, DAT_00692538, 0x00692538);
MGLOBAL(undefined1, DAT_0069253c, 0x0069253c);
MGLOBAL(undefined1, DAT_00692540, 0x00692540);
MGLOBAL(undefined1, DAT_00692544, 0x00692544);
MGLOBAL(undefined1, DAT_00692548, 0x00692548);
MGLOBAL(undefined1, DAT_0069254c, 0x0069254c);
MGLOBAL(undefined1, DAT_00692550, 0x00692550);
MGLOBAL(undefined4, DAT_006132a4, 0x006132a4);
MGLOBAL(int, _DAT_0069253c, 0x0069253c);
MGLOBAL(int, _DAT_00692540, 0x00692540);
MGLOBAL(int, _DAT_00692544, 0x00692544);
MGLOBAL(int, _DAT_00692548, 0x00692548);
MGLOBAL(int, _DAT_0069254c, 0x0069254c);
MGLOBAL(int, _DAT_00692550, 0x00692550);
// MATCH: 0x00476a80 FUN_00476a80
/* [C1 2026-05-19] Copies 8 dwords (32 bytes) from `param_1` (default `&DAT_006132a4` if null) into
   globals at `DAT_00692534..0x00692550`… */

void FUN_00476a80(undefined4 *param_1)

{
  if (param_1 == (undefined4 *)0x0) {
    param_1 = &DAT_006132a4;
  }
  DAT_00692534 = *param_1;
  DAT_00692538 = param_1[1];
  _DAT_0069253c = param_1[2];
  _DAT_00692540 = param_1[3];
  _DAT_00692544 = param_1[4];
  _DAT_00692548 = param_1[5];
  _DAT_0069254c = param_1[6];
  _DAT_00692550 = param_1[7];
  return;
}
