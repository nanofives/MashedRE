// FLAGS: /O2 /Ob1 /G5
/* MATCH-NEARMISS-2-B (2026-11-09): BYTE-IDENTICAL at 369 bytes, relocheck OK. The state-2 store is one statement after
   the in-range / overflow if-else; 9176 sinks it into both arms as the original does (the per-arm stores of h4 got hoisted). */
/* MATCH-S24 0x00550da0 (h4): nosize.py 94.9% (regs 94.9%; 96.4% at the real 369-byte size): no Ghidra size; the original ends at the
   ret at 0x00550f10 (369 bytes, then 90 padding; next known entry 0x00551090). File read (RtFSHandler read):
   overlapped ReadFile with a CreateEventA event when +0x18 is set, else a plain ReadFile that advances the
   64-bit position at +8/+0xc; GetLastError stored through FUN_00550740()+0x10. Moved the score: interface
   local (same as 0x00550950), the in-range branch first with `pos += got` and the overflow branch on a separate
   newpos (77.7 -> 82.0), `got` declared first so `n` takes param_1's dead stack slot as in the original, and
   the permuter's store order (96.4). Residual (b): the original stores state 2 inside both position branches;
   ours hoists the common store above the compare. Tried: unsigned lvalue on one store, `pos + got` in the
   in-range branch (92.1), goto out of the in-range branch, permuter 461 compiles. */
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
typedef struct Overlapped550da0 { unsigned long Internal; unsigned long InternalHigh; unsigned long Offset; unsigned long OffsetHigh; void *hEvent; } Overlapped550da0;
typedef int (*FsSeekFn)(int file, unsigned long n, int origin);
typedef struct { int f0; int f4; int f8; int fc; FsSeekFn seek; } FsIf;
typedef struct { char pad[0x28]; FsIf fsif; } Fs;
typedef struct { int f0; int f4; int f8; int fc; unsigned long err; } FsErr;
__declspec(dllimport) void *__stdcall CreateEventA(void *sa, int manual, int initial, const char *name);
__declspec(dllimport) unsigned long __stdcall GetLastError(void);
__declspec(dllimport) int __stdcall ReadFile(void *h, void *buf, unsigned long len, unsigned long *got, Overlapped550da0 *ov);
FsErr *FUN_00550740(void);
void FUN_00551550(int file);
// MATCH: 0x00550da0 FUN_00550da0
unsigned long FUN_00550da0(int param_1, void *param_2, unsigned long param_3)
{
  unsigned long got = 0;
  FsIf *fsif = &(*(Fs **)(param_1 + 0x38))->fsif;
  void *h = *(void **)(param_1 + 0x40);
  unsigned long n;
  void *ev;
  int ok;
  int pos;
  int newpos;

  if (*(int *)(param_1 + 0x18) != 0) {
    *(int *)(param_1 + 0x28) = 5;
  }
  if (*(int *)(param_1 + 0x20) != 2) {
    return 0;
  }
  if (*(int *)(param_1 + 0x18) != 0) {
    n = 0;
    ev = CreateEventA(0, 0, 0, 0);
    *(void **)(param_1 + 0x58) = ev;
    *(int *)(param_1 + 0x4c) = *(int *)(param_1 + 8);
    *(void **)(param_1 + 0x54) = ev;
    *(int *)(param_1 + 0x50) = *(int *)(param_1 + 0xc);
    ok = ReadFile(h, param_2, param_3, &n, (Overlapped550da0 *)(param_1 + 0x44));
    FUN_00550740()->err = GetLastError();
    if (ok) {
      if (n != param_3) {
        *(int *)(param_1 + 0x20) = 4;
      }
      fsif->seek(param_1, n, 2);
      *(int *)(param_1 + 0x20) = 2;
      FUN_00551550(param_1);
      return got;
    }
    if (FUN_00550740()->err != 0x3e5 && FUN_00550740()->err != 0) {
      *(int *)(param_1 + 0x20) = 4;
      return got;
    }
    *(int *)(param_1 + 0x20) = 3;
    return got;
  }
  *(int *)(param_1 + 0x20) = 3;
  if (ReadFile(h, param_2, param_3, &got, 0)) {
    pos = *(int *)(param_1 + 8);
    newpos = pos + got;
    if (newpos >= pos) {
      *(int *)(param_1 + 8) += got;
    } else {
      *(int *)(param_1 + 8) = newpos + 0x80000001;
      *(int *)(param_1 + 0xc) = *(int *)(param_1 + 0xc) + 1;
    }
    *(int *)(param_1 + 0x20) = 2;
  } else {
    *(int *)(param_1 + 0x20) = 4;
  }
  FUN_00550740()->err = GetLastError();
  return got;
}
