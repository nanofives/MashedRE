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
};

// Direction codes for Nav() (EDI arg of FUN_0043d2a0).
enum NavDir : int { kNavPush = 0, kNavPop = 1, kNavReload = 2 };

// --- public API ------------------------------------------------------------

// Initialize the nav stack at the root screen (id 0). Analogue of
// FUN_0043df00's FUN_0043d2a0(0, 2) frontend-enter reload.
void Nav_Init();

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

// --- accessors for the renderer --------------------------------------------

int                 Nav_Depth();                 // 0 = root
const NavSlot&      Nav_TopSlot();
const MenuRecord*   Nav_Records();               // kMaxRecords entries
int                 Nav_RecordCount();           // # of populated item records
int                 Nav_Cursor();                // highlighted item index (-1 = none)
int                 Nav_ScreenId();              // current screen id

} // namespace Frontend
} // namespace mashed_re

#endif // MASHED_RE_FRONTEND_MENUNAVSM_H
