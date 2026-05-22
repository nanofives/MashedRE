// Mashed RE — Save/CareerEvents.cpp
// Career-event helpers. C1->C3 session save-sdone-a-s3.
#include "../Core/HookSystem.h"

#include <cstdint>

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

// DAT_00898ab0 — event priority word; 0x0042a920: MOV dword ptr [0x00898ab0], 0x40
static std::uint32_t* const kFrontendEventPriority =
    reinterpret_cast<std::uint32_t*>(0x00898ab0u);

// DAT_00899140 — event ID slot; 0x0042a92a: MOV [0x00899140], param_1
static std::uint32_t* const kFrontendEventId =
    reinterpret_cast<std::uint32_t*>(0x00899140u);

// 0x40 — event priority constant; cited at 0x0042a920
static constexpr std::uint32_t kTrophyEventPriority = 0x40u;

extern "C" __declspec(dllexport) void __cdecl PostTrophyEvent(std::uint32_t event_id) {
    *kFrontendEventPriority = kTrophyEventPriority;   // 0x0042a920
    *kFrontendEventId       = event_id;               // 0x0042a92a
}

RH_ScopedInstall(PostTrophyEvent, 0x0042a920);
