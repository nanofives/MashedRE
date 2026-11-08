"""C4R-DXLIB-NEWROWS candidate builder (2026-11-07). Reads verify/c4rdxlib_20261107/d3dx_sum2003.tsv, hooks.csv and
re/console/pc_funcs.csv. For each identical-unlisted address with no hooks.csv row: object -> sub-tag (from the
library objects that existing d3dx9 rows matched with status identical, only when the object maps to one tag), function boundary
(pc_funcs entry == rva, body_size == lib_size: smaller does not cover the symbol, larger is a partial identity). Writes candidates.tsv (one line per address, verdict add | skip <why>).

  py -3.12 verify/c4rdxlibfu_20261107/newrows.py
"""
import csv, collections, pathlib
W = pathlib.Path(__file__).resolve().parents[2]
HERE = pathlib.Path(__file__).resolve().parent
T = list(csv.DictReader(open(W / "verify/c4rdxlib_20261107/d3dx_sum2003.tsv", encoding="utf-8"), delimiter="\t"))
hooks = {}
for l in open(W / "hooks.csv", encoding="utf-8"):
    k = l.split(",", 1)[0].lower()
    if len(k) == 8:
        hooks[k] = l
pf = {}
for r in csv.DictReader(open(W / "re/console/pc_funcs.csv", encoding="utf-8")):
    pf[int(r["entry"], 16)] = r
tag = collections.defaultdict(collections.Counter)
for r in T:
    if r["subsystem"].startswith("third-party-library[d3dx9") and r["library_object"] and r["status"] == "identical":
        tag[r["library_object"]][r["subsystem"]] += 1
by = collections.defaultdict(list)
for r in T:
    if r["status"] == "identical-unlisted":
        by[r["rva"]].append(r)
out = []
for rva in sorted(by, key=lambda s: int(s, 16)):
    k = f"{int(rva, 16):08x}"
    if k in hooks:
        continue
    ents = by[rva]
    names = sorted({e["lib_function"] for e in ents})
    objs = sorted({e["library_object"] for e in ents})
    sizes = sorted({int(e["lib_size"]) for e in ents})
    p = pf.get(int(rva, 16))
    why = []
    if len(names) != 1:
        why.append(f"ambiguous: {len(names)} library functions")
    if len(objs) != 1:
        why.append(f"{len(objs)} objects")
    t = ""
    if len(objs) == 1:
        c = tag.get(objs[0])
        if not c:
            why.append(f"no sub-tag: object {objs[0]} matched by no d3dx9 row")
        elif len(c) > 1:
            why.append(f"sub-tag ambiguous: {objs[0]} -> {'/'.join(sorted(c))}")
        else:
            t = next(iter(c))
    if p is None:
        a = int(rva, 16)
        inside = [q for e, q in pf.items() if e < a < int(q["body_end"], 16)]
        why.append("no pc_funcs entry at rva" + (f" (inside {inside[0]['entry']} {inside[0]['name']})" if inside else " (in no pc_funcs body)"))
    elif int(p["body_size"]) < max(sizes):
        why.append(f"size: pc_funcs body {p['body_size']} < lib {max(sizes)}")
    elif int(p["body_size"]) > max(sizes):
        why.append(f"size: pc_funcs body {p['body_size']} B > symbol {max(sizes)} B (identity covers only the first {max(sizes)} B)")
    out.append([f"0x{k}", names[0] if len(names) == 1 else "|".join(names[:3]), objs[0] if len(objs) == 1 else "|".join(objs),
                str(max(sizes)), p["body_size"] if p else "", p["name"] if p else "", t,
                "add" if not why else "skip", "; ".join(why)])
with open(HERE / "candidates.tsv", "w", encoding="utf-8", newline="") as f:
    w = csv.writer(f, delimiter="\t", lineterminator="\n")
    w.writerow(["rva", "lib_function", "library_object", "lib_size", "pc_body_size", "pc_name", "subsystem", "verdict", "why"])
    w.writerows(out)
c = collections.Counter(r[7] for r in out)
print(f"addresses {len(out)}: {dict(c)}")
for r in out:
    if r[7] != "add":
        print("  ", r[0], r[1][:60], "|", r[8])
print(collections.Counter(r[6] for r in out if r[7] == "add"))
