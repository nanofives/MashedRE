// Mashed RE — Save/CareerEvents.cpp
// Career-event helpers. Sessions: save-sdone-a-s3, save-sdone-final.
#include "../Core/HookSystem.h"

#include <cstdint>
#include <cstddef>

// ─────────────────────────────────────────────────────────────────────────────
// 0x0042a920  Frontend::PostTrophyEvent
//
// Writes a trophy/cup-award event into the frontend event queue.
// Pure leaf — no callees.
//
// Disasm (0x0042a920..0x0042a930, 20 bytes):
//   0x0042a920  MOV dword ptr [0x00898ab0], 0x40   // event priority = 64
//   0x0042a92a  MOV [0x00899140], param_1          // event ID into queue slot
//   0x0042a930  RET
//
// Caller: 0x00430290 Championship::Complete (C1) — posts IDs 0x264..0x26a
//   (Bronze/Silver/Gold cup milestones; 2 tracks, 9 tracks, 10 tracks, etc.)
//
// Globals written:
//   DAT_00898ab0 (0x0042a920) — event priority/type word; always 0x40 (64)
//   DAT_00899140 (0x0042a92a) — event ID; consumed by frontend state machine
//
// [UNCERTAIN U-1552] DAT_00898ab0 = 0x40: meaning of value 64 (priority or enum?)
//   is unknown; resolution: decomp FUN_00448220 (0x00434360, 17KB frontend fn).
// ─────────────────────────────────────────────────────────────────────────────

// FUN_0042a920: defined in Match/MatchS06_0042a920.cpp (C linkage); called below by Championship::Complete.
extern "C" void __cdecl PostTrophyEvent(std::uint32_t event_id);

// MATCH-S06: the body of 0x0042a920 (PostTrophyEvent) moved to Match/MatchS06_0042a920.cpp,
// which compiles the byte-matched source re/match/s06/0042a920.c.

// MATCH-S07: the body of 0x00430290 (ChampionshipComplete) moved to Match/MatchS07_00430290.cpp,
// which compiles the byte-matched source re/match/s07/00430290.c.
// Championship::Complete (986 bytes, 0x00430290..0x0043066a; note re/analysis/profile_career/00430290.md). The matched
// source holds a HELD copy of 0x0042a920 (for the DDK only); the wrapper binds FUN_0042a920 to PostTrophyEvent above.
