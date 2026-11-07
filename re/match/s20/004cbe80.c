// FLAGS: /O2 /Ob1 /G5
/* MATCH-S20 0x004cbe80 (h1): 78.6% (regs 93.1%). Raw converter 34.6%.
   Residual: register rotation (eax/ecx/edx) through most of the body, and per-block scheduling of the RWERROR
   stores. Our cl tail-merges identical RWERROR blocks; the original keeps each block separate with different
   register choices. The pluginID store order differs between the blocks to stop the merge (permuter: 56.6% ->
   78.0%), and an RwStreamMemory pointer local (+0.6). Tried: game flags (62.9%), memcpy operand forms, signed
   length, permuter 533 compiles. Class (b). Written in the shape of RW 3.6 RwStreamWrite: switch on stream->type;
   1/2 (file): FUN_00550980(buffer, 1, length, file at +0xc) != length -> RWERROR 0x8000001c, NULL; 3 (memory):
   allocate 0x200 bytes through RwGlobals+0x108 (hint 0x30404) when +0x14 is NULL (RWERROR 0x80000013 with 0x200 on
   failure), grow through RwGlobals+0x110 (block, nSize + max(0x200 when length < 0x200, length), 0x1030404) when
   nSize - position < length (RWERROR 0x80000013 with the growth), then copy and advance +0xc; 4 (custom): +0x14
   callback (+0x1c data, buffer, length) nonzero -> stream; other types RWERROR 0xe. RWERROR = {1,
   FUN_004d7ff0(code, ...)} passed to FUN_004d8480. The jump table at 0x004cc040 follows the Ghidra size.
   FLAGS /G5 (RW core). */
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
typedef struct RwError { int pluginID; int errorCode; } RwError;
typedef int (*CustWriteFn)(void *data, const void *buf, unsigned int len);
typedef struct RwStreamMemory { unsigned int position; unsigned int nSize; unsigned char *memBlock; } RwStreamMemory;
typedef struct RwStream {
  int type; int accessType; int position;
  union {
    RwStreamMemory memory;
    struct { void *fpFile; } file;
    struct { void *sfnclose; void *sfnread; CustWriteFn sfnwrite; void *sfnskip; void *data; } custom;
  } Type;
} RwStream;
typedef void *(*RwMallocFn)(unsigned int size, unsigned int hint);
typedef void *(*RwReallocFn)(void *p, unsigned int size, unsigned int hint);
MGLOBAL(int, DAT_007d3ff8, 0x007d3ff8);
int FUN_004d7ff0(int code, ...);
RwError *FUN_004d8480(RwError *err);
unsigned int FUN_00550980(const void *buf, unsigned int size, unsigned int n, void *fp);
void *memcpy(void *, const void *, unsigned int);
#pragma intrinsic(memcpy)

// MATCH: 0x004cbe80 FUN_004cbe80
RwStream *FUN_004cbe80(RwStream *stream, const void *buffer, unsigned int length)
{
  switch (stream->type) {
  case 1:
  case 2:
    if (FUN_00550980(buffer, 1, length, stream->Type.file.fpFile) != length) {
      RwError err;
      err.pluginID = 1;
      err.errorCode = FUN_004d7ff0(0x8000001c);
      FUN_004d8480(&err);
      return 0;
    }
    return stream;
  case 3:
    {
      RwStreamMemory *mem = &stream->Type.memory;
      if (mem->memBlock == 0) {
        mem->memBlock = (unsigned char *)(*(RwMallocFn *)(DAT_007d3ff8 + 0x108))(0x200, 0x30404);
        if (mem->memBlock == 0) {
          RwError err;
          err.errorCode = FUN_004d7ff0(0x80000013, 0x200);
          err.pluginID = 1;
          FUN_004d8480(&err);
          return 0;
        }
        mem->nSize = 0x200;
      }
      if (mem->nSize - mem->position < length) {
        unsigned char *newBlock;
        unsigned int newSize = mem->nSize + 0x200;
        if (length >= 0x200) {
          newSize = mem->nSize + length;
        }
        newBlock = (unsigned char *)(*(RwReallocFn *)(DAT_007d3ff8 + 0x110))(mem->memBlock, newSize, 0x1030404);
        if (newBlock == 0) {
          RwError err;
          err.pluginID = 1;
          err.errorCode = FUN_004d7ff0(0x80000013, newSize - mem->nSize);
          FUN_004d8480(&err);
          return 0;
        }
        mem->memBlock = newBlock;
        mem->nSize = newSize;
      }
      memcpy(mem->memBlock + mem->position, buffer, length);
      mem->position += length;
      return stream;
    }
  case 4:
    return stream->Type.custom.sfnwrite(stream->Type.custom.data, buffer, length) ? stream : 0;
  default:
    {
      RwError err;
      FUN_004d8480(&err);
      err.errorCode = FUN_004d7ff0(0xe);
      err.pluginID = 1;
      return 0;
    }
  }
}
