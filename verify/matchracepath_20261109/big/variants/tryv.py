"""tryv.py <src> <rva8> <variants.py>: variants.py defines V = [(name, [(old, new), ...]), ...].
Each variant applies string replacements to a copy of src (in scratchpad/var/) and runs match.py --bytes.
Prints one line per variant: name, score line, bytes line. Copy kept as var/<rva>_<name>.c."""
import subprocess, sys, pathlib, runpy
from concurrent.futures import ThreadPoolExecutor
src, rva, vf = sys.argv[1], sys.argv[2], sys.argv[3]
show = len(sys.argv) > 4
root = pathlib.Path.cwd()
out = pathlib.Path(__file__).parent / "var"; out.mkdir(exist_ok=True)
text = (root / src).read_text()
V = runpy.run_path(vf)["V"]
def run(v):
    name, reps = v
    t = text
    for o, n in reps:
        if o not in t:
            return f"{name}: MISSING {o[:60]!r}"
        t = t.replace(o, n, 1)
    p = out / f"{rva}_{name}.c"; p.write_text(t)
    r = subprocess.run([sys.executable, "re/tools/match/match.py", str(p), "--only", rva, "--bytes"] + (["--show"] if show else []),
                       capture_output=True, text=True, cwd=root)
    lines = (r.stdout + r.stderr).splitlines()
    if show:
        return f"--- {name}\n" + "\n".join(l[:110] for l in lines if not l.startswith("   =") )
    sc = next((l for l in lines if "%" in l and "ins" in l), "?")
    by = next((l for l in lines if "bytes:" in l), "")
    err = "" if sc != "?" else " | " + " / ".join(lines[-3:])[:300]
    return f"{name}: {sc.split(':',1)[-1].split('flags')[0].strip()} {by.strip()}{err}"
with ThreadPoolExecutor(6) as ex:
    for line in ex.map(run, V):
        print(line, flush=True)
