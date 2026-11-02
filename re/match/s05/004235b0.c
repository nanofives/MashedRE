// FLAGS: /O2 /GS /Ob1
/* MATCH-S05 0x004235b0: raw g2s.py output (re/tools/match/g2s.py on decomp_pc.py --port --callees), body
   unchanged; globals moved to MGLOBAL. Score: see verify/matchs05_20261031/results.tsv. */
/* ---- MATCH-S05 preamble (same text in every re/match/s05 source; not part of any function body) ----
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
MGLOBAL(undefined1, DAT_00644110, 0x00644110);
MGLOBAL(undefined4, DAT_007f1a9c, 0x007f1a9c);
undefined1 * FUN_00423480(void);
void FUN_00495280(const char *p0);
void FUN_004952f0(void);
uint FUN_004cbd30(int p0, void * p1, uint p2);
bool FUN_004cc160(int p0, int p1);
int FUN_004cc230(undefined4 p0, int p1, void * p2);
undefined4 FUN_004cc5e0(int p0, int p1, undefined4 * p2, void * p3);
// MATCH: 0x004235b0 FUN_004235b0
undefined4 FUN_004235b0(void)

{
  int iVar1;
  int iVar2;
  undefined4 uVar3;
  undefined4 uStack_8;
  undefined1 auStack_4 [4];
  
  uVar3 = 0;
  FUN_00423480();
  FUN_00495280("d:\\ToastArt\\Common\\ai.piz");
  iVar1 = FUN_004cc230(2,1,&DAT_00644110);
  if (iVar1 != 0) {
    iVar2 = FUN_004cc5e0(iVar1,0x13269902,&uStack_8,auStack_4);
    if (iVar2 != 0) {
      FUN_004cbd30(iVar1,&DAT_007f1a9c,uStack_8);
      FUN_004cc160(iVar1,0);
      uVar3 = 1;
    }
  }
  FUN_004952f0();
  return uVar3;
}
