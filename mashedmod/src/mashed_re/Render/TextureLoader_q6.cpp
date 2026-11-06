// Mashed RE — Render/TextureLoader_q6.cpp
// RenderWare stream I/O + texture-dictionary creation helpers (c3-batch-q-s6).
//
// Covers:
//   (RwStreamRead / RwStreamSkip moved to Render/RwStreamCore.cpp, one body shared with the exe, lane dcstream)
//   0x004c5890  RwTexDictionaryCreate  — allocate+init RwTexDictionary; link into global TXD list
//
// Deferred (>200B and complex body, or harness-limited):
//   0x005abfa0  FUN_005abfa0  — per-wave data loader: reads 0x803+0x804 RWS chunks; complex multi-phase
//   0x005ac210  FUN_005ac210  — wave_node factory: 16 callees, flag-dispatch bits 0-5; complex multi-phase
//
// Binary anchor: MASHED.exe SHA-256 (unpatched):
//   BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E
//   (preserved in original\MASHED.exe.unpatched; patched copy SHA differs — expected)
//
// Analysis refs:
//   re/analysis/texture_loader_d3/0x004c5890.md
//
// Verification strategy:
//   All three functions access live RwStream / RwFreeList objects at runtime.
//   At quiescent main-menu (null/zero context) each crashes deterministically
//   the same way as the original — crash_equal_ok=True GREEN per established
//   pattern (see audio_rws_chunk_header_read, vfs_stream_get_type precedents).

#include "../Core/HookSystem.h"
#include <cstdint>

// ---------------------------------------------------------------------------
// 0x004c5890  FUN_004c5890  RwTexDictionaryCreate   (small body, ~60 bytes est.)
//
// RwTexDictionary *RwTexDictionaryCreate(void)
//   No parameters.
//   Returns newly allocated and linked RwTexDictionary on success, NULL on failure.
//
// Body:
//   1. Allocate object via vtable+0x118 call with type-hint 0x30016.
//      vtable base at DAT_007d3ff8; size/slot from *(DAT_007d4054 + 0x0c + DAT_007d3ff8).
//   2. On success: set object type = 6 (rwTEXDICTIONARY) at *result.
//      Zero out next 4 fields (result[1..4]).
//   3. Link into global TXD doubly-linked list (head at DAT_007d4054 + DAT_007d3ff8):
//      result[4] = fwd link (result+0x10); result[5] = bwd link (result+0x14).
//   4. Init circular sentinel for internal texture list at result+0x08/+0x0c.
//   5. Notify event DAT_00618150 via FUN_004d8000(&DAT_00618150, result).
//   6. Return result.
//
// Facts cited from re/analysis/texture_loader_d3/0x004c5890.md:
//   Alloc type-hint 0x30016, via vtable+0x118:  0x004c5897
//   *result = 6 (rwTEXDICTIONARY):              0x004c58a5
//   result+0x10/+0x14 = RwLLLink in global TXD: 0x004c58b4..0x004c58c9
//   result+0x08/+0x0c = RwLinkList sentinel:     0x004c58cc..0x004c58d3
//   Event DAT_00618150 via FUN_004d8000:         0x004c58d8
//
// External callees (called via original addresses — not yet C3):
//   *(DAT_007d3ff8 + 0x118) — RW engine allocator (vtable slot)
//   FUN_004d8000            — RW event notify
// ---------------------------------------------------------------------------

// External callee: FUN_004d8000 — RW event notification.
// void __cdecl FUN_004d8000(void *event_obj, void *result)
using EventNotifyFn_t = void (__cdecl*)(void*, void*);
static EventNotifyFn_t const s_FUN_004d8000 = reinterpret_cast<EventNotifyFn_t>(0x004d8000u);

// MATCH-S19: the body of 0x004c5890 (std::uint32_t* __cdecl) moved to Match/MatchS19_004c5890.cpp,
// which compiles the byte-matched source re/match/s19/004c5890.c.
