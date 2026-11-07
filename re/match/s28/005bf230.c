// FLAGS: /O2 /Ob1 /G5
/* MATCH-S28 h1 0x005bf230 : 100.0% (regs 100). By hand from raw: cdecl 5 args (param_1..param_5; raw showed in_stack/unaff regs), size default 0x340, builds the object from FUN_005beae0 + FUN_005bfd10, vtbl[0] stdcall QueryInterface, string globals via MGLOBALA, the hr test must be iVar4 >= 0 (test/jl, not cmp -1). Residual: none. */

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

MGLOBAL(unsigned int, DAT_005d0bac, 0x005d0bac);
MGLOBAL(unsigned char, DAT_005e7160, 0x005e7160);
MGLOBALA(char, u_RwaDSRenderer_0063581c, 14, 0x0063581c);
MGLOBAL(unsigned char *, PTR_FUN_00635970, 0x00635970);
MGLOBAL(unsigned char *, PTR_LAB_006359b0, 0x006359b0);
MGLOBAL(unsigned char *, PTR_LAB_006359f8, 0x006359f8);
MGLOBALA(char, u_Input_00635a1c, 6, 0x00635a1c);
typedef int (__stdcall *QIFN)(int *, void *, void *);
typedef int (__stdcall *RFN)(int *);
int FUN_005bc920(int *p0, int p1);
int *FUN_005beae0(int p0, int p1);
int *FUN_005bfd10(int *p0, int *p1, int p2);
short *FUN_005c2f0a(short *p0, short *p1);
// MATCH: 0x005bf230 FUN_005bf230
int *FUN_005bf230(int param_1, int param_2, int param_3, int param_4, int param_5)
{
  int *puVar1;
  int *puVar2;
  int *piVar3;
  int iVar4;

  if (param_5 == 0) {
    param_5 = 0x340;
  }
  puVar2 = FUN_005beae0(param_4, param_5);
  if (puVar2 != (int *)0) {
    piVar3 = FUN_005bfd10(puVar2, puVar2 + 0x56, 0);
    if (piVar3 != (int *)0) {
      puVar1 = puVar2 + 0xb4;
      iVar4 = ((QIFN)(*(int **)piVar3)[0])(piVar3, &DAT_005d0bac, puVar1);
      if (iVar4 >= 0) {
        puVar2[0x56] = (int)&PTR_LAB_006359f8;
        puVar2[0x57] = (int)&PTR_LAB_006359b0;
        FUN_005c2f0a((short *)(puVar2 + 0x5c), (short *)u_Input_00635a1c);
        *puVar2 = (int)&PTR_FUN_00635970;
        puVar2[199] = param_2;
        puVar2[200] = param_3;
        iVar4 = FUN_005bc920(puVar2 + 0xb5, param_1);
        if (iVar4 != 0) {
          puVar2[0x9f] = 1;
          puVar2[0x48] = 1;
          puVar2[0x9e] = (int)(puVar2 + 0xb5);
          puVar2[0xcf] = 0;
          puVar2[0x47] = (int)puVar1;
          puVar2[4] = (int)&DAT_005e7160;
          FUN_005c2f0a((short *)(puVar2 + 6), (short *)u_RwaDSRenderer_0063581c);
          ((RFN)(*(int **)piVar3)[2])(piVar3);
          return puVar2;
        }
        ((RFN)(*(int **)*puVar1)[2])((int *)*puVar1);
      }
      ((RFN)(*(int **)piVar3)[2])(piVar3);
    }
  }
  return (int *)0;
}

