"""Score every RW-slice match source under ddk and rw9044 (MATCH-RW-COMPILER-9044)."""
import sys, pathlib, subprocess, re, concurrent.futures as cf
root = pathlib.Path(__file__).resolve().parents[2]
files = sorted(p for d in sys.argv[1:] for p in (root / "re" / "match").glob(d + "/*.c"))
pat = re.compile(r"^(0x[0-9a-f]+) (\S+): ([\d.]+)%.*?(\d+)/(\d+) ins", re.M)
def run(f, tc):
    r = subprocess.run([sys.executable, str(root / "re/tools/match/match.py"), str(f), "--toolchain", tc, "--bytes"],
                       capture_output=True, text=True)
    out = r.stdout + r.stderr
    res, ident, cur = {}, set(), None
    for ln in out.splitlines():
        m = pat.match(ln)
        if m: cur = m.group(1); res[cur] = float(m.group(3))
        elif cur and "BYTE-IDENTICAL" in ln: ident.add(cur)
    return f, tc, res, ident, (None if res else out.strip().splitlines()[-1:] )
jobs = [(f, tc) for f in files for tc in ("ddk", "rw9044")]
rows = {}
with cf.ThreadPoolExecutor(8) as ex:
    for f, tc, res, ident, err in ex.map(lambda j: run(*j), jobs):
        for va, acc in res.items():
            rows.setdefault((f.relative_to(root).as_posix(), va), {})[tc] = (acc, va in ident)
        if err: print("#ERR", f.name, tc, err, file=sys.stderr)
print("source\trva\tddk\tddk_ident\trw9044\trw9044_ident\tdelta")
for (src, va), d in sorted(rows.items(), key=lambda kv: kv[0][1]):
    a = d.get("ddk", (None, False)); b = d.get("rw9044", (None, False))
    delta = "" if a[0] is None or b[0] is None else f"{b[0]-a[0]:+.1f}"
    print(f"{src}\t{va}\t{a[0]}\t{int(a[1])}\t{b[0]}\t{int(b[1])}\t{delta}")
