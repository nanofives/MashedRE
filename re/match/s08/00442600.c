// FLAGS: /O2 /GS /Ob1
/* MATCH-S08 0x00442600: 100% by hand (raw 42%). Path tables typed as {float count; 48-byte entry[]} (the entry
   is the record FUN_00441b30 reads), switch with default last, float == compares (9176 emits them as integer
   compares against the bit pattern), FUN_00472650 declared as float(float, float) (its result goes through
   _ftol; the ECX that Ghidra lists is not read by this caller). Case bodies in the source order the original
   emits them (0x27, 0x21, 3, 0x26, 0x1e, 2, 0xb, 0x25, 0x22, 0x19, 0x24, 0x1a, default); with that order the whole
   symbol incl. the jump tables is byte-identical (match.py --bytes) and relocheck is OK. */
/* ---- MATCH-S08 preamble (same text in every re/match/s08 source; not part of any function body) ----
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
typedef struct {
  float pos[3];
  float f3, f4, f5, f6;
  float f7, f8, f9;
  float f10, f11;
} Src_00442600;
typedef struct {
  float count;
  Src_00442600 e[1];
} Path_00442600;
typedef struct { int m0; char pad[0xd4]; } Cam_00442600;
MGLOBAL(int, DAT_0089898c, 0x0089898c);
MGLOBAL(int, DAT_008964a0, 0x008964a0);
MGLOBAL(int, DAT_0089899c, 0x0089899c);
MGLOBAL(int, DAT_00898998, 0x00898998);
MGLOBAL(int, DAT_00898994, 0x00898994);
MGLOBAL(int, DAT_0089649c, 0x0089649c);
MGLOBAL(int, DAT_008964a4, 0x008964a4);
MGLOBAL(int, DAT_00897fc0, 0x00897fc0);
MGLOBAL(Path_00442600, DAT_005f7a48, 0x005f7a48);
MGLOBAL(Path_00442600, DAT_005f7bd0, 0x005f7bd0);
MGLOBAL(Path_00442600, DAT_005f7e48, 0x005f7e48);
MGLOBAL(Path_00442600, DAT_005f8030, 0x005f8030);
MGLOBAL(Path_00442600, DAT_005f82a8, 0x005f82a8);
MGLOBAL(Path_00442600, DAT_005f8490, 0x005f8490);
MGLOBAL(Path_00442600, DAT_005f8798, 0x005f8798);
MGLOBAL(Path_00442600, DAT_005f8980, 0x005f8980);
MGLOBAL(Path_00442600, DAT_005f8b68, 0x005f8b68);
MGLOBAL(Path_00442600, DAT_005f8d50, 0x005f8d50);
MGLOBAL(Path_00442600, DAT_005f8ea8, 0x005f8ea8);
MGLOBAL(Path_00442600, DAT_005f91e0, 0x005f91e0);
MGLOBAL(Path_00442600, DAT_005f9368, 0x005f9368);
MGLOBALA(Cam_00442600, DAT_008964c0, 32, 0x008964c0);
void FUN_00405540(Path_00442600 *p0);
int FUN_00426c00(void);
void FUN_00441b30(Cam_00442600 *p0, Src_00442600 *p1);
float FUN_00472650(float p0, float p1);
// MATCH: 0x00442600 FUN_00442600
void FUN_00442600(void)
{
  Path_00442600 *path;
  int n;
  int i;

  if (DAT_0089898c == 1) {
    return;
  }
  DAT_0089898c = 1;
  switch (FUN_00426c00()) {
  case 0x27:
    path = &DAT_005f7a48;
    break;
  case 0x21:
    path = &DAT_005f7bd0;
    break;
  case 3:
    path = &DAT_005f7e48;
    break;
  case 0x26:
    path = &DAT_005f8030;
    break;
  case 0x1e:
    path = &DAT_005f8d50;
    break;
  case 2:
    path = &DAT_005f91e0;
    break;
  case 0xb:
    path = &DAT_005f9368;
    break;
  case 0x25:
    path = &DAT_005f8ea8;
    break;
  case 0x22:
    path = &DAT_005f8798;
    break;
  case 0x19:
    path = &DAT_005f8980;
    break;
  case 0x24:
    path = &DAT_005f8b68;
    break;
  case 0x1a:
    path = &DAT_005f8490;
    break;
  default:
    path = &DAT_005f82a8;
    break;
  }
  FUN_00405540(path);
  DAT_008964a0 = -1;
  DAT_0089899c = -1;
  DAT_00898998 = -1;
  n = (int)path->count;
  DAT_00898994 = n;
  for (i = 0; i < n; i++) {
    FUN_00441b30(&DAT_008964c0[i], &path->e[i]);
    if (path->e[i].f7 == 1.0f) {
      if (DAT_008964a0 == -1) {
        DAT_008964a0 = i;
      }
      DAT_0089649c = i;
    }
    if (path->e[i].f7 == 2.0f) {
      if (DAT_0089899c == -1) {
        DAT_0089899c = i;
      }
      DAT_008964a4 = i;
    }
    if (path->e[i].f7 == 3.0f && DAT_00898998 == -1) {
      DAT_00898998 = i;
    }
  }
  DAT_00897fc0 = (int)FUN_00472650(0.0f, 7.0f) << 8;
}
