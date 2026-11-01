// FLAGS: /O2 /GS /Ob1
/* MATCH-S06 0x004264d0: 100% by hand (h4), regs 100%. This file also holds its callee 0x00426460 (100%):
   that callee takes its argument in ECX (Ghidra unaff_/ECX param, plain ret). It matches only as a file-static
   function defined before its caller (pilot finding 1); as an extern __fastcall it scored 90.7% (the shr temp
   came out in ecx instead of edx). 0x00426460 needed `obj = *(int *)(list[i] + 4) + 0x40` before the call.
   0x004264d0: char[128] = {0} buffers give the byte store + rep stosd/stosw/stosb; /GS cookie frame as is.
   relocheck OK (unchecked: the /GS cookie helpers only). Residual: none. */
/* ---- MATCH-S06 preamble (same text in every re/match/s06 source; not part of any function body) ----
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
#ifdef __cplusplus
#define MATCH_STATIC
#else
#define MATCH_STATIC static
#endif
typedef void (*RwStrFn)(char *dst, const char *src);
MGLOBAL(int, DAT_007d3ff8, 0x007d3ff8);
MGLOBAL(float, DAT_005cc94c, 0x005cc94c);
MGLOBALA(char, DAT_005cd280, 5, 0x005cd280);
char *FUN_0041e980(int p0);
int FUN_0042a470(int p0, char *p1, int p2);
int FUN_0042a5d0(char *p0, int p1, int p2);
char *FUN_0042a8d0(char *p0);
int FUN_0042fe30(void);
void FUN_00458c70(void);
void FUN_00458fd0(int p0, uint p1, float p2);
void FUN_0047a130(char *p0);
void FUN_00495280(char *p0);
void FUN_004952f0(void);
int FUN_004b3fc0(int p0, int *p1);
uint FUN_004b5190(int p0, int p1, int p2);
int FUN_004e6e00(int p0);

// MATCH: 0x00426460 FUN_00426460
MATCH_STATIC void FUN_00426460(int param_1)
{
  int i;
  int n;
  uint v;
  int obj;
  int list[32];

  n = FUN_004b3fc0(param_1, list);
  for (i = 0; i < n; i++) {
    obj = *(int *)(list[i] + 4) + 0x40;
    v = FUN_004b5190(list[i], 0, 0);
    FUN_00458fd0(obj, v & 0xff, (float)(v >> 8));
  }
}

// MATCH: 0x004264d0 FUN_004264d0
void FUN_004264d0(int param_1)
{
  char *names[4];
  char dir[128] = {0};
  char path[128] = {0};
  char *track;
  char *name;
  int rank;
  int clump;

  track = FUN_0041e980(param_1);
  names[1] = "powerups_gold.lua";
  names[2] = "powerups_gold.lua";
  names[3] = "powerups_gold.lua";
  (*(RwStrFn *)(DAT_007d3ff8 + 0xcc))(dir, "toastart/tracks/");
  (*(RwStrFn *)(DAT_007d3ff8 + 0xd4))(dir, track);
  (*(RwStrFn *)(DAT_007d3ff8 + 0xd4))(dir, DAT_005cd280);
  (*(RwStrFn *)(DAT_007d3ff8 + 0xcc))(path, FUN_0042a8d0(dir));
  FUN_00458c70();
  rank = FUN_0042fe30();
  if (rank == 0) {
    return;
  }
  name = names[rank];
  FUN_00495280(path);
  clump = FUN_0042a5d0("powerups_gold.dff", 0, 0);
  if (clump != 0) {
    FUN_00426460(clump);
    FUN_004e6e00(clump);
  } else if (FUN_0042a470(0, name, 1) != 0) {
    FUN_0047a130(name);
  } else if (FUN_0042a470(0, "powerups.lua", 1) != 0) {
    FUN_0047a130("powerups.lua");
  }
  FUN_004952f0();
}
