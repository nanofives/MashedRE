// FLAGS: /O2 /Ob1 /G5
/* h3 hand-match: best 95.6% (regs 96.5%), class a. Function-pointer stores as immediates via real prototypes (FUN_004c6140, LAB_004c61b0/7200/7490 declared as void fn) took it from 90.4 to 95.6. Residual: original keeps three separate return-0 epilogues (xor eax,eax; pop esi; ret), ours cross-jumps them into one at the end, plus one lea operand order; no statement order moved it, permuter no gain. */
/* MATCH-RWL01 0x004c5f60: raw g2s.py output (re/tools/match/g2s.py on decomp_pc.py --port --callees), body
   unchanged; globals moved to MGLOBAL. Score: see verify/matchrwl01_20261107/results.tsv. */
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
typedef unsigned char undefined1;
typedef unsigned short undefined2;
typedef unsigned int undefined4;
typedef unsigned int uint;
#ifdef __cplusplus
typedef int code(...);
#else
typedef int code();
#endif
MGLOBAL(undefined4, DAT_0061816c, 0x0061816c);
MGLOBAL(undefined4, DAT_00618168, 0x00618168);
MGLOBAL(int, DAT_007d4054, 0x007d4054);
MGLOBAL(undefined4, DAT_00618138, 0x00618138);
MGLOBAL(int, DAT_007d3ff8, 0x007d3ff8);
MGLOBAL(undefined4, DAT_00618174, 0x00618174);
MGLOBAL(undefined4, DAT_00618170, 0x00618170);
MGLOBAL(undefined4, DAT_00618150, 0x00618150);
MGLOBAL(int, DAT_007d4058, 0x007d4058);
MGLOBAL(undefined4, DAT_007d4080, 0x007d4080);
void LAB_004c61b0(void);
void LAB_004c7200(void);
void LAB_004c7490(void);
MGLOBAL(undefined1, DAT_007d4030, 0x007d4030);
MGLOBAL(undefined1, DAT_007d405c, 0x007d405c);
void FUN_004c6140(void);
undefined1 * FUN_004c5890(void);
undefined4 FUN_004cc9f0(int p0);
uint * thunk_FUN_004cc820(int p0, uint p1, uint p2, int p3, uint * p4, uint p5);
typedef struct RwLLLink { struct RwLLLink *next, *prev; } RwLLLink;
typedef struct { RwLLLink link; } RwLinkList;
typedef struct {
  RwLinkList texDictList; uint *textureFreeList; uint *texDictFreeList; undefined1 *currentTexDict;
  void *f14; void *f18; undefined4 f1c; undefined4 f20; undefined4 f24; undefined2 f28; undefined2 pad2a;
  void *f2c; void *f30;
} TG;
#define TGL(v) (((TG *)(((unsigned char *)DAT_007d3ff8) + DAT_007d4054))->v)
#define rwLinkListInitialize(list) { (list)->link.next = ((RwLLLink *)(list)); (list)->link.prev = ((RwLLLink *)(list)); }
// MATCH: 0x004c5f60 FUN_004c5f60
void * FUN_004c5f60(void *instance,int offset)
{
  DAT_007d4054 = offset;
  TGL(textureFreeList) = thunk_FUN_004cc820(DAT_00618138,DAT_00618168,4,DAT_0061816c,(uint *)&DAT_007d405c,0x40006);
  if (!TGL(textureFreeList)) {
    return 0;
  }
  TGL(texDictFreeList) = thunk_FUN_004cc820(DAT_00618150,DAT_00618170,4,DAT_00618174,(uint *)&DAT_007d4030,0x40408);
  if (!TGL(texDictFreeList)) {
    FUN_004cc9f0((int)TGL(textureFreeList));
    TGL(textureFreeList) = 0;
    return 0;
  }
  rwLinkListInitialize(&TGL(texDictList));
  DAT_007d4058 = DAT_007d4058 + 1;
  TGL(currentTexDict) = (undefined1 *)(DAT_007d4080 = (undefined4)FUN_004c5890());
  if (!TGL(currentTexDict)) {
    FUN_004cc9f0((int)TGL(texDictFreeList));
    TGL(texDictFreeList) = 0;
    FUN_004cc9f0((int)TGL(textureFreeList));
    TGL(textureFreeList) = 0;
    return 0;
  }
  TGL(f1c) = 0;
  TGL(f20) = 0;
  TGL(f18) = (void *)LAB_004c7490;
  TGL(f14) = (void *)LAB_004c61b0;
  TGL(f2c) = (void *)LAB_004c7200;
  TGL(f30) = (void *)FUN_004c6140;
  TGL(f24) = 0;
  TGL(f28) = 0;
  return instance;
}
