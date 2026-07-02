// WS-G rules debt (D-11052) verification — per-rule win-condition engine
// (Race/RuleEngine). Cross-checks the port against hand-decoded ground truth
// from the pool13 decomp of FUN_00410d10 / FUN_00410510 / FUN_004177b0 /
// FUN_004046a0 (session race_rules_d1-20260702; constants byte-verified:
// 3.0f @ 0x005cc31c, 2.0f @ 0x005cc574, 0.9f @ 0x005cc9c8, -1.0f @ 0x005cc33c,
// timer seeds @ 0x005cc740..0x005cc754, default 30.0).
//
// Build (from mashedmod/src/mashed_re):
//   cl /EHsc /I.. Race\tests\ruleengine_test.cpp Race\RuleEngine.cpp
//   ruleengine_test.exe
#include "../RuleEngine.h"

#include <cstdio>
#include <cmath>

using namespace mashed_re::Race::RuleEngine;

static int g_fail = 0;
static void eqi(const char* what, int got, int want) {
    bool ok = (got == want);
    std::printf("  %s %-52s got=%d want=%d\n", ok ? "[ok]  " : "[FAIL]", what,
                got, want);
    if (!ok) ++g_fail;
}
static void eqb(const char* what, bool got, bool want) {
    eqi(what, got ? 1 : 0, want ? 1 : 0);
}
static void eqf(const char* what, float got, float want) {
    bool ok = std::fabs(got - want) < 1e-4f;
    std::printf("  %s %-52s got=%.4f want=%.4f\n", ok ? "[ok]  " : "[FAIL]",
                what, got, want);
    if (!ok) ++g_fail;
}

static Cars mkCars() {
    Cars c;
    c.participants = 4;
    for (int i = 0; i < 4; ++i) {
        c.active[i] = true;
        c.alive[i] = true;
        c.score[i] = 0;
        c.metric[i] = 0.f;
    }
    return c;
}

int main() {
    std::printf("RuleEngine — FUN_00410d10 SegmentCheck per-rule pre-blocks\n");
    {
        Cars c = mkCars(); Persist p; bool elim = false;
        // default rules {0,1,2,3,6}: straight to the elimination block
        eqi("rule 0 running -> continue", SegmentCheck(0, c, p, false, &elim), 0);
        eqb("rule 0 runs elimination", elim, true);
        // resultDeclared (FUN_00443080()==1) -> immediate 0, no elimination
        eqi("result declared -> 0", SegmentCheck(0, c, p, true, &elim), 0);
        eqb("result declared skips elim", elim, false);
        // last man standing (alive==1, multi-slot)
        c.alive[1] = c.alive[2] = c.alive[3] = false;
        eqi("last man -> segment over", SegmentCheck(0, c, p, false, &elim), 1);
    }
    {
        Cars c = mkCars(); Persist p; bool elim = true;
        // rule 5: player dead -> over; else CollectAllDone decides; NEVER elim
        eqi("rule 5 nothing collected -> continue", SegmentCheck(5, c, p, false, &elim), 0);
        eqb("rule 5 skips elimination", elim, false);
        p.collectTotal = 3; p.collectDone = 3;
        eqi("rule 5 all collected -> over", SegmentCheck(5, c, p, false, &elim), 1);
        c.alive[0] = false;
        eqi("rule 5 player dead -> over", SegmentCheck(5, c, p, false, &elim), 1);
    }
    {
        Cars c = mkCars(); Persist p; bool elim = true;
        // rule 7: no elim; over when any metric > 3.0, player dead, or all-but-one dead
        eqi("rule 7 running -> continue", SegmentCheck(7, c, p, false, &elim), 0);
        eqb("rule 7 skips elimination", elim, false);
        c.metric[2] = 3.5f;
        eqi("rule 7 car2 crossed 3.0 -> over", SegmentCheck(7, c, p, false, &elim), 1);
        c.metric[2] = 0.f; c.alive[1] = c.alive[2] = c.alive[3] = false;
        eqi("rule 7 all-but-one dead -> over", SegmentCheck(7, c, p, false, &elim), 1);
        c = mkCars(); c.alive[0] = false;
        eqi("rule 7 player dead -> over", SegmentCheck(7, c, p, false, &elim), 1);
    }
    {
        Cars c = mkCars(); Persist p; bool elim = false;
        // rule 4: elim continues while metric[0] < 3.0
        eqi("rule 4 running -> continue", SegmentCheck(4, c, p, false, &elim), 0);
        eqb("rule 4 runs elimination", elim, true);
        c.metric[0] = 3.0f;
        eqi("rule 4 metric0 at 3.0 -> over", SegmentCheck(4, c, p, false, &elim), 1);
    }
    {
        Cars c = mkCars(); Persist p; bool elim = false;
        // rule 8: metric0 >= 3.0 or motion0 != 0 -> over
        c.metric[0] = 2.9f;
        eqi("rule 8 running -> continue", SegmentCheck(8, c, p, false, &elim), 0);
        c.motion0 = 2;
        eqi("rule 8 motion0 -> over", SegmentCheck(8, c, p, false, &elim), 1);
    }
    {
        Cars c = mkCars(); Persist p; bool elim = false;
        // rule 9: metric0/metric1 >= 3.0, motion0, snapshot1
        eqi("rule 9 running -> continue", SegmentCheck(9, c, p, false, &elim), 0);
        c.metric[1] = 3.1f;
        eqi("rule 9 metric1 crossed -> over", SegmentCheck(9, c, p, false, &elim), 1);
        c.metric[1] = 0.f; c.snapshot1 = 1;
        eqi("rule 9 snapshot1 -> over", SegmentCheck(9, c, p, false, &elim), 1);
    }
    {
        Cars c = mkCars(); Persist p; bool elim = false;
        // rule 10: timer expiry / metric0 >= 2.0 / motion0
        p.timer = 10.f;
        eqi("rule 10 running -> continue", SegmentCheck(10, c, p, false, &elim), 0);
        p.timer = -0.1f;
        eqi("rule 10 timer expired -> over", SegmentCheck(10, c, p, false, &elim), 1);
        p.timer = 10.f; c.metric[0] = 2.0f;
        eqi("rule 10 metric0 at 2.0 -> over", SegmentCheck(10, c, p, false, &elim), 1);
        // timer exactly 0 IS expired: (a<0)!=(a==0) at a==0 -> false!=true ->
        // over (and FUN_00410510 case 10 agrees: (a<0)==(a==0) -> false ->
        // not-continue).
        c.metric[0] = 0.f; p.timer = 0.f;
        eqi("rule 10 timer exactly 0 -> over", SegmentCheck(10, c, p, false, &elim), 1);
    }

    std::printf("RuleEngine — FUN_004177b0 finish-order fragment\n");
    {
        Cars c = mkCars(); Persist p;
        c.metric[2] = 3.2f;
        UpdateFinishOrder(7, c, p);
        eqf("first finisher slot = car 2", p.finishOrder[0], 2.f);
        eqf("slot 1 still empty", p.finishOrder[1], kEmptySlot);
        UpdateFinishOrder(7, c, p);           // no duplicate append
        eqf("no duplicate append", p.finishOrder[1], kEmptySlot);
        c.metric[0] = 3.0f;                    // >= threshold finishes
        UpdateFinishOrder(7, c, p);
        eqf("second finisher slot = car 0", p.finishOrder[1], 0.f);
        Persist p2;
        UpdateFinishOrder(0, c, p2);           // rules outside {4,7,8,9}: no-op
        eqf("rule 0 records no finishers", p2.finishOrder[0], kEmptySlot);
    }

    std::printf("RuleEngine — FUN_00410510 EvaluateResult\n");
    {
        Cars c = mkCars(); Persist p; bool p0 = false;
        // no conclusion -> 0 (next round)
        eqi("rule 0 no target -> 0 (next round)", EvaluateResult(0, c, p, &p0), 0);
        // score target: participants 4 -> score > 11
        c.score[1] = 12;
        eqi("rule 0 score 12 -> winner car1 (2)", EvaluateResult(0, c, p, &p0), 2);
        eqb("p0Won false", p0, false);
        c.score[1] = 0; c.score[0] = 12;
        eqi("rule 0 score 12 on car0 -> 1", EvaluateResult(0, c, p, &p0), 1);
        eqb("p0Won true", p0, true);
        // participants 3: target is score == 8
        c = mkCars(); c.participants = 3; c.score[2] = 8;
        eqi("3 players score 8 -> winner car2 (3)", EvaluateResult(0, c, p, &p0), 3);
        // rule 2 (quick-race length 2): score == 8 target at any participants
        c = mkCars(); c.score[3] = 8;
        eqi("rule 2 score 8 -> winner car3 (4)", EvaluateResult(2, c, p, &p0), 4);
        // dead-winner refinement: alive car with target preferred
        c = mkCars(); c.score[1] = 12; c.alive[1] = false; c.score[2] = 12;
        eqi("refinement prefers alive car2 (3)", EvaluateResult(0, c, p, &p0), 3);
        // time attack (mode 2) -> handled elsewhere
        Persist pt; pt.timeAttack = true; c.score[1] = 12;
        eqi("time attack -> 0", EvaluateResult(0, c, pt, &p0), 0);
    }
    {
        Cars c = mkCars(); Persist p; bool p0 = false;
        // rule 5: player dead -> -1; collected -> 1 (car 0)
        c.alive[0] = false;
        eqi("rule 5 player dead -> -1", EvaluateResult(5, c, p, &p0), -1);
        c.alive[0] = true; p.collectTotal = 2; p.collectDone = 2;
        eqi("rule 5 all collected -> car0 wins (1)", EvaluateResult(5, c, p, &p0), 1);
        eqb("rule 5 p0Won", p0, true);
        p.collectDone = 1;
        eqi("rule 5 not done -> 0", EvaluateResult(5, c, p, &p0), 0);
    }
    {
        Cars c = mkCars(); Persist p; bool p0 = false;
        // rule 7: first car with metric > 3.0
        c.metric[2] = 3.5f;
        eqi("rule 7 first crosser car2 (3)", EvaluateResult(7, c, p, &p0), 3);
        c.metric[0] = 3.5f;                   // car 0 checked first
        eqi("rule 7 car0 preferred (1)", EvaluateResult(7, c, p, &p0), 1);
        eqb("rule 7 p0Won", p0, true);
    }
    {
        Cars c = mkCars(); Persist p; bool p0 = false;
        // rule 4: winner = finishOrder[0] + 1 (0 finishers -> -1+1 = 0 -> next
        // round via the `if (iVar6==0) return 0` tail)
        eqi("rule 4 no finisher -> 0", EvaluateResult(4, c, p, &p0), 0);
        p.finishOrder[0] = 1.f;
        eqi("rule 4 first finisher car1 (2)", EvaluateResult(4, c, p, &p0), 2);
        // rule 8: car0-first -> 1; someone else first -> that car; none -> -1
        Persist p8; p8.finishOrder[0] = 0.f;
        eqi("rule 8 car0 first -> 1", EvaluateResult(8, c, p8, &p0), 1);
        p8.finishOrder[0] = 2.f;
        eqi("rule 8 car2 first -> 3", EvaluateResult(8, c, p8, &p0), 3);
        Persist p8n;
        eqi("rule 8 nobody finished -> -1", EvaluateResult(8, c, p8n, &p0), -1);
    }
    {
        Cars c = mkCars(); Persist p; bool p0 = false;
        // rule 9: any listed finisher -> -1; gap >= 0.9 -> -1; else car0 wins
        eqi("rule 9 close race -> car0 wins (1)", EvaluateResult(9, c, p, &p0), 1);
        eqb("rule 9 p0Won", p0, true);
        c.metric[1] = 1.0f;                   // car1 leads car0 by 1.0 >= 0.9
        eqi("rule 9 gap 1.0 -> -1", EvaluateResult(9, c, p, &p0), -1);
        c.metric[1] = 0.f; p.finishOrder[0] = 2.f;
        eqi("rule 9 finisher listed -> -1", EvaluateResult(9, c, p, &p0), -1);
        Persist p9; c.motion0 = 2;
        eqi("rule 9 motion0 -> -1", EvaluateResult(9, c, p9, &p0), -1);
    }
    {
        Cars c = mkCars(); Persist p; bool p0 = false;
        // rule 10: metric0 >= 2.0 -> car0 wins; timer running + below -> 0;
        // timer expired + below -> -1
        c.metric[0] = 2.0f;
        eqi("rule 10 metric 2.0 -> car0 wins (1)", EvaluateResult(10, c, p, &p0), 1);
        c.metric[0] = 1.5f; p.timer = 5.f;
        eqi("rule 10 timer running -> 0", EvaluateResult(10, c, p, &p0), 0);
        p.timer = -0.5f;
        eqi("rule 10 timer expired -> -1", EvaluateResult(10, c, p, &p0), -1);
    }

    std::printf("RuleEngine — FUN_004046a0 rule-10 seeds\n");
    {
        float b = -1.f;
        eqf("default course seed 30.0", Rule10Seed(0, 0.f, &b), 30.f);
        eqf("default course bonus 0", b, 0.f);
        eqf("course 0xb seed 3+14.5", Rule10Seed(0xb, 0.f, &b), 17.5f);
        eqf("course 0xb bonus 0.5+4.15", b, 4.65f);
        eqf("course 0x1a seed 3+17", Rule10Seed(0x1a, 0.f, &b), 20.f);
        eqf("course 0x27 seed 3+16", Rule10Seed(0x27, 0.f, &b), 19.f);
        // difficulty 10 (clamp ceiling _DAT_005cc55c): f = 0
        eqf("course 0xb seed at difficulty 10", Rule10Seed(0xb, 10.f, &b), 14.5f);
    }

    std::printf("\n%s (%d failures)\n", g_fail ? "FAILED" : "ALL PASS", g_fail);
    return g_fail ? 1 : 0;
}
