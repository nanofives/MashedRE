// Self-check for MenuStringTable (the menu id->glyph-string loader).
// Links MenuStringTable.cpp + PizReader.cpp. Drives both load paths against the
// real assets and verifies id resolution + the FUN_004277a0 control-char remap.
//
//   cl /EHsc /std:c++17 /I..\..\..\..\mashedmod\src\mashed_re menustr_test.cpp \
//      ..\..\..\..\mashedmod\src\mashed_re\D3d9Render\MenuStringTable.cpp \
//      ..\..\..\..\mashedmod\src\mashed_re\Piz\PizReader.cpp
//
// Run from repo root so the relative asset paths resolve.

#include <cstdio>
#include <cstring>
#include <cstdint>
#include <initializer_list>
#include "D3d9Render/MenuStringTable.h"

using mashed_re::D3d9Render::MenuStringTable;

static int g_fail = 0;
static void CHECK(bool cond, const char* what) {
    std::printf("  [%s] %s\n", cond ? "PASS" : "FAIL", what);
    if (!cond) ++g_fail;
}

static void dump(const MenuStringTable& t, int id) {
    wchar_t w[128];
    int n = t.Decode(id, w, 128);
    std::uint32_t off = 0; bool r = t.Resolve(id, &off);
    char a[128]; int k = 0;
    for (; k < n && k < 127; ++k) a[k] = (w[k] >= 32 && w[k] < 127) ? (char)w[k] : '.';
    a[k] = 0;
    std::printf("    id 0x%-4x resolve=%d off=0x%-5x len=%-2d \"%s\"\n",
                id, r ? 1 : 0, off, n, a);
}

int main() {
    std::printf("== MenuStringTable self-check ==\n");

    // ---- Path 1: raw FONT/<lang>.dat (mirror of FUN_004274e0) ----
    MenuStringTable usa;
    bool okusa = usa.LoadFile("original/TOASTART/Common/FONT/USA.dat");
    std::printf("USA.dat (raw FONT/): load=%d size=%u id_count=%u\n",
                okusa, usa.size(), usa.id_count());
    CHECK(okusa, "USA.dat loads");
    CHECK(usa.id_count() == 449, "USA.dat covers 449 ids (offtable 0x704/4)");
    {
        // id 0 must decode to "English" (off 0x704, len 7).
        wchar_t w[64]; int n = usa.Decode(0, w, 64);
        bool eng = (n == 7 && w[0]=='E' && w[1]=='n' && w[6]=='h');
        CHECK(eng, "id 0 decodes to \"English\"");
        std::uint32_t off=0; usa.Resolve(0,&off);
        CHECK(off == 0x704, "id 0 offset == 0x704");
    }
    // Known menu/prompt ids (mostly blank in USA.dat; resolve must still be sane).
    for (int id : {0, 0x13, 0x18, 0x21, 0x22, 0x23, 0x27, 0x42, 0x43, 0x48, 0x150, 0x224})
        dump(usa, id);
    CHECK(!usa.Resolve(0x224, nullptr), "id 0x224 is the unused/centered sentinel (off 0)");
    CHECK(!usa.Resolve(449, nullptr),   "id beyond table is rejected");

    // ---- Path 2: real localized labels live in English.dat ----
    MenuStringTable eng;
    bool okeng = eng.LoadFile("original/TOASTART/Common/FONT/English.dat");
    std::printf("English.dat: load=%d size=%u id_count=%u\n",
                okeng, eng.size(), eng.id_count());
    CHECK(okeng, "English.dat loads");
    // The standalone main menu uses ids 33/34/35/36/39 (LoadMenuItems). Verify at
    // least one decodes to non-empty real text in English.dat.
    {
        int hits = 0;
        for (int id : {33, 34, 35, 36, 39}) {
            wchar_t w[128]; int n = eng.Decode(id, w, 128);
            if (n > 0) ++hits;
            dump(eng, id);
        }
        CHECK(hits >= 1, "at least one main-menu label (33/34/35/36/39) is non-empty");
    }

    // ---- Path 3: .piz entry (mirror of the standalone's LoadMessageTable) ----
    MenuStringTable piz;
    bool okpiz = piz.LoadPizEntry("original/TOASTART/Common/Font36.piz", "USA.DAT");
    std::printf("Font36.piz/USA.DAT: load=%d size=%u id_count=%u\n",
                okpiz, piz.size(), piz.id_count());
    CHECK(okpiz, "Font36.piz/USA.DAT loads via .piz path");

    // ---- Control-char remap (FUN_004277a0): synthesize a blob whose entry holds
    //      control codes 8..0xe and confirm they map to 0x81/7f/81/8d/80/87/8f. ----
    {
        // offtable: 1 id, off = 8; entry: len=7, chars 8,9,a,b,c,d,e
        unsigned char blob[8 + 2 + 7*2] = {0};
        unsigned int off = 8;
        std::memcpy(blob, &off, 4);            // id 0 offset
        unsigned short len = 7; std::memcpy(blob + 8, &len, 2);
        unsigned short src[7] = {8,9,0xa,0xb,0xc,0xd,0xe};
        std::memcpy(blob + 10, src, sizeof(src));
        // Write to a temp file and LoadFile it (LoadFile is the canonical path).
        std::FILE* f = std::fopen("verify/_menustr_remap.dat", "wb");
        std::fwrite(blob, 1, sizeof(blob), f); std::fclose(f);
        MenuStringTable rm; bool okrm = rm.LoadFile("verify/_menustr_remap.dat");
        wchar_t w[16]; int n = rm.Decode(0, w, 16);
        bool ok = okrm && n == 7 &&
                  w[0]==0x81 && w[1]==0x7f && w[2]==0x81 && w[3]==0x8d &&
                  w[4]==0x80 && w[5]==0x87 && w[6]==0x8f;
        CHECK(ok, "control codes 8..0xe remap to 0x81/7f/81/8d/80/87/8f");
        std::printf("    remap out: %04x %04x %04x %04x %04x %04x %04x\n",
                    w[0],w[1],w[2],w[3],w[4],w[5],w[6]);
    }

    std::printf("== %s (%d failures) ==\n", g_fail ? "FAILED" : "ALL PASS", g_fail);
    return g_fail ? 1 : 0;
}
