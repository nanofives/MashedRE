// FLAGS: /O2 /Ob1 /G5
/* MATCH-S24 0x00551090 (h1): 100% (regs 100%). FsFile struct; ops = &dev->ops (embedded at +0x28, as 0x00550980) taken
   before the test; state local 2/4 (ebx); the error path returns early on 0x3e5 / 0 / 0x3e4. */
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
typedef struct FsFile FsFile;
typedef struct FsOps {
  void *f0;
  void *f4;
  void *f8;
  void *read;
  void (*complete)(FsFile *file, unsigned long count, int status);
} FsOps;
typedef struct FsDev { char pad[0x28]; FsOps ops; } FsDev;
typedef struct FsErr { int f0; int f4; int f8; int fc; int code; } FsErr;
struct FsFile {
  int f0;
  int f4;
  int f8;
  int fc;
  int f10;
  int f14;
  int busy;
  int f1c;
  int state;
  int f24;
  int mode;
  int result;
  int f30;
  int f34;
  FsDev *dev;
  int f3c;
  void *handle;
  int overlapped[5];
};
__declspec(dllimport) int __stdcall GetOverlappedResult(void *hFile, void *ov, unsigned long *count, int wait);
__declspec(dllimport) unsigned long __stdcall GetLastError(void);
int FUN_004d7ff0(int a, int b);
FsErr *FUN_00550740(void);
void FUN_00551550(FsFile *file);

// MATCH: 0x00551090 FUN_00551090
int FUN_00551090(FsFile *param_1, int param_2)
{
  FsOps *ops;
  unsigned long count;
  int state;
  unsigned long err;

  ops = &param_1->dev->ops;
  if (param_1->busy != 0 && param_1->state == 3) {
    count = 0;
    state = 2;
    param_1->result = FUN_004d7ff0(param_1->f0, param_1->f4);
    if (param_1->mode == 5) {
      if (!GetOverlappedResult(param_1->handle, param_1->overlapped, &count, param_2)) {
        err = GetLastError();
        FUN_00550740()->code = err;
        if (FUN_00550740()->code == 0x3e5 || FUN_00550740()->code == 0 ||
            FUN_00550740()->code == 0x3e4) {
          return param_1->state;
        }
        state = 4;
        param_1->result = 0;
      }
      else {
        ops->complete(param_1, count, 2);
        if (param_1->mode == 5) {
          param_1->result = count;
        }
      }
    }
    param_1->state = state;
    FUN_00551550(param_1);
  }
  return param_1->state;
}
