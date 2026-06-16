// F4 lap-line crossing-sequence test. Exercises the REAL Track::LapLineStep
// (shared verbatim with TrackRenderer::UpdateRace) on synthetic forward gate
// streams. Build:  cl /EHsc /I.. lap_test.cpp  &&  lap_test.exe
#include "../LapLogic.h"

#include <cstdint>
#include <cstdio>
#include <vector>

using mashed_re::Track::LapLineStep;

static int g_fail = 0;
static void check(const char* what, bool ok) {
    std::printf("  %s %s\n", ok ? "[ok]  " : "[FAIL]", what);
    if (!ok) ++g_fail;
}

// Drive `gates` in order through LapLineStep starting from a fresh lap_mask;
// return the number of laps completed.
static int run(const std::vector<int>& lapLines, const std::vector<int>& gates,
               std::uint32_t startMask = 0) {
    std::uint32_t mask = startMask;
    int laps = 0;
    for (int g : gates) if (LapLineStep(g, lapLines, mask)) ++laps;
    return laps;
}

int main() {
    std::printf("F4 LapLineStep — Arctic lap lines [0, 93]\n");
    {
        const std::vector<int> L = {0, 93};
        // one full forward loop from the grid (start at gate 1): 1..93 then 0
        std::vector<int> oneLap;
        for (int g = 1; g <= 93; ++g) oneLap.push_back(g);
        oneLap.push_back(0);
        check("one loop (1..93,0) = 1 lap", run(L, oneLap) == 1);

        // two full loops -> 2 laps
        std::vector<int> twoLaps = oneLap;
        for (int g = 1; g <= 93; ++g) twoLaps.push_back(g);
        twoLaps.push_back(0);
        check("two loops = 2 laps", run(L, twoLaps) == 2);

        // anti-shortcut: reaching the primary (0) WITHOUT having crossed 93
        // must not count (mask never gets bit 1)
        check("reach 0 without 93 = 0 laps", run(L, {0}) == 0);
        check("0,0,0 (never 93) = 0 laps", run(L, {0, 0, 0}) == 0);
        // crossing 93 then 0 (the sequence) = 1 lap
        check("93 then 0 = 1 lap", run(L, {93, 0}) == 1);
        // 93 alone (not the primary) = 0 laps
        check("93 alone = 0 laps", run(L, {93}) == 0);
    }

    std::printf("F4 LapLineStep — single lap line [0]\n");
    {
        const std::vector<int> L = {0};
        check("each 0 reach = a lap (0,0,0 -> 3)", run(L, {0, 0, 0}) == 3);
        check("non-primary gates ignored", run(L, {5, 9, 40}) == 0);
    }

    std::printf("F4 LapLineStep — three lap lines [0,30,60] (sequence)\n");
    {
        const std::vector<int> L = {0, 30, 60};
        // full loop crossing 30 and 60 before 0 = 1 lap
        check("30,60,0 = 1 lap", run(L, {30, 60, 0}) == 1);
        // missing one intermediate (only 30) -> no lap
        check("30,0 (missing 60) = 0 laps", run(L, {30, 0}) == 0);
        check("60,0 (missing 30) = 0 laps", run(L, {60, 0}) == 0);
        // order within the loop doesn't matter, only that all are crossed
        check("60,30,0 = 1 lap", run(L, {60, 30, 0}) == 1);
    }

    std::printf("F4 LapLineStep — no LAPDATA -> plain gate 0\n");
    {
        const std::vector<int> L = {};
        check("empty lap lines: each 0 = a lap", run(L, {0, 5, 0}) == 2);
    }

    std::printf(g_fail ? "\nFAILED (%d)\n" : "\nALL PASS\n", g_fail);
    return g_fail ? 1 : 0;
}
