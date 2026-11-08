// FLAGS: /O2 /Ob1 /G5
/* MATCH-S21 0x004e1df0 (h1, by hand): best 80.9% exact, regs 80.9% (raw 52.1%). Extension-chunk skip: find chunk 3
   (0x004cc5e0), then per sub-chunk read header (0x004cc400) and skip its length (0x004cc050), size -= 12 + len.
   Moves: `register` stream copy, which puts the stream in esi and the size in the dead argument slot (52.1 -> 80.9).
   Residual: block layout only. The original places the shared `xor eax,eax; ret` right after the first call (jne
   over it); 9178 puts it at the end. Tried: early returns, nested if, for(;;)+break (raw shape), goto fail, ||
   merge, `while (size)`, four spellings of the size update, permuter (89 compiles, no gain). Class (a). C++ ok. */
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
int FUN_004cc050(int stream, uint size);
int FUN_004cc400(int stream, uint *type, uint *length, uint *version, uint *build);
int FUN_004cc5e0(int stream, uint type, uint *length, uint *version);
// MATCH: 0x004e1df0 FUN_004e1df0
int FUN_004e1df0(int param_1,int param_2)

{
  uint size;
  uint len;
  register int stream = param_2;

  if (!FUN_004cc5e0(stream,3,&size,0)) {
    return 0;
  }
  while (size > 0) {
    if (!FUN_004cc400(stream,0,&len,0,0)) {
      return 0;
    }
    if (!FUN_004cc050(stream,len)) {
      return 0;
    }
    size -= 12 + len;
  }
  return param_1;
}
