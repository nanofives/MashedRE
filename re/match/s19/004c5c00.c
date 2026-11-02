// FLAGS: /O2 /GS /Ob1 /G5
/* MATCH-S19 0x004c5c00 (h4): 100% (regs 100%), relocheck OK. RwTexDictionaryFindNamedTexture shape: ForAll loop over texturesInDict, NULL check on tex->name, then a case-insensitive compare in a static __forceinline helper (TexNameEq, not a game function, no MATCH line). The inlined parameter copies (mov esi,ecx / mov edi,ebp) need the helper: an open-coded loop gave 74-87%, static __inline was not inlined under /Ob1. Helper must return 1 on *a == *b. */
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
static __forceinline int TexNameEq(const char *a, const char *b)
{
  char c1;

  c1 = *a;
  while (c1 && *b) {
    char c2 = *b;
    if (c1 >= 'a' && c1 <= 'z') c1 -= 0x20;
    if (c2 >= 'a' && c2 <= 'z') c2 -= 0x20;
    if (c1 != c2) return 0;
    a++; b++; c1 = *a;
  }
  if (*a == *b) return 1; return 0;
}
// MATCH: 0x004c5c00 FUN_004c5c00
RwTexture * FUN_004c5c00(RwTexDictionary *dict,const char *name)
{
  RwLLLink *cur = dict->texturesInDict.link.next;
  RwLLLink *end = &dict->texturesInDict.link;
  while (cur != end) {
    RwTexture *tex = (RwTexture *)((char *)cur - 8);
    if (tex->name && TexNameEq(tex->name, name)) return tex;
    cur = cur->next;
  }
  return 0;
}
