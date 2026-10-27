// Mashed RE — Save/GameSaveFormat.h  (WS-G5, 2026-06-16)
//
// Header-only, buffer-based port of the REAL gamesave.bin serialization, for the
// STANDALONE mashed_re.exe (replaces GameFlow's mashed_re_progress.bin sidecar
// with the real 0x24FA0-byte gamesave.bin format, written to a standalone-copy
// file — NEVER original/gamesave.bin).
//
// Save/GameSaveBuffer.cpp already ports Save::SerializeToBuffer (0x00404EE0) /
// DeserializeFromBuffer (0x00404E80) as dev-.asi HOOKS that operate on the live
// MASHED buffer at 0x00803358 via raw pointers. This header is the same FORMAT
// expressed over an explicit byte buffer, so it links into the standalone exe and
// is unit-testable in isolation. All offsets RVA-cited (Mashed_pool6, 2026-06-16,
// disasm of FUN_00404ee0 / FUN_00404f50; layout re/analysis/structs/gamesave_layout.md).
//
// The standalone has no live profile block (DAT_008a94a8 == 0), so the writer's
// profile copy (FUN_00404f2f) is skipped and the profile region stays zero — this
// matches the shipped blank-profile gamesave.bin exactly. The faithful pieces the
// standalone DOES carry are the magic, the 0x148-dword championship span, and the
// save-state counter.
#pragma once
#include <cstdint>
#include <cstdlib>
#include <cstring>

namespace mashed_re {
namespace Save {

// ── Real gamesave.bin envelope (file == 1:1 image of save_buf @ 0x00803358) ──
constexpr unsigned     kSaveSize   = 0x24FA0;   // FUN_00404f50: PUSH 0x24fa0 (total file size)
constexpr unsigned     kSpanOff    = 0x24A40;   // 0x00827d98 - 0x00803358 (FUN_00404f0f/f14)
constexpr unsigned     kSpanBytes  = 0x520;     // 0x148 dwords (FUN_00404f0a MOV ECX,0x148)
constexpr unsigned     kCounterOff = 0x24EFC;   // 0x00828254 (FUN_00404f23); span-relative 0x4BC
constexpr unsigned     kProfileOff = 0x4;       // save_buf+4 (FUN_00404f2f); standalone: stays zero
constexpr std::uint32_t kMagic     = 0xDEADBEEFu; // FUN_00404f37: MOV [0x803358],0xDEADBEEF

// ── Championship-span row layout (0x007f0a40 table; stride 0xC dwords / 0x30B).
//    Verified columns (used by the reader Nav_GameStateLoadSave too):
//      col 4 (+0x10) = track unlock (DAT_007f0a50); col 6 (+0x18) = car unlock.
//    The savedata / profile gates the menu grey-out reads live in the trailing
//    span (DAT_007f0f2c @ +0x4ec, DAT_007f0ad4 @ +0x094). ──
constexpr unsigned kRowStride      = 0x30;
constexpr unsigned kColTrackUnlock = 0x10;   // DAT_007f0a50  (verified)
constexpr unsigned kColCarUnlock   = 0x18;   // DAT_007f0a58  (verified)
constexpr unsigned kColTrophyPriv  = 0x14;   // standalone-private medal store (real col-5 [UNCERTAIN])
constexpr unsigned kSpanSavedataGate = 0x4ec; // DAT_007f0f2c (savedata present)
constexpr int      kSpanRows       = 13;
constexpr unsigned kSpanCounterOff = 0x4bc;  // counter within the span (= file 0x24EFC)

// ── [D4-WSF-SAVE-SPANINIT 2026-10-26] FUN_004924f0's writes that land in the span ──
// The original's span (0x007f0a40..0x007f0f5f) is .data BSS, so a first-run save serializes what
// FUN_004924f0 (called from FUN_00492370) put there. Buffer form, span-relative = va - 0x007f0a40.
// Disassembly of MASHED.exe.unpatched 0x004924f0..0x0049268d:
//   0x00492504..0x0049251c  [0x7f0f2c..0x7f0f3c] = 0        (span +0x4ec..+0x4fc)
//   0x0049252e/33/38 -> 0x00431ae0/af0/b00  [0x7f0f04]/[0x7f0f08]/[0x7f0f00] = 0x3f333333
//   0x00492542 -> 0x00431b10 (0x00431b12)    [0x7f0f10] = 2
//   0x00492550  [0x7f105c + i*0x4c] = 1 while < 0x7f12bc (i = 0..7). Outside the span, but the
//               serializer gathers their low bytes into span +0x514 + i (0x00404ef2), so they are
//               written here at the gather position.
//   rows r = 0..12 (ecx = 0x7f0cb0 + r*0x14 while < 0x7f0db4, 0x00492663):
//     0x00492575..80  5 dwords at 0x7f0cb0 + r*0x14 = 0xffffffff
//     0x00492583      [0x7f0db4 + r*4] = 9
//     0x0049258a      [0x7f0de8 + r*4] = 0x3b
//     0x00492590      [0x7f0e1c + r*4] = 0x3f7d70a4
//     0x004925a3      BYTE [0x7f0e50 + r*12 + c] = 1, c = 0..11
//     jump table 0x00492690, row base 0x7f0a40 + r*0x30:
//       col 1 / col 11 (0x004925c5 / 0x0049263d) = (ecx < 0x7f0cc4) -> row 0 only = 1
//       col 3 (0x004925e2..ec) = ((ecx >= 0x7f0d00) - 1) & 2 -> rows 0..3 = 2
//       col 4 (0x004925f8) = 2, cols 0 / 2 / 5..10 = 0
// The rest of FUN_004924f0 lands outside the span (zero-fill 0x7f0f60 x 0xdce9 dwords,
// 0x7f0fec / 0x7f0fd0 / 0x7f0ff0 / 0x803344, FUN_00431d00's 0x0067ea.. globals).
// The Lua binding Set_Track_Powerups (0x0047b280, via FUN_00430250) later overwrites the 0x7f0cb0
// rows from Powerups.piz: SetTrackPowerups / RunTrackPowerupsScript below (D4-FU-SETTRACKPU).
// Not modelled here: a live writer clears gather bytes 0..2 before the original's first-run save. verify/wsfspan_20261026/RESULT_D4-WSF-SAVE-SPANINIT.md.
inline void InitSpanDefaults(unsigned char span[kSpanBytes]) {
    auto put = [&](unsigned off, std::uint32_t v) { std::memcpy(span + off, &v, 4); };
    std::memset(span, 0, kSpanBytes);
    put(0x4c4, 0x3f333333u);   // 0x007f0f04  FUN_00431ae0
    put(0x4c8, 0x3f333333u);   // 0x007f0f08  FUN_00431af0
    put(0x4c0, 0x3f333333u);   // 0x007f0f00  FUN_00431b00
    put(0x4d0, 2u);            // 0x007f0f10  FUN_00431b10
    for (unsigned i = 0; i < 8; ++i)
        span[0x514 + i] = 1;   // 0x00492550 [0x7f105c + i*0x4c] = 1, gathered by 0x00404ef2
    for (unsigned r = 0; r < kSpanRows; ++r) {
        const unsigned p1 = 0x270 + r * 0x14;            // 0x007f0cb0 + r*0x14
        for (unsigned k = 0; k < 5; ++k) put(p1 + 4 * k, 0xffffffffu);
        put(0x374 + r * 4, 9u);                          // 0x007f0db4
        put(0x3a8 + r * 4, 0x3bu);                       // 0x007f0de8
        put(0x3dc + r * 4, 0x3f7d70a4u);                 // 0x007f0e1c
        for (unsigned c = 0; c < 12; ++c) span[0x410 + r * 12 + c] = 1;   // 0x007f0e50 bytes
        const unsigned row = r * kRowStride;
        put(row + 0x04, r == 0 ? 1u : 0u);               // col 1  (0x007f0a44)
        put(row + 0x0c, r < 4 ? 2u : 0u);                // col 3  (0x007f0a4c)
        put(row + kColTrackUnlock, 2u);                  // col 4  (0x007f0a50)
        put(row + 0x2c, r == 0 ? 1u : 0u);               // col 11 (0x007f0a6c)
    }
}

// ── [D4-FU-SETTRACKPU 2026-10-26] Set_Track_Powerups (0x0047b280), buffer form ──
// Course id of each span row, row order. FUN_00430250 walks FUN_0040d430()'s table:
// FUN_0040d430 returns [0x005f2770] = 0x005f2728, 13 pointers to course records whose first
// dword is the course id (.data, initialized, read from MASHED.exe.unpatched):
//   0x005f2570 30, 0x005f25fc 3, 0x005f2660 39, 0x005f2584 0, 0x005f264c 38, 0x005f25c0 11,
//   0x005f25ac 25, 0x005f2598 2, 0x005f2624 36, 0x005f25e8 34, 0x005f2638 37, 0x005f25d4 33,
//   0x005f2610 26.
constexpr int kSpanRowCourseId[kSpanRows] = {30, 3, 39, 0, 38, 11, 25, 2, 36, 34, 37, 33, 26};

// 0x00430250: -1 (NULL) when param_2 == 0 or no row matches (the loop runs to 0xd), else the
// FIRST row r with **table[r] == param_1 (&DAT_007f0cb0 + r*5).
inline int TrackPowerupsRow(int course, int tier) {
    if (tier == 0) return -1;
    for (int r = 0; r < kSpanRows; ++r)
        if (kSpanRowCourseId[r] == course) return r;
    return -1;
}

// 0x0047b280: n = FUN_004b6fc0(L), then FUN_004b7090(L, i) + FUN_004a2c48() on stack slots
// n-6 .. n (course, tier, five ids), FUN_00430250(course, tier), and if non-NULL five dword
// stores at the row (span +0x270 + r*0x14, = 0x007f0cb0 - 0x007f0a40).
inline void SetTrackPowerups(unsigned char span[kSpanBytes], const int a[7]) {
    const int r = TrackPowerupsRow(a[0], a[1]);
    if (r < 0) return;
    for (int k = 0; k < 5; ++k) {
        const std::uint32_t v = static_cast<std::uint32_t>(a[2 + k]);
        std::memcpy(span + 0x270 + r * 0x14 + 4 * k, &v, 4);
    }
}

// The boot script powerups_all.lua (POWERUPS_ALL.LUA in Powerups.piz) is 13 `local NAME = INT`
// constants, `--` comments and 39 `Set_Track_Powerups(...)` calls of 7 arguments each. This runs
// exactly that statement set: names resolve through the locals (an unknown name is nil, and
// lua_tonumber of nil is 0), integer literals may carry a leading '-'. A call with other than 7
// arguments, or any other statement, is skipped and counted in *skipped.
// [UNCERTAIN] general Lua 4.0 is not interpreted: a script outside this statement set would diverge.
inline int RunTrackPowerupsScript(unsigned char span[kSpanBytes], const char* text, unsigned len,
                                  int* skipped) {
    struct Local { char name[32]; int value; };
    Local locals[64];
    int nLocals = 0, calls = 0, skip = 0;
    auto isId = [](char c) { return (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') || c == '_' ||
                                    (c >= '0' && c <= '9'); };
    unsigned i = 0;
    while (i < len) {
        unsigned e = i;
        while (e < len && text[e] != '\n') ++e;
        char line[512];
        unsigned n = 0;
        for (unsigned k = i; k < e && n + 1 < sizeof line; ++k) {
            if (text[k] == '-' && k + 1 < e && text[k + 1] == '-') break;   // comment
            if (text[k] != '\r') line[n++] = text[k];
        }
        line[n] = '\0';
        i = e + 1;
        // tokenise: identifiers, signed integers, punctuation
        char tok[24][32];
        int nt = 0;
        bool bad = false;
        for (unsigned k = 0; k < n && !bad;) {
            const char c = line[k];
            if (c == ' ' || c == '\t') { ++k; continue; }
            if (nt == 24) { bad = true; break; }
            unsigned t = 0;
            if (isId(c) || (c == '-' && k + 1 < n && line[k + 1] >= '0' && line[k + 1] <= '9')) {
                tok[nt][t++] = c; ++k;
                while (k < n && isId(line[k]) && t + 1 < sizeof tok[0]) tok[nt][t++] = line[k++];
            } else {
                tok[nt][t++] = c; ++k;
            }
            tok[nt][t] = '\0';
            ++nt;
        }
        if (nt == 0) continue;
        auto isInt = [](const char* s) {
            if (*s == '-') ++s;
            if (!*s) return false;
            for (; *s; ++s) if (*s < '0' || *s > '9') return false;
            return true;
        };
        auto value = [&](const char* s) {
            if (isInt(s)) return std::atoi(s);
            for (int k = nLocals - 1; k >= 0; --k)
                if (std::strcmp(locals[k].name, s) == 0) return locals[k].value;
            return 0;
        };
        if (!bad && nt == 4 && std::strcmp(tok[0], "local") == 0 && std::strcmp(tok[2], "=") == 0 &&
            isInt(tok[3]) && nLocals < 64 && std::strlen(tok[1]) < sizeof locals[0].name) {
            std::strcpy(locals[nLocals].name, tok[1]);
            locals[nLocals].value = std::atoi(tok[3]);
            ++nLocals;
            continue;
        }
        if (!bad && nt == 17 && std::strcmp(tok[0], "Set_Track_Powerups") == 0 &&
            std::strcmp(tok[1], "(") == 0 && std::strcmp(tok[16], ")") == 0) {
            int a[7];
            bool ok = true;
            for (int k = 0; k < 7; ++k) {
                if (k < 6 && std::strcmp(tok[3 + 2 * k], ",") != 0) ok = false;
                a[k] = value(tok[2 + 2 * k]);
            }
            if (ok) { SetTrackPowerups(span, a); ++calls; continue; }
        }
        ++skip;
    }
    if (skipped) *skipped = skip;
    return calls;
}

// Build a full real-format image from a championship span + save counter.
// Faithful order of Save::SerializeToBuffer (0x00404ee0), buffer form:
//   champ span copy (0x00404f19) -> counter overwrite at span+0x4bc (0x00404f23)
//   -> [profile skipped, ptr==0] -> magic at +0 (0x00404f37). Rest zero.
inline void BuildImage(const unsigned char span[kSpanBytes],
                       std::uint32_t counter, unsigned char out[kSaveSize]) {
    std::memset(out, 0, kSaveSize);
    std::memcpy(out + kSpanOff, span, kSpanBytes);   // 0x00404f19 championship snapshot
    std::memcpy(out + kCounterOff, &counter, 4);     // 0x00404f23 counter overwrite
    std::memcpy(out, &kMagic, 4);                    // 0x00404f37 magic
}

// Extract the championship span + counter from an image. Returns false if the
// size is wrong or the magic gate fails (blank/not-a-written-save). The blob is
// always exactly kSaveSize (FUN_00404f50); the DEADBEEF magic gate is caller-side
// (the shipped blank save has 0 at +0, first written by 0x00404f37 on first save).
inline bool ParseImage(const unsigned char* data, unsigned len,
                       unsigned char span[kSpanBytes], std::uint32_t* counter) {
    if (data == nullptr || len != kSaveSize) return false;
    std::uint32_t magic;
    std::memcpy(&magic, data, 4);
    if (magic != kMagic) return false;
    std::memcpy(span, data + kSpanOff, kSpanBytes);
    if (counter) std::memcpy(counter, data + kCounterOff, 4);
    return true;
}

}  // namespace Save
}  // namespace mashed_re
