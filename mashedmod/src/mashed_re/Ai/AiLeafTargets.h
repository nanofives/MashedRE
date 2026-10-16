// Mashed RE — D3 bulk G-AILEAF: FUN_00414570 / FUN_00415880 / FUN_0046d570 verbatim naked x87 (exe-only TU).
// Plan: verify/d3_bulk_20261011/PLAN.md (§0 amendment, §3 rank 2). Spec: re/tools/naked_specs/aileaf_g1.json.
#pragma once

namespace Ai {

// MASHED_AILEAF (default OFF) AND MASHED_PATHPROG (the progress producer): run the two targeting-chain leaves of FUN_00416250 at their call sites
// 0x004162f7 / 0x0041633f instead of the stub (mode 0).
bool AiLeaf_Enabled();

// 0x00414570 FUN_00414570(spline, &look, &cand, v) -> 0 / 1 (mode 1 candidate in cand).
int AiLeaf_414570(unsigned int spline, int v, const float look[2], float cand[2]);
// 0x00415880 FUN_00415880(spline, &look, &cand, v) -> 0 / 1 (mode 2 candidate in cand). Writes the
// original-layout AI slot words 0x0089a508 / 0x0089a4dc / 0x0089a4e0 (+ v*0x74), as the original does.
int AiLeaf_415880(unsigned int spline, int v, const float look[2], float cand[2]);

// 0x0046d570 FUN_0046d570(&out, v) -> out (the "range" float FUN_00417640 compares against 90.0f). Verbatim naked body, 0.0f if the
// record array is not up. Unlike the two leaves above it is NOT behind MASHED_AILEAF: its only caller is the shared brake override.
float AiLeaf_46d570(int v);

// Counters for the step dump / trace: calls and returns per leaf, mode-2 accepts not committed
// because the exe has no FUN_00415d00 body.
void AiLeaf_CountAccept(int leaf, int committed);

// G-AILEAF-REPLAY: MASHED_AILEAF_REPLAY=<rows.tsv> (re/tools/aileaf_unicorn.py) replays every row
// through the naked bodies under the row's x87 control word; report to MASHED_AILEAF_REPLAY_OUT.
// Called once after the course block is built.
void AiLeaf_MaybeReplay();

}  // namespace Ai
