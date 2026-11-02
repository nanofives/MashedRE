// FLAGS: /O2 /GS /Ob1 /G5
/* MATCH-S19 0x004c59c0 (h4): 100% (regs 100%), relocheck OK. RW ForAllTextures loop (while cur != end, next read before the callback, break on NULL). FLAGS add /G5: this RW-core code matches only with /G5 (also /G3,/G4); under the game flags /O2 /GS /Ob1 alone it is 85.7% (loop-head alignment pad + first-load schedule). */
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
typedef struct RwLLLink { struct RwLLLink *next, *prev; } RwLLLink;
typedef struct { RwLLLink link; } RwLinkList;
typedef struct { unsigned char type, subType, flags, privateFlags; void *parent; } RwObject;
typedef struct { RwObject object; RwLinkList texturesInDict; RwLLLink lInInstance; } RwTexDictionary;
typedef struct { void *raster; void *dict; RwLLLink lInDictionary; } RwTexture;
typedef RwTexture *(*RwTextureCallBack)(RwTexture *, void *);
// MATCH: 0x004c59c0 FUN_004c59c0
RwTexDictionary * FUN_004c59c0(RwTexDictionary *dict,RwTextureCallBack fpCallBack,void *pData)
{
  RwLLLink *cur = dict->texturesInDict.link.next;
  RwLLLink *end = &dict->texturesInDict.link;
  while (cur != end) {
    RwLLLink *next = cur->next;
    if (!fpCallBack((RwTexture *)((char *)cur - 8), pData)) break;
    cur = next;
  }
  return dict;
}
