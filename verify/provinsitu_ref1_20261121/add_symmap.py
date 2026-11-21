# PROV-INSITU-REF1 NOT-WIRED leg (lead request, run 53): append symbol -> original address rows to
# re/tools/match/equiv_symmap.tsv, each resolved from a libid row of hooks.csv (and, where the original body calls it,
# the original call site from notwired_calls.py). Hand rows: equiv_symmap.py regeneration does not produce them.
from pathlib import Path
P = Path(__file__).resolve().parents[2] / "re" / "tools" / "match" / "equiv_symmap.tsv"
ROWS = [
    ("__CIpow", "0x004a2d70", "hooks.csv 004a2d70 libid identical __CIpow (libcmt pow.obj)"),
    ("__CIacos", "0x004a3370", "hooks.csv 004a3370 libid identical __CIacos (libcmt acos.obj); the original 0x00487280 calls 0x004a3384 at 0x00487c2a, not this entry"),
    ("@__fload_withFB@0", "0x004a70d5", "hooks.csv 004a70d5 libid identical __fload_withFB (libcmt common.obj); original call 0x005c2f88 -> 0x004a70d5"),
    ("__rand", "0x005c229b", "hooks.csv 005c229b libid identical _rand (libcmt rand.obj); source name has one more underscore; original call 0x00564317 -> 0x005c229b"),
    ("__pow", "0x004a2d30", "hooks.csv 004a2d30 libid identical _pow (libcmt pow.obj); source name has one more underscore; original call 0x005b91c2 -> 0x004a2d30"),
]
txt = P.read_text()
have = {l.split("\t")[0] for l in txt.splitlines() if l and not l.startswith("#")}
add = [r for r in ROWS if r[0] not in have]
if not txt.endswith("\n"):
    txt += "\n"
txt += "# hand rows, PROV-INSITU-REF1 2026-11-21 (verify/provinsitu_ref1_20261121/RESULT_PROV-INSITU-REF1.md): name\taddress\tlibid\tcitation\n"
txt += "".join(f"{n}\t{a}\tlibid\t{c}\n" for n, a, c in add)
P.write_text(txt)
print("added", [r[0] for r in add])
