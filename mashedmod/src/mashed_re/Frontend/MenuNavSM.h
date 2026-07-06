// Mashed RE - Standalone menu navigation state machine.
//
// Self-contained port of MASHED's frontend nav stack (FUN_0043d2a0 push/pop +
// the kv-iterator FUN_0042ad90 + the cursor-placement tail of FUN_00432800).
// Drives the standalone mashed_re.exe main menu from the REAL per-screen
// descriptor tables harvested from original/MASHED.exe (the 0x005f6xxx blobs,
// which live inside the image-padded zeroed range and so must be hard-coded).
//
// All RVAs are for MASHED.exe (size 2,846,720 / SHA-256 BDCAE093...).
//
// See re/analysis/standalone_menu_sm/FUN_0043c5b0_port_spec.md for the full spec.
#ifndef MASHED_RE_FRONTEND_MENUNAVSM_H
#define MASHED_RE_FRONTEND_MENUNAVSM_H

#include <cstdint>

namespace mashed_re {
namespace Frontend {

// One draw-record. Faithful 52-byte (13-int) layout of MASHED's 0x00898ac0
// record array (Part-1 spec field map). The standalone only consumes a subset
// of the fields for its text-path render; the rest are kept for layout fidelity
// and the (deferred) faithful draw-loop / anim-tick port.
struct MenuRecord {
    std::int32_t  tag;        // +0x00  anim/color tag (0xff000000 back, 0xff040000 item)
    std::int32_t  type;       // +0x04  item type flag / visible-state code
    std::int32_t  row_index;  // +0x08  running visible-item index
    std::int32_t  color;      // +0x0c  ARGB color
    std::int32_t  slide;      // +0x10  slide/anim counter (or scale fixed-point)
    float         scale;      // +0x14  float scale
    float         x;          // +0x18  X coord
    float         y;          // +0x1c  Y coord
    std::int32_t  pad20;      // +0x20
    std::int32_t  flag24;     // +0x24
    std::int32_t  sec_id;     // +0x28  secondary sprite/text id (-1 = none)
    std::int32_t  prim_id;    // +0x2c  primary sprite/text id   (-1 = none)
    std::int32_t  pad30;      // +0x30
};
static_assert(sizeof(MenuRecord) == 52, "MenuRecord must be 52 bytes (0x34)");

constexpr int kMaxRecords  = 31;   // 0x898ac0..0x899104 / 0x34 (Part-1)
constexpr int kMaxNavDepth = 8;    // nav stack depth slots
constexpr int kMaxItems    = 12;   // per-screen availability flags (DAT_0067ed84..)

// One nav-stack slot. Standalone analogue of MASHED's [depth*0x10]-indexed
// .data arrays (DAT_0067ed78/7c/80/84/b4/38/74). See spec Part-2 NavSlot.
struct NavSlot {
    std::int32_t      screen_id;          // DAT_0067ed7c
    const std::uint32_t* desc_table;      // DAT_0067ed78 (&harvested table for screen_id)
    std::int32_t      cursor;             // DAT_0067ed80 (highlighted item; -1 = none)
    std::int32_t      item_count;         // DAT_0067edb4 (visible item count)
    std::int32_t      avail[kMaxItems];   // DAT_0067ed84.. (1 = enabled)
    const std::uint32_t* slide_src;       // DAT_0067ed38 (slide-out source table)
    std::int32_t      saved_cursor;       // DAT_0067ed74
    std::int32_t      slot_kind;          // DAT_0067ed3c (per-slot screen-kind; 0 fresh)
};

// Direction codes for Nav() (EDI arg of FUN_0043d2a0).
enum NavDir : int { kNavPush = 0, kNavPop = 1, kNavReload = 2 };

// --------------------------------------------------------------------------
// Standalone game-state model (fresh-boot main-menu defaults).
//
// MASHED's frontend dispatcher (FUN_0043dfd0) and grey-out builder
// (FUN_00432800) branch on runtime/save globals (team slots DAT_007f1a14.. ,
// game-mode DAT_0067e9fc, unlock arrays DAT_007f0a40.. , player-active
// DAT_007e96fc.. , mode flags DAT_0067ea64/ecdc/ed6c, per-slot screen-kind
// DAT_0067ed3c). The standalone is a fresh main menu with NO race/team/mode in
// progress and no save loaded, so MASHED initializes those to their reset
// values (player slots = -1, every flag/array = 0). This struct mirrors exactly
// those defaults so the ported queries (FUN_0042bb60 / FUN_00402f40 /
// FUN_00430b60 / FUN_0042f500 / FUN_00497450) reproduce the routing & grey-out
// the real game shows at a fresh main menu. Settable so a future standalone that
// loads a save or enters a mode can drive faithful deeper routing.
struct MenuGameState {
    int  team_slot[4];      // DAT_007f1a14 / 24 / 34 / 44 (active player team; -1 = empty)
    int  game_mode;         // DAT_0067e9fc (0 = none at fresh menu)
    int  flag_ea64;         // DAT_0067ea64 (FUN_0042f500 return)
    int  flag_ecdc;         // DAT_0067ecdc
    int  flag_ed6c;         // DAT_0067ed6c
    int  player_active[4];  // DAT_007e96fc[i*0x80] (0 = inactive)

    // grey-out (FUN_00432800) state. Fresh-menu defaults below.
    int  has_savedata;      // DAT_007f0f2c (screen 1 item 3 enable; 0 = none)
    int  has_profiles;      // DAT_007f0ad4 (screen 2 item 1 enable; 0 = none)
    int  ea88;              // DAT_0067ea88 GameLength (sel==6 screen18; also MP game-length
                            //   rule source, WS-G1). Wrap depends on DAT_007f0a5c[track_set]
                            //   (unavailable standalone; assumed 0/false -> wrap {0,1,2} via
                            //   dec skips 2, inc visits it — see Nav_ConfigEditWrap [UNCERTAIN])
    int  ea7c;              // DAT_0067ea7c
    int  ea84;              // DAT_0067ea8c-adjacent player-count code (DAT_0067ea8c)
    int  cur_track_set;     // DAT_007f17c-indexed track count (unlock array head)
    int  unlock_track[1];   // DAT_007f0a50[DAT_0067f17c*0x30] (0 = locked)
    int  unlock_car[1];     // DAT_007f0a58[DAT_0067f17c*0x30] (0 = locked)

    // [D-11057] continue-cup / tier-progression state (FUN_0043dfd0 0xff240000
    // arm; harvest FUN_0043dfd0.c L291-331). ecdc/ed6c already above.
    int  ea6c;              // DAT_0067ea6c (cup-continue phase latch: 4 arming,
                            //   5 armed/shown; 1->2 toggle in the game_mode 6/2 gate)
    int  ed4c;             // DAT_0067ed4c (selects the ecdc!=0 restart-confirm
                            //   variant: 0 -> body 0x38, else -> body 0xa9)

    // [D-11057] game-config option-row values (screens 18/24; frontend_config_
    // screens_REmap_20260614.md). Edited LEFT/RIGHT with wrap via the
    // decrement/increment handler (both dumped in full 2026-07-04, Mashed_pool15,
    // FUN_0043dfd0.c dec L1449-1487/L1506-1548, inc L1685-1719/L1738-1781),
    // dispatched by screen-kind (DAT_0067ed3c[depth], switch cases 0x12=18 /
    // 0x18=24) then by the row selector `iVar8 = DAT_0067ed40[depth]` (read
    // directly as the dispatch value — NOT a separate lookup table; see
    // Nav_ConfigEditWrap). Fresh defaults are the s18/s24 jumped-to probe values.
    int  ea74;              // DAT_0067ea74 game-type   (sel==1 both screens; wrap 0..2)
    int  ea90;              // DAT_0067ea90             (sel==2 screen24 only; wrap 1..4)
    int  ea94;              // DAT_0067ea94 vehicle     (sel==4 screen18/sel==3 screen24;
                             //   wrap 0..0xc, orig skips FUN_00430830()==0 slots — that
                             //   skip is NOT ported, see Nav_ConfigEditWrap [UNCERTAIN])
    int  ea80;              // DAT_0067ea80 PowerUps    (sel==2 screen18; wrap 0..2)
    int  ea78;              // DAT_0067ea78 toggle bool (sel==5 screen18, XOR 1)
    int  ea98;              // DAT_0067ea98 Opp1        (sel==4 screen24; wrap 0..6,
                             //   orig also calls FUN_00431b80(1) — not ported [UNCERTAIN])
    int  ea9c;              // DAT_0067ea9c Opp2        (sel==5 screen24; wrap 0..6, ditto)
    int  eaa0;              // DAT_0067eaa0 Opp3        (sel==6 screen24; wrap 0..6, ditto)
    int  eaac;              // DAT_0067eaac toggle bool (sel==7 screen24, XOR 1)
};

// Access / reset the standalone game state (defaults = fresh main menu).
MenuGameState& Nav_GameState();
void           Nav_GameStateReset();

// Save-driven game state — scoped port of Save::DeserializeFromBuffer
// (FUN_00404e80, 0x00404e80): its step-1 REP MOVSD restores 0x148 dwords from
// save_buf+0x24A40 (= DAT_00827d98; save_buf 0x00803358 is a 1:1 image of
// gamesave.bin) into the live span 0x007f0a40..0x007f0f60 — the track/cup
// table (0x7f0a40, 13x12 int32), the car flags (0x7f0e50, 156 bytes) and the
// menu state bytes (incl. DAT_007f0f2c savedata gate / DAT_007f0ad4 profile
// gate). This loader replays that copy into the standalone's span model and
// re-derives the MenuGameState fields the grey-out/routing ports consume.
// `data/len` = a full gamesave.bin image (0x24FA0 bytes). Returns false (and
// keeps fresh defaults) when the image is not a written save (magic at +0 !=
// 0xDEADBEEF, first written by 0x00404F37 — the shipped blank save has 0;
// FUN_00404e80 itself does not validate the magic, the gate is caller-side).
bool           Nav_GameStateLoadSave(const unsigned char* data, unsigned len);

// --- public API ------------------------------------------------------------

// Initialize the nav stack at the root screen (id 0). Analogue of
// FUN_0043df00's FUN_0043d2a0(0, 2) frontend-enter reload.
void Nav_Init();

// Force the current screen's records to slide in (title->menu entry motion).
void Nav_AnimateIn();

// Dev screen jump (PageUp/PageDown inspection of every screen).
void Nav_DevGoto(int screen_id);
int  Nav_DevScreen();

// Push/pop/reload. Port of FUN_0043d2a0(screen_id, dir). For kNavPush, screen_id
// is the child screen to enter; for kNavPop/kNavReload it is ignored (operates on
// the current top). Repopulates g_records from the active screen's descriptor table.
void Nav(int screen_id, int dir);

// Move the highlight cursor on the current screen by +1 (down) or -1 (up),
// wrapping over the avail[] mask. Standalone feeder into the per-screen selection
// cursor (DAT_0067ed80 analogue), driven by UpdateMenuSelection().
void Nav_MoveCursor(int delta);

// Activate the currently-highlighted item: push its child screen (if mapped).
// Returns true if a push happened.
bool Nav_Select();

// Pop one level. Returns true if a pop happened (false at root).
bool Nav_Back();

// --- [D-11057] continue-cup confirmation chain -----------------------------
// Confirmation modal the caller (exe_main's standalone modal system) must open
// after the continue-cup action (0xff240000) arms one. Values mirror the
// FUN_0042bf30 dialogs the original opens in the FUN_0043dfd0 0xff240000 arm.
enum CupModal : int {
    kCupModalNone      = 0,
    kCupModalSingle    = 1,  // body 0x135, 1 button 0x2d      (single "Continue")
    kCupModalTwoChoice = 2,  // body 0x136, 2 buttons 0x2e/0x2f (Yes/No)
    kCupModalRestart38 = 3,  // body 0x38 (ecdc!=0, ed4c==0)    [not opened by caller yet]
    kCupModalRestartA9 = 4,  // body 0xa9 (ecdc!=0, ed4c!=0)    [not opened by caller yet]
};

// Begin the continue-cup action (verbatim port of FUN_0043dfd0's 0xff240000
// arm). Called from Nav_Select when the highlighted item's action == 0xff240000.
// Mutates ea6c/ed6c/e9fc(game_mode) exactly as the original, and EITHER performs
// the advance push (screen 7) itself and returns true, OR arms a confirmation
// modal (retrieve via Nav_TakePendingCupModal) and returns false (no nav).
bool Nav_ContinueCupBegin();

// Retrieve + clear the pending continue-cup modal armed by Nav_ContinueCupBegin.
// Returns kCupModalNone when none is pending.
int  Nav_TakePendingCupModal();

// Called when a continue-cup confirmation modal (single 0x135 / two-choice
// 0x136) is CONFIRMED. Verbatim: FUN_0043d2a0(7,0); if (ed6c==2) e9fc=3.
void Nav_ContinueCupConfirm();

// [D-11059(a)] Post-race cup-standings entry: arms the cup-continue latch
// (ea6c=4, the pre-state Nav_ContinueCupBegin's game_mode-6/2 gate requires)
// and pushes screen 5 (kT5) so the verbatim 0xff240000 Continue chain is
// reachable in a live playthrough. STANDALONE FLOW WIRING, not a verbatim
// port: the original's screen-5 push site and its post-race DAT_0067ea6c=4
// writer live OUTSIDE FUN_0043dfd0 and are unharvested
// (d11057_claude2_handoff_2026-07-03.md gap #1) [UNCERTAIN U-9014]. Called
// from the GameFlow results-dismissal boundary (exe_main), cup modes only.
void Nav_PushContinueCup();

// Live port of FUN_004307a0 (ElapsedVsThresholdCheck, 0x004307a0) — selects
// the modal variant (0 -> two-choice 0x136; nonzero -> single 0x135) by
// comparing race-elapsed seconds against the per-track DAT_00614718 threshold
// table (harvested 2026-07-06: row 0 = 15.0 s, rows 1..12 = 59.99 s; law +
// citations at the definition). Feed the finished race via
// Nav_SetCupContinueRace(trackIdx, elapsedSec) before the standings push;
// without race data it falls back to the settable default (dev/verification).
void Nav_SetCupContinueRace(int trackIdx, float elapsedSec);
void Nav_SetCupContinueVariant(int v);

// --- [D-11057] game-config option-row editing (screens 18/24) --------------
// Verbatim decrement/increment-with-wrap over the option-row value selected by
// the screen-kind (`screen_id`: 18 or 24; DAT_0067ed3c[depth] switch cases
// 0x12/0x18) and the row selector (`sel`; DAT_0067ed40[depth], read directly as
// the dispatch value — the original does NOT use a separate row->selector
// lookup table, see MenuNavSM.h/.cpp D-11057 comments). `dir` is -1 (LEFT/dec)
// or +1 (RIGHT/inc). Both directions confirmed 2026-07-04 (Mashed_pool15) via
// full decode of FUN_0043dfd0's dec (0x00440283+) and inc handlers:
//   screen 18: sel 1->ea74(0..2) 2->ea80(0..2) 3->ea7c(0..4) 4->ea94(0..0xc)
//              5->ea78(toggle) 6->ea88(GameLength, see MenuGameState comment).
//   screen 24: sel 1->ea74(0..2) 2->ea90(1..4) 3->ea94(0..0xc) 4->ea98(0..6)
//              5->ea9c(0..6) 6->eaa0(0..6) 7->eaac(toggle).
// Returns true if a value changed. NOT ported (see per-field [UNCERTAIN] notes
// in MenuGameState): the ea94 skip-invalid-vehicle check (orig calls
// FUN_00430830, an original-address hook incompatible with the standalone
// process), the FUN_00431b80(1) opponent-refresh side effect, and ea88's
// FUN_0042f500 recheck gate + track-set-dependent upper bound.
bool Nav_ConfigEditWrap(int screen_id, int sel, int dir);

// --- accessors for the renderer --------------------------------------------

int                 Nav_Depth();                 // 0 = root
const NavSlot&      Nav_TopSlot();
const MenuRecord*   Nav_Records();               // kMaxRecords entries
int                 Nav_RecordCount();           // # of populated item records
int                 Nav_Cursor();                // highlighted item index (-1 = none)
int                 Nav_ScreenId();              // current screen id

// The active screen's prompt-strip string id (the 0xff080000 descriptor entry,
// per FUN_0043d2a0 Phase 7 / FUN_00432b30). Drawn at the bottom prompt strip
// (virtual X=0x40, Y=0x1ac, scale 0x3f19999a). Returns -1 if none.
int                 Nav_PromptId();

// Whether list item `row_index` on the current screen is enabled (selectable).
// Disabled (greyed) items have avail[row]==0 (FUN_00432800 per-screen disables).
bool                Nav_ItemEnabled(int row_index);

// Advance the menu slide animation one frame (port of FUN_004325c0). `delta` is
// Frame clock — verbatim port of FUN_00493480 (0x00493480): quantizes the raw
// per-frame elapsed ms (the standalone's clock stands in for FUN_00493390) to
// multiples of 50 ms with a carry accumulator and snap bands, producing the
// DAT_007f1000 (int ms) / DAT_007f1004 (float = ms/3000) equivalents that the
// anim tick consumes. Call once per frame BEFORE Nav_AnimTick.
void                Nav_FrameClockUpdate(int raw_ms);

// Verbatim port of FUN_004325c0 (0x004325c0): advances every record's anim
// state using the frame delta from Nav_FrameClockUpdate (frame-rate-scaled,
// exactly the original's per-tag multipliers). Returns true when no record is
// left animating. Call once per RenderFrame.
bool                Nav_AnimTick();

// Slide counter (0x1ff..0) for record `rec_index` (NOT row index; includes the
// back row at index 0). 0x1ff = fully off-screen / just-entered, 0 = settled.
// The renderer maps this to a horizontal slide-in offset.
int                 Nav_RecordSlide(int rec_index);

// Vertical-grow fraction (0 = collapsed line, 1 = full height) for the
// menu-entry animation. The original grows item plates in HEIGHT (not a
// horizontal slide); the renderer scales each row's plate/text height by this.
float               Nav_RecordGrow(int rec_index);

} // namespace Frontend
} // namespace mashed_re

#endif // MASHED_RE_FRONTEND_MENUNAVSM_H
