"""Part C allowlist transaction (fwd494fd0): remove the 11 AI-area keys that stop being findings, add the 3
that are real pairs under their new, accurate file sets. Dry run by default, `--apply` writes.

    py -3.12 allowlist_edit.py [--apply]

Removed: harness drivers (AbAi*), a CRT-thunk caller, a typedef comment and a prose comment that the scanner had
bound as bodies; two dead bodies deleted (AiVehicleVelocity3, AiPerFrameTick); one pair consolidated (CarSlotStateSet).
Added: the three pairs that survive, now keyed on the files that actually hold the bodies.
"""
import pathlib
import sys

ROOT = pathlib.Path(__file__).resolve().parent.parent.parent
P = ROOT / "re" / "tools" / "dual_copy_allowlist.txt"

REMOVE = [
    "DUP-IN-TARGET:004a2c48:Ai/VehicleFloatFieldAsInt.cpp,Race/RaceCamera.cpp",
    "DUP-IN-TARGET:004177b0:Ai/AiControllerAB.cpp,Ai/AiPreTick.cpp,Core/HookSystem.cpp",
    "DUP-IN-TARGET:0046d510:Ai/AiTargeting.cpp,Ai/VehicleVelocityWorldGet.cpp",
    "DUP-IN-TARGET:004a2c48:Ai/VehicleFloatFieldAsInt.cpp,Frontend/SetupScreenRenderers.cpp",
    "CROSS-TARGET:004177b0:Ai/AiControllerAB.cpp,Ai/AiPreTick.cpp,Core/HookSystem.cpp,Race/RuleEngine.cpp",
    "CROSS-TARGET:004a2c48:Frontend/SetupScreenRenderers.cpp,Race/RaceCamera.cpp",
    "DUP-IN-TARGET:0040e480:Ai/AiStandalone.cpp,Frontend/RaceResults.cpp",
    "DUP-IN-TARGET:00417180:Ai/AiControllerAB.cpp,Ai/AiPreTick.cpp",
    "DUP-IN-TARGET:00417640:Ai/AiController.cpp,Ai/AiControllerAB.cpp,Ai/AiStandalone.cpp",
    "DUP-IN-TARGET:00418560:Ai/AiController.cpp,Ai/AiControllerAB.cpp",
    "DUP-IN-TARGET:00418860:Ai/AiController.cpp,Ai/AiControllerAB.cpp,HUD/ScenarioLeaves_sa2.cpp",
]

ADD = """
# --- ADDED 2026-10-16 (fwd494fd0, ROADMAP D4 item 2, the AI copies). The AI-area keys above went from 11 to the
# three below: harness drivers, call-throughs and prose the scanner had bound as bodies were re-anchored, two dead
# bodies deleted (AiVehicleVelocity3, AiPerFrameTick), CarSlotStateSet consolidated into Ai/CarSlotState.cpp.
# What is left is REAL and none of it is a mechanical merge: the exe body is a different, shorter body. Detail:
# verify/fwd494fd0_20261016/RESULT_fwd494fd0.md section C, and each row's DEMOTED note in hooks.csv.
DUP-IN-TARGET:00417640:Ai/AiController.cpp,Ai/AiStandalone.cpp   # DELIBERATE until D3 criterion (b). asi AiController.cpp:AiPostStepPowerupBrake calls the real FUN_0046d6d0 and CAN apply full brake (ctrl[5]=0xff); exe+asi AiStandalone.cpp:PostStepPowerupBrake pins `rate = 0.0f` ([U-C-RATE1]) so its brake branch is unreachable and it omits FUN_0046d570. Opponent AI on track index 0x21 never full-brakes in the exe. Merge needs the Host to supply both floats (veh_f32(v, 0x9e4) exists for FUN_0046d6d0, nothing for FUN_0046d570, whose port is exe-only in Ai/AiLeafTargets.cpp) and changes exe behaviour on track 0x21, so G-CTRL must be re-run. Row C2 since the 2026-09-29 demotion.
CROSS-TARGET:004177b0:Ai/AiPreTick.cpp,Race/RuleEngine.cpp   # DELIBERATE until D3 criterion (b). exe-only Race/RuleEngine.cpp:UpdateFinishOrder is the finish-order FRAGMENT of FUN_004177b0 (its own comment: `finish-order fragment`); asi-only Ai/AiPreTick.cpp:AiPreTick is the whole function (verbatim, A/B driver AbAiPreTick). The other exe copy, Ai/AiStandalone.cpp:AiPreTickRubberBand, is invisible to the scanner (no RVA comment directly above it) and per the 2026-09-29 demotion lacks the metric write, the mode-9/mode-4 speed scaling, the powerup doubling and the 0x0089a368 state machine, and reads 0x008a9648/0x008a96ec which no exe TU writes. Row C2.
DUP-IN-TARGET:004a2c48:Math/FPURound.cpp,Race/RaceCamera.cpp   # audit=DIFFERS-BEHAVIOUR; kept as the one honest entry for the __ftol family. asi+exe Math/FPURound.cpp:FPURound_4a2c48 is the verbatim naked x87 body (C3, installed in the .asi; in the exe it is compiled but has no caller); exe-only Race/RaceCamera.cpp:Ftol_4a2c48 is static_cast<int>. RaceCamera.cpp's own comment records them agreeing on the camera's range (prog >= 0, well inside int range, measured 2026-08-26); out-of-int-range and NaN inputs were not compared [UNCERTAIN]. Math/FPURound.cpp's header lists the further unanchored approximations (AiStandalone.cpp RoundST0/RoundToInt, AiWallLateral.cpp, AiLineOfSight.cpp). Merge = a shared ST0 wrapper over FPURound_4a2c48 for the exe callers, then camera parity. Outside the AI lane (Race/RaceCamera.cpp).
""".lstrip("\n")


def main() -> int:
    apply = "--apply" in sys.argv
    raw = P.read_bytes().decode("utf-8")
    nl = "\r\n" if "\r\n" in raw else "\n"
    lines = raw.split(nl)
    out, removed = [], []
    for ln in lines:
        key = ln.split("#", 1)[0].strip()
        if key in REMOVE:
            removed.append(key)
            continue
        out.append(ln)
    missing = [k for k in REMOVE if k not in removed]
    if missing:
        print("NOT FOUND:", *missing, sep="\n  ")
        return 1
    # drop the trailing empty element, append the new block, re-add the trailing newline
    while out and out[-1] == "":
        out.pop()
    out.append("")
    out.extend(ADD.rstrip("\n").split("\n"))
    out.append("")
    text = nl.join(out)
    print(f"removed {len(removed)} keys, added 3, file {len(lines)} -> {len(out)} lines")
    if apply:
        P.write_bytes(text.encode("utf-8"))
        print("written")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
