// Mashed RE - Standalone menu navigation state machine (implementation).
//
// Port of MASHED's frontend nav centerpiece for the standalone mashed_re.exe.
// Because the standalone builds /BASE:0x10000 + image-pad .bss (which zeroes
// MASHED's address space), this port reimplements BOTH the logic AND every
// const/table it reads. The per-screen descriptor tables live at 0x005f6xxx
// (inside the zeroed range) so they are hard-coded here, harvested verbatim from
// original/MASHED.exe (SHA-256 BDCAE093...).
//
// Ported pieces (RVA-cited, NO-GUESSING):
//   FUN_0042ad90  kv-iterator      -> KvLookup()      (exact decomp transcription)
//   FUN_0042ac00  group-count      -> (not needed; item count derived from KvLookup)
//   FUN_0043d2a0  push/pop builder  -> Nav()          (Phase 3-7, minimal)
//   FUN_00432800  cursor-placement -> PlaceCursor()   (tail only; minimal all-enabled)
//   FUN_0042d3e0  array zero (C3)   -> RecordsZero()   (reimplemented locally)
//
// Deferred (documented in port spec): faithful per-screen grey-out cases
// (FUN_00430b60/0042f500/00497450 are sub-C2), the slide/countdown anim tick
// (FUN_004325c0), the prompt-strip glyph row (FUN_00432b30), and the faithful
// pixel draw loop (FUN_0043c5b0). The standalone renders g_records through its
// existing TextRenderer/MashedFont path instead.

#include "MenuNavSM.h"

#include <cstring>

namespace mashed_re {
namespace Frontend {

namespace {

// --------------------------------------------------------------------------
// Harvested per-screen descriptor tables (verbatim from MASHED.exe, pool13).
//
// Format: a flat u32 stream of (key, value) entries, walked by the kv-iterator
// FUN_0042ad90. Group delimiter = 0xff060000 (-0xfa0000); terminator =
// 0xff070000 (-0xf90000). The selectable list items are the 0xff040000 entries;
// 0xff000000 is the back-target string id; 0xff080000 is the prompt/screen id.
//
// KvLookup(table, 0xff040000, n) returns the id of the n-th list item; the
// number of items = count of 0xff040000 occurrences before 0xff070000.
//
// Source RVAs (read_only Mashed_pool13, anchor untouched):
//   PTR_DAT_005f7638[0..33] pointer array
//   table[0]=0x005f6860 (root / main menu; pushed by FUN_0043df00 as id 0)
//   table[1]=0x005f6980  table[2]=0x005f6a20  table[3]=0x005f6a98 ...
// --------------------------------------------------------------------------

// table[0] @0x005f6860 - root MAIN MENU (screen id 0). Items 0xff040000:
//   0x18, 0x27, 0x1c, 0x1d, 0x1e  (5 items, before first 0xff070000).
const std::uint32_t kTable0_Root[] = {
    0xff000000u, 0x00000047u, 0xff080000u, 0x00000005u,
    0xff020000u, 0x00000050u, 0xff030000u, 0x00000078u,
    0xff040000u, 0x00000018u, 0xff140000u, 0xff150000u, // 0xff140000 val = 0xff150000 (verbatim)
    0xff060000u, 0xff040000u, 0x00000027u, 0xff140000u,
    0x00000050u, 0xff060000u, 0xff040000u, 0x0000001cu,
    0xff140000u, 0x0000001eu, 0xff060000u, 0xff040000u,
    0x0000001du, 0xff140000u, 0x0000001fu, 0xff060000u,
    0xff040000u, 0x0000001eu, 0xff140000u, 0x00000020u,
    0xff060000u, 0xff090000u, 0xff0b0000u, 0xff0c0000u,
    0xff100000u, 0xff120000u, 0xff0a0000u, 0xff070000u, // terminator
};

// table[1] @0x005f6980. back-id 0x40. Items: 0x21, 0x22, 0x27, 0x26b, 0x261.
const std::uint32_t kTable1[] = {
    0xff000000u, 0x00000040u, 0xff080000u, 0x00000001u,
    0xff020000u, 0x00000050u, 0xff030000u, 0x00000078u,
    0xff040000u, 0x00000021u, 0xff050000u, 0x00000002u,
    0xff060000u, 0xff040000u, 0x00000022u, 0xff050000u,
    0x00000003u, 0xff060000u, 0xff040000u, 0x00000027u,
    0xff140000u, 0x00000050u, 0xff060000u, 0xff040000u,
    0x0000026bu, 0xff140000u, 0x00000082u, 0xff060000u,
    0xff040000u, 0x00000261u, 0xff140000u, 0x00000080u,
    0xff060000u, 0xff090000u, 0xff0b0000u, 0xff0c0000u,
    0xff100000u, 0xff0a0000u, 0xff070000u,
};

// table[2] @0x005f6a20. back-id 0x21. Items: 0xe5, 0xe6, 0x24.
const std::uint32_t kTable2[] = {
    0xff000000u, 0x00000021u, 0xff080000u, 0x00000002u,
    0xff020000u, 0x00000050u, 0xff030000u, 0x00000078u,
    0xff040000u, 0x000000e5u, 0xff140000u, 0x0000003du,
    0xff060000u, 0xff040000u, 0x000000e6u, 0xff140000u,
    0x0000004du, 0xff060000u, 0xff040000u, 0x00000024u,
    0xff050000u, 0x00000040u, 0xff060000u, 0xff090000u,
    0xff0b0000u, 0xff0c0000u, 0xff100000u, 0xff110000u,
    0xff0a0000u, 0xff070000u,
};

// table[3] @0x005f6a98. back-id 0x22. Items: 0x13e, 0x140.
const std::uint32_t kTable3[] = {
    0xff000000u, 0x00000022u, 0xff080000u, 0x00000002u,
    0xff020000u, 0x00000050u, 0xff030000u, 0x00000078u,
    0xff040000u, 0x0000013eu, 0xff140000u, 0x0000002cu,
    0xff060000u, 0xff040000u, 0x00000140u, 0xff140000u,
    0x0000002eu, 0xff060000u, 0xff090000u, 0xff0b0000u,
    0xff0c0000u, 0xff100000u, 0xff110000u, 0xff0a0000u,
    0xff070000u,
};

// PTR_DAT_005f7638 analogue: screen id -> descriptor table. Only the screens we
// have harvested are populated; the rest are nullptr (KvLookup tolerates null ->
// empty screen). Verbatim screen->table addresses from PTR_DAT_005f7638 confirm
// indices 0/1/2/3 map to these tables.
const std::uint32_t* const kScreenTables[] = {
    kTable0_Root, // [0] 0x005f6860 root
    kTable1,      // [1] 0x005f6980
    kTable2,      // [2] 0x005f6a20
    kTable3,      // [3] 0x005f6a98
};
constexpr int kScreenTableCount =
    static_cast<int>(sizeof(kScreenTables) / sizeof(kScreenTables[0]));

const std::uint32_t* TableForScreen(int screen_id) {
    if (screen_id < 0 || screen_id >= kScreenTableCount) return nullptr;
    return kScreenTables[screen_id];
}

// Root item index -> child screen pushed on Select(). The faithful item->screen
// map lives in the distributed per-screen select handlers (not reversed here;
// documented follow-up). For the demo, the first two root items push the two
// harvested submenus so a push lands on REAL submenu content; the rest are
// no-ops (documented). This is the only non-harvested heuristic in this module.
int RootChildScreen(int root_item_index) {
    switch (root_item_index) {
        case 0: return 1;   // -> table[1]
        case 1: return 2;   // -> table[2]
        case 2: return 3;   // -> table[3]
        default: return -1; // unmapped (follow-up RE)
    }
}

// Tag keys (negated forms confirmed in the iterator decomp).
constexpr std::uint32_t kTagBack = 0xff000000u; // -0x1000000
constexpr std::uint32_t kTagItem = 0xff040000u; // -0xfc0000
constexpr std::uint32_t kTermVal = 0xff070000u; // -0xf90000 (terminator)

// --------------------------------------------------------------------------
// Module state (standalone analogue of MASHED's .data nav globals).
// --------------------------------------------------------------------------
int        g_nav_depth = 0;                 // DAT_0067e9f8
NavSlot    g_stack[kMaxNavDepth] = {};
MenuRecord g_records[kMaxRecords] = {};     // 0x00898ac0 analogue
int        g_record_count = 0;
int        g_last_dir = 0;                  // DAT_0067e844

// --------------------------------------------------------------------------
// FUN_0042ad90  kv-iterator. Verbatim transcription of the decomp:
//   __fastcall(ECX unused, EDX=table); EBX=tag_key, EDI=occurrence.
//   Walks the flat u32 stream; on the occurrence-th match of tag_key, returns
//   the NEXT u32. Stops (returns -1) at 0xff070000.
// 0x0042ad90
// --------------------------------------------------------------------------
int KvLookup(const std::uint32_t* table, std::uint32_t tag_key, int occurrence) {
    if (table == nullptr) {
        return -1;
    }
    std::int32_t iVar1 = static_cast<std::int32_t>(table[0]);
    int idx = 0;        // iVar2
    int occ = 0;        // iVar3
    for (;;) {
        if (static_cast<std::uint32_t>(iVar1) == kTermVal) {
            return -1;
        }
        if (static_cast<std::uint32_t>(iVar1) == tag_key) {
            if (occurrence == occ) {
                return static_cast<std::int32_t>(table[idx + 1]);
            }
            ++occ;
        }
        iVar1 = static_cast<std::int32_t>(table[idx + 1]);
        ++idx;
    }
}

// FUN_0042d3e0 (MenuEntryArrayInit, C3) analogue - zero the whole record array.
// 0x0042d3e0
void RecordsZero() {
    std::memset(g_records, 0, sizeof(g_records));
    g_record_count = 0;
}

// Count the selectable items (0xff040000 occurrences) in a screen's table.
int CountItems(const std::uint32_t* table) {
    int n = 0;
    while (KvLookup(table, kTagItem, n) != -1) {
        ++n;
        if (n >= kMaxItems) break;
    }
    return n;
}

// FUN_00432800 tail (cursor placement) - MINIMAL all-enabled variant.
// Faithful grey-out cases are deferred (sub-C2 deps). Sets all avail[]=1, then
// scans forward from the stored cursor (wrapping at item_count) to the first
// enabled item; -1 if none. 0x00432800
void PlaceCursor(NavSlot& slot) {
    for (int i = 0; i < kMaxItems; ++i) slot.avail[i] = 1;
    if (slot.item_count <= 0) { slot.cursor = -1; return; }
    int c = (slot.cursor < 0) ? 0 : slot.cursor;
    for (int n = 0; n < slot.item_count; ++n) {
        const int probe = (c + n) % slot.item_count;
        if (slot.avail[probe] == 1) { slot.cursor = probe; return; }
    }
    slot.cursor = -1;
}

// FUN_0043d2a0 Phase 5+6 - expand the active screen's descriptor table into the
// record array. Back-button = record slot 0; list items follow. Minimal port:
// populates tag/type/ids/coords sufficient for the standalone text renderer.
void BuildRecords(const NavSlot& slot) {
    RecordsZero();

    int rec = 0;

    // Phase 5: back-button row (record slot 0). Tag 0xff000000; its id is the
    // first 0xff000000 entry in the table.
    const int back_id = KvLookup(slot.desc_table, kTagBack, 0);
    g_records[rec].tag    = static_cast<std::int32_t>(kTagBack);
    g_records[rec].type   = 1;
    g_records[rec].color  = static_cast<std::int32_t>(0xffffffffu);
    g_records[rec].scale  = 0.6f;       // 0x3f19999a
    g_records[rec].x      = 64.0f;      // 0x42800000
    g_records[rec].y      = 48.0f;      // 0x42400000
    g_records[rec].prim_id = back_id;   // back string id
    g_records[rec].sec_id  = -1;
    g_records[rec].row_index = -1;      // back row is not a list index
    ++rec;

    // Phase 6: list items (tag 0xff040000). One record per 0xff040000 occurrence.
    for (int row = 0; row < slot.item_count && rec < kMaxRecords; ++row) {
        const int prim = KvLookup(slot.desc_table, kTagItem, row);
        if (prim == -1) break;
        g_records[rec].tag       = static_cast<std::int32_t>(kTagItem);
        g_records[rec].type      = (g_nav_depth == 0) ? 1 : 0x1ff;
        g_records[rec].row_index = row;
        g_records[rec].color     = static_cast<std::int32_t>(0xffffffffu);
        g_records[rec].scale     = 0.8f;    // 0x3f4ccccd
        g_records[rec].x         = 64.0f;
        g_records[rec].y         = 0.0f;    // laid out by the renderer
        g_records[rec].prim_id   = prim;
        g_records[rec].sec_id    = -1;
        ++rec;
    }

    g_record_count = rec;
}

} // namespace

// --------------------------------------------------------------------------
// Public API
// --------------------------------------------------------------------------

void Nav_Init() {
    g_nav_depth = 0;
    std::memset(g_stack, 0, sizeof(g_stack));
    // FUN_0043df00 enters the frontend via FUN_0043d2a0(0, 2) (reload screen 0).
    NavSlot& s = g_stack[0];
    s.screen_id  = 0;
    s.desc_table = TableForScreen(0);
    s.slide_src  = s.desc_table;
    s.cursor     = 0;
    s.item_count = CountItems(s.desc_table);
    PlaceCursor(s);
    BuildRecords(s);
    g_last_dir = kNavReload;
}

// FUN_0043d2a0(screen_id, dir).  0x0043d2a0
void Nav(int screen_id, int dir) {
    if (dir == kNavPush) {
        if (g_nav_depth + 1 >= kMaxNavDepth) return; // stack full guard
        // Save the current cursor for restore on pop.
        g_stack[g_nav_depth].saved_cursor = g_stack[g_nav_depth].cursor;
        const int d = g_nav_depth + 1;
        NavSlot& s = g_stack[d];
        std::memset(&s, 0, sizeof(s));
        s.screen_id  = screen_id;                       // DAT_0067ed7c
        s.desc_table = TableForScreen(screen_id);       // DAT_0067ed78 = PTR_005f7638[id]
        s.slide_src  = s.desc_table;                    // DAT_0067ed38
        s.cursor     = 0;                               // DAT_0067ed80 = 0
        s.item_count = CountItems(s.desc_table);        // DAT_0067edb4
        PlaceCursor(s);                                 // FUN_00432800 tail
        g_nav_depth = d;                                // commit push (Phase 7)
        BuildRecords(s);
    } else if (dir == kNavPop) {
        if (g_nav_depth < 1) return;                    // can't pop past root
        --g_nav_depth;                                  // DAT_0067e9f8 = depth-1
        NavSlot& s = g_stack[g_nav_depth];
        s.cursor = (s.saved_cursor >= 0) ? s.saved_cursor : 0;
        PlaceCursor(s);
        BuildRecords(s);
    } else { // kNavReload
        NavSlot& s = g_stack[g_nav_depth];
        s.item_count = CountItems(s.desc_table);
        PlaceCursor(s);
        BuildRecords(s);
    }
    g_last_dir = dir;                                   // DAT_0067e844
}

void Nav_MoveCursor(int delta) {
    NavSlot& s = g_stack[g_nav_depth];
    if (s.item_count <= 0) return;
    int c = (s.cursor < 0) ? 0 : s.cursor;
    // Step over the avail[] mask (wrap). Mirrors the forward scan in FUN_00432800.
    for (int n = 0; n < s.item_count; ++n) {
        c = (c + (delta >= 0 ? 1 : -1) + s.item_count) % s.item_count;
        if (s.avail[c] == 1) break;
    }
    s.cursor = c;
}

bool Nav_Select() {
    const NavSlot& s = g_stack[g_nav_depth];
    if (s.cursor < 0 || s.cursor >= s.item_count) return false;
    // Only the root screen has a (heuristic) child map wired; deeper screens are
    // documented follow-up (faithful per-screen select handlers not yet reversed).
    int child = -1;
    if (g_nav_depth == 0) child = RootChildScreen(s.cursor);
    if (child < 0) return false;
    Nav(child, kNavPush);
    return true;
}

bool Nav_Back() {
    if (g_nav_depth < 1) return false;
    Nav(0 /*ignored*/, kNavPop);
    return true;
}

// --- accessors -------------------------------------------------------------

int               Nav_Depth()       { return g_nav_depth; }
const NavSlot&    Nav_TopSlot()      { return g_stack[g_nav_depth]; }
const MenuRecord* Nav_Records()      { return g_records; }
int               Nav_RecordCount()  { return g_record_count; }
int               Nav_Cursor()       { return g_stack[g_nav_depth].cursor; }
int               Nav_ScreenId()     { return g_stack[g_nav_depth].screen_id; }

} // namespace Frontend
} // namespace mashed_re
