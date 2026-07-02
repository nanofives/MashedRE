// RuleEngine — see RuleEngine.h for sources/citations. Transcriptions follow
// the pool13 decomp literally; every branch cites its original address range.
#include "RuleEngine.h"

namespace mashed_re {
namespace Race {
namespace RuleEngine {

// 0x00405890: `if (DAT_0063a5d0 == 0) return false;
//              return DAT_0063a5d4 == DAT_0063a5d0;`
bool CollectAllDone(const Persist& p) {
    if (p.collectTotal == 0) return false;
    return p.collectDone == p.collectTotal;
}

// 0x004177b0 finish-order fragment. Original loop (cars 0..3):
//   if ((rule==4 || rule==9 || rule==7 || rule==8) && metric >= _DAT_005cc31c)
//     if car not in {0x0089a870..7c}: store into the first slot equal to
//     _DAT_005cc33c (-1.0f). Car indices are stored AS FLOATS.
void UpdateFinishOrder(int rule, const Cars& c, Persist& p) {
    if (rule != 4 && rule != 9 && rule != 7 && rule != 8) return;
    for (int car = 0; car < kCars; ++car) {
        if (c.metric[car] < kFinishMetric) continue;         // >= 3.0 finishes
        const float f = static_cast<float>(car);
        if (p.finishOrder[0] == f || p.finishOrder[1] == f ||
            p.finishOrder[2] == f || p.finishOrder[3] == f) continue;
        for (int s = 0; s < kCars; ++s)
            if (p.finishOrder[s] == kEmptySlot) { p.finishOrder[s] = f; break; }
    }
}

// 0x004046a0. Course ids compared against 0xb / 0x1a / 0x27 (FUN_00426c00 =
// DAT_00644158). f = 1.0 - difficulty*0.1 (_DAT_005cc320, _DAT_005cc56c;
// difficulty = FUN_004173a0 = _DAT_0089a360). Seeds: f*3.0 + 14.5 (0x005cc744)
// / 17.0 (0x005cc74c) / 16.0 (0x005cc750); default branch: 30.0. Checkpoint
// bonus: f*0.5 (_DAT_005cc32c) + 4.15 (0x005cc740) / 5.8 (0x005cc748) /
// 3.6 (0x005cc754); no table is built on other courses.
float Rule10Seed(int courseId, float difficulty, float* checkpointBonus) {
    const float f = 1.0f - difficulty * 0.1f;
    float bonus = 0.f, seed = kTimerDefault;
    switch (courseId) {
    case 0xb:  seed = f * 3.0f + 14.5f; bonus = f * 0.5f + 4.15f; break;
    case 0x1a: seed = f * 3.0f + 17.0f; bonus = f * 0.5f + 5.8f;  break;
    case 0x27: seed = f * 3.0f + 16.0f; bonus = f * 0.5f + 3.6f;  break;
    default: break;                       // 0x004046a0: DAT_007f0fe4 = 30.0
    }
    if (checkpointBonus) *checkpointBonus = bonus;
    return seed;
}

namespace {
// (int)finishOrder[slot] — FUN_00417740: FLD [0x89a870 + i*4]; JMP __ftol.
// __ftol truncates toward zero; slots hold whole car indices or -1.0f.
inline int FinishSlotInt(const Persist& p, int slot) {
    return static_cast<int>(p.finishOrder[slot]);
}
// count of active cars (slot probe 0x34..0x40) and alive among them
inline int CountAlive(const Cars& c) {
    int n = 0;
    for (int i = 0; i < kCars; ++i)
        if (c.active[i] && c.alive[i]) ++n;
    return n;
}
}  // namespace

// 0x00410d10 verbatim skeleton (elimination internals externalized).
int SegmentCheck(int rule, const Cars& c, const Persist& p,
                 bool resultDeclared, bool* runElimination) {
    if (runElimination) *runElimination = false;
    // 0x00410d10 head: `if (FUN_00443080() == 1) return 0;`
    if (resultDeclared) return 0;

    switch (rule) {                                   // switch(DAT_007f0fd0)
    case 4:
        // `fVar10 = FUN_00417730(0); bVar1 = fVar10 < 3.0; if (!bVar1) return 1;`
        if (!(c.metric[0] < kFinishMetric)) return 1;
        break;                                        // -> elimination block
    case 5:
        // `if (FUN_0046c7b0(0) == 0) return 1; return FUN_00405890();`
        if (!c.alive[0]) return 1;
        return CollectAllDone(p) ? 1 : 0;             // NO elimination block
    case 7: {
        // `if (FUN_0046c7b0(0) == 0) return 1;` then loop cars 0..participants:
        // count dead; `if (metric > 3.0) return 1;` finally
        // `return participants-1 <= deadCount;`
        if (!c.alive[0]) return 1;
        int dead = 0;
        for (int i = 0; i < c.participants; ++i) {
            if (!c.alive[i]) ++dead;
            if (kFinishMetric < c.metric[i]) return 1;
        }
        return (c.participants - 1 <= dead) ? 1 : 0;  // NO elimination block
    }
    case 8:
        // `if (metric[0] >= 3.0) return 1; FUN_0046cbb0(0,..); if (state != 0)
        //  return 1;` else fall into the elimination block.
        if (kFinishMetric <= c.metric[0]) return 1;
        if (c.motion0 != 0) return 1;
        break;
    case 9:
        // metric[0] >= 3.0 -> 1; metric[1] >= 3.0 -> 1; motion0 != 0 -> 1;
        // FUN_00423b20(1) != 0 -> 1; else elimination block.
        if (kFinishMetric <= c.metric[0]) return 1;
        if (kFinishMetric <= c.metric[1]) return 1;
        if (c.motion0 != 0) return 1;
        if (c.snapshot1 != 0) return 1;
        break;
    case 10:
        // motion0 != 0 -> 1; timer expired (NaN-aware `(a<0)!=(a==0)`,
        // DAT_007f0fe4 vs DAT_005d757c=0) -> 1; metric[0] >= 2.0 -> 1;
        // else elimination block.
        if (c.motion0 != 0) return 1;
        if ((p.timer < 0.f) != (p.timer == 0.f)) return 1;
        if (!(c.metric[0] < kRule10Metric)) return 1;
        break;
    default:
        break;                                        // rules {0,1,2,3,6}
    }

    // Elimination block (0x00410d10 tail). The trailing-car pick +
    // FUN_0040eee0 scoring live in the race layer (ScoreOnElimination /
    // RaceCamera EliminationCheck — the FUN_00442df0()==10.0 zoom-sat gate).
    if (runElimination) *runElimination = true;

    // Alive-count logic (0x00410d10 tail, after the elimination internals):
    // single active slot: alive==0 -> 1. Multiple: alive==1 -> 1 (last man);
    // alive==0 -> equalize progress + 1 (the FUN_00408a70 writes are the race
    // layer's concern).
    int slots = 0;
    for (int i = 0; i < kCars; ++i)
        if (c.active[i]) ++slots;
    const int alive = CountAlive(c);
    if (slots == 1) {
        if (alive == 0) return 1;
    } else {
        if (alive == 1) return 1;
        if (alive == 0) return 1;
    }
    return 0;
}

// 0x00410510 verbatim skeleton. See header for the caller contract.
int EvaluateResult(int rule, const Cars& c, const Persist& p, bool* p0Won) {
    if (p0Won) *p0Won = false;
    // `if (FUN_0042f6a0() == 2) return 0;` — time-attack handled elsewhere.
    if (p.timeAttack) return 0;

    int winner = 0;                                   // iVar6
    // Score-target scan (0x00410510 first loop, cars 1..4 as 1-based):
    //   participants 2/3 (or teams): score == 8 -> winner
    //   rule == 2:                   score == 8 -> winner
    //   participants 4:              score > 11 -> winner
    for (int i = 1; i <= kCars; ++i) {
        const int sc = c.score[i - 1];                // FUN_0040b6d0(i-1)
        if ((c.participants == 2 || c.participants == 3 || p.teams) && sc == 8)
            winner = i;
        if (rule == 2 && sc == 8) winner = i;
        if (c.participants == 4 && sc > 11) winner = i;
    }
    // Alive refinement (second loop): prefer an active+alive car that meets
    // the same target (slot probe + FUN_0046c7b0(i)==1).
    if (winner != 0) {
        for (int i = 0; i < kCars; ++i) {
            if (!c.active[i] || !c.alive[i]) continue;
            const int sc = c.score[i];
            if ((c.participants == 2 || c.participants == 3 || p.teams) && sc == 8)
                winner = i + 1;
            if (c.participants == 4 && sc > 11) winner = i + 1;
        }
    }

    bool concluded = false;                           // goto LAB_0041062a
    bool won0 = false;                                // DAT_007f0fcc write
    switch (rule) {                                   // switch(DAT_007f0fd0)
    case 4:
        winner = FinishSlotInt(p, 0) + 1;             // first finisher + 1
        break;
    case 5:
        // `if (FUN_0046c7b0(0) == 0) { -1; goto end; } iVar6 = FUN_00405890();`
        if (!c.alive[0]) { winner = -1; concluded = true; break; }
        winner = CollectAllDone(p) ? 1 : 0;
        break;
    case 7:
        // first car (0..participants) with metric > 3.0 -> car+1
        winner = 0;
        for (int i = 0; i < c.participants; ++i)
            if (kFinishMetric < c.metric[i]) { winner = i + 1; break; }
        break;
    case 8:
        winner = FinishSlotInt(p, 0) + 1;
        if (winner != 1) {                            // car 0 not first
            if (winner == 0) winner = -1;             // nobody finished
            concluded = true;
        }
        break;
    case 9:
        // `if (FUN_00417740(0) != -1) { -1; fcc=0; goto end; }` — ANY finisher
        // already listed ends it with no winner; then motion0 / gap checks.
        if (FinishSlotInt(p, 0) != -1) { winner = -1; concluded = true; break; }
        if (c.motion0 != 0)            { winner = -1; concluded = true; break; }
        // `if (0.9 <= metric[1] - metric[0]) { -1; goto end; }` else winner=1,
        // fcc=1 (LAB_004107e3).
        if (kRule9Gap <= c.metric[1] - c.metric[0]) { winner = -1; concluded = true; break; }
        winner = 1; won0 = true; concluded = true;
        break;
    case 10:
        if (c.motion0 != 0) { winner = -1; concluded = true; break; }
        if (c.metric[0] < kRule10Metric) {
            // timer still running (`(a<0)==(a==0)` vs 0) -> return 0
            if ((p.timer < 0.f) == (p.timer == 0.f)) return 0;
            winner = -1; concluded = true; break;     // LAB_00410801
        }
        winner = 1; won0 = true; concluded = true;    // LAB_004107e3
        break;
    default:
        break;                                        // rules {0,1,2,3,6}
    }

    // `if (iVar6 == 0) return 0;` (not concluded, no winner -> next round)
    if (!concluded && winner == 0) return 0;

    // LAB_0041062a end block. The original's side effects (FUN_0045bed0/
    // FUN_0045bf30 powerup teardown, FUN_00458f80, DAT_0063ba8c=0xb,
    // FUN_00448700 result set, course-0x26 FUN_0044df80) are the race layer's
    // job. DAT_007f0fcc: winner==-1 -> 0; else (winner-1 == 0).
    if (p0Won) *p0Won = (winner == -1) ? false : (won0 || winner - 1 == 0);
    return winner;
}

}  // namespace RuleEngine
}  // namespace Race
}  // namespace mashed_re
