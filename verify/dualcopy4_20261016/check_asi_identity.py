"""check_asi_identity.py <asi under test> <dir with path2_*.txt> : for every path2 report, (reimpl addr - asi base) must equal that export's RVA in the
.asi under test (dumpbin /exports). A mismatch means the shared .asi was not the one this lane deployed at that step (another session's window overwrote it
between steps) and the step is CONTAMINATED: it must be re-run, not read."""
import re, subprocess, sys
from pathlib import Path

asi, d = Path(sys.argv[1]), Path(sys.argv[2])
vc = r'C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\VC\Auxiliary\Build\vcvars32.bat'
txt = subprocess.run(f'call "{vc}" >nul && dumpbin /exports "{asi}"', shell=True, capture_output=True, text=True).stdout
rva = {}
for ln in txt.splitlines():
    m = re.match(r"\s+\d+\s+[0-9A-F]+\s+([0-9A-F]{8})\s+(\S+)", ln)
    if m:
        rva[m.group(2)] = int(m.group(1), 16)
bad = 0
for f in sorted(d.glob("path2_*.txt")):
    t = f.read_text(encoding="utf-8", errors="replace")
    ex = re.search(r"export:\s+(\S+)", t)
    base = re.search(r"asi base:\s+(0x[0-9a-fA-F]+)", t)
    reim = re.search(r"reimpl addr:\s+(0x[0-9a-fA-F]+)", t)
    if not (ex and base and reim):
        print(f"{f.name:46s} no asi base / reimpl addr in the report (harness failed before the load)")
        continue
    got = int(reim.group(1), 16) - int(base.group(1), 16)
    want = rva.get(ex.group(1))
    ok = got == want
    bad += not ok
    print(f"{f.name:46s} {ex.group(1):26s} rva {got:#08x} want {want if want is None else format(want, '#08x')}  {'OK' if ok else 'CONTAMINATED'}")
print("contaminated:", bad)
sys.exit(1 if bad else 0)
