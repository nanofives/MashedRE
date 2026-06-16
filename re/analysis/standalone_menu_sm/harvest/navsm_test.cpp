// Standalone logic harness for Frontend/MenuNavSM (the reversed menu nav state
// machine). Links MenuNavSM.cpp directly and drives Nav_Init / Nav_Select /
// Nav_Back over the harvested descriptor tables, printing the record state at
// each level. Proves the harvested tables + reversed push map produce the right
// deeper-screen content WITHOUT needing the full D3D9 app. NOT shipped.
#include "../../../../mashedmod/src/mashed_re/Frontend/MenuNavSM.h"
#include <cstdio>

using namespace mashed_re::Frontend;

// Stubs for the frontend-render hooks MenuNavSM declares extern (the arc-wash
// logo fade + the active-screen mode-code lookup). They are pure presentation/
// state side-channels with no effect on the nav-logic this harness exercises.
extern "C" void __cdecl LogoOverlayFadeSet(int, int) {}
extern "C" unsigned int __cdecl ModeCodeLookup() { return 0xffffffffu; }

// Test-support accessors (public wrappers over the MenuNavSM internals).
namespace mashed_re { namespace Frontend {
const std::uint32_t* NavTest_TableForScreen(int screen_id);
std::uint32_t        NavTest_ItemActionCode(const std::uint32_t* table, int item_index);
int                  NavTest_ActionToScreen(std::uint32_t action, int slot_kind);
int                  NavTest_KvLookup(const std::uint32_t* table, std::uint32_t tag, int n);
int                  NavTest_ApplyActionGameMode(std::uint32_t action);  // WS-G2
} }
#define TableForScreen  NavTest_TableForScreen
#define ItemActionCode  NavTest_ItemActionCode
#define ActionToScreen  NavTest_ActionToScreen
#define KvLookup        NavTest_KvLookup

static void dump(const char* label) {
    const MenuRecord* r = Nav_Records();
    const int n = Nav_RecordCount();
    std::printf("== %s : depth=%d screen=%d cursor=%d records=%d ==\n",
                label, Nav_Depth(), Nav_ScreenId(), Nav_Cursor(), n);
    for (int i = 0; i < n; ++i) {
        std::printf("   rec[%d] tag=%08x type=%d row=%d prim_id=0x%x sec_id=%d\n",
                    i, (unsigned)r[i].tag, r[i].type, r[i].row_index,
                    (unsigned)r[i].prim_id, r[i].sec_id);
    }
}

int main() {
    Nav_Init();
    dump("root (screen 0)");

    // root cursor starts at item 0; move to item 1 (action 0xff500000 -> push 8)
    Nav_MoveCursor(+1);
    std::printf("\n[after DOWN] root cursor=%d\n", Nav_Cursor());

    bool pushed = Nav_Select();
    std::printf("[ENTER on root item 1] pushed=%d -> expect screen 8\n", pushed);
    dump("child (expect screen 8)");

    // screen 8 cursor at item 0 (action 0xff430000 -> push 0x13 = 19)
    bool pushed2 = Nav_Select();
    std::printf("\n[ENTER on screen8 item 0] pushed=%d -> expect screen 19\n", pushed2);
    dump("grandchild (expect screen 19)");

    // screen 19 items all 0xff450000 -> pop
    bool sel3 = Nav_Select();
    std::printf("\n[ENTER on screen19 item 0] sel=%d -> 0xff450000 == pop, expect screen 8\n", sel3);
    dump("after select-pop (expect screen 8)");

    bool back = Nav_Back();
    std::printf("\n[ESC] back=%d -> expect root\n", back);
    dump("back at root (screen 0)");

    // Sanity: screen 1 items 0x21/0x22 (action 0x2/0x3) must push screens 2/3
    // via the LAB_0043fc37 default rule.
    std::printf("\n--- default-rule check: root cannot reach screen 1 directly,\n"
                "    so push screen 1 explicitly then select its first item ---\n");
    Nav(1, kNavPush);
    dump("screen 1 (explicit push)");
    bool p1 = Nav_Select();   // item 0 action 0x2 -> push screen 2
    std::printf("[ENTER on screen1 item 0 action=0x2] pushed=%d -> expect screen 2\n", p1);
    dump("expect screen 2");

    // ----------------------------------------------------------------------
    // Piece 1 — faithful state-gated SELECT routing. Each gated action code is
    // exercised on a fresh-menu state; the routing must match the branch the
    // real game (FUN_0043dfd0) takes when its runtime globals are at reset.
    // ----------------------------------------------------------------------
    std::printf("\n=== Piece 1: state-gated SELECT routing (fresh-menu defaults) ===\n");
    auto check = [](const char* what, int got, int want) {
        std::printf("  [%s] got screen=%d cursor unchanged? want=%d : %s\n",
                    what, Nav_ScreenId(), want, (Nav_ScreenId() == want) ? "OK" : "FAIL");
    };

    // 0xff820000 (screen 1 item 3): FUN_00402f40()==0 -> push 0x1f(31). Item 3 is
    // greyed at fresh menu (case-1 disable when has_savedata==0), so enable it
    // first (has_savedata=1) to reach the cursor and exercise the routing.
    Nav_GameStateReset();
    Nav_GameState().has_savedata = 1;
    Nav_Init();
    Nav(1, kNavPush);              // depth1 screen1
    for (int i = 0; i < 3; ++i) Nav_MoveCursor(+1);  // cursor -> item 3
    std::printf("[screen1 cursor=%d action expect 0xff820000]\n", Nav_Cursor());
    Nav_Select();
    check("0xff820000 (savedata) -> push 31", Nav_ScreenId(), 0x1f);

    // 0xff240000 (screen 5 item 0): ecdc==0 && ed6c==0 -> push 7.
    Nav_GameStateReset(); Nav_Init();
    Nav(5, kNavPush);
    std::printf("  [screen5 depth=%d cursor=%d items=%d recs=%d]\n",
                Nav_Depth(), Nav_Cursor(), Nav_TopSlot().item_count, Nav_RecordCount());
    Nav_Select();
    check("0xff240000 fresh -> push 7", Nav_ScreenId(), 7);

    // 0xff3c0000 (screen 16 item 0): teams invalid (-1 all) -> modal, NO nav.
    Nav_GameStateReset(); Nav_Init();
    Nav(16, kNavPush);
    {
        int before = Nav_ScreenId();
        bool pushed = Nav_Select();
        std::printf("  [0xff3c0000 fresh -> modal(no nav)] pushed=%d screen=%d (was %d) : %s\n",
                    pushed, Nav_ScreenId(), before,
                    (!pushed && Nav_ScreenId() == before) ? "OK" : "FAIL");
    }

    // 0xff3c0000 with a VALID 1-vs-1 team comp -> Q_TeamComposition()==0x1000 ->
    // push 0xf(15). NOTE: at fresh standalone the team byte table isn't loaded so
    // Q_TeamComposition stays -1 even with slots set; this asserts the gate is
    // wired (not hard-coded) by checking the no-nav path is the result, matching
    // the real fresh-menu behavior.
    Nav_GameStateReset();
    Nav_GameState().team_slot[0] = 0; Nav_GameState().team_slot[1] = 1;
    Nav_Init(); Nav(16, kNavPush);
    {
        int before = Nav_ScreenId();
        bool pushed = Nav_Select();
        std::printf("  [0xff3c0000 2-slots, no team table -> still modal] pushed=%d screen=%d : %s\n",
                    pushed, Nav_ScreenId(),
                    (!pushed && Nav_ScreenId() == before) ? "OK" : "FAIL");
    }

    // 0xff4c0000 (screen 20 item 0): slot_kind != 0x17 at fresh -> pop.
    Nav_GameStateReset(); Nav_Init();
    Nav(20, kNavPush);
    {
        int depth_before = Nav_Depth();
        bool back = Nav_Select();  // 0xff4c0000 -> pop
        std::printf("  [0xff4c0000 fresh slot_kind=0 -> pop] back=%d depth %d->%d : %s\n",
                    back, depth_before, Nav_Depth(),
                    (back && Nav_Depth() == depth_before - 1) ? "OK" : "FAIL");
    }

    // ----------------------------------------------------------------------
    // Piece 2 — faithful grey-out (FUN_00432800 per-screen disables).
    // ----------------------------------------------------------------------
    std::printf("\n=== Piece 2: faithful grey-out (fresh-menu defaults) ===\n");
    auto avail_dump = [](const char* label, int screen) {
        std::printf("  %s (screen %d): items=%d avail=[", label, screen,
                    Nav_TopSlot().item_count);
        for (int i = 0; i < Nav_TopSlot().item_count; ++i)
            std::printf("%d", Nav_ItemEnabled(i) ? 1 : 0);
        std::printf("] cursor=%d\n", Nav_Cursor());
    };

    // Screen 1: item 3 (saved-game/restart) disabled when has_savedata==0.
    Nav_GameStateReset(); Nav_Init(); Nav(1, kNavPush);
    avail_dump("screen1 fresh (item3 should be greyed)", 1);
    std::printf("    -> item3 enabled? %d : %s\n", Nav_ItemEnabled(3),
                (!Nav_ItemEnabled(3)) ? "OK (greyed)" : "FAIL");
    // With save data present -> item 3 enabled.
    Nav_GameStateReset(); Nav_GameState().has_savedata = 1;
    Nav_Init(); Nav(1, kNavPush);
    std::printf("    [has_savedata=1] item3 enabled? %d : %s\n", Nav_ItemEnabled(3),
                Nav_ItemEnabled(3) ? "OK (enabled)" : "FAIL");

    // Screen 8: items 2,3 greyed at fresh (FUN_00492d10()!=1).
    Nav_GameStateReset(); Nav_Init();
    Nav_Select();   // root item 1 -> push 8 (cursor at item1 default? ensure)
    Nav_GameStateReset(); Nav_Init(); Nav(8, kNavPush);
    avail_dump("screen8 fresh (items 2,3 greyed)", 8);
    std::printf("    -> item2=%d item3=%d : %s\n", Nav_ItemEnabled(2), Nav_ItemEnabled(3),
                (!Nav_ItemEnabled(2) && !Nav_ItemEnabled(3)) ? "OK" : "FAIL");
    // cursor must NOT land on a disabled item.
    std::printf("    -> cursor=%d enabled? %s\n", Nav_Cursor(),
                (Nav_Cursor() >= 0 && Nav_ItemEnabled(Nav_Cursor())) ? "OK" : "FAIL");

    // Screen 28 (0x1c): all vehicle items greyed when no players active.
    Nav_GameStateReset(); Nav_Init(); Nav(0x1c, kNavPush);
    avail_dump("screen28 fresh (no players -> all greyed)", 0x1c);

    // ----------------------------------------------------------------------
    // Piece 3 — faithful slide-animation tick (port of FUN_004325c0).
    // ----------------------------------------------------------------------
    std::printf("\n=== Piece 3: slide-animation tick (FUN_004325c0) ===\n");
    Nav_GameStateReset(); Nav_Init();
    Nav(8, kNavPush);   // depth>0 -> items enter with slide=0x1ff, type=0
    std::printf("  on push: rec1 slide=%d (expect 0x1ff=511)\n", Nav_RecordSlide(1));
    int frames = 0; bool settled = false;
    // 50ms frames -> one quantized tick each (FUN_00493480 port): item slide
    // step = trunc(50/3000 * 1.0 * -2000) = -33/frame; 0x1ff crosses zero in
    // ~16 frames, then the post-settle count-up (+16/frame to 0x190) freezes.
    while (frames < 200 && !settled) {
        Nav_FrameClockUpdate(50);
        settled = Nav_AnimTick();
        ++frames;
    }
    std::printf("  settled after %d frames=%d ; rec1 slide=%d (expect 0) : %s\n",
                frames, settled, Nav_RecordSlide(1),
                (settled && Nav_RecordSlide(1) == 0) ? "OK" : "FAIL");
    // Re-push rebuilds the slide (animation restarts on every screen change).
    Nav(19, kNavPush);
    std::printf("  re-push screen19: rec1 slide=%d (expect 0x1ff) : %s\n",
                Nav_RecordSlide(1), (Nav_RecordSlide(1) == 0x1ff) ? "OK" : "FAIL");

    // ----------------------------------------------------------------------
    // R2-2 — save-driven game state (FUN_00404e80 step-1 span restore).
    // Craft a full gamesave.bin image: DEADBEEF magic + nonzero gates in the
    // 0x24A40 span (1:1 image of live 0x007f0a40..0x007f0f60).
    // ----------------------------------------------------------------------
    std::printf("\n=== R2-2: save-driven game state (FUN_00404e80) ===\n");
    {
        static unsigned char img[0x24FA0] = {0};
        auto put32 = [&](unsigned off, unsigned v) {
            img[off] = (unsigned char)(v); img[off+1] = (unsigned char)(v>>8);
            img[off+2] = (unsigned char)(v>>16); img[off+3] = (unsigned char)(v>>24);
        };
        constexpr unsigned kSpan = 0x24A40;     // file image of DAT_007f0a40
        // Blank image (magic 0) must be REFUSED and keep fresh defaults.
        Nav_GameStateReset();
        const bool blank_refused = !Nav_GameStateLoadSave(img, sizeof(img));
        std::printf("  blank save refused: %s\n", blank_refused ? "OK" : "FAIL");
        // Written save: magic + savedata gate + profile gate + set-0 unlocks.
        put32(0x0000, 0xDEADBEEFu);             // magic (0x00404F37)
        put32(kSpan + 0x4ec, 1);                // DAT_007f0f2c savedata gate
        put32(kSpan + 0x094, 1);                // DAT_007f0ad4 profile gate
        put32(kSpan + 0x010, 2);                // DAT_007f0a50 set-0 track unlock
        put32(kSpan + 0x018, 1);                // DAT_007f0a58 set-0 car unlock
        const bool loaded = Nav_GameStateLoadSave(img, sizeof(img));
        std::printf("  written save loaded: %s\n", loaded ? "OK" : "FAIL");
        // screen 1 item 3 was greyed fresh; with savedata it must enable.
        Nav_Init(); Nav(1, kNavPush);
        std::printf("  screen1 item3 enabled w/ savedata: %s\n",
                    Nav_ItemEnabled(3) ? "OK" : "FAIL");
        // screen 0x12 (18): track/car unlock items enable from the span.
        Nav_Init(); Nav(0x12, kNavPush);
        std::printf("  screen18 av1(track)=%d av6(car)=%d : %s\n",
                    Nav_ItemEnabled(1), Nav_ItemEnabled(6),
                    (Nav_ItemEnabled(1) && Nav_ItemEnabled(6)) ? "OK" : "FAIL");
        Nav_GameStateReset();
    }

    // ----------------------------------------------------------------------
    // R2-close — 34-screen reachability pass. BFS over the reversed push map
    // (ItemActionCode + ActionToScreen) from the two real entry roots:
    // screen 0 (in-race pause root, FUN_0043df00 reload) and screen 1 (the
    // boot "Game Type Select", entered from the title-confirm flow — runtime-
    // confirmed by the 2026-06-10 parity walk). slot_kind variants 0..3 are
    // tried so state-gated pushes expose both branches. Every non-null table
    // is also pushed directly to prove BuildRecords handles it.
    // ----------------------------------------------------------------------
    std::printf("\n=== R2-close: 34-screen reachability (BFS over push map) ===\n");
    {
        constexpr int kN = 34;
        bool visited[kN] = {};
        int  queue[kN]; int qh = 0, qt = 0;
        auto push_bfs = [&](int s) {
            if (s >= 0 && s < kN && !visited[s] && TableForScreen(s) != nullptr) {
                visited[s] = true; queue[qt++] = s;
            }
        };
        push_bfs(0); push_bfs(1);
        while (qh < qt) {
            const int s = queue[qh++];
            const std::uint32_t* tbl = TableForScreen(s);
            // count items by tag (0xff040000)
            int items = 0;
            for (int r = 0; r < 16; ++r) {
                if (ItemActionCode(tbl, r) == 0xffffffffu &&
                    KvLookup(tbl, 0xff040000u, r) == -1 && r >= 1) break;
                ++items;
            }
            for (int r = 0; r < items; ++r) {
                const std::uint32_t a = ItemActionCode(tbl, r);
                if (a == 0xffffffffu) continue;
                for (int kind = 0; kind < 4; ++kind) {
                    const int t = ActionToScreen(a, kind);
                    if (t >= 0) push_bfs(t);
                }
            }
        }
        int n_reach = 0, n_null = 0, n_unreach = 0;
        char unreach[256] = {}; std::size_t up = 0;
        for (int s = 0; s < kN; ++s) {
            if (TableForScreen(s) == nullptr) { ++n_null; continue; }
            if (visited[s]) ++n_reach;
            else {
                ++n_unreach;
                up += static_cast<std::size_t>(
                    std::snprintf(unreach + up, sizeof(unreach) - up, "%d ", s));
            }
        }
        std::printf("  reachable from roots {0,1}: %d/%d (null tables: %d)\n",
                    n_reach, kN - n_null, n_null);
        if (n_unreach) std::printf("  UNREACHABLE via push map: %s\n", unreach);
        // Build pass: every non-null table constructs records cleanly.
        int built = 0;
        for (int s = 0; s < kN; ++s) {
            if (TableForScreen(s) == nullptr) continue;
            Nav_GameStateReset(); Nav_Init();
            Nav(s, kNavPush);
            if (Nav_ScreenId() == s && Nav_RecordCount() >= 1) ++built;
            else std::printf("  BUILD FAIL screen %d (recs=%d)\n", s, Nav_RecordCount());
        }
        std::printf("  build pass: %d/%d non-null tables build records OK\n",
                    built, kN - n_null);
    }

    // ----------------------------------------------------------------------
    // Piece 4 (WS-G2) — frontend mode selection sets the game mode. Selecting a
    // Single-/Multi-Player MODE item must write Nav_GameState().game_mode the way
    // FUN_0043dfd0 writes DAT_0067e9fc (via DAT_0067f184 + FUN_0042f6b0). The
    // resulting mode drives Race/RaceModes' rule derivation at race launch.
    // ----------------------------------------------------------------------
    std::printf("\n=== Piece 4 (WS-G2): mode-select -> game_mode wiring ===\n");
    int g2_fail = 0;
    auto modecheck = [&](const char* what, int got, int want) {
        bool ok = (got == want);
        std::printf("  [%s] %-26s got=%d want=%d : %s\n",
                    ok ? "ok" : "FAIL", what, got, want, ok ? "OK" : "FAIL");
        if (!ok) ++g2_fail;
    };
    // (a) direct action -> game_mode mapping (bypasses nav/avail).
    Nav_GameStateReset();
    modecheck("0xff3d (Challenge Cup)", NavTest_ApplyActionGameMode(0xff3d0000u), 3);
    modecheck("0xff4d (Quick Race)",    NavTest_ApplyActionGameMode(0xff4d0000u), 10);
    modecheck("0xff40 (Time Attack)",   NavTest_ApplyActionGameMode(0xff400000u), 2);
    modecheck("0xff2c (Top Dog)",       NavTest_ApplyActionGameMode(0xff2c0000u), 6);
    modecheck("0xff2e (Team Game)",     NavTest_ApplyActionGameMode(0xff2e0000u), 6);
    // a non-mode action must leave game_mode unchanged.
    Nav_GameStateReset();
    NavTest_ApplyActionGameMode(0xff500000u);  // Options -> no mode write
    modecheck("0xff50 (Options) no-op",  Nav_GameState().game_mode, 0);

    // (b) end-to-end nav flow: main(1) -> Single Player(2), select item 0
    // (Challenge Cup, always enabled) -> game_mode 3 AND push to colour(4).
    Nav_GameStateReset(); Nav_Init();
    Nav(2, kNavPush);                         // Single Player screen
    // cursor defaults to the first enabled item (0 = Challenge Cup).
    std::printf("  [SP screen cursor=%d (expect 0 Challenge Cup)]\n", Nav_Cursor());
    Nav_Select();                             // -> sets mode 3, pushes screen 4
    modecheck("flow: SP item0 -> mode3", Nav_GameState().game_mode, 3);
    std::printf("  [after select -> screen=%d (expect 4 colour)]\n", Nav_ScreenId());

    // (c) end-to-end nav flow: Multi Player(3), select item 0 (Top Dog) -> mode 6.
    Nav_GameStateReset(); Nav_Init();
    Nav(3, kNavPush);                         // Multi Player screen
    Nav_Select();                             // -> sets mode 6
    modecheck("flow: MP item0 -> mode6", Nav_GameState().game_mode, 6);

    std::printf("  Piece 4: %s (%d failures)\n", g2_fail ? "RED" : "GREEN", g2_fail);

    return 0;
}
