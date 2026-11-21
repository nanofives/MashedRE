// FLAGS: /O2 /Ob1 /G5
/* MATCH-S27 h1: 42.7% (regs 48.8%, permuter best (240 s). Not classified beyond structure/register differences (large function, budget spent). Class: b. */
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
typedef unsigned char undefined;
typedef unsigned char undefined1;
typedef unsigned int undefined4;
typedef unsigned int uint;
typedef unsigned short ushort;
typedef unsigned char byte;
#ifdef __cplusplus
typedef int code(...);
#else
typedef int code();
#endif
typedef void *HANDLE;
typedef int BOOL;
typedef unsigned long DWORD;
typedef void *LPVOID;
typedef long LONG;
typedef struct { unsigned char b[16]; } IID;
MGLOBAL(undefined1, DAT_005d098c, 0x005d098c);
MGLOBAL(undefined1, DAT_005e6424, 0x005e6424);
MGLOBAL(undefined1, FUN_005bb380, 0x005bb380);
__declspec(dllimport) BOOL __stdcall CloseHandle(HANDLE p0);
#ifdef __cplusplus
void FUN_004522d0(...);
#else
void FUN_004522d0(void);
#endif
#ifdef __cplusplus
undefined4 FUN_005aea50(...);
#else
undefined4 FUN_005aea50(void);
#endif
uint FUN_005aeea0(undefined4 * p0, LONG p1, LONG p2);
void FUN_005aef00(undefined4 * p0, undefined4 p1, undefined4 p2, undefined4 p3);
undefined4 * FUN_005aef30(undefined4 * p0, undefined4 p1);
undefined4 FUN_005bbdb0(int * p0, int * p1, int p2, undefined4 p3, uint p4, undefined4 * p5);
void FUN_005bbf30(int * p0);
LPVOID * FUN_005bc470(IID * p0, undefined4 p1, LPVOID * p2);
void FUN_005bc640(int * p0);
__declspec(dllimport) DWORD __stdcall WaitForSingleObject(HANDLE p0, DWORD p1);
/* PORT-TOP-AUDIO-5BA1D0-STOPS: body rewritten from the listing of MASHED.exe.unpatched 0x005bb000..0x005bb372. The Ghidra
   form cleared the first lock region from ptr+4 (0x005bb216 is a plain rep stosd from the pointer), read +0xec before the
   store at 0x005bb127 (the block's third dword is the new value, 0x005bb12d), and called the buffer methods cdecl (they are
   __stdcall: no add esp after 0x005bb1e9 / 0x005bb260 / 0x005bb271 / 0x005bb2b2 / 0x005bb32d). Field offsets below are from
   esi = param_1 + 0x28 (0x005bb013). */
typedef int (__stdcall *PFN1_005bb000)(int *);
typedef int (__stdcall *PFN4_005bb000)(int *, int, int, int);
typedef int (__stdcall *PFN5_005bb000)(int *, void *, uint, void *, uint);
typedef int (__stdcall *PFN8_005bb000)(int *, int, uint, void **, uint *, void **, uint *, int);
#define F_005bb000(o, T) (*(T *)((char *)b + (o)))
// MATCH: 0x005bb000 FUN_005bb000
void FUN_005bb000(undefined4 *param_1,undefined4 *param_2)

{
  char *b;
  int **buf;
  uint n;
  uint sz;
  int mem;
  uint fl;
  uint i;
  byte *q;
  uint len2;
  uint len1;
  void *ptr1;
  struct { undefined4 a; undefined *p; uint c; byte d0; byte d1; ushort d2; undefined4 e; undefined4 f;
           byte g0; byte g1; ushort g2; } blk;   /* [esp+0x1c] (0x005bb16a .. 0x005bb14b) */

  b = (char *)(param_1 + 10);
  buf = (int **)(b + 0x74);
  if (param_2 == (undefined4 *)0x0) {
    if (*buf != 0) {
      F_005bb000(0x50, uint) = F_005bb000(0x50, uint) | 0x200;
      WaitForSingleObject(F_005bb000(0xcc, HANDLE),0xffffffff);
      F_005bb000(0x50, uint) = F_005bb000(0x50, uint) & 0xfffffdff;
      if ((F_005bb000(0x50, byte) & 8) != 0) {
        FUN_005bc640((int *)(b + 0x80));
      }
      FUN_005bbf30((int *)buf);
      *buf = 0;
      F_005bb000(0x78, int) = 0;
      F_005bb000(0x7c, int) = 0;
      if (F_005bb000(0xe4, int) != 0) {
        FUN_004522d0(F_005bb000(0xe4, int));
        F_005bb000(0xe4, int) = 0;
      }
      CloseHandle(F_005bb000(0xcc, HANDLE));
    }
    F_005bb000(0xb8, int) = 0;
    F_005bb000(0xbc, int) = 0;
    F_005bb000(0xc0, int) = 0;
    return;
  }
  if ((F_005bb000(0x50, byte) & 8) != 0) {
    sz = (((uint)(F_005bb000(2, ushort) >> 3) * F_005bb000(4, uint) * 0x140) / 1000) >> 1;   /* 0x005bb0b4..0x005bb0d6 */
    n = param_2[1];
    if (n < sz) {
      sz = ((sz + n) / n) * n;
      mem = FUN_005aea50(1,sz,0x3080c);
      F_005bb000(0xe4, int) = mem;
      if (mem == 0) {
        return;
      }
      F_005bb000(0xf0, uint) = sz;
      F_005bb000(0xe8, int) = 0;
      F_005bb000(0xec, int) = 0;
    }
  }
  F_005bb000(0xc4, uint) = param_2[1] * 2;
  blk.d0 = F_005bb000(2, byte);
  blk.c = F_005bb000(0xc4, uint);
  blk.a = F_005bb000(4, undefined4);
  F_005bb000(0xc8, int) = 0;
  fl = 0x88;
  blk.g0 = 0;
  blk.g1 = 0;
  blk.e = 0;
  blk.f = 0;
  blk.p = &DAT_005e6424;
  blk.d1 = 2;
  if ((F_005bb000(0x50, uint) & 0x400) != 0) {
    fl = 0x8088;
  }
  if (FUN_005bbdb0(F_005bb000(0x54, int *),(int *)&blk,0,0,fl,(undefined4 *)buf) == 0) goto free_mem;
  if ((F_005bb000(0x50, byte) & 8) != 0) {
    blk.c = blk.c >> 1;
    if (FUN_005bc470((IID *)&DAT_005d098c,(undefined4)&blk,(LPVOID *)(b + 0x80)) == 0) goto release;
  }
  /* 0x005bb1c6: Lock(buf, 0, +0xc4, &ptr1, &len1, &ptr2, &len2, 0); ptr2 lives in the param_1 slot ([esp+0x3c]) */
  if ((*(PFN8_005bb000 *)(**buf + 0x2c))(*buf,0,F_005bb000(0xc4, uint),&ptr1,&len1,(void **)&param_1,&len2,0) < 0)
    goto notify;
  if ((ptr1 != (void *)0x0) && (len1 != 0)) {
    q = (byte *)ptr1;
    for (i = len1; i != 0; i = i - 1) { *q = 0; q = q + 1; }
  }
  if ((param_1 != (undefined4 *)0x0) && (len2 != 0)) {
    q = (byte *)param_1;
    for (i = len2; i != 0; i = i - 1) { *q = 0; q = q + 1; }
  }
  if ((*(PFN5_005bb000 *)(**buf + 0x4c))(*buf,ptr1,len1,param_1,len2) < 0) goto notify;
  if ((*(PFN4_005bb000 *)(**buf + 0x30))(*buf,0,0,1) < 0) goto notify;
  F_005bb000(0xb8, undefined4) = param_2[0];
  F_005bb000(0xbc, undefined4) = param_2[1];
  F_005bb000(0xc0, undefined4) = param_2[2];
  if (FUN_005aeea0((undefined4 *)(b + 0xcc),0,1) != 0) {
    FUN_005aef00((undefined4 *)(b + 0xd0),(undefined4)&FUN_005bb380,0xf,0x1000);
    if (FUN_005aef30((undefined4 *)(b + 0xd0),(undefined4)b) != 0) {
      return;
    }
    CloseHandle(F_005bb000(0xcc, HANDLE));
    (*(PFN1_005bb000 *)(**buf + 0x48))(*buf);
    if ((F_005bb000(0x50, byte) & 8) != 0) {
      FUN_005bc640((int *)(b + 0x80));
    }
    FUN_005bbf30((int *)buf);
    if (F_005bb000(0xe4, int) != 0) {
      FUN_004522d0(F_005bb000(0xe4, int));
      F_005bb000(0xe4, int) = 0;
    }
    return;
  }
  (*(PFN1_005bb000 *)(**buf + 0x48))(*buf);
notify:
  if ((F_005bb000(0x50, byte) & 8) != 0) {
    FUN_005bc640((int *)(b + 0x80));
  }
release:
  FUN_005bbf30((int *)buf);
free_mem:
  if (F_005bb000(0xe4, int) != 0) {
    FUN_004522d0(F_005bb000(0xe4, int));
    F_005bb000(0xe4, int) = 0;
  }
  return;
}
