// Mashed RE — hook-side twin of FUN_004325c0 (menu slide-animation tick),
// operating on MASHED's LIVE record table at 0x00898ac0 (stride 0x34) and
// MASHED's globals — the diff-original subject for the standalone port in
// MenuNavSM.cpp (Anim_Tick). Every offset/RVA below mirrors the verbatim
// port's disasm citations (MenuNavSM.cpp 626..702, pool0 2026-06-09).
//
// Promotion lane: nav-driven A/B (re/frida/menu_anim_diff.py) — the table is
// repopulated mid-animation via the original FUN_0043d2a0, then original vs
// twin are run on identical snapshots ON THE GAME THREAD and the table bytes
// compared. (Settled-state diffs are degenerate; see the menu-attach ceiling
// note in project memory.)
#include <cstdint>
#include <cmath>

#include "../Core/HookSystem.h"

namespace {

constexpr std::uintptr_t kRecBase   = 0x00898ac0;  // record array (31 x 0x34)
constexpr std::uintptr_t kDt        = 0x007f1004;  // DAT_007f1004 frame dt
constexpr std::uintptr_t kGate      = 0x0067e914;  // DAT_0067e914 anim gate
constexpr std::uintptr_t kPhase     = 0x0067eca4;  // DAT_0067eca4 frontend phase
constexpr std::uintptr_t kItemTotal = 0x0067ece0;  // DAT_0067ece0 item total
constexpr std::uintptr_t kScrA      = 0x005f72a0;  // &DAT_005f72a0 (screen 18)
constexpr std::uintptr_t kScrB      = 0x005f7370;  // &DAT_005f7370 (screen 24)

template <typename T>
inline T& At(std::uintptr_t a) { return *reinterpret_cast<T*>(a); }

// FUN_004a2c48 semantic: ROUND(st0) -> int (x87 default round-to-nearest-even;
// verified C2/C4 notes). Called here as plain math (the original passes the
// product in ST0).
inline int Trunc48(float v) { return static_cast<int>(std::nearbyint(v)); }

// FUN_0042ac50 (Y-centering, C2 pure): call the original (cdecl, 2 ints).
using QListBaseYFn = int(__cdecl*)(int, int);
const QListBaseYFn s_QListBaseY = reinterpret_cast<QListBaseYFn>(0x0042ac50);

struct Rec {                 // the 0x34-byte record (Part-1 field map)
    std::int32_t  tag;       // +0x00
    std::int32_t  type;      // +0x04
    float         f8;        // +0x08
    float         y;         // +0x0c
    std::int32_t  slide;     // +0x10
    float         scale;     // +0x14
    std::int32_t  x;         // +0x18
    std::int32_t  pad1c;     // +0x1c
    std::int32_t  row20;     // +0x20
    std::int32_t  pad24;     // +0x24
    std::int32_t  sec_id;    // +0x28
    std::int32_t  prim_id;   // +0x2c
    std::int32_t  screen;    // +0x30
};
static_assert(sizeof(Rec) == 0x34, "record stride");

}  // namespace

// 0x004325c0 — returns 1 when all records settled.
extern "C" __declspec(dllexport) int __cdecl MenuAnimTickTwin(void) {
    const float dt = At<float>(kDt);
    const float base = (At<std::int32_t>(kPhase) >= 4) ? 2.0f : 1.0f; // 0x004325c8
    int all_settled = 1;                                              // [ESP+0x10]
    Rec* recs = reinterpret_cast<Rec*>(kRecBase);
    for (int i = 0; i < 30; ++i) {            // ESI 0x898ad0..0x8990e8
        Rec& r = recs[i];
        const std::uint32_t tag = static_cast<std::uint32_t>(r.tag);
        if (tag != 0 && r.type != 0x1000 && At<std::int32_t>(kGate) != 0)
            all_settled = 0;                                          // 0x004325f5
        const bool slide_class =
            (tag == 0xff110000u || tag == 0xff000000u || tag == 0xff120000u ||
             tag == 0xff130000u || tag == 0xff230000u || tag == 0xff100000u);
        if (tag == 0xff040000u) {
            if (r.type == 0 || r.type == 2) {
                r.slide += Trunc48(dt * base * -2000.0f);             // 0x00432682
                if (r.slide <= 0) {
                    if (r.type == 2) { r.slide = 0x1ff; r.type = 0x1000; }
                    else {                                            // 0x004326b8
                        const int row20 = r.row20;
                        r.slide = 0; r.type = 1;
                        float y = static_cast<float>(
                            s_QListBaseY(At<std::int32_t>(kItemTotal), 0x1e) +
                            row20 * 0x1e);
                        if (r.screen == static_cast<std::int32_t>(kScrB)) {
                            y = static_cast<float>(
                                    s_QListBaseY(At<std::int32_t>(kItemTotal),
                                                 0x1c) + row20 * 0x1c) - 58.0f;
                        }
                        if (r.screen == static_cast<std::int32_t>(kScrA)) {
                            y -= 58.0f;                               // _DAT_005cd900
                        }
                        r.y = y;
                    }
                }
            } else {                                                  // 0x0043264e
                r.slide += Trunc48(dt * base * 1000.0f);
                if (r.slide >= 0x190) { r.slide = 0x1ff; r.type = 0x1000; }
            }
        } else if (slide_class) {                                     // LAB_0043277c
            if (r.type == 0 || r.type == 2) {
                r.slide += Trunc48(dt * base * -800.0f);              // 0x0043279b
                if (r.slide <= 0) {
                    if (r.type == 2) { r.slide = 0x1ff; r.type = 0x1000; }
                    else             { r.slide = 0;     r.type = 1; }
                }
            } else {                                                  // 0x00432788
                r.slide += Trunc48(dt * base * 500.0f);
                if (r.slide >= 0x190) { r.slide = 0x1ff; r.type = 0x1000; }
            }
        } else {                                                      // 0x00432751
            const float f = dt * 300.0f + r.f8;
            r.f8 = f;
            if (f >= 180.0f) { r.f8 = 180.0f; r.type = 0x1000; }
        }
    }
    return all_settled;
}

// Runtime-toggleable install (C3 gate). The nav-driven diff
// (re/frida/menu_anim_diff.py) replaces the RVA itself, independent of this.
RH_ScopedInstall(MenuAnimTickTwin, 0x004325c0);
