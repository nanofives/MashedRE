// FLAGS: /O2 /Ob1 /G5
/* MATCH-S28 0x005bf8f0: 40.5% (regs 69.0%). Goto-structured COM routine (QI, vtbl+0xc/0x10/0x14 stdcall calls, cdecl this+0x144 hook, 16-byte buf). Frame size now matches (0x24 locals, buf[16]). Orig keeps the 0..1 loop counter in the arg1 stack slot and never uses ebp; ours enregisters it in ebp. Class: b. */
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
typedef int (__stdcall *qi_fn)(int *, void *, void *);
typedef int (__stdcall *m2_fn)(int *, void *);
typedef int (__stdcall *m3_fn)(int *, void *, void *);
typedef int (__stdcall *m4_fn)(int *, int *, int);
typedef int (__stdcall *rel_fn)(int *);
typedef int (__cdecl *hook_fn)(int, int *, void *, void *);
MGLOBAL(undefined4, DAT_005d0b1c, 0x005d0b1c);
int *FUN_005c0e00(int p0, int p1);
// MATCH: 0x005bf8f0 FUN_005bf8f0
int FUN_005bf8f0(int param_1, int *param_2)
{
  int *puVar1;
  int *piVar2;
  int iVar3;
  int *piVar4;
  int x;
  int v[4];
  char buf[16];
  unsigned int i;

  puVar1 = (int *)(param_1 + 0x140);
  x = 0;
  iVar3 = ((qi_fn)*(int *)*param_2)(param_2, &DAT_005d0b1c, puVar1);
  if (-1 < iVar3) {
    v[0] = 0; v[1] = 0; v[2] = 0; v[3] = 0;
    i = 0;
    do {
      piVar2 = (int *)(param_1 + 0x138);
      if (i == 0) {
        iVar3 = ((m2_fn)*(int *)(*(int *)*puVar1 + 0xc))((int *)*puVar1, piVar2);
        if (-1 < iVar3) goto LAB_005bf966;
        piVar4 = (int *)*piVar2;
        if (piVar4 != (int *)0) goto LAB_005bf9c0;
      }
      else {
        piVar4 = FUN_005c0e00(0, 0);
        *piVar2 = (int)piVar4;
LAB_005bf966:
        if (*piVar2 != 0) {
          iVar3 = ((hook_fn)*(int *)(param_1 + 0x144))(param_1 + 4, (int *)*piVar2, v, &x);
          if (iVar3 != 0) {
            iVar3 = ((m3_fn)*(int *)(*(int *)*piVar2 + 0xc))((int *)*piVar2, v, buf);
            if (iVar3 < 0) {
              ((rel_fn)*(int *)(**(int **)(param_1 + 0x138) + 8))(*(int **)(param_1 + 0x138));
              *(int *)(param_1 + 0x138) = 0;
              return 0;
            }
            iVar3 = ((m4_fn)*(int *)(*(int *)*puVar1 + 0x10))((int *)*puVar1, (int *)*piVar2, x);
            if ((-1 < iVar3) &&
               (iVar3 = ((rel_fn)*(int *)(*(int *)*piVar2 + 0x14))((int *)*piVar2), -1 < iVar3)) {
              return 1;
            }
          }
          piVar4 = (int *)*piVar2;
LAB_005bf9c0:
          ((rel_fn)*(int *)(*piVar4 + 8))(piVar4);
          *piVar2 = 0;
        }
      }
      i = i + 1;
    } while (i < 2);
  }
  return 0;
}

