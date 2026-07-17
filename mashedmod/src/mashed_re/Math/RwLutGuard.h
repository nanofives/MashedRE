// Mashed RE — shared RW fast-sqrt LUT root resolver + validity guard.
//
// VECCAP-1 fix (2026-07-16). History of this guard:
//   * WS-PHYS-CRASH-FIX (2026-06-17) added a per-file in-image range check
//     (kImgLo=0x10000..kImgHi=0xb40000) because the STANDALONE exe never runs
//     RwEngineOpen: the selector *(0x007d3ff8) holds boot garbage (~0xb54f88,
//     see Math/RwSqrt.cpp note) and following it AVs (MASHED_REAL_PHYSICS AV,
//     eip in FastInvSqrt, READ @0x2000).
//   * The range check OVER-rejects in the dev .asi: the live RwEngineInstance
//     and LUTs are HEAP allocations and land above 0xb40000 under current
//     Win11 layouts (captured 2026-07-16: globals 0x02b7a528 / 0x013ba528
//     across runs, LUT root 0x03c20640 / 0x03c40640) -> the hooks silently
//     took the CPU fallback, which is NOT bit-identical to the LUT math
//     (veccap faithful replay: 511/873 vectors diverged). That silently
//     voids the C4 bit-identity of every LUT-path caller.
//
// New guard, strict in both directions:
//   1. resolve slot = *(0x007d3ff8) + *(0x007d3ffc) + delta; require the slot
//      and the full 0x1000-entry table span to be committed readable pages
//      (VirtualQuery) — handles ANY heap placement, rejects unmapped garbage;
//   2. require exact content sentinels, captured from the live table built by
//      RwEngineOpen (re/tools/veccap/out/vectors_004c3ac0.json, 2026-07-16) —
//      rejects a garbage pointer chain that happens to land in readable
//      memory (it cannot reproduce 4 exact table words per table).
// The result is cached per (globals, offset) pair so the hot paths
// (FastSqrt ~2700 calls/s at menu) pay the syscall cost only once.
//
// The LUT tables are built once by RwEngineOpen and never mutated, so the
// sentinels are stable per game version (anchored to MASHED.exe SHA-256
// BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E).
#pragma once

#include <windows.h>
#include <cstdint>
#include <cstddef>

namespace RwLutGuard {

struct Sentinel { std::uint32_t idx; std::uint32_t value; };

// sqrt table (delta 0) — captured entries, veccap 2026-07-16
static const Sentinel kSqrtSentinels[] = {
    { 0x000u, 0x1fb504f3u }, { 0x001u, 0x1fb51043u },
    { 0x800u, 0x1fc00000u }, { 0xfffu, 0x1ff4ff4bu },
};
// inv-sqrt table (delta 4) — captured entries, veccap 2026-07-16
static const Sentinel kInvSqrtSentinels[] = {
    { 0x000u, 0x1f7504f3u }, { 0x001u, 0x1f74f9a4u },
    { 0x800u, 0x1f800000u }, { 0xfffu, 0x1f350a9cu },
};

struct Cache {
    std::uint32_t        rw_globals;
    std::uint32_t        rw_offset;
    const std::uint32_t* root;    // nullptr => CPU fallback
    bool                 primed;
};

inline bool ReadableSpan(std::uintptr_t addr, std::size_t size) {
    for (std::uintptr_t p = addr & ~0xFFFu; p < addr + size; p += 0x1000u) {
        MEMORY_BASIC_INFORMATION mbi;
        if (!VirtualQuery(reinterpret_cast<LPCVOID>(p), &mbi, sizeof mbi)) return false;
        if (mbi.State != MEM_COMMIT) return false;
        if (mbi.Protect & (PAGE_NOACCESS | PAGE_GUARD)) return false;
    }
    return true;
}

// Returns the validated LUT root for `delta` (0 = sqrt, 4 = inv-sqrt), or
// nullptr for the caller's CPU fallback. `cache` must be a function-local or
// file-local static owned by ONE (call site, delta) pair.
inline const std::uint32_t* Resolve(std::uint32_t delta, Cache& cache,
                                    const Sentinel* sent, std::size_t n_sent) {
    const std::uint32_t rw_globals = *reinterpret_cast<std::uint32_t*>(0x007d3ff8u);
    const std::uint32_t rw_offset  = *reinterpret_cast<std::uint32_t*>(0x007d3ffcu);
    if (cache.primed && rw_globals == cache.rw_globals && rw_offset == cache.rw_offset)
        return cache.root;
    cache.primed = true; cache.rw_globals = rw_globals; cache.rw_offset = rw_offset;
    cache.root = nullptr;

    const std::uintptr_t slotAddr = (std::uintptr_t)rw_globals + rw_offset + delta;
    if (slotAddr < 0x10000u || !ReadableSpan(slotAddr, 4u)) return cache.root;
    const std::uint32_t root = *reinterpret_cast<std::uint32_t*>(slotAddr);
    if (root < 0x10000u || (root & 3u) || !ReadableSpan(root, 0x1000u * 4u)) return cache.root;

    const std::uint32_t* table = reinterpret_cast<const std::uint32_t*>(root);
    for (std::size_t i = 0; i < n_sent; ++i)
        if (table[sent[i].idx] != sent[i].value) return cache.root;
    cache.root = table;
    return cache.root;
}

// convenience wrappers — one static cache per (TU, delta)
inline const std::uint32_t* SqrtRoot() {
    static Cache c{};
    return Resolve(0u, c, kSqrtSentinels, sizeof kSqrtSentinels / sizeof kSqrtSentinels[0]);
}
inline const std::uint32_t* InvSqrtRoot() {
    static Cache c{};
    return Resolve(4u, c, kInvSqrtSentinels, sizeof kInvSqrtSentinels / sizeof kInvSqrtSentinels[0]);
}

} // namespace RwLutGuard
