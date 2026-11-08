// FLAGS: /O2 /GS /Ob1
/* re/match/topm01/0040d110.c -- PORT-TOP-MATCH-01: 0x0040d110 (hooks.csv FUN_0040d110), 320 B.
   match.py: 100.0% (regs 100.0%, ops 100.0%) 107/107 ins, whole symbol BYTE-IDENTICAL (relocations masked).
   relocheck.py: OK (checked=16). Written from the listing (re/tools/match/odis.py 0x0040d110); no prior source.
   Caller: 0x0040d270 (re/match/top07/0040d270.c), twice, pushing (param_1, param_2).
   Facts read from the disassembly:
     Frame: sub esp,0x20. Byte locals at frame+0..+7 = c8 c8 c8 80 / ff ff 00 80 (0x0040d114..0x0040d14e),
     two dwords at frame+8/+0xc, four dwords at frame+0x10..+0x1c, all zeroed (0x0040d146..0x0040d15e).
     Group value = *(int *)(param_1[param_2] + 4) / 6 (magic 0x2aaaaaab, 0x0040d143..0x0040d173); recomputed
     with index FUN_00431d70() when FUN_0042f6a0() == 10 (0x0040d17a) and again when == 6 (0x0040d19f);
     multiplied by 6 at 0x0040d1bf (lea + shl).
     0x00420420 is pushed two arguments (index, value) at 0x0040d1f5/0x0040d1f6 and cleaned with add esp,8;
     its matched body (re/match/s05/00420420.c) takes one, so it is declared without a prototype here.
     The loop walks 0x007f1a1c..0x007f1a5c in 0x10 steps (0x0040d1de, 0x0040d203, 0x0040d207) with a signed
     compare (jl at 0x0040d20d); value = [rec] + group, or index + group when FUN_0042f6a0() == 0xb (0x0040d1ed).
     0x0041a8d0 gets (&frame+8, &frame+0) at 0x0040d213/0x0040d218; both frame+8 dwords are set to the
     group value at 0x0040d219/0x0040d21d. 0x004220d0 gets &frame+0x10 (0x0040d22a).
     0x0041a8d0 and 0x004220d0 arguments are cleaned together with 0x0040cea0's by add esp,0x18 at 0x0040d245.
   What closed it: an index loop (i < 4 over a 16-byte struct overlay) gives the signed jl; a pointer loop gives
   jb (98.1% with the init order below fixed). Zeroing the frame+8 pair before the byte stores sets the store
   schedule (xor ebx,ebx at 0x0040d123, the byte 0 store at 0x0040d14e after the pair stores); the pair zeroed
   after the colours misplaces both (98.1%), the byte 0 store moved after the pair leaves xor ebx late (99.1%).
   Rec_007f1a1c / RGBA_0040d110 are mechanical overlay names, not source names.
   Residual: none. */
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
typedef struct { unsigned char r, g, b, a; } RGBA_0040d110;
typedef struct { int v0; int v1; int v2; int v3; } Rec_007f1a1c;
MGLOBALA(Rec_007f1a1c, DAT_007f1a1c, 4, 0x007f1a1c);
int FUN_0042f6a0(void);
int FUN_00431d70(void);
void FUN_0045b350(void);
char *FUN_00420230(int id);
char *FUN_0042a8d0(char *param_1);
void FUN_00495280(char *param_1);
#ifdef __cplusplus
void FUN_00420420(...);
#else
void FUN_00420420();
#endif
void FUN_0041a8d0(int *param_1, RGBA_0040d110 *param_2);
void FUN_004220d0(int *param_1);
void FUN_004952f0(void);
void FUN_0040cea0(int **param_1, int param_2, int param_3);
// MATCH: 0x0040d110 FUN_0040d110
void FUN_0040d110(int **param_1, int param_2)
{
  RGBA_0040d110 local_cols[2];
  int local_pair[2];
  int local_ids[4];
  int iVar1;
  int iVar2;
  int i;

  local_pair[0] = 0;
  local_pair[1] = 0;
  local_cols[0].r = 0xc8;
  local_cols[0].g = 0xc8;
  local_cols[0].b = 0xc8;
  local_cols[0].a = 0x80;
  local_cols[1].r = 0xff;
  local_cols[1].g = 0xff;
  local_cols[1].b = 0;
  local_cols[1].a = 0x80;
  local_ids[0] = 0;
  local_ids[1] = 0;
  local_ids[2] = 0;
  local_ids[3] = 0;
  iVar1 = param_1[param_2][1] / 6;
  if (FUN_0042f6a0() == 10) {
    iVar1 = param_1[FUN_00431d70()][1] / 6;
  }
  if (FUN_0042f6a0() == 6) {
    iVar1 = param_1[FUN_00431d70()][1] / 6;
  }
  iVar1 = iVar1 * 6;
  FUN_0045b350();
  FUN_00495280(FUN_0042a8d0(FUN_00420230(iVar1)));

  for (i = 0; i < 4; i++) {
    iVar2 = DAT_007f1a1c[i].v0 + iVar1;
    if (FUN_0042f6a0() == 0xb) {
      iVar2 = i + iVar1;
    }
    local_ids[i] = iVar2;
    FUN_00420420(i, iVar2);
  }
  local_pair[0] = iVar1;
  local_pair[1] = iVar1;
  FUN_0041a8d0(local_pair, local_cols);
  FUN_004220d0(local_ids);
  FUN_004952f0();
  FUN_0040cea0(param_1, param_2, iVar1);
}
