// RuleEngine — standalone port of Mashed's per-rule race win-condition engine
// (WS-G rules debt, D-11052, 2026-07-02). Pure logic, no MASHED globals: the
// race layer feeds a per-tick snapshot of its own state and keeps a small
// persistent block between ticks.
//
// Sources (all decompiled read-only from Mashed_pool13, session 2026-07-02;
// binary anchor MASHED.exe SHA-256 unpatched
//   BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E):
//
//   FUN_00410d10 (0x00410d10)  per-tick segment checker: per-rule pre-blocks
//                              (cases 4/5/7/8/9/10) + trailing-car elimination
//                              + last-man/alive-count logic. Returns 1 when the
//                              race segment (round) is over.
//   FUN_00410510 (0x00410510)  result evaluator, called ONLY when the segment
//                              checker returned 1 (caller FUN_00411170,
//                              0x00411170: `if (FUN_00410d10()==0) return;
//                              iVar1=FUN_00410510(); if (iVar1!=0) results
//                              else DAT_0063ba8c=7 (next round)`).
//                              Returns winner (1-based) / -1 draw / 0 pending.
//   FUN_004177b0 (0x004177b0)  per-tick metric + finish-order updater:
//                              metric[car] = int(lapCounter) + frac*0.01 and,
//                              for rules {4,7,8,9}, appends finishers
//                              (metric >= 3.0) to the 4-slot order list.
//   FUN_004046a0 (0x004046a0)  rule-10 countdown seed + per-lap-line
//                              checkpoint-time table.
//   FUN_004039f0 (0x004039f0)  rule-10 per-tick countdown + checkpoint bonus.
//   FUN_00414060 (0x00414060)  race-init: finish-order slots = -1.0f.
//   FUN_00405890 (0x00405890)  rule-5 win predicate: collectTotal != 0 &&
//                              collectDone == collectTotal (DAT_0063a5d0/
//                              DAT_0063a5d4; registrar FUN_00405780, collect
//                              increment FUN_004064c0).
//
// Constants are byte-verified .rdata reads (memory_read, 2026-07-02):
//   0x005cc31c = 3.0f   finish metric threshold (the "lap target" — LAPDATA.LUA
//                       carries lap LINES, not counts; see D-11055 resolution)
//   0x005cc574 = 2.0f   rule-10 metric threshold
//   0x005cc9c8 = 0.9f   rule-9 metric gap
//   0x005cc328 = 0.01f  metric fraction multiplier
//   0x005cc33c = -1.0f  finish-order empty-slot sentinel
//   0x005cc740 = 4.15f / 0x005cc744 = 14.5f   rule-10 course 0xb bonus/seed
//   0x005cc748 = 5.8f  / 0x005cc74c = 17.0f   rule-10 course 0x1a bonus/seed
//   0x005cc754 = 3.6f  / 0x005cc750 = 16.0f   rule-10 course 0x27 bonus/seed
//   0x005cc56c = 0.1f   difficulty scale in the rule-10 seed
//   0x005cc320 = 1.0f
//
// Rule objective split (proven, FUN_00410d10 control flow): rules {5,7} skip
// the elimination block entirely; every other rule runs it.
#pragma once
#include <cstdint>

namespace mashed_re {
namespace Race {
namespace RuleEngine {

constexpr int   kCars         = 4;
constexpr float kFinishMetric = 3.0f;    // _DAT_005cc31c
constexpr float kRule10Metric = 2.0f;    // _DAT_005cc574
constexpr float kRule9Gap     = 0.9f;    // _DAT_005cc9c8
constexpr float kMetricFrac   = 0.01f;   // _DAT_005cc328
constexpr float kEmptySlot    = -1.0f;   // _DAT_005cc33c
constexpr float kTimerDefault = 30.0f;   // FUN_004046a0 default branch

// Per-tick snapshot of the race layer's own car state.
struct Cars {
    int   participants = 4;      // DAT_008a94d0 (2..4)
    bool  active[kCars] = {};    // slot-active probe *(PTR_PTR_005f2770+0x34+i*4)
    bool  alive[kCars]  = {};    // vehicle +0x004 kActiveFlag == 1 (FUN_0046c7b0)
    int   score[kCars]  = {};    // DAT_008a94e0 (FUN_0040b6d0)
    float metric[kCars] = {};    // DAT_0089a880: laps + lapFrac% * 0.01
                                 // (FUN_00407a20 + FUN_00408ad0 * _DAT_005cc328)
    int   motion0 = 0;           // vehicle[0] +0x9f0 kMotionState (FUN_0046cbb0 out1)
    int   motion1 = 0;           // vehicle[1] +0x9f0 (rule 9 reads car 1 too)
    int   snapshot1 = 0;         // FUN_00423b20(1): dword 0x008995ec+0x138 [UNCERTAIN
                                 // U: semantics unresolved; original ends the rule-9
                                 // segment when nonzero. Standalone feeds 0.]
};

// Persistent (cross-tick) rule state.
struct Persist {
    float finishOrder[kCars] = { kEmptySlot, kEmptySlot, kEmptySlot, kEmptySlot };
                                 // 0x0089a870..7c; -1.0f = empty (FUN_00414060)
    float timer = kTimerDefault; // DAT_007f0fe4 (rule 10 countdown, seconds)
    int   collectTotal = 0;      // DAT_0063a5d0 (rule 5, registrar FUN_00405780)
    int   collectDone  = 0;      // DAT_0063a5d4 (increment FUN_004064c0)
    bool  teams = false;         // DAT_0067ea64 (FUN_0042f500)
    bool  timeAttack = false;    // FUN_0042f6a0() == 2 (mode-2 early-out)
};

// FUN_00405890 (0x00405890): rule-5 win predicate.
bool CollectAllDone(const Persist& p);

// FUN_004177b0 finish-order fragment (0x004177b0, write sites 0x0089a870..7c):
// for rules {4,7,8,9} append every car whose metric >= 3.0 that is not already
// listed into the first empty (-1.0f) slot. Call once per tick after metrics.
void UpdateFinishOrder(int rule, const Cars& c, Persist& p);

// FUN_004046a0 (0x004046a0) rule-10 countdown seed. courseId = engine
// Course_Id (FUN_00426c00 / DAT_00644158): 0xb -> 3*f+14.5, 0x1a -> 3*f+17.0,
// 0x27 -> 3*f+16.0 where f = 1.0 - difficulty*0.1; anything else -> 30.0.
// checkpointBonus (same courses) = 0.5*f + 4.15 / 5.8 / 3.6; 0 otherwise
// (the original builds no checkpoint table on other courses).
float Rule10Seed(int courseId, float difficulty, float* checkpointBonus);

// FUN_004039f0 (0x004039f0) fragment: countdown tick. dt in seconds
// (DAT_007f0fe4 -= _DAT_007f100c). Checkpoint bonuses are the race layer's
// job (award p.timer += bonus once per lap-line gate per lap, matching the
// [gate, gate+1) window on the car-0 gate-position float).
inline void Rule10Tick(Persist& p, float dt) { p.timer -= dt; }

// FUN_00410d10 (0x00410d10) verbatim skeleton. Out-params tell the race layer
// what the original did around its elimination internals this tick:
//   *runElimination  true  => execute the trailing-car elimination block
//                             (rules other than {5,7}; the original also gates
//                             it on the zoom-sat float FUN_00442df0()==10.0 —
//                             the standalone's RaceCamera EliminationCheck
//                             models that timing).
// Return: 1 = segment over (call EvaluateResult), 0 = race continues.
// `resultDeclared` = FUN_00443080() != 0 (a result was already set): the
// original returns 0 immediately (case FUN_00443080()==1 at 0x00410d10 head).
int SegmentCheck(int rule, const Cars& c, const Persist& p,
                 bool resultDeclared, bool* runElimination);

// FUN_00410510 (0x00410510) verbatim skeleton. Call ONLY when SegmentCheck
// returned 1 (caller contract FUN_00411170). Returns:
//   >0  winner car (1-based)   -> match/race over
//   -1  ended with no winner   -> match/race over (draw / player failed)
//    0  no conclusion          -> next round (DAT_0063ba8c = 7 path)
// *p0Won mirrors DAT_007f0fcc (player-0 won).
int EvaluateResult(int rule, const Cars& c, const Persist& p, bool* p0Won);

}  // namespace RuleEngine
}  // namespace Race
}  // namespace mashed_re
