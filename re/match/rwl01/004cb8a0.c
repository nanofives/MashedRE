// FLAGS: /O2 /Ob1 /G5
/* RWC-DRIVER round 2: no change. Tried a walking item pointer (68.3%), &table[n] local (71.3%), operand order (70.7%), sizeof-form count (71.3%). */
/* RWC-DRIVER 2026-11-11: 43.3% -> 71.3% (regs 87.8%, ops 97.0%, 164/164 ins). Moved by: 8-byte element / 12-byte cache-item structs, int return, every COM call __stdcall (+4 and +0x158 included; the raw cdecl typing was wrong), the dword compare as a static __inline helper (gives the original's param_1-walking loop and the count reload), 'if (n >= count) {grow, create} else {found}' block order, the found arm as 'decl != 0 ? +4 : +0x158 create'. Residual: register choice when a global table pointer is loaded (original loads it into a scratch register and lea's into another; ours adds in place) and the load order of count/table in the creation stores; permuter 536 compiles found only a store reorder that changes the original's store order (rejected). */
/* MATCH-RWL01 0x004cb8a0: best 43.3% (regs 50%), class (b). Vertex-declaration cache lookup: typed cdecl/stdcall COM-slot prototypes (the original mixes both: found-path calls pop their args, the CreateDecl call does not), memcpy intrinsic for the copy. The original keeps param_1 in ebp and re-reads the cache count into ebx after the compare loop; ours keeps param_1 in esi. A CE-struct rewrite scored lower (25%); permuter variants moved the count increment before the memcpy (wrong), discarded. */
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
MGLOBAL(uint, DAT_007d4144, 0x007d4144);
typedef struct DeclElem { unsigned short stream; unsigned short offset; unsigned char type; unsigned char method; unsigned char usage; unsigned char usageIndex; } DeclElem;
typedef struct DeclItem { int *decl; uint num; DeclElem *elems; } DeclItem;
MGLOBAL(DeclItem *, DAT_007d414c, 0x007d414c);
MGLOBAL(uint, DAT_007d4148, 0x007d4148);
MGLOBAL(int, DAT_007d3ff8, 0x007d3ff8);
MGLOBAL(int *, DAT_007d4110, 0x007d4110);
void *memcpy(void *, const void *, unsigned int);
#pragma intrinsic(memcpy)
typedef int (__cdecl *PFN_CDECL3)(int *, int, int);
typedef int (__cdecl *PFN_CDECL1)(int *);
typedef int (__stdcall *PFN_CREATEDECL)(int *, int *, int **);
typedef int (__stdcall *PFN_ADDREF)(int *);
typedef int (__cdecl *PFN_ALLOC1)(uint);
typedef int (__cdecl *PFN_ALLOC2)(uint, uint);
typedef int (__cdecl *PFN_REALLOC)(int, uint, uint);
static __inline int s_cmp(const uint *a, const uint *b, uint n)
{
  while (n != 0) {
    if (*a != *b) return 1;
    a++;
    b++;
    n--;
  }
  return 0;
}
// MATCH: 0x004cb8a0 FUN_004cb8a0
int FUN_004cb8a0(DeclElem *param_1,int **param_2)
{
  uint num;
  uint n;
  uint cnt;
  DeclElem *e;
  uint *a;
  uint *b;

  num = 0;
  e = param_1;
  while (e->type != 0x11) {
    e++;
    num++;
  }
  num++;
  for (n = 0; n < DAT_007d4144; n++) {
    if (DAT_007d414c[n].num == num && s_cmp((uint *)DAT_007d414c[n].elems, (uint *)param_1, (num * 8) >> 2) == 0) break;
  }
  if (n >= DAT_007d4144) {
    if (DAT_007d4144 >= DAT_007d4148) {
      DAT_007d4148 = DAT_007d4148 + 0x10;
      if (DAT_007d414c == 0) {
        DAT_007d414c = (DeclItem *)(*(PFN_ALLOC2)*(int *)(DAT_007d3ff8 + 0x108))(DAT_007d4148 * 0xc,0x1040411);
      }
      else {
        DAT_007d414c = (DeclItem *)(*(PFN_REALLOC)*(int *)(DAT_007d3ff8 + 0x110))((int)DAT_007d414c,DAT_007d4148 * 0xc,0x1040411);
      }
    }
    if ((*(PFN_CREATEDECL *)(*DAT_007d4110 + 0x158))(DAT_007d4110,(int *)param_1,param_2) >= 0) {
      DAT_007d414c[DAT_007d4144].decl = *param_2;
      DAT_007d414c[DAT_007d4144].num = num;
      DAT_007d414c[DAT_007d4144].elems = (DeclElem *)(*(PFN_ALLOC2)*(int *)(DAT_007d3ff8 + 0x108))(num * 8,0x40411);
      memcpy(DAT_007d414c[DAT_007d4144].elems,param_1,num * 8);
      DAT_007d4144 = DAT_007d4144 + 1;
    }
    else {
      *param_2 = 0;
    }
    }
  else {
    if (DAT_007d414c[n].decl != 0) {
      (*(PFN_ADDREF *)(*DAT_007d414c[n].decl + 4))(DAT_007d414c[n].decl);
    }
    else {
      (*(PFN_CREATEDECL *)(*DAT_007d4110 + 0x158))(DAT_007d4110,(int *)DAT_007d414c[n].elems,&DAT_007d414c[n].decl);
    }
    *param_2 = DAT_007d414c[n].decl;
    }
  return *param_2 != 0;
}
