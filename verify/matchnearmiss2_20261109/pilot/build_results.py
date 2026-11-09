"""Build results.tsv for MATCH-NEARMISS-2-pilot: re-score each row's source at the base commit (orch/MATCH-NEARMISS-2)
and in the working tree with match.py --bytes (DDK, the file's line-1 FLAGS), and join the hand log.

    py -3.12 verify/matchnearmiss2_20261109/pilot/build_results.py <hand_log.tsv> > results.tsv
hand_log.tsv: rva<TAB>verdict<TAB>score<TAB>what closed it / residual<TAB>compiles=N
"""
import csv, hashlib, pathlib, re, subprocess, sys
ROOT = pathlib.Path(__file__).resolve().parents[3]
BASE = "bbda49248"
ROWS = [("0x0057c0d0", "rwl06/0057c0d0.c", "order"), ("0x005a8390", "s25/005a8390.c", "order"),
        ("0x005a86a0", "s25/005a86a0.c", "order"), ("0x005a8560", "s25/005a8560.c", "order"),
        ("0x005ab560", "s25/005ab560.c", "order"), ("0x005ac7e0", "s25/005ac7e0.c", "order"),
        ("0x004e4450", "rwl02/004e4450.c", "order"), ("0x005735f0", "rwl06/005735f0.c", "order"),
        ("0x005a7ee0", "s25/005a7ee0.c", "order"), ("0x005bf8a0", "s28/005bf8a0.c", "order"),
        ("0x00540340", "rwl03/00540340.c", "regalloc"), ("0x004c1040", "rwl01/004c1040.c", "regalloc"),
        ("0x004c1210", "rwl01/004c1210.c", "regalloc"), ("0x0055ded0", "rwl05/0055ded0.c", "regalloc"),
        ("0x004967e0", "s16/004967e0.c", "count"), ("0x00402b70", "s01/00402b70.c", "count"),
        ("0x005a9c50", "s25/005a9c50.c", "count"), ("0x005b0df0", "s26/005b0df0.c", "count"),
        ("0x005af070", "s26/005af070.c", "const"), ("0x005c4ad0", "s28/005c4ad0.c", "const")]
RE = re.compile(r"^(0x[0-9a-f]{8}) \S+: ([\d.]+)%.*?(\d+)/(\d+) ins")
RB = re.compile(r"bytes: (\d+) B whole symbol, (\d+) differing")

def score(path, rva):
    out = subprocess.run(["py", "-3.12", "re/tools/match/match.py", "--bytes", "--only", rva, str(path)],
                         capture_output=True, text=True, cwd=ROOT).stdout
    pct = bad = None
    for ln in out.splitlines():
        m = RE.match(ln)
        if m and m.group(1) == rva: pct = m.group(2)
        b = RB.search(ln)
        if b and pct is not None and bad is None: bad = b.group(2)
    return pct, bad

hand = {}
for ln in pathlib.Path(sys.argv[1]).read_text().splitlines():
    f = ln.split("\t")
    hand[f[0]] = (f[1], f[3], f[4])
sub = {}
with open(ROOT / "hooks.csv", newline="") as fh:
    for r in csv.DictReader(fh):
        sub["0x" + r["rva"].lower().removeprefix("0x").zfill(8)] = r["subsystem"]
print("\t".join(["rva", "source", "subsystem", "miss_type", "before_pct", "after_pct", "before_bad_bytes",
                 "after_bad_bytes", "verdict", "detail", "compiles", "sha256"]))
for rva, src, mt in ROWS:
    p = ROOT / "re/match" / src
    tmp = p.with_name("_base_" + p.name)
    tmp.write_bytes(subprocess.run(["git", "show", f"{BASE}:re/match/{src}"], capture_output=True, cwd=ROOT).stdout)
    try:
        b_pct, b_bad = score(tmp, rva)
    finally:
        tmp.unlink()
    a_pct, a_bad = score(p, rva)
    v, detail, comp = hand[rva]
    print("\t".join([rva, "re/match/" + src, sub.get(rva, ""), mt, b_pct, a_pct, b_bad, a_bad, v, detail,
                     comp.replace("compiles=", ""), hashlib.sha256(p.read_bytes()).hexdigest()]), flush=True)
