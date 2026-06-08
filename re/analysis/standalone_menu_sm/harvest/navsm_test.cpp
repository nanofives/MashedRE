// Standalone logic harness for Frontend/MenuNavSM (the reversed menu nav state
// machine). Links MenuNavSM.cpp directly and drives Nav_Init / Nav_Select /
// Nav_Back over the harvested descriptor tables, printing the record state at
// each level. Proves the harvested tables + reversed push map produce the right
// deeper-screen content WITHOUT needing the full D3D9 app. NOT shipped.
#include "../../../../mashedmod/src/mashed_re/Frontend/MenuNavSM.h"
#include <cstdio>

using namespace mashed_re::Frontend;

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

    return 0;
}
