// FLAGS: /O2 /GS /Ob1
/* re/match/topm01/00411d90.c -- PORT-TOP-MATCH-01: 0x00411d90 (hooks.csv Replay::CreateOrLoad), 410 B.
   match.py: 99.2% (regs 99.2%, ops 99.2%) 118/117 ins; --bytes: whole symbol BYTE-IDENTICAL (relocations
   masked); relocheck OK. The 0.8% is a tool limit, not code: re/console/pc_funcs.csv gives Ghidra size 407,
   which cuts the final 5-byte `jmp 0x004a2be9` (__security_check_cookie tail) at 0x00411f25. Scored with
   size 410 via match.score_function(..., size=0x19a): 118/118 ins, 100.0%.
   Caller: 0x0040d270 pushes one argument (0x0040d3df/0x0040d3e1); read as ebx from [esp+0x58] at 0x00411dbf.
   Facts read from the disassembly (re/tools/match/odis.py 0x00411d90):
     - `sub eax,2 / jne` at 0x00411dad: the 0x0042f6a0 result is a switch with one case 2 (an if gives cmp).
     - Table 0x005f29d0 = {int key; float val} pairs ended by key -1; the first key is tested before the loop
       (0x00411dbb) and the match stores val to the stack slot [esp] (0x00411dd7), which is also the slot
       0x00411350 later writes its float output into (lea at 0x00411e9d, third push).
     - Strings: 0x005ccfe4 "replay size will be %d bytes\n", 0x005ccfc4 "creating replay of length %f\n",
       0x005ccf98 "setting time attack record too %d:%d:%f\n", 0x005ccf14 "%sReplay%d.rep",
       0x005cce1c "c:\toast\" (backslashes literal). 0x004a2cbd is the printf entry, RwGlobals slot +0xc4 the
       sprintf (call [edi] at 0x00411e62), 0x00483d10 gets &DAT_008a94a8 (push 0x8a94a8 at 0x00411e76).
     - After the 0x00411350 call, the two int outputs are loaded once into esi/edi (0x00411eb7, 0x00411ebf)
       and pushed again for 0x004299d0 (0x00411ed6/0x00411ed7) after the 0x004987b0 call, while the float
       output is re-read from memory (0x00411ed2). The ints are kept across a call although their addresses
       were passed to 0x00411350.
   What closed it: FUN_00411350's body (the matched text of re/match/s02/00411350.c) defined in the same
   translation unit before this function, so the compiler sees that the pointers do not escape and keeps the
   int outputs in esi/edi. With only a prototype it reloads them after 0x004987b0 (82.5%); copying them into
   int locals gives `mov eax,[..]; push eax; ...; mov esi,eax` (90.8%). The body is compiled only under the
   DDK (C); a C++ port build sees the prototype and links the s02 definition, so this file adds no symbol
   there. One float variable serves as the duration and as the 0x00411350 float output, as the shared slot
   shows. The `if (first key != -1)` guard around the loop is needed (without it: 3 differing bytes).
   Tried: if vs switch, separate duration variable, inner-scope outputs, declaration order permutations,
   int copies in several orders, a static __inline wrapper for the two report calls, unprototyped callees,
   /G5 /Oa /Ow, match_permute (best legal variants stayed at 90.8%; its 97.5% variant read an output before
   the call and was rejected).
   Residual: none (byte-identical). */
/* ---- MATCH-TOPM01 preamble (same text in every re/match/topm01 source; not part of any function body) ----
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
typedef struct Pair_00411d90 { int key; float val; } Pair_00411d90;
typedef struct RwMem_00411d90 { int *start; int length; } RwMem_00411d90;
typedef int (*RwSprintfFn_00411d90)(char *buf, const char *fmt, ...);
MGLOBALA(Pair_00411d90, DAT_005f29d0, 1, 0x005f29d0);
MGLOBAL(char *, DAT_007d3ff8, 0x007d3ff8);
MGLOBAL(RwMem_00411d90, DAT_008a94a8, 0x008a94a8);
MGLOBAL(int, DAT_0063bb04, 0x0063bb04);
MGLOBAL(int, DAT_0063bb08, 0x0063bb08);
MGLOBAL(int, DAT_0063bb10, 0x0063bb10);
MGLOBAL(int, DAT_0063bb14, 0x0063bb14);
MGLOBAL(int, DAT_0063bb18, 0x0063bb18);
MGLOBAL(int, DAT_0063bb2c, 0x0063bb2c);
int FUN_0042f6a0(void);
int FUN_00482900(float param_1, int param_2);
int FUN_004a2cbd(const char *fmt, ...);
int FUN_00482930(float param_1, int param_2, int param_3);
int FUN_00426c00(void);
int FUN_00430820(void);
int FUN_00483d10(RwMem_00411d90 *param_1, int param_2);
#ifdef __cplusplus
void FUN_00411350(unsigned int param_1, float *param_2, int *param_3, int *param_4);
#else
/* same text as re/match/s02/00411350.c (0x00411350); visible here so the caller codegen matches */
void FUN_00411350(unsigned int param_1, float *param_2, int *param_3, int *param_4)
{
  float t;

  t = (float)param_1 * 0.00033333333f;
  *param_3 = (int)t;
  *param_2 = t - (float)*param_3;
  *param_4 = 0;
  while (*param_3 > 59) {
    *param_4 = *param_4 + 1;
    *param_3 = *param_3 - 60;
  }
}
#endif
void FUN_004987b0(const char *fmt, ...);
void FUN_004299d0(float param_1, int param_2, int param_3);
void FUN_00411750(void);
// MATCH: 0x00411d90 FUN_00411d90
void FUN_00411d90(int param_1)
{
  Pair_00411d90 *p;
  float local_50;
  int local_4c;
  int local_48;
  char name[64];

  local_50 = 0.0f;
  switch (FUN_0042f6a0()) {
  case 2:
    if (DAT_005f29d0[0].key != -1) {
      for (p = DAT_005f29d0; p->key != -1; p++) {
        if (p->key == param_1) {
          local_50 = p->val;
        }
      }
    }
    FUN_004a2cbd("replay size will be %d bytes\n", FUN_00482900(local_50, 10));
    FUN_004a2cbd("creating replay of length %f\n", (double)local_50);
    if (DAT_0063bb04 == 0) {
      DAT_0063bb04 = FUN_00482930(local_50, 10, param_1);
      DAT_0063bb14 = DAT_0063bb04;
    }
    if (DAT_0063bb08 == 0) {
      (*(RwSprintfFn_00411d90 *)(DAT_007d3ff8 + 0xc4))(name, "%sReplay%d.rep", "c:\\toast\\", FUN_00426c00());
      if (FUN_00430820() == 0) {
        DAT_0063bb08 = FUN_00483d10(&DAT_008a94a8, FUN_00426c00());
      }
      if (DAT_0063bb08 != 0) {
        DAT_0063bb10 = DAT_0063bb08;
        FUN_00411350(*(unsigned int *)(DAT_0063bb08 + 0x174), &local_50, &local_4c, &local_48);
        FUN_004987b0("setting time attack record too %d:%d:%f\n", local_48, local_4c, (double)local_50);
        DAT_0063bb2c = 1;
        FUN_004299d0(local_50, local_4c, local_48);
      }
      else {
        DAT_0063bb2c = 0;
        DAT_0063bb08 = FUN_00482930(local_50, 10, param_1);
      }
      DAT_0063bb18 = 0;
    }
    break;
  }
  FUN_00411750();
}
