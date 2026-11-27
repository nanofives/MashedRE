"""DDKOBJ-ADDR-REGISTERED: match.py --bytes and relocheck.py on the source of each of the 75 in-scope units.

    py -3.12 verify/ddkobj_addrreg_20261127/scope_match.py mashedmod/build/ddkobj > verify/ddkobj_addrreg_20261127/scope_match.tsv
"""
import csv
import pathlib
import re
import subprocess
import sys

HERE = pathlib.Path(__file__).resolve().parent
ROOT = HERE.parents[1]
sys.path.insert(0, str(HERE))
import compare_head as C  # noqa: E402  (prints its own comparison first; that output goes to stderr below)

out = pathlib.Path(sys.argv[1])
rows = {r["rva"]: r for r in csv.DictReader(open(out / "BYTECHECK.tsv", encoding="utf-8"), delimiter="\t")}
print("rva\tgroup\tddkobj\tmatch_bytes\trelocheck\tsrc")
tally = {}
for rva, g in sorted(C.SCOPE.items()):
    src = rows[rva]["src"]
    m = subprocess.run([sys.executable, str(ROOT / "re/tools/match/match.py"), str(ROOT / src), "--bytes"],
                       capture_output=True, text=True, cwd=ROOT)
    last = [l for l in m.stdout.splitlines() if "bytes:" in l]
    mb = "BYTE-IDENTICAL" if last and "BYTE-IDENTICAL" in last[-1] else (
        re.sub(r"\s+", " ", last[-1].strip()) if last else "no bytes line")
    r = subprocess.run([sys.executable, str(ROOT / "re/tools/match/relocheck.py"), str(ROOT / src)],
                       capture_output=True, text=True, cwd=ROOT)
    txt = (r.stdout + r.stderr)
    rc = "OK" if re.search(r"\bOK\b", txt) and r.returncode == 0 else ("rc%d: " % r.returncode) + txt.strip().splitlines()[-1][:80] if txt.strip() else "rc%d" % r.returncode
    key = (rows[rva]["result"], mb == "BYTE-IDENTICAL", rc == "OK")
    tally[key] = tally.get(key, 0) + 1
    print(f"{rva}\t{g}\t{rows[rva]['result']}\t{mb}\t{rc}\t{src}")
for k, v in sorted(tally.items()):
    print(f"# ddkobj={k[0]} match_byte_identical={k[1]} relocheck_ok={k[2]}: {v}")
