// Mashed RE — FUN_004325c0 (menu slide-animation tick), ONE body for both targets (D4-FU-MENUSPEED, 2026-10-27,
// decision L-35 = 3). This TU is in both rsp lists. MenuAnimTickBody is the logic; the .asi export
// MenuAnimTickTwin (the diff-original subject, operating on MASHED's LIVE record table at 0x00898ac0, stride
// 0x34) and the exe's Anim_Tick (Frontend/MenuNavSM.cpp, on g_records) only supply inputs (MenuAnimTick.h).
//
// Citations from the disassembly of the unpatched MASHED.exe (capstone, 2026-10-27,
// verify/menuspeed_20261027/PREREG_D4-FU-MENUSPEED.md section 1). ESI = record + 0x10, so [esi-0x10] = +0x00 tag,
// [esi-0xc] = +0x04 type, [esi-8] = +0x08 float, [esi] = +0x10 slide, [esi+0xc] = +0x1c Y, [esi+0x10] = +0x20 row,
// [esi+0x20] = +0x30 screen pointer.
//
// Before this step the .asi copy wrote the settle Y to +0x0c and called FUN_0042ac50 as cdecl. The original stores
// +0x1c (0x004326f2) and passes the count in EAX, the spacing in ECX. menu_anim_diff.py never reached that path
// (it reloads every slide to 0x1ff and runs one tick), so the 2026-06-10 / 2026-10-26 GREENs did not see it.
//
// Promotion lane: nav-driven A/B (re/frida/menu_anim_diff.py) — the table is
// repopulated mid-animation via the original FUN_0043d2a0, then original vs
// twin are run on identical snapshots ON THE GAME THREAD and the table bytes
// compared. (Settled-state diffs are degenerate; see the menu-attach ceiling
// note in project memory.)
#include <cstdint>
#include <cmath>
#include <cstdlib>
#include <cstring>

#include "../Core/HookSystem.h"
#include "MenuAnimTick.h"

namespace mashed_re {
namespace Frontend {

namespace {

// FUN_004a2c48 (0x004a2c48): FISTP round-to-nearest, then the sign-aware
// carry adjust (0x004a2c6b..0x004a2ca5) = truncation toward zero (_ftol2
// shape; decomp D4-WSH-EXEFILE 2026-10-26). Truncation is the default
// (decision L-32 ON, Mariano 2026-10-26); MASHED_NO_ANIM_TRUNC=1 restores the
// .asi copy's old rounding (std::nearbyint) on both targets.
bool AnimTruncOn() {
    static int v = -1;
    if (v < 0) {
        char buf[8];
        const DWORD n = GetEnvironmentVariableA("MASHED_NO_ANIM_TRUNC", buf, sizeof buf);
        v = (n > 0 && n < sizeof buf && buf[0] && buf[0] != '0') ? 0 : 1;
    }
    return v != 0;
}
inline int Trunc48(float v) {
    if (AnimTruncOn()) return static_cast<int>(v);
    return static_cast<int>(std::nearbyint(v));
}

// Float view of record +0x08 (the float path's accumulator; item rows use the cell as the int row_index).
inline float F8Get(const MenuRecord& r) { float f; std::memcpy(&f, &r.row_index, 4); return f; }
inline void  F8Set(MenuRecord& r, float f) { std::memcpy(&r.row_index, &f, 4); }

}  // namespace

// 0x004325c0
int MenuAnimTickBody(MenuRecord* recs, const MenuAnimTickIn& in) {
    // 0x004325c8 cmp [0x0067eca4],4 / jl: 2.0f (0x005cc574) else 1.0f (0x005cc320), held in ST1 for the walk.
    const float base = (in.phase >= 4) ? 2.0f : 1.0f;
    int all_settled = 1;                                              // 0x004325e5 [esp+0x10] = 1
    for (int i = 0; i < 30; ++i) {                                    // esi 0x898ad0 .. 0x8990e8 (0x004327da)
        MenuRecord& r = recs[i];
        const std::uint32_t tag = static_cast<std::uint32_t>(r.tag);
        if (tag != 0 && r.type != 0x1000 && in.gate != 0)
            all_settled = 0;                                          // 0x004325f5..0x0043260d
        const bool slide_class =                                      // LAB_0043277c (0x00432615..0x0043274f)
            (tag == 0xff110000u || tag == 0xff000000u || tag == 0xff120000u ||
             tag == 0xff130000u || tag == 0xff230000u || tag == 0xff100000u);
        if (tag == 0xff040000u) {
            const int type = r.type;                                  // 0x00432642 edi
            if (type == 0 || type == 2) {
                r.slide += Trunc48(in.dt * base * -2000.0f);          // 0x00432682 (0x005cd904)
                if (r.slide <= 0) {                                   // 0x0043269f jg
                    if (type == 2) { r.slide = 0x1ff; r.type = 0x1000; }   // 0x004326aa
                    else {
                        const int row = r.pad20;                      // 0x004326b8 [esi+0x10]
                        r.slide = 0;                                  // 0x004326c2
                        r.type  = 1;                                  // 0x004326c8
                        const int b30 = in.list_base_y(in.item_total, 0x1e);   // 0x004326cf
                        const std::int32_t scr =                      // 0x004326d4 [esi+0x20]
                            in.screen_per_record ? r.pad30 : in.screen;
                        r.y = static_cast<float>(b30 + row * 0x1e);   // 0x004326db..0x004326f2 fstp [esi+0xc]
                        if (scr == in.screen_24) {                    // 0x004326e4 cmp ebx,0x5f7370
                            const int b28 = in.list_base_y(in.item_total, 0x1c);   // 0x004326fe
                            r.y = static_cast<float>(b28 + row * 0x1c) - 58.0f;    // 0x00432716 (_DAT_005cd900)
                        }
                        if (scr == in.screen_18) {                    // 0x0043271f cmp ebx,0x5f72a0
                            r.y -= 58.0f;                             // 0x0043272b..0x00432734
                        }
                    }
                }
            } else {                                                  // 0x0043264e (0x005cc9fc)
                r.slide += Trunc48(in.dt * base * 1000.0f);
                if (r.slide >= 0x190) { r.slide = 0x1ff; r.type = 0x1000; }   // 0x00432667..0x0043267a
            }
        } else if (slide_class) {
            const int type = r.type;                                  // 0x0043277c edi
            if (type == 0 || type == 2) {                             // 0x0043279b (0x005cd8f8)
                r.slide += Trunc48(in.dt * base * -800.0f);
                if (r.slide <= 0) {                                   // 0x004327b8 jg
                    if (type == 2) { r.slide = 0x1ff; r.type = 0x1000; }   // 0x004327bf
                    else           { r.slide = 0;     r.type = 1; }        // 0x004327ca
                }
            } else {                                                  // 0x00432788 (0x005ccd04), joins 0x0043265c
                r.slide += Trunc48(in.dt * base * 500.0f);
                if (r.slide >= 0x190) { r.slide = 0x1ff; r.type = 0x1000; }
            }
        } else {                                                      // 0x00432751 float path, no base
            const float f = in.dt * 300.0f + F8Get(r);                // 0x005cd8fc, fadd [esi-8]
            F8Set(r, f);                                              // 0x00432760 fst
            if (f >= 180.0f) { F8Set(r, 180.0f); r.type = 0x1000; }   // 0x005cd09c; 0x00432770 raw 0x43340000
        }
    }
    return all_settled;                                               // 0x004327e6
}

}  // namespace Frontend
}  // namespace mashed_re

#ifndef MASHED_STANDALONE
namespace {

using mashed_re::Frontend::MenuAnimTickIn;
using mashed_re::Frontend::MenuRecord;

constexpr std::uintptr_t kRecBase   = 0x00898ac0;  // record array (31 x 0x34)
constexpr std::uintptr_t kDt        = 0x007f1004;  // DAT_007f1004 frame dt
constexpr std::uintptr_t kGate      = 0x0067e914;  // DAT_0067e914 anim gate
constexpr std::uintptr_t kPhase     = 0x0067eca4;  // DAT_0067eca4 frontend phase
constexpr std::uintptr_t kItemTotal = 0x0067ece0;  // DAT_0067ece0 item total
constexpr std::uintptr_t kScrA      = 0x005f72a0;  // &DAT_005f72a0 (screen 18)
constexpr std::uintptr_t kScrB      = 0x005f7370;  // &DAT_005f7370 (screen 24)

template <typename T>
inline T& At(std::uintptr_t a) { return *reinterpret_cast<T*>(a); }

// Call helper (not a port): FUN_0042ac50 takes the count in EAX and the spacing in ECX, returns EAX
// (0x0042ac51 mov ebx,eax; 0x0042ac6b imul ebx,ecx). No C convention fits. Integer-only callee.
__declspec(noinline) int OrigListBaseY(int count, int spacing) {
    int r;
    __asm {
        mov  eax, count
        mov  ecx, spacing
        mov  edx, 0x0042ac50
        call edx
        mov  r, eax
    }
    return r;
}

}  // namespace

// 0x004325c0 — returns 1 when all records settled.
extern "C" __declspec(dllexport) int __cdecl MenuAnimTickTwin(void) {
    MenuAnimTickIn in;
    in.dt                = At<float>(kDt);
    in.phase             = At<std::int32_t>(kPhase);
    in.gate              = At<std::int32_t>(kGate);
    in.item_total        = At<std::int32_t>(kItemTotal);
    in.list_base_y       = &OrigListBaseY;
    in.screen_per_record = true;
    in.screen            = 0;
    in.screen_24         = static_cast<std::int32_t>(kScrB);
    in.screen_18         = static_cast<std::int32_t>(kScrA);
    return mashed_re::Frontend::MenuAnimTickBody(reinterpret_cast<MenuRecord*>(kRecBase), in);
}

// Runtime-toggleable install (C3 gate). The nav-driven diff
// (re/frida/menu_anim_diff.py) replaces the RVA itself, independent of this.
RH_ScopedInstall(MenuAnimTickTwin, 0x004325c0);
#endif  // !MASHED_STANDALONE
