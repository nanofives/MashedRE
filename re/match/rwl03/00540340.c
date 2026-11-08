// FLAGS: /O2 /Ob1 /G5
// h3: 93.7% regs 93.7%. Branch order flipped (mask != 0 first, 0x148 call arm first), typed callee prototypes. Residual: deferred add esp merging and ebx/eax for piVar3; class (a).
/* MATCH-RWL03 0x00540340: raw g2s.py output (re/tools/match/g2s.py on decomp_pc.py --port --callees), body
   unchanged; globals moved to MGLOBAL. Score: see verify/matchrwl03_20261108/results.tsv. */
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
typedef int (*Fn_540340)(int *, int, int, int, int, int, int);
MGLOBAL(int, DAT_007dc5dc, 0x007dc5dc);
MGLOBAL(float, DAT_00623d38, 0x00623d38);
MGLOBAL(float, DAT_00623d3c, 0x00623d3c);
MGLOBAL(float, DAT_00623d48, 0x00623d48);
MGLOBAL(float, DAT_00623d4c, 0x00623d4c);
MGLOBAL(float, DAT_00623d58, 0x00623d58);
MGLOBAL(float, DAT_00623d5c, 0x00623d5c);
MGLOBAL(int, DAT_006181d4, 0x006181d4);
MGLOBAL(int **, DAT_007d4110, 0x007d4110);
MGLOBAL(int, DAT_006181d0, 0x006181d0);
int FUN_004cb330(int p0, void *p1);
void FUN_004d53b0(void);
void FUN_004d54f0(int p0, int p1, int p2);
int FUN_004d6e70(int p0, int p1);
// MATCH: 0x00540340 FUN_00540340
void FUN_00540340(int param_1,int param_2,byte param_3,int param_4)
{
  int *piVar1;
  float *puVar2;
  int *piVar3;

  piVar3 = (int *)0;
  if ((param_3 & 0x84) != 0) {
    FUN_004d6e70(param_4,0);
    piVar1 = *(int **)(DAT_007dc5dc + *(int *)(param_2 + 8));
    if ((((piVar1 != (int *)0x0) && (piVar1[0xc] == 5)) &&
        (puVar2 = (float *)*piVar1, piVar3 = piVar1, puVar2 != (float *)0x0)) &&
       ((((int *)puVar2)[3] & 0x20000) == 0)) {
      FUN_004d54f0(0,0x18,2);
      DAT_00623d38 = puVar2[0];
      DAT_00623d3c = puVar2[1];
      DAT_00623d48 = puVar2[4];
      DAT_00623d4c = puVar2[5];
      DAT_00623d58 = puVar2[0xc];
      DAT_00623d5c = puVar2[0xd];
      FUN_004cb330(0x10,&DAT_00623d38);
    }
    FUN_004d54f0(0,1,4);
    FUN_004d54f0(0,2,2);
    FUN_004d54f0(0,3,0);
    FUN_004d54f0(0,4,4);
    FUN_004d54f0(0,5,2);
    FUN_004d54f0(0,6,0);
  }
  else {
    FUN_004d6e70(0,0);
    FUN_004d54f0(0,1,3);
    FUN_004d54f0(0,3,0);
    FUN_004d54f0(0,4,3);
    FUN_004d54f0(0,6,0);
  }
  if (DAT_006181d4 != 0) {
    DAT_006181d4 = 0;
    (*(int (**)(int **, int))((char *)*DAT_007d4110 + 0x1ac))(DAT_007d4110,0);
  }
  if (DAT_006181d0 != *(int *)(param_2 + 0x10)) {
    DAT_006181d0 = *(int *)(param_2 + 0x10);
    (*(int (**)(int **, int))((char *)*DAT_007d4110 + 0x170))(DAT_007d4110,*(int *)(param_2 + 0x10));
  }
  if (*(int *)(param_1 + 8) != 0) {
    FUN_004d53b0();
    (*(int (**)(int **, int, int, int, int, int, int))((char *)*DAT_007d4110 + 0x148))
              (DAT_007d4110,*(int *)(param_1 + 0xc),*(int *)(param_2 + 0x14),0,
               *(int *)(param_2 + 0x18),*(int *)(param_2 + 0x1c),
               *(int *)(param_2 + 0x20));
  }
  else {
    FUN_004d53b0();
    (*(int (**)(int **, int, int, int))((char *)*DAT_007d4110 + 0x144))
              (DAT_007d4110,*(int *)(param_1 + 0xc),*(int *)(param_2 + 0x14),
               *(int *)(param_2 + 0x20));
  }
  if (piVar3 != (int *)0x0) {
    FUN_004d54f0(0,0x18,0);
  }
  return;
}
