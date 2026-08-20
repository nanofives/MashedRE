// Slice B (save/rva-neutralize) verification — proves the RVA-tunnel neutralization
// in Save/GameSaveBuffer.cpp for the standalone (MASHED_STANDALONE) build.
//
// The two ported functions SerializeToBuffer (0x00404ee0) / DeserializeFromBuffer
// (0x00404e80) memcpy fixed data globals in/out of a save buffer. In the standalone
// build every base must resolve to a NAMED standalone symbol (no MASHED address):
//   * SHARED engine state is BOUND BY NAME — the live champ span via
//     mashed_re::Frontend::Nav_SaveSpanData(), the live save counter via
//     mashed_re::Race::GameFlow_SaveCounterPtr(). This test SUPPLIES those two
//     accessors over its OWN storage so it can watch Serialize snapshot the bound
//     state and Deserialize restore it — the bind is the whole point of the slice.
//   * A MASHED absolute deref (0x007xxxxx/0x008xxxxx) would AV in THIS process, so
//     surviving the round-trip is itself proof no MASHED address survived.
//
// This exercises exactly the code the slice changed (the two functions are DEAD
// EXPORTS in mashed_re.exe — the RH_ScopedInstall registrar is no-op'd there, so
// nothing on the default path calls them; here we call them directly).
//
// Build (from mashedmod/src/mashed_re):
//   cl /nologo /EHsc /std:c++17 /DMASHED_STANDALONE /I.. ^
//      Save\tests\gamesave_buffer_bind_test.cpp Save\GameSaveBuffer.cpp ^
//      /Fe:build\gamesave_buffer_bind_test.exe
//   build\gamesave_buffer_bind_test.exe      (exit 0 = GREEN)
#include <cstdint>
#include <cstdio>
#include <cstring>

// Stub the exe's no-op hook registrar (Stubs/HookSystemNoOp.cpp analogue) so the
// RH_ScopedInstall static ctors in GameSaveBuffer.cpp link.
namespace HookSystem {
    void Register(std::uintptr_t, void*, const char*) {}
}

// Test-owned bind targets standing in for the live standalone engine state. Their
// addresses are link-time constants, so returning them from the accessors is valid
// even at static-init time (when GameSaveBuffer.cpp initialises its kTrackTable /
// kSaveStateCounter bases from these calls).
static unsigned char s_testSpan[0x520];      // stands in for Frontend g_save_span
static std::uint32_t s_testCounter = 0;      // stands in for Race g_saveCounter
namespace mashed_re { namespace Frontend { unsigned char* Nav_SaveSpanData() { return s_testSpan; } } }
namespace mashed_re { namespace Race     { std::uint32_t* GameFlow_SaveCounterPtr() { return &s_testCounter; } } }

// The neutralized functions under test (extern "C" exports in GameSaveBuffer.cpp).
extern "C" void SerializeToBuffer();
extern "C" void DeserializeFromBuffer();

static int g_fail = 0;
static void check(const char* what, bool ok) {
    std::printf("  %s %s\n", ok ? "[ok]  " : "[FAIL]", what);
    if (!ok) ++g_fail;
}

int main() {
    std::printf("gamesave_buffer_bind_test (Slice B RVA-tunnel neutralization)\n");

    // Seed the BOUND live state with a recognisable pattern. Serialize's stride-gather
    // writes 12 zero bytes into span[0x514..0x520) (standalone has no stride records),
    // so seed that tail as zero up front to keep the round-trip exact end-to-end.
    for (int i = 0; i < 0x514; ++i)
        s_testSpan[i] = static_cast<unsigned char>((i * 7 + 3) & 0xff);
    std::memset(s_testSpan + 0x514, 0, 0x520 - 0x514);
    s_testCounter = 0x12345678u;

    unsigned char spanBefore[0x520];
    std::memcpy(spanBefore, s_testSpan, sizeof spanBefore);
    const std::uint32_t counterBefore = s_testCounter;

    // 1. Serialize snapshots the BOUND span + counter into GameSaveBuffer's private
    //    save-side scratch. Reaching the next line means no MASHED-address AV.
    SerializeToBuffer();
    check("Serialize ran without a MASHED-address AV", true);

    // 2. Clobber the live state; Deserialize must restore it from the snapshot.
    std::memset(s_testSpan, 0xAB, sizeof s_testSpan);
    s_testCounter = 0xFFFFFFFFu;
    DeserializeFromBuffer();

    // 3. The BOUND live state round-trips: Serialize read it, Deserialize wrote it
    //    back — proving both bind to Nav_SaveSpanData()/GameFlow_SaveCounterPtr(),
    //    not private duplicates and not MASHED addresses.
    check("champ span restored [0..0x514) via bind",
          std::memcmp(s_testSpan, spanBefore, 0x514) == 0);
    bool tailZero = true;
    for (int i = 0x514; i < 0x520; ++i) if (s_testSpan[i] != 0) tailZero = false;
    check("span tail [0x514..0x520) packed to zero (no stride records)", tailZero);
    check("save counter restored via bind", s_testCounter == counterBefore);

    std::printf("\n%s — %d failures\n", g_fail ? "RED" : "GREEN", g_fail);
    return g_fail ? 1 : 0;
}
