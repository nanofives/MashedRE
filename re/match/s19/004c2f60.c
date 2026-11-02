// FLAGS: /O2 /GS /Ob1
/* MATCH-S19 0x004c2f60 (h2): 76.9%, regs 92.3%. Hand: device pointer (RwGlobals+0x10) held in a local across the calls. Residual (b): original loads RwGlobals into eax before push esi and forms esi with lea; here mov esi/add esi. Tried typed RwGlobals struct, separate g local; permuter 67 compiles no gain. */
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
MGLOBAL(int, DAT_007d3ff8, 0x007d3ff8);
MGLOBALA(char, DAT_00617fe0, 1, 0x00617fe0);
int FUN_004c2c90(void *p0, int p1, void *p2, int p3, int p4);
int FUN_004d8060(char *p0, int p1);
// MATCH: 0x004c2f60 FUN_004c2f60
void FUN_004c2f60(void)
{
  void *dev = (void *)(DAT_007d3ff8 + 0x10);
  FUN_004c2c90(dev, 0x12, 0, 0, 0);
  FUN_004d8060(DAT_00617fe0, DAT_007d3ff8);
  if (FUN_004c2c90(dev, 3, 0, 0, 0)) {
    *(int *)(DAT_007d3ff8 + 0x124) = 2;
  }
}
