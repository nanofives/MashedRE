// Mashed RE - Save/GameSave reimplementation.
// Four functions in the gamesave cluster. All C2→C3 session c3-batch-e-s1.
//
// ─────────────────────────────────────────────────────────────────────────────
// [D4 2026-10-05] THIS TU NOW BUILDS INTO BOTH TARGETS, AND THE TWO BUILDS ARE
// NOT EQUIVALENT. Read this before trusting either.
//
// PRE-REGISTRATION: verify/d4_save_20261005/PREREG_GAMESAVE_EXE.md.
//
// The .asi build (`#ifndef MASHED_STANDALONE`) is the ORIGINAL port, unchanged —
// it calls the original's own file wrappers at 0x004b3b70 / 0x004b3bb0 /
// 0x00550b00 and uses the original's buffer at 0x00803358. That is what the four
// C4 rows were earned against and nothing below alters it.
//
// The exe build (`/DMASHED_STANDALONE`, build.bat:210/:212) CANNOT do that: all
// three callees live in address space the standalone does not map —
// 0x00400000..0x004fffff is unmapped (Compat/StandaloneRvaThunks.h:7; an access
// AVs, Frontend/MenuButtonDetect.cpp:71) and 0x00500000..0x009fffff is
// VirtualAlloc-mapped BLANK by exe_main.cpp:54, so a call there enters zeroed
// memory. A verbatim exe port needs a TWELVE-TU closure (17 callout targets, 14
// with no exe body, pulling in RenderWare stream functions, a driver dispatch, a
// texture loader and Save/SettingsDialog.cpp, because SettingsAndIO.cpp bundles
// the two gamesave file wrappers with RW stream code and a Win32 dialog).
//
// SO THE EXE BUILD IS A STATED DEVIATION, NOT A PORT. Its three file operations
// are standalone equivalents built on the CRT, and its buffer / filename /
// status storage are private. Precedent for a declared equivalent:
// CarDropNonRenderAtomics, recorded as "a measured data-driven equivalent of the
// original's selection, not a verbatim port … No hooks.csv row and no C-level
// promotion follows from it".
//
// WHAT THIS DOES NOT CLAIM:
//   * No C-level is claimed for the exe copy. The four rows gain an `exe_file`
//     and keep the C4 they earned ON THE .ASI; that C4 does NOT transfer here.
//   * No C-level is claimed for the three substituted callees. Their rows
//     (C4 impl, SettingsAndIO.cpp / GameSaveVFS.cpp) are untouched and keep an
//     empty exe_file.
//   * These bodies are PRESENT, NOT LIVE. RH_ScopedInstall resolves to the no-op
//     Stubs/HookSystemNoOp.cpp:19 in the exe, so nothing calls them until
//     Race/GameFlow.cpp is routed through them — deliberately out of scope.
// ─────────────────────────────────────────────────────────────────────────────
#include "../Core/HookSystem.h"

#include <cstdint>

#ifdef MASHED_STANDALONE
#include <cstdio>
#include <cstdlib>
#include <cstring>
#endif

// ─────────────────────────────────────────────────────────────────────────────
// 0x24fa0 — gamesave buffer size 151,456 bytes (0x00404e50 / 0x00404f50)
// [2026-10-05 corrected, comment-only] this said "150,432 bytes", which is wrong:
// 0x24FA0 == 151,456. 150,432 would be 0x24BA0. The constant itself was always
// right and no behaviour changes; only the gloss was wrong. The figure is
// corroborated on disk — original/gamesave.bin is exactly 151,456 bytes, as is
// the standalone's mashed_re_gamesave.bin.
static constexpr std::uint32_t kGameSaveSize = 0x24fa0u;

#ifndef MASHED_STANDALONE
// ── .asi build: the ORIGINAL port, textually unchanged ──────────────────────
// DAT_008a95a0 — save-status global. Written to 0 after both load and write.
// 0x004099e4: MOV [0x008a95a0], EAX
static constexpr std::uintptr_t kSaveStatusGlobal = 0x008a95a0;

// Callee stubs — referenced by address only; not linked into reimpl DLL.
// File-read wrapper  0x004b3b70: __cdecl int FUN_004b3b70(const char*, void*, uint32_t)
// File-write wrapper 0x004b3bb0: __cdecl int FUN_004b3bb0(const char*, void*, uint32_t)
// File-exists check  0x00550b00: __cdecl int FUN_00550b00(const char*)
using FileReadFn_t  = int(__cdecl*)(const char*, void*, std::uint32_t);
using FileWriteFn_t = int(__cdecl*)(const char*, void*, std::uint32_t);
using FileExistsFn_t = int(__cdecl*)(const char*);

// reinterpret_cast from integer is not constexpr in MSVC; use plain statics.
static FileReadFn_t  const gFileRead   = reinterpret_cast<FileReadFn_t >(0x004b3b70u);  // 0x004b3b70
static FileWriteFn_t const gFileWrite  = reinterpret_cast<FileWriteFn_t>(0x004b3bb0u);  // 0x004b3bb0
static FileExistsFn_t const gFileExists = reinterpret_cast<FileExistsFn_t>(0x00550b00u); // 0x00550b00

// 0x005cc8e0 — "gamesave.bin" string address (0x00404e5a / 0x00404f5a / 0x00404f80)
static const char* const kGameSaveFilename = reinterpret_cast<const char*>(0x005cc8e0u);

// 0x00803358 — gamesave buffer address (0x00404e55 / 0x00404f55)
static void* const kGameSaveBuf = reinterpret_cast<void*>(0x00803358u);

#else   // MASHED_STANDALONE
// ── exe build: PRIVATE STORAGE + CRT-BACKED FILE OPS. A DEVIATION, see header ──
//
// Every original address this TU used is unusable in the standalone, so each gets
// a private substitute. The SHAPE of the four functions below is unchanged — they
// still call read/write/exists then SaveStatusClear(0) and return 0 — so only the
// primitives differ, not the control flow.

// Substitute for DAT_008a95a0. File-static, so nothing outside this TU can alias
// it; the original's global is blank-mapped and writing there would be a silent
// no-op rather than an error, which is worse than using our own.
static std::uint32_t g_saveStatus = 0;
static const std::uintptr_t kSaveStatusGlobal =
    reinterpret_cast<std::uintptr_t>(&g_saveStatus);

// Substitute for the 0x00803358 buffer. 0x24FA0 bytes, zero-initialised.
static std::uint8_t g_saveBuf[kGameSaveSize] = {};
static void* const kGameSaveBuf = static_cast<void*>(g_saveBuf);

// Substitute for the 0x005cc8e0 "gamesave.bin" literal.
//
// DELIBERATELY NOT "gamesave.bin". Race/GameFlow.cpp:78-85 records the rule — the
// standalone writes mashed_re_gamesave.bin and "NEVER original/gamesave.bin" — and
// this TU honours it. MASHED_SAVE_PATH overrides, which is what the default-OFF
// self-test uses so it cannot clobber a real progress file.
static const char* GameSaveFilename() {
    static const char* s_path = nullptr;
    if (!s_path) {
        const char* env = std::getenv("MASHED_SAVE_PATH");
        s_path = (env && env[0]) ? env : "mashed_re_gamesave.bin";
    }
    return s_path;
}
#define kGameSaveFilename (GameSaveFilename())

// Substitute for 0x004b3b70. Reads up to `size` bytes; short reads leave the
// remainder of the buffer as it was, which is what a partial read does.
static int gFileRead(const char* path, void* buf, std::uint32_t size) {
    std::FILE* f = std::fopen(path, "rb");
    if (!f) return 0;
    const std::size_t n = std::fread(buf, 1, size, f);
    std::fclose(f);
    return static_cast<int>(n);
}

// Substitute for 0x004b3bb0.
static int gFileWrite(const char* path, void* buf, std::uint32_t size) {
    std::FILE* f = std::fopen(path, "wb");
    if (!f) return 0;
    const std::size_t n = std::fwrite(buf, 1, size, f);
    std::fclose(f);
    return static_cast<int>(n);
}

// Substitute for 0x00550b00. Returns non-zero when the file can be opened for
// reading; SaveFileExists below normalises that to {0,1} exactly as the original's
// NEG/SBB/NEG does, so the normalisation is still being exercised.
static int gFileExists(const char* path) {
    std::FILE* f = std::fopen(path, "rb");
    if (!f) return 0;
    std::fclose(f);
    return 1;
}
#endif  // MASHED_STANDALONE

// ─────────────────────────────────────────────────────────────────────────────
// 0x004099e0  SaveStatusClear
// Writes param_1 to DAT_008a95a0. Called with 0 after both save and load.
// Disasm: MOV EAX,[ESP+4]; MOV [0x008a95a0],EAX; RET
// ─────────────────────────────────────────────────────────────────────────────
extern "C" __declspec(dllexport) void __cdecl SaveStatusClear(std::uint32_t param_1) {
    *reinterpret_cast<std::uint32_t*>(kSaveStatusGlobal) = param_1;
}

RH_ScopedInstall(SaveStatusClear, 0x004099e0);  // re-enabled 2026-05-24 batch-save-b

// ─────────────────────────────────────────────────────────────────────────────
// 0x00404e50  SaveLoad
// SAVE_LOAD_FN. Calls FUN_004b3b70(filename, buf, size) then SaveStatusClear(0).
// Always returns 0 regardless of I/O result.
// Disasm (0x00404e50..0x00404e70):
//   PUSH 0x24fa0     @00404e50
//   PUSH 0x803358    @00404e55
//   PUSH 0x5cc8e0    @00404e5a
//   CALL 004b3b70    @00404e5f
//   PUSH 0           @00404e64
//   CALL 004099e0    @00404e66
//   ADD ESP,0x10     @00404e6b
//   XOR EAX,EAX      @00404e6e
//   RET              @00404e70
// ─────────────────────────────────────────────────────────────────────────────
extern "C" __declspec(dllexport) int __cdecl SaveLoad() {
    gFileRead(kGameSaveFilename, kGameSaveBuf, kGameSaveSize);
    SaveStatusClear(0);
    return 0;
}

RH_ScopedInstall(SaveLoad, 0x00404e50);  // re-enabled 2026-05-24 batch-save-a

// ─────────────────────────────────────────────────────────────────────────────
// 0x00404f50  SaveWrite
// SAVE_WRITE_FN. Mirror of SaveLoad but calls FUN_004b3bb0 (write wrapper).
// Disasm (0x00404f50..0x00404f70):
//   PUSH 0x24fa0     @00404f50
//   PUSH 0x803358    @00404f55
//   PUSH 0x5cc8e0    @00404f5a
//   CALL 004b3bb0    @00404f5f
//   PUSH 0           @00404f64
//   CALL 004099e0    @00404f66
//   ADD ESP,0x10     @00404f6b
//   XOR EAX,EAX      @00404f6e
//   RET              @00404f70
// ─────────────────────────────────────────────────────────────────────────────
extern "C" __declspec(dllexport) int __cdecl SaveWrite() {
    gFileWrite(kGameSaveFilename, kGameSaveBuf, kGameSaveSize);
    SaveStatusClear(0);
    return 0;
}

RH_ScopedInstall(SaveWrite, 0x00404f50);  // re-enabled 2026-05-24 batch-save-a

// ─────────────────────────────────────────────────────────────────────────────
// 0x00404f80  SaveFileExists
// Calls FUN_00550b00(filename); normalizes result to 0 or 1 via NEG/SBB/NEG.
// Disasm (0x00404f80..0x00404f93):
//   PUSH 0x5cc8e0    @00404f80
//   CALL 00550b00    @00404f85
//   ADD ESP,0x4      @00404f8a
//   NEG EAX          @00404f8d
//   SBB EAX,EAX      @00404f8f
//   NEG EAX          @00404f91
//   RET              @00404f93
// NEG/SBB/NEG idiom: any non-zero → 1; zero → 0.
// ─────────────────────────────────────────────────────────────────────────────
extern "C" __declspec(dllexport) int __cdecl SaveFileExists() {
    const int raw = gFileExists(kGameSaveFilename);
    // NEG/SBB/NEG: normalize to {0,1}
    return (raw != 0) ? 1 : 0;
}

RH_ScopedInstall(SaveFileExists, 0x00404f80);  // re-enabled 2026-05-24 batch-save-a

#ifdef MASHED_STANDALONE
// ─────────────────────────────────────────────────────────────────────────────
// [D4 2026-10-05] G-SMOKE — default-OFF self-test for the exe-build substitutes.
//
// PRE-REGISTERED: verify/d4_save_20261005/PREREG_GAMESAVE_EXE.md section 3.
//
// WHY THIS EXISTS. The four bodies above are PRESENT but have no callers in the
// exe (RH_ScopedInstall is the no-op HookSystemNoOp.cpp:19 there), so without an
// explicit exercise there would be no evidence they work at all — only that they
// compile. "Compile-passes-and-doesn't-crash is not acceptance" (CLAUDE.md).
//
// It is OFF unless MASHED_SAVE_SELFTEST is set, and it points MASHED_SAVE_PATH at
// its own scratch file, so it cannot touch a real progress file. It returns a
// count rather than a bool so a PARTIAL round-trip is visible as a partial number
// instead of passing as a bool.
//
// WHAT IT DOES NOT PROVE, registered before it ran: nothing about whether the
// ORIGINAL accepts the file. That is PREREG_SAVE.md's KA-ACCEPT, which is VOID
// pending a corrected address.
namespace {
// append-with-bound; returns nothing and silently stops when full
void SelfTestSay(char* out, int outSize, const char* line) {
    if (!out || outSize <= 0) return;
    const std::size_t used = std::strlen(out);
    const std::size_t room = static_cast<std::size_t>(outSize) - used - 1;
    if (used + 1 >= static_cast<std::size_t>(outSize)) return;
    std::strncpy(out + used, line, room);
    out[outSize - 1] = '\0';
}
}  // namespace

extern "C" __declspec(dllexport) int __cdecl GameSave_SelfTest(char* out, int outSize) {
    char line[160];
    #define say(...) do { std::snprintf(line, sizeof line, __VA_ARGS__); \
                          SelfTestSay(out, outSize, line); } while (0)
    if (out && outSize > 0) out[0] = '\0';

    int pass = 0;
    const char* path = GameSaveFilename();

    // step 1 — a path that does not exist must report 0
    std::remove(path);
    const int existsBefore = SaveFileExists();
    if (existsBefore == 0) ++pass;
    say("step1 exists_before=%d (want 0)\n", existsBefore);

    // step 2 — deterministic pattern, write, check the on-disk size
    for (std::uint32_t i = 0; i < kGameSaveSize; ++i)
        g_saveBuf[i] = static_cast<std::uint8_t>((i * 31u + 7u) & 0xffu);
    SaveWrite();
    long onDisk = -1;
    if (std::FILE* f = std::fopen(path, "rb")) {
        std::fseek(f, 0, SEEK_END);
        onDisk = std::ftell(f);
        std::fclose(f);
    }
    if (onDisk == static_cast<long>(kGameSaveSize)) ++pass;
    say("step2 on_disk=%ld (want %u)\n", onDisk, kGameSaveSize);

    // step 3 — now it must report 1
    const int existsAfter = SaveFileExists();
    if (existsAfter == 1) ++pass;
    say("step3 exists_after=%d (want 1)\n", existsAfter);

    // step 4 — zero the buffer, load, and compare every byte
    std::memset(g_saveBuf, 0, kGameSaveSize);
    SaveLoad();
    std::uint32_t match = 0;
    for (std::uint32_t i = 0; i < kGameSaveSize; ++i)
        if (g_saveBuf[i] == static_cast<std::uint8_t>((i * 31u + 7u) & 0xffu)) ++match;
    if (match == kGameSaveSize) ++pass;
    say("step4 roundtrip %u of %u matching\n", match, kGameSaveSize);

    // step 5 — the status store must be observable
    SaveStatusClear(0x1234u);
    const std::uint32_t st = *reinterpret_cast<std::uint32_t*>(kSaveStatusGlobal);
    if (st == 0x1234u) ++pass;
    say("step5 status=0x%04x (want 0x1234)\n", st);

    SaveStatusClear(0u);            // leave the status as the original would
    std::remove(path);              // leave no scratch file behind
    say("RESULT %d of 5 steps passed\n", pass);
    #undef say
    return pass;
}

// The trigger. A file-static initialiser, deliberately kept in THIS TU so the
// whole change is one file — no exe_main.cpp edit, nothing for another subsystem
// to inherit. With MASHED_SAVE_SELFTEST unset it does nothing at all, so it
// cannot move the default build's (b)/(e) numbers; that inertness is G-INERT and
// is measured, not asserted.
namespace {
struct GameSaveSelfTestRunner {
    GameSaveSelfTestRunner() {
        const char* on = std::getenv("MASHED_SAVE_SELFTEST");
        if (!on || !on[0] || on[0] == '0') return;
        char buf[1024] = {};
        const int pass = GameSave_SelfTest(buf, static_cast<int>(sizeof buf));
        const char* outPath = std::getenv("MASHED_SAVE_SELFTEST_OUT");
        if (!outPath || !outPath[0]) outPath = "gamesave_selftest.txt";
        if (std::FILE* f = std::fopen(outPath, "w")) {
            std::fputs(buf, f);
            std::fclose(f);
        }
        (void)pass;
    }
};
static GameSaveSelfTestRunner s_gameSaveSelfTestRunner;
}  // namespace
#endif  // MASHED_STANDALONE
