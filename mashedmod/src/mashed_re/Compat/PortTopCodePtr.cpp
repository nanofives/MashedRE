// RVA-LINT: ORIGTOP-ONLY
// PORT-OT-SEED-CODEPTR (L-111 (a), follows PORT-OT-SEED-2): the code-pointer data the port-state seed left out.
//
// PORT-OT-SEED-2 seeded the original's file-backed .rdata / .data bytes for the ORIG_TOP rows, except 22 entries
// whose bytes are function entries of the original (vtables, handler tables, PTR_FUN_ globals). Copied verbatim
// those would be original code addresses, which are not the original's code in mashed_re.exe. This file binds
// them instead: every dword of those entries that points into the original's .text gets the exe address of its
// target's body (the exe symbol BINDINGS.tsv records for that RVA), or, when the exe has no body for that RVA, a
// stop thunk that logs the target and ends the process (the RwCore_Missing pattern: a named stop in place of a
// call into exe code at an original address). The non-code dwords of the same entries, and the strings they point
// at, are seeded with the original's bytes.
//
// Generated table: Compat/PortTopCodePtr.inc (verify/portorigtop_20261109/codeptr_inventory.py --write).
// Knob: MASHED_ORIG_TOP=1 only (MASHED_NO_OT_CODEPTR=1 opts out for A/B). The default path never calls this, so its bytes and gates cannot move.
#include "PortTopCodePtr.h"

#include <windows.h>
#include <cstdarg>
#include <cstdint>
#include <cstdio>
#include <cstring>

extern "C" char g_b17_low_arena_pad[0x00A00000];   // exe_main.cpp B17: the original-address data pad

// Exe bodies the table binds to (C linkage; the real prototypes live in their own TUs. Only the address is taken).
#define CP_EXTERN(sym) extern "C" void sym(void);
#include "PortTopCodePtr.inc"
#undef CP_EXTERN

namespace {

const char* s_logPath = "mashed_re.log";

void CodePtrLog(const char* fmt, ...) {
    if (std::FILE* f = std::fopen(s_logPath, "a")) {
        va_list ap;
        va_start(ap, fmt);
        std::fputs("[OT_CODEPTR] ", f);
        std::vfprintf(f, fmt, ap);
        std::fputc('\n', f);
        va_end(ap);
        std::fclose(f);
    }
}

[[noreturn]] void CodePtrStop(unsigned target) {
    CodePtrLog("STOP: a bound code pointer to original 0x%08X was called; the exe has no body for it. exit 0x%08X",
               target, target);
    ExitProcess(target);
}

} // namespace

// One stop thunk per unbound target. It never returns, so the caller's calling convention does not matter.
#define CP_STOP(target, name) extern "C" void name(void) { CodePtrStop(target); }
#include "PortTopCodePtr.inc"
#undef CP_STOP

namespace mashed_re {
namespace Compat {

unsigned PortTopCodePtr_Apply(const char* logPath) {
    static bool s_done = false;
    if (s_done) return 0;
    s_done = true;
    if (logPath) s_logPath = logPath;
    // A/B opt-out for the knob boots of this row (MASHED_NO_OT_CODEPTR=1): no write, one log line.
    if (GetEnvironmentVariableA("MASHED_NO_OT_CODEPTR", nullptr, 0) != 0) {
        CodePtrLog("opted out (MASHED_NO_OT_CODEPTR): no slot written");
        return 0;
    }
    const std::uintptr_t lo = reinterpret_cast<std::uintptr_t>(&g_b17_low_arena_pad[0]);
    const std::uintptr_t hi = lo + sizeof(g_b17_low_arena_pad);
    unsigned fixes = 0, bound = 0, stops = 0, seeds = 0, seedBytes = 0, outside = 0, failed = 0;

    // Seeds first: a slot never overlaps a seed (the generator takes seeds only from non-code dwords).
#define CP_SEED(va, n, bytes)                                                                       \
    if ((va) < lo || (va) + (n) > hi) { ++outside; }                                                \
    else { std::memcpy(reinterpret_cast<void*>(static_cast<std::uintptr_t>(va)), bytes, n); ++seeds; seedBytes += (n); }
#define CP_FIX(slot, sym, orig)                                                                     \
    if ((slot) < lo || (slot) + 4 > hi) { ++outside; }                                              \
    else {                                                                                          \
        *reinterpret_cast<std::uint32_t*>(static_cast<std::uintptr_t>(slot)) =                      \
            static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(&sym));                     \
        ++fixes;                                                                                    \
        if (std::strncmp(#sym, "PortTopCodePtr_Stop_", 20) == 0) ++stops; else ++bound;             \
    }
#include "PortTopCodePtr.inc"
#undef CP_FIX
#undef CP_SEED

    // Self-test: read every slot and seed back. A slot must hold its symbol's exe address, inside the exe image and
    // not inside the pad; a seed must equal the table bytes.
    const std::uintptr_t img = reinterpret_cast<std::uintptr_t>(GetModuleHandleA(nullptr));
    const IMAGE_NT_HEADERS* nt = reinterpret_cast<const IMAGE_NT_HEADERS*>(
        img + reinterpret_cast<const IMAGE_DOS_HEADER*>(img)->e_lfanew);
    const std::uintptr_t imgEnd = img + nt->OptionalHeader.SizeOfImage;
    unsigned checks = 0;
#define CP_SEED(va, n, bytes)                                                                       \
    if (!((va) < lo || (va) + (n) > hi)) {                                                          \
        ++checks;                                                                                   \
        if (std::memcmp(reinterpret_cast<const void*>(static_cast<std::uintptr_t>(va)), bytes, n)) { \
            ++failed; CodePtrLog("self-test FAIL: seed 0x%08X (%u B) differs", (unsigned)(va), (unsigned)(n)); } }
#define CP_FIX(slot, sym, orig)                                                                     \
    if (!((slot) < lo || (slot) + 4 > hi)) {                                                        \
        ++checks;                                                                                   \
        const std::uint32_t v = *reinterpret_cast<const std::uint32_t*>(static_cast<std::uintptr_t>(slot)); \
        if (v != static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(&sym)) || v < img || v >= imgEnd || \
            (v >= lo && v < hi)) {                                                                  \
            ++failed; CodePtrLog("self-test FAIL: slot 0x%08X (orig 0x%08X) holds %08X, want %s", (unsigned)(slot), \
                                 (unsigned)(orig), v, #sym); } }
#include "PortTopCodePtr.inc"
#undef CP_FIX
#undef CP_SEED

    CodePtrLog("applied: %u slots (%u bound to an exe body, %u to a stop thunk), %u data seeds (%u B), %u outside the "
               "pad (table: %u slots, %u seeds); self-test %s (%u of %u checks failed)",
               fixes, bound, stops, seeds, seedBytes, outside, (unsigned)PORTTOP_CODEPTR_SLOTS,
               (unsigned)PORTTOP_CODEPTR_SEEDS, (failed || outside) ? "FAIL" : "PASS", failed, checks);
    return failed + outside;
}

} // namespace Compat
} // namespace mashed_re
