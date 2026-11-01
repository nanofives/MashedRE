// MenuSlotInit — 0x004224d0
//
// One-file class per gta-reversed convention. Reimplementation of FUN_004224d0.
//
// Original (0x004224d0..0x0042256d, 158 bytes). `void __cdecl FUN_004224d0(int slot,
// uint32_t* src4, uint32_t p3)`. Initialises one 0xf40-byte slot of the array rooted at
// DAT_006403e8 (stride 0xf40): zero-fills the slot via FUN_004b6520 (ZeroFillWrapper, C3)
// then writes ~15 fields at fixed offsets within the slot:
//   +0xf00..+0xf0c = src4[0..3]           (the 4 dwords behind the pointer arg)
//   +0xf1c,+0xf24  = 0x3f000000 (0.5f)
//   +0xf28         = DAT_005f6154          (a global scalar, bare-absolute load)
//   +0xf2c         = slot                  (the index itself)
//   +0xf10         = 0xbfe66666            +0xf14 = 0x3f800000 (1.0f)
//   +0xf18         = 0xbf59999a            +0xf20,+0xf34 = 0
//   +0xf38         = p3
//
// Pure integer/dword-copy transform (no x87 arithmetic; the float constants are copied as
// raw dwords). The naked body is a verbatim transcription of the original instruction
// stream, so the written slot is bit-identical by construction. Two non-verbatim but
// functionally identical substitutions: the CALL to FUN_004b6520 (0x4224ea `e8 rel32` ->
// `mov eax,0x004b6520; call eax`, ECX/EAX are dead across it) and the bare-absolute load
// at 0x422512 needs `ds:` (MSVC inline asm assembles `mov edx,[imm]` as mov-IMMEDIATE, the
// r6 0x00430670 lesson).
//
// DEV-HOOK TU only (asi_sources.rsp), NOT the greenfield exe: absolute MASHED addresses.
// Callee FUN_004b6520 ZeroFillWrapper C3. ref: re/analysis/frontend_round5_frontier.md
// (r7 addendum) ; listing 0x004224d0..0x0042256d
// ---------------------------------------------------------------------------

#include "../Core/HookSystem.h"

// MATCH-S05: the body of 0x004224d0 (MenuSlotInit) moved to Match/MatchS05_004224d0.cpp,
// which compiles the byte-matched source re/match/s05/004224d0.c.
