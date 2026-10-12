# Compare c4_front_scenario.py runs: every port/ctl arm and the second original run against stock1.
#   boot rows (subinit, bqf): the ordered callee log inside the row (pointer args tokenised by the
#     harness; begin()'s return is a pointer that differs per boot, so it is reduced to NZ/0).
#   bc: per call key (4 stack floats, AL, screen gate, return address) -> the five 0x00472c60 arg
#     tuples. A call is FOREIGN when its (key, output) pair never occurs in the reference.
# Usage: py -3.12 re/frida/c4_front_compare.py <evidence dir>   -> c4_front_compare.csv + _summary.json
import csv, json, re, sys
from pathlib import Path


def norm_log(log):
    out = []
    for e in log:
        m = re.match(r"^(begin\(P\d+\))=([0-9a-f]+)$", e)
        if m: e = m.group(1) + "=" + ("0" if m.group(2) == "0" else "NZ")
        out.append(e)
    return out


def load(d, tag):
    p = d / f"c4_front_{tag}.json"
    return json.loads(p.read_text(encoding="utf-8")) if p.exists() else None


def main():
    d = Path(sys.argv[1])
    rows = []
    for row in ("subinit", "bqf", "bc"):
        ref = load(d, f"{row}_stock1")
        if not ref: continue
        for tag in (f"{row}_stock2", f"{row}_port1", f"{row}_port2", f"{row}_ctl1", f"{row}_ctl2"):
            r = load(d, tag)
            if not r: continue
            w = r.get("arm_result", {}).get("w", {})
            base = {"row": row, "run": tag, "ref": f"{row}_stock1", "verdict_run": r["verdict"],
                    "first_bytes": w.get("bytes"), "target": (w.get("module") or "") + (("+" + w["off"]) if w.get("off") else ""),
                    "export": ",".join(w.get("exports") or []), "game_calls": r["dump"]["st"]["calls"],
                    "crash": len(r.get("crash", [])), "asi_sha256": (r.get("asi_sha256") or "")[:16]}
            if row in ("subinit", "bqf"):
                a, b = norm_log(ref["dump"]["log"]), norm_log(r["dump"]["log"])
                diff = [i for i in range(max(len(a), len(b))) if (a[i] if i < len(a) else None) != (b[i] if i < len(b) else None)]
                first = diff[0] if diff else None
                base.update({"compared": len(a), "observed": len(b), "mismatch": len(diff),
                             "first_mismatch": "" if first is None else f"#{first}: ref={a[first] if first < len(a) else '-'} run={b[first] if first < len(b) else '-'}",
                             "result": "GREEN" if not diff and r["verdict"] == "DONE" else "RED"})
            else:
                # The inputs are game state and can differ per boot (the 0x0043667b plate's width follows the
                # race outcome), so the reference is the POOLED original: stock1 + stock2 (stock2 itself is
                # compared to stock1 alone, which is the original's noise). Per call:
                #   matched  : the key occurs in the reference and the output equals the reference output
                #   MISMATCH : the key occurs in the reference with a different output  -> RED
                #   unmatched: the key never occurs in the reference -> not comparable, reported
                refs = [ref] if tag.endswith("stock2") else [x for x in (ref, load(d, f"{row}_stock2")) if x]
                refout = {}
                for rr in refs:
                    for kk, v in rr["dump"]["bc"]["keys"].items():
                        refout.setdefault(kk, set()).update(v["outs"])
                k = r["dump"]["bc"]["keys"]
                calls = sum(v["n"] for v in k.values())
                mism = sum(c for kk, v in k.items() if kk in refout for o, c in v["outs"].items() if o not in refout[kk])
                unm = sum(v["n"] for kk, v in k.items() if kk not in refout)
                ex = next(((kk, o) for kk, v in k.items() if kk in refout for o in v["outs"] if o not in refout[kk]), None)
                base.update({"ref": "+".join(x["tag"] for x in refs), "compared": calls - unm, "observed": calls,
                             "mismatch": mism, "unmatched": unm,
                             "first_mismatch": (f"key={ex[0]} out={ex[1][:120]}" if ex else ""),
                             "window": json.dumps(r.get("window")),
                             "result": ("RED" if mism else ("GREEN" if unm == 0 else "GREEN-PARTIAL"))
                                       if calls > 0 and r["verdict"] == "DONE" else "VOID"})
            rows.append(base)
    cols = ["row", "run", "ref", "result", "verdict_run", "game_calls", "compared", "observed", "mismatch", "unmatched",
            "first_mismatch", "first_bytes", "target", "export", "crash", "asi_sha256", "window"]
    with open(d / "c4_front_compare.csv", "w", newline="", encoding="utf-8") as f:
        wr = csv.DictWriter(f, fieldnames=cols, extrasaction="ignore"); wr.writeheader(); wr.writerows(rows)
    (d / "c4_front_compare_summary.json").write_text(json.dumps(rows, indent=1), encoding="utf-8")
    for r in rows:
        print(f"{r['run']:<16} {r['result']:<5} calls={r['game_calls']} compared={r['compared']} mismatch={r['mismatch']} unmatched={r.get('unmatched','')} "
              f"target={r['target']} {r['first_mismatch'][:110]}")


if __name__ == "__main__":
    main()
