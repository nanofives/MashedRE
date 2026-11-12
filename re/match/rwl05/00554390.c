// TOOLCHAIN rw9044
/* MATCH-RT2D-RESID run 40 (2026-11-12): 100.0% under rw9044, BYTE-IDENTICAL (1110 B), relocheck OK (39 checked).
   One change from the 92.5% form: the outline loop indexes the glyph array (`g[i].code`, `g[i].path`) with the same
   counter i as the bitmap loops, instead of walking a pointer (`g->code`, `g++`) with a second counter. c2 then
   strength-reduces g[i] to a pointer that it creates in the loop preheader, after the guard (`mov ebp,eax` at
   0x005545be follows the `jle` at 0x005545b8), so ebp is free until then and holds the constant 0 for
   `cmp [ebx],ebp` (0x00554595), the guard `cmp ecx,ebp` (0x005545b2) and the counter store (0x005545b4). The hoisted
   `i = 0` and the counter j of runs 38 and 39 are gone: each loop is a plain `for (i = 0; i < font->count; i++)`.
   Kept from run 39: `char name[36]`. With name[32] the same source is 93.8% (frame 0x2c, every [esp+N] above the
   name shifted). [UNCERTAIN: what the original declares in the 4 bytes at [esp+0x3c]; no instruction of
   0x00554390 references them. Evidence missing: the original source.] The comments below are the history. */
/* MATCH-RT2D-RESID run 39 (2026-11-12): 92.5% under rw9044 (regs 92.7%, ops 98.4%, 439/439 ins), was 90.2%.
   Two changes: (1) the outline loop counter is its own variable j = i (spilled at [esp+0x10] as in the original,
   0x005545b4), the first bitmap loop keeps i, so i is in ebp for the bitmap loop and the bitmap glyph pointer shares
   [esp+0x10] (0x005546bf) with oldTxd at [esp+0x18] (0x0055469e); (2) name is 36 bytes, which gives the original's
   0x30 frame with name at [esp+0x1c] (0x00554745) and an unused dword at [esp+0x3c]. The read is still 0x20 bytes.
   Residual: c2 does not make 0 a register before the isOutline test. The original's ebp = 0 (0x0055458b) is used by
   `cmp [ebx],ebp`, both loop guards and the j store, then is the bitmap i. Probes show c2 9044 makes such a
   zero register once four constant-0 uses share the region (one extra store of 0 in either branch is enough,
   though it lands in edi, not ebp). The source has three. Open: which source form gives the fourth use with
   no code of its own. Not reached by: dead initialisers of oldTxd, g, font, version, path, a spare int (63
   variants), explicit `count > 0` guards (60), i = 0 at 7 points, two reaching defs of i, i = 0 as the first
   statement of both arms, name sizes 33-40, match_permute.py from 89.5% and 92.5% (1,059 + 1,353 compiles). */
/* MATCH-RT2D-RESID (2026-11-11): 90.2% under rw9044 (regs 91.1%, ops 98.2%, 440/439 ins), was 86.1%. Changes, each
   read off the original listing: (1) the read after FUN_00554200 goes into the same temporary t1 as the other outline
   reads (the original's `lea eax,[esp+0x48]` at 0x00554625 is [esp+0x44] plus the pushed arg of the 0x00554200 call);
   (2) both branches end in one `return font` (the original duplicates `mov eax,ebx; pop..; ret` into the outline
   success, the outline fail at 0x0055467e and the bitmap end), with each fail block placed after its success code
   (`if (0) { fail: .. }`); (3) the glyph list pointer is fetched in its own statement before memset, so the count is
   loaded after the call (0x00554575); (4) i = 0 before the isOutline test, shared by the outline loop and the first
   bitmap loop, which gives the 0x30 frame; (5) t1 and n block-scoped so both take the stream parameter home [esp+0x44].
   Residual: the original holds 0 in ebp from 0x0055458b (`xor ebp,ebp`) and uses it for the isOutline test
   (0x00554595 `cmp [ebx],ebp`), both count > 0 tests and the outline i = 0 store (0x005545b4), with i in ebp for the first bitmap
   loop and in [esp+0x10] for the outline loop. Ours keeps i in [esp+0x10] in both loops and uses immediate zeros.
   Not reached by: i hoisted at 4 other points x outline/bitmap counter forms (51 variants), a separate outline
   counter j (= 0 or = i), `register`, an init in the declaration, both branches as __inline/__forceinline helpers
   (12 variants, best 87.8%), flag sets (/Os /O1 /Ox /G6 /Oy- /Op /Ob2), match_permute.py (1,345 compiles, walk 200). */
/* MATCH-RW-COMPILER-9044 (2026-11-11): 86.1% under rw9044 (62.6% under the DDK). rw9044 gives one epilogue per return and the 0x30 frame; the outline-loop reads use two block-scoped temporaries (t1 at [esp+0x44], t2 at [esp+0x48], the original's param homes). Residual: the original zeroes ebp before the isOutline test and keeps i at [esp+0x10] in both loops, plus a separate outline fail tail. */
// FLAGS: /O2 /Ob1 /G5
/* MATCH-RT2D-FONT 2026-11-11 0x00554390: 62.6% (regs 62.6%, ops 73.6%), 353/439 ins (was 33.5% raw g2s). Rewritten from the disassembly: Font (0x144 B) and Glyph (0x20 B) structs, memset intrinsics, outline branch (isOutline != 0, falls through first in the original) and bitmap branch with TXD save/restore. Residual (b): the original pushes ebx/ebp/esi/edi at entry and gives every early `return 0` its own epilogue that returns the zero already in eax; DDK 9176 here shrink-wraps ebp/esi/edi past the first return and cross-jumps the later returns into one block with xor eax,eax. Tried: read helper typed as returning a pointer, 7 flag sets (/G5 is right). Needs Opus-high shaping of the return structure. */
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
typedef struct Glyph {
  int code;          /* +0x00 */
  int *tex;          /* +0x04 */
  float uv[4];       /* +0x08 */
  void *path;        /* +0x18 */
  char texIndex;     /* +0x1c */
} Glyph;
typedef struct Font {
  int isOutline;     /* +0x000 */
  float f4, f8, fC, f10;
  int *tex[4];       /* +0x014 */
  char name[0x100];  /* +0x024 */
  int f124;          /* +0x124 */
  int nMap;          /* +0x128 */
  short *map;        /* +0x12c */
  int count;         /* +0x130 */
  void *glyphs;      /* +0x134 */
  void *cb;          /* +0x138 */
  int f13c;          /* +0x13c */
  int f140;          /* +0x140 */
} Font;
typedef void *(*MallocFn)(unsigned int size, unsigned int hint);
void *memset(void *, int, unsigned int);
#pragma intrinsic(memset)
MGLOBAL(int, DAT_007d3ff8, 0x007d3ff8);
MGLOBAL(void *, DAT_00912a20, 0x00912a20);
void FUN_00554940(void);
void FUN_005547f0(void);
void FUN_004c5c80(void *txd);
void *FUN_004c5ca0(void);
int *FUN_004c5cb0(char *name, char *mask);
unsigned int FUN_004cbd30(void *stream, void *buf, unsigned int len);
void *FUN_004cc5e0(void *stream, int type, unsigned int *size, unsigned int *version);
void *FUN_004cc790(void *stream, void *buf, unsigned int len);
void *FUN_00554200(void *stream);
Font *FUN_005551d0(void);
int FUN_00555830(Font *font);
void *FUN_005c4c60(void *list, int n, unsigned int hint);
Glyph *FUN_005c4d30(void *list);
// MATCH: 0x00554390 FUN_00554390
Font *FUN_00554390(void *stream)
{
  Font *font;
  unsigned int version;
  int i;
  Glyph *g;

  font = FUN_005551d0();
  if (font == 0) {
    return 0;
  }
  if (!FUN_004cc790(stream, &version, 4)) {
    return 0;
  }
  if ((int)(version >> 24) < 1) {
    return 0;
  }
  if (!FUN_004cc790(stream, &font->isOutline, 4)) {
    return 0;
  }
  if (!FUN_004cc790(stream, &font->f4, 4)) {
    return 0;
  }
  if (!FUN_004cc790(stream, &font->f8, 4)) {
    return 0;
  }
  if (!FUN_004cc790(stream, &font->fC, 4)) {
    return 0;
  }
  if (!FUN_004cc790(stream, &font->f10, 4)) {
    return 0;
  }
  if (!FUN_004cc790(stream, &font->f124, 4)) {
    return 0;
  }
  if (!FUN_004cc790(stream, &font->count, 4)) {
    return 0;
  }
  if (!FUN_004cc790(stream, &font->nMap, 4)) {
    return 0;
  }
  if (font->nMap > 0) {
    font->map = (short *)(*(MallocFn *)(DAT_007d3ff8 + 0x108))(font->nMap * 2, 0x30190);
    memset(font->map, 0, font->nMap * 2);
    if (font->map == 0) {
      FUN_00555830(font);
      return 0;
    }
    if (!FUN_004cc790(stream, font->map, font->nMap * 2)) {
      return 0;
    }
  }
  if (!FUN_004cc790(stream, font->name, 0x100)) {
    return 0;
  }
  FUN_005c4c60(font->glyphs, font->count, 0x30190);
  g = FUN_005c4d30(font->glyphs);
  memset(g, 0, font->count * sizeof(Glyph));
  if (font->isOutline) {
    int t1;
    g = FUN_005c4d30(font->glyphs);
    for (i = 0; i < font->count; i++) {
      void *path = 0;
      if (!FUN_004cc790(stream, &t1, 4)) {
        goto fail;
      }
      if (!FUN_004cc790(stream, &g[i].code, 4)) {
        goto fail;
      }
      if (!FUN_004cc790(stream, &t1, 4)) {
        goto fail;
      }
      if (t1 == 5) {
        if (!FUN_004cc5e0(stream, 0x1a1, 0, 0)) {
          goto fail;
        }
        path = FUN_00554200(stream);
        if (!FUN_004cc790(stream, &t1, 4)) {
          goto fail;
        }
      }
      if (!FUN_004cc790(stream, &t1, 4)) {
        goto fail;
      }
      g[i].path = path;
    }
    font->cb = (void *)FUN_005547f0;
    if (0) {
fail:
      FUN_00555830(font);
      font = 0;
    }
  } else {
    int n;
    void *oldTxd;
    char name[36];
    oldTxd = FUN_004c5ca0();
    FUN_004c5c80(DAT_00912a20);
    g = FUN_005c4d30(font->glyphs);
    for (i = 0; i < font->count; i++) {
      if (!FUN_004cc790(stream, g[i].uv, 0x10) ||
          !FUN_004cc790(stream, &g[i].code, 4) ||
          FUN_004cbd30(stream, &g[i].texIndex, 1) != 1) {
        goto fail2;
      }
      g[i].path = 0;
    }
    if (!FUN_004cc790(stream, &n, 4)) {
      goto fail2;
    }
    for (i = 0; i < n; i++) {
      if (FUN_004cbd30(stream, name, 0x20) != 0x20) {
        goto fail2;
      }
      font->tex[i] = FUN_004c5cb0(name, 0);
      if (font->tex[i] == 0) {
        goto fail2;
      }
    }
    for (i = 0; i < font->count; i++) {
      g[i].tex = font->tex[g[i].texIndex];
      if (*g[i].tex == 0) {
        goto fail2;
      }
    }
    font->f13c = 0;
    font->f140 = 0;
    font->cb = (void *)FUN_00554940;
    if (0) {
fail2:
      FUN_00555830(font);
      font = 0;
    }
    FUN_004c5c80(oldTxd);
  }
  return font;
}
