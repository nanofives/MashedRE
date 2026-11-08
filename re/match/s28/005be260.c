// FLAGS: /O2 /Ob1 /G5
// best 90.6 (ops 95.3, regs 90.6). Fixes: FUN_005be4d0/790/870 are code addresses (prototypes, pushed as immediates, not MGLOBAL byte reads); vtable calls stdcall. Residual: original duplicates the success epilogue (mov eax,esi; pop x4; ret) after the FUN_005aef30 test, ours shares the first return. Permuter 194 compiles no gain. Class (b).
/* MATCH-S28 0x005be260: raw g2s.py output (re/tools/match/g2s.py on decomp_pc.py --port --callees), body
   unchanged; globals moved to MGLOBAL. Score: see verify/matchs28_20261107/results.tsv. */
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
typedef unsigned char byte;

typedef void *HANDLE;
typedef int BOOL;
typedef long LONG;
void FUN_005be4d0(void);
MGLOBAL(undefined1, DAT_005d0c8c, 0x005d0c8c);
void FUN_005be790(void);
MGLOBAL(undefined1, DAT_005e7150, 0x005e7150);
void FUN_005be870(void);
extern char u_RwaDSSource_00635838[];
MGLOBAL(void *, PTR_FUN_00635868, 0x00635868);
MGLOBAL(undefined *, PTR_LAB_006358a4, 0x006358a4);
extern char u_Output_006358d0[];
__declspec(dllimport) BOOL __stdcall CloseHandle(HANDLE p0);
int FUN_005adf30(void * p0, void * p1);
uint FUN_005aeea0(void * p0, LONG p1, LONG p2);
undefined4 FUN_005aef00(void * p0, undefined4 p1, undefined4 p2, undefined4 p3);
undefined4 * FUN_005aef30(void * p0, void * p1);
undefined4 FUN_005bc920(void * p0, undefined4 p1);
undefined4 * FUN_005bcbe0(void * p0, int p1, int p2);
void FUN_005bcc70(void * p0);
void __stdcall FUN_005be450(int p0);
undefined4 * FUN_005beae0(undefined4 p0, int p1);
int FUN_005bf710(undefined4 p0, undefined4 p1, undefined4 p2, int p3, undefined4 * p4, int p5);
void FUN_005bf7d0(int p0, undefined4 p1, undefined4 p2);
short * FUN_005c2f0a(void * p0, void * p1);
// MATCH: 0x005be260 FUN_005be260
int * FUN_005be260(undefined4 param_1,int param_2,int param_3,int param_4,int param_5,
                  undefined4 param_6,undefined4 param_7,int param_8)

{
  int *piVar1;
  int *piVar2;
  int *piVar3;
  int iVar4;
  
  if (param_8 == 0) {
    param_8 = 0x378;
  }
  piVar3 = (int *)FUN_005beae0(param_7,param_8);
  if (piVar3 != (int *)0x0) {
    iVar4 = FUN_005bf710((undefined4)piVar3,(undefined4)FUN_005be870,0,1,(undefined4 *)(piVar3 + 0x56),0);
    piVar1 = piVar3 + 0xcd;
    *piVar1 = iVar4;
    if (iVar4 != 0) {
      piVar3[0x47] = (int)piVar1;
      piVar3[0x48] = 1;
      piVar3[4] = (int)&DAT_005e7150;
      FUN_005c2f0a(piVar3 + 6,u_RwaDSSource_00635838);
      FUN_005c2f0a(piVar3 + 0x5c,u_Output_006358d0);
      piVar2 = piVar3 + 0xbb;
      iVar4 = FUN_005bc920(piVar2,param_1);
      if (iVar4 != 0) {
        piVar3[0x56] = (int)&PTR_LAB_006358a4;
        *piVar3 = (int)&PTR_FUN_00635868;
        piVar3[0x9f] = 1;
        piVar3[0x9e] = (int)piVar2;
        piVar3[0xaf] = param_2;
        piVar3[0xb0] = param_3;
        piVar3[0xb3] = 0;
        piVar3[0xce] = param_4;
        piVar3[0xcf] = param_5;
        iVar4 = FUN_005adf30(piVar2,&DAT_005d0c8c);
        if (iVar4 == 0) {
          FUN_005bf7d0(*piVar1,2,(undefined4)FUN_005be790);
          return piVar3;
        }
        iVar4 = (int)FUN_005bcbe0(piVar3 + 0xd0,param_6,param_3);
        if (iVar4 != 0) {
          iVar4 = FUN_005aeea0(piVar3 + 0xdb,0,param_3);
          if (iVar4 != 0) {
            iVar4 = FUN_005aeea0(piVar3 + 0xdc,0,1);
            if (iVar4 != 0) {
              iVar4 = FUN_005aef00((undefined4 *)(piVar3 + 0xd6),(undefined4)FUN_005be4d0,0,0);
              if (iVar4 != 0) {
                piVar3[0xdd] = 1;
                iVar4 = (int)FUN_005aef30(piVar3 + 0xd6,piVar3);
                if (iVar4 != 0) {
                  return piVar3;
                }
              }
              CloseHandle((HANDLE)piVar3[0xdc]);
            }
            CloseHandle((HANDLE)piVar3[0xdb]);
          }
          FUN_005bcc70(piVar3 + 0xd0);
        }
      }
      (**(void (__stdcall **)(int *))(*(int *)*piVar1 + 8))((int *)*piVar1);
    }
    (**(void (__stdcall **)(int *))(*piVar3 + 8))(piVar3);
  }
  return (int *)0x0;
}
