"""DDKOBJ-STATIC-GLOBALS: match.py --bytes and relocheck.py on each re/match source that holds a C-view file-static
DAT_<addr> (the 35 of RACE-PIZ-DEVICE's static_global_units.txt). No source is edited by this row: this records that the
sources the tool change touches stay byte-identical with relocheck OK under their own // FLAGS.

    py -3.12 verify/ddkobj_staticglobals_20261127/scope_match.py > verify/ddkobj_staticglobals_20261127/scope_match.tsv
"""
import pathlib
import re
import subprocess
import sys

ROOT = pathlib.Path(__file__).resolve().parents[2]
ST = re.compile(r'^static\s+(?:const\s+|volatile\s+)*[A-Za-z_][\w\s\*]*?\b(DAT_[0-9a-fA-F]{8})\s*(\[[^\]]*\])?\s*(=[^;]*)?;', re.M)
print("src\tstatic_names\tmatch_bytes\trelocheck")
tally = {}
for p in sorted((ROOT / "re" / "match").rglob("*.c")):
    names = sorted({m.group(1) for m in ST.finditer(p.read_text(encoding="utf-8", errors="replace"))})
    if not names:
        continue
    src = p.relative_to(ROOT).as_posix()
    m = subprocess.run([sys.executable, str(ROOT / "re/tools/match/match.py"), str(p), "--bytes"],
                       capture_output=True, text=True, cwd=ROOT)
    lines = [l for l in m.stdout.splitlines() if "bytes:" in l]
    if lines and all("BYTE-IDENTICAL" in l for l in lines):
        mb = f"BYTE-IDENTICAL x{len(lines)}"
    else:
        mb = "; ".join(re.sub(r"\s+", " ", l.strip()) for l in lines)[:200] if lines else "no bytes line"
    r = subprocess.run([sys.executable, str(ROOT / "re/tools/match/relocheck.py"), str(p)],
                       capture_output=True, text=True, cwd=ROOT)
    txt = (r.stdout + r.stderr).strip()
    rc = "OK" if re.search(r"\bOK\b", txt) and r.returncode == 0 else (
        f"rc{r.returncode}: " + (txt.splitlines()[-1][:100] if txt else ""))
    key = (mb.startswith("BYTE-IDENTICAL"), rc == "OK")
    tally[key] = tally.get(key, 0) + 1
    print(f"{src}\t{','.join(names)}\t{mb}\t{rc}")
for k, v in sorted(tally.items()):
    print(f"# byte_identical={k[0]} relocheck_ok={k[1]}: {v}")
