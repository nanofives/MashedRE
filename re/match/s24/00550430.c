// FLAGS: /O2 /Ob1 /G5
/* MATCH-S24 0x00550430 (h4): 100.0% (regs 100.0%). File-system register: refuse with error callback 5 when the
   count limit is hit, refuse when the name (node+0x50, compared through RwEngine slot +0xe8) is already listed,
   else link at the list head. The lookup is a static __forceinline helper that calls the error callback with 6
   when not found; with it inlined the found path shares the first return 0 and the pushes stay at entry.
   Tried: hand-inlined loop with break (82.5%), Ghidra goto form (72.6%), result variable (75.4%); a plain
   __inline helper is not inlined under /Ob1 (23%). The helper is not a real callee (no MATCH line). */
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
typedef struct FsNode {
  struct FsNode *next;
  int f4[19];
  char name[1];
} FsNode;
typedef void (*FsErrCb)(int code);
typedef int (*StrCmpFn)(const char *a, const char *b);
MGLOBAL(int, DAT_007dc758, 0x007dc758);
MGLOBAL(int, DAT_007dc75c, 0x007dc75c);
MGLOBAL(FsErrCb, DAT_007dc768, 0x007dc768);
MGLOBAL(FsNode *, DAT_007dc754, 0x007dc754);
MGLOBAL(int, DAT_007d3ff8, 0x007d3ff8);
void FUN_00551510(FsNode *node, int mode);
static __forceinline FsNode *FsManagerFind_00550430(const char *name)
{
  FsNode *node = DAT_007dc754;
  while (node != 0) {
    if ((*(StrCmpFn *)(DAT_007d3ff8 + 0xe8))(node->name, name) == 0) return node;
    node = node->next;
  }
  if (DAT_007dc768 != 0) DAT_007dc768(6);
  return 0;
}
// MATCH: 0x00550430 FUN_00550430
int FUN_00550430(FsNode *param_1)
{
  if (DAT_007dc758 > 0 && DAT_007dc75c + 1 > DAT_007dc758) {
    if (DAT_007dc768 != 0) DAT_007dc768(5);
  } else if (FsManagerFind_00550430(param_1->name) == 0) {
    FUN_00551510(param_1, 1);
    param_1->next = DAT_007dc754;
    DAT_007dc754 = param_1;
    DAT_007dc75c++;
    return 1;
  }
  return 0;
}
