// FLAGS: /O2 /GS /Ob1 /G5
/* MATCH-S19 0x004c5890 (h4): 100% (regs 100%), relocheck OK. RwTexDictionary-create shape: freelist alloc through RwGlobals +0x118, rwObjectInitialize(6,0), rwLinkListAddLLLink into the module list at plugin offset DAT_007d4054, rwLinkListInitialize (prev first), FUN_004d8000. FLAGS add /G5: this RW-core code matches only with /G5 (also /G3,/G4); under the game flags /O2 /GS /Ob1 alone it is 75.6% (final-call pushes hoisted over link stores). */
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
typedef struct { RwLinkList texDictList; char pad[4]; void *freelist; } TG;
typedef void *(*fpm)(void *, int);
MGLOBAL(void *, DAT_007d3ff8, 0x007d3ff8);
MGLOBAL(int, DAT_007d4054, 0x007d4054);
MGLOBALA(char, DAT_00618150, 4, 0x00618150);
int FUN_004d8000(void *p0, void *p1);
#define rwObjectInitialize(o,t,s) { ((RwObject *)(o))->type = (unsigned char)(t); ((RwObject *)(o))->subType = (unsigned char)(s); ((RwObject *)(o))->flags = 0; ((RwObject *)(o))->privateFlags = 0; ((RwObject *)(o))->parent = 0; }
#define TGL(v) (((TG *)(((unsigned char *)DAT_007d3ff8) + DAT_007d4054))->v)
#define rwLinkListAddLLLink(list, linkvar) { (linkvar)->next = (list)->link.next; (linkvar)->prev = &(list)->link; ((list)->link.next)->prev = (linkvar); (list)->link.next = (linkvar); }
#define rwLinkListInitialize(list) { (list)->link.prev = ((RwLLLink *)(list)); (list)->link.next = ((RwLLLink *)(list)); }
// MATCH: 0x004c5890 FUN_004c5890
RwTexDictionary * FUN_004c5890(void)
{
  RwTexDictionary *d;
  d = (RwTexDictionary *)(*(fpm *)((char *)DAT_007d3ff8 + 0x118))(TGL(freelist),0x30016);
  if (!d) return 0;
  rwObjectInitialize(d, 6, 0);
  rwLinkListAddLLLink(&TGL(texDictList), &d->lInInstance);
  rwLinkListInitialize(&d->texturesInDict);
  FUN_004d8000(DAT_00618150, d);
  return d;
}
