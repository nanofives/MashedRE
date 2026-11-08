// FLAGS: /O2 /Ob1 /G5
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
MGLOBAL(uint, DAT_00911fbc, 0x00911fbc);
MGLOBAL(int *, DAT_007d4110, 0x007d4110);
MGLOBAL(undefined4, DAT_007d7108, 0x007d7108);
MGLOBAL(int *, DAT_007d710c, 0x007d710c);
MGLOBAL(int, DAT_007d7118, 0x007d7118);
MGLOBAL(int, DAT_007d7114, 0x007d7114);
MGLOBAL(int, DAT_007d7110, 0x007d7110);
bool FUN_004cb8a0(int * p0, int * p1);
void FUN_004cba80(int p0);
// MATCH: 0x004e0920 FUN_004e0920
undefined4 FUN_004e0920(void)

{
  int iVar1;
  undefined4 uStack_44;
  undefined4 uStack_40;
  uint uStack_3c;
  undefined4 *puStack_38;
  undefined4 uStack_34;
  
  DAT_007d7108 = 0;
  iVar1 = (*(int (__stdcall *)(int *,int,int,int,uint,int *,int))*(int *)(*DAT_007d4110 + 0x6c))
                    (DAT_007d4110,0x1fffe,0x208,0x65,~DAT_00911fbc >> 0xf & 2,(int *)&DAT_007d710c,0);
  if (iVar1 == 0) {
    uStack_44 = 0;
    uStack_40 = 2;
    uStack_3c = 0xc0000;
    puStack_38 = (undefined4 *)0x30002;
    uStack_34 = 0x180000;
    FUN_004cb8a0((int *)&uStack_44,(int *)&DAT_007d7110);
    uStack_44 = 0;
    uStack_40 = 2;
    uStack_3c = 0xc0000;
    puStack_38 = (undefined4 *)0xa0004;
    uStack_34 = 0x100000;
    FUN_004cb8a0((int *)&uStack_44,(int *)&DAT_007d7114);
    uStack_44 = 0;
    uStack_40 = 2;
    uStack_3c = 0xc0000;
    puStack_38 = (undefined4 *)0xa0004;
    uStack_34 = 0xff;
    FUN_004cb8a0((int *)&uStack_44,(int *)&DAT_007d7118);
    return 1;
  }
  if (DAT_007d710c != (int *)0x0) {
    (*(int (__stdcall *)(int *))*(int *)(*DAT_007d710c + 8))(DAT_007d710c);
    DAT_007d710c = (int *)0x0;
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
  return 0;
}
