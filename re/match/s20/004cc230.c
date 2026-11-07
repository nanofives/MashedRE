// FLAGS: /O2 /Ob1 /G5
/* MATCH-S20 0x004cc230 (h3): 48.0% (regs 45.4%); jump table at 0x004cc3e8 past Ghidra's size. FLAGS /G5 (no cookie). RwStreamOpen shape: RwFreeListAlloc (RwGlobals+0x118, freelist at module+0 with module = RwGlobals + DAT_007d45c4, hint 0x30404); type/access/rwOwned=1; switch type: 1 file (FUN_00550bc0(fp) != -1), 2 filename (mode string 0x0061737c/0x006182a8/0x006182ac by access, FUN_005507b0 open, RWERROR 0xd / 0x80000002), 3 memory (read/write/append position/size/block from {start,length}), 4 custom (5-dword copy), default RWERROR 0xe; on failure RwFreeListFree (+0x11c) and NULL. Residual: the original keeps a separate result register (esi) that it tests at one merge point (only the case-1 and default failures jump straight to the free), uses 4 separate RwError slots (frame 0x20), and ours threads result==stream straight to the returns (fewer blocks, one shared RwError slot, frame 8). Tried: if(stream) nesting, goto-fail form, a static __inline initialize helper (not inlined), RWERROR do/while macro, permuter 764 compiles (this file: it reorders the RwError field stores, 34.7% -> 48.0%). Class (b). */
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
void *memcpy(void *d, const void *s, unsigned int n);
#pragma intrinsic(memcpy)
typedef struct RwError { int pluginID; int errorCode; } RwError;
typedef struct RwMemory { unsigned char *start; unsigned int length; } RwMemory;
typedef struct RwStreamMemory { unsigned int position; unsigned int nSize; unsigned char *memBlock; } RwStreamMemory;
typedef struct RwStreamCustom { void *a; void *b; void *c; void *d; void *e; } RwStreamCustom;
typedef union RwStreamUnion {
  RwStreamMemory memory;
  void *fpFile;
  RwStreamCustom custom;
} RwStreamUnion;
typedef struct RwStream {
  int type;
  int accessType;
  int position;
  RwStreamUnion Type;
  int rwOwned;
} RwStream;
typedef struct Mod { void *freeList; } Mod;
typedef void *(*fpFreeListAlloc)(void *fl, unsigned int hint);
typedef void (*fpFreeListFree)(void *fl, void *mem);
MGLOBAL(int, DAT_007d3ff8, 0x007d3ff8);
MGLOBAL(int, DAT_007d45c4, 0x007d45c4);
MGLOBALA(char, DAT_0061737c, 4, 0x0061737c);
MGLOBALA(char, DAT_006182a8, 4, 0x006182a8);
MGLOBALA(char, DAT_006182ac, 4, 0x006182ac);
int FUN_004d7ff0(int code, ...);
RwError *FUN_004d8480(RwError *err);
void *FUN_005507b0(const char *name, const char *mode);
int FUN_00550bc0(void *fp);
#define MOD ((Mod *)(DAT_007d3ff8 + DAT_007d45c4))

// MATCH: 0x004cc230 FUN_004cc230
RwStream *FUN_004cc230(int param_1, int param_2, void *param_3)
{
  RwStream *stream;
  RwStream *result;

  stream = (RwStream *)(*(fpFreeListAlloc *)(DAT_007d3ff8 + 0x118))(MOD->freeList, 0x30404);
  if (!stream) {
    goto fail;
  }
  {
    stream->type = param_1;
    stream->accessType = param_2;
    stream->rwOwned = 1;
    switch (param_1) {
    case 1:
      if (FUN_00550bc0(param_3) == -1) {
        result = 0;
      } else {
        stream->Type.fpFile = param_3;
        result = stream;
      }
      break;
    case 2: {
      const char *mode;
      void *fp;
      result = 0;
      switch (param_2) {
      case 1:
        mode = DAT_0061737c;
        break;
      case 2:
        mode = DAT_006182a8;
        break;
      case 3:
        mode = DAT_006182ac;
        break;
      default: {
        RwError err;
        err.errorCode = FUN_004d7ff0(0xd);
        FUN_004d8480(&err);
        err.pluginID = 1;
        mode = 0;
        break;
      }
      }
      if (mode && (fp = FUN_005507b0((const char *)param_3, mode)) != 0) {
        stream->Type.fpFile = fp;
        result = stream;
      } else {
        RwError err;
        err.errorCode = FUN_004d7ff0(0x80000002, param_3);
        FUN_004d8480(&err);
        err.pluginID = 1;
      }
      break;
    }
    case 3:
      result = 0;
      switch (param_2) {
      case 1:
        stream->Type.memory.position = 0;
        stream->Type.memory.nSize = ((RwMemory *)param_3)->length;
        stream->Type.memory.memBlock = ((RwMemory *)param_3)->start;
        result = stream;
        break;
      case 2:
        stream->Type.memory.position = 0;
        stream->Type.memory.nSize = 0;
        stream->Type.memory.memBlock = 0;
        result = stream;
        break;
      case 3:
        stream->Type.memory.position = ((RwMemory *)param_3)->length;
        stream->Type.memory.nSize = ((RwMemory *)param_3)->length;
        stream->Type.memory.memBlock = ((RwMemory *)param_3)->start;
        result = stream;
        break;
      default: {
        RwError err;
        FUN_004d8480(&err);
        err.errorCode = FUN_004d7ff0(0xd);
        err.pluginID = 1;
        break;
      }
      }
      break;
    case 4:
      stream->Type.custom = *(RwStreamCustom *)param_3;
      result = stream;
      break;
    default: {
      RwError err;
      FUN_004d8480(&err);
      err.errorCode = FUN_004d7ff0(0xe);
      err.pluginID = 1;
      result = 0;
      break;
    }
    }
    if (result) {
      return stream;
    }
  }
fail:
  (*(fpFreeListFree *)(DAT_007d3ff8 + 0x11c))(MOD->freeList, stream);
  return 0;
}
