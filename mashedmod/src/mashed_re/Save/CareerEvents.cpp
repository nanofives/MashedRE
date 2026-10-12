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

RH_ScopedInstall(PostTrophyEvent, 0x0042a920);  // re-enabled 2026-05-24 batch-save-b

// ─────────────────────────────────────────────────────────────────────────────
// 0x00430290  Championship::Complete   (986 bytes, 0x00430290..0x0043066a)
// Re-ported from the bytes 2026-10-12 (reearn/fstate). void(void), cdecl.
// Source note: re/analysis/profile_career/00430290.md
//
// Table: 0x007f0a40, row stride 0x30. A cell is 0x007f0a40 + row*0x30 + col;
// below, columns are named by their row-0 absolute address.
// R = DAT_0067f17c * 0x30 (re-read at 0x004302cb / 0x004303ec / 0x004304df).
//
// Gate 0x00430290..0x004302af: DAT_0067e9fc in {3,4,5} (three cmp/je) AND
//   FUN_0040dd60() != 0, else plain ret (0x0043066a). The mode is re-read at
//   0x004302b5 (== 3 ?) and at 0x004304d2 (== 5 ?).
//
// Mode 3 (0x004302cb..0x004303d9):
//   if [R+0x7f0a44] != 2:                                        0x004302d6
//     n = #(col 0x7f0a44 rows 0..11 == 2)  (eax 0x7f0a74 step 0x120 < 0x7f0cb4,
//         six rows per pass at eax-0x30..eax+0xc0)               0x004302e2..0x0043031a
//     id = n==2 ? 0x264 : n==9 ? 0x267 : 0                       0x0043031c..0x0043032c
//     FUN_0042a920(id), posted UNCONDITIONALLY (id may be 0)     0x00430332
//   [R+0x7f0a44]=2; if [R+0x7f0a48]!=2: [R+0x7f0a48]=1           0x0043033a..0x0043034d
//   [R+0x7f0a50]=2; [R+0x7f0a40]=1; [R+0x7f0a70]=1               0x00430353..0x0043035f
//   [R+0x7f0a6c]=1; [R+0x7f0a9c]=1                               0x00430365..0x0043036b
//   c = #(col 0x7f0a44 rows 0..11 == 2)                          0x00430371..0x004303ab
//   for k in 0..c-1, p = 0x7f0a74 + k*0x30:                      0x004303b5..0x004303d4
//     if p < 0x7f0c54 (signed jge) and [p] != 2: [p]=1, [p+8]=2
//   tail-jmp FUN_004099a0                                        0x004303d9
//
// Mode 4 (0x004303ec..0x004304cf):
//   if [R+0x7f0a48] != 2: FUN_0042a920(0x265)                    0x004303f7..0x00430404
//   [R+0x7f0a48]=2; if [R+0x7f0a54]!=2: [R+0x7f0a54]=1; [R+0x7f0a58]=2   0x0043040c..0x00430420
//   n = #(col 0x7f0a48 rows 0..9 == 2)                           0x00430426..0x00430483
//   printf(0x5cd8c8 "silver add=%d\n", n)                        0x00430484..0x0043048a
//   printf(0x5cd8b0 "TOTALNUMTRACKS-3=%d\n", 10)                 0x0043048f..0x00430496
//   if n == 10: printf(0x5cd8a8 "HERE!!\n")                      0x004304a3..0x004304a8
//     if [0x7f0c54] != 2: [0x7f0c54]=1; [0x7f0c5c]=2; FUN_0042a920(0x268)   0x004304ad..0x004304ca
//   then falls into the mode-5 test at 0x004304d2.
//
// Mode 5 (0x004304d2..0x0043065a), taken when the re-read mode == 5:
//   if [R+0x7f0a54] != 2:                                        0x004304ea
//     id = 0x266; [R+0x7f0a54] = 2 (BEFORE the scan)             0x004304f6..0x004304fb
//     all = 1; any cell of cols 0x7f0a44/0x7f0a48/0x7f0a54 rows 0..11 != 2 -> all = 0
//         (eax 0x7f0a48 step 0x120 < 0x7f0c88, 18 cells per pass)   0x00430501..0x004305aa
//     if all: id = 0x26a; [0x7f0f2c] = 1; FUN_0042a920(0x26a)    0x004305b0..0x004305c0
//     FUN_0042a920(id)                                           0x004305c8
//   [R+0x7f0a54]=2; [R+0x7f0a5c]=2                               0x004305d1..0x004305d7
//   n = #(col 0x7f0a54 rows 0..9 == 2)                           0x004305dd..0x00430639
//   if n == 10 and [0x7f0c84] != 2: [0x7f0c84]=1; [0x7f0c8c]=2; FUN_0042a920(0x269)   0x0043063c..0x0043065a
//
// Every path past the gate ends in tail-jmp FUN_004099a0 (0x004303d9, 0x00430665).
// Binary anchor: MASHED.exe size=2,846,720
//   SHA-256 BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E
// ─────────────────────────────────────────────────────────────────────────────

static volatile std::uint32_t* const kGameMode =
    reinterpret_cast<volatile std::uint32_t*>(0x0067e9fcu);   // 0x00430290, 0x004302b5, 0x004304d2
static volatile std::uint32_t* const kCurrentRow =
    reinterpret_cast<volatile std::uint32_t*>(0x0067f17cu);   // 0x004302cb, 0x004303ec, 0x004304df

using Fn_uint_void_t = std::uint32_t (__cdecl*)();
using Fn_void_void_t = void (__cdecl*)();
using Fn_printf_t    = int (__cdecl*)(const char*, ...);

// 0x0040dd60  GuardConcludedAndP1Won (called at 0x004302a8)
static Fn_uint_void_t const kGuardConcludedAndP1Won = reinterpret_cast<Fn_uint_void_t>(0x0040dd60u);
// 0x004099a0  Save::AutosaveTrigger (tail-jmp at 0x004303d9 / 0x00430665)
static Fn_void_void_t const kAutosaveTrigger = reinterpret_cast<Fn_void_void_t>(0x004099a0u);
// 0x004a2cbd  CRT narrow printf (0x0043048a); same verbatim pattern as Frontend/MenuMixed.cpp
static Fn_printf_t const kPrintf = reinterpret_cast<Fn_printf_t>(0x004a2cbdu);
static constexpr std::uintptr_t kFmtSilverAdd   = 0x005cd8c8u;  // 0x00430485 push
static constexpr std::uintptr_t kFmtTotalTracks = 0x005cd8b0u;  // 0x00430491 push
static constexpr std::uintptr_t kFmtHere        = 0x005cd8a8u;  // 0x004304a3 push

static inline volatile std::uint32_t& Cell(std::uintptr_t addr) {
    return *reinterpret_cast<volatile std::uint32_t*>(addr);
}
// Count cells == 2 in the column whose row-0 address is col0, rows 0..nrows-1, stride 0x30.
static inline std::uint32_t CountTwos(std::uintptr_t col0, std::uint32_t nrows) {
    std::uint32_t n = 0;
    for (std::uint32_t r = 0; r < nrows; ++r)
        if (Cell(col0 + r * 0x30u) == 2u) ++n;
    return n;
}

// 0x00430290
extern "C" __declspec(dllexport) void __cdecl ChampionshipComplete() {
    {
        const std::uint32_t m = *kGameMode;                                   // 0x00430290
        if (m != 3u && m != 4u && m != 5u) return;                            // 0x00430295..0x004302a2
    }
    if (kGuardConcludedAndP1Won() == 0u) return;                              // 0x004302a8..0x004302af

    const std::uint32_t mode = *kGameMode;                                    // 0x004302b5
    if (mode == 3u) {                                                         // 0x004302ba..0x004302c5
        const std::uintptr_t r = static_cast<std::uint32_t>(*kCurrentRow * 0x30u);   // 0x004302cb..0x004302d3
        if (Cell(r + 0x007f0a44u) != 2u) {                                    // 0x004302d6
            const std::uint32_t n = CountTwos(0x007f0a44u, 12u);              // 0x004302e2..0x0043031a
            std::uint32_t id = 0;                                             // 0x004302de
            if (n == 2u) id = 0x264u;                                         // 0x00430320
            else if (n == 9u) id = 0x267u;                                    // 0x0043032c
            PostTrophyEvent(id);                                              // 0x00430332
        }
        Cell(r + 0x007f0a44u) = 2u;                                           // 0x0043033a
        if (Cell(r + 0x007f0a48u) != 2u) Cell(r + 0x007f0a48u) = 1u;          // 0x00430340..0x0043034d
        Cell(r + 0x007f0a50u) = 2u;                                           // 0x00430353
        Cell(r + 0x007f0a40u) = 1u;                                           // 0x00430359
        Cell(r + 0x007f0a70u) = 1u;                                           // 0x0043035f
        Cell(r + 0x007f0a6cu) = 1u;                                           // 0x00430365
        Cell(r + 0x007f0a9cu) = 1u;                                           // 0x0043036b
        std::uint32_t c = CountTwos(0x007f0a44u, 12u);                        // 0x00430371..0x004303ab
        if (c != 0u) {                                                        // 0x004303ad
            std::uintptr_t p = 0x007f0a74u;                                   // 0x004303b5
            do {
                if (static_cast<std::int32_t>(p) < 0x007f0c54 && Cell(p) != 2u) {   // 0x004303c0..0x004303c9
                    Cell(p) = 1u;                                             // 0x004303cb
                    Cell(p + 8u) = 2u;                                        // 0x004303cd
                }
                p += 0x30u;                                                   // 0x004303d0
            } while (--c != 0u);                                              // 0x004303d3
        }
        kAutosaveTrigger();                                                   // 0x004303d9
        return;
    }

    if (mode == 4u) {                                                         // 0x004303de
        const std::uintptr_t r = static_cast<std::uint32_t>(*kCurrentRow * 0x30u);   // 0x004303ec..0x004303f4
        if (Cell(r + 0x007f0a48u) != 2u) PostTrophyEvent(0x265u);             // 0x004303f7..0x00430404
        Cell(r + 0x007f0a48u) = 2u;                                           // 0x0043040c
        if (Cell(r + 0x007f0a54u) != 2u) Cell(r + 0x007f0a54u) = 1u;          // 0x00430412..0x0043041a
        Cell(r + 0x007f0a58u) = 2u;                                           // 0x00430420
        const std::uint32_t n = CountTwos(0x007f0a48u, 10u);                  // 0x00430426..0x00430483
        kPrintf(reinterpret_cast<const char*>(kFmtSilverAdd), n);            // 0x0043048a
        kPrintf(reinterpret_cast<const char*>(kFmtTotalTracks), 10);         // 0x00430496
        if (n == 10u) {                                                       // 0x0043049e
            kPrintf(reinterpret_cast<const char*>(kFmtHere));                // 0x004304a8
            if (Cell(0x007f0c54u) != 2u) {                                    // 0x004304ad..0x004304b7
                Cell(0x007f0c54u) = 1u;                                       // 0x004304be
                Cell(0x007f0c5cu) = 2u;                                       // 0x004304c4
                PostTrophyEvent(0x268u);                                      // 0x004304ca
            }
        }
    }

    if (*kGameMode == 5u) {                                                   // 0x004304d2
        const std::uintptr_t r = static_cast<std::uint32_t>(*kCurrentRow * 0x30u);   // 0x004304df..0x004304e7
        if (Cell(r + 0x007f0a54u) != 2u) {                                    // 0x004304ea
            std::uint32_t id = 0x266u;                                        // 0x004304f6
            Cell(r + 0x007f0a54u) = 2u;                                       // 0x004304fb
            std::uint32_t all = 1u;                                           // 0x00430501
            for (std::uint32_t row = 0; row < 12u; ++row) {                   // 0x00430503..0x004305aa
                const std::uintptr_t b = 0x007f0a40u + row * 0x30u;
                if (Cell(b + 0x04u) != 2u) all = 0u;
                if (Cell(b + 0x08u) != 2u) all = 0u;
                if (Cell(b + 0x14u) != 2u) all = 0u;
            }
            if (all != 0u) {                                                  // 0x004305b0
                id = 0x26au;                                                  // 0x004305b4
                Cell(0x007f0f2cu) = 1u;                                       // 0x004305ba
                PostTrophyEvent(0x26au);                                      // 0x004305c0
            }
            PostTrophyEvent(id);                                              // 0x004305c9
        }
        Cell(r + 0x007f0a54u) = 2u;                                           // 0x004305d1
        Cell(r + 0x007f0a5cu) = 2u;                                           // 0x004305d7
        const std::uint32_t n = CountTwos(0x007f0a54u, 10u);                  // 0x004305dd..0x00430639
        if (n == 10u && Cell(0x007f0c84u) != 2u) {                            // 0x0043063c..0x00430647
            Cell(0x007f0c84u) = 1u;                                           // 0x0043064e
            Cell(0x007f0c8cu) = 2u;                                           // 0x00430654
            PostTrophyEvent(0x269u);                                          // 0x0043065a
        }
    }
    kAutosaveTrigger();                                                       // 0x00430665
}

RH_ScopedInstall(ChampionshipComplete, 0x00430290);  // re-ported 2026-10-12 reearn/fstate
