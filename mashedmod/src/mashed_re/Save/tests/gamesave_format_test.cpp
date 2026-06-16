// WS-G5 verification — real gamesave.bin format round-trip (Save/GameSaveFormat.h).
//
// Proves the standalone writer/reader produce a byte-faithful real-format image
// (magic@0, championship span@0x24A40, counter@0x24EFC, size 0x24FA0) and that a
// progression survives write -> read unchanged, and that the DEADBEEF magic gate
// rejects a blank / wrong-size image.
//
// Build (from mashedmod/src/mashed_re):
//   cl /EHsc /I.. Save\tests\gamesave_format_test.cpp
//   Save\tests\gamesave_format_test.exe
#include "../GameSaveFormat.h"

#include <cstdio>
#include <cstring>
#include <vector>

using namespace mashed_re::Save;

static int g_fail = 0;
static void check(const char* what, bool ok) {
    std::printf("  %s %s\n", ok ? "[ok]  " : "[FAIL]", what);
    if (!ok) ++g_fail;
}
// Compare two spans ignoring the save-counter dword at span+0x4bc — the writer
// overwrites that position in the image with the live counter (file +0x24EFC,
// FUN_00404f23), so the image span differs there from the input table by design.
static bool span_eq_ignore_counter(const unsigned char* a, const unsigned char* b) {
    return std::memcmp(a, b, kSpanCounterOff) == 0 &&
           std::memcmp(a + kSpanCounterOff + 4, b + kSpanCounterOff + 4,
                       kSpanBytes - kSpanCounterOff - 4) == 0;
}

// Build a championship span from a per-area (unlock, trophy) progression, exactly
// as GameFlow does: track-unlock col (verified) + a standalone-private trophy col,
// plus the savedata gate when any progress exists.
static void span_from_progress(const int unlock[kSpanRows], const int trophy[kSpanRows],
                               unsigned char span[kSpanBytes]) {
    std::memset(span, 0, kSpanBytes);
    bool any = false;
    for (int r = 0; r < kSpanRows; ++r) {
        std::uint32_t u = unlock[r] ? 2u : 0u;             // real "track available" = 2
        std::uint32_t t = static_cast<std::uint32_t>(trophy[r]);
        std::memcpy(span + r * kRowStride + kColTrackUnlock, &u, 4);
        std::memcpy(span + r * kRowStride + kColTrophyPriv,  &t, 4);
        if (unlock[r] || trophy[r]) any = true;
    }
    std::uint32_t gate = any ? 1u : 0u;
    std::memcpy(span + kSpanSavedataGate, &gate, 4);
}

int main() {
    std::printf("WS-G5 — gamesave.bin format round-trip\n");

    // A progression: tracks 0..3 unlocked, trophies gold/silver/bronze/none.
    int unlock[kSpanRows] = {1,1,1,1,0,0,0,0,0,0,0,0,0};
    int trophy[kSpanRows] = {3,2,1,0,0,0,0,0,0,0,0,0,0};
    unsigned char span[kSpanBytes];
    span_from_progress(unlock, trophy, span);

    // 1. BuildImage -> byte layout assertions.
    std::vector<unsigned char> img(kSaveSize);
    BuildImage(span, /*counter=*/7, img.data());
    check("image size == 0x24FA0", img.size() == kSaveSize);
    std::uint32_t m; std::memcpy(&m, img.data(), 4);
    check("magic 0xDEADBEEF at +0", m == kMagic);
    std::uint32_t c; std::memcpy(&c, img.data() + kCounterOff, 4);
    check("counter at file +0x24EFC", c == 7);
    // span landed at file 0x24A40 (counter dword excepted — it overwrites span+0x4bc)
    check("span copied at +0x24A40", span_eq_ignore_counter(img.data() + kSpanOff, span));
    // profile region (file +4 .. +0x24A40) is all zero (no live profile block).
    bool prof_zero = true;
    for (unsigned i = kProfileOff; i < kSpanOff; ++i) if (img[i]) { prof_zero = false; break; }
    check("profile region all zero", prof_zero);

    // 2. ParseImage round-trip: span + counter recovered exactly.
    unsigned char span2[kSpanBytes]; std::uint32_t c2 = 0;
    bool ok = ParseImage(img.data(), kSaveSize, span2, &c2);
    check("ParseImage accepts written save", ok);
    check("counter round-trips", c2 == 7);
    check("span round-trips (counter dword excepted)", span_eq_ignore_counter(span, span2));

    // 3. progression recovered from the round-tripped span.
    bool prog_ok = true;
    for (int r = 0; r < kSpanRows; ++r) {
        std::uint32_t u, t;
        std::memcpy(&u, span2 + r * kRowStride + kColTrackUnlock, 4);
        std::memcpy(&t, span2 + r * kRowStride + kColTrophyPriv,  4);
        bool want_unlock = (unlock[r] != 0);
        if ((u != 0) != want_unlock) prog_ok = false;
        if (static_cast<int>(t) != trophy[r]) prog_ok = false;
    }
    check("unlock+trophy recovered per area", prog_ok);
    // savedata gate set (drives menu screen-1 Load/Restart enable via the reader).
    std::uint32_t gate; std::memcpy(&gate, span2 + kSpanSavedataGate, 4);
    check("savedata gate set when progressed", gate == 1);

    // 4. magic gate rejects blank + wrong-size images.
    std::vector<unsigned char> blank(kSaveSize, 0);
    check("blank (magic 0) rejected", !ParseImage(blank.data(), kSaveSize, span2, &c2));
    check("wrong-size rejected", !ParseImage(img.data(), kSaveSize - 1, span2, &c2));

    // 5. file round-trip (write to a standalone-copy temp; never original/).
    const char* path = "mashed_re_gamesave_test.bin";
    if (std::FILE* f = std::fopen(path, "wb")) {
        std::fwrite(img.data(), 1, kSaveSize, f); std::fclose(f);
    }
    std::vector<unsigned char> rd(kSaveSize);
    bool fileok = false;
    if (std::FILE* f = std::fopen(path, "rb")) {
        size_t n = std::fread(rd.data(), 1, kSaveSize, f); std::fclose(f);
        fileok = (n == kSaveSize) && (std::memcmp(rd.data(), img.data(), kSaveSize) == 0);
    }
    check("file write/read byte-identical", fileok);
    std::remove(path);

    std::printf("\n%s — %d failures\n", g_fail ? "RED" : "GREEN", g_fail);
    return g_fail ? 1 : 0;
}
