// EvalResultCore - see EvalResultCore.h. Compiled into BOTH targets (exe_sources.rsp and asi_sources.rsp). The call order on
// EvalResultInputs is the original's call order (the .asi forwards each one to the original callee), so the .asi body runs
// the same callee sequence as the former ScoringHooks.cpp:EvalResult_00410510.
#include "EvalResultCore.h"

namespace mashed_re {
namespace Race {

// 0x00410510 EvaluateResultCore
int EvaluateResultCore(EvalResultInputs& in, RuleEngine::Rule7Trace* r7) {
    in.OnEnter();                                             // DAT_0063b90c = 0
    if (in.RaceType() == 2) return 0;                         // FUN_0042f6a0() == 2

    int winner = 0;                                           // iVar6
    // Score-target scan, cars 1..4 as 1-based (FUN_0040b6d0(i-1)):
    //   participants 2/3 (or teams): score == 8 -> winner
    //   rule == 2:                   score == 8 -> winner
    //   participants 4:              score > 11 -> winner
    for (int i = 1; i <= RuleEngine::kCars; ++i) {
        const int sc = in.Score(i - 1);
        if ((in.Participants() == 2 || in.Participants() == 3 || in.Teams() != 0) && sc == 8) winner = i;
        if (in.Rule() == 2 && sc == 8) winner = i;
        if (in.Participants() == 4 && 0xb < sc) winner = i;
    }
    // Alive refinement (slot probe 0x34..0x40 + FUN_0046c7b0(i) == 1): an active, alive car meeting the same target.
    if (winner != 0) {
        for (int i = 0; i < RuleEngine::kCars; ++i) {
            if (in.SlotActive(i) == 0 || in.Alive(i) != 1) continue;
            const int sc = in.Score(i);
            if ((in.Participants() == 2 || in.Participants() == 3 || in.Teams() != 0) && sc == 8) winner = i + 1;
            if (in.Participants() == 4 && 0xb < sc) winner = i + 1;
        }
    }

    bool concluded = false;                                   // goto LAB_0041062a
    switch (in.Rule()) {                                      // switch(DAT_007f0fd0)
    case 4:
        winner = in.FinishSlot(0) + 1;                        // first finisher + 1
        break;
    case 5:
        if (in.Alive(0) == 0) { winner = -1; concluded = true; break; }
        winner = in.CollectDone();
        break;
    case 7: {
        // First car (0..FUN_0040e340()) with metric > 3.0 -> car+1. The original's loop (0x0041075a) counts in edi only
        // and leaves esi (the score-scan winner) as it is when no car is above 3.0 (0x00410790). Rule7Keep() false clears
        // it (the exe's pre-r7keep behaviour, MASHED_NO_RULE7_KEEP).
        const int scan = winner;
        if (!in.Rule7Keep()) winner = 0;
        int metricWinner = 0;
        int n = in.Rule7Count();
        for (int i = 0; i < n;) {
            const float m = in.Metric(i);                     // fVar7 = FUN_00417730(iVar3)
            if (in.FinishMetric() < m) { metricWinner = i + 1; break; }
            ++i;
            n = in.Rule7Count();
        }
        if (metricWinner != 0) winner = metricWinner;
        if (r7) { r7->seen = true; r7->scan = scan; r7->metric = metricWinner; }
        break;
    }
    case 8:
        winner = in.FinishSlot(0) + 1;
        if (winner != 1) {                                    // car 0 not first
            if (winner == 0) winner = -1;                     // nobody finished
            concluded = true;
        }
        break;
    case 9: {
        // Any finisher already listed ends it with no winner (LAB_00410801), then motion, then the metric gap.
        if (in.FinishSlot(0) != -1) { winner = -1; in.OnPlayer0Flag(0); concluded = true; break; }
        if (in.Motion(0) != 0)      { winner = -1; in.OnPlayer0Flag(0); concluded = true; break; }
        const float m1 = in.Metric(1);                        // fStack_8 = FUN_00417730(1)
        const float m0 = in.Metric(0);
        if (in.Rule9Gap() <= m1 - m0) { winner = -1; in.OnPlayer0Flag(0); concluded = true; break; }
        winner = 1; in.OnPlayer0Flag(1); concluded = true;    // LAB_004107e3
        break;
    }
    case 10: {
        if (in.Motion(0) != 0) { winner = -1; in.OnPlayer0Flag(0); concluded = true; break; }
        const float m0 = in.Metric(0);                        // fVar7 = FUN_00417730(0)
        if (m0 < in.Rule10Metric()) {
            // NaN-aware `(a<b)==(a==b)`: timer still running -> return 0 (no end block)
            const float t = in.Timer(), z = in.TimerZero();
            if ((t < z) == (t == z)) {
                if (r7 && r7->seen) r7->ret = 0;
                return 0;
            }
            winner = -1; in.OnPlayer0Flag(0); concluded = true; break;   // LAB_00410801
        }
        winner = 1; in.OnPlayer0Flag(1); concluded = true;    // LAB_004107e3
        break;
    }
    default:
        break;                                                // rules {0,1,2,3,6}
    }

    if (!concluded && winner == 0) {                          // `if (iVar6 == 0) return 0;`
        if (r7 && r7->seen) r7->ret = 0;
        return 0;
    }
    in.OnConclude(winner);                                    // LAB_0041062a
    if (r7 && r7->seen) r7->ret = winner;
    return winner;
}

}  // namespace Race
}  // namespace mashed_re
