"""Fill RESULT_D4-WSB-C4-BVH.md placeholders from racediff_summary.tsv, shadow_summary.tsv and the raw dir."""
import csv, hashlib, io, json, pathlib, re, sys
HERE = pathlib.Path(__file__).resolve().parent
DATA = pathlib.Path("D:/MashedWT/D4-WSB-C4-BVH_data")
import types
T = types.SimpleNamespace()   # the category constants only (the tracker asserts C3 rows, i.e. it is not re-runnable once applied)
_src = (HERE / "tracker_bvh.py").read_text(encoding="utf-8")
_ns = {}; exec(_src[_src.index("CAT_A = "):_src.index("def tsv(")], _ns)
T.CAT_A, T.CAT_B, T.HOLD = _ns["CAT_A"], _ns["CAT_B"], _ns["HOLD"]

def tsv(p):
    return {r["rva"][2:].zfill(8): r for r in csv.DictReader(io.StringIO(p.read_text(encoding="utf-8")), delimiter="\t")}
race, shad = tsv(HERE / "racediff_summary.tsv"), tsv(HERE / "shadow_summary.tsv")
ALL = sorted(set(T.CAT_A) | set(T.CAT_B) | set(T.HOLD))
def compared(r):
    d = json.loads((DATA / r / "c4_racediff_samples.json").read_text(encoding="utf-8"))
    sig = next(iter(next(iter(d["off"].values()), {}) or {"-": 0}))
    calls = len(re.findall(r"\[c[0-9a-f]+", sig)); ret = sig.split(" ")[-1] if sig != "-" else "-"
    return f"{calls} call(s), {ret.split('=')[0]}" if sig != "-" else "nothing captured"
rows = ["| rva | racediff | calls ON/OFF | sigs | compared | out-memory | level |", "|---|---|---|---|---|---|---|"]
for r in ALL:
    s = race[r]; w = shad.get(r)
    mem = ("no store outside own frame" if r in T.CAT_A else
           f"write-tracked shadow {w['verdict']} {w['n']}/{w['ndiff']}" if r in T.CAT_B and w else
           "not compared" if r in T.HOLD else "-")
    lvl = "**C4**" if r in T.CAT_A + T.CAT_B else "C3"
    rows.append(f"| `0x{r}` | {s['verdict']} {s['why']} | {s['on_calls']}/{s['off_calls']} | {s['on_sigs']}/{s['off_sigs']} "
                f"| {compared(r)} | {mem} | {lvl} |")
holds = "\n".join(f"- `0x{k}`: {v}." for k, v in sorted(T.HOLD.items()))
raw = ["| file | SHA-256 |", "|---|---|"]
for r in ALL:
    for f in ("transcript.txt", "c4_racediff_samples.json", "c4_racediff_result.json", "shadow_ab.log"):
        p = DATA / r / f
        if p.exists(): raw.append(f"| `{r}/{f}` | `{hashlib.sha256(p.read_bytes()).hexdigest()}` |")
for f in ("batch2.log", "batch3.log", "batch3b.log", "batch4.log", "shadow1.log"):
    p = DATA / f
    if p.exists(): raw.append(f"| `{f}` | `{hashlib.sha256(p.read_bytes()).hexdigest()}` |")
res = HERE / "RESULT_D4-WSB-C4-BVH.md"
t = res.read_text(encoding="utf-8")
t = (t.replace("__TABLE__", "\n".join(rows)).replace("__HOLDS__", holds).replace("__RAW__", "\n".join(raw))
      .replace("__NPROMO__", str(len(T.CAT_A) + len(T.CAT_B))).replace("__NHOLD__", str(len(T.HOLD))))
res.write_text(t, encoding="utf-8")
print("filled")
