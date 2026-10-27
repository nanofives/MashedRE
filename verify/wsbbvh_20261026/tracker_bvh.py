"""hooks.csv + CHANGELOG transaction for D4-WSB-C4-BVH (re-classify side effects 1 and 4).

Promotion C3 -> C4 needs, per row (re/CONFIDENCE.md C4, WORKER_RULES strict C4, lead note on
out-memory 2026-10-26):
  * racediff C4-CLEAN on a final tool version (v3 / v5; marker 'ON-observed callee arities' in the
    transcript; v4 rows are superseded), hook installed (0xE9), calls observed on both sides;
  * out-memory covered: either the original body has no store outside its own frame (cat A, the
    racediff's calls + args + return are its whole effect), or an armed write-tracked shadow A/B
    (RunTracked / RunRegion) in the census scenario with the hook installed is CLEAN (cat B);
  * no `STUB S-` marker in the TU; exe_file is one TU compiled into both targets.
Everything else stays C3, with the reason appended to notes (no ladder move, no CHANGELOG line).

    py -3.12 tracker_bvh.py            # dry run
    py -3.12 tracker_bvh.py --apply
"""
import csv
import io
import pathlib
import sys

ROOT = pathlib.Path(__file__).resolve().parent.parent.parent
HERE = pathlib.Path(__file__).resolve().parent
DATA = pathlib.Path("D:/MashedWT/D4-WSB-C4-BVH_data")
DATE = "2026-10-26"
EV = "verify/wsbbvh_20261026"
RES = f"{EV}/RESULT_D4-WSB-C4-BVH.md"
SCEN = "c4_racediff_bvh:QuickBattle-f300-1200"

CAT_A = ["0055fe50", "0055ff70", "00561c50", "00561e60", "00561e80", "005641b0", "00565550"]
CAT_B = ["005601f0", "00560260", "00563f60", "00565120", "005646c0", "005651b0"]
HOLD = {
    "0055fea0": "racediff C4-CLEAN, but the body stores through pointers (9 non-stack stores, 0x0055fee4..) and "
                "neither leg compared out-memory: the racediff compares call-outs + return, the C3 shadow is "
                "ShadowAB::Run (return only). Needs a write-set compare (RunTracked wrapper or a store tracer)",
    "0055ff90": "racediff C4-CLEAN, but 4 non-stack stores (0x0055ffa4..) and no out-memory compare (Run wrapper). "
                "Needs a write-set compare",
    "00561040": "racediff C4-CLEAN, but 2 non-stack stores (+0x5c at 0x00561061 / 0x00561242) and no out-memory "
                "compare (Run wrapper). Needs a write-set compare",
    "00561390": "racediff C4-CLEAN, but 31 non-stack stores + rep stos (0x005613c9) and no out-memory compare "
                "(Run wrapper). Needs a write-set compare",
    "00564040": "racediff C4-CLEAN, but 3 non-stack stores (0x005640a0..) and no out-memory compare (Run "
                "wrapper). Needs a write-set compare",
    "00564c80": "racediff BEHAVIOR-DIFF: 3rd arg of the call to 0x005646c0 is param_2 unmasked in the original "
                "(push edx at 0x00564cac, edx = [esp+0x54] at 0x00564c83) and (ushort)param_2 in the port "
                "(RwpSolverCore4.cpp:273); whether 0x005646c0 reads the high half is not established",
    "00565260": "racediff BEHAVIOR-DIFF on EAX only (port declares void; original leaves 0/1/5 where the port "
                "leaves 0x16/0xe/..). Not unobservable: caller 0x0055aeff writes AX only and pushes the full EAX "
                "at 0x0055af06 as param_2 of 0x00564c80, which reads param_2 in full at 0x00564c83",
    "00563e70": "not observed in the Quick Battle window on either side (A4 calls 300-1200, 2 boots; 1-3000, "
                "2 boots); census EXEC is track 0 / mode 10 / 4 cars. Needs a racediff on that scenario",
    "00565200": "not observed in the Quick Battle window on either side (A4 calls 300-1200, 2 boots; 1-3000, "
                "2 boots); census EXEC race_run. Needs a racediff on the census scenario",
}


def tsv(p):
    return {r["rva"][2:].zfill(8): r for r in csv.DictReader(io.StringIO(p.read_text(encoding="utf-8")), delimiter="\t")}


race = tsv(HERE / "racediff_summary.tsv")
shad = tsv(HERE / "shadow_summary.tsv") if (HERE / "shadow_summary.tsv").exists() else {}
asi = (ROOT / "mashedmod/asi_sources.rsp").read_text(encoding="utf-8")
exe = (ROOT / "mashedmod/exe_sources.rsp").read_text(encoding="utf-8")


def final_tool(rva8):
    """v3 (no 'return kind=' line) or v5 ('bvh tool v5'); v4 ('return kind=' without the v5 line) is superseded."""
    t = (DATA / rva8 / "transcript.txt").read_text(encoding="utf-8", errors="replace")
    if "ON-observed callee arities" not in t:
        return False
    return "bvh tool v5" in t or "return kind=" not in t


promote, problems = {}, []
for rva8 in CAT_A + CAT_B:
    s = race.get(rva8)
    ok = (s and s["verdict"] == "C4-CLEAN" and s["installed"] == "True" and int(s["off_calls"]) > 0
          and int(s["on_calls"]) > 0 and final_tool(rva8))
    if rva8 in CAT_B:
        w = shad.get(rva8)
        ok = ok and w and w["verdict"] == "CLEAN"
    if ok:
        promote[rva8] = (s, shad.get(rva8))
    else:
        problems.append(rva8)
print("promote", len(promote), sorted(promote))
print("not promotable (gate failed, stays C3):", problems)
hold = dict(HOLD)
for rva8 in problems:
    s, w = race.get(rva8, {}), shad.get(rva8, {})
    hold[rva8] = (f"gate failed: racediff {s.get('verdict')} {s.get('why')}"
                  + (f", write-set shadow {w.get('verdict')}" if rva8 in CAT_B else ""))

hp = ROOT / "hooks.csv"
lines = hp.read_text(encoding="utf-8").splitlines(keepends=True)
out, changelog = [], []
for ln in lines:
    if not ln.strip() or ln.startswith("#") or ln.startswith("rva,"):
        out.append(ln); continue
    row = next(csv.reader([ln.rstrip("\r\n")]))
    rva8 = row[0]
    if rva8 not in promote and rva8 not in hold:
        out.append(ln); continue
    assert row[3] == "C3", (rva8, row[3])
    tu = pathlib.Path(row[9]).name
    assert tu and tu in asi and tu in exe, (rva8, tu)
    if rva8 in promote:
        s, w = promote[rva8]
        assert "STUB S-" not in (ROOT / row[9]).read_text(encoding="utf-8", errors="replace"), rva8
        mem = ("out-memory: the original body has no store outside its own frame, so its calls + args + return "
               "are its whole effect" if rva8 in CAT_A else
               f"out-memory: armed write-tracked shadow A/B, hook installed alone, track 0 / mode 10 / 4 cars, "
               f"{w['n']} samples {w['ndiff']} divergent, {w['proof']}")
        row[3], row[4], row[6], row[7] = "C4", "verified", SCEN, f"{EV}/racediff_summary.tsv"
        row[8] += (f" || C4 {DATE} (D4-WSB-C4-BVH): canonical in-race Stalker racediff, hook installed alone "
                   f"(MASHED_HOOK_ONLY=0x{rva8}, JMP 0xE9), Quick Battle A4 calls 300-1200, ON then OFF boot, "
                   f"{s['on_calls']}/{s['off_calls']} calls, {s['on_sigs']} signature each side, equal "
                   f"(callee, args, return) with heap/stack values relative to the entry args. {mem}. "
                   f"Census EXEC race_run 3/3. One body: {tu} in both targets. {RES}")
        changelog.append(f"{DATE}  {rva8}  {row[1]}  C3->C4  D4-WSB-C4-BVH: in-race racediff C4-CLEAN (hook "
                         f"installed alone, args compared); "
                         + ("no store outside its frame" if rva8 in CAT_A else "write-tracked shadow CLEAN")
                         + f"; {RES}\n")
    else:
        row[8] += f" || C4 attempt {DATE} (D4-WSB-C4-BVH): stays C3. {hold[rva8]}. {RES}"
    b = io.StringIO()
    csv.writer(b, lineterminator="").writerow(row)
    out.append(b.getvalue() + ("\r\n" if ln.endswith("\r\n") else "\n"))

print("hold", len(hold), sorted(hold))
if "--apply" in sys.argv:
    hp.write_text("".join(out), encoding="utf-8", newline="")
    cp = ROOT / "re/analysis/CHANGELOG.md"
    c = cp.read_text(encoding="utf-8")
    mk = "<!-- ENTRIES -->\n"
    assert c.count(mk) == 1
    cp.write_text(c.replace(mk, mk + "".join(changelog), 1), encoding="utf-8", newline="")
    print("applied")
