// Mashed RE — Sprite table lookup forwarders and HUD slot-type mappers.
// Analysis notes: re/analysis/sprite_gate_c3/
//
// Covers:
//   0x0040bb70  SpriteLookupTableA  — moved to Match/MatchS02_0040bb70.cpp (MATCH-S02)
//   0x0040bb90  SpriteLookupTableB  — moved to Match/MatchS02_0040bb90.cpp (MATCH-S02)
//   0x0042ee00  SpriteSlotGate      — calls SpriteLookupTableA_orig for slot 0/1/2, else 0
//   0x00430a10  HudSlotTypePlayer0  — game-mode → slot-type-0 (leaf, reads DAT_0067e9fc)
//   0x00430a60  HudSlotTypePlayer1  — game-mode → slot-type-1 (leaf)
//   0x00430ab0  HudSlotTypePlayer2  — game-mode → slot-type-2 (leaf)
//
// Binary anchor: MASHED.exe SHA-256 (unpatched):
//   BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E
//   (preserved in original\MASHED.exe.unpatched)

#include "../Core/HookSystem.h"
#include <cstdint>

// MATCH-S02: the body of 0x0040bb70 (SpriteLookupTableA) moved to Match/MatchS02_0040bb70.cpp,
// which compiles the byte-matched source re/match/s02/0040bb70.c.

// MATCH-S02: the body of 0x0040bb90 (SpriteLookupTableB) moved to Match/MatchS02_0040bb90.cpp,
// which compiles the byte-matched source re/match/s02/0040bb90.c.

// ---------------------------------------------------------------------------
// SpriteSlotGate  --  0x0042ee00
//
// Original: FUN_0042ee00 (59 bytes, 0x0042ee00..0x0042ee3b)
// Signature: void* (int slot)
//   For slot 0, 1, or 2: tail-calls FUN_0040bb50() (the DAT_0063b8fc variant).
//   For any other slot: returns 0 (null pointer).
//
// Note: FUN_0040bb50 receives no explicit argument visible in Ghidra decomp
// for all three branches — the string key arrives via a non-standard mechanism
// (likely a register or caller-set global not captured by the decompiler).
// We call the original FUN_0040bb50 directly to preserve exact ABI semantics.
//
// Ghidra decomp (re/analysis/promote_c2_frontend_menus/0x0042ee00.md):
//   if (slot == 0) return FUN_0040bb50();
//   if (slot == 1) return FUN_0040bb50();
//   if (slot == 2) return FUN_0040bb50();
//   return 0;
// ref: re/analysis/promote_c2_frontend_menus/0x0042ee00.md
// ref: re/analysis/hud_frontend/0x0040bb50.md
// ---------------------------------------------------------------------------

// BUGFIX 2026-07-27 — the previous port called FUN_0040bb50 with NO argument, on the
// speculation (written into the comment it replaced) that "the string key arrives via a
// non-standard mechanism — likely a register or caller-set global not captured by the
// decompiler". The anchored bytes disprove that: 0x0042ee00 is an ARGUMENT-REWRITING
// TAIL-JMP thunk. It overwrites its own incoming stack slot with a sprite-NAME string
// and jumps; it never returns to itself. Verbatim bytes (0x0042ee00..0x0042ee3b):
//   0x0042ee00 8b442404      MOV EAX,[ESP+4]
//   0x0042ee04 85c0          TEST EAX,EAX
//   0x0042ee06 750d          JNE 0x0042ee15
//   0x0042ee08 c7442404 b8d75c00  MOV dword [ESP+4],0x005cd7b8   ; "lock"
//   0x0042ee10 e93bcdfdff    JMP 0x0040bb50
//   0x0042ee15 83f801        CMP EAX,1
//   0x0042ee18 750d          JNE 0x0042ee27
//   0x0042ee1a c7442404 b4d75c00  MOV dword [ESP+4],0x005cd7b4   ; "dot"
//   0x0042ee22 e929cdfdff    JMP 0x0040bb50
//   0x0042ee27 83f802        CMP EAX,2
//   0x0042ee2a 750d          JNE 0x0042ee39
//   0x0042ee2c c7442404 acd75c00  MOV dword [ESP+4],0x005cd7ac   ; "check"
//   0x0042ee34 e917cdfdff    JMP 0x0040bb50
//   0x0042ee39 33c0          XOR EAX,EAX
//   0x0042ee3b c3            RET
// FUN_0040bb50 (0x0040bb50..0x0040bb64) is `void* (const char* key)`:
//   MOV EAX,[ESP+4] / MOV ECX,[0x0063b8fc] / PUSH EAX / PUSH ECX / CALL 0x004c5c00.
// Passing the raw slot int through as that `key` is what fed a NULL/garbage pointer into
// FUN_004c5c00's unconditional `*query` deref. Transcribed verbatim as naked asm: the
// arg-rewrite + tail-jmp reuses the CALLER's frame, which no __cdecl C function can express.
// The JMP goes through a memory slot so no register is clobbered (the original's direct
// relative JMP clobbers nothing).
static void* const s_jmp_0040bb50 = reinterpret_cast<void*>(0x0040bb50u);

static constexpr std::uintptr_t kStr_lock  = 0x005cd7b8u;  // "lock"
static constexpr std::uintptr_t kStr_dot   = 0x005cd7b4u;  // "dot"
static constexpr std::uintptr_t kStr_check = 0x005cd7acu;  // "check"

// MATCH-S07: the body of 0x0042ee00 (SpriteSlotGate) moved to Match/MatchS07_0042ee00.cpp,
// which compiles the byte-matched source re/match/s07/0042ee00.c.

// ---------------------------------------------------------------------------
// HudSlotTypePlayer0  --  0x00430a10
//
// Original: FUN_00430a10 (40 bytes, 0x00430a10..0x00430a38)
// Signature: int ()   — no arguments; reads DAT_0067e9fc (game-mode global).
// Returns the slot-type code for player slot 0 given the current game mode:
//   Mode 2         → 0
//   Mode 3, 4, 5   → 1
//   Mode 6, 7, 8, 9→ 3
//   Mode 10        → 11 (0xb)
//   Other          → 0 (default/implicit)
//
// ref: re/analysis/hud_frontend_d4/0x00430a10.md
// ---------------------------------------------------------------------------

// MATCH-S07: the body of 0x00430a10 (HudSlotTypePlayer0) moved to Match/MatchS07_00430a10.cpp,
// which compiles the byte-matched source re/match/s07/00430a10.c.

// ---------------------------------------------------------------------------
// HudSlotTypePlayer1  --  0x00430a60
//
// Original: FUN_00430a60 (28 bytes, 0x00430a60..0x00430a7c)
// Signature: int ()   — no arguments; reads DAT_0067e9fc (game-mode global).
// Returns the slot-type code for player slot 1:
//   Mode 2, 6, 7, 8, 9, 10 → 0
//   Mode 3, 4, 5            → 2
//   Other                   → 0 (default)
//
// ref: re/analysis/hud_frontend_d4/0x00430a60.md
// ---------------------------------------------------------------------------

// MATCH-S07: the body of 0x00430a60 (HudSlotTypePlayer1) moved to Match/MatchS07_00430a60.cpp,
// which compiles the byte-matched source re/match/s07/00430a60.c.

// ---------------------------------------------------------------------------
// HudSlotTypePlayer2  --  0x00430ab0
//
// Original: FUN_00430ab0 (28 bytes, 0x00430ab0..0x00430acc)
// Signature: int ()   — no arguments; reads DAT_0067e9fc (game-mode global).
// Returns the slot-type code for player slot 2:
//   Mode 2, 6, 7, 8, 9, 10 → 0
//   Mode 3, 4, 5            → 5
//   Other                   → 0 (default)
//
// ref: re/analysis/hud_frontend_d4/0x00430ab0.md
// ---------------------------------------------------------------------------

// MATCH-S07: the body of 0x00430ab0 (HudSlotTypePlayer2) moved to Match/MatchS07_00430ab0.cpp,
// which compiles the byte-matched source re/match/s07/00430ab0.c.
