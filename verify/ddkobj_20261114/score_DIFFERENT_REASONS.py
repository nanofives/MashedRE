"""Run match.py + relocheck.py on each listed source; print the score/verdict lines (summary only)."""
import subprocess, sys, re, pathlib
root = pathlib.Path(sys.argv[1]).resolve()
for src in sys.argv[2:]:
    m = subprocess.run([sys.executable, "re/tools/match/match.py", src], cwd=root, capture_output=True, text=True)
    lines = [l for l in (m.stdout + m.stderr).splitlines()
             if re.search(r"%|MATCH|IDENTICAL|score|error|Error", l) and not l.lstrip().startswith("!")]
    r = subprocess.run([sys.executable, "re/tools/match/relocheck.py", src], cwd=root, capture_output=True, text=True)
    rl = [l for l in (r.stdout + r.stderr).splitlines() if l.strip()]
    print("==", src)
    for l in lines[:6]:
        print("  match:", l.strip()[:200])
    for l in rl[-3:]:
        print("  reloc:", l.strip()[:200])
