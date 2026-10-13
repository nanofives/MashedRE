# aggregate_sites.py <screen> ...   -- reads quad_<screen>.json (recheck_quad_diff.py --json) and groups the
# per-call rows by original call site (return address) and print scale. Per site: screens, glyphs, matched
# (post), median of the per-call dx / dh (post), dy pre and dy post ranges, and whether |dy| grows on any call.
import json, sys, statistics as st
sites = {}
for s in sys.argv[1:]:
    try: rows = json.load(open(f"quad_{s}.json"))
    except FileNotFoundError: continue
    for r in rows:
        k = (r["ret"], round(r["scale"] or 0, 3))
        e = sites.setdefault(k, dict(screens=set(), n=0, m=0, dx=[], dh=[], pre=[], post=[], grows=False, unm=0))
        e["screens"].add(int(s)); e["n"] += r["glyphs"]; e["m"] += r["post"]["matched"]
        if r["post"]["matched"] == 0: e["unm"] += 1; continue
        e["dx"].append(r["post"]["dx"]); e["dh"].append(r["post"]["dh"])
        if r["pre"]["dy"] is not None: e["pre"].append(r["pre"]["dy"])
        e["post"].append(r["post"]["dy"])
        if r["pre"]["dy"] is not None and abs(r["post"]["dy"]) > abs(r["pre"]["dy"]) + 1e-3: e["grows"] = True
rng = lambda a: "n/a" if not a else (f"{min(a):+.4f}" if max(a) - min(a) < 5e-5 else f"{min(a):+.4f}..{max(a):+.4f}")
print("| ret | scale | screens | glyphs | matched | dx (post, median) | dh (post, median) | dy pre | dy post | grows |")
print("|---|---|---|---|---|---|---|---|---|---|")
for k in sorted(sites):
    e = sites[k]
    md = lambda a: "n/a" if not a else f"{st.median(a):+.4f}"
    sc = ",".join(str(v) for v in sorted(e["screens"]))
    print(f"| `{k[0]}` | {k[1]} | {sc} | {e['n']} | {e['m']} | {md(e['dx'])} | {md(e['dh'])} | {rng(e['pre'])} | {rng(e['post'])} | {'**GROWS**' if e['grows'] else ('unmatched' if not e['post'] else '')} |")
