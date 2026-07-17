// re/tools/veccap/replay_offline.cpp
//
// Registry-driven offline captured-vector replayer (generalized 2026-07-16).
// Reconstructs the memory regions the target functions read (captured live by
// capture_vectors.py + statics packed from the exe file) inside THIS process,
// then runs the ported implementations (compiled in UNCHANGED from
// mashedmod/src) against every captured vector and bit-compares returns and
// out-buffers. No game process involved.
//
// Modes:
//   faithful  — regions mapped at the exact captured addresses
//   relocated — LUTs placed in a low TLS-reserved granule (alternate layout)
//
// Vectors flagged degenerate (flags bit1) are SKIPPED and counted: on that
// path the normalize functions call the RW error stubs (0x004d7ff0/0x004d8480,
// C1) which only exist in a live game image.
//
// Usage: replay_offline.exe <vectors.bin> <faithful|relocated>
// Exit 0 = all compared vectors bit-identical; 1 = mismatches; 2 = setup error.
#include <windows.h>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <vector>
#include <string>

// --- image pad ---------------------------------------------------------------
// Deterministically OWN the low game-address range by making THIS image span it
// (same idea as the standalone exe's B17 image pad, exe_main.cpp). Linked
// /BASE:0x10000, a pad this large pushes the image end past 0x00b40000, so the
// fixed game addresses the ports read — the RW globals selector 0x007d3ff8 and
// the .rdata statics at 0x005d757c — land inside our own committed image pages.
// No VirtualAlloc lottery for those. Only the FAITHFUL-mode captured HEAP
// addresses (high, ~0x03exxxxx) are mapped dynamically, and high user space is
// uncontended so that reliably succeeds.
//
// It MUST be referenced or /O2 strips the unreferenced .bss array (that was the
// earlier "pad vanished" bug); the write in main() forces it in.
static char g_image_pad[0x00B40000];
volatile char g_pad_sink = 0;

// --- stub out the hook framework so ported TUs compile unchanged ------------
#include "../../../mashedmod/src/mashed_re/Core/HookSystem.h"
namespace HookSystem {
void Register(std::uintptr_t, void*, const char*) {}
}

// --- the ported functions under test, verbatim (linked TUs) ------------------
extern "C" float __cdecl Vec3Magnitude(const float* v);
extern "C" float __cdecl FastSqrt(float x);
extern "C" float __cdecl FastInvSqrt(float x);
extern "C" float __cdecl Vec2Length(const float* v);
extern "C" float __cdecl Vec2Normalize(float* out, const float* in);
extern "C" float __cdecl RwV3dNormalize(float* out, const float* in);
// B5e solver-island pure leaves — copied from r7/b5e-solver-island
// (Collision/RwpSolverLeaves1.cpp). Void return, out-buffer compared only.
extern "C" void __cdecl FUN_00566830(float* out, const float* in);                 // v_out_in
extern "C" void __cdecl FUN_00565ef0(float* out, const float* a, const float* b);  // v_out_2in
extern "C" void __cdecl FUN_00565fa0(float* out, const float* a, const float* b, float s);

enum Kind : std::uint32_t {
    KIND_F_F = 1, KIND_F_PTRN = 2, KIND_F_OUT_IN = 3, KIND_V_OUT_IN = 4, KIND_V_OUT_2IN = 5
};

typedef float (__cdecl* FnFF)(float);
typedef float (__cdecl* FnFPtr)(const float*);
typedef float (__cdecl* FnFOutIn)(float*, const float*);
typedef void  (__cdecl* FnVOutIn)(float*, const float*);
typedef void  (__cdecl* FnVOut2In)(float*, const float*, const float*);
typedef void  (__cdecl* FnVOut2InS)(float*, const float*, const float*, float);

struct ExportRow { const char* name; void* fn; };
static const ExportRow kExports[] = {
    { "Vec3Magnitude",  reinterpret_cast<void*>(&Vec3Magnitude) },
    { "FastSqrt",       reinterpret_cast<void*>(&FastSqrt) },
    { "FastInvSqrt",    reinterpret_cast<void*>(&FastInvSqrt) },
    { "Vec2Length",     reinterpret_cast<void*>(&Vec2Length) },
    { "Vec2Normalize",  reinterpret_cast<void*>(&Vec2Normalize) },
    { "RwV3dNormalize", reinterpret_cast<void*>(&RwV3dNormalize) },
    { "FUN_00566830",   reinterpret_cast<void*>(&FUN_00566830) },
    { "FUN_00565ef0",   reinterpret_cast<void*>(&FUN_00565ef0) },
    { "FUN_00565fa0",   reinterpret_cast<void*>(&FUN_00565fa0) },
};

// Ensure [addr, addr+size) is committed writable memory. Works page-by-page so
// a span that straddles free / our-reserved / already-committed pages (the
// address-space lottery, see exe_main.cpp B17 notes) is handled uniformly.
static bool MapAt(std::uint32_t addr, std::uint32_t size) {
    const std::uint32_t first = addr & ~0xFFFu;
    const std::uint32_t last  = (addr + size - 1u) & ~0xFFFu;
    for (std::uint32_t p = first; p <= last; p += 0x1000u) {
        MEMORY_BASIC_INFORMATION q{};
        if (!VirtualQuery(reinterpret_cast<void*>(p), &q, sizeof q)) return false;
        if (q.State == MEM_COMMIT) {
            if (q.Protect & (PAGE_READWRITE | PAGE_WRITECOPY | PAGE_EXECUTE_READWRITE))
                continue;
            return false;   // committed but read-only (a foreign mapping) — lost
        }
        if (q.State == MEM_FREE) {
            // MEM_RESERVE is 64 KB-granular — reserve the whole aligned granule,
            // then commit the page. If the granule is partly occupied, reserve
            // fails and this layout is lost (driver retries in a fresh process).
            const std::uint32_t gran = p & ~0xFFFFu;
            if (!VirtualAlloc(reinterpret_cast<void*>(gran), 0x10000u,
                              MEM_RESERVE, PAGE_READWRITE)) {
                std::printf("SETUP-FAIL: reserve granule 0x%08x failed (gle=%lu)\n",
                            gran, GetLastError());
                return false;
            }
        }
        if (!VirtualAlloc(reinterpret_cast<void*>(p), 0x1000u, MEM_COMMIT, PAGE_READWRITE)) {
            std::printf("SETUP-FAIL: commit page 0x%08x failed (state=0x%lx gle=%lu)\n",
                        p, q.State, GetLastError());
            return false;
        }
    }
    return true;
}

static bool ReadN(FILE* f, void* dst, std::size_t n) {
    return std::fread(dst, 1, n, f) == n;
}

int main(int argc, char** argv) {
    if (argc != 3) { std::printf("usage: replay_offline.exe <vectors.bin> <faithful|relocated>\n"); return 2; }
    const bool relocated = std::strcmp(argv[2], "relocated") == 0;

    // force the image pad to be linked in (see its comment) and report its span
    g_pad_sink = g_image_pad[0] + g_image_pad[sizeof(g_image_pad) - 1];
    const std::uintptr_t pad_lo = reinterpret_cast<std::uintptr_t>(&g_image_pad[0]);
    const std::uintptr_t pad_hi = pad_lo + sizeof(g_image_pad);
    if (pad_hi < 0x007e0000u) {
        std::printf("SETUP-FAIL: image pad [0x%08zx,0x%08zx) does not cover fixed addrs\n",
                    pad_lo, pad_hi);
        return 2;
    }

    FILE* f = std::fopen(argv[1], "rb");
    if (!f) { std::printf("SETUP-FAIL: cannot open %s\n", argv[1]); return 2; }
    std::uint32_t magic = 0, n_funcs = 0;
    std::uint32_t rw_globals, rw_offset, slot_addr, lut_root, lut_inv_root, lut_size;
    if (!ReadN(f, &magic, 4) || magic != 0x32504356u /*'VCP2'*/ ||
        !ReadN(f, &n_funcs, 4) || !ReadN(f, &rw_globals, 4) || !ReadN(f, &rw_offset, 4) ||
        !ReadN(f, &slot_addr, 4) || !ReadN(f, &lut_root, 4) || !ReadN(f, &lut_inv_root, 4) ||
        !ReadN(f, &lut_size, 4)) { std::printf("SETUP-FAIL: bad header\n"); return 2; }
    std::vector<std::uint8_t> lut(lut_size), lut_inv(lut_size);
    if (!ReadN(f, lut.data(), lut_size) || !ReadN(f, lut_inv.data(), lut_size)) {
        std::printf("SETUP-FAIL: lut read\n"); return 2;
    }

    // --- reconstruct memory --------------------------------------------------
    if (relocated) {
        const std::uint32_t base = 0x00a00000u;
        rw_globals = base; rw_offset = 0; slot_addr = base;
        lut_root = base + 0x1000u; lut_inv_root = base + 0x6000u;
    }
    if (!MapAt(0x007d3ff8, 8) || !MapAt(slot_addr, 8) ||
        !MapAt(lut_root, lut_size) || !MapAt(lut_inv_root, lut_size)) { return 2; }
    *reinterpret_cast<std::uint32_t*>(0x007d3ff8) = rw_globals;
    *reinterpret_cast<std::uint32_t*>(0x007d3ffc) = rw_offset;
    *reinterpret_cast<std::uint32_t*>(static_cast<std::uintptr_t>(slot_addr))      = lut_root;
    *reinterpret_cast<std::uint32_t*>(static_cast<std::uintptr_t>(slot_addr) + 4u) = lut_inv_root;
    std::memcpy(reinterpret_cast<void*>(static_cast<std::uintptr_t>(lut_root)),     lut.data(),     lut_size);
    std::memcpy(reinterpret_cast<void*>(static_cast<std::uintptr_t>(lut_inv_root)), lut_inv.data(), lut_size);

    // statics from the exe file (mapped at their true addresses in BOTH modes)
    std::uint32_t n_statics = 0;
    if (!ReadN(f, &n_statics, 4)) { std::printf("SETUP-FAIL: statics count\n"); return 2; }
    for (std::uint32_t i = 0; i < n_statics; ++i) {
        std::uint32_t addr, size;
        if (!ReadN(f, &addr, 4) || !ReadN(f, &size, 4)) { std::printf("SETUP-FAIL: static hdr\n"); return 2; }
        std::vector<std::uint8_t> bytes(size);
        if (!ReadN(f, bytes.data(), size)) { std::printf("SETUP-FAIL: static bytes\n"); return 2; }
        if (!MapAt(addr, size)) return 2;
        std::memcpy(reinterpret_cast<void*>(static_cast<std::uintptr_t>(addr)), bytes.data(), size);
    }

    // --- replay ---------------------------------------------------------------
    std::uint32_t total_fail = 0;
    for (std::uint32_t fi = 0; fi < n_funcs; ++fi) {
        char name[32] = {};
        std::uint32_t kind, rva, ni, no, na, sflag, nv;
        if (!ReadN(f, name, 32) || !ReadN(f, &kind, 4) || !ReadN(f, &rva, 4) ||
            !ReadN(f, &ni, 4) || !ReadN(f, &no, 4) || !ReadN(f, &na, 4) ||
            !ReadN(f, &sflag, 4) || !ReadN(f, &nv, 4)) {
            std::printf("SETUP-FAIL: func header %u\n", fi); return 2;
        }
        void* fn = nullptr;
        for (const auto& row : kExports)
            if (std::strncmp(row.name, name, 32) == 0) { fn = row.fn; break; }
        if (!fn) { std::printf("SETUP-FAIL: unknown export %s\n", name); return 2; }

        std::uint32_t fail = 0, skipped = 0, shown = 0;
        for (std::uint32_t i = 0; i < nv; ++i) {
            std::uint32_t in_bits[16], ret_exp, out_exp[16];
            std::uint8_t flags;
            if (!ReadN(f, in_bits, ni * 4) || !ReadN(f, &ret_exp, 4) ||
                !ReadN(f, out_exp, no * 4) || !ReadN(f, &flags, 1)) {
                std::printf("SETUP-FAIL: vec read %s#%u\n", name, i); return 2;
            }
            if (flags & 2u) { ++skipped; continue; }   // degenerate -> live-only path

            float in_f[16], out_f[16];
            std::memcpy(in_f, in_bits, ni * 4);
            // seed out with the SAME 0xcc sentinel the capture used, so out slots a
            // function leaves untouched (e.g. AABB padding index 3) compare equal.
            std::memset(out_f, 0xcc, no * 4);

            float r = 0.0f;
            const bool has_ret = (kind != KIND_V_OUT_IN && kind != KIND_V_OUT_2IN);
            if (kind == KIND_F_F)           r = reinterpret_cast<FnFF>(fn)(in_f[0]);
            else if (kind == KIND_F_PTRN)   r = reinterpret_cast<FnFPtr>(fn)(in_f);
            else if (kind == KIND_F_OUT_IN) r = reinterpret_cast<FnFOutIn>(fn)(out_f, in_f);
            else if (kind == KIND_V_OUT_IN) reinterpret_cast<FnVOutIn>(fn)(out_f, in_f);
            else {                          // KIND_V_OUT_2IN: in2 = in_f + n_a (contiguous)
                const float* in2 = in_f + na;
                if (sflag) reinterpret_cast<FnVOut2InS>(fn)(out_f, in_f, in2, in_f[ni - 1]);
                else       reinterpret_cast<FnVOut2In>(fn)(out_f, in_f, in2);
            }

            std::uint32_t ret_got = ret_exp;  // void kinds: neutralize the ret compare
            if (has_ret) std::memcpy(&ret_got, &r, 4);
            bool bad = (ret_got != ret_exp);
            for (std::uint32_t k = 0; k < no && !bad; ++k) {
                std::uint32_t got_k;
                std::memcpy(&got_k, &out_f[k], 4);
                if (got_k != out_exp[k]) bad = true;
            }
            if (bad) {
                ++fail;
                if (shown < 3) {
                    std::printf("MISMATCH %s#%u in0=%g expected_ret=0x%08x got=0x%08x\n",
                                name, i, in_f[0], ret_exp, ret_got);
                    ++shown;
                }
            }
        }
        std::printf("%-16s mode=%s vectors=%u skipped_degenerate=%u mismatches=%u  %s\n",
                    name, relocated ? "relocated" : "faithful", nv, skipped, fail,
                    fail ? "FAIL" : "PASS");
        total_fail += fail;
    }
    std::fclose(f);
    std::printf("TOTAL: %s\n", total_fail ? "FAIL" : "PASS");
    return total_fail ? 1 : 0;
}
