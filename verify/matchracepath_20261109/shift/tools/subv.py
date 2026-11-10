"""subv.py <src> <rva8> <variants.py>: V = [(name, fn)] where fn(text)->text. Runs match.py --bytes per variant."""
import subprocess, sys, pathlib, runpy, re
from concurrent.futures import ThreadPoolExecutor
src, rva, vf = sys.argv[1], sys.argv[2], sys.argv[3]
root = pathlib.Path.cwd(); out = pathlib.Path(__file__).parent / "var"; out.mkdir(exist_ok=True)
text = (root / src).read_text(); V = runpy.run_path(vf)["V"]
def run(v):
    name, fn = v
    t = fn(text)
    if t == text: return f"{name}: UNCHANGED"
    p = out / f"{rva}_{name}.c"; p.write_text(t)
    r = subprocess.run([sys.executable, "re/tools/match/match.py", str(p), "--only", rva, "--bytes"], capture_output=True, text=True, cwd=root)
    lines = (r.stdout + r.stderr).splitlines()
    sc = next((l for l in lines if "%" in l and "ins" in l), "?"); by = next((l for l in lines if "bytes:" in l), "")
    err = "" if sc != "?" else " | " + " / ".join(lines[-3:])[:300]
    return f"{name}: {sc.split(':',1)[-1].split('flags')[0].strip()} {by.strip()}{err}"
with ThreadPoolExecutor(6) as ex:
    for line in ex.map(run, V): print(line, flush=True)
