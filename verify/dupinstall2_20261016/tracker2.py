"""hooks.csv + CHANGELOG transaction for the dupinstall2 lane (re-classify side effects 1 and 4).

Rows touched, with the reason (every value below is read from the evidence files this lane wrote):
  0x0046cbb0  consolidation: name/file -> the survivor VehicleCarStateRead, C4 kept (racediff CLEAN on the survivor)
  0x0040b9a0, 0x0040ba60  C4 -> C3 DEMOTED (no canonical-scenario evidence the surviving body ever runs)
  0x00407a20, 0x004098a0, 0x00498bf0  C4 kept, strict re-check recorded
  0x00495080  C3 -> C2 DEMOTED (the port assembles `mov esi,imm32` for `mov esi,[0x771a0c]`, AV EIP=1 when installed)
  8 rows whose path2 call-through could not be driven: PASS recorded after the vector-shape fix
  3 race rows (0x004f8660, 0x004f8690, 0x00496930): the QUEUED phrase replaced by the result

    py -3.12 tracker2.py            # dry run
    py -3.12 tracker2.py --apply    # on a tree where hooks.csv / CHANGELOG are still at the base commit
"""
import csv
import io
import pathlib
import re
import sys

ROOT = pathlib.Path(__file__).resolve().parent.parent.parent
sys.path.insert(0, str(ROOT / "re" / "frida"))
import hooks_registry as H  # noqa: E402

DATE = "2026-10-16"
EV = "verify/dupinstall2_20261016"
PFX = "mashedmod/src/mashed_re/"
HERE = ROOT / "verify" / "dupinstall2_20261016"

KEY_BY_RVA = {}
for k, h in H.HOOKS.items():
    KEY_BY_RVA.setdefault(h["rva"], []).append(k)


def p2(report: pathlib.Path) -> str:
    """'PASS (E9 + rel32 to the survivor, n/n entries)' | explanation, from one run_verify_hook report."""
    t = report.read_text(encoding="utf-8", errors="replace")
    op = re.search(r"opcode:\s+\S+\s+expected 0xe9\s+->\s+(\w+)", t)
    rel = re.search(r"rel32:\s+\S+\s+expected \S+\s+->\s+(\w+)", t)
    ent = re.search(r"interceptor entries \(reimpl\):\s+(\d+) / (\d+)", t)
    tail = t.split("\nFAIL:", 1)[-1] if "\nFAIL:" in t else ""
    if not (op and rel):
        return "NOT REACHED"
    if op.group(1) == "OK" and rel.group(1) == "OK" and "(none)" in tail.split("report written")[0]:
        return f"PASS (E9 + rel32 to the survivor, {ent.group(1)}/{ent.group(2)} entries)"
    if op.group(1) == "OK" and rel.group(1) == "OK":
        errs = sorted(set(re.findall(r"err=([^\n]{0,60})", t)))
        return "install OK (E9 + rel32 to the survivor), call vectors failed: " + "; ".join(errs)[:80]
    return "INSTALL FAIL"


W1 = HERE / "win1"
W2 = HERE / "win2"
W3 = HERE / "win3"
res = {}
for f in list(W1.glob("path2_*.txt")):
    res[f.stem[6:]] = ("win1", p2(f))
for f in list(W2.glob("path2_*.txt")) + list(W3.glob("path2_*.txt")):
    res[f.stem[6:]] = ("win2/3 with MASHED_HOOK_SKIP=Fwd494fd0_DoublePlusHalf", p2(f))

ROWS_PATH2_FIXED = {0x00407640: "copter_find_639dc4", 0x0040B970: "bool0_out_8a94e0", 0x0040BA00: "boolmin_out_8a94e0",
                    0x0040B9A0: "maxscoreflags_40b9a0", 0x0040BA60: "active4slots_40ba60", 0x005B3580: "audio_list_init",
                    0x00431F30: "frontend_page_id_dispatch", 0x0042BDE0: "hud_rect_emitter5"}
RACE_ROWS = {0x004F8660: "plugin_data_dword_a", 0x004F8690: "plugin_data_dword_b", 0x00496930: "table_773030_get"}
QUEUED = ("path2 QUEUED (race-scenario nav drive lost its Frida session, also on the unmodified .asi; cause not "
          "established, orchestrator reports the PC has no usable audio endpoint)")

NAV_FINDING = ("NAV FINDING 2026-10-16: the race drive died because hook 0x00495080 Fwd494fd0_DoublePlusHalf "
               "(Render/PromoLoop_round256.cpp:172) assembles `mov esi, dword ptr [771A0Ch]` as `mov esi, 0x771a0c` "
               "(BE 0C 1A 77 00, an immediate) instead of the memory load (8B 35 0C 1A 77 00); FUN_00494fd0 reads its "
               "device pointer from ESI, so `call [eax+0x1c]` at 0x00494ff1 jumps to 0x1 (AV, exit 0xC0000005). "
               "KNOWN_ISSUES AUTOHOOK-INLINE-ASM-IMMEDIATE")

notes_add = {}
conf_new = {}
status_new = {}
name_new = {}
file_new = {}
changelog = []


def add(rva, text):
    notes_add.setdefault(rva, []).append(text)


# --- 0x0046cbb0 consolidation ---------------------------------------------------------------------------------------
add(0x0046CBB0,
    f"DUP-INSTALL CONSOLIDATED {DATE} (dupinstall2, ROADMAP D4 item 3, the one entry the first pass skipped): one body, "
    "one install. Survivor VehicleCarStateRead (Vehicle/VehicleState.cpp, also compiled into mashed_re.exe, so exe_file == "
    "file), the body that installed by link order when the C4 racediff ran (re/analysis/phys_c4_evidence/"
    "c4_racediff_result_2026-08-15.json, installed by RVA). Deleted CarStatePairGet (Util/PromoLoop_round25.cpp) and its "
    f"registry key car_state_pair_get; no Vehicle/ file touched. After: path1 vehicle_car_state_read GREEN (9 cases), "
    f"path2 {res['vehicle_car_state_read'][1]}, install manifest single, canonical_c4_racediff in-race frames 300-1200 "
    f"C4-CLEAN (off 1 sig / on 1 sig, JMP 0xE9, survivor installed by RVA). C4 kept. {EV}/RESULT_dupinstall2.md")
name_new[0x0046CBB0] = "VehicleCarStateRead"
file_new[0x0046CBB0] = PFX + "Vehicle/VehicleState.cpp"
changelog.append((0x0046CBB0, "VehicleCarStateRead", "C4->C4 (no ladder move)",
                  "DUP-INSTALL consolidated (dupinstall2): CarStatePairGet deleted, survivor VehicleCarStateRead "
                  "(Vehicle/VehicleState.cpp, exe_file == file). In-race racediff C4-CLEAN on the survivor, path1 GREEN, "
                  f"path2 {res['vehicle_car_state_read'][1]}."))

# --- C4 strict re-check -------------------------------------------------------------------------------------------------
add(0x00407A20,
    f"C4 STRICT RE-CHECK {DATE} (dupinstall2): canonical in-race Stalker racediff (Quick Battle, frames 300-1200, OFF vs ON, "
    "survivor GetAiLapCounter installed by RVA) C4-CLEAN, 1 signature each side, JMP 0xE9; the committed 2026-08-15 result "
    "for this RVA agrees. Boot-to-menu: 0 calls (race-only function). C4 kept. "
    f"{EV}/win1/c4_racediff.txt")
changelog.append((0x00407A20, "GetAiLapCounter", "C4->C4 (no ladder move, strict re-check)",
                  "In-race racediff C4-CLEAN on the surviving body (1 sig each side, JMP 0xE9); committed 2026-08-15 result agrees."))
add(0x004098A0,
    f"C4 STRICT RE-CHECK {DATE} (dupinstall2): the earlier race CLEAN (re/analysis/c4_race_processed.txt) ran on the then-"
    "installed GetLedEntryArrayBase, now deleted, and this row's notes recorded no C4 basis. canonical_c4_racediff on the "
    "survivor Ret63a5f0 (Quick Battle, frames 300-1200, installed by RVA): C4-CLEAN, 1 signature each side, JMP 0xE9. "
    f"Boot-to-menu: 0 calls (race-only). C4 kept on the new run. {EV}/win1/c4_racediff.txt")
changelog.append((0x004098A0, "Ret63a5f0", "C4->C4 (no ladder move, strict re-check)",
                  "The earlier race CLEAN ran on the deleted GetLedEntryArrayBase; in-race racediff on the survivor Ret63a5f0 "
                  "is C4-CLEAN (1 sig each side, JMP 0xE9). C4 basis now recorded."))
add(0x00498BF0,
    f"C4 STRICT RE-CHECK {DATE} (dupinstall2): canonical_c4_verify on the survivor DisplayActiveFlagGet: OFF run "
    "(Interceptor on the original RVA, boot-to-menu) 955 calls, ON run JMP 0xE9, game survived: C4-READY. The 2026-06-06 "
    "install-observe could not tell the two identical one-liners apart. C4 kept. "
    f"{EV}/win1/c4_verify.txt")
changelog.append((0x00498BF0, "DisplayActiveFlagGet", "C4->C4 (no ladder move, strict re-check)",
                  "canonical_c4_verify on the surviving body: exercised 955x at boot-to-menu, JMP 0xE9 live, survived."))

# --- C4 -> C3 demotions ---------------------------------------------------------------------------------------------------
for rva, sym, other in ((0x0040B9A0, "MaxScoreFlags40b9a0", "PlayerScoreMaxTest"),
                        (0x0040BA60, "Active4Slots40ba60", "PlayerScoreGateInvert")):
    k = ROWS_PATH2_FIXED[rva]
    why = (f"C4->C3 DEMOTED {DATE} (dupinstall2, strict CONFIDENCE.md check): no canonical-scenario evidence that the "
           f"surviving body {sym} ever runs with the hook live. canonical_c4_verify (boot-to-menu, OFF run, Interceptor on "
           "the original RVA): 0 calls. canonical_c4_racediff (Quick Battle race, frames 300-1200, OFF vs ON): original not "
           "observed, 0 signatures on both sides. The 2026-07-24 C4 was an install-observe (E9 live, boot to menu, 25 s), "
           f"which cannot show the body executed, and it ran the deleted {other}. The caller FUN_0041b540 hangs off the "
           "Race::Tick non-ghost branch (re/analysis/leaderboard_d3/0x0041b540.md), not the Quick Battle path. C3 stands: "
           f"path1 GREEN, path2 {res[k][1]}. Re-promotion needs a scenario that reaches FUN_0041b540 with the inline JMP "
           f"live. {EV}/win1/c4_verify.txt, c4_racediff.txt")
    add(rva, why)
    conf_new[rva] = "C3"
    status_new[rva] = "impl"
    changelog.append((rva, sym, "C4->C3 (DEMOTED, strict C4 check)",
                      f"No canonical-scenario evidence the surviving body runs: 0 calls at boot-to-menu (canonical_c4_verify), "
                      f"original not observed in the Quick Battle race frames 300-1200 (canonical_c4_racediff). The 2026-07-24 "
                      f"C4 was an install-observe on the deleted {other}, which cannot show execution. path1 GREEN, path2 "
                      f"{res[k][1]} keep C3. Re-promotion needs a scenario that reaches FUN_0041b540."))

# --- path2 call-through fixed ------------------------------------------------------------------------------------------------
for rva, k in ROWS_PATH2_FIXED.items():
    if rva in (0x0040B9A0, 0x0040BA60):
        continue                                   # already carried by the demotion note
    add(rva, f"path2 call-through RE-RUN {DATE} (dupinstall2; the registry path2 vector shape was fixed, the install was "
             f"never in doubt): {res[k][1]}. {EV}/win1/path2_{k}.txt")

# --- race rows -----------------------------------------------------------------------------------------------------------------
race_text = {}
for rva, k in RACE_ROWS.items():
    race_text[rva] = f"path2 {res[k][1]} [run with MASHED_HOOK_SKIP=Fwd494fd0_DoublePlusHalf, {NAV_FINDING}]"

# --- 0x00495080 demotion -----------------------------------------------------------------------------------------------------
add(0x00495080,
    f"C3->C2 DEMOTED {DATE} (dupinstall2): {NAV_FINDING}. With the full auto-hook set MASHED.exe dies while the nav drive boots to the menu "
    "(EIP 0x1, exit 0xC0000005); bisected by MASHED_HOOK_LO/HI to this hook (registry index 1335 of 1354) and confirmed by "
    "MASHED_HOOK_SKIP: skipped, the race drive completes. The C3 A/B could not see it (the recorder stubs the callee and "
    "reads only stack args, ESI is a register input). One-line fix, verified to assemble to a memory load in a probe: "
    "`mov esi, dword ptr ds:[771A0Ch]`. Re-earn after the fix: path1 plus a nav boot with the hook installed. "
    f"{EV}/RESULT_dupinstall2.md")
conf_new[0x00495080] = "C2"
changelog.append((0x00495080, "Fwd494fd0_DoublePlusHalf", "C3->C2 (DEMOTED, live crash)",
                  "The port assembles `mov esi, dword ptr [771A0Ch]` as `mov esi, 0x771a0c` (immediate); FUN_00494fd0 reads its "
                  "device pointer from ESI, so the installed hook jumps to 0x1 while the nav drive boots to the menu (AV, exit 0xC0000005). Found "
                  "while chasing the race-scenario path2 failure: bisected by MASHED_HOOK_LO/HI, confirmed by MASHED_HOOK_SKIP. "
                  "The C3 A/B stubs the callee and cannot see ESI. Fix: `mov esi, dword ptr ds:[771A0Ch]` (probe verified), then "
                  "re-earn. KNOWN_ISSUES AUTOHOOK-INLINE-ASM-IMMEDIATE."))


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
    touched = set(notes_add) | set(RACE_ROWS)
    for rva in sorted(touched):
        r = by[rva]
        old = (r[ix["name"]], r[ix["confidence"]], r[ix["status"]], r[ix["file"]][-30:])
        if rva in name_new:
            r[ix["name"]] = name_new[rva]
        if rva in file_new:
            r[ix["file"]] = file_new[rva]
        if rva in conf_new:
            r[ix["confidence"]] = conf_new[rva]
        if rva in status_new:
            r[ix["status"]] = status_new[rva]
        if rva in RACE_ROWS:
            n = r[ix["notes"]]
            assert QUEUED in n, hex(rva)
            r[ix["notes"]] = n.replace(QUEUED, race_text[rva])
        for t in notes_add.get(rva, []):
            r[ix["notes"]] = (r[ix["notes"]] + " | " if r[ix["notes"]] else "") + t
        print(f"0x{rva:08x} {old} -> ({r[ix['name']]}, {r[ix['confidence']]}, {r[ix['status']]}, {r[ix['file']][-30:]})")
    for rva in RACE_ROWS:
        changelog.append((rva, by[rva][ix["name"]], by[rva][ix["confidence"]] + "->" + by[rva][ix["confidence"]] + " (no ladder move)",
                          f"path2 for the race-scenario row: {res[RACE_ROWS[rva]][1]}. The nav drive had been dying because of hook "
                          "0x00495080 (KNOWN_ISSUES AUTOHOOK-INLINE-ASM-IMMEDIATE); run with MASHED_HOOK_SKIP=Fwd494fd0_DoublePlusHalf."))
    buf = io.StringIO(newline="")
    csv.writer(buf, lineterminator="\r\n").writerows(rows)
    cp = ROOT / "re" / "analysis" / "CHANGELOG.md"
    ct = cp.read_bytes().decode("utf-8")
    nl = "\r\n" if "\r\n" in ct else "\n"
    L = ct.split(nl)
    at = [i for i, l in enumerate(L) if l == "<!-- ENTRIES -->"]
    assert len(at) == 1
    lines = [f"{DATE}  {rva:08x}  {name}  {lvl}  {txt} Evidence {EV}/RESULT_dupinstall2.md." for rva, name, lvl, txt in changelog]
    L[at[0] + 1:at[0] + 1] = lines
    # the first pass's three QUEUED changelog lines are replaced in place (same session, nothing merged on top of them)
    for i, l in enumerate(L):
        if QUEUED in l:
            for rva, k in RACE_ROWS.items():
                if re.match(rf"{DATE}  {rva:08x}  ", l):
                    L[i] = l.replace(QUEUED, race_text[rva])
    print(len(lines), "changelog lines;", "QUEUED left in changelog:", sum(QUEUED in l for l in L))
    if apply:
        p.write_bytes(buf.getvalue().encode("utf-8"))
        cp.write_bytes(nl.join(L).encode("utf-8"))
        print("written")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
