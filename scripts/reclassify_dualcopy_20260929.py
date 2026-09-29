"""re-classify transaction: demote the 2026-09-29 dual-copy rows.

User decision 2026-09-29 on `re/analysis/DUAL_COPY_AUDIT_2026-09-29.md`: demote
every C3/C4 row whose shipping-exe copy differs in behaviour from the verified
`.asi` copy, to the level the EXE copy's own evidence supports.

Gate applied per row (`re/CONFIDENCE.md`, "Which copy the evidence covers",
added 2026-09-29):

  * the C3 gate ("hooked through RH_ScopedInstall and runtime-toggleable") and
    the C4 gate ("a clean diff-original Frida CSV") are both defined over the
    `.asi`. In `mashed_re.exe` `HookSystem::Register` is empty
    (`Stubs/HookSystemNoOp.cpp:19`), so neither says anything about the exe.
  * every row below was RE-VERIFIED against the tree at HEAD before demotion --
    three exe copies were fixed after the audit and those fixes are recorded, not
    ignored.
  * a copy fixed BY READING has been re-verified by reading. That is a C2-grade
    statement about the exe copy: the decomp was read and the body matches, but
    nothing measured it. So it lands at C2 too.

Target level is C2 for every row here: each exe copy's decomp has been read and
transcribed (that is what the audit and the HEAD re-verification did), and none
has evidence of its own. No row drops below C2 -- the analysis work is real.

Mechanics: raw-line edit, never a `csv.writer` round-trip of the whole file (the
project convention -- it corrupts existing quoting). Appends a dated clause to
`notes` and writes the CHANGELOG entry separately.

  py -3.12 scripts/reclassify_dualcopy_20260929.py --check
  py -3.12 scripts/reclassify_dualcopy_20260929.py
"""
from __future__ import annotations

import csv
import io
import pathlib
import shutil
import sys

ROOT = pathlib.Path(__file__).resolve().parent.parent
CSV_PATH = ROOT / "hooks.csv"
BACKUP = ROOT / "log" / "hooks.csv.pre-dualcopy-demote.bak"

AUDIT = "re/analysis/DUAL_COPY_AUDIT_2026-09-29.md"
FIXNOTE = "re/analysis/DUAL_COPY_FIX_2026-09-29.md"

# rva -> (new_confidence, reason). Reason is appended verbatim to `notes`.
# Every reason states a difference RE-CONFIRMED in the tree at HEAD on
# 2026-09-29, citing file:line -- not a difference quoted from the audit.
DEMOTIONS: dict[str, tuple[str, str]] = {

    # --- physics A-chain (default race path) ---------------------------------
    "00468980": ("C2",
        "exe rotation-apply is DEAD: Vehicle/VehicleControl.cpp:195 still passes "
        "nullptr and both guards (Vehicle/AeroStabilize.cpp:74, :91) fail, so "
        "airborne auto-level and the velocity-align rotation never execute. "
        "AND the .asi C4 forwarder is itself in question: the original sets A6b's "
        "ESI from a stack slot at 0x0047093b, contradicting "
        "Vehicle/PhysicsChainHooks.cpp:220 `mov esi, ecx` -- U-9149. NEITHER copy "
        "is established, so the C4 cannot stand for either."),

    "0046b540": ("C2",
        "output stride FIXED to 0x40 (Vehicle/VehicleInit.cpp:145, :153, 3e4fba77) "
        "but the exe copy still differs: the handling-override table is a 1-entry "
        "stub carrying an open marker ([UNCERTAIN U-A3-TABLE], VehicleInit.cpp:36) "
        "against the .asi's live walk of 0x00613148; +0x170 is the decimal "
        "0.397094f (:94) where its mirror at :95 uses exact bits 0x3ecb4fe8; and "
        "the four handling globals 0x00613108/14/30/3c are never written. The "
        "stride fix was verified by READING the original's disassembly, which is "
        "C2-grade evidence for the exe copy."),

    "0046ddb0": ("C2",
        "all EIGHT .rdata constants FIXED to exact bits (Vehicle/ForceIntegrator.h:"
        "41-57 asFb(...), 3e4fba77) but the exe copy still differs: the added "
        "std::memset(m,0,sizeof m) at Vehicle/ForceIntegrator.cpp:73 has no "
        "counterpart in the .asi body, and the TriangleFaceNormal operand order at "
        ":214 is still unresolved (Collision/CarWorldContacts.cpp:46 and "
        "Vehicle/PhysicsChainHooks.cpp:235 state different ABIs for the same "
        "callee). Constants fixed by READING .rdata = C2-grade for the exe copy."),

    "00467650": ("C2",
        "boost block restored and the gear-index clamp bound FIXED to 5 "
        "(Vehicle/Integrate2.cpp:141 local_54[5], :165 gear < 5, 3e4fba77) but the "
        "exe copy still differs: the +0x478 gear-column read is CLAMPED at :206 "
        "where the .asi is unclamped (Vehicle/PhysicsChainHooks.cpp:1902 "
        "Fb(v, 0x478 + gear*4)). Fixed by reading = C2-grade for the exe copy."),

    "00470670": ("C2",
        "the car-index fix landed, and the body math is otherwise term-for-term "
        "identical, but this RVA is the SOLE caller of A6b and still hardcodes its "
        "orient argument to nullptr (Vehicle/VehicleControl.cpp:195) -- the A6b "
        "defect lives at this call site."),

    # --- AI (D3's active phase) ----------------------------------------------
    "004177b0": ("C2",
        "two exe copies and one is fed from nothing: Race/RuleEngine.cpp:16 is the "
        "finish-order fragment only, while AiStandalone's AiPreTickRubberBand reads "
        "0x008a9648 / 0x008a96ec, which no exe TU writes at runtime, so it can never "
        "record a finisher. Absent from BOTH exe copies: the metric write, mode-9 / "
        "mode-4 speed scaling, powerup speed doubling and the entire DAT_0089a368 "
        "state machine the .asi body (Ai/AiPreTick.cpp) has."),

    "00416a30": ("C2",
        "wrong input quantity, unchanged at HEAD: Ai/AiStandalone.cpp:981 takes the "
        "steer magnitude from own_vel_xz where the .asi uses two distinct callees, "
        "and rate1 is pinned 0.0f at :983 ([U-C-RATE1]), which makes the brake gate "
        "permanently false and fires the curvature multiplier unconditionally at "
        ":1002 / :1023. This is the AI steering output and D3 criterion (b) is the "
        "phase's sole open blocker."),

    "00417da0": ("C2",
        "same defect as 0x00416a30 in the M49 copy: Ai/AiStandalone.cpp:1100 takes "
        "the magnitude from own_vel_xz and rate1 is pinned 0.0f at :1102, so the "
        "curvature multiplier at :1121 / :1142 fires unconditionally. M49 also drops "
        "the DAT_0089a368 == 1 debug-accel override the .asi has."),

    "00415e20": ("C2",
        "the copy M49/M8 call takes the heading from VELOCITY, not the body forward "
        "vector: Ai/AiStandalone.cpp:175 still uses own_vel_xz while its sibling "
        "SteerAngleErrorFwd at :215 uses own_fwd_xz, contradicting the file's own "
        "2026-09-26 re-read that 0x0046d510 returns body forward. Wrap polarity also "
        "disagrees at exactly 0, and the scale is the hardcoded 57.2957802 at :146 "
        "where the .asi reads F64(0x005cc970) live."),

    "00416250": ("C2",
        "`int mode = 0;` at Ai/AiStandalone.cpp:844 makes the whole targeting chain "
        "unreachable, so modes 1,2,3,5,7,8,9,10 never run. Two early returns are "
        "missing and the mode-2 dot product zeroes its y term."),

    "00418560": ("C2",
        "three whole arms missing from the exe copy (Ai/AiStandalone.cpp:1600) vs "
        "Ai/AiController.cpp:163 -- the gameMode == 5 startup-countdown hold, the "
        "debug-spline override and the mode-8 track-0x21 brake clear -- plus the "
        "spline-index reset on every short bank, so the index can stay > 0 pointing "
        "at an unusable bank."),

    "00418860": ("C2",
        "the DAT_007f0fd0 == 7 force-step of vehicle 0 is missing from the exe copy "
        "(Ai/AiStandalone.cpp:1682): CarSlotStateSet returns early when 0x005f2770 "
        "is 0, so the slot write never happens. The .asi ALSO carries a second body "
        "for this RVA whose install is commented out."),

    "00443080": ("C2",
        "the exe has NO port: D3d9Render/TrackRenderer.cpp:93 is a host callback "
        "`int aib_ai_target_enable() { return 0; }`, a literal, against the .asi's "
        "`return *(uint32*)0x00897ffc`. The literal IS measured against the original "
        "and the measurement holds -- tgt_7ffc is 0 on 6259/6259 AI steps of "
        "verify/d3_ai_20260926/o_inputs.msd.aistep.csv, re-counted 2026-09-29 -- but "
        "that establishes agreement WITH A CONSTANT on one captured race, not a "
        "reimplementation of a global read. The C3 gate requires a reimplementation; "
        "a measured stub is C2."),

    # --- race / scoring / render ---------------------------------------------
    "0040b290": ("C2",
        "exe ScoreAward (D3d9Render/TrackRenderer.cpp:3980, comment still scopes "
        "itself to `standard path, mode 0 / no teams`) keeps only the prev-snapshot "
        "/ delta / 6000 ms timer / floor-at-0 tail. Missing: the mode-1 gate, the "
        "mode-2 gate, the network clamp and the entire event-ring write that the C4 "
        "body Race/ScoringHooks.cpp:119-161 has."),

    "0040eee0": ("C2",
        "the exe implements only the Participants()==4 arm "
        "(D3d9Render/TrackRenderer.cpp:4164 / :4080). Missing vs "
        "Race/ScoringHooks.cpp:203: the DAT_008a94d0 == 2 arm, the whole == 3 arm "
        "(FFA and team), the FFA 3-alive and 2-alive blocks, and the FFA 1-alive "
        "progress-equalise. 2- and 3-participant matches score nothing."),

    "00410510": ("C2",
        "case 7's loop bound is c.participants (Race/RuleEngine.cpp:88, :92) where "
        "the .asi re-reads FUN_0040e340() -- two different globals. Every "
        "LAB_0041062a global side effect is absent from the exe, and :226 adds a "
        "`won0 ||` disjunct with no counterpart."),

    "00408ad0": ("C2",
        "different expression entirely: the exe computes fmod(progress,n)/n*100 "
        "(D3d9Render/TrackRenderer.cpp:4007) against the .asi leaf's "
        "`return *(float*)(0x008a96ec + v*0x30c)`. Frontend/SmallLeaves_t1.cpp is in "
        "BOTH source lists, so the exe also links a dead export reading an address "
        "no exe TU writes."),

    "0045baa0": ("C2",
        "different RETURN CONTRACT: Powerup/PowerupSystem.cpp:94 returns a 0-based "
        "index and -1 on miss (:97) where the original / .asi returns the entry "
        "pointer or 0. Count is the compile-time 9 in the exe where the original "
        "reads *(int*)0x005f9bd8. The .asi installs a register-ABI shim, not the C "
        "body."),

    "004b4650": ("C2",
        "three implementations, none bit-identical. The exe copy "
        "(Powerup/PowerupContact.cpp:173) keeps the diffs in locals and emits a "
        "`Log(0x004b4650, ...)` call at :179 that the original has not; the "
        "installed .asi body is the naked x87 Lerp4b4650; a third copy in "
        "Render/PromoLoop_round22.cpp is not installed."),

    "004cbd30": ("C2",
        "the exe handles ONLY stream type 3 (D3d9Render/RwWorldStream.cpp:56 "
        "`if (p[0] != 3) return 0;`) where the .asi dispatches four cases including "
        "the fread and callback paths. The exe also emits an error on short read "
        "that the .asi does not."),

    "004cc050": ("C2",
        "on a failed type-3 skip the exe WRITES the position "
        "(D3d9Render/RwWorldStream.cpp:87 `p[3] = p[4];`) where the .asi returns "
        "without touching it. The exe adds an n == 0 early-out and omits the file "
        "and callback cases (:82)."),

    "004c39b0": ("C2",
        "Race/RaceCamera.cpp:55 Vec3Norm is a private approximation of the "
        "C4-verified RW body: std::sqrt + divide, and on ZERO magnitude it copies "
        "the input through (:60) where Math/RwV3dNormalize.cpp leaves scale 0 and "
        "has a degenerate-magnitude error path. Both TUs link into the exe, so the "
        "camera path uses the approximation while everything else uses the verified "
        "body. The camera is default-build."),

    "004c4d20": ("C2",
        "Race/RaceCamera.cpp:66 RotateAboutAxis is a different function shape "
        "(rotates a vector, no RwMatrix) with different numerics: CRT cos/sin (:70) "
        "and std::sqrt normalise vs the .asi's inline FSIN/FCOS and FastInvSqrt, and "
        "deg->rad is the decimal 0.01745329252f (:69) vs the bit pattern 0x3c8efa35. "
        "Both TUs link into the exe."),

    "004a2c48": ("C2",
        "the exe copy is `static_cast<int>(v)` truncation "
        "(Race/RaceCamera.cpp:43-44) where the installed body Math/FPURound.cpp:63 "
        "is the verbatim naked x87 __ftol with its residual correction. "
        "FPURound.cpp's own comment names the RaceCamera copy an approximation."),

    # 0x0042d3e0 DELIBERATELY NOT DEMOTED. The audit filed it DIFFERS-BEHAVIOUR,
    # but the 2026-09-29 re-read (twice, independently) says NOT-A-PAIR: the two
    # exe bodies act on DISJOINT memory and are both reached on purpose --
    # Frontend/MenuInit.cpp:73 MenuEntryArrayInit writes 14 selected offsets over
    # 0x00898ac4.. (skipping +28 at :100), Frontend/MenuNavSM.cpp:386 RecordsZero
    # memsets the standalone's own g_records and zeroes g_record_count at :388.
    # They are not two implementations of one function, so the dual-copy demotion
    # rule does not apply and the C3 stands. What IS wrong is the RVA comment on
    # RecordsZero claiming an RVA it does not implement -- filed separately.

    "0042fa00": ("C2",
        "guard differs (Frontend/MenuNav.cpp:251 `*team != 0` vs "
        "Frontend/SkeletonAndScatter_t6.cpp:270 `*piVar5 > 0`) and the .asi adds a "
        "trailing clamp the exe lacks. MenuNav.cpp:257 records that the exe copy is "
        "DELIBERATELY NOT INSTALLED and MenuNav.cpp:272-273 argues the EXE copy is "
        "the faithful one -- so the C3 is attached to the copy its own source says "
        "is the less faithful of the two."),
}


def parse(line: str) -> list[str]:
    return next(csv.reader([line.rstrip("\n")]))


def emit(fields: list[str]) -> str:
    b = io.StringIO()
    csv.writer(b, lineterminator="").writerow(fields)
    return b.getvalue()


def main(argv) -> int:
    check = "--check" in argv
    if not DEMOTIONS:
        print("FATAL: DEMOTIONS is empty")
        return 1

    lines = CSV_PATH.read_text(encoding="utf-8").split("\n")
    cols = parse(lines[0])
    ci, ni = cols.index("confidence"), cols.index("notes")
    ncols = len(cols)

    seen, out, changed = set(), [], []
    for i, line in enumerate(lines):
        if i == 0 or not line.strip() or line.startswith("#"):
            out.append(line)
            continue
        f = parse(line)
        if len(f) != ncols:
            print(f"FATAL: line {i+1} has {len(f)} fields, expected {ncols}")
            return 1
        key = f[0].strip().lower()
        if key not in DEMOTIONS:
            out.append(line)
            continue
        if key in seen:
            print(f"FATAL: duplicate row for {key} at line {i+1}")
            return 1
        seen.add(key)
        new_c, reason = DEMOTIONS[key]
        old_c = f[ci]
        if old_c == new_c:
            print(f"  SKIP {key}: already {new_c} (idempotent)")
            out.append(line)
            continue
        if old_c not in ("C3", "C4"):
            print(f"FATAL: {key} is {old_c!r}, expected C3 or C4 -- refusing")
            return 1
        f[ci] = new_c
        note = (f" | DEMOTED {old_c}->{new_c} 2026-09-29 dual-copy: {reason} "
                f"Evidence for {old_c} measured the `file` copy (.asi) only; the "
                f"shipping exe copy in `exe_file` has no evidence of its own. "
                f"Audit {AUDIT}; decision + re-verification {FIXNOTE}.")
        f[ni] = (f[ni] + note) if f[ni] else note.lstrip(" |").strip()
        out.append(emit(f))
        changed.append((key, old_c, new_c))

    missing = set(DEMOTIONS) - seen
    if missing:
        print(f"FATAL: {len(missing)} target RVA(s) not found in hooks.csv: "
              f"{sorted(missing)}")
        return 1

    from collections import Counter
    tally = Counter(f"{o}->{n}" for _, o, n in changed)
    print(f"{len(changed)} row(s) demoted: "
          + ", ".join(f"{k} x{v}" for k, v in sorted(tally.items())))
    for key, o, n in changed:
        print(f"  {key}  {o} -> {n}")
    if check:
        print("--check: not written")
        return 0
    BACKUP.parent.mkdir(parents=True, exist_ok=True)
    shutil.copy2(CSV_PATH, BACKUP)
    CSV_PATH.write_text("\n".join(out), encoding="utf-8", newline="")
    print(f"written; backup at {BACKUP.relative_to(ROOT)}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main(sys.argv[1:]))
