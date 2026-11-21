import csv, collections
R = list(csv.DictReader(open("t2_nonc4.tsv", encoding="utf-8"), delimiter="\t"))
def cls(s):
    s = s.lower()
    if "lua" in s or "qhull" in s or "crt" in s or "libpng" in s or "zlib" in s: return "library (Lua/qhull/CRT)"
    if "renderware-physics" in s: return "RW-Physics"
    if "renderware" in s: return "RenderWare core"
    if s == "": return "no hooks.csv row"
    return "game code"
def src(r):
    p = r["best_pct"]
    if p == "None": return "no source"
    p = float(p)
    return ">=80" if p >= 80 else "50-80" if p >= 50 else "<50"
t = collections.defaultdict(collections.Counter); calls = collections.Counter(); ins = collections.defaultdict(collections.Counter)
for r in R:
    c = cls(r["subsystem"]); t[c][src(r)] += 1; calls[c] += int(r["t2_calls"])
    v = r["insitu"].split("@")[0] or "untested"
    ins[c][v] += 1
tot = sum(calls.values())
print(f"{'class':26s} {'n':>4s} {'>=80':>5s} {'50-80':>6s} {'<50':>5s} {'nosrc':>6s} {'calls%':>7s}  in-situ(11-11)")
for c in sorted(t, key=lambda c: -sum(t[c].values())):
    n = sum(t[c].values())
    print(f"{c:26s} {n:4d} {t[c]['>=80']:5d} {t[c]['50-80']:6d} {t[c]['<50']:5d} {t[c]['no source']:6d} {100*calls[c]/tot:6.1f}%  {dict(ins[c])}")
g = [r for r in R if cls(r["subsystem"]) == "game code"]
print("\ngame code by subsystem:", dict(collections.Counter(r["subsystem"] for r in g).most_common()))
print("game code by level:", dict(collections.Counter(r["level"] for r in g)))
print("game code, no source, by level:", dict(collections.Counter(r["level"] for r in g if r["best_pct"] == "None")))
