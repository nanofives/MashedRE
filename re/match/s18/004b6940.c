// FLAGS: /O2 /GS /Ob1
/* MATCH-S18 0x004b6940 (h4): 100%, regs 100% (g2s did not compile). By hand: piz open. Clears the 0x20ac-byte record at DAT_008ab7e0, strncpy (FUN_004a34b0) of the name into its first 0x80 bytes, stricmp (FUN_004b302f) against the current name DAT_008ad8a0 (clears the 0x20000-byte cache on change), inline strcpy, progress callback, FUN_004b65e0, opens through FUN_004b6710, reads the 0x800-byte header into the 4-dword DAT_008ad9a0 (struct copy), then count entries of 0x80 bytes 16 per 0x800 block into DAT_0090dac0. This file also holds FUN_004b6710 (static, name in EAX, same body as re/match/s18/004b6710.c), which is the real register-arg callee. Keys: left > 0 with left >= 0x10 ? 0x800 : left << 7 (84.3%), then pos = 0x800 before the header copy (permuter, 100%). Sizes 0x100 on DAT_008ab6e0/DAT_008ad8a0 are placeholders. */
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
typedef struct { char name[0x80]; int rest[0x82b - 0x20]; } S_008ab7e0;
typedef struct { unsigned int d0, d1, count, d3; } Hdr_008ad9a0;
typedef void (*CbFn_004b6940)(void *, void *, int);
MGLOBAL(unsigned char, DAT_007d3e50, 0x007d3e50);
MGLOBAL(void *, DAT_007d3e48, 0x007d3e48);
MGLOBAL(int, DAT_007d3e4c, 0x007d3e4c);
MGLOBAL(int, DAT_007d3e54, 0x007d3e54);
MGLOBAL(int, DAT_007d3e58, 0x007d3e58);
MGLOBAL(CbFn_004b6940, DAT_007d3e5c, 0x007d3e5c);
MGLOBAL(int, DAT_007d3e60, 0x007d3e60);
MGLOBAL(int, DAT_007d3e64, 0x007d3e64);
MGLOBAL(int, DAT_007d3e68, 0x007d3e68);
MGLOBALA(unsigned char, DAT_008ab6e0, 0x100, 0x008ab6e0);
MGLOBAL(S_008ab7e0, DAT_008ab7e0, 0x008ab7e0);
MGLOBALA(char, DAT_008ad8a0, 0x100, 0x008ad8a0);
MGLOBAL(Hdr_008ad9a0, DAT_008ad9a0, 0x008ad9a0);
MGLOBALA(unsigned char, DAT_008ad9c0, 0x20000, 0x008ad9c0);
MGLOBALA(unsigned char, DAT_0090dac0, 0x4000, 0x0090dac0);
MGLOBALA(char, s_rb_005cf010, 3, 0x005cf010);
void *FUN_004a4541(const char *p0, const char *p1);
char *FUN_004a34b0(char *p0, const char *p1, unsigned int p2);
int FUN_004b302f(const char *p0, const char *p1);
void FUN_004b65e0(void);
void FUN_004b67e0(unsigned long p0, void *p1, unsigned int p2, unsigned long *p3);
int FUN_005507a0(void);
void FUN_00550790(int p0);
__declspec(dllimport) void *__stdcall CreateFileA(const char *, unsigned long, unsigned long, void *, unsigned long, unsigned long, void *);
__declspec(dllimport) unsigned long __stdcall GetLastError(void);
void *memset(void *, int, unsigned int);
void *memcpy(void *, const void *, unsigned int);
char *strcpy(char *, const char *);
#pragma intrinsic(memset, memcpy, strcpy)

// MATCH: 0x004b6710 FUN_004b6710
static void *FUN_004b6710(const char *param_1)
{
  if (DAT_007d3e50 & 1) {
    DAT_007d3e48 = FUN_004a4541(param_1, s_rb_005cf010);
    return DAT_007d3e48;
  }
  DAT_007d3e48 = CreateFileA(param_1, 0x80000000, 0, 0, 3, 0x60000000, 0);
  if (GetLastError() == 0 && DAT_007d3e48 != (void *)-1) {
    return (void *)1;
  }
  return 0;
}

// MATCH: 0x004b6940 FUN_004b6940
int FUN_004b6940(char *param_1)
{
  unsigned long nread;
  char buf[0x800];
  unsigned int pos;
  unsigned int left;
  unsigned int n;
  unsigned char *dst;

  memset(&DAT_008ab7e0, 0, sizeof(DAT_008ab7e0));
  FUN_004a34b0(DAT_008ab7e0.name, param_1, 0x80);
  DAT_008ab7e0.name[0x7f] = 0;
  if (FUN_004b302f(DAT_008ad8a0, param_1) != 0) {
    DAT_007d3e64 = 0;
    DAT_007d3e68 = 0;
    memset(DAT_008ad9c0, 0, 0x20000);
  }
  strcpy(DAT_008ad8a0, param_1);
  if (DAT_007d3e5c != 0) {
    DAT_007d3e5c(DAT_008ad8a0, DAT_008ab6e0, DAT_007d3e60);
  }
  FUN_004b65e0();
  DAT_007d3e48 = 0;
  if (!FUN_004b6710(param_1)) {
    return 0;
  }
  DAT_007d3e4c = 1;
  FUN_004b67e0(0, buf, 0x800, &nread);
  pos = 0x800;
  DAT_008ad9a0 = *(Hdr_008ad9a0 *)buf;
  memset(DAT_0090dac0, 0, 0x4000);
  dst = DAT_0090dac0;
  for (left = DAT_008ad9a0.count; left > 0; left -= n >> 7) {
    FUN_004b67e0(pos, buf, 0x800, &nread);
    pos += 0x800;
    n = left >= 0x10 ? 0x800 : left << 7;
    memcpy(dst, buf, n);
    dst += 0x800;
  }
  DAT_007d3e58 = FUN_005507a0();
  FUN_00550790(DAT_007d3e54);
  return 1;
}
