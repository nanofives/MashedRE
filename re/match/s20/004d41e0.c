// FLAGS: /O2 /Ob1 /G5
/* MATCH-S20 0x004d41e0 (h4): 100% (regs 100%). Destroys a container: FUN_004d3e90(entry, container) for each of
   numEntries (+4) 0x28-byte entries (+8), entries = NULL, frees +0x20 through RwGlobals+0x10c and clears +0x20/+0x24,
   then returns the container to the free list at *(RwGlobals + DAT_00911ae0) through RwGlobals+0x11c.
   What moved it: entry count read once into a local (70.7 -> 100). */
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
typedef void (*FreeFn)(void *p);
typedef void (*FreeToListFn)(void *list, void *block);
typedef struct Entry { int data[10]; } Entry;
typedef struct Container {
  int field_0; unsigned int numEntries; Entry *entries; int field_c; int field_10; int field_14; int field_18;
  int field_1c; void *buffer; int bufferSize;
} Container;
MGLOBAL(int, DAT_007d3ff8, 0x007d3ff8);
MGLOBAL(int, DAT_00911ae0, 0x00911ae0);
void *FUN_004d3e90(Entry *entry, Container *container);

// MATCH: 0x004d41e0 FUN_004d41e0
void FUN_004d41e0(Container *c)
{
  if (c) {
    unsigned int i;
    unsigned int n = c->numEntries;
    Entry *e = c->entries;
    for (i = 0; i < n; i++) {
      FUN_004d3e90(e, c);
      e++;
    }
    c->entries = 0;
    if (c->buffer) {
      (*(FreeFn *)(DAT_007d3ff8 + 0x10c))(c->buffer);
      c->buffer = 0;
      c->bufferSize = 0;
    }
    (*(FreeToListFn *)(DAT_007d3ff8 + 0x11c))(*(void **)(DAT_007d3ff8 + DAT_00911ae0), c);
  }
}
