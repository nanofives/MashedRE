// FLAGS: /O2 /Ob1 /G5
/* MATCH-S28 0x005c88b0: 88.9% (regs 88.9%). Tail-jump arg overwrite fixed with a cdecl 2-arg fn-ptr call (85.7->88.9). Residual: rep stosd dest reg (orig loads edi=[esi+8] directly, ours reuses esi); tried pointer/index loop forms and permuter (94 compiles). Class: b. */
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
typedef void (__cdecl *tail_fn)(int *, int);
MGLOBAL(undefined1, DAT_005e6e40, 0x005e6e40);
void FUN_005a9f60(int * p0, undefined * p1, undefined4 p2);
undefined4 FUN_005aa060(undefined4 p0, undefined4 * p1, undefined4 * p2);
// MATCH: 0x005c88b0 FUN_005c88b0
void FUN_005c88b0(int *param_1,undefined4 *param_2)

{
  int iVar1;
  undefined4 *puVar2;
  
  if (*param_2 != 0) {
                    /* WARNING: Could not recover jumptable at 0x005c88c3. Too many branches */
                    /* WARNING: Treating indirect jump as call */
    ((tail_fn)*param_2)(param_1,(int)param_2[1]);
    return;
  }
  iVar1 = FUN_005aa060(0,(undefined4 *)&DAT_005e6e40,(undefined4 *)0);
  if (*param_1 == iVar1) {
    puVar2 = (undefined4 *)(param_2[2] + 0x34);
    for (iVar1 = 0x4a; iVar1 != 0; iVar1 = iVar1 + -1) {
      *puVar2 = 0xcdcdcdcd;
      puVar2 = puVar2 + 1;
    }
    return;
  }
  FUN_005a9f60(param_1,(undefined *)*param_2,param_2[1]);
  return;
}
