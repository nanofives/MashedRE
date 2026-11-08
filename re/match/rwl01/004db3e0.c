// FLAGS: /O2 /Ob1 /G5
/* MATCH-RWL01 0x004db3e0: best 95.2% (regs 99%, ops 99%), class (a). Written from the listing with real vtable call prototypes (IDirect3DDevice9-style slots 0x10/0x68/0x6c, 7-word CreateBuffer-style calls) and a 4-entry D3DVERTEXELEMENT9 array. Remaining: the two Release calls load the vtable into edx/ecx in the opposite order, and one trailing return value (original returns esi, ours returns 0 via xor). Dropping the explicit return 0 changed the block layout and scored lower (93.3%). */
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
typedef int code();
MGLOBAL(uint, DAT_00911fbc, 0x00911fbc);
MGLOBAL(int, DAT_007d7098, 0x007d7098);
MGLOBAL(int *, DAT_007d4110, 0x007d4110);
MGLOBAL(undefined4, DAT_007d709c, 0x007d709c);
MGLOBAL(undefined4, DAT_007d70a4, 0x007d70a4);
MGLOBAL(int, DAT_007d70ac, 0x007d70ac);
MGLOBAL(int *, DAT_007d70a0, 0x007d70a0);
MGLOBAL(int *, DAT_007d70a8, 0x007d70a8);
typedef struct VE { ushort Stream; ushort Offset; byte Type; byte Method; byte Usage; byte UsageIndex; } VE;
typedef uint (__stdcall *PFN_GETMEM)(int *);
typedef int (__stdcall *PFN_CREATE)(int *, uint, uint, uint, uint, int **, uint);
typedef uint (__stdcall *PFN_RELEASE)(int *);
int FUN_004cb8a0(VE * p0, int * p1);
void FUN_004cba80(int p0);
// MATCH: 0x004db3e0 FUN_004db3e0
undefined4 FUN_004db3e0(void)
{
  uint uVar1;
  int iVar2;
  undefined4 uVar3;
  VE ve[4];
  uVar3 = 0;
  DAT_007d7098 = 0;
  if ((DAT_00911fbc & 0x10000) == 0) {
    uVar1 = (*(PFN_GETMEM *)(*DAT_007d4110 + 0x10))(DAT_007d4110);
    if (0x800000 < uVar1) {
      iVar2 = (*(PFN_CREATE *)(*DAT_007d4110 + 0x68))(DAT_007d4110,0x40000,0x228,0,0,&DAT_007d70a0,0);
      if (iVar2 == 0) {
        DAT_007d7098 = 1;
        DAT_007d709c = 0;
      }
    }
  }
  DAT_007d70a4 = 0;
  iVar2 = (*(PFN_CREATE *)(*DAT_007d4110 + 0x6c))(DAT_007d4110,20000,0x228,0x65,0,&DAT_007d70a8,0);
  if (iVar2 == 0) {
    uVar3 = 1;
    ve[0].Stream = 0; ve[0].Offset = 0; ve[0].Type = 3; ve[0].Method = 0; ve[0].Usage = 9; ve[0].UsageIndex = 0;
    ve[1].Stream = 0; ve[1].Offset = 0x10; ve[1].Type = 4; ve[1].Method = 0; ve[1].Usage = 10; ve[1].UsageIndex = 0;
    ve[2].Stream = 0; ve[2].Offset = 0x14; ve[2].Type = 1; ve[2].Method = 0; ve[2].Usage = 5; ve[2].UsageIndex = 0;
    ve[3].Stream = 0xff; ve[3].Offset = 0; ve[3].Type = 0x11; ve[3].Method = 0; ve[3].Usage = 0; ve[3].UsageIndex = 0;
    FUN_004cb8a0(ve,&DAT_007d70ac);
  }
  else {
    if (DAT_007d70ac != 0) {
      FUN_004cba80(DAT_007d70ac);
      DAT_007d70ac = 0;
    }
    if ((DAT_007d7098 != 0) && (DAT_007d70a0 != (int *)0x0)) {
      (*(PFN_RELEASE *)(*DAT_007d70a0 + 8))(DAT_007d70a0);
      DAT_007d70a0 = (int *)0x0;
    }
    if (DAT_007d70a8 != (int *)0x0) {
      (*(PFN_RELEASE *)(*DAT_007d70a8 + 8))(DAT_007d70a8);
      DAT_007d70a8 = (int *)0x0;
      return 0;
    }
  }
  return uVar3;
}
