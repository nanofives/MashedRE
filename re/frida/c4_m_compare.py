# Summarise the c4_m_scenario.py run matrix (C4 group M). Per run and row: hook state, game calls,
# per-call byte-model mismatches (mmOrig = vs the original bytes, mmCtl = vs the row's old defect),
# discriminating calls, and the consumer channel: per press-window ret==1 counts and the menu state
# (depth/type/cursor) at the end of each window, compared against a setup-matched reference run.
#
# Usage: py -3.12 re/frida/c4_m_compare.py <runs_dir> [--csv OUT]
import json, sys, csv
from pathlib import Path

ROWS = ["A", "B", "C", "T", "F"]
REF = {"off": "off1", "on": "off1", "stock": "stock1", "late": "stock1"}


def load(d):
    out = {}
    for p in sorted(Path(d).glob("c4_m_*.json")):
        r = json.loads(p.read_text(encoding="utf-8"))
        out[r["tag"]] = r
    return out


def wsig(r, row):
    """per-window (ret1 of the row, end-of-window menu state) for press windows only"""
    ws = r.get("dump", {}).get("WS", {})
    sig = {}
    for w in r.get("windows", []):
        n = w["win"]; s1 = w["s1"]
        # hold windows: the repeat count inside a 1.5 s hold is frame-timing noise on the original
        # itself (off1 vs off2 differ by one repeat), so only the end-of-window menu state is compared
        r1 = None if "hold" in n else ws.get(n, {}).get(row, {}).get("ret1", 0)
        sig[n] = (r1, (s1["d"], s1["t"], s1["sub"], s1["row"], s1["col"], s1["cur6"]))
    return sig


def main():
    d = sys.argv[1]
    runs = load(d)
    rows_out = []
    for tag, r in runs.items():
        ref_tag = REF[r["arm"]]
        if r.get("force_f"): ref_tag = "offF1"
        ref = runs.get(ref_tag)
        st = r.get("dump", {}).get("ST", {})
        arm = r.get("dump", {}).get("ARM", {})
        for row in ROWS:
            e = st.get(row, {})
            a = arm.get(row, {})
            line = {"run": tag, "arm": r["arm"], "force_f": r.get("force_f"), "row": row,
                    "live": row in r.get("rows_live", []), "b0_pre": r.get("witness_pre_attach", {}).get(row, {}).get("b0"),
                    "b0_after_load": r.get("witness_after_load", {}).get(row, {}).get("b0") if r.get("witness_after_load") else None,
                    "entry": a.get("mod"), "calls": e.get("calls", 0), "ret1": e.get("ret1", 0),
                    "mmOrig": e.get("mmOrig", 0), "mmCtl": e.get("mmCtl", 0), "disc": e.get("disc", 0),
                    "unscored": e.get("unscored", 0), "cwSplit": e.get("cwSplit", 0),
                    "st1": e.get("st1"), "padNZ": e.get("padNZ"), "byP": json.dumps(e.get("byP")) if e.get("byP") else "",
                    "verdict": r.get("verdict"), "crash": len(r.get("crash", [])), "alive_end": r.get("alive_at_end"),
                    "exit": r.get("exit_code"), "ref": ref_tag}
            if ref and ref is not r:
                a1, b1 = wsig(r, row), wsig(ref, row)
                common = [k for k in b1 if k in a1]
                line["win_match"] = f"{sum(1 for k in common if a1[k] == b1[k])}/{len(common)}"
                line["win_diff"] = ";".join(f"{k}:{a1[k]}vs{b1[k]}" for k in common if a1[k] != b1[k])[:400]
            rows_out.append(line)
    keys = list(rows_out[0].keys()) + ["win_match", "win_diff"]
    keys = list(dict.fromkeys(keys))
    if "--csv" in sys.argv:
        p = Path(sys.argv[sys.argv.index("--csv") + 1])
        with p.open("w", newline="", encoding="utf-8") as f:
            w = csv.DictWriter(f, fieldnames=keys); w.writeheader()
            for x in rows_out: w.writerow(x)
        print("->", p)
    for x in rows_out:
        if not x["live"] and x["arm"] in ("on", "late") : continue
        print(f"{x['run']:8} {x['row']} live={int(x['live'])} b0={x['b0_pre']}/{x['b0_after_load']} calls={x['calls']:6} ret1={x['ret1']:4} "
              f"mmO={x['mmOrig']:5} mmC={x['mmCtl']:5} disc={x['disc']:5} {x['verdict']} crash={x['crash']} "
              f"win={x.get('win_match','-')} {x.get('win_diff','')[:160]}")


if __name__ == "__main__":
    main()
