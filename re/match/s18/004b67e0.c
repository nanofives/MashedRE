// FLAGS: /O2 /GS /Ob1
/* MATCH-S18 0x004b67e0 (h4): 100%, regs 100%. By hand (g2s did not compile). File read at an offset: fseek FUN_005c1d63 + fread FUN_004a49cf (CRT, by address) when DAT_007d3e50 bit 0, else overlapped ReadFile + GetOverlappedResult poll loop. Keys: memset(&ov,0,sizeof ov) then ov.Offset = param_1 (the dead zero store of Offset is in the original), for(;;) with done = 0; continue on ERROR_IO_INCOMPLETE (0x3e4). The 0x100 sizes on DAT_008ab6e0/DAT_008ad8a0 are placeholders (only their addresses are used). */
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
typedef struct { unsigned long Internal; unsigned long InternalHigh; unsigned long Offset; unsigned long OffsetHigh; void *hEvent; } OVL_004b67e0;
typedef void (*CbFn_004b67e0)(void *, void *, int);
MGLOBAL(unsigned char, DAT_007d3e50, 0x007d3e50);
MGLOBAL(void *, DAT_007d3e48, 0x007d3e48);
MGLOBAL(CbFn_004b67e0, DAT_007d3e5c, 0x007d3e5c);
MGLOBAL(int, DAT_007d3e60, 0x007d3e60);
MGLOBALA(unsigned char, DAT_008ab6e0, 0x100, 0x008ab6e0);
MGLOBALA(unsigned char, DAT_008ad8a0, 0x100, 0x008ad8a0);
int FUN_005c1d63(void *p0, long p1, int p2);
unsigned int FUN_004a49cf(void *p0, unsigned int p1, unsigned int p2, void *p3);
__declspec(dllimport) int __stdcall ReadFile(void *, void *, unsigned long, unsigned long *, OVL_004b67e0 *);
__declspec(dllimport) int __stdcall GetOverlappedResult(void *, OVL_004b67e0 *, unsigned long *, int);
__declspec(dllimport) unsigned long __stdcall GetLastError(void);

void *memset(void *, int, unsigned int);
#pragma intrinsic(memset)

// MATCH: 0x004b67e0 FUN_004b67e0
void FUN_004b67e0(unsigned long param_1, void *param_2, unsigned int param_3, unsigned long *param_4)
{
  OVL_004b67e0 ov;
  int done;
  int r;
  unsigned long err;

  if (DAT_007d3e50 & 1) {
    FUN_005c1d63(DAT_007d3e48, param_1, 0);
    *param_4 = FUN_004a49cf(param_2, 1, param_3, DAT_007d3e48);
    return;
  }
  memset(&ov, 0, sizeof(ov));
  ov.Offset = param_1;
  ReadFile(DAT_007d3e48, param_2, param_3, 0, &ov);
  GetLastError();
  done = 0;
  for (;;) {
    if (DAT_007d3e5c != 0) {
      DAT_007d3e5c(DAT_008ad8a0, DAT_008ab6e0, DAT_007d3e60);
    }
    r = GetOverlappedResult(DAT_007d3e48, &ov, param_4, 0);
    err = GetLastError();
    if (r == 0) {
      if (err == 0x3e4) {
        done = 0;
        continue;
      }
    } else if (r == 1) {
      done = 1;
    }
    if (err == 0x26 || done) break;
  }
}
