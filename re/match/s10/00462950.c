// FLAGS: /O2 /GS /Ob1
/* MATCH-S10 0x00462950 (h6): 88.8% (regs 94.7%, ops 97.3%). Raw 43.0%. Shape: printf (0x004a2cbd) progress
   lines, sprintf 0x004a2b60 + strcat intrinsics for the path, two cdecl RwGlobals slots (+0x114 3 args,
   +0x10c 1 arg), a 0x90-byte bank record (+0x80 first voice, +0x84 count, +0x88 loop flag, +0x8c float).
   Moves that scored: DAT_0068fc80 / DAT_0068fc84 file-static (pilot finding 2: the original keeps both in
   ebp/esi across the voice loop and reloads them after each call) 70->85%; FUN_005a6dc0 is called with an
   int 4th arg at four sites (push 1 / push 0) and a float at one (fld/fstp): the float prototype plus an
   int-typed cast at the int sites (two source prototypes folded into one function); tail as a 2-case
   switch; the bank float `volatile` (only way found to get the original's fld/fstp copy) 85->88.8%.
   Residual (b): register choice around DAT_0088e670: the original reloads DAT_00690478 into esi for the
   count-down sum loop and keeps DAT_0088e670 in eax into the third loop (ours reuses eax for the count and
   reloads the base into ecx), which flips eax/ecx in the third loop. Tried: != 0 vs > 0, a local pointer,
   do/while count-down, local total, another counter, permuter (270 compiles, no gain). */
/* ---- MATCH-S10 preamble (same text in every re/match/s10 source; not part of any function body) ----
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
typedef struct { int h; int pad; } S10Voice;
typedef struct {
  char pad00[0x80];
  unsigned int first;
  unsigned int count;
  int loop;
  volatile float vol;
} S10Bank;
typedef void (*S10SetI)(int, int, int, int);
typedef int (*S10RwFn3)(int, int, int);
typedef int (*S10RwFn1)(int);
MGLOBAL(S10Bank *, DAT_0088e670, 0x0088e670);
MGLOBAL(char *, DAT_007d3ff8, 0x007d3ff8);
MGLOBAL(int, DAT_007dcaa8, 0x007dcaa8);
MGLOBAL(int, DAT_007dcaac, 0x007dcaac);
MGLOBAL(int, DAT_0068f62c, 0x0068f62c);
MGLOBAL(int, DAT_0069049c, 0x0069049c);
MGLOBAL(unsigned int, DAT_00690478, 0x00690478);
#ifndef __cplusplus
static unsigned int DAT_0068fc80;
static unsigned int DAT_0068fc84;
#else
MGLOBAL(unsigned int, DAT_0068fc80, 0x0068fc80);
MGLOBAL(unsigned int, DAT_0068fc84, 0x0068fc84);
#endif
MGLOBAL(unsigned int, DAT_0088e674, 0x0088e674);
MGLOBALA(S10Bank *, PTR_s_skid_on_wet_tarmac_0060c820, 1, 0x0060c820);
MGLOBALA(int, DAT_0068f4d0, 1, 0x0068f4d0);
MGLOBALA(S10Voice, DAT_0068f640, 1, 0x0068f640);
int FUN_004a2cbd(const char *p0, ...);
int FUN_004a2b60(char *p0, const char *p1, ...);
char *FUN_0041e980(int p0);
void FUN_0045de80(char *p0, int p1);
void FUN_0045e160(void);
void FUN_0045e2a0(void);
int FUN_004cc160(int p0, int p1);
int FUN_004cc230(int p0, int p1, char *p2);
int FUN_004cc5e0(int p0, int p1, int p2, int p3);
void FUN_005a6710(int p0, int p1);
void FUN_005a6dc0(int p0, int p1, int p2, float p3);
int FUN_005a7aa0(int p0, S10Bank *p1);
unsigned int FUN_005a7af0(int p0);
int FUN_005a7b60(int p0, int p1, int p2, int p3, int p4, int p5);
char *strcat(char *, const char *);
#pragma intrinsic(strcat)
// MATCH: 0x00462950 FUN_00462950
void FUN_00462950(int param_1)
{
  unsigned int i;
  char name[16];
  char path[128] = "";
  int h;
  int dev;
  unsigned int n;
  unsigned int v;
  unsigned int total;

  FUN_004a2cbd("loading course\n");
  FUN_004a2b60(path, "toastaudio\\pc\\audio\\pcdics\\");
  FUN_004a2b60(name, "");
  strcat(name, FUN_0041e980(param_1));
  strcat(path, FUN_0041e980(param_1));
  strcat(path, ".rws");
  DAT_0088e670 = PTR_s_skid_on_wet_tarmac_0060c820[param_1];
  FUN_004a2cbd("loading courseA\n");
  h = FUN_004cc230(2, 1, path);
  FUN_004a2cbd("loading courseA %s\n", path);
  dev = (*(S10RwFn3 *)(DAT_007d3ff8 + 0x114))(0x10001, 1, 0);
  FUN_004a2cbd("loading courseB\n");
  FUN_004a2cbd("loading courseC\n");
  if (dev != 0) {
    DAT_007dcaa8 = dev;
    DAT_007dcaac = 0x10000;
  }
  else {
    DAT_007dcaa8 = 0;
    DAT_007dcaac = 0;
  }
  FUN_004a2cbd("loading courseD\n");
  DAT_0068f62c = 0;
  while (FUN_004cc5e0(h, 0x809, 0, 0) != 0) {
    FUN_004a2cbd("loading courseDA\n");
    DAT_0068f62c = FUN_005a7b60(0, h, DAT_0069049c, 0, 0, 0);
    FUN_004a2cbd("loading courseDB\n");
  }
  FUN_004a2cbd("loading courseE\n");
  (*(S10RwFn1 *)(DAT_007d3ff8 + 0x10c))(dev);
  FUN_004a2cbd("loading courseF\n");
  FUN_004cc160(h, 0);
  FUN_004a2cbd("loading courseG\n");
  DAT_00690478 = FUN_005a7af0(DAT_0068f62c);
  for (i = 0; i < DAT_00690478; i++) {
    DAT_0068f4d0[i] = FUN_005a7aa0(DAT_0068f62c, &DAT_0088e670[i]);
  }
  DAT_0068fc80 = 0;
  for (i = 0; i < DAT_00690478; i++) {
    if (DAT_0088e670[i].count > 0) {
      DAT_0068fc80 += DAT_0088e670[i].count;
    }
  }
  for (v = DAT_0068fc84; v < DAT_0068fc84 + DAT_0068fc80; v++) {
    *(unsigned int *)(DAT_0068f640[v].h + 0x34) |= 4;
    if (*(int *)(DAT_0068f640[v].h + 0x30) != 0) {
      ((S10SetI)FUN_005a6dc0)(*(int *)(DAT_0068f640[v].h + 0x30), 8, 1, 1);
    }
    *(int *)(DAT_0068f640[v].h + 0x44) = 0;
    if (*(int *)(DAT_0068f640[v].h + 0x30) != 0) {
      ((S10SetI)FUN_005a6dc0)(*(int *)(DAT_0068f640[v].h + 0x30), 10, 1, 0);
    }
  }
  v = DAT_0068fc84;
  for (i = 0; i < DAT_00690478; i++) {
    DAT_0088e670[i].first = v;
    for (n = 0; n < DAT_0088e670[i].count; n++) {
      FUN_005a6710(DAT_0068f640[v].h, DAT_0068f4d0[i]);
      if (DAT_0088e670[i].loop == 0) {
        *(unsigned int *)(DAT_0068f640[v].h + 0x34) &= ~4;
        if (*(int *)(DAT_0068f640[v].h + 0x30) != 0) {
          ((S10SetI)FUN_005a6dc0)(*(int *)(DAT_0068f640[v].h + 0x30), 8, 1, 0);
        }
      }
      else {
        *(unsigned int *)(DAT_0068f640[v].h + 0x34) |= 4;
        if (*(int *)(DAT_0068f640[v].h + 0x30) != 0) {
          ((S10SetI)FUN_005a6dc0)(*(int *)(DAT_0068f640[v].h + 0x30), 8, 1, 1);
        }
      }
      *(float *)(DAT_0068f640[v].h + 0x74) = DAT_0088e670[i].vol;
      if (*(int *)(DAT_0068f640[v].h + 0x30) != 0) {
        FUN_005a6dc0(*(int *)(DAT_0068f640[v].h + 0x30), 3, 1, DAT_0088e670[i].vol);
      }
      v++;
    }
  }
  DAT_0088e674 = v;
  FUN_0045de80(name, param_1);
  switch (param_1) {
  case 0:
    FUN_0045e2a0();
    break;
  case 0x24:
    FUN_0045e160();
    break;
  }
}
