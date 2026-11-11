// TOOLCHAIN rw9044
// FLAGS: /O2 /GS /Ob1 /G5
/* MATCH-S19 0x004c5930 (h4): 89.1% (regs 91.3%). RwTexDictionaryDestroy shape: clear current dict (plugin +0x10), ForAll loop calling FUN_004c5a60(tex,0), FUN_004d8060 plugin deinit, unlink lInInstance, freelist free slot +0x11c (plugin +0xc). Residual: 9176 shrink-wraps push/pop esi around the loop only; the original pushes esi in the prologue. Tried function-scope locals, for form, dict copy, __forceinline ForAll helper with the callback, permuter 92 compiles. relocheck BAD only from the shifted offsets. h6 resume: inline loop in the body, do-while with pre-check, function-scope next, __inline (not inlined: 41%), flags (no /G5 78.7, /Ob2 and /GS- same 89.1): no move. Class (b). */
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
typedef struct { void *raster; RwTexDictionary *dict; RwLLLink lInDictionary; char name[32]; char mask[32]; unsigned int filterAddressing; int refCount; } RwTexture;
#define rwLinkListAddLLLink(list, linkvar) { (linkvar)->next = (list)->link.next; (linkvar)->prev = &(list)->link; ((list)->link.next)->prev = (linkvar); (list)->link.next = (linkvar); }
#define rwLinkListRemoveLLLink(linkvar) { ((linkvar)->prev)->next = (linkvar)->next; ((linkvar)->next)->prev = (linkvar)->prev; }
typedef struct { RwLinkList texDictList; void *texFreeList; void *texDictFreeList; void *currentTexDict; } TG;
#define TGL(v) (((TG *)(((unsigned char *)DAT_007d3ff8) + DAT_007d4054))->v)
typedef void (*fpfree)(void *, void *);
MGLOBAL(int, DAT_007d3ff8, 0x007d3ff8);
MGLOBAL(int, DAT_007d4054, 0x007d4054);
MGLOBALA(char, DAT_00618150, 4, 0x00618150);
RwTexture * FUN_004c5a60(RwTexture *tex, void *data);
int FUN_004d8060(void *p0, void *p1);
typedef RwTexture *(*RwTextureCallBack)(RwTexture *, void *);
static __forceinline RwTexDictionary *ForAll(RwTexDictionary *dict, RwTextureCallBack fp, void *data)
{
  RwLLLink *cur = dict->texturesInDict.link.next;
  RwLLLink *end = &dict->texturesInDict.link;
  while (cur != end) {
    RwLLLink *next = cur->next;
    if (!fp((RwTexture *)((char *)cur - 8), data)) break;
    cur = next;
  }
  return dict;
}
// MATCH: 0x004c5930 FUN_004c5930
int FUN_004c5930(RwTexDictionary *dict)
{
  if (TGL(currentTexDict) == dict) TGL(currentTexDict) = 0;
  ForAll(dict, FUN_004c5a60, 0);
  FUN_004d8060(DAT_00618150, dict);
  rwLinkListRemoveLLLink(&dict->lInInstance);
  (*(fpfree *)((char *)DAT_007d3ff8 + 0x11c))(TGL(texDictFreeList), dict);
  return 1;
}
