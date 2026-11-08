// FLAGS: /O2 /Ob1 /G5
// h2: 100.0% (regs 100%), relocheck OK. FUN_004d7ff0 stored as a code pointer.
/* MATCH-RWL02 0x004e6d80: raw g2s.py output (re/tools/match/g2s.py on decomp_pc.py --port --callees), body
   unchanged; globals moved to MGLOBAL. Score: see verify/matchrwl02_20261107/results.tsv. */
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
#ifdef __cplusplus
typedef int code(...);
#else
typedef int code();
#endif
MGLOBAL(int, DAT_007d3ff8, 0x007d3ff8);
MGLOBAL(int, DAT_007d7200, 0x007d7200);
MGLOBAL(undefined4, DAT_0061867c, 0x0061867c);
void FUN_004d7ff0(void);
int FUN_004d8000(void *p0, void *p1);
// MATCH: 0x004e6d80 FUN_004e6d80
undefined1 * FUN_004e6d80(void)

{
  undefined1 *puVar1;
  undefined1 *puVar2;
  
  puVar2 = (undefined1 *)
           (**(code **)(DAT_007d3ff8 + 0x118))
                     (*(undefined4 *)(DAT_007d7200 + 4 + DAT_007d3ff8),0x30010);
  if (puVar2 == (undefined1 *)0x0) {
    return (undefined1 *)0x0;
  }
  puVar1 = puVar2 + 8;
  *(undefined1 **)(puVar2 + 0xc) = puVar1;
  *(undefined1 **)puVar1 = puVar1;
  puVar1 = puVar2 + 0x10;
  *(undefined1 **)(puVar2 + 0x14) = puVar1;
  *puVar2 = 2;
  *(undefined1 **)puVar1 = puVar1;
  puVar1 = puVar2 + 0x18;
  puVar2[1] = 0;
  puVar2[2] = 0;
  puVar2[3] = 0;
  *(undefined4 *)(puVar2 + 4) = 0;
  *(undefined1 **)puVar1 = puVar1;
  *(undefined1 **)(puVar2 + 0x1c) = puVar1;
  *(undefined4 *)(puVar2 + 0x24) = 0;
  *(undefined4 *)(puVar2 + 0x20) = 0;
  *(code **)(puVar2 + 0x28) = (code *)FUN_004d7ff0;
  FUN_004d8000(&DAT_0061867c,puVar2);
  return puVar2;
}
