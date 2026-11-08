// FLAGS: /O2 /Ob1 /G5
/* MATCH-S27 0x005b2b00: 42.3% (regs 67.8%). Hand: file did not compile (LPLONG); rewritten with Obj/Ent structs (entries 0x14, handle +0x18c, flags +0x188), same control flow as the decompile. Residual (b): original keeps a stack local (push ecx) for the return code and hoists *(f118+0x20) into ebp/[esp+0x18], different register allocation; 226 vs 239 ins. Not iterated further (budget). */
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
typedef void *HANDLE;
typedef int BOOL;
typedef unsigned long DWORD;
typedef long LONG;
typedef struct Ent { uint f0; int f4; uint f8; uint fc; uint f10; } Ent;
typedef struct Obj {
  char p0[0x8c]; char sub[0x60];
  int fec; uint ff0; uint ff4; uint ff8; char pfc[4]; int f100; char p104[0x14];
  char *f118; char p11c[0x20];
  uint f13c; char p140[4]; int f144; Ent *f148; uint f14c; char p150[4]; uint f154; char p158[8];
  uint f160; int f164; char p168[0x18];
  uint f180; int f184; uint flags; HANDLE h;
} Obj;
undefined4 FUN_005b30e0(Obj *p0, uint p1, uint p2);
undefined4 FUN_005b8090(void *p0, int p1, uint p2, int p3, int p4, int p5);
__declspec(dllimport) BOOL __stdcall ReleaseSemaphore(HANDLE p0, LONG p1, LONG *p2);
__declspec(dllimport) DWORD __stdcall WaitForSingleObject(HANDLE p0, DWORD p1);
// MATCH: 0x005b2b00 FUN_005b2b00
undefined4 FUN_005b2b00(Obj *o)
{
  Ent *e;
  int iVar2;
  uint uVar3;
  int iVar4;
  int iVar5;
  uint uVar6;
  undefined4 local_4;

  WaitForSingleObject(o->h, 0xffffffff);
  uVar6 = o->f14c;
  iVar2 = o->f148[uVar6].f10;
  while (1) {
    if (iVar2 != 1) {
      ReleaseSemaphore(o->h, 1, (LONG *)0);
      return 3;
    }
    iVar2 = *(int *)(o->f118 + 0x20);
    if (uVar6 == o->f154) {
      uVar6 = o->f164;
    } else if (o->f154 < uVar6) {
      uVar6 = (o->f164 - uVar6) + o->f154;
    } else {
      uVar6 = o->f154 - uVar6;
    }
    if (uVar6 < (uint)(o->f164 - o->f184)) {
      ReleaseSemaphore(o->h, 1, (LONG *)0);
      return 3;
    }
    if (o->ff8 != 0) {
      local_4 = 1;
      uVar3 = o->f180;
      if (uVar6 < o->f180) {
        uVar3 = uVar6;
      }
      if (o->ff8 < uVar3) {
        uVar3 = o->ff8;
      }
      iVar5 = o->f14c * iVar2;
      uVar3 = uVar3 * iVar2;
      uVar6 = o->f160 - iVar5;
      if (uVar3 < uVar6) {
        uVar6 = uVar3;
      }
      iVar4 = uVar3 - uVar6;
      uVar3 = 0;
      if (uVar6 != 0 && (iVar5 = FUN_005b8090(o->sub, o->f144 + iVar5, uVar6, 0, 0, 4), uVar3 = uVar6, iVar5 == 0)) {
        ReleaseSemaphore(o->h, 1, (LONG *)0);
        return 2;
      }
      if (iVar4 != 0) {
        iVar5 = FUN_005b8090(o->sub, o->f144, iVar4, 0, 0, 4);
        if (iVar5 == 0) {
          local_4 = 2;
        } else {
          uVar3 = uVar3 + iVar4;
        }
      }
      while (uVar3 != 0) {
        e = o->f148 + o->f14c;
        e->f4 = o->f100 - o->ff8;
        e->f0 = o->f13c;
        e->f10 = 8;
        if (o->ff0 == o->ff4) {
          e->fc = 0;
        } else {
          e->fc = o->ff4;
          o->ff0 = o->ff4;
        }
        uVar3 = uVar3 - iVar2;
        e->f8 = o->ff0;
        o->ff8 = o->ff8 - 1;
        iVar5 = o->f14c + 1;
        if (iVar5 == o->f164) {
          iVar5 = 0;
        }
        o->f14c = iVar5;
      }
      if (o->ff8 == 0 && o->fec != -1 && (o->flags & 0x4000) != 0) {
        iVar2 = o->f14c;
        if (iVar2 == 0) {
          iVar2 = o->f164;
        }
        o->f148[iVar2 - 1].f10 = o->f148[iVar2 - 1].f10 | 0x10;
      }
      ReleaseSemaphore(o->h, 1, (LONG *)0);
      return local_4;
    }
    ReleaseSemaphore(o->h, 1, (LONG *)0);
    if ((o->flags & 0x4000) == 0) {
      return 4;
    }
    iVar2 = FUN_005b30e0(o, 0, 0);
    if (iVar2 == 0) break;
    WaitForSingleObject(o->h, 0xffffffff);
    uVar6 = o->f14c;
    iVar2 = o->f148[uVar6].f10;
  }
  return 0;
}
