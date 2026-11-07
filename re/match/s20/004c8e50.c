// FLAGS: /O2 /Ob1 /G5
/* MATCH-S20 0x004c8e50 (h3): 100% (regs 100%). Device standards table: a local RwStandard[27] {id, fn} initialiser, fills numStandardsFunctions slots with FUN_005c9d00 (null standard), then walks the table backwards storing each fn at its id. The guard tests standards->nStandard (element 0, the constant 1) against the count, which is why the original compares the count with 1 (cmp esi,1): the RW source indexes the array but tests through the array name. From 24.6%: RwStandard struct initialiser, separate numDriverFunctions counter and out cast, the element-0 guard. Relocheck OK. */
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
typedef int (*RwStandardFunc)(void *pOut, void *pInOut, int nI);
typedef struct RwStandard { int nStandard; RwStandardFunc fpStandard; } RwStandard;
int FUN_005c9d00(void *pOut, void *pInOut, int nI);
int FUN_004ca160(void *pOut, void *pInOut, int nI);
int FUN_004cab30(void *pOut, void *pInOut, int nI);
int FUN_004c9290(void *pOut, void *pInOut, int nI);
int FUN_004cac10(void *pOut, void *pInOut, int nI);
int FUN_004ddd60(void *pOut, void *pInOut, int nI);
int FUN_004ddfb0(void *pOut, void *pInOut, int nI);
int FUN_004df120(void *pOut, void *pInOut, int nI);
int FUN_004de1b0(void *pOut, void *pInOut, int nI);
int FUN_004d3090(void *pOut, void *pInOut, int nI);
int FUN_004d2340(void *pOut, void *pInOut, int nI);
int FUN_004df030(void *pOut, void *pInOut, int nI);
int FUN_004d34d0(void *pOut, void *pInOut, int nI);
int FUN_004cff20(void *pOut, void *pInOut, int nI);
int FUN_004d0290(void *pOut, void *pInOut, int nI);
int FUN_004d0520(void *pOut, void *pInOut, int nI);
int FUN_004d05b0(void *pOut, void *pInOut, int nI);
int FUN_004d15f0(void *pOut, void *pInOut, int nI);
int FUN_004d1000(void *pOut, void *pInOut, int nI);
int FUN_004d0640(void *pOut, void *pInOut, int nI);
int FUN_004d0fe0(void *pOut, void *pInOut, int nI);
int FUN_004d0fc0(void *pOut, void *pInOut, int nI);
int FUN_004d1630(void *pOut, void *pInOut, int nI);
int FUN_004d34e0(void *pOut, void *pInOut, int nI);
int FUN_004dd150(void *pOut, void *pInOut, int nI);
int FUN_004dd2e0(void *pOut, void *pInOut, int nI);
int FUN_004dd660(void *pOut, void *pInOut, int nI);
int FUN_004d3450(void *pOut, void *pInOut, int nI);

// MATCH: 0x004c8e50 FUN_004c8e50
int FUN_004c8e50(void *out, void *inOut, int numStandardsFunctions)
{
  RwStandard standards[] = {
    {0x1, FUN_004ca160},
    {0xa, FUN_004cab30},
    {0x15, FUN_004c9290},
    {0x14, FUN_004cac10},
    {0x2, FUN_004ddd60},
    {0x3, FUN_004ddfb0},
    {0x7, FUN_004df120},
    {0x6, FUN_004de1b0},
    {0x5, FUN_004d3090},
    {0x4, FUN_004d2340},
    {0x9, FUN_004df030},
    {0x8, FUN_004d34d0},
    {0xf, FUN_004cff20},
    {0x10, FUN_004d0290},
    {0x17, FUN_004d0520},
    {0x18, FUN_004d05b0},
    {0xe, FUN_004d15f0},
    {0xd, FUN_004d1000},
    {0x11, FUN_004d0640},
    {0x12, FUN_004d0fe0},
    {0x13, FUN_004d0fc0},
    {0xb, FUN_004d1630},
    {0xc, FUN_004d34e0},
    {0x19, FUN_004dd150},
    {0x1b, FUN_004dd2e0},
    {0x1a, FUN_004dd660},
    {0x1c, FUN_004d3450},
  };
  int i;
  int numDriverFunctions;
  RwStandardFunc *standardFunctions;

  standardFunctions = (RwStandardFunc *)out;
  numDriverFunctions = sizeof(standards) / sizeof(RwStandard);
  for (i = 0; i < numStandardsFunctions; i++) {
    standardFunctions[i] = FUN_005c9d00;
  }
  while (numDriverFunctions--) {
    if (standards->nStandard < numStandardsFunctions && standards->nStandard >= 0) {
      standardFunctions[standards[numDriverFunctions].nStandard] = standards[numDriverFunctions].fpStandard;
    }
  }
  return 1;
}
