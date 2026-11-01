// Mashed RE — D5-ICONCUBE-GLOW: the pickup glow batch (exe-only TU). See PickupGlow.h.
//
// The verbatim FUN_00459000 (Gameplay/PickupTick_naked.inc) calls, cdecl, caller cleans:
//   state 2 (respawning, timer > 0 and resp - timer < 1.0):      state 1 (live):
//     0x004590ba FUN_004769f0(0x005f9838 + type*4)  colour          0x004591f8
//     0x004590c3 FUN_004769a0(entry + 0x38)          position        0x004591fe (frame + 0x40)
//     0x004590cd FUN_004769d0(&size)                 size            0x00459208
//     0x004590d3 FUN_00476a30(0)                     rotation        0x0045921c (entry+0x04 * [0x005cd7a8])
//     0x004590dd FUN_00476d00(&DAT_0068b968)         submit          0x00459226
//   and 0x0045927f FUN_00476df0(&DAT_0068b968) once after the loop (flush).
// Each PK_Glow* shim below is that call on the exe's Rfx core (Render/RfxEmitter.h), which carries the
// original's staging globals and batch layout; with the knob off each returns at once, as PK_Nop did.
//
// The colour argument is an address in the ORIGINAL's .data. The exe's own image covers that range
// (exe_main.cpp, the B7 wedge note), so the pointer is not dereferenced: its index (ptr - 0x005f9838) / 4
// selects kGlowCol, the table transcribed from original/MASHED.exe.unpatched. The table has no writer
// (datarefs: 2 references, both the LEAs at 0x00459090 / 0x004591f0).
#include "PickupGlow.h"
#include "PickupTick.h"
#include "../Render/RfxEmitter.h"

#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>

namespace {

using mashed_re::Rfx::Emitter;

// DAT_005f9838, 22 dwords (index = pool entry +0x24 type), plus index 22 = the dword after the table
// (0x005f9890, image value 0, no reference). RGBA bytes in address order (verify/rfx_emitters_20261028
// section 2: type 19 = 0xffff0000 draws blue).
const std::uint32_t kGlowCol[23] = {
    0xffffffffu, 0xffffffffu, 0xffffffffu, 0xffffffffu, 0xffffffffu, 0xffffffffu, 0xffffffffu, 0xfff1c12du,
    0xffffffffu, 0xff00e0ffu, 0xff0d8a51u, 0xffac78f1u, 0xff241cecu, 0xffffffffu, 0xffffffffu, 0xffffffffu,
    0xff1d94f7u, 0xffab3578u, 0xff84ff84u, 0xffff0000u, 0xffffffffu, 0xffffffffu, 0x00000000u};
constexpr std::uintptr_t kColTable = 0x005f9838u;
constexpr std::uintptr_t kBatch = 0x0068b968u;     // DAT_0068b968, the original's batch address (a tag here)
constexpr std::uintptr_t kPool = 0x0068b198u, kCount = 0x0068b9a8u, kStride = 0x50u;
constexpr int kMax = 25;

Emitter s_glow;
bool     s_created = false;
long     s_badCol = 0, s_badBatch = 0;

bool On() {
    static const bool on = [] {
        const char* e = std::getenv("MASHED_RFX_EMITTERS");
        const char* n = std::getenv("MASHED_NO_PICKUP_GLOW");
        return e && e[0] == '1' && !(n && n[0] == '1');
    }();
    return on;
}

// FUN_004587a0's FUN_004770c0(&DAT_0068b968, 0x817, 0x19, puglow) (0x004587c5..0x004587d2). The texture is
// bound by the draw (RwRaceFx.cpp DrawPickupGlow), on the render side.
Emitter& Glow() {
    if (!s_created) {
        s_created = true;
        mashed_re::Rfx::Create(s_glow, 0x817u, 0x19, nullptr);
    }
    return s_glow;
}

bool BatchOk(const void* b) {
    if (reinterpret_cast<std::uintptr_t>(b) == kBatch) return true;
    if (s_badBatch++ == 0) std::fprintf(stderr, "[puglow] batch argument %p is not 0x0068b968\n", b);
    return false;
}

std::FILE* s_dbg = nullptr;
bool       s_dbgInit = false;
long       s_dbgTick = 0;

std::FILE* Dbg() {
    if (!s_dbgInit) {
        s_dbgInit = true;
        if (const char* p = std::getenv("MASHED_DBG_PUGLOW")) {
            s_dbg = std::fopen(p, "w");
            if (s_dbg)
                std::fprintf(s_dbg, "# PickupGlow.cpp: PRE/POST <tick> <i> state type a04 r18 t1c ax ay az px py pz fx fy fz "
                                    "(hex dwords; a=+0x2c anchor, p=+0x38, f=frame+0x40); P <tick> <k> x y z sx sy col rot\n");
        }
    }
    return s_dbg;
}

void DumpPool(const char* tag) {
    std::FILE* f = Dbg();
    if (!f) return;
    int n = *reinterpret_cast<const std::int32_t*>(kCount);
    n = n < 0 ? 0 : (n > kMax ? kMax : n);
    for (int i = 0; i < n; ++i) {
        const unsigned char* e = reinterpret_cast<const unsigned char*>(kPool + i * kStride);
        auto d = [&](std::uintptr_t o) { std::uint32_t v; std::memcpy(&v, e + o, 4); return v; };
        const unsigned char* obj = *reinterpret_cast<unsigned char* const*>(e);
        const unsigned char* fr = obj ? *reinterpret_cast<unsigned char* const*>(obj + 4) : nullptr;
        std::uint32_t fp[3] = {};
        if (fr) std::memcpy(fp, fr + 0x40, 12);
        std::fprintf(f, "%s\t%ld\t%d\t%d\t%d\t%08x\t%08x\t%08x\t%08x\t%08x\t%08x\t%08x\t%08x\t%08x\t%08x\t%08x\t%08x\n",
                     tag, s_dbgTick, i, static_cast<int>(d(0x20)), static_cast<int>(d(0x24)), d(0x04), d(0x18), d(0x1c),
                     d(0x2c), d(0x30), d(0x34), d(0x38), d(0x3c), d(0x40), fp[0], fp[1], fp[2]);
    }
}

}  // namespace

extern "C" {

void __cdecl PK_GlowColour(const std::uint32_t* p) {    // FUN_004769f0
    if (!On()) return;
    const std::uintptr_t a = reinterpret_cast<std::uintptr_t>(p);
    const std::uintptr_t k = (a - kColTable) / 4u;
    if (a < kColTable || ((a - kColTable) & 3u) != 0 || k >= sizeof(kGlowCol) / sizeof(kGlowCol[0])) {
        if (s_badCol++ == 0) std::fprintf(stderr, "[puglow] colour pointer %p outside the transcribed table\n", p);
        mashed_re::Rfx::SetColour(nullptr);
        return;
    }
    mashed_re::Rfx::SetColour(&kGlowCol[k]);
}
void __cdecl PK_GlowPos(const float* p) {               // FUN_004769a0
    if (On()) mashed_re::Rfx::SetPos(p);
}
void __cdecl PK_GlowSize(const float* p) {              // FUN_004769d0
    if (On()) mashed_re::Rfx::SetSize(p);
}
void __cdecl PK_GlowRotate(std::uint32_t bits) {        // FUN_00476a30: the float argument, as its bits
    if (On()) mashed_re::Rfx::SetRotateBits(bits);
}
void __cdecl PK_GlowSubmit(void* batch) {               // FUN_00476d00
    if (On() && BatchOk(batch)) mashed_re::Rfx::Submit(Glow());
}
void __cdecl PK_GlowFlush(void* batch) {                // FUN_00476df0
    if (On() && BatchOk(batch)) mashed_re::Rfx::Flush(Glow());
}

}  // extern "C"

namespace mashed_re {
namespace Gameplay {

bool PickupGlow_Enabled() { return On(); }

void* PickupGlow_Emitter() {
    if (!On() || !s_created) return nullptr;
    return &s_glow;
}

void PickupGlow_DbgPre() {
    if (!On() || !Dbg()) return;
    ++s_dbgTick;
    DumpPool("PRE");
}

void PickupGlow_DbgPost() {
    if (!On() || !Dbg()) return;
    DumpPool("POST");
    const Rfx::PTank& t = Glow().tank;
    for (int k = 0; k < t.count; ++k) {
        const std::uint32_t* P = Rfx::TankElem(t, t.pos, k);
        const std::uint32_t* S = Rfx::TankElem(t, t.size, k);
        const std::uint32_t* C = Rfx::TankElem(t, t.col, k);
        const std::uint32_t* R = Rfx::TankElem(t, t.rot, k);
        if (!P || !S || !C || !R) break;
        std::fprintf(s_dbg, "P\t%ld\t%d\t%08x\t%08x\t%08x\t%08x\t%08x\t%08x\t%08x\n", s_dbgTick, k,
                     P[0], P[1], P[2], S[0], S[1], C[0], R[0]);
    }
    std::fprintf(s_dbg, "N\t%ld\t%d\n", s_dbgTick, t.count);
    if ((s_dbgTick % 600) == 0) std::fflush(s_dbg);
}

}  // namespace Gameplay
}  // namespace mashed_re
