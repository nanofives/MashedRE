# Compare c4_detect_e_scenario.py arms -> CSV + verdict.
# Per press: E returned 1 (count), FUN_0042c510 entries via the 0x0067ec28 flag path (return
# 0x0042c9c4, c510e) and via any other caller (c510o: the scroll-driven 0x0042d01b call, whose
# timing varies run to run even between two stock runs), page index delta (includes c510o). Per arm: detector calls,
# mismatches against the byte model of 0x0042b770 (mmOrig) and of the pre-dc98c93a port (mmOld),
# hook-live witness (first byte at 0x0042b770 before our attach, E9 target module).
# A test arm is GREEN vs the reference iff every press row matches on (players, eRet1, c510e)
# and its mmOrig == 0. c510o / page delta are reported but not compared (non-deterministic).
# Usage: py -3.12 re/frida/c4_detect_e_compare.py <dir> ref=off1 off2 on1 on2 ctl1 ctl2
import csv, json, sys
from pathlib import Path

d = Path(sys.argv[1]); tags = [t for t in sys.argv[2:] if not t.startswith("ref=")]
ref = [t.split("=", 1)[1] for t in sys.argv[2:] if t.startswith("ref=")][0]
R = {t: json.loads((d / f"c4_detect_e_{t}.json").read_text(encoding="utf-8")) for t in [ref] + tags}

def row(p):
    return (",".join(map(str, p["players"])), int(p["eRet1"] > 0), p["eRet1"], p.get("c510e", -1),
            p["page1"] - p["page0"], p["gate"], p.get("c510o", -1))

out = d / f"c4_detect_e_compare_ref{ref}.csv"
with out.open("w", newline="", encoding="utf-8") as f:
    w = csv.writer(f)
    w.writerow(["arm", "press_idx", "players", "e_ret1_any", "e_ret1", "c510_flagpath", "page_delta", "gate",
                "c510_other", "ref_e_ret1", "ref_c510_flagpath", "match"])
    summ = []
    rp = [row(p) for p in R[ref].get("presses", [])]
    for t in [ref] + tags:
        r = R[t]; tp = [row(p) for p in r.get("presses", [])]
        n_mis = 0
        for i, x in enumerate(tp):
            y = rp[i] if i < len(rp) else None
            m = (y is not None and x[0] == y[0] and x[2] == y[2] and x[3] == y[3])
            n_mis += (not m)
            w.writerow([t, i, x[0], x[1], x[2], x[3], x[4], x[5], x[6], y[2] if y else "",
                        y[3] if y else "", "MATCH" if m else "DIFF"])
        st = r.get("dump", {}).get("st", {})
        complete = r.get("verdict") == "DONE" and len(tp) == len(rp) and len(rp) > 0
        green = complete and n_mis == 0 and st.get("mmOrig", -1) == 0
        summ.append({"arm": t, "verdict_run": r.get("verdict"), "presses": len(tp), "press_diffs": n_mis,
                     "eCalls": st.get("eCalls"), "eRet1": st.get("eRet1"), "mmOrig": st.get("mmOrig"),
                     "mmOld": st.get("mmOld"), "diffModels": st.get("diffModels"),
                     "b0_before_attach": hex(st.get("jmp", -1)) if isinstance(st.get("jmp"), int) else st.get("jmp"),
                     "entry": st.get("entry"), "c510by": r.get("dump", {}).get("c510by"),
                     "crash": len(r.get("crash", [])), "alive_at_end": r.get("alive_at_end"),
                     "reached": (r.get("reached") or {}).get("gate"),
                     "result": ("REF" if t == ref else ("GREEN" if green else ("RED" if complete else "INCOMPLETE")))})
for s in summ: print(json.dumps(s))
(d / f"c4_detect_e_compare_ref{ref}_summary.json").write_text(json.dumps(summ, indent=1), encoding="utf-8")
print(f"-> {out}")
