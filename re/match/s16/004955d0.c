// FLAGS: /O2 /GS /Ob1
/* MATCH-S16 0x004955d0 (h2): matched: whole obj symbol BYTE-IDENTICAL (419 B, match.py --bytes); per-instruction 97.7% only because Ghidra's size (416) stops before the last 3 epilogue instructions. Hand-written (raw converter did not compile): char dir[260] = an empty initializer, intrinsic strcat, for loop over GetLogicalDrives()>>1, else-if for the two FUN_00550790 calls (tail-merged). */
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
typedef unsigned long DWORD;
typedef unsigned int UINT;
char *strcat(char *dst, const char *src);
#pragma intrinsic(strcat)
__declspec(dllimport) DWORD __stdcall GetCurrentDirectoryA(DWORD n, char *buf);
__declspec(dllimport) char *__stdcall CharUpperA(char *s);
__declspec(dllimport) DWORD __stdcall GetLogicalDrives(void);
__declspec(dllimport) UINT __stdcall GetDriveTypeA(const char *root);
void *FUN_00551190(int p0, char *p1, char *p2);
int FUN_00550430(void *p0);
void FUN_00550790(void *p0);
// MATCH: 0x004955d0 FUN_004955d0
int FUN_004955d0(void)
{
  DWORD len;
  DWORD mask;
  UINT type;
  void *h;
  char root[4];
  char letter[2];
  int idx;
  void *first;
  char dir[260] = "";

  len = GetCurrentDirectoryA(0x104, dir);
  if (len != 0 && len <= 0x104) {
    strcat(dir, "\\");
    first = FUN_00551190(0x14, dir, "unc");
    if (first != 0 && FUN_00550430(first) != 0) {
      CharUpperA(dir);
      idx = 2;
      for (mask = GetLogicalDrives() >> 1; mask != 0; mask >>= 1) {
        if (mask & 1) {
          root[0] = (char)idx + '@';
          root[1] = ':';
          root[2] = '\\';
          root[3] = 0;
          type = GetDriveTypeA(root);
          if (type != 0 && type != 1) {
            root[2] = 0;
            letter[0] = root[0];
            letter[1] = 0;
            h = FUN_00551190(0x14, root, letter);
            if (h == 0) {
              return 0;
            }
            if (FUN_00550430(h) == 0) {
              return 0;
            }
            if (dir[1] != ':') {
              FUN_00550790(first);
            }
            else if (root[0] == dir[0]) {
              FUN_00550790(h);
            }
          }
        }
        idx++;
      }
      return 1;
    }
  }
  return 0;
}
