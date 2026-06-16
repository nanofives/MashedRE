// WS-G2 verification — game-mode -> race-rule pipeline (Race/RaceModes).
//
// Proves that each REAL game mode launches with the right race rule + objective,
// by cross-checking RaceModes against an INDEPENDENT ground-truth table decoded
// by hand from the original decomp (re/analysis/game_mode_rules_REmap_20260616.md
// steps 1-4: FUN_0042f6b0 selection map, the DAT_005f65c8 cup event-type table,
// FUN_0043dfd0 event->rule map, FUN_00410d10 elimination split). If the C++ port
// diverges from that decoded data this test fails.
//
// Build (from mashedmod/src/mashed_re):
//   cl /EHsc /I.. Race\tests\racemodes_test.cpp Race\RaceModes.cpp
//   Race\tests\racemodes_test.exe
#include "../RaceModes.h"

#include <cstdio>
#include <initializer_list>

using namespace mashed_re::Race;

static int g_fail = 0;
static void check(const char* what, bool ok) {
    std::printf("  %s %s\n", ok ? "[ok]  " : "[FAIL]", what);
    if (!ok) ++g_fail;
}
static void eqi(const char* what, int got, int want) {
    bool ok = (got == want);
    std::printf("  %s %-46s got=%d want=%d\n", ok ? "[ok]  " : "[FAIL]", what, got, want);
    if (!ok) ++g_fail;
}

int main() {
    // --- Step 1: selection index -> game-mode (FUN_0042f6b0) ------------------
    std::printf("Step 1 — SelectionToGameMode (FUN_0042f6b0)\n");
    const int sel2mode[12] = { 2, 3, 4, 6, -1, 5, -1, -1, 7, 8, 9, 10 };
    for (int sel = 0; sel < 12; ++sel) {
        char b[40]; std::snprintf(b, sizeof b, "sel %d", sel);
        eqi(b, RaceModes::SelectionToGameMode(sel), sel2mode[sel]);
    }

    // --- Step 2/3: full cup (mode,track) -> rule table -----------------------
    // Ground truth hand-decoded from the REmap step-3 table (event-type per cell
    // -> rule via step 2). Modes 3/4/5 are the three championship cup tiers; the
    // 13 cup tracks are columns. All other modes derive rule 0 on this path.
    std::printf("\nStep 2/3 — RaceRule(mode, track) vs decoded cup table\n");
    const int kTracks = 13;
    const int rule3[kTracks] = { 0,0,0,4,0,0,0,0,4,0,4,0,0 };
    const int rule4[kTracks] = { 4,0,10,0,4,0,8,4,0,0,9,0,10 };
    const int rule5[kTracks] = { 0,4,9,5,9,10,0,5,0,9,0,8,5 };
    for (int t = 0; t < kTracks; ++t) {
        char b[48];
        std::snprintf(b, sizeof b, "mode3 track%d", t); eqi(b, RaceModes::RaceRule(3, t), rule3[t]);
        std::snprintf(b, sizeof b, "mode4 track%d", t); eqi(b, RaceModes::RaceRule(4, t), rule4[t]);
        std::snprintf(b, sizeof b, "mode5 track%d", t); eqi(b, RaceModes::RaceRule(5, t), rule5[t]);
    }
    // Non-championship modes derive rule 0 (case 2 clears slots; 6/7/8/9 MP gate;
    // 10 push-screen; none write DAT_007f0fd0 on the championship action path).
    for (int m : {2, 6, 7, 8, 9, 10}) {
        char b[40]; std::snprintf(b, sizeof b, "mode%d track0 -> rule0", m);
        eqi(b, RaceModes::RaceRule(m, 0), 0);
    }

    // --- Step 2: event-type -> rule (FUN_0043dfd0 0xff240000 switch) ----------
    std::printf("\nStep 2 — RaceRuleFromEvent\n");
    const int ev2rule[13] = {
        /*0*/0, /*1*/0, /*2*/4, /*3*/4, /*4*/10, /*5*/9, /*6*/8,
        /*7*/5, /*8*/7, /*9*/0, /*a*/0, /*b*/4, /*c*/0 };
    for (int ev = 0; ev <= 0xc; ++ev) {
        char b[40]; std::snprintf(b, sizeof b, "ev 0x%x", ev);
        eqi(b, RaceModes::RaceRuleFromEvent(ev), ev2rule[ev]);
    }

    // --- Step 4: elimination split (FUN_00410d10: rules {5,7} non-elim) -------
    std::printf("\nStep 4 — IsEliminationRule / objective\n");
    for (int r = 0; r <= 10; ++r) {
        bool want_elim = (r != 5 && r != 7);
        char b[40]; std::snprintf(b, sizeof b, "rule %d elim", r);
        check(b, RaceModes::IsEliminationRule(r) == want_elim);
        // objective: 0 = elimination, 1 = laps/non-elim race
        std::snprintf(b, sizeof b, "rule %d objective", r);
        eqi(b, RaceModes::RaceModeForObjective(r), want_elim ? 0 : 1);
    }
    // The only non-elimination cup races are mode 5 on tracks 3, 7, 12 (event 7
    // -> rule 5). Spot-check the end-to-end mode->rule->objective for those.
    std::printf("\nEnd-to-end — the 3 non-elimination cup races (mode5, tracks 3/7/12)\n");
    for (int t : {3, 7, 12}) {
        int rule = RaceModes::RaceRule(5, t);
        int obj  = RaceModes::RaceModeForObjective(rule);
        char b[56]; std::snprintf(b, sizeof b, "mode5 track%d rule=5 -> laps", t);
        check(b, rule == 5 && obj == 1);
    }
    // ...and a representative elimination race (Challenge Cup default mode3 t0).
    {
        int rule = RaceModes::RaceRule(3, 0);
        check("mode3 track0 -> rule0 -> elimination",
              rule == 0 && RaceModes::RaceModeForObjective(rule) == 0);
    }

    std::printf("\n%s — %d failures\n", g_fail ? "RED" : "GREEN", g_fail);
    return g_fail ? 1 : 0;
}
