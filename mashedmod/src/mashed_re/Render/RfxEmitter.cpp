// Render/RfxEmitter.cpp — see RfxEmitter.h. ONE body per RVA for both build targets (D4-DC-RFX-CORE).
//
// Before this TU each of the 15 staging / batch / submit RVAs had two bodies that could drift apart, the
// dual-copy class (re/analysis/DUAL_COPY_AUDIT_2026-09-29.md, ROADMAP D4): the .asi's C3 hooks
// (Render/FrameHelpers_q2.cpp, Util/PromoLoop_sessionB.cpp) on the original's raw globals, and the exe's
// D5-RFX-EMITTERS core on its own storage (a Staging struct, std::vector arrays, the PTank by value). This TU
// is in BOTH exe_sources.rsp and asi_sources.rsp. RH_ScopedInstall installs the 15 in the .asi and is a no-op
// in the exe, so the exe runs the very bodies the .asi's Frida evidence judges.
//
// Layouts are the original's: the staging globals at their absolute addresses (in the exe the image-pad
// owns 0x006924d8 .. 0x006925a8, exe_main.cpp B17), the batch struct by offset (Batch), the plugin block as
// *(atomic + [0x007dc57c]) (PTank). [0x007dc57c] has one writer in the original, FUN_00534a80 (0x00534ac4,
// the plugin registration); the exe has none, so it reads 0 there and Create places the plugin pointer at
// atomic + 0.
//
// The null-pointer defaults 0x00613288 / 0x00613290 / 0x00613294 / 0x006132a4 are file-backed .data with NO
// direct writer in the original (decomp_pc.py --datarefs, verify/rfxcore_20261101/RESULT_D4-DC-RFX-CORE.md
// section 2). The .asi reads them at their addresses, as the original does; the exe's image-pad holds 0 there, so
// under MASHED_STANDALONE the same body points at the file-backed values instead. The two BSS defaults keep their
// addresses in both targets: 0x00692558 (the matrix template, written by FUN_004770c0) and 0x006925a8 (no writer: 0).
//
// [D5-RFX-EMITTERS 2026-10-28] Create / Flush / SelectCallbacks / Instance were transcribed from the disassembly
// of original/MASHED.exe.unpatched and checked dword for dword against those bytes run in Unicorn
// (verify/rfx_emitters_20261028/emit_unicorn.py); re-run on this core in verify/rfxcore_20261101/.
// Arrays hold raw dwords so every copy is a bit copy, as the original's MOVs and REP MOVSD are.
//
// Anchored to MASHED.exe SHA-256:
//   BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E
//   (preserved in original\MASHED.exe.unpatched)

#include "RfxEmitter.h"

#include "../Core/HookSystem.h"

#include <cstdio>
#include <cstring>

namespace {

float F(std::uint32_t u) { float f; std::memcpy(&f, &u, 4); return f; }
std::uint32_t U(float f) { std::uint32_t u; std::memcpy(&u, &f, 4); return u; }

// The null-pointer defaults. The .asi reads the original's own .data, as the original does (the negctl fixtures
// that hold these rows at C3 seed these addresses and require the read). The exe's image-pad is zero-filled there,
// so the exe points at the file-backed values (read from the anchored binary; no writer in the original).
#ifdef MASHED_STANDALONE
std::uint32_t s00613288[2] = {0x3f800000u, 0x3f800000u};              // 1.0, 1.0
std::uint32_t s00613290    = 0xffffffffu;
std::uint32_t s00613294[4] = {0u, 0u, 0x3f800000u, 0x3f800000u};      // 0, 0, 1, 1
std::uint32_t s006132a4[8] = {0u, 0x3f800000u, 0x3f800000u, 0x3f800000u,
                              0x3f800000u, 0u, 0u, 0u};               // 0,1,1,1,1,0,0,0
std::uint32_t* const k00613288 = s00613288;
std::uint32_t* const k00613290 = &s00613290;
std::uint32_t* const k00613294 = s00613294;
std::uint32_t* const k006132a4 = s006132a4;
#else
std::uint32_t* const k00613288 = reinterpret_cast<std::uint32_t*>(0x00613288u);
std::uint32_t* const k00613290 = reinterpret_cast<std::uint32_t*>(0x00613290u);
std::uint32_t* const k00613294 = reinterpret_cast<std::uint32_t*>(0x00613294u);
std::uint32_t* const k006132a4 = reinterpret_cast<std::uint32_t*>(0x006132a4u);
#endif
const std::int32_t k006132c4 = 5, k006132c8 = 6;
const float        k005cc32c = 0.5f;

constexpr std::uintptr_t kTemplate = 0x00692558u;   // DAT_00692558, 16 dwords
constexpr std::uintptr_t kPlugOff  = 0x007dc57cu;   // DAT_007dc57c, the PTank plugin offset

std::uint32_t& G(std::uintptr_t a) { return *reinterpret_cast<std::uint32_t*>(a); }

// *(*(p1 + 4) + [0x007dc57c]): the plugin block of the batch's atomic.
std::uint8_t* Plugin(const void* batch) {
    const std::uint8_t* atomic = *reinterpret_cast<std::uint8_t* const*>(static_cast<const std::uint8_t*>(batch) + 4);
    return *reinterpret_cast<std::uint8_t* const*>(atomic + G(kPlugOff));
}

// x87 FSINCOS under the precision control the original runs at (PC_24, D3D's FPU state). The exe's default
// control word is PC_53, so it is set for the one instruction and restored.
void SinCos24(float a, float* s, float* c) {
#if defined(_M_IX86)
    unsigned short cw, cw24;
    float ls, lc;
    __asm {
        fnstcw cw
        mov    ax, cw
        and    ax, 0xfcff
        mov    cw24, ax
        fldcw  cw24
        fld    a
        fsincos
        fstp   lc
        fstp   ls
        fldcw  cw
    }
    *s = ls;
    *c = lc;
#else
#error RfxEmitter.cpp needs the x86 FSINCOS path
#endif
}

}  // namespace

// ===========================================================================
// Staging setters (FUN_00476d00 copies the globals they fill)
// ===========================================================================

// MATCH-S12: the body of 0x004769a0 (ParticleEmitter_SetPosition) moved to Match/MatchS12_004769a0.cpp,
// which compiles the byte-matched source re/match/s12/004769a0.c.

// MATCH-S12: the body of ParticleEmitter_SetVelocity moved to Match/MatchS12_004769d0.cpp (PORT-STATE-02),
// which compiles the byte-matched source re/match/s12/004769d0.c.

// MATCH-S12: the body of ParticleEmitter_SetColour moved to Match/MatchS12_004769f0.cpp (PORT-STATE-02),
// which compiles the byte-matched source re/match/s12/004769f0.c.

// MATCH-S12: the body of 0x00476a10 (Copy476a10) moved to Match/MatchS12_00476a10.cpp,
// which compiles the byte-matched source re/match/s12/00476a10.c.

// MATCH-S12: the body of 0x00476a30 (ParticleEmitter_SetScalar) moved to Match/MatchS12_00476a30.cpp,
// which compiles the byte-matched source re/match/s12/00476a30.c.

// 0x00476a40
extern "C" __declspec(dllexport) void __cdecl ParticleEmitter_SetRGBA(std::uint32_t* param_1)
{
    const std::uint32_t* s = param_1 ? param_1 : k00613294;
    G(0x00692598u) = s[0];
    G(0x0069259cu) = s[1];
    G(0x006925a0u) = s[2];
    G(0x006925a4u) = s[3];
}
RH_ScopedInstall(ParticleEmitter_SetRGBA, 0x00476a40);  // re-enabled 2026-05-24 c3-render-b

// 0x00476a80
extern "C" __declspec(dllexport) void __cdecl Copy476a80(std::uint32_t* p)
{
    const std::uint32_t* s = p ? p : k006132a4;
    std::uint32_t* d = reinterpret_cast<std::uint32_t*>(0x00692534u);
    for (int i = 0; i < 8; i++) d[i] = s[i];
}
RH_ScopedInstall(Copy476a80, 0x00476a80);

// ===========================================================================
// Batch setters: p1 = the batch, plugin = *(*(p1 + 4) + [0x007dc57c]); each ORs one dirty bit at +0x40
// ===========================================================================

// MATCH-S12: the body of Batch476ae0 moved to Match/MatchS12_00476ae0.cpp (PORT-STATE-02),
// which compiles the byte-matched source re/match/s12/00476ae0.c.

// 0x00476b30: val = *(p2 ? p2 : 0x00613290); plugin +0xc4 = val; material 0 (atomic +0x18 -> +0x20 -> [0])
// +4 = plugin +0xc4 when the material exists (0x00476b74); plugin +0x40 |= 0x2000. Every plugin access
// re-reads the atomic and [0x007dc57c], as the original does.
extern "C" __declspec(dllexport) void __cdecl Set476b30(std::uint8_t* p1, std::uint8_t* p2)
{
    std::uint32_t val = *reinterpret_cast<std::uint32_t*>(p2 ? p2 : reinterpret_cast<std::uint8_t*>(k00613290));
    std::uint32_t g = G(kPlugOff);
    std::uint8_t* e = *reinterpret_cast<std::uint8_t**>(p1 + 4);
    std::uint8_t* obj = *reinterpret_cast<std::uint8_t**>(e + g);
    *reinterpret_cast<std::uint32_t*>(obj + 0xc4) = val;
    std::uint8_t* x = *reinterpret_cast<std::uint8_t**>(e + 0x18);
    std::uint8_t* y = *reinterpret_cast<std::uint8_t**>(x + 0x20);
    std::uint8_t* n = *reinterpret_cast<std::uint8_t**>(y);
    if (n != nullptr) {
        std::uint8_t* obj2 = *reinterpret_cast<std::uint8_t**>(e + G(kPlugOff));
        *reinterpret_cast<std::uint32_t*>(n + 4) = *reinterpret_cast<std::uint32_t*>(obj2 + 0xc4);
    }
    std::uint8_t* obj3 = *reinterpret_cast<std::uint8_t**>(e + G(kPlugOff));
    *reinterpret_cast<std::uint32_t*>(obj3 + 0x40) |= 0x2000u;
}
RH_ScopedInstall(Set476b30, 0x00476b30);

// MATCH-S12: the body of 0x00476b90 (Batch476b90) moved to Match/MatchS12_00476b90.cpp,
// which compiles the byte-matched source re/match/s12/00476b90.c.

// MATCH-S12: the body of 0x00476be0 (Batch476be0) moved to Match/MatchS12_00476be0.cpp,
// which compiles the byte-matched source re/match/s12/00476be0.c.

// 0x00476c10
extern "C" __declspec(dllexport) void __cdecl Batch476c10(std::uint32_t* p1, std::uint32_t* p2)
{
    if (!p2) p2 = k00613294;
    std::uint8_t* b = Plugin(p1);
    *reinterpret_cast<std::uint32_t*>(b + 0xd8) = p2[0];
    *reinterpret_cast<std::uint32_t*>(b + 0xdc) = p2[1];
    *reinterpret_cast<std::uint32_t*>(b + 0xe0) = p2[2];
    *reinterpret_cast<std::uint32_t*>(b + 0xe4) = p2[3];
    *reinterpret_cast<std::uint32_t*>(b + 0x40) |= 0x80000u;
}
RH_ScopedInstall(Batch476c10, 0x00476c10);

// 0x00476c60
extern "C" __declspec(dllexport) void __cdecl Batch476c60(std::uint32_t* p1, std::uint32_t* p2)
{
    if (!p2) p2 = k006132a4;
    std::uint8_t* b = Plugin(p1);
    std::uint32_t* d = reinterpret_cast<std::uint32_t*>(b + 0xd8);
    for (int k = 0; k < 8; k++) d[k] = p2[k];
    *reinterpret_cast<std::uint32_t*>(b + 0x40) |= 0x100000u;
}
RH_ScopedInstall(Batch476c60, 0x00476c60);

// MATCH-S12: the body of 0x00476cb0 (Batch476cb0) moved to Match/MatchS12_00476cb0.cpp,
// which compiles the byte-matched source re/match/s12/00476cb0.c.

// ===========================================================================
// Submit: if (count < capacity) copy the staging globals into each present batch array at [count], ++count.
// A pure leaf. The copies are dword MOVs / REP MOVSD in the original, so bit copies here.
// ===========================================================================

// MATCH-S12: the body of 0x00476d00 (MultiArrayScatter476d00) moved to Match/MatchS12_00476d00.cpp,
// which compiles the byte-matched source re/match/s12/00476d00.c.

// ===========================================================================
// The exe's side: create, flush, the instance callbacks (no .asi install)
// ===========================================================================

namespace mashed_re {
namespace Rfx {
namespace {

// Size v to n elements of `per` dwords when `on`; returns the storage pointer (nullptr when off).
std::uint32_t* Alloc(std::vector<std::uint32_t>& v, bool on, std::size_t n, std::size_t per) {
    v.assign(on ? n * per : 0, 0u);
    return v.empty() ? nullptr : v.data();
}

// FUN_00535700(atomic, out, flag, mode): {ptr, stride} of the plugin array for `flag`; {0, 0} when the
// plugin's data flags (desc +0x58 = +0xa0) lack it (0x00535715..0x0053572d). A non-zero desc +0x54 (+0x9c)
// replaces the stride (0x005357c0..0x005357c8). Under mode 0x40000000 a non-null lock ORs flag | mode into
// +0x3c (0x005357cb..0x005357ed).
Lock LockArray(PTank& t, std::uint32_t flag, std::uint32_t mode) {
    Lock out{nullptr, 0};
    if ((t.data_flags & flag) == 0) return out;
    const Lock* l = nullptr;
    switch (flag) {
    case 1: l = &t.pos; break;     case 8: l = &t.mat; break;      case 0x10: l = &t.nrm; break;
    case 4: l = &t.size; break;    case 2: l = &t.col; break;      case 0x40: l = &t.col4; break;
    case 0x20: l = &t.rot; break;  case 0x80: l = &t.uv2; break;   case 0x100: l = &t.uv4; break;
    default: break;
    }
    if (l) out = *l;
    if (t.stride_all != 0) out.stride = t.stride_all;
    if (out.ptr && (mode & 0x40000000u) == 0x40000000u) t.lock_bits |= flag | mode;
    return out;
}

}  // namespace

// 0x00534b60
std::uint32_t PTankNormaliseFlags(std::uint32_t f) {
    const std::uint8_t b = static_cast<std::uint8_t>(f);
    const bool pos = (b & 1) == 1, mat = (b & 8) == 8, rot = (b & 0x20) == 0x20;
    const std::uint32_t u100 = f & 0x100, u20000 = f & 0x20000, u40000 = f & 0x40000,
                        u80000 = f & 0x80000, u100000 = f & 0x100000;
    if (mat && (f & 0x8000) == 0x8000) f = 0x8000;
    if (rot && u20000 == 0x20000) f &= 0x20000;
    if ((b & 0x40) == 0x40 && u40000 == 0x40000) f &= 0x40000;
    if ((b & 0x80) == 0x80 && u80000 == 0x80000) f &= 0x80000;
    if (u100 == 0x100 && u100000 == 0x100000) f &= 0x100000;
    if (mat) {
        if (pos) f &= 0xfffffffeu;
        if ((b & 4) == 4) f &= 0xfffffffbu;
        if (rot) f &= 0xffffffdfu;
    }
    if (!mat && !pos) f |= 1;
    if ((f & 0x30000000) == 0) f |= 0x10000000;
    return f;
}

// 0x004770c0
void Create(Emitter& e, std::uint32_t flags, std::int32_t cap, const void* texture) {
    std::uint32_t d = 0x10000000u;
    if (flags == 0) flags = 0x807;
    if (flags & 1) d = 0x10000001u;
    if (flags & 2) d |= 4;
    if (flags & 4) d |= 2;
    if (flags & 8) d |= 8;
    if (flags & 0x10) d |= 0x20;
    if (flags & 0x20) d |= 0x80;
    if (flags & 0x40) d |= 0x100;
    if (flags & 0x200) d |= 0x8000;
    if (flags & 0x400) d |= 0x20000;
    if (flags & 0x800) d |= 0x80000;
    if (flags & 0x1000) d |= 0x100000;
    e.batch = Batch{};                               // FUN_004b6520(param_1, 0x40)
    e.tank = PTank{};
    e.mat0 = Emitter::Material{};
    e.c4_set = false;
    std::memset(e.geo, 0, sizeof(e.geo));
    std::memset(e.atom, 0, sizeof(e.atom));

    // The stand-in atomic: the plugin pointer at +[0x007dc57c] (0 in the exe), the geometry at +0x18.
    const std::uint32_t off = G(kPlugOff);
    e.ok = off + 4 <= sizeof(e.atom) && (off & 3) == 0 && off != 0x18;
    if (!e.ok) {
        std::fprintf(stderr, "[rfx] plugin offset [0x007dc57c] = 0x%x does not fit the stand-in atomic\n", off);
        return;
    }
    std::uint8_t* atom = reinterpret_cast<std::uint8_t*>(e.atom);
    *reinterpret_cast<PTank**>(atom + off) = &e.tank;
    *reinterpret_cast<std::uint32_t**>(atom + 0x18) = e.geo;
    e.mat_list[0] = &e.mat0;
    *reinterpret_cast<Emitter::Material***>(reinterpret_cast<std::uint8_t*>(e.geo) + 0x20) = e.mat_list;
    e.batch.atomic = atom;

    // FUN_00534b60(cap, d, 0) -> FUN_00534d00: +0 / +4 / +0xa0 / +0x40 / +0x3c (0x00534d67..0x00534d80).
    // Not transcribed: the ARRAY-layout allocation (0x0053510c..). The port sizes each array max_count
    // elements by its data flag, packed (stride = the element size FUN_00476df0 copies).
    PTank& t = e.tank;
    t.max_count = cap;
    t.count = 0;
    t.data_flags = PTankNormaliseFlags(d);
    t.dirty = 0xffffffffu;
    t.lock_bits = 0;
    const std::size_t n = cap > 0 ? static_cast<std::size_t>(cap) : 0;
    t.pos  = {Alloc(e.t_pos,  (t.data_flags & 1) != 0, n, 3),     12};
    t.size = {Alloc(e.t_size, (t.data_flags & 4) != 0, n, 2),     8};
    t.col  = {Alloc(e.t_col,  (t.data_flags & 2) != 0, n, 1),     4};
    t.mat  = {Alloc(e.t_mat,  (t.data_flags & 8) != 0, n, 16),    64};
    t.rot  = {Alloc(e.t_rot,  (t.data_flags & 0x20) != 0, n, 1),  4};
    t.uv2  = {Alloc(e.t_uv2,  (t.data_flags & 0x80) != 0, n, 4),  16};
    t.uv4  = {Alloc(e.t_uv4,  (t.data_flags & 0x100) != 0, n, 8), 32};

    // the matrix template DAT_00692558 (FUN_004770c0 decomp); +0x0c / +0x1c / +0x2c / +0x3c are left as they are
    std::uint32_t* m = reinterpret_cast<std::uint32_t*>(kTemplate);
    m[3] |= 0x20003u;
    m[10] = 0x3f800000u; m[5] = 0x3f800000u; m[0] = 0x3f800000u;
    m[4] = 0; m[2] = 0; m[1] = 0; m[9] = 0; m[8] = 0; m[6] = 0; m[14] = 0; m[13] = 0; m[12] = 0;

    e.batch.flags = flags;
    if (static_cast<std::int8_t>(flags) < 0) SetBatchB8(e, nullptr);
    if (flags & 0x100) SetBatchColour(e, nullptr);
    if (flags & 0x200) SetBatchMatrix(e, nullptr);
    if (flags & 0x400) SetBatchC0(e, 0);
    if (flags & 0x800) SetBatchUv2(e, nullptr);
    if (flags & 0x1000) SetBatchUv4(e, nullptr);
    e.mat0.texture = texture;                        // FUN_004e8090(material 0, param_4)
    t.vtx_alpha = 1;
    t.src_blend = k006132c4;
    t.dst_blend = k006132c8;
    t.dirty |= 0x10000000u;
    t.dirty |= 0x800000u;
    t.count = 0;
    // FUN_004c0b30 / FUN_004e7e30: a fresh frame (identity LTM) on the atomic.
    Batch& b = e.batch;
    b.capacity = cap;
    b.pos  = Alloc(e.b_pos,  (flags & 1) != 0, n, 3);      // FUN_00474db0(cap, 0xc)
    b.size = Alloc(e.b_size, (flags & 2) != 0, n, 2);      // (cap, 8)
    b.col  = Alloc(e.b_col,  (flags & 4) != 0, n, 1);      // (cap, 4)
    b.mat  = Alloc(e.b_mat,  (flags & 8) != 0, n, 16);     // (cap, 0x40)
    b.rot  = Alloc(e.b_rot,  (flags & 0x10) != 0, n, 1);   // (cap, 4)
    b.uv2  = Alloc(e.b_uv2,  (flags & 0x20) != 0, n, 4);   // (cap * 2, 8)
    b.uv4  = Alloc(e.b_uv4,  (flags & 0x40) != 0, n, 8);   // (cap * 4, 8)
}

void SetRotate(float r) { ParticleEmitter_SetScalar(U(r)); }

void SetBatchColour(Emitter& e, const std::uint32_t* rgba) {
    Set476b30(reinterpret_cast<std::uint8_t*>(&e.batch),
              reinterpret_cast<std::uint8_t*>(const_cast<std::uint32_t*>(rgba)));
    e.c4_set = true;
}

// 0x00476df0. plugin +0x40 |= 0x800000, plugin +4 = count; then each present batch array goes through
// FUN_00535700 (lock, mode 0x40000000) and is copied element by element at the lock's stride; one
// FUN_00535910 (unlock: +0x40 |= +0x3c, +0x3c = 0) ends the batch and the count is cleared. A lock on a flag
// the PTank lacks returns a null pointer, which the original then writes through; the port skips that copy
// (no caller pairs such flags).
void Flush(Emitter& e) {
    if (!e.ok) return;
    Batch& b = e.batch;
    PTank& t = *reinterpret_cast<PTank*>(Plugin(&b));
    t.dirty |= 0x800000u;
    t.count = b.count;
    if (b.count == 0) return;
    const int n = b.count;
    auto copy = [&](const std::uint32_t* src, std::uint32_t flag, std::size_t per) {
        if (!src) return;
        const Lock l = LockArray(t, flag, 0x40000000u);
        if (!l.ptr) return;
        std::uint8_t* dst = reinterpret_cast<std::uint8_t*>(l.ptr);
        for (int i = 0; i < n; ++i, dst += l.stride)
            std::memcpy(dst, src + static_cast<std::size_t>(i) * per, per * 4);
    };
    copy(b.pos, 1, 3);
    copy(b.size, 4, 2);
    copy(b.col, 2, 1);
    copy(b.mat, 8, 16);
    copy(b.rot, 0x20, 1);
    copy(b.uv2, 0x80, 4);
    copy(b.uv4, 0x100, 8);
    t.dirty |= t.lock_bits;                          // FUN_00535910
    t.lock_bits = 0;
    b.count = 0;
}

const std::uint32_t* TankElem(const PTank& t, const Lock& l, int i) {
    if (!l.ptr) return nullptr;
    const std::uint32_t stride = t.stride_all != 0 ? t.stride_all : l.stride;
    return reinterpret_cast<const std::uint32_t*>(reinterpret_cast<const std::uint8_t*>(l.ptr) +
                                                  static_cast<std::size_t>(i) * stride);
}

// 0x00536780
Callbacks SelectCallbacks(std::uint32_t f) {
    Callbacks c{};
    c.render = 0x00537fb0;
    const bool center = (f & 0x1000000) == 0x1000000;
    if ((f & 8) == 8) {
        c.pos = center ? 0x005379c0 : 0x00537890;
    } else if ((f & 0x8000) != 0x8000 && (f & 0x20) == 0x20) {
        if ((f & 4) == 4) c.pos = center ? 0x00537610 : 0x00536fe0;
        else              c.pos = center ? 0x005371f0 : 0x00536c90;
    } else if ((f & 0x8000) != 0x8000 && (f & 4) == 4) {
        c.pos = center ? 0x00537460 : 0x00536e90;
    } else {
        c.pos = center ? 0x00536ad0 : 0x00536940;
    }
    if ((f & 0x80) == 0x80)            c.uv = 0x00537d70;
    else if ((f & 0x100) == 0x100)     c.uv = 0x00537de0;
    else if ((f & 0x80000) == 0x80000) c.uv = 0x00537e60;
    else if ((f & 0x100000) == 0x100000) c.uv = 0x00537ed0;
    else                               c.uv = 0;
    if ((f & 2) == 2)                  c.col = 0x00537b20;
    else if ((f & 0x40) == 0x40)       c.col = 0x00537b80;
    else if ((f & 0x40000) == 0x40000) c.col = 0x00537c40;
    else                               c.col = 0x00537d20;
    c.end = 0x00538300;
    c.normal = ((f & 0x10) == 0x10) ? 0x00537f40 : 0;
    return c;
}

bool Instance(const PTank& t, bool c4_known, const float r[3], const float u[3], Streams& out) {
    const Callbacks cb = SelectCallbacks(t.data_flags);
    if (cb.pos != 0x00536e90 && cb.pos != 0x00536fe0) return false;
    if (cb.uv != 0 && cb.uv != 0x00537d70 && cb.uv != 0x00537e60) return false;
    if (cb.col != 0x00537b20 && cb.col != 0x00537d20) return false;
    if (cb.col == 0x00537d20 && !c4_known) return false;   // +0xc4 initial value not transcribed
    if (cb.normal != 0) return false;
    const int n = t.count;
    if (n <= 0) { out.xyz.clear(); out.uv.clear(); out.argb.clear(); return true; }
    out.xyz.assign(static_cast<std::size_t>(n) * 12, 0.f);
    out.uv.assign(cb.uv ? static_cast<std::size_t>(n) * 8 : 0, 0.f);
    out.argb.assign(static_cast<std::size_t>(n) * 4, 0u);
    float* o = out.xyz.data();
    for (int i = 0; i < n; ++i, o += 12) {
        const std::uint32_t* P = TankElem(t, t.pos, i);
        const std::uint32_t* S = TankElem(t, t.size, i);
        const float px = F(P[0]), py = F(P[1]), pz = F(P[2]);
        const float sx = F(S[0]), sy = F(S[1]);
        float Xx, Xy, Xz, Yx, Yy, Yz;
        if (cb.pos == 0x00536e90) {
            // 0x00536ed0..0x00536f11
            const float hx = sx * k005cc32c;
            Xx = hx * r[0]; Xy = hx * r[1]; Xz = hx * r[2];
            const float hy = sy * k005cc32c;
            Yx = hy * u[0]; Yy = hy * u[1]; Yz = hy * u[2];
        } else {
            // 0x00537042..0x00537133: fsincos, then the rotated pair scaled by size * 0.5
            float s, c;
            SinCos24(F(*TankElem(t, t.rot, i)), &s, &c);
            const float ns = -s;
            const float r0x = (ns * u[0]) + (c * r[0]);
            const float r0y = (ns * u[1]) + (c * r[1]);
            const float r0z = (ns * u[2]) + (c * r[2]);
            const float u0x = (s * r[0]) + (c * u[0]);
            const float u0y = (s * r[1]) + (c * u[1]);
            const float u0z = (s * r[2]) + (c * u[2]);
            Xx = (r0x * sx) * k005cc32c; Xy = (r0y * sx) * k005cc32c; Xz = (r0z * sx) * k005cc32c;
            Yx = (u0x * sy) * k005cc32c; Yy = (u0y * sy) * k005cc32c; Yz = (u0z * sy) * k005cc32c;
        }
        // v0 = (-X - Y) + p, v1 = (X - Y) + p, v2 = (Y + X) + p, v3 = (Y - X) + p (both instancers)
        o[0] = (-Xx - Yx) + px;  o[1] = (-Xy - Yy) + py;  o[2] = (-Xz - Yz) + pz;
        o[3] = (Xx - Yx) + px;   o[4] = (Xy - Yy) + py;   o[5] = (Xz - Yz) + pz;
        o[6] = (Yx + Xx) + px;   o[7] = (Yy + Xy) + py;   o[8] = (Yz + Xz) + pz;
        o[9] = (Yx - Xx) + px;   o[10] = (Yy - Xy) + py;  o[11] = (Yz - Xz) + pz;
    }
    // uv: (u0,v1) (u1,v1) (u1,v0) (u0,v0) from a per-particle (u0,v0,u1,v1) (0x00537d70) or plugin +0xd8
    // (0x00537e60)
    for (int i = 0; i < n && cb.uv; ++i) {
        const std::uint32_t* q = (cb.uv == 0x00537d70) ? TankElem(t, t.uv2, i) : t.f_d8;
        float* w = &out.uv[static_cast<std::size_t>(i) * 8];
        w[0] = F(q[0]); w[1] = F(q[3]);
        w[2] = F(q[2]); w[3] = F(q[3]);
        w[4] = F(q[2]); w[5] = F(q[1]);
        w[6] = F(q[0]); w[7] = F(q[1]);
    }
    // colour: the RGBA bytes b0..b3 packed b3<<24 | b0<<16 | b1<<8 | b2 on all four vertices
    for (int i = 0; i < n; ++i) {
        const std::uint32_t v = (cb.col == 0x00537b20) ? *TankElem(t, t.col, i) : t.f_c4;
        const std::uint32_t b0 = v & 0xff, b1 = (v >> 8) & 0xff, b2 = (v >> 16) & 0xff, b3 = v >> 24;
        const std::uint32_t argb = (b3 << 24) | (b0 << 16) | (b1 << 8) | b2;
        for (int k = 0; k < 4; ++k) out.argb[static_cast<std::size_t>(i) * 4 + k] = argb;
    }
    return true;
}

}  // namespace Rfx
}  // namespace mashed_re
