// FLAGS: /O2 /Ob1 /G5
/* MATCH-S27 0x005bb910: RO-AUDIOINIT-45D460-REACH (2026-11-21), new source from the listing (no Ghidra function, no caller: 0x005bb850 stores its address at +0xc, 0x005bb88d). */
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
/* RO-AUDIOINIT-45D460-REACH (2026-11-21): new source from the listing of MASHED.exe.unpatched 0x005bb910..0x005bba26 (no Ghidra
   function, no caller: 0x005bb850 stores its address at +0xc, 0x005bb890; 0x005aba20 calls it). param_1 = the voice record
   (esi), param_2's +0x20 = the device record (ebp). Import [0x5cc0e0] = KERNEL32 GetVersionExA (OSVERSIONINFOA, 0x94 bytes). */
typedef struct { unsigned long size; unsigned long major; unsigned long minor; unsigned long build; unsigned long platform;
                 char csd[128]; } OSV_005bb910;
__declspec(dllimport) int __stdcall GetVersionExA(OSV_005bb910 *p0);
int FUN_005c91f0(int p0, int p1, int p2, uint p3, int p4);
void FUN_005a7e70(int p0);
uint FUN_005ac540(int p0);
int FUN_005bbdb0(int p0, int p1, uint p2, int p3, uint p4, int p5);
// MATCH: 0x005bb910 FUN_005bb910
int FUN_005bb910(int param_1,int param_2)

{
  int dev;
  int r;
  uint dup;
  uint fl;
  OSV_005bb910 v;

  dev = *(int *)(param_2 + 0x20);
  *(int *)(param_1 + 0x48) = *(int *)(param_1 + 0x34);
  if ((*(byte *)(dev + 0x50) & 8) != 0) {
    r = FUN_005c91f0(*(int *)(dev + 0xf4),param_1 + 0x2c,param_1 + 0x2c,*(uint *)(param_1 + 0x54) >> 1 & 1,0);   /* 0x005bb94e */
    *(int *)(param_1 + 0x4c) = r;
    FUN_005a7e70(r);
    return -(*(int *)(param_1 + 0x4c) != 0) & param_1;                    /* 0x005bb95c .. 0x005bb966 */
  }
  dup = *(uint *)(param_1 + 0x54) >> 1 & 1;
  fl = 0x100a2;
  if (FUN_005ac540(param_1) != 0) {
    v.size = 0x94;
    GetVersionExA(&v);                                                    /* 0x005bb99a */
    if ((v.platform == 1) && ((4 < v.major) || ((v.major == 4) && (v.minor != 0)))) {
      fl = 0x101a2;
    }
  }
  if ((dup == 0) || ((*(byte *)(dev + 0x50) & 0x40) != 0)) {             /* 0x005bb9c1 .. 0x005bb9cd */
    dup = 0;
    fl = fl | 0x40;
  }
  if ((*(byte *)(dev + 0x50) & 0x20) != 0) {
    fl = fl | 8;
    if ((*(byte *)(dev + 0x50) & 8) != 0) {
      fl = fl | 0x200;
    }
  }
  else {
    fl = fl | 0x40000;
  }
  if ((*(uint *)(dev + 0x50) & 0x400) != 0) {
    fl = fl | 0x8000;
  }
  *(int *)(param_1 + 0x60) = 0;
  *(int *)(param_1 + 0x5c) = 0;
  *(int *)(param_1 + 0x64) = 0;
  r = FUN_005bbdb0(*(int *)(dev + 0x54),param_1 + 0x2c,dup,1,fl,param_1 + 0x5c);   /* 0x005bba0e */
  return -(r != 0) & param_1;
}
