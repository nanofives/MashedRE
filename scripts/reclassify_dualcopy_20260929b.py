"""re-classify transaction 2: the UNREVIEWED worklist + one relabelled row.

Same user decision and the same gate as `scripts/reclassify_dualcopy_20260929.py`
(read its header). This pass covers the **53 UNREVIEWED** structural candidates in
`re/analysis/dual_copy_audit_2026-09-29.csv`, which the audit listed as a worklist
rather than scoring, plus one row the audit scored DIFFERS-COSMETIC whose own text
says the output differs.

The 53, classified 2026-09-29 (four parallel read-only passes, then each
DIFFERS-BEHAVIOUR row re-verified in-session before it was touched):

  NO-EXE-COPY        21   the exe has no body at all -- the C3/C4 says nothing
                          about the default build, but nothing is WRONG either
  NOT-A-PAIR         20   trampoline / extern fn-pointer / comment-only mention /
                          A/B harness twin / complementary fragments
  BOTH-SHARED         8   both TUs are in BOTH rsp lists, so there is no
                          per-target divergence (a DUP-IN-TARGET, not a dual copy)
  DIFFERS-BEHAVIOUR   3   demoted here
  DIFFERS-COSMETIC    1   0x00417180, not demoted (see NOT-DEMOTED below)

NOT DEMOTED, recorded so the reasoning is not lost:
  0x00417180  DIFFERS-COSMETIC. The exe rolls its variety value with `RandUnit()`
              (an LCG stand-in, `Ai/AiStandalone.cpp:1324`) where the .asi calls
              the real `RandFloat(0x3f800000)` (`Ai/AiPreTick.cpp:154`). Branch
              structure and offsets agree. `AiStandalone.cpp:1242` asserts this
              "does not affect steer/accel/brake output" -- that is an IN-FILE
              CLAIM, not a measurement, and it is the only thing holding the row.
              Left at C3 rather than demoted on an unmeasured claim in either
              direction; flagged in DUAL_COPY_FIX_2026-09-29.md instead.
"""
from __future__ import annotations

import sys
import pathlib

sys.path.insert(0, str(pathlib.Path(__file__).resolve().parent))
import reclassify_dualcopy_20260929 as T   # reuse parse/emit/main and the gate

T.DEMOTIONS = {

    # --- from the 53 UNREVIEWED, all three re-verified in-session -------------
    "00417640": ("C2",
        "exe pins the brake rate: `const float rate = 0.0f;` "
        "(Ai/AiStandalone.cpp:1363, [U-C-RATE1]) makes the gate "
        "`kPowerupBrakeRateGate < rate` provably always false, so the exe copy "
        "always falls through to `ctrl[4]=0; ctrl[5]=0` (coast). The .asi copy "
        "calls the real `call_0046d6d0(&local_14, param_1)` (Ai/AiController.cpp:131) "
        "and CAN apply full brake `param_2[5]=0xff` (:141). Shipping consequence: "
        "opponent AI on track index 0x21 never executes the powerup-brake "
        "full-stop override. Same rate1-pinning family as the AI rows demoted in "
        "the first transaction."),

    "0042d5a0": ("C2",
        "the exe has NO port: `exe_main.cpp:8351` installs a thunk to "
        "`Standalone_CreditsNoOp` for this RVA, where the .asi installs the full "
        "`MenusBodyA` credits sprite-timeline renderer (Frontend/MenuMixed.cpp:143, "
        "766 bytes of original at 0x0042d5a0..0x0042e39b). The credits screen "
        "renders nothing in the shipping build."),

    "00497450": ("C2",
        "different DATA SOURCE: the exe returns "
        "`g_game_state.player_active[player]`, a standalone placeholder array "
        "(Frontend/MenuNavSM.cpp:462), where the byte-verified .asi copy reads "
        "`*(u32*)(0x007e96fc + i*0x200)` (Util/PromoLoop_sessionB.cpp:231). The "
        "exe copy's own comment at :459 also mis-states the stride as `*0x80` "
        "against the .asi's byte-verified `0x200`. Drives the screen-0x1c "
        "grey-out predicate."),

    # --- relabelled from the audit's DIFFERS-COSMETIC ------------------------
    "0040e180": ("C2",
        "RELABELLED from the audit's DIFFERS-COSMETIC on the audit's own text "
        "(\"cosmetic in structure but not in output\"). The exe copy takes the "
        "pair magnitude with `std::sqrt` (Race/RaceCamera.cpp:50, called at :194) "
        "where the original and the C4-verified .asi copy forward to the RW "
        "fast-sqrt LUT at 0x004c3ac0 (Race/CameraClusterHooks.cpp:38). That "
        "magnitude is the operand of the `best <= m` comparison at "
        "RaceCamera.cpp:196 that CHOOSES the pair, so an approximation difference "
        "flips near-ties. The exe copy's own comment records 7.8% of 766 captured "
        "frames choosing a genuinely different pair from the original "
        "(RaceCamera.cpp:225-233). [UNCERTAIN] that comment names a DIFFERENT "
        "leading suspect for the 7.8% -- the offline driver's `active` derivation, "
        "untested -- so the cause is not established; what IS established is that "
        "the shipping copy is a different implementation and disagrees with the "
        "original on 7.8% of frames. Default-build camera path."),
}

if __name__ == "__main__":
    raise SystemExit(T.main(sys.argv[1:]))
