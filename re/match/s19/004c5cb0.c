// FLAGS: /O2 /GS /Ob1 /G5
/* MATCH-S19 0x004c5cb0 (h6): 89.4%, regs 90.6%. RwTextureRead shape: find slot (texture globals +0x18) with refCount++ early return, read slot (+0x14), RWERROR local struct with an if/else on maskName (the ternary was 63.5), RwTexDictionaryAddTexture inlined via the link macros. Residual (b): the original pushes edi in the prologue (before the find call), 9176 shrink-wraps it past the early return (same class as 0x004c5930). Tried: single-exit if/else-if form (35.6), function-scope RwError, assign-in-condition, double currentTexDict read, flags (game 81.2, /G6 81.2, /Oy- 73.3), permuter 99 compiles. cxx ok. */
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
typedef struct RwTexture { void *raster; RwTexDictionary *dict; RwLLLink lInDictionary; char name[32]; char mask[32]; unsigned int filterAddressing; int refCount; } RwTexture;
typedef RwTexture *(*RwTextureFindFn)(const char *);
typedef RwTexture *(*RwTextureReadFn)(const char *, const char *);
typedef struct { RwLinkList texDictList; void *texFreeList; void *texDictFreeList; RwTexDictionary *currentTexDict; RwTextureReadFn readFunc; RwTextureFindFn findFunc; } TG;
#define TGL(v) (((TG *)(((unsigned char *)DAT_007d3ff8) + DAT_007d4054))->v)
#define rwLinkListAddLLLink(list, linkvar) { (linkvar)->next = (list)->link.next; (linkvar)->prev = &(list)->link; ((list)->link.next)->prev = (linkvar); (list)->link.next = (linkvar); }
#define rwLinkListRemoveLLLink(linkvar) { ((linkvar)->prev)->next = (linkvar)->next; ((linkvar)->next)->prev = (linkvar)->prev; }
typedef struct { int pluginID; int errorCode; } RwError;
MGLOBAL(int, DAT_007d3ff8, 0x007d3ff8);
MGLOBAL(int, DAT_007d4054, 0x007d4054);
MGLOBALA(char, s__null__00618178, 7, 0x00618178);
int FUN_004d7ff0(int code, ...);
RwError *FUN_004d8480(RwError *p0);
// MATCH: 0x004c5cb0 FUN_004c5cb0
RwTexture *FUN_004c5cb0(const char *name, const char *maskName)
{
  RwTexture *texture;
  RwTexDictionary *dict;

  texture = TGL(findFunc)(name);
  if (texture) {
    texture->refCount++;
    return texture;
  }
  texture = TGL(readFunc)(name, maskName);
  if (!texture) {
    RwError err;
    err.pluginID = 1;
    if (maskName) {
      err.errorCode = FUN_004d7ff0(0x16, name, maskName);
      FUN_004d8480(&err);
    } else {
      err.errorCode = FUN_004d7ff0(0x16, name, s__null__00618178);
      FUN_004d8480(&err);
    }
    return 0;
  }
  dict = TGL(currentTexDict);
  if (dict) {
    if (texture->dict) {
      rwLinkListRemoveLLLink(&texture->lInDictionary);
    }
    texture->dict = dict;
    rwLinkListAddLLLink(&dict->texturesInDict, &texture->lInDictionary);
  }
  return texture;
}
