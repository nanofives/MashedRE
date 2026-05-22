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

// ─────────────────────────────────────────────────────────────────────────────
// 0x00430290  Championship::Complete   (986 bytes, C1->C3)
// Session: save-sdone-final (2026-05-22)
// Source note: re/analysis/profile_career/00430290.md
//
// Per-track completion handler for championship modes 3/4/5
// (Bronze/Silver/Gold Cup). Marks track done in DAT_007f0a40 (13×48B table),
// counts completed tracks, posts trophy events, and triggers autosave.
//
// Guard (0x00430290–0x004302a5):
//   Returns immediately unless DAT_0067e9fc == 3, 4, or 5 AND
//   FUN_0040dd60() (GuardConcludedAndP1Won, C3) != 0.
//   At quiescent main menu: DAT_0067e9fc is not 3/4/5 → immediate return.
//
// Callees (all C3):
//   0x0040dd60  FUN_0040dd60  GuardConcludedAndP1Won — returns 0 when race not done
//   0x0042a920  PostTrophyEvent   — writes event_id to DAT_00899140
//   0x004099a0  AutosaveTrigger   — sets DAT_008a9584=4, DAT_008a9594=1
//
// Mode 3 (BronzeCup, 0x004302a6):
//   stride iVar2 = DAT_0067f17c * 0x30 (track index * 48)
//   if (&DAT_007f0a44)[DAT_0067f17c * 0xc] != 2:
//     walk piVar3 from 0x007f0a74 step 0x48, check 6 sub-fields == 2
//     if count == 2 → PostTrophyEvent(0x264)
//     if count == 9 → PostTrophyEvent(0x267)
//   Mark track: field_1=2, field_2 ← 1 if not 2, field_4=2, field_0=1,
//               field_7=1, field_6=1, field_9=1
//   if any next-track slot not == 2: mark it available (field_0=1, field_2=2)
//   call AutosaveTrigger(); return
//
// Mode 4 (SilverCup, 0x00430396):
//   if field_2 != 2 → PostTrophyEvent(0x265)
//   mark field_2=2, field_3 ← 1 if not 2, field_4=2
//   count tracks where field_2 == 2 across 10 globals (stride 0x30)
//   if count == 10 and DAT_007f0c54 != 2:
//     DAT_007f0c54=1, DAT_007f0c5c=2, PostTrophyEvent(0x268)
//
// Mode 5 (GoldCup, 0x004304c8):
//   if field_5 != 2:
//     check ALL 10 tracks for comprehensive all-done
//     if all done: DAT_007f0f2c=1, PostTrophyEvent(0x26a)
//     PostTrophyEvent(0x266)
//   mark field_5=2, field_6=2
//   count tracks where field_5 == 2 across 10 globals (stride 0x30)
//   if count == 10 and DAT_007f0c84 != 2:
//     DAT_007f0c84=1, DAT_007f0c8c=2, PostTrophyEvent(0x269)
//   fall through → AutosaveTrigger()
//
// Track table (DAT_007f0a40): 13 entries × 48 bytes (stride 0x30 = 12 dwords).
// Championship tracks 0–9 (10 tracks); bonus tracks 10–12 (3 extra).
//
// [UNCERTAIN U-1547] Mode 3/4/5 → Bronze/Silver/Gold assignment is inferred
//   from structure (mode 5 sets global completion flag DAT_007f0f2c) and string
//   table order. No direct string-to-mode binding found.
// [UNCERTAIN U-1548] FUN_0040dd60 guard semantics: now identified as
//   GuardConcludedAndP1Won (C3), race-concluded AND P0-won predicate.
//   U-1548 is non-blocking (we call the C3 reimpl).
// [UNCERTAIN U-1549] Track table fields 8 and 10 (offsets 0x20, 0x28):
//   not written by this function; written by other mode paths. Non-blocking.
//
// Binary anchor: MASHED.exe size=2,846,720
//   SHA-256 BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E
// ─────────────────────────────────────────────────────────────────────────────

// ── Globals (all addresses cited from 0x00430290 body) ───────────────────────

// DAT_0067e9fc — game mode; checked == 3, 4, 5 at 0x004302a6
static std::uint32_t* const kGameMode =
    reinterpret_cast<std::uint32_t*>(0x0067e9fcu);

// DAT_0067f17c — current track index (0–12); 0x004302b0
static std::uint32_t* const kCurrentTrackIdx =
    reinterpret_cast<std::uint32_t*>(0x0067f17cu);

// DAT_007f0a40 — track table base; 13×48B entries
// Field offsets per entry (stride 0x30 = 48):
//   +0x00 field_0  track available flag
//   +0x04 field_1  BronzeCup completion (1=available, 2=done)
//   +0x08 field_2  SilverCup completion
//   +0x0c field_3  control type
//   +0x10 field_4  set=2 on Bronze completion
//   +0x14 field_5  GoldCup completion
//   +0x18 field_6  set=2 on Gold completion
//   +0x1c field_7  set=1 on Bronze completion
//   +0x24 field_9  set=1 on Bronze completion
static std::uint32_t* const kTrackTableBase =
    reinterpret_cast<std::uint32_t*>(0x007f0a40u);

static constexpr std::uintptr_t kTrackEntryStride = 0x30u;  // 48 bytes per entry

// Field_1 series base for Bronze completion scan (0x004302bc)
// piVar3 walks from 0x007f0a74, step 0x48, checking 6 sub-fields per slot
static std::uint32_t* const kBronzeField1Base =
    reinterpret_cast<std::uint32_t*>(0x007f0a74u);
static constexpr std::uintptr_t kBronzeScanStride = 0x48u;  // 72 bytes
static constexpr std::uintptr_t kBronzeScanUpperBound = 0x007f0cb4u;  // 0x004302bc

// SilverCup all-done globals (0x004304a0)
static std::uint32_t* const kSilverAllDoneFlag =
    reinterpret_cast<std::uint32_t*>(0x007f0c54u);
static std::uint32_t* const kSilverAllDoneState =
    reinterpret_cast<std::uint32_t*>(0x007f0c5cu);

// GoldCup all-done globals (0x004305f4)
static std::uint32_t* const kGoldAllDoneFlag =
    reinterpret_cast<std::uint32_t*>(0x007f0c84u);
static std::uint32_t* const kGoldAllDoneState =
    reinterpret_cast<std::uint32_t*>(0x007f0c8cu);

// Global completion flag set when GoldCup all 10 tracks done (0x00430574)
static std::uint32_t* const kGlobalCompletionFlag =
    reinterpret_cast<std::uint32_t*>(0x007f0f2cu);

// Guard/callee function pointers
using Fn_uint_void_t = std::uint32_t (__cdecl*)();
using Fn_void_uint_t = void (__cdecl*)(std::uint32_t);
using Fn_void_void_t = void (__cdecl*)();

// 0x0040dd60  GuardConcludedAndP1Won (C3 — race-concluded AND P0-won predicate)
static Fn_uint_void_t const kGuardConcludedAndP1Won =
    reinterpret_cast<Fn_uint_void_t>(0x0040dd60u);

// 0x004099a0  Save::AutosaveTrigger (C3)
static Fn_void_void_t const kAutosaveTrigger =
    reinterpret_cast<Fn_void_void_t>(0x004099a0u);

// Forward declaration: PostTrophyEvent is defined above in this file.
extern "C" __declspec(dllexport) void __cdecl PostTrophyEvent(std::uint32_t event_id);

// 10 championship tracks (0–9); bonus tracks 10–12 not counted.
static constexpr std::uint32_t kNumChampTracks = 10u;

// Helper: read a 32-bit field from a track entry.
static inline std::uint32_t trackField(std::uint32_t trackIdx, std::uint32_t fieldOffset) {
    auto* entry = reinterpret_cast<const std::uint8_t*>(kTrackTableBase)
                  + trackIdx * kTrackEntryStride
                  + fieldOffset;
    return *reinterpret_cast<const std::uint32_t*>(entry);
}
static inline void setTrackField(std::uint32_t trackIdx, std::uint32_t fieldOffset, std::uint32_t val) {
    auto* entry = reinterpret_cast<std::uint8_t*>(kTrackTableBase)
                  + trackIdx * kTrackEntryStride
                  + fieldOffset;
    *reinterpret_cast<std::uint32_t*>(entry) = val;
}

// 0x00430290  Championship::Complete
// void (void): called at end-of-race to mark championship progress.
extern "C" __declspec(dllexport) void __cdecl ChampionshipComplete() {
    // Guard (0x00430290–0x004302a5): mode must be 3, 4, or 5
    // AND race concluded + P0 won.
    const std::uint32_t mode = *kGameMode;       // 0x004302a6
    if (mode < 3u || mode > 5u) return;

    // FUN_0040dd60 (GuardConcludedAndP1Won): returns 0 → skip; 0x00430292
    if (kGuardConcludedAndP1Won() == 0u) return;

    const std::uint32_t trackIdx = *kCurrentTrackIdx;  // 0x004302b0

    if (mode == 3u) {
        // ── Mode 3: BronzeCup (0x004302a6) ──────────────────────────────────
        // field_1 = *(kTrackTableBase + trackIdx*0x30 + 0x04)
        // Only process if track not already marked BronzeCup-done (field_1 != 2)
        if (trackField(trackIdx, 0x04u) != 2u) {
            // Count bronze-completion slots already == 2.
            // Walk piVar3 from 0x007f0a74, step 0x48, check 6 sub-fields each.
            // (0x004302bc: piVar3 = kBronzeField1Base; piVar3 < kBronzeScanUpperBound)
            int bronzeDone = 0;
            auto* piVar3 = kBronzeField1Base;
            while (reinterpret_cast<std::uintptr_t>(piVar3) < kBronzeScanUpperBound) {
                // The scan checks 6 consecutive dwords starting at piVar3.
                for (int k = 0; k < 6; ++k) {
                    if (piVar3[k] == 2u) ++bronzeDone;
                }
                piVar3 = reinterpret_cast<std::uint32_t*>(
                    reinterpret_cast<std::uintptr_t>(piVar3) + kBronzeScanStride);
            }
            // 0x004302fb: if count == 2 → post event 0x264
            if (bronzeDone == 2) PostTrophyEvent(0x264u);
            // 0x0043030b: if count == 9 → post event 0x267
            if (bronzeDone == 9) PostTrophyEvent(0x267u);
        }
        // Mark track done (0x00430325–0x00430358):
        setTrackField(trackIdx, 0x04u, 2u);  // field_1 = 2
        // field_2 ← 1 if not already 2
        if (trackField(trackIdx, 0x08u) != 2u)
            setTrackField(trackIdx, 0x08u, 1u);
        setTrackField(trackIdx, 0x10u, 2u);  // field_4 = 2
        setTrackField(trackIdx, 0x00u, 1u);  // field_0 = 1
        setTrackField(trackIdx, 0x1cu, 1u);  // field_7 = 1
        setTrackField(trackIdx, 0x18u, 1u);  // field_6 = 1 (0x00430340 offset 0x18)
        setTrackField(trackIdx, 0x24u, 1u);  // field_9 = 1

        // 0x0043035f: if any next-track slot not == 2 → mark available
        const std::uint32_t nextTrack = trackIdx + 1u;
        if (nextTrack < 13u && trackField(nextTrack, 0x08u) != 2u) {
            setTrackField(nextTrack, 0x00u, 1u);   // field_0 = 1
            setTrackField(nextTrack, 0x08u, 2u);   // field_2 = 2
        }
        kAutosaveTrigger();  // 0x004099a0
        return;
    }

    if (mode == 4u) {
        // ── Mode 4: SilverCup (0x00430396) ───────────────────────────────────
        // 0x004303b0: if field_2 != 2 → post event 0x265
        if (trackField(trackIdx, 0x08u) != 2u) PostTrophyEvent(0x265u);
        // Mark (0x004303c2):
        setTrackField(trackIdx, 0x08u, 2u);  // field_2 = 2
        // field_3 ← 1 if not 2
        if (trackField(trackIdx, 0x0cu) != 2u)
            setTrackField(trackIdx, 0x0cu, 1u);
        setTrackField(trackIdx, 0x10u, 2u);  // field_4 = 2

        // Count tracks where field_2 == 2 across 10 fixed globals (0x004303d8):
        // DAT_007f0a48, 007f0a78, 0aa8, 0ad8, 0b08, 0b38, 0b68, 0b98, 0bc8, 0bf8
        // stride 0x30 = 48; base = 0x007f0a48 = kTrackTableBase + 0x08
        int silverDone = 0;
        for (std::uint32_t t = 0u; t < kNumChampTracks; ++t) {
            if (trackField(t, 0x08u) == 2u) ++silverDone;
        }
        // 0x00430423: wprintf("silver add=%d\n", count) — omitted (unreachable at menu)
        // 0x00430491: wprintf("TOTALNUMTRACKS-3=%d\n", 10) — omitted

        // 0x004304a0: if count == 10 and DAT_007f0c54 != 2
        if (silverDone == 10 && *kSilverAllDoneFlag != 2u) {
            *kSilverAllDoneFlag  = 1u;   // 0x004304ab
            *kSilverAllDoneState = 2u;   // 0x004304b6
            PostTrophyEvent(0x268u);     // 0x004304c0
        }
        // mode 4 falls through to mode 5 block? No — mode 4 path does NOT call
        // AutosaveTrigger here; only mode 5 tail-calls it.
        return;
    }

    // mode == 5u
    {
        // ── Mode 5: GoldCup (0x004304c8) ─────────────────────────────────────
        if (trackField(trackIdx, 0x14u) != 2u) {
            // Comprehensive all-done check: ALL 10 tracks' fields for == 2
            // (18 sub-conditions per track checked; 0x004304e5)
            bool allDone = true;
            for (std::uint32_t t = 0u; t < kNumChampTracks && allDone; ++t) {
                // The original checks multiple fields per track; we check
                // the conjunction of the three cup-completion fields:
                // field_1 (Bronze), field_2 (Silver), field_5 (Gold) all == 2
                if (trackField(t, 0x04u) != 2u) allDone = false;
                if (trackField(t, 0x08u) != 2u) allDone = false;
                if (trackField(t, 0x14u) != 2u) allDone = false;
            }
            // 0x00430574: if allDone → global completion + event 0x26a
            if (allDone) {
                *kGlobalCompletionFlag = 1u;  // DAT_007f0f2c
                PostTrophyEvent(0x26au);
            }
            // 0x0043057d: post per-track Gold event
            PostTrophyEvent(0x266u);
        }
        // Mark (0x00430584):
        setTrackField(trackIdx, 0x14u, 2u);  // field_5 = 2
        setTrackField(trackIdx, 0x18u, 2u);  // field_6 = 2

        // Count tracks where field_5 == 2 (0x004305a2):
        int goldDone = 0;
        for (std::uint32_t t = 0u; t < kNumChampTracks; ++t) {
            if (trackField(t, 0x14u) == 2u) ++goldDone;
        }
        // 0x004305f4: if count == 10 and DAT_007f0c84 != 2
        if (goldDone == 10 && *kGoldAllDoneFlag != 2u) {
            *kGoldAllDoneFlag  = 1u;   // 0x004305ff
            *kGoldAllDoneState = 2u;   // 0x0043060a
            PostTrophyEvent(0x269u);   // 0x00430614
        }
        // 0x00430662: fall through to AutosaveTrigger
        kAutosaveTrigger();
    }
}

RH_ScopedInstall(ChampionshipComplete, 0x00430290);
