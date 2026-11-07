// FLAGS: /O2 /Ob1 /G5
/* MATCH-S24 h3 0x00550f20: no Ghidra size; nosize.py: acc 90.6, regs 97.2, 296 B, ends 0x00551048 on nop padding (next known entry 0x00551090; 0x00551050 follows the padding). RtFSHandler write method (stored at handler+0x34 by 0x00551190). Return value is the local written, which the compiler homes in param_1's stack slot (that is why Ghidra shows return param_1). Residual (b): register choice for the WriteFile argument loads (ours ecx/edx/eax vs edx/eax/ecx in both paths) and the order of the pos/posHigh loads after CreateEventA. Tried: OffsetHigh before Offset (this file, +1.9), Offset first (88.7, regs 99.1), event via a separate statement, U64 struct copy and __int64 for pos/Offset, handle read in place, locals hoisted, explicit ok2 temp, int len; permuter 300 s (best 91.5% only by moving the Offset stores after WriteFile, which changes behaviour, rejected). */
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
typedef struct OVL { unsigned long Internal, InternalHigh, Offset, OffsetHigh; void *hEvent; } OVL;
typedef struct File File;
typedef struct FsOps {
  int pad[4];
  void (__cdecl *cb)(File *, unsigned long, int);  /* ops +0x10 (fs +0x38) */
} FsOps;
typedef struct Fs { char pad[0x28]; FsOps ops; } Fs;
struct File {
  int pad00[2];
  unsigned long pos;     /* +0x08 */
  unsigned long posHigh; /* +0x0c */
  int pad10[2];
  int async;             /* +0x18 */
  int pad1c;
  int status;            /* +0x20 */
  int pad24[5];
  Fs *fs;                /* +0x38 */
  int pad3c;
  void *handle;          /* +0x40 */
  OVL ov;                /* +0x44 */
  void *event;           /* +0x58 */
};
typedef struct ErrCtx { int pad[4]; unsigned long err; } ErrCtx;
__declspec(dllimport) void *__stdcall CreateEventA(void *lpEventAttributes, int bManualReset, int bInitialState, const char *lpName);
__declspec(dllimport) unsigned long __stdcall GetLastError(void);
__declspec(dllimport) int __stdcall WriteFile(void *hFile, const void *lpBuffer, unsigned long nNumberOfBytesToWrite, unsigned long *lpNumberOfBytesWritten, OVL *lpOverlapped);
ErrCtx *__cdecl FUN_00550740(void);
// MATCH: 0x00550f20 FUN_00550f20
unsigned long FUN_00550f20(File *file, const void *buf, unsigned long len)
{
  unsigned long written;
  FsOps *ops = &file->fs->ops;
  void *h = file->handle;
  if (file->async) {
    unsigned long n = 0;
    int ok;
    file->ov.hEvent = file->event = CreateEventA(0, 0, 0, 0);
    file->ov.OffsetHigh = file->posHigh;
    file->ov.Offset = file->pos;
    ok = WriteFile(h, buf, len, &n, &file->ov);
    FUN_00550740()->err = GetLastError();
    if (ok) {
      if (n != len) file->status = 4;
      ops->cb(file, n, 2);
      file->status = 2;
      return written;
    }
    if (FUN_00550740()->err == 0x3e5 || FUN_00550740()->err == 0) {
      file->status = 3;
      return written;
    }
    file->status = 4;
    return written;
  }
  file->status = 3;
  if (WriteFile(h, buf, len, &written, 0)) {
    file->status = 2;
    file->pos += written;
    return written;
  }
  FUN_00550740()->err = GetLastError();
  file->status = 4;
  return written;
}
