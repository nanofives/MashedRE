// racemodes_selfcheck — host unit check for Race/RaceModes (WS-G1).
// Builds the module + this main, prints the full game-mode -> race-rule ->
// objective derivation, and asserts it against the values independently
// decoded from MASHED.exe .rdata (DAT_005f65c8) — see
// re/analysis/game_mode_rules_REmap_20260616.md.
//
// Build (host, no deps):
//   cl /nologo /EHsc racemodes_selfcheck.cpp ^
//      ..\..\mashedmod\src\mashed_re\Race\RaceModes.cpp
//   racemodes_selfcheck.exe
#include "../../mashedmod/src/mashed_re/Race/RaceModes.h"
#include <cstdio>

using namespace mashed_re::Race::RaceModes;

// Independent expectation: event-type per (track, mode 3/4/5) decoded by the
// Python re-parse of the 0x005f65c8 bytes (re map step 3).
static const int kExpectEvent[13][3] = {
    {0x9,0x3,0x1}, {0xa,0x1,0xb}, {0xa,0x4,0x5}, {0x3,0x1,0x7},
    {0x1,0x2,0x5}, {0x1,0xc,0x4}, {0x1,0x6,0xc}, {0xa,0x3,0x7},
    {0x3,0x1,0xc}, {0x1,0xc,0x5}, {0x3,0x5,0x1}, {0x1,0xc,0x6},
    {0x1,0x4,0x7},
};

int main() {
    int fails = 0;

    // 1) selection -> game mode (FUN_0042f6b0)
    const int selExp[12] = {2,3,4,6,-1,5,-1,-1,7,8,9,10};
    for (int s = 0; s < 12; ++s) {
        int got = SelectionToGameMode(s);
        if (got != selExp[s]) { std::printf("FAIL sel %d -> %d (exp %d)\n", s, got, selExp[s]); ++fails; }
    }

    // 2) championship table -> rule -> objective, all (mode 3/4/5, track 0..12)
    std::printf("mode track ev rule objective\n");
    for (int m = 3; m <= 5; ++m) {
        for (int t = 0; t < 13; ++t) {
            int ev = EventType(m, t);
            int rule = RaceRule(m, t);
            int obj = RaceModeForObjective(rule);
            int evExp = kExpectEvent[t][m - 3];
            if (ev != evExp) { std::printf("FAIL ev mode %d track %d -> 0x%x (exp 0x%x)\n", m, t, ev, evExp); ++fails; }
            // objective must equal the proven split: race(1) iff rule in {5,7}
            int objExp = (rule == 5 || rule == 7) ? 1 : 0;
            if (obj != objExp) { std::printf("FAIL obj mode %d track %d rule %d -> %d (exp %d)\n", m, t, rule, obj, objExp); ++fails; }
            std::printf("  %d   %2d   0x%x  %2d   %s\n", m, t, ev, rule, obj ? "race" : "elim");
        }
    }

    // 3) non-championship modes -> rule 0 -> elimination (default demo)
    const int otherModes[6] = {2,6,7,8,9,10};
    for (int i = 0; i < 6; ++i) {
        int rule = RaceRule(otherModes[i], 0);
        if (rule != 0 || RaceModeForObjective(rule) != 0) {
            std::printf("FAIL mode %d -> rule %d obj %d (exp rule 0 / elim)\n",
                        otherModes[i], rule, RaceModeForObjective(rule)); ++fails;
        }
    }
    // default challenge-select scaffold: gameMode 6 -> elimination (no regression)
    if (RaceModeForObjective(RaceRule(6, 0)) != 0) { std::printf("FAIL default mode-6 not elim\n"); ++fails; }

    std::printf(fails ? "\nSELFCHECK FAILED (%d)\n" : "\nSELFCHECK OK\n", fails);
    return fails ? 1 : 0;
}
