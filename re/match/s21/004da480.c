// FLAGS: /O2 /Ob1 /G5
/* MATCH-S21 0x004da480 (h2): 100% (regs 100%), relocheck OK. Recursive walk of the 16-way tree (+0x1c child[16],
   +0x18 byte index): at depth 0 a non-NULL leaf gets FUN_004d9a60(&pal[idx], leaf), stores idx at +0x18 and
   idx++; inner nodes recurse over all 16 children with depth - 1; returns the running index.
   What moved it: one `if (n) { ... } return idx;` with `idx++` (early returns gave 72.3%: ebx push shrink-wrapped
   past the NULL return and `lea eax,[ebx+1]`).
   match.py --bytes reports 1 bad byte at +0x2e: the self-recursive `call 0x004da480` REL32 site. byte_compare
   checks same-section relocations as absolute targets (tgt + addend vs orig - addr), which does not hold for a
   relative call to the function itself; every other byte is identical and relocheck is OK. */
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
typedef struct RwRGBA { unsigned char red, green, blue, alpha; } RwRGBA;
typedef struct Node { char pad[0x18]; unsigned char val; char pad2[3]; struct Node *child[16]; } Node;
void FUN_004d9a60(RwRGBA *out, Node *n);
// MATCH: 0x004da480 FUN_004da480
int FUN_004da480(Node *n, RwRGBA *pal, int idx, int depth)
{
  int i;
  if (n) {
    if (depth > 0) {
      for (i = 0; i < 16; i++) idx = FUN_004da480(n->child[i], pal, idx, depth - 1);
    } else {
      FUN_004d9a60(&pal[idx], n);
      n->val = (unsigned char)idx;
      idx++;
    }
  }
  return idx;
}
