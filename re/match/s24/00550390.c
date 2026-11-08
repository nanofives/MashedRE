// FLAGS: /O2 /Ob1 /G5
/* MATCH-S24 0x00550390 (h1): 100% (regs 100%). while-loop over the handler list with next saved first; frees through RwEngine slot +0x10c. relocheck unchecked=1 is the DeleteCriticalSection import. */
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
typedef struct CritSec { int w[6]; } CritSec;
typedef struct FsNode {
  struct FsNode *next;
  int f4;
  int f8;
  void *buf;
  int f10;
  int f14;
  void (*close)(struct FsNode *self);
} FsNode;
typedef void (*FreeFn)(void *p);
__declspec(dllimport) void __stdcall DeleteCriticalSection(CritSec *cs);
void FUN_00551510(FsNode *node, int mode);
MGLOBAL(int, DAT_007dc760, 0x007dc760);
MGLOBAL(FsNode *, DAT_007dc754, 0x007dc754);
MGLOBAL(int, DAT_007d3ff8, 0x007d3ff8);
MGLOBAL(int, DAT_007dc75c, 0x007dc75c);
MGLOBAL(CritSec, DAT_009124e0, 0x009124e0);

// MATCH: 0x00550390 FUN_00550390
void FUN_00550390(void)
{
  FsNode *node;
  FsNode *next;

  if (DAT_007dc760 == 1) {
    node = DAT_007dc754;
    while (node != 0) {
      next = node->next;
      node->close(node);
      (*(FreeFn *)(DAT_007d3ff8 + 0x10c))(node->buf);
      node->buf = 0;
      FUN_00551510(node, 2);
      (*(FreeFn *)(DAT_007d3ff8 + 0x10c))(node);
      node = next;
    }
    DeleteCriticalSection(&DAT_009124e0);
    DAT_007dc760 = 0;
    DAT_007dc75c = 0;
  }
}
