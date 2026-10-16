"""hooks.csv + CHANGELOG transaction for dualcopy4 (re-classify side effects 1 and 4), one entry per RVA. Every claim is read from an evidence file and
asserted; a missing or failing file aborts the whole transaction. Unchanged records are written back byte for byte.   py -3.12 tracker4.py [--apply]"""
import csv, io, pathlib, re, sys

ROOT = pathlib.Path(__file__).resolve().parent.parent.parent
H = ROOT / "verify" / "dualcopy4_20261016"
EV, DATE, P = "verify/dualcopy4_20261016", "2026-10-16", "mashedmod/src/mashed_re/"


def rd(p):
    return (H / p).read_text(encoding="utf-8", errors="replace")


def sha8(log):
    return tuple(re.findall(r"sha8=(\w{8})", rd(log)))


def gctrl():
    a, b = sha8("b_gctrl.log"), sha8("nb_gctrl.log")
    assert len(a) == 3 and a == b, (a, b)
    return "/".join(b)


def p1(key):
    t = rd(f"wb1/path1_{key}.txt")
    m = re.search(r"total cases:\s+(\d+)\s+mismatches:\s+(\d+)", t)
    assert m and m.group(2) == "0" and "GREEN" in t, key
    return f"path1 GREEN {m.group(1)}/{m.group(1)} (run_diff, final asi f4449212)"


def ew(key):
    t = rd(f"wb1/ew_{key}.txt")
    m = re.search(r"cases=(\d+) mismatches=(\d+)", t)
    assert m and m.group(2) == "0" and "VERDICT: GREEN" in t, key
    return f"path1 GREEN {m.group(1)}/{m.group(1)} (early-window leaf lane, final asi f4449212)"


def p2(key, win):
    t = rd(f"{win}/path2_{key}.txt")
    assert "opcode == 0xE9" in t and "rel32 matches" in t, key
    m = re.search(r"reimpl interceptor fired (\d+)x", t)
    if "FAIL:\n  (none)" in t and m:
        return f"path2 PASS (E9 + rel32, {m.group(1)} entries)"
    return "path2 E9 + rel32 site-verified, entries 0 (call-through not drivable: harness argument gap)"


def mut():
    a, b = rd("wm/mut_timer_bit_field_set.txt"), rd("wm/mut_pred_405890.txt")
    assert "mismatches:  10" in a and "RED" in a and "VERDICT: RED" in b, "mutant controls"
    return "M-CTRL: flag-inverted TimerBitFieldSet RED 10/10, done!=total CollectDoneBody405890 RED 7/10"


def twin():
    a, b = rd("wm/twin_pse_on_base_asi.txt"), rd("wm/faithful_MenuTeamSelectTick_on_base_asi.txt")
    assert "total cases: 9" in a and "mismatches:  2" in a and "idx=7" in a and "idx=8" in a and "mismatches:  0" in b
    return ("twin control: the retired clamped PlayerSlotEdgeAdjust (the .asi's INSTALLED body until now) is RED on exactly v7 and v8 (team -1 with UP, team 3), "
            "the faithful MenuTeamSelectTick GREEN 9/9 on the same asi")


def install():
    t = rd("wb3/install_observe_all.txt")
    assert "manifest: 1331 hooks seen, 1290 installed" in t and "survived 45s" in t
    man = (H / "wb3/install_all/hook_install_manifest.txt").read_text(errors="replace")
    for name in ("TimerBitFieldSet", "TimerSlotDataCopy", "TimerStructArrayClear", "TimerInitLoop", "MenuFlagDat007f0f10Get", "Search4c5c00", "Accum420de0",
                 "Pred405890", "MenuModeSync", "Find42ad90_RegAbi", "MenuTeamSelectTick"):
        assert len(re.findall(rf"\t{name}\s*$", man, re.M)) == 1, name
    nav = rd("wb3/nav_probe_full_hookset.txt")
    assert "drive_to_race=True" in nav and "proc.poll()=None" in nav
    return "full set 1290/1331 installed (each kept symbol once), survived 45 s, nav boot reaches a race alive"


def host():
    t = rd("host_tests_new.txt")
    for k in ("navsm: GREEN", "ruleengine: GREEN", "racemodes: GREEN"):
        assert k in t, k
    kv = rd("kvfind_equiv_test.txt")
    assert "cases=400000 mismatches=0" in kv
    return "host tests navsm/ruleengine/racemodes GREEN, KvLookup old-vs-new 400000 cases 0 mismatches"


def m3():
    t = rd("m3r_runs.txt")
    assert t.count("REPLAY GREEN") == 2 and t.count("rows=3888 exact=3888") == 4 and t.count("rows=6 exact=6") == 4
    return "exe M3 pickup replay (calls Pred405890) 6/6 and 3888/3888 at PC_24 and PC_53, same as base"


def main():
    raw = (ROOT / "hooks.csv").read_bytes().decode("utf-8")
    sio = io.StringIO(raw, newline="")
    recs, pos = [], 0
    for f in csv.reader(sio):
        end = sio.tell()
        recs.append([raw[pos:end], f, False])
        pos = end
    cols = recs[0][1]
    ix = {c: i for i, c in enumerate(cols)}
    by = {}
    for r in recs[1:]:
        if len(r[1]) == len(cols) and not r[0].startswith("#"):
            try:
                by[int(r[1][0], 16)] = r
            except ValueError:
                pass
    g = gctrl()
    common = f"exe G-CTRL {g} before and after (base 9b0248128); {install()}; Evidence {EV}/RESULT_dualcopy4.md"
    log = []

    def upd(rva, level, name, file, exe, fd, text, cl_line):
        r = by[rva]
        row = r[1]
        old = row[ix["confidence"]]
        assert old == level[0], (hex(rva), old)
        row[ix["confidence"]] = level[1]
        if name:
            row[ix["name"]] = name
        if file is not None:
            row[ix["file"]] = file
        if exe is not None:
            row[ix["exe_file"]] = exe
        if fd:
            row[ix["frida_diff"]] = fd
        row[ix["notes"]] += " | " + text
        r[2] = True
        log.append(f"{DATE}  {rva:08x}  {row[ix['name']]}  {level[0]}->{level[1]}{' (RE-EARNED)' if level[0] == level[1] or level[1] > level[0] else ' (DEMOTED)'}  {cl_line} Evidence {EV}/RESULT_dualcopy4.md.")

    tm = f"{p1('timer_bit_field_set')}; {p2('timer_bit_field_set', 'wb2')}"
    upd(0x0041EDA0, ("C3", "C3"), "TimerBitFieldSet", P + "Util/TimerInit.cpp", P + "Util/TimerInit.cpp", f"{EV}/wb1/diff_timer_bit_field_set.csv",
        f"{DATE} (dualcopy4) ONE body Util/TimerInit.cpp:TimerBitFieldSet; the TimerSlot.cpp copy SlotBitSet (never installed, no caller) and its registry key slot_bit_set deleted. "
        f"{tm}; {mut()}; the exe has no caller of this RVA. {common}",
        "One body; TimerSlot.cpp SlotBitSet deleted; path1+path2+mutant control.")
    upd(0x0041F000, ("C3", "C3"), "TimerSlotDataCopy", P + "Util/TimerInit.cpp", P + "Util/TimerInit.cpp", f"{EV}/wb1/diff_timer_slot_data_copy.csv",
        f"{DATE} (dualcopy4) ONE body Util/TimerInit.cpp:TimerSlotDataCopy; TimerSlot.cpp SlotDataCopy deleted (key slot_data_copy was already gone). "
        f"{p1('timer_slot_data_copy')}; {p2('timer_slot_data_copy', 'wb2')} (path2 vectors repaired: they were one list argument -> 'bad argument count' before entry); {common}",
        "One body; TimerSlot.cpp SlotDataCopy deleted; path1+path2.")
    upd(0x00420D40, ("C3", "C3"), "TimerStructArrayClear", P + "Util/TimerInit.cpp", P + "Util/TimerInit.cpp", f"{EV}/wb1/diff_timer_array_clear_d40.csv",
        f"{DATE} (dualcopy4) ONE body Util/TimerInit.cpp:TimerStructArrayClear; TimerSlot.cpp SlotArrayClear and key slot_array_clear deleted. "
        f"{p1('timer_array_clear_d40')}; {p2('timer_array_clear_d40', 'wb2')}; {common}",
        "One body; TimerSlot.cpp SlotArrayClear deleted; path1+path2.")
    upd(0x00422120, ("C3", "C3"), None, None, P + "Util/TimerInit.cpp", None,
        f"{DATE} (dualcopy4) exe_file == file: Util/TimerSubarrayInit.cpp (the plain-call copy TimerInit.cpp:151 calls silently broken) deleted from the tree and both rsp lists. "
        f"Re-run on the final asi: {p1('timer_init_loop')}; {p2('timer_init_loop', 'wb2')} ({EV}/wb1/diff_timer_init_loop.csv); {common}",
        "TimerSubarrayInit.cpp deleted; exe_file == file.")
    upd(0x00431B70, ("C3", "C3"), None, None, P + "Frontend/MenuLeaves_af5.cpp", f"{EV}/wb1/diff_menu_flag_dat_007f0f10_get.csv",
        f"{DATE} (dualcopy4) ONE body Frontend/MenuLeaves_af5.cpp:MenuFlagDat007f0f10Get; BucketMixed_t3.cpp GetDat007f0f10 (never installed) and its STAGED key get_dat_007f0f10 deleted. "
        f"{p1('menu_flag_dat_007f0f10_get')}; {p2('menu_flag_dat_007f0f10_get', 'wb2')}; {common}",
        "One body; BucketMixed_t3.cpp copy deleted.")
    upd(0x004C5C00, ("C3", "C3"), None, P + "Util/PromoLoop_sessionB.cpp", None, f"{EV}/wb1/diff_search_4c5c00.csv",
        f"{DATE} (dualcopy4) file now names the evidence body Util/PromoLoop_sessionB.cpp:Search4c5c00 (the installed naked port). The asi-only Frontend/SpriteCluster.cpp:LinkedListStringSearch "
        f"(never installed, one dead caller SpriteLookupC) deleted; SpriteLookupC calls the RVA through a pointer. The exe has no body of this RVA (exe_file empty). "
        f"{ew('search_4c5c00')}; {p2('search_4c5c00', 'wb3')}; {common}",
        "SpriteCluster.cpp LinkedListStringSearch deleted; one body Search4c5c00.")
    r = by[0x004B4650]
    r[1][ix["notes"]] += (f" | {DATE} (dualcopy4): the dead third copy Render/PromoLoop_round22.cpp:Vec3Lerp deleted (never installed; the file held nothing else). Level unchanged: the "
                          "exe copy Powerup/PowerupContact.cpp:Vec3Lerp is still a separate body (C++ `t*d + a` with a float rounding between the multiply and the add, plus a Log() "
                          "call; the original keeps the unrounded x87 product, Util/PromoLoop_sessionB.cpp:Lerp4b4650 is the verbatim port). Unifying it needs the x87 body in the exe "
                          f"under a PC_24 scope: open. {EV}/RESULT_dualcopy4.md")
    r[2] = True
    log.append(f"{DATE}  004b4650  {r[1][ix['name']]}  (no ladder move)  dead round22 copy deleted; exe PowerupContact copy remains the open split. Evidence {EV}/RESULT_dualcopy4.md.")
    upd(0x00420DE0, ("C3", "C3"), None, P + "Util/PromoLoop_sessionB.cpp", None, f"{EV}/wb1/diff_accum_420de0.csv",
        f"{DATE} (dualcopy4) file now names the evidence body Util/PromoLoop_sessionB.cpp:Accum420de0 (naked x87, installed). The plain-C Vehicle/Damage.cpp:VehicleDamageAccumulatorAdd "
        f"(MASS-DISABLED, no caller, compared the rounded f32 sum where the original compares the unrounded st0 at 0x00420df1) and its /export pragma deleted. {ew('accum_420de0')}; "
        f"{p2('accum_420de0', 'wb2')}; {common}", "Dead C copy deleted; one body Accum420de0.")
    upd(0x00405890, ("C4", "C3"), None, P + "Race/CollectDone.cpp", P + "Race/CollectDone.cpp", f"{EV}/wb1/diff_pred_405890.csv",
        f"{DATE} (dualcopy4) C4 -> C3 DEMOTED: the body is no longer the one the 2026-06/08 C4 evidence ran. ONE body Race/CollectDone.h:CollectDoneBody405890 with three thin readers "
        f"(CollectDone.cpp:Pred405890 reads 0x0063a5d0/0x0063a5d4 and carries the install, RuleEngine.cpp:CollectAllDone reads Persist, PickupCollect_naked.inc's PK_458920 calls Pred405890); "
        f"Util/PromoLoop_round78.cpp and the naked PK_405890 deleted. {ew('pred_405890')}; {p2('pred_405890', 'wb2')}; {mut()}; {m3()}; {host()}. "
        "C4 is NOT re-earned: the canonical race (TRAINING, rule 4) never calls the predicate, so the strict-C4 rule (the body must execute with the hook installed) needs a mode-5 collect race. "
        f"Re-pickup: that run. {common}", "C4->C3: one body CollectDone.h, three thin readers; path1+path2+mutant+M3 replay; C4 needs a mode-5 collect race.")
    upd(0x0042F6B0, ("C3", "C3"), None, P + "Frontend/FrontendState.cpp", P + "Frontend/FrontendState.cpp", f"{EV}/wb1/diff_menu_mode_sync.csv",
        f"{DATE} (dualcopy4) ONE table Race/SelectionToGameMode.h:SelectionToGameModeBody; FrontendState.cpp:MenuModeSync (installed) calls it and writes DAT_0067e9fc only when the result is >= 0; "
        f"Race/RaceModes.cpp:SelectionToGameMode is a one-line wrapper (no RVA anchor). {p1('menu_mode_sync')} (negctl fixture, seeds DAT_0067f184); {p2('menu_mode_sync', 'wb2')}; {host()}; {common}",
        "One table SelectionToGameMode.h; RaceModes.cpp wrapper; path1+path2+host tests.")
    upd(0x0042AD90, ("C3", "C3"), None, P + "Frontend/KvFind.cpp", P + "Frontend/KvFind.cpp", f"{EV}/wb1/diff_find_42ad90.csv",
        f"{DATE} (dualcopy4) ONE body Frontend/KvFind.cpp:Find42ad90 in both rsp lists (the stack-ABI C body the registry drives); the .asi's installed naked shim Util/PromoLoop_sessionB.cpp:Find42ad90_RegAbi "
        f"calls it and MenuNavSM.cpp:KvLookup (exe, 14 call sites) is a call into it. {ew('find_42ad90')} (2 vectors: n=0 -> 0xAAAA, n=1 -> 0xBBBB); {host()}. "
        "path2 N/A: the registry key measures the stack-ABI export Find42ad90 but the installed hook is the register shim Find42ad90_RegAbi, so the rel32 check cannot match (pre-existing); "
        f"the install is covered by the manifest and the nav boot. {common}", "One body KvFind.cpp; KvLookup calls it; early-window GREEN, 400k equivalence, host tests.")
    upd(0x0042FA00, ("C2", "C3"), "MenuTeamSelectTick", P + "Frontend/MenuNav.cpp", P + "Frontend/MenuNav.cpp", f"{EV}/wb1/diff_menu_team_select_tick.csv",
        f"{DATE} (dualcopy4) C2->C3 RE-EARNED on ONE body Frontend/MenuNav.cpp:MenuTeamSelectTick, which follows the original (decomp_42fa00.txt: guard `!= 0` on the decrement, `< 2` on the increment, no clamp) and is now "
        f"the install. The .asi's previous installed body Frontend/SkeletonAndScatter_t6.cpp:PlayerSlotEdgeAdjust (guard `> 0` plus a [0,2] clamp) and its key player_slot_edge_adjust deleted. "
        f"{p1('menu_team_select_tick')} on 9 vectors incl. v7 (team -1 with UP -> original -2) and v8 (team 3 -> 3); {p2('menu_team_select_tick', 'wb3')}; {twin()}; {common}",
        "Faithful MenuTeamSelectTick installed, clamped asi twin deleted; path1 9/9 + path2 + twin control.")
    out = io.StringIO(newline="")
    for raw_rec, fields, changed in recs:
        if not changed:
            out.write(raw_rec)
        else:
            term = raw_rec[len(raw_rec.rstrip("\r\n")):]
            b = io.StringIO(newline="")
            csv.writer(b, lineterminator=term).writerow(fields)
            out.write(b.getvalue())
    cp = ROOT / "re" / "analysis" / "CHANGELOG.md"
    ct = cp.read_bytes().decode("utf-8")
    nl = "\r\n" if "\r\n" in ct else "\n"
    L = ct.split(nl)
    at = [i for i, l in enumerate(L) if l == "<!-- ENTRIES -->"]
    assert len(at) == 1
    L[at[0] + 1:at[0] + 1] = log
    print(f"{sum(1 for r in recs if r[2])} rows changed, {len(log)} changelog lines")
    for ln in log:
        print("  ", ln[:150])
    if "--apply" in sys.argv:
        (ROOT / "hooks.csv").write_bytes(out.getvalue().encode("utf-8"))
        cp.write_bytes(nl.join(L).encode("utf-8"))
        print("written")


main()
