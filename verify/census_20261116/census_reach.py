# T2-RACEPATH-CENSUS: static reach of the exe's ORIG_TOP route. The linked mashed_re.exe call graph
# (scripts/backfill_port.py build_graph: rel32 calls/jmps onto function starts + base relocations) is walked from
# _Orig_00492370 (the matched WinMain, entered when MASHED_ORIG_TOP=1). A reached exe symbol counts for an original
# RVA when its undecorated name ends in that RVA's 8 hex digits (Orig_, PortTop_Bound_, RwCore_, ___ddkreg_, ...) or
# when verify/portorigtop_20261113/tools/exeidx.json names it as that RVA's body. Static: a reached symbol may not run.
#   py -3.12 verify/census_20261116/census_reach.py <build_dir> <out.tsv>
import json, re, sys
from pathlib import Path

WT = Path(__file__).resolve().parent.parent.parent
sys.path.insert(0, str(WT / "scripts"))
import backfill_port as B

bd, out = Path(sys.argv[1]), Path(sys.argv[2])
B.ROOT_SYM = "_Orig_00492370@16"
ents, names_at, edges, nrel, _ = B.build_graph(bd / "mashed_re.exe", bd / "mashed_re.map")
prev, barriers = B.walk(ents, names_at, edges)
idx = json.loads((WT / "verify/portorigtop_20261113/tools/exeidx.json").read_text())
sym2rva = {}
for rva, lst in idx.items():
    for sym, _f, _how in lst:
        sym2rva.setdefault(sym.split("::")[-1], set()).add("0x" + rva.lower())
hexend = re.compile(r"([0-9a-fA-F]{8})$")
reached = {}
for va in prev:
    for s in names_at.get(va, ()):
        b = B.base_name(s) or s
        m = hexend.search(b)
        rv = set()
        if m and 0x00401000 <= int(m.group(1), 16) < 0x00600000:
            rv.add("0x" + m.group(1).lower())
        rv |= sym2rva.get(b, set())
        for r in rv:
            reached.setdefault(r, set()).add(b)
with open(out, "w", newline="") as f:
    f.write("rva\texe_symbols\n")
    for r in sorted(reached):
        f.write(f"{r}\t{','.join(sorted(reached[r]))}\n")
print(f"reached exe functions {len(prev)}, original rvas {len(reached)}, barriers {len(barriers)}")
