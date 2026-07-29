// Mashed RE — snapshot/restore A/B driver for 0x004e4320 (RwPluginLinkSet).
//
// First driver on a target chosen by measurement: exercised in a Quick Battle
// race (pre-screen), A/B-REACHABLE (no indirect dispatch in its call tree), and
// a MUTATOR the synthetic lane cannot judge. Only 6 of 142 reachable targets are
// also known-exercised; this is the smallest of them.
//
// WHY RESTORE IS LOAD-BEARING HERE, not ceremony:
// the callee 0x004c0e50 always does *(u8*)(node+3) |= 3, and its own
// `test cl,3 / jne 0x4c0eb7` at 0x004c0e62 skips the list insertion when that
// bit is already set. So after the PORT runs, the ORIGINAL would take a
// DIFFERENT BRANCH. Without a correct restore the two sides are not comparable
// and the diff is meaningless — which is exactly the class of error this whole
// lane exists to avoid.
//
// WRITE SURFACE — 2 windows found by scripts/write_surface.py, 5 more it flagged
// for reading and I resolved from the disassembly. All are RUNTIME-COMPUTED, so
// they are derived fresh on every call, BEFORE either side runs (both sides then
// start from the same restored state, so the addresses are identical for both):
//
//   pluginOff = *(u32*)0x007d716c
//   W1  obj + pluginOff + 0xc          4B   the outer store
//   arg  = *(u32*)(obj + 4)                 [skip W2..W7 if 0]
//   W2  arg + 3                        1B   *(u8*)(arg+3) |= 0xc   (always)
//   node = *(u32*)(arg + 0xa0)
//   W3  node + 3                       1B   *(u8*)(node+3) |= 3    (always)
//   ...if !(*(u8*)(node+3) & 3), the list insertion also runs:
//   head = *(u32*)0x007d3ff8
//   W4  node + 8                       4B   node->next = *(head+0xbc)
//   W5  node + 0xc                     4B   node->prev = head+0xbc
//   W6  head + 0xbc                    4B   list head := node+8
//   W7  (*(u32*)(head+0xbc)) + 4       4B   old head node's back-pointer
//
// W7's ADDRESS depends on the list head's value BEFORE the call, which is why
// every window is recomputed per call rather than cached.
//
// TRAMPOLINE. The 5-byte inline JMP clobbers 0x004e4320..0x004e4324 — all of
// `mov eax,[esp+8]` (4 B) AND the first byte of `mov ecx,[0x007d716c]` — so the
// trampoline re-executes BOTH and resumes at 0x004e432a (`push esi`).
// The `ds:` override on the absolute load is REQUIRED: without it MSVC
// assembles `mov ecx, dword ptr [0x007d716c]` as mov ecx, IMMEDIATE 0x7d716c
// and it compiles clean (memory feedback_msvc_inline_asm_needs_ds_override).
// The emitted bytes are disassembled from the built .asi and checked.
//
// Env-gated: MASHED_LINK_AB=1, and REFUSED without MASHED_HOOK_ONLY because the
// port hook sits at the same RVA. Driver name AbLink43 shares no substring with
// the port name RwPluginLinkSet (MASHED_HOOK_ONLY matches as a substring BOTH
// ways; a driver whose name contains a port name installs both and the port
// silently clobbers the driver).
//
// A GREEN is A/B with the hook installed at the target RVA driven by natural
// call sites in a live race => C3. NOT C4: the ORIGINAL's writes are committed,
// so the port never steers the scenario.
#include "../Core/HookSystem.h"
#include <windows.h>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>

extern "C" void __cdecl RwPluginLinkSet(int, void*);

namespace {

void* g_orig_4e432a = reinterpret_cast<void*>(0x004e432a);

__declspec(naked) void __cdecl Orig4e4320(int, void*) {
    __asm {
        mov eax, dword ptr [esp + 8]        // 0x004e4320
        mov ecx, dword ptr ds:[0x007d716c]  // 0x004e4324  (ds: REQUIRED)
        jmp dword ptr [g_orig_4e432a]       // resume at push esi
    }
}

void AbLog(const char* s) {
    HANDLE h = CreateFileA("link_ab.log", FILE_APPEND_DATA, FILE_SHARE_READ, nullptr,
                           OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (h == INVALID_HANDLE_VALUE) return;
    DWORD w; WriteFile(h, s, (DWORD)std::strlen(s), &w, nullptr);
    CloseHandle(h);
}

struct Seg { std::uint8_t* p; std::uint32_t n; };
constexpr int kMaxSeg = 8;

bool Readable(const void* p, std::uint32_t n) {
    return p && !IsBadReadPtr(p, n) && !IsBadWritePtr(const_cast<void*>(p), n);
}

// Recompute every window from the CURRENT state. Called once per pair, before
// either side runs.
int BuildSegs(void* obj, Seg* segs) {
    int k = 0;
    const std::uintptr_t o = reinterpret_cast<std::uintptr_t>(obj);
    if (!Readable(reinterpret_cast<void*>(o), 8)) return 0;

    const std::uint32_t off = *reinterpret_cast<const volatile std::uint32_t*>(0x007d716cu);
    std::uint8_t* w1 = reinterpret_cast<std::uint8_t*>(o + off + 0xcu);
    if (Readable(w1, 4)) segs[k++] = {w1, 4};

    const std::uint32_t arg = *reinterpret_cast<const std::uint32_t*>(o + 4u);
    if (!arg || !Readable(reinterpret_cast<void*>(arg), 0xa4)) return k;
    segs[k++] = {reinterpret_cast<std::uint8_t*>(arg + 3u), 1};                 // W2

    const std::uint32_t node = *reinterpret_cast<const std::uint32_t*>(arg + 0xa0u);
    if (!node || !Readable(reinterpret_cast<void*>(node), 0x10)) return k;
    segs[k++] = {reinterpret_cast<std::uint8_t*>(node + 3u), 1};                // W3
    segs[k++] = {reinterpret_cast<std::uint8_t*>(node + 8u), 4};                // W4
    segs[k++] = {reinterpret_cast<std::uint8_t*>(node + 0xcu), 4};              // W5

    const std::uint32_t head = *reinterpret_cast<const volatile std::uint32_t*>(0x007d3ff8u);
    if (!head || !Readable(reinterpret_cast<void*>(head + 0xbcu), 4)) return k;
    std::uint8_t* w6 = reinterpret_cast<std::uint8_t*>(head + 0xbcu);
    segs[k++] = {w6, 4};                                                        // W6
    const std::uint32_t oldhead = *reinterpret_cast<const std::uint32_t*>(w6);
    if (oldhead && Readable(reinterpret_cast<void*>(oldhead + 4u), 4))
        segs[k++] = {reinterpret_cast<std::uint8_t*>(oldhead + 4u), 4};         // W7
    return k;
}

unsigned g_pairs = 0, g_mism = 0, g_skipped = 0;

// COVERAGE, because "28000 pairs, 0 mismatches" does not say which BRANCHES ran.
// The callee early-outs on *(u8*)(node+3) & 3; if every call took that arm then
// W4..W7 (the list insertion) were never written and their agreement is vacuous
// — a port that omitted the insertion entirely would still read GREEN. That is
// feedback_evidence_discipline §2: which line could I delete and still pass?
// g_seg_written[i] counts pairs where the ORIGINAL actually changed segment i;
// g_ins / g_early sample the predicate directly, before either side runs.
unsigned g_seg_written[kMaxSeg] = {0};
unsigned g_ins = 0, g_early = 0;

void __cdecl AbLink43(int value, void* obj) {
    Seg segs[kMaxSeg];
    const int k = BuildSegs(obj, segs);
    if (k == 0) {                       // cannot bound the surface -> do NOT
        ++g_skipped;                    // guess; run the original only.
        Orig4e4320(value, obj);
        return;
    }
    // Sample the callee's branch predicate BEFORE either side runs.
    {
        const std::uintptr_t o = reinterpret_cast<std::uintptr_t>(obj);
        const std::uint32_t arg = *reinterpret_cast<const std::uint32_t*>(o + 4u);
        if (arg && Readable(reinterpret_cast<void*>(arg), 0xa4)) {
            const std::uint32_t node = *reinterpret_cast<const std::uint32_t*>(arg + 0xa0u);
            if (node && Readable(reinterpret_cast<void*>(node), 4)) {
                if (*reinterpret_cast<const volatile std::uint8_t*>(node + 3u) & 3) ++g_early;
                else ++g_ins;
            }
        }
    }
    std::uint8_t snap[64], vmine[64], vorig[64];
    std::uint32_t tot = 0;
    for (int i = 0; i < k; ++i) tot += segs[i].n;
    if (tot > sizeof snap) { ++g_skipped; Orig4e4320(value, obj); return; }

    std::uint32_t at = 0;
    for (int i = 0; i < k; ++i) { std::memcpy(snap + at, segs[i].p, segs[i].n); at += segs[i].n; }

    RwPluginLinkSet(value, obj);
    at = 0;
    for (int i = 0; i < k; ++i) { std::memcpy(vmine + at, segs[i].p, segs[i].n); at += segs[i].n; }

    at = 0;                                            // RESTORE
    for (int i = 0; i < k; ++i) { std::memcpy(segs[i].p, snap + at, segs[i].n); at += segs[i].n; }

    Orig4e4320(value, obj);
    at = 0;
    for (int i = 0; i < k; ++i) { std::memcpy(vorig + at, segs[i].p, segs[i].n); at += segs[i].n; }

    ++g_pairs;
    {   // did the ORIGINAL actually write each segment, or is its agreement vacuous?
        std::uint32_t o3 = 0;
        for (int i = 0; i < k; ++i) {
            if (std::memcmp(vorig + o3, snap + o3, segs[i].n) != 0) ++g_seg_written[i];
            o3 += segs[i].n;
        }
    }
    if (std::memcmp(vmine, vorig, tot) != 0) {
        ++g_mism;
        char buf[320];
        int m = std::snprintf(buf, sizeof buf,
            "MISMATCH pair=%u segs=%d value=%08x obj=%p mask=", g_pairs, k,
            (unsigned)value, obj);
        std::uint32_t o2 = 0;
        for (int i = 0; i < k && m < (int)sizeof buf - 8; ++i) {
            m += std::snprintf(buf + m, sizeof buf - m, "%c",
                               std::memcmp(vmine + o2, vorig + o2, segs[i].n) ? 'X' : '.');
            o2 += segs[i].n;
        }
        std::snprintf(buf + m, sizeof buf - m, "\r\n");
        AbLog(buf);
    } else if ((g_pairs % 500) == 0) {
        char buf[320];
        int m = std::snprintf(buf, sizeof buf,
            "ok pairs=%u mism=%u skipped=%u segs=%d insert=%u earlyout=%u written=",
            g_pairs, g_mism, g_skipped, k, g_ins, g_early);
        for (int i = 0; i < k && m < (int)sizeof buf - 16; ++i)
            m += std::snprintf(buf + m, sizeof buf - m, "%u%s",
                               g_seg_written[i], (i + 1 < k) ? "/" : "");
        std::snprintf(buf + m, sizeof buf - m, "\r\n");
        AbLog(buf);
    }
}

struct AbLinkReg {
    AbLinkReg() {
        const char* s = std::getenv("MASHED_LINK_AB");
        if (!s || !s[0]) return;
        const char* only = std::getenv("MASHED_HOOK_ONLY");
        if (!only || !only[0]) {
            AbLog("MASHED_LINK_AB requires MASHED_HOOK_ONLY=AbLink43 - refusing "
                  "to register (collides with the port hook at the same RVA)\r\n");
            return;
        }
        HookSystem::Register(0x004e4320u, reinterpret_cast<void*>(&AbLink43), "AbLink43");
    }
} g_ab_link_reg;

} // namespace
