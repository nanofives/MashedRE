// Mashed RE — hook-side twin of FUN_0043c5b0 (per-frame menu draw routine),
// operating on MASHED's live record table 0x00898ac0 and calling the
// ORIGINAL chrome/draw callees at their RVAs. Diff subject for the nav-driven
// draw-sequence A/B (re/frida/menu_drawloop_diff.py). Transcribed from
// re/analysis/standalone_menu_sm/FUN_0043c5b0_full.asm + Part 1 of
// FUN_0043c5b0_port_spec.md; ambiguous arg orders are resolved iteratively
// against the live diff (logo-twin precedent: 6 iterations to bit-identity).
#include <cstdint>

#include "../Core/HookSystem.h"

namespace {

template <typename T>
inline T& At(std::uintptr_t a) { return *reinterpret_cast<T*>(a); }

// ---- originals --------------------------------------------------------------
using V_I    = void(__cdecl*)(int);
using I_V    = int(__cdecl*)();
using V_V    = void(__cdecl*)();
using Draw6  = void(__cdecl*)(int, float, float, std::uint32_t, float, int);
using Text7  = void(__cdecl*)(int, float, float, std::uint32_t, float, int, int);
using Quad5  = void(__cdecl*)(float, float, float, float, std::uint32_t);
using Tri7   = void(__cdecl*)(float, float, float, float, float, float,
                              std::uint32_t);
using Name1  = int(__cdecl*)(const char*);
using Sprite12 = void(__cdecl*)(int, float, float, float, float,
                                std::uint32_t, float, float, float, float,
                                int, int);
using Round0 = int(__cdecl*)();          // 0x004a2c48 reads ST0; see Trunc48
using V_PI   = void(__cdecl*)(void*);

auto* const oSetAux       = reinterpret_cast<V_I>(0x0042aae0);
auto* const oChromeShellB = reinterpret_cast<V_I>(0x0042e5b0);
auto* const oChromeShellA = reinterpret_cast<V_V>(0x0042e3a0);
auto* const oScreenGate   = reinterpret_cast<I_V>(0x0042b930);
auto* const oSpriteText   = reinterpret_cast<Draw6>(0x00427e00);
auto* const oItemText7    = reinterpret_cast<Text7>(0x00428140);

auto* const oFillQuad     = reinterpret_cast<Quad5>(0x00472c60);
auto* const oGradQuad     = reinterpret_cast<Quad5>(0x00473540);
auto* const oTriangle     = reinterpret_cast<Tri7>(0x00472dc0);
auto* const oSpriteByName = reinterpret_cast<Name1>(0x0040bb50);
// FUN_0042aad0 takes its pointer in EAX (lea eax,[esp+0x10]; call) — it
// greys the 4-byte color scratch in place for unavailable rows.
static void CursorAuxEax(void* p) {
    __asm { mov eax, p
            mov ecx, 0x0042aad0
            call ecx }
}
auto* const oSubMenus     = reinterpret_cast<V_V>(0x0043bf30);
using Sub12 = void(__cdecl*)(int, float, float, float, float, std::uint32_t,
                             float, float, float, float, int, int);
auto* const oSpriteSubmit = reinterpret_cast<Sub12>(0x004739f0);

inline void vt20(int a, int b) {   // (*(DAT_007d3ff8+0x20))(a,b)
    auto fn = *reinterpret_cast<void(__cdecl**)(int, int)>(
        At<std::uintptr_t>(0x007d3ff8) + 0x20);
    fn(a, b);
}

struct Rec {                 // stride 0x34; esi parks at +0x2c
    std::int32_t  tag;       // +0x00
    std::int32_t  type;      // +0x04
    std::int32_t  f8;        // +0x08
    std::int32_t  color;     // +0x0c (ARGB)
    std::int32_t  slide;     // +0x10
    float         scale;     // +0x14
    float         x;         // +0x18
    float         y;         // +0x1c
    std::int32_t  row20;     // +0x20
    std::int32_t  flag24;    // +0x24
    std::int32_t  sec_id;    // +0x28
    std::int32_t  prim_id;   // +0x2c
    std::int32_t  screen;    // +0x30
};
static_assert(sizeof(Rec) == 0x34, "stride");

constexpr std::uintptr_t kRecBase = 0x00898ac0;
constexpr std::uintptr_t kPhase   = 0x0067eca4;
constexpr std::uintptr_t kDepth   = 0x0067e9f8;
constexpr std::uintptr_t kScrPtr  = 0x0067ed38;   // [+depth*0x40]
constexpr std::uintptr_t kSelBase = 0x0067ed80;   // sel + avail rows
constexpr std::uintptr_t kAnimGate= 0x0067e914;
constexpr std::uintptr_t kLastDir = 0x0067e844;
constexpr std::uintptr_t kDimAll  = 0x008990e4;
static const char* const kButton = reinterpret_cast<const char*>(0x005cda7c);

// per-record draw for the static tag family (LAB_0043d185): shadow pass
// colored from the persistent scratch, main pass from the record color;
// 7-arg form (last = slide). Primary pair for type 0/2; secondary pair
// (sec_id) when type is neither.
inline void StaticDraw(const Rec& r, std::uint32_t scratch) {
    if (r.type == 0 || r.type == 2) {
        if (r.prim_id == -1) return;
        oItemText7(r.prim_id, r.x + 3.0f, r.y + 3.0f, scratch, r.scale,
                   r.flag24, r.slide);
        oItemText7(r.prim_id, r.x, r.y, static_cast<std::uint32_t>(r.color),
                   r.scale, r.flag24, r.slide);
    } else {
        if (r.sec_id == -1) return;
        oItemText7(r.sec_id, r.x + 3.0f, r.y + 3.0f, scratch, r.scale,
                   r.flag24, r.slide);
        oItemText7(r.sec_id, r.x, r.y, static_cast<std::uint32_t>(r.color),
                   r.scale, r.flag24, r.slide);
    }
}

}  // namespace

// 0x0043c5b0
extern "C" __declspec(dllexport) void __cdecl MenuDrawLoopTwin(void) {
    const int phase = At<std::int32_t>(kPhase);
    if (phase == 0) return;
    if (phase == 1) {                       // 0x0043c61c..0x0043c647
        oSetAux(1);
        vt20(6, 1);
        vt20(8, 1);
        return;
    }
    vt20(6, 0);                              // 0x0043c647 main path
    vt20(8, 1);
    if (phase < 5) {                         // 0x0043c66a
        oChromeShellB(0);
        oChromeShellA();
        if (oScreenGate() != 0x21) {         // chrome decal pair (0x0043c690)
            oSpriteText(0x41, 600.0f, 52.0f, 0xff000000u, 0.8f, 2);
            oSpriteText(0x41, 596.0f, 48.0f, 0xffffffffu, 0.8f, 2);
        }
    }
    oSetAux(1);                              // 0x0043c6f9
    if (phase < 4) {                         // 0x0043c70a record loop
        Rec* recs = reinterpret_cast<Rec*>(kRecBase);
        int row = 0;                         // [esp+0x1c] visible-row counter
        // [esp+0x10] color scratch — FUNCTION scope, persists across rows
        // (verified: 0x42aad0 inputs ffffffff then ff000000 in the live A run)
        std::uint32_t col_scratch = 0xffffffffu;
        std::uint8_t* csb = reinterpret_cast<std::uint8_t*>(&col_scratch);
        for (int i = 0; i < 31; ++i) {       // esi 0x898aec..<0x899104
            const Rec& r = recs[i];
            const std::uint32_t tag = static_cast<std::uint32_t>(r.tag);
            const bool stat =
                (tag == 0xff110000u || tag == 0xff000000u ||
                 tag == 0xff100000u || tag == 0xff120000u ||
                 tag == 0xff130000u || tag == 0xff230000u);
            if (tag == 0xff040000u) {        // 0x0043c766 slide/highlight
                const int depth = At<std::int32_t>(kDepth);
                const std::int32_t scr =
                    At<std::int32_t>(kScrPtr + static_cast<unsigned>(depth) * 0x40);
                ++row;
                csb[3] = 0xff;               // [esp+0x13] = 0xff (0x0043c79c)
                float plate_y = r.y - 13.0f + 1.0f;   // 0x005cd8d8 + 0x005cc320
                float text_y_base = r.y - 13.0f + 1.0f;
                float hl_x = 60.0f;                    // 0x42700000
                // list metrics (screen-ptr identity, 0x0043c78b..0x0043c7fb)
                float w_fill, grad_w, grad_x;
                const bool listA =
                    (scr == 0x005f72a0 || scr == 0x005f7370 ||
                     scr == 0x005f6da0 ||
                     (scr == 0x005f6df8 && At<std::int32_t>(kLastDir) != 0));
                if (listA) {
                    w_fill = 166.0f;   // 0x43260000 plate width
                    grad_w = 84.0f;    // 0x42a80000 right-fade width
                    grad_x = 226.0f;   // 0x43620000 right-fade x (60+166)
                } else {
                    w_fill = 210.0f;   // 0x43520000
                    grad_w = 100.0f;   // 0x42c80000
                    grad_x = 270.0f;   // 0x43870000 (60+210)
                }
                float h_quad = 26.0f, h_quad2 = 26.0f;   // 0x41d00000
                if (r.slide < 0x1ff) {                    // 0x0043c804
                    const int q = r.slide / 10;
                    if (q < 13) {
                        plate_y = r.y - static_cast<float>(q);
                        text_y_base = plate_y;
                        h_quad = static_cast<float>(q) * 2.0f;
                        h_quad2 = h_quad;
                    }
                }
                // selection-bias d (0x0043c84b..0x0043c86c)
                int d;
                if (At<std::int32_t>(kAnimGate) != 0) d = 1;
                else d = (At<std::int32_t>(kLastDir) != 0) ? 0 : 2;
                const bool primary = (r.type == 0 || r.type == 2);
                const int id = primary ? r.prim_id : r.sec_id;
                if (id == -1) continue;                   // 0x0043c885
                const int slot = depth - d;
                const std::int32_t avail = At<std::int32_t>(
                    kSelBase + (static_cast<unsigned>(slot) * 0x10 + row) * 4);
                if (avail == 0) {
                    CursorAuxEax(&col_scratch);           // 0x0042aad0 (EAX)
                }
                const std::int32_t sel = At<std::int32_t>(
                    kSelBase + static_cast<unsigned>(slot) * 0x40);
                const bool selected = (row - 1 == sel);
                const bool skip224 = selected && id == 0x224;
                if (!skip224) {
                    // plate fill + right-fade gradient (idle vs selected)
                    if (selected) {
                        csb[0] = 0xf0; csb[1] = 0x6e; csb[2] = 0x14;
                        csb[3] = 0xa0;            // 0x0043ca1f scratch
                        oFillQuad(60.0f, plate_y, w_fill, h_quad2,
                                  col_scratch);
                        oGradQuad(grad_x, plate_y, grad_w, h_quad2,
                                  col_scratch);
                        csb[0] = 0xb4; csb[1] = 0x50; csb[2] = 0x10;
                        csb[3] = 0xff;            // 0x0043ca68..79
                    } else {
                        oFillQuad(60.0f, plate_y, w_fill, h_quad2,
                                  0x40f8d0e8u);
                        oGradQuad(grad_x, plate_y, grad_w, h_quad2,
                                  0x40f8d0e8u);
                        csb[0] = 0x13; csb[1] = 0x15; csb[2] = 0x50;
                        // alpha persists (0x42aad0's grey shows through)
                    }
                    // COMMON border kit (both paths; 0x0043c91f / 0x0043ca87):
                    // top fill+grad, bottom fill+grad, narrow left at 58.
                    if (h_quad2 > 2.0f) {         // fcomp h vs 0x005cc574
                        oFillQuad(60.0f, plate_y, w_fill, 2.0f, col_scratch);
                        oGradQuad(grad_x, plate_y, grad_w, 2.0f, col_scratch);
                        oFillQuad(60.0f, plate_y + h_quad2 - 2.0f, w_fill,
                                  2.0f, col_scratch);
                        oGradQuad(grad_x, plate_y + h_quad2 - 2.0f, grad_w,
                                  2.0f, col_scratch);
                        oFillQuad(58.0f, plate_y, 2.0f, h_quad2, col_scratch);
                    }
                    if (selected) {
                        // "Button" cap (0x0043cb6a): w=13 left of the narrow
                        // edge (58 - 13 = 45)
                        const int desc = oSpriteByName(kButton);
                        oSpriteSubmit(desc, 58.0f - 13.0f, plate_y, 13.0f,
                                      h_quad2, col_scratch, 0.0f, 1.0f, 0.0f,
                                      1.0f, 1, 0);
                    }
                }
                // text (0x0043cbd4..): scratch RGB zeroed UNCONDITIONALLY,
                // then ONE 7-arg FUN_00428140 call (shadow internal):
                csb[0] = 0; csb[1] = 0; csb[2] = 0;
                if (At<std::int32_t>(kDimAll) != 0) {
                    oItemText7(id, r.x, r.y, 0x80000000u, r.scale, r.flag24,
                               0x80);
                } else if (id == 0x224) {
                    oItemText7(0x224, 320.0f, r.y, col_scratch, 0.7f, 2,
                               r.slide);
                } else {
                    oItemText7(id, r.x, r.y, col_scratch, r.scale, r.flag24,
                               r.slide);
                }
                (void)text_y_base; (void)hl_x; (void)h_quad;
            } else if (stat) {               // LAB_0043d185
                StaticDraw(r, col_scratch);
            }
            // else: skip record (0x0043d25d)
        }
        oSubMenus();                          // FUN_0043bf30
    }
    vt20(6, 1);                               // tail
    vt20(8, 1);
}

RH_ScopedInstall(MenuDrawLoopTwin, 0x0043c5b0);
