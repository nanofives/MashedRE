// FLAGS: /O2 /Ob1 /G5
/* MATCH-S24 0x005504d0 (h1): 100% (regs 100%). for-loop over the handler list, string compare through RwEngine slot +0xe8. */
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
typedef int (*StrCmpFn)(const char *a, const char *b);
typedef void (*ErrFn)(int code);
typedef struct FsNode { struct FsNode *next; char pad[0x4c]; char name[1]; } FsNode;
MGLOBAL(FsNode *, DAT_007dc754, 0x007dc754);
MGLOBAL(int, DAT_007d3ff8, 0x007d3ff8);
MGLOBAL(ErrFn, DAT_007dc768, 0x007dc768);

// MATCH: 0x005504d0 FUN_005504d0
FsNode *FUN_005504d0(const char *param_1)
{
  FsNode *node;

  for (node = DAT_007dc754; node != 0; node = node->next) {
    if ((*(StrCmpFn *)(DAT_007d3ff8 + 0xe8))(node->name, param_1) == 0) {
      return node;
    }
  }
  if (DAT_007dc768 != 0) {
    DAT_007dc768(6);
  }
  return 0;
}
