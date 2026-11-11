// FLAGS: /O2 /Ob1 /G5
/* RWC-DRIVER round 2: 41.6% -> 46.3%. result = 1 set at the start of the hr == 0 arm and returned there; the failure arm returns the 0 set at the top. Residual unchanged: the original holds 0 in esi before the call, 1 in ebp and 0xff in esi on the success arm; ours stores immediates. */
/* RWC-DRIVER 2026-11-11: 20.8% -> 41.6% (ops 96.6%, 146/149 ins). Rewritten from the disassembly: the element tables are a local 5 x 8-byte array ({ushort, ushort, 4 x uchar}) filled field by field (5, 4 and 3 entries, end marker stream 0xff type 0x11), +0x6c call __stdcall with the pool (~0x00911fbc >> 15) & 2 in a local computed first, result variable (0, set to 1 on the hr == 0 path) returned once. Residual: the original keeps 1 in ebp (pushed only on the success path) and 0xff in esi; ours returns the constant and stores 0xff as an immediate, so the success-path stack offsets differ by 4..0xc. */
/* h3 hand-match: best 20.8% (regs 37.6%), class b. Raw Ghidra output for the vertex-element table part is wrong (the uStack_ block is really a call's 7 stack args plus 8-byte {WORD,WORD,BYTE x4} element arrays built with byte/word stores); fixed the first IDirect3D call (7 args, __stdcall) and the COM Release, file now compiles. The element tables (0x14..0x3b off esp, 4 variants) were not rewritten: out of budget. */
/* MATCH-RWL01 0x004e0920: raw g2s.py output (re/tools/match/g2s.py on decomp_pc.py --port --callees), body
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
typedef unsigned int undefined4;
typedef unsigned int uint;
#ifdef __cplusplus
typedef int code(...);
#else
typedef int code();
#endif
typedef struct DeclElem { unsigned short stream; unsigned short offset; unsigned char type; unsigned char method; unsigned char usage; unsigned char usageIndex; } DeclElem;
typedef long (__stdcall *cvb_fn)(int *, uint, uint, uint, uint, int **, void *);
typedef long (__stdcall *rel_fn)(int *);
MGLOBAL(uint, DAT_00911fbc, 0x00911fbc);
MGLOBAL(int *, DAT_007d4110, 0x007d4110);
MGLOBAL(undefined4, DAT_007d7108, 0x007d7108);
MGLOBAL(int *, DAT_007d710c, 0x007d710c);
MGLOBAL(int *, DAT_007d7118, 0x007d7118);
MGLOBAL(int *, DAT_007d7114, 0x007d7114);
MGLOBAL(int *, DAT_007d7110, 0x007d7110);
int FUN_004cb8a0(DeclElem *p0, int **p1);
void FUN_004cba80(int *p0);
// MATCH: 0x004e0920 FUN_004e0920
int FUN_004e0920(void)
{
  DeclElem decl[5];
  int result;
  uint pool;
  long hr;

  pool = (~DAT_00911fbc >> 0xf) & 2;
  result = 0;
  DAT_007d7108 = 0;
  if ((*(cvb_fn *)(*DAT_007d4110 + 0x6c))(DAT_007d4110,0x1fffe,0x208,0x65,pool,&DAT_007d710c,0) == 0) {
    result = 1;
    decl[0].stream = 0;
    decl[0].offset = 0;
    decl[0].type = 2;
    decl[0].method = 0;
    decl[0].usage = 0;
    decl[0].usageIndex = 0;
    decl[1].stream = 0;
    decl[1].offset = 12;
    decl[1].type = 2;
    decl[1].method = 0;
    decl[1].usage = 3;
    decl[1].usageIndex = 0;
    decl[2].stream = 0;
    decl[2].offset = 24;
    decl[2].type = 4;
    decl[2].method = 0;
    decl[2].usage = 10;
    decl[2].usageIndex = 0;
    decl[3].stream = 0;
    decl[3].offset = 28;
    decl[3].type = 1;
    decl[3].method = 0;
    decl[3].usage = 5;
    decl[3].usageIndex = 0;
    decl[4].stream = 255;
    decl[4].offset = 0;
    decl[4].type = 17;
    decl[4].method = 0;
    decl[4].usage = 0;
    decl[4].usageIndex = 0;
    FUN_004cb8a0(decl,&DAT_007d7110);
    decl[0].stream = 0;
    decl[0].offset = 0;
    decl[0].type = 2;
    decl[0].method = 0;
    decl[0].usage = 0;
    decl[0].usageIndex = 0;
    decl[1].stream = 0;
    decl[1].offset = 12;
    decl[1].type = 4;
    decl[1].method = 0;
    decl[1].usage = 10;
    decl[1].usageIndex = 0;
    decl[2].stream = 0;
    decl[2].offset = 16;
    decl[2].type = 1;
    decl[2].method = 0;
    decl[2].usage = 5;
    decl[2].usageIndex = 0;
    decl[3].stream = 255;
    decl[3].offset = 0;
    decl[3].type = 17;
    decl[3].method = 0;
    decl[3].usage = 0;
    decl[3].usageIndex = 0;
    FUN_004cb8a0(decl,&DAT_007d7114);
    decl[0].stream = 0;
    decl[0].offset = 0;
    decl[0].type = 2;
    decl[0].method = 0;
    decl[0].usage = 0;
    decl[0].usageIndex = 0;
    decl[1].stream = 0;
    decl[1].offset = 12;
    decl[1].type = 4;
    decl[1].method = 0;
    decl[1].usage = 10;
    decl[1].usageIndex = 0;
    decl[2].stream = 255;
    decl[2].offset = 0;
    decl[2].type = 17;
    decl[2].method = 0;
    decl[2].usage = 0;
    decl[2].usageIndex = 0;
    FUN_004cb8a0(decl,&DAT_007d7118);
    return result;
  }
  {
  if (DAT_007d710c != 0) {
    (*(rel_fn *)(*DAT_007d710c + 8))(DAT_007d710c);
    DAT_007d710c = 0;
  }
  if (DAT_007d7118 != 0) {
    FUN_004cba80(DAT_007d7118);
    DAT_007d7118 = 0;
  }
  if (DAT_007d7114 != 0) {
    FUN_004cba80(DAT_007d7114);
    DAT_007d7114 = 0;
  }
  if (DAT_007d7110 != 0) {
    FUN_004cba80(DAT_007d7110);
    DAT_007d7110 = 0;
  }
  }
  return result;
}
