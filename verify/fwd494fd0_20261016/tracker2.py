"""hooks.csv + CHANGELOG transaction for Part C of the fwd494fd0 lane (re-classify side effects 1 and 4).

  0x0040e480  CarSlotStateSet  C3 RE-EARNED on the consolidated body: the one body is now Ai/CarSlotState.cpp (both
              targets); Frontend/RaceResults.cpp's copy and AiStandalone.cpp's private copy are gone. `file` and
              `exe_file` both point at it.
  0x00417180  AiBankSwitch     C3 RE-EARNED on the consolidated body: the one body is now Ai/AiBankSwitch.cpp (both
              targets), the AiPreTick.cpp text the evidence cites; AiStandalone.cpp's BankSwitch is gone.
  0x00418860  exe_file drops HUD/ScenarioLeaves_sa2.cpp (the dead, uninstalled AiPerFrameTick copy was deleted). Level stays C2.
  0x0046d510  note only: the dead AiVehicleVelocity3 copy in Ai/AiTargeting.cpp was deleted. Level stays C3.
  0x004a2c48  exe_file now names the verbatim body Math/FPURound.cpp (it was never anchored) instead of the CRT-thunk
              caller Ai/VehicleFloatFieldAsInt.cpp that the scanner had mistaken for a body. Level stays C2.

    py -3.12 tracker2.py            # dry run
    py -3.12 tracker2.py --apply
"""
import csv
import io
import pathlib
import re
import sys

ROOT = pathlib.Path(__file__).resolve().parent.parent.parent
DATE = "2026-10-16"
EV = "verify/fwd494fd0_20261016"
W = ROOT / "verify" / "fwd494fd0_20261016" / "win2"
PFX = "mashedmod/src/mashed_re/"


def read(name: str) -> str:
    return (W / name).read_text(encoding="utf-8", errors="replace")


def path1() -> str:
    t = read("path1_car_slot_state_set.txt")
    m = re.search(r"total cases:\s+(\d+)\s+mismatches:\s+(\d+)", t)
    assert m and m.group(2) == "0" and "GREEN" in t, "path1 not GREEN"
    return f"path1 GREEN {m.group(1)}/{m.group(1)} (ptr_ptr_entity_set, hook bypassed)"


def path2() -> str:
    # window 3: window 2's path2 installed fine (E9 + rel32 OK) but its call-through died 'bad argument count' on a
    # registry vector shape that this lane fixed ({'scalars': [...]}), so the PASS is the re-run's.
    t = (W.parent / "win3" / "path2_car_slot_state_set.txt").read_text(encoding="utf-8", errors="replace")
    m = re.search(r"interceptor entries \(reimpl\):\s+(\d+) / (\d+)", t)
    op = re.search(r"opcode:\s+\S+\s+expected 0xe9\s+->\s+(\w+)", t)
    rel = re.search(r"rel32:\s+\S+\s+expected \S+\s+->\s+(\w+)", t)
    assert m and op.group(1) == "OK" and rel.group(1) == "OK" and m.group(1) == m.group(2), "path2 not PASS"
    return f"path2 PASS (E9 + rel32 to the export, {m.group(1)}/{m.group(2)} entries)"


def nav(name: str) -> str:
    t = read(name)
    assert "drive_to_race=True" in t and "proc.poll()=None" in t and "detached events=[]" in t and "CRASH" not in t, name
    return "reached a race, process alive, no detach"


def boot() -> str:
    t = read("install_observe_all.txt")
    m = re.search(r"manifest: (\d+) hooks seen, (\d+) installed", t)
    assert m and re.search(r"survived 45s", t), "boot not survived"
    man = (W / "install_all" / "hook_install_manifest.txt").read_text(encoding="utf-8", errors="replace")
    for rva, nm in (("0x0040E480", "CarSlotStateSet"), ("0x00417180", "AiBankSwitch")):
        assert re.search(rf"{rva}\s+1\s+{nm}", man, re.I), f"{nm} not installed in the manifest"
    return f"full set {m.group(2)} of {m.group(1)} installed incl. CarSlotStateSet and AiBankSwitch, survived 45 s"


def gctrl() -> str:
    def trio(p):
        t = (ROOT / EV / p).read_text(encoding="utf-8")
        return tuple(re.findall(r"sha8=(\w{8})", t))
    a, b = trio("gctrl_base_a.log"), trio("gctrl_after_b.log")
    assert len(a) == 3 and a == b, (a, b)
    return f"exe G-CTRL {'/'.join(b)} before and after (baseline exe built from 81b2add0)"


def ab(tag: str) -> str:
    """The AbAiBank log of one live-race run: last summary line + confirmed-mismatch count."""
    t = read(f"ab/AbAiBank.{tag}.log")
    summ = [ln for ln in t.splitlines() if "calls=" in ln]
    assert summ, f"no summary line in AbAiBank.{tag}.log"
    m = re.search(r"calls=(\d+) mism=(\d+) rawmism=(\d+) cov\[req=(\d+) timer=(\d+) rand=(\d+)\]", summ[-1])
    assert m, summ[-1]
    calls, mism, raw, req, timer, rand = map(int, m.groups())
    confirmed = sum(1 for ln in t.splitlines() if "MISMATCH-CONFIRMED" in ln)
    assert mism == 0 and confirmed == 0, f"AbAiBank {tag}: mism={mism} confirmed lines={confirmed}"
    return f"{tag}: >= {calls} calls, mism=0 (confirmed), rawmism={raw}, coverage req={req} timer={timer} rand={rand}"


def ab_both() -> str:
    return ("AbAiBank in a live race, MINE = the shared body vs the ORIGINAL, whole write surface: "
            f"{ab('track0')}; {ab('track3')}")


SLOT = (f"C3 RE-EARNED {DATE} (fwd494fd0) ON THE CONSOLIDATED BODY: ONE body now, Ai/CarSlotState.cpp, compiled into BOTH "
        "targets and installed in the .asi (file == exe_file). Frontend/RaceResults.cpp's installed copy and "
        "AiStandalone.cpp's private exe copy are deleted (AiStandalone keeps a one-line forwarder). The standalone's "
        "null-base guard (`base==0 -> return`: the image-pad zero-fills the original's .data) lives in the shared body; the "
        "original has no guard because *(u32*)0x005f2770 is 0x005f2728 once loaded, so the guarded path is unreachable "
        f"inside MASHED.exe. Evidence on the new body: {path1()}; {path2()}; nav boot, full hook set: {nav('nav_probe_full_hookset.txt')}; "
        f"nav boot with ONLY CarSlotStateSet live (the original's AI tick FUN_00418860 -> FUN_0040e480 calls it in a live race): "
        f"{nav('nav_probe_only_CarSlotStateSet.txt')}; {boot()}; {gctrl()}. {EV}/RESULT_fwd494fd0.md")

BANK = (f"C3 RE-EARNED {DATE} (fwd494fd0) ON THE CONSOLIDATED BODY: ONE body now, Ai/AiBankSwitch.cpp (the AiPreTick.cpp text the "
        "evidence cites), compiled into BOTH targets and installed in the .asi (file == exe_file). AiStandalone.cpp's BankSwitch, "
        "SplineBankTimerReset and RandUnit are deleted and its VehicleStep calls the shared body. Compared line by line first: same "
        "addresses, branch order, read-once / re-read pattern, 9000 cap. Two thunks differ by target: the reset is an inline loop "
        "writing 1000 into FOUR slots 0x008032d4 + i*0x14 (= FUN_00414030(-1), read from the unpatched original at "
        "0x00414039..0x0041404e; the 'resets all 5 slots' wording on that row is wrong), and the random roll is FUN_00472650 in the "
        ".asi vs the verbatim FUN_00534870 ring port Ai_RandRange in the exe (same law, not the same draw index, so the exe has no "
        f"per-draw evidence). Evidence on the new body: {ab_both()}; nav boot with ONLY AiBankSwitch live: "
        f"{nav('nav_probe_only_AiBankSwitch.txt')}; nav boot full hook set: {nav('nav_probe_full_hookset.txt')}; {boot()}; "
        f"{gctrl()}. {EV}/RESULT_fwd494fd0.md")

changelog = [
    (0x0040E480, "CarSlotStateSet", "C3->C3 (RE-EARNED)",
     f"Two bodies (Frontend/RaceResults.cpp installed, AiStandalone.cpp exe copy) -> one, Ai/CarSlotState.cpp in both targets. {path1()}; {path2()}; "
     f"nav boot full set and ONLY-this-hook both reach a race alive; {boot()}; {gctrl()}."),
    (0x00417180, "", "C3->C3 (RE-EARNED)",
     f"Two bodies (Ai/AiPreTick.cpp installed, AiStandalone.cpp BankSwitch exe copy) -> one, Ai/AiBankSwitch.cpp in both targets; "
     f"reset and RNG are the two per-target thunks. {ab_both()}; ONLY-AiBankSwitch nav boot reaches a race alive; {boot()}; {gctrl()}."),
    (0x00414030, "", "(no ladder move)",
     "NOTE CORRECTED: the param=-1 path writes 1000 into FOUR slots, not 'all 5': the unpatched original at 0x00414039..0x0041404e is "
     "`mov eax,0x8032d4 / mov [eax],0x3e8 / add eax,0x14 / cmp eax,0x803324 / jl`, i.e. 0x008032d4 + i*0x14 for i = 0..3 "
     "(0x803324 - 0x8032d4 = 0x50 = 4 * 0x14). Read by disassembly: U-7564's remark that the -1 path was never vectored by a "
     "diff still stands (the AbAiBank runs of this lane now exercise it through the shared AiBankSwitch). Level unchanged."),
    (0x00418860, "", "(no ladder move)",
     "Dead uninstalled AiPerFrameTick copy (HUD/ScenarioLeaves_sa2.cpp, compiled into both targets, called original VAs) deleted; "
     "exe_file now Ai/AiStandalone.cpp. AbAiTick re-anchored as a harness driver, not a body. Row stays C2."),
    (0x0046D510, "", "(no ladder move)",
     "Dead AiVehicleVelocity3 copy (Ai/AiTargeting.cpp, no callers, installer removed 2026-09-01) deleted; the one body is "
     "Ai/VehicleVelocityWorldGet.cpp. Row stays C3."),
    (0x004A2C48, "", "(no ladder move)",
     "Scanner anchors corrected: the CRT-thunk caller Ai/VehicleFloatFieldAsInt.cpp:FtolFrom and a typedef comment in "
     "Frontend/SetupScreenRenderers.cpp were never bodies; the verbatim body Math/FPURound.cpp is now RVA-anchored. "
     "exe_file = FPURound.cpp|RaceCamera.cpp, the real pair. Row stays C2."),
]


def main() -> int:
    apply = "--apply" in sys.argv
    p = ROOT / "hooks.csv"
    rows = list(csv.reader(io.StringIO(p.read_bytes().decode("utf-8"), newline="")))
    ix = {c: i for i, c in enumerate(rows[0])}
    by = {}
    for r in rows[1:]:
        try:
            by[int(r[0], 16)] = r
        except (ValueError, IndexError):
            pass

    r = by[0x0040E480]
    assert r[ix["confidence"]] == "C3", r[ix["confidence"]]
    r[ix["file"]] = PFX + "Ai/CarSlotState.cpp"
    r[ix["exe_file"]] = PFX + "Ai/CarSlotState.cpp"
    r[ix["notes"]] = r[ix["notes"]] + " | " + SLOT
    print("0x0040e480  file + exe_file -> Ai/CarSlotState.cpp, C3 re-earned")

    r = by[0x00417180]
    assert r[ix["confidence"]] == "C3", r[ix["confidence"]]
    assert r[ix["file"]] == PFX + "Ai/AiPreTick.cpp" and r[ix["exe_file"]] == PFX + "Ai/AiStandalone.cpp"
    r[ix["file"]] = PFX + "Ai/AiBankSwitch.cpp"
    r[ix["exe_file"]] = PFX + "Ai/AiBankSwitch.cpp"
    r[ix["frida_diff"]] = r[ix["frida_diff"]] + ";" + EV + "/win2/ab/AbAiBank.track0.log;" + EV + "/win2/ab/AbAiBank.track3.log"
    r[ix["notes"]] = r[ix["notes"]] + " | " + BANK
    print("0x00417180  file + exe_file -> Ai/AiBankSwitch.cpp, C3 re-earned")

    r = by[0x00414030]
    r[ix["notes"]] += (f" | {DATE} (fwd494fd0) NOTE CORRECTED: the param=-1 path writes 1000 into FOUR slots (0x008032d4 + i*0x14, i = 0..3), "
                       "not 'all 5': unpatched original 0x00414039..0x0041404e = mov eax,0x8032d4 / mov [eax],0x3e8 / add eax,0x14 / "
                       "cmp eax,0x803324 / jl; 0x803324-0x8032d4 = 0x50 = 4*0x14. Read by disassembly, not by a diff; level unchanged")
    print("0x00414030  note (4 slots, not 5)")

    r = by[0x00418860]
    assert r[ix["exe_file"]] == PFX + "Ai/AiStandalone.cpp|" + PFX + "HUD/ScenarioLeaves_sa2.cpp", r[ix["exe_file"]]
    r[ix["exe_file"]] = PFX + "Ai/AiStandalone.cpp"
    r[ix["notes"]] += (f" | {DATE} (fwd494fd0): the dead uninstalled AiPerFrameTick copy in HUD/ScenarioLeaves_sa2.cpp is deleted, "
                       "exe_file no longer names it; level unchanged")
    print("0x00418860  exe_file -> Ai/AiStandalone.cpp")

    r = by[0x0046D510]
    r[ix["notes"]] += (f" | {DATE} (fwd494fd0): the dead AiVehicleVelocity3 copy in Ai/AiTargeting.cpp (no callers, installer removed "
                       "2026-09-01) is deleted; this file is now the only body; level unchanged")
    print("0x0046d510  note")

    r = by[0x004A2C48]
    assert r[ix["exe_file"]] == PFX + "Ai/VehicleFloatFieldAsInt.cpp|" + PFX + "Race/RaceCamera.cpp", r[ix["exe_file"]]
    r[ix["exe_file"]] = PFX + "Math/FPURound.cpp|" + PFX + "Race/RaceCamera.cpp"
    r[ix["notes"]] += (f" | {DATE} (fwd494fd0): exe_file corrected. Ai/VehicleFloatFieldAsInt.cpp:FtolFrom is a thunk INTO the original "
                       "(push 0x004a2c48 / ret), not a body, and Math/FPURound.cpp (the verbatim body, compiled into the exe with no "
                       "caller there) is now RVA-anchored; the real exe-side pair is FPURound.cpp vs Race/RaceCamera.cpp:Ftol_4a2c48; level unchanged")
    print("0x004a2c48  exe_file -> FPURound.cpp|RaceCamera.cpp")

    buf = io.StringIO(newline="")
    csv.writer(buf, lineterminator="\r\n").writerows(rows)
    cp = ROOT / "re" / "analysis" / "CHANGELOG.md"
    ct = cp.read_bytes().decode("utf-8")
    nl = "\r\n" if "\r\n" in ct else "\n"
    L = ct.split(nl)
    at = [i for i, l in enumerate(L) if l == "<!-- ENTRIES -->"]
    assert len(at) == 1
    lines = []
    for rva, name, lvl, txt in changelog:
        nm = name or by[rva][ix["name"]]
        lines.append(f"{DATE}  {rva:08x}  {nm}  {lvl}  {txt} Evidence {EV}/RESULT_fwd494fd0.md.")
    L[at[0] + 1:at[0] + 1] = lines
    print(len(lines), "changelog lines")
    if apply:
        p.write_bytes(buf.getvalue().encode("utf-8"))
        cp.write_bytes(nl.join(L).encode("utf-8"))
        print("written")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
