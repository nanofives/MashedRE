// RuleEngine — see RuleEngine.h for sources/citations. Transcriptions follow
// the pool13 decomp literally; every branch cites its original address range.
#include "RuleEngine.h"
#include "CollectDone.h"
#include "EvalResultCore.h"

namespace mashed_re {
namespace Race {
namespace RuleEngine {

// FUN_00405890 over the host-side Persist struct (dualcopy4: the predicate itself is the one body,
// CollectDone.h:CollectDoneBody405890, `if (DAT_0063a5d0 == 0) return false; return DAT_0063a5d4 == DAT_0063a5d0;`;
// CollectDone.cpp:Pred405890 reads the two globals, this reads Persist).
bool CollectAllDone(const Persist& p) {
    return CollectDoneBody405890(static_cast<std::uint32_t>(p.collectTotal),
                                 static_cast<std::uint32_t>(p.collectDone)) != 0u;
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

// FUN_00410510 over the host-side snapshot (D4-DC-0510: the control flow is the one body in EvalResultCore.cpp, shared with
// the .asi's installed hook; this reader feeds it Cars/Persist). The end block's side effects (FUN_0045bed0/FUN_0045bf30
// powerup teardown, FUN_00458f80, DAT_0063ba8c=0xb, FUN_00448700 result set, course-0x26 FUN_0044df80) and DAT_0063b90c
// stay the race layer's job, so OnEnter / OnConclude are empty here and OnPlayer0Flag is folded into *p0Won.
namespace {
struct SnapshotInputs final : EvalResultInputs {
    int rule; const Cars& c; const Persist& p;
    SnapshotInputs(int r, const Cars& cars, const Persist& per) : rule(r), c(cars), p(per) {}
    int   RaceType() override { return p.timeAttack ? 2 : 0; }
    int   Rule() override { return rule; }
    int   Participants() override { return c.participants; }
    int   Teams() override { return p.teams ? 1 : 0; }
    int   Score(int car) override { return c.score[car]; }
    int   SlotActive(int car) override { return c.active[car] ? 1 : 0; }
    int   Alive(int car) override { return c.alive[car] ? 1 : 0; }
    int   FinishSlot(int slot) override { return FinishSlotInt(p, slot); }
    int   CollectDone() override { return CollectAllDone(p) ? 1 : 0; }
    int   Rule7Count() override { return c.participants; }
    float Metric(int car) override { return c.metric[car]; }
    int   Motion(int) override { return c.motion0; }      // only car 0 is read (cases 9 / 10)
    float Timer() override { return p.timer; }
    float TimerZero() override { return 0.f; }            // DAT_005d757c = 0
    float FinishMetric() override { return kFinishMetric; }
    float Rule9Gap() override { return kRule9Gap; }
    float Rule10Metric() override { return kRule10Metric; }
    bool  Rule7Keep() override { return p.rule7KeepWinner; }
    void  OnEnter() override {}
    void  OnPlayer0Flag(int) override {}
    void  OnConclude(int) override {}
};
}  // namespace

int EvaluateResult(int rule, const Cars& c, const Persist& p, bool* p0Won, Rule7Trace* r7) {
    SnapshotInputs in(rule, c, p);
    const int winner = EvaluateResultCore(in, r7);
    // DAT_007f0fcc: winner == -1 -> 0, else (winner - 1 == 0); the case-9/10 LAB_004107e3 writes are winner == 1 too.
    if (p0Won) *p0Won = (winner == 1);
    return winner;
}

}  // namespace RuleEngine
}  // namespace Race
}  // namespace mashed_re
