// Mashed RE - FUN_00410510 (0x00410510): the match-end result evaluator, ONE body for both targets.
//
// D4-DC-0510 (2026-10-25, ROADMAP D4, DECISIONS W0-11). The RVA had two bodies: Race/RuleEngine.cpp:EvaluateResult (exe,
// over the host-side Cars/Persist snapshot, end block left to the race layer) and Race/ScoringHooks.cpp:EvalResult_00410510
// (.asi, installed, live callees and the full end block). They ran the same control flow from two data sources. The control
// flow is EvaluateResultCore (EvalResultCore.cpp). Each target supplies an EvalResultInputs:
//   inputs      the .asi reads the original's callees and globals live, in the original's call order; the exe reads its
//               per-tick snapshot (RuleEngine.cpp:SnapshotInputs).
//   side effects the three original writes and calls outside the decision (DAT_0063b90c = 0 at entry, the DAT_007f0fcc
//               writes inside cases 9/10, the LAB_0041062a end block) go through OnEnter / OnPlayer0Flag / OnConclude.
//               The .asi performs them. The exe leaves them to the race layer (TrackRenderer reads the return value and
//               p0Won), as it did before the merge.
// Rule-7 split kept as an input: Rule7Keep() = true is the original (0x0041075a..0x00410798 leave esi alone on the
// no-match exit 0x00410790). The exe passes Persist::rule7KeepWinner (r7keep, TrackRenderer ON by default).
#pragma once

#include "RuleEngine.h"   // Rule7Trace

namespace mashed_re {
namespace Race {

struct EvalResultInputs {
    virtual int   RaceType() = 0;            // FUN_0042f6a0()             (== 2 -> return 0)
    virtual int   Rule() = 0;                // DAT_007f0fd0
    virtual int   Participants() = 0;        // DAT_008a94d0
    virtual int   Teams() = 0;               // FUN_0042f500()
    virtual int   Score(int car) = 0;        // FUN_0040b6d0(car)
    virtual int   SlotActive(int car) = 0;   // *(*(int*)0x005f2770 + 0x34 + car*4)
    virtual int   Alive(int car) = 0;        // FUN_0046c7b0(car)
    virtual int   FinishSlot(int slot) = 0;  // FUN_00417740(slot)
    virtual int   CollectDone() = 0;         // FUN_00405890()
    virtual int   Rule7Count() = 0;          // FUN_0040e340()             (rule-7 loop bound, re-read every pass)
    virtual float Metric(int car) = 0;       // FUN_00417730(car)
    virtual int   Motion(int car) = 0;       // FUN_0046cbb0(car, &out1, buf) -> out1
    virtual float Timer() = 0;               // DAT_007f0fe4
    virtual float TimerZero() = 0;           // DAT_005d757c
    virtual float FinishMetric() = 0;        // _DAT_005cc31c (3.0f)
    virtual float Rule9Gap() = 0;            // _DAT_005cc9c8 (0.9f)
    virtual float Rule10Metric() = 0;        // _DAT_005cc574 (2.0f)
    virtual bool  Rule7Keep() = 0;           // true = the original
    virtual void  OnEnter() = 0;             // 0x00410510 head: DAT_0063b90c = 0
    virtual void  OnPlayer0Flag(int v) = 0;  // DAT_007f0fcc = v inside cases 9 / 10 (before the end block)
    virtual void  OnConclude(int winner) = 0;  // LAB_0041062a end block, winner != 0
};

// Returns winner (1-based) / -1 ended with no winner / 0 no conclusion (OnConclude not called). r7 (optional) is filled
// for rule 7 only.
int EvaluateResultCore(EvalResultInputs& in, RuleEngine::Rule7Trace* r7);

}  // namespace Race
}  // namespace mashed_re
