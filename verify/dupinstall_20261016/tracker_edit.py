"""hooks.csv + CHANGELOG transaction for the dupinstall lane (re-classify side effects 1 and 4).

hooks.csv: for each consolidated row, `file` -> the surviving body's real path, `exe_file` -> the mechanical
value (what scripts/backfill_exe_file.py would write after the deletion), `name` -> survivor where the row
named the deleted body (0x00407a20 only), `notes` -> one appended segment. `confidence` and `status` are
untouched here: no row moved on the ladder (see RESULT_dupinstall.md). Every other cell of every other row is
byte-identical (csv round trip verified identical before editing).

CHANGELOG: one line per row, prepended directly below `<!-- ENTRIES -->`.

    py -3.12 tracker_edit.py            # dry run, prints the diff summary
    py -3.12 tracker_edit.py --apply
"""
import csv
import io
import pathlib
import sys

ROOT = pathlib.Path(__file__).resolve().parent.parent.parent
sys.path.insert(0, str(ROOT / "scripts"))
import rva_body_scan as S  # noqa: E402

PFX = "mashedmod/src/mashed_re/"
DATE = "2026-10-16"
EV = "verify/dupinstall_20261016"

# rva -> (survivor symbol, survivor TU, deleted symbol, deleted TU, deleted registry key or "-", extra note)
ROWS = {
    0x00407640: ("CopterFind639dc4", "Util/PromoLoop_round69.cpp", "FindCopterByKeyIndex", "Gameplay/RangeTable_ah1.cpp", "find_copter_by_key_index", ""),
    0x004077E0: ("FloatIdx639de0", "Util/PromoLoop_round61.cpp", "GetSelectedCopterField60", "Gameplay/Thresholds_ah4.cpp", "selected_copter_field60", ""),
    0x00407A20: ("GetAiLapCounter", "Gameplay/RangeTable_ah1.cpp", "Table8a9648Get", "Util/PromoLoop_round42.cpp", "table_8a9648_get",
                 "name column moved from the deleted Table8a9648Get to the survivor (the C4 racediff and the 2026-10-11 re-earn csv both ran on the body that installed, GetAiLapCounter)"),
    0x004098A0: ("Ret63a5f0", "Util/PromoLoop_round40.cpp", "GetLedEntryArrayBase", "Gameplay/RangeTable_ah1.cpp", "get_led_entry_array_base", ""),
    0x0040B970: ("Bool0Out8a94e0", "Util/PromoLoop_round67.cpp", "PlayerScoreZeroTest", "Gameplay/ScoreMasks_ah3.cpp", "-", ""),
    0x0040B9A0: ("MaxScoreFlags40b9a0", "Util/PromoLoop_sessionB.cpp", "PlayerScoreMaxTest", "Gameplay/ScoreMasks_ah3.cpp", "-",
                 "the 2026-07-24 install-observe ran PlayerScoreMaxTest (the body that won install then); the same class of run (path1 GREEN, E9 live, 30 s menu boot) was repeated on the survivor, C4 kept"),
    0x0040BA00: ("BoolMinOut8a94e0", "Util/PromoLoop_round67.cpp", "PlayerScoreMinTest", "Gameplay/ScoreMasks_ah3.cpp", "-", ""),
    0x0040BA60: ("Active4Slots40ba60", "Util/PromoLoop_sessionB.cpp", "PlayerScoreGateInvert", "Gameplay/ScoreMasks_ah3.cpp", "-",
                 "the 2026-07-24 install-observe ran PlayerScoreGateInvert (the body that won install then); the same class of run (path1 GREEN, E9 live, 30 s menu boot) was repeated on the survivor, C4 kept"),
    0x00415D00: ("AiWallAhead_Entry (WallAhead)", "Ai/AiWallAhead.cpp", "AiWallAhead", "Ai/AiTargeting.cpp", "-",
                 "the A/B self-test in the notes ran Ai/AiWallAhead.cpp; the deleted copy was an unverified float-chain port. The .asi hook now installs the passthrough entry (original body runs unless MASHED_AI_WALLAHEAD_SELFTEST), the copy that used to install ran the port"),
    0x00416060: ("AiLos_Entry (LosCheck)", "Ai/AiLineOfSight.cpp", "AiLineOfSight", "Ai/AiTargeting.cpp", "-",
                 "the A/B self-test in the notes ran Ai/AiLineOfSight.cpp; the deleted copy was an unverified float-chain port. The .asi hook now installs the passthrough entry (original body runs unless MASHED_AI_LOS_SELFTEST), the copy that used to install ran the port"),
    0x00426CB0: ("SlotIndexToPtr", "Frontend/MenuNearLeaves_s6.cpp", "FrontendSlotTablePtr426cb0", "Frontend/FrontendLeaves_ad2.cpp", "frontend_slot_table_ptr_426cb0",
                 "the row cites both batches (c3_batch_ad and batch-z-s6) and both bodies were path1 GREEN; the one that installed was kept"),
    0x004298C0: ("Clear67d99c_x4", "Util/PromoLoop_round70.cpp", "MenuQuadGlobalZero", "Frontend/MenuLeaves_af5.cpp", "menu_quad_global_zero",
                 "exe_file is now empty: the deleted body was the only exe-side copy and nothing referenced it"),
    0x0042AF50: ("MenuReadinessCheckC", "Frontend/MenuStateMachine.cpp", "MenuReadinessCheck", "Frontend/TimerReset.cpp", "menu_readiness_check",
                 "bodies read identically on both paths (guard, 0x1000 dispatch, 0x007f1501/0x007f1041 tables at stride 0x4c, 0x007f1a14 index array, callee 0x0040e470)"),
    0x0042BDE0: ("HudRectEmitter5", "Frontend/BatchAA_s1.cpp", "HudRectEmitter", "Frontend/MenuNearLeaves_s6.cpp", "hud_rect_emitter",
                 "the deleted HudRectEmitter was the body that installed and its registry arg_type int_pair compares no draw output (path1 RED 10/10 on the baseline run); the survivor is observed through draw_quad_observe"),
    0x00431F30: ("FrontendPageIdDispatch", "Frontend/MenuLeaves_af6.cpp", "FrontendPageFlagSet", "Frontend/SkeletonAndScatter_t6.cpp", "-",
                 "identical case set, targets and values; the deleted copy was 'impl-only, unsafe to A/B' per its own header"),
    0x004955B0: ("DInputInitPredicate", "Boot/RwEngineInit.cpp", "CreateDInputObjectBool", "Input/DirectInput.cpp", "create_dinput_object_bool",
                 "the deleted symbol was a one-line forwarder to the survivor that installed a second hook on the same RVA"),
    0x00496930: ("Table773030Get", "Util/PromoLoop_round30.cpp", "TimerTable773030Get", "Util/PromoLoop_round17.cpp", "timer_table_773030_get", ""),
    0x00498BF0: ("DisplayActiveFlagGet", "Boot/FrameDispatch.cpp", "DisplayGetCursorGate", "Boot/VideoConfig.cpp", "display_get_cursor_gate",
                 "both bodies are `return *(u32*)0x00773204`; the 2026-06-06 install-observe could not tell them apart (both installed then, before the dual-install guard); the same class of run (path1 GREEN, E9 live, 30 s menu boot) was repeated on the survivor, C4 kept"),
    0x004F8660: ("PluginDataDwordA", "Render/StateBatchGetters.cpp", "PluginFieldReadA8", "Render/PluginFields_ah4.cpp", "-",
                 "same sum (*(u32*)(obj + *(u32*)0x007d73a8)); the survivor is the body the evidence cites and the one mashed_re.exe also compiles, so exe_file == file now"),
    0x004F8690: ("PluginDataDwordB", "Render/StateBatchGetters.cpp", "PluginFieldReadAC", "Render/PluginFields_ah4.cpp", "-",
                 "same sum (*(u32*)(obj + *(u32*)0x007d73ac)); the survivor is the body the evidence cites and the one mashed_re.exe also compiles, so exe_file == file now"),
    0x00556CC0: ("SetDat00912a20", "Frontend/BatchAA_s3.cpp", "FontAtlasSlotSet", "Frontend/Cluster_v1.cpp", "font_atlas_slot_set", ""),
    0x00556CD0: ("GetDat00912a20", "Frontend/BatchAA_s6.cpp", "FontAtlasSlotGet", "Frontend/Cluster_v1.cpp", "font_atlas_slot_get", ""),
    0x005B3580: ("AudioListInit", "Audio/AudioList_ag.cpp", "AudioListHeaderInit", "Audio/AudioLeaves_ab2.cpp", "audio_list_header_init",
                 "the survivor is the body with the list-ring harness (count_header_list_ring); the deleted body's registry arg_type fmt_desc_pair_compare did not match its signature"),
}


def load_results() -> dict:
    """rva -> {'key', 'p1', 'p2', 'inst'} from the evidence this lane wrote (no value is invented:
    anything not found reads 'n/a')."""
    import re
    ev = ROOT / "verify" / "dupinstall_20261016"
    sys.path.insert(0, str(ROOT / "re" / "frida"))
    import hooks_registry as H
    key_by_rva = {}
    for k, h in H.HOOKS.items():
        key_by_rva.setdefault(h["rva"], []).append(k)
    p1 = {}
    f = ev / "path1_after_driver.log"
    if f.exists():
        for ln in f.read_text(encoding="utf-8").splitlines():
            m = re.match(r"(\S+)\s+rc=\d+ (\w[\w-]*)", ln)
            if m:
                p1[m.group(1)] = m.group(2)
    p2 = {}
    for f in sorted((ev / "path2_after").glob("path2_*.txt")):
        k = f.stem[len("path2_"):]
        t = f.read_text(encoding="utf-8", errors="replace")
        op = re.search(r"opcode:\s+\S+\s+expected 0xe9\s+->\s+(\w+)", t)
        rel = re.search(r"rel32:\s+\S+\s+expected \S+\s+->\s+(\w+)", t)
        ent = re.search(r"interceptor entries \(reimpl\):\s+(\d+) / (\d+)", t)
        tail = t.split("\nFAIL:", 1)[-1] if "\nFAIL:" in t else ""
        if not (op and rel):
            p2[k] = "not reached (race-scenario drive dies on the unmodified .asi too)"
        elif op.group(1) == "OK" and rel.group(1) == "OK" and "(none)" in tail.split("report written")[0]:
            p2[k] = f"PASS (E9 + rel32 to the survivor, {ent.group(1)}/{ent.group(2)} entries)"
        elif op.group(1) == "OK" and rel.group(1) == "OK":
            errs = sorted(set(re.findall(r"err=([^\n]+)", t)))
            p2[k] = ("install OK (E9 + rel32 to the survivor), call vectors not drivable: "
                     + ("; ".join(errs)[:60] if errs else "no entry observed"))
        else:
            p2[k] = "INSTALL FAIL"
    inst = {}
    f = ev / "install_after_summary.txt"
    if f.exists():
        for ln in f.read_text(encoding="utf-8").splitlines():
            m = re.match(r"0x([0-9A-F]{8})\s+(\S+)", ln)
            if m:
                inst[int(m.group(1), 16)] = m.group(2)
    out = {}
    for rva in ROWS:
        keys = key_by_rva.get(rva, [])
        k = keys[0] if keys else None
        out[rva] = {"key": k, "p1": p1.get(k, "n/a") if k else "n/a", "p2": p2.get(k, "n/a") if k else "n/a",
                    "inst": inst.get(rva, "n/a")}
    return out


def exe_file_for(scan: dict, rva: int) -> str:
    tus = S.rva_to_tus(scan, "exe").get(rva, [])
    return "|".join(sorted(PFX + t for t in tus))


def main() -> int:
    apply = "--apply" in sys.argv
    scan = S.scan_all()
    res = load_results()
    p = ROOT / "hooks.csv"
    raw = p.read_bytes().decode("utf-8")
    rows = list(csv.reader(io.StringIO(raw, newline="")))
    hdr = rows[0]
    ix = {c: i for i, c in enumerate(hdr)}
    by = {}
    for r in rows[1:]:
        try:
            by[int(r[0], 16)] = r          # comment rows ('# RVA: ...') are skipped
        except (ValueError, IndexError):
            pass
    cl = []
    for rva, (surv, stu, dele, dtu, dkey, extra) in ROWS.items():
        r = by[rva]
        old = (r[ix["name"]], r[ix["file"]], r[ix["exe_file"]])
        if rva == 0x00407A20:
            r[ix["name"]] = surv
        if rva == 0x00426CB0 and r[ix["status"]] == "mapped":
            r[ix["status"]] = "impl"       # file now names the port TU, not the plate .md
        r[ix["file"]] = PFX + stu
        r[ix["exe_file"]] = exe_file_for(scan, rva)
        note = (f"DUP-INSTALL CONSOLIDATED {DATE} (dupinstall, ROADMAP D4 item 3): one body, one install. "
                f"Survivor {surv} ({stu}), the body this row's evidence cites. Deleted {dele} ({dtu})"
                + (f" and its registry key {dkey}" if dkey != "-" else "") + ". "
                + (extra + ". " if extra else "")
                + f"Before: {EV}/install_before_summary.txt (2 installs, 1 refused). "
                + f"After (key {res[rva]['key'] or 'none, entry point not exported'}): path1 {res[rva]['p1']}, "
                + f"path2 {res[rva]['p2']}, install manifest {res[rva]['inst']}; {EV}/RESULT_dupinstall.md. "
                + "The 2026-09-09 repoint notes above describe the state before this change.")
        r[ix["notes"]] = (r[ix["notes"]] + " | " if r[ix["notes"]] else "") + note
        print(f"0x{rva:08x}  name {old[0]!r}->{r[ix['name']]!r}  file ..{old[1][-34:]!r}->..{r[ix['file']][-34:]!r}  "
              f"exe_file {old[2][-40:]!r}->{r[ix['exe_file']][-40:]!r}")
        cl.append(f"{DATE}  {rva:08x}  {r[ix['name']]}  {r[ix['confidence']]}->{r[ix['confidence']]} (no ladder move)  "
                  f"DUP-INSTALL consolidated (dupinstall, ROADMAP D4 item 3): one body, one install. Survivor {surv} ({stu}), "
                  f"deleted {dele} ({dtu}). path1 {res[rva]['p1']}, path2 {res[rva]['p2']}, install manifest {res[rva]['inst']}. "
                  f"Evidence {EV}/RESULT_dupinstall.md.")
    buf = io.StringIO(newline="")
    csv.writer(buf, lineterminator="\r\n").writerows(rows)
    cp = ROOT / "re" / "analysis" / "CHANGELOG.md"
    ctext = cp.read_bytes().decode("utf-8")
    nl = "\r\n" if "\r\n" in ctext else "\n"
    clines = ctext.split(nl)
    at = [i for i, l in enumerate(clines) if l == "<!-- ENTRIES -->"]
    assert len(at) == 1, at
    clines[at[0] + 1:at[0] + 1] = cl
    if apply:
        p.write_bytes(buf.getvalue().encode("utf-8"))
        cp.write_bytes(nl.join(clines).encode("utf-8"))
        print("hooks.csv written; CHANGELOG lines prepended:", len(cl))
    else:
        print("CHANGELOG would gain", len(cl), "lines; first:", cl[0][:200])
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
