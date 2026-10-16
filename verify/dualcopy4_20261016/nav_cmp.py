"""nav_cmp.py <control_a> <control_b> <new_a> <new_b> : per-BMP verdict for the nav-demo walks under nav/<name>/.
A screen is INFORMATIVE only if the two control runs (same base image twice) agree on it; then both new runs must equal it (SAME) or the
change moved it (DIFF). Screens where the controls disagree are NOISY (animation / wall clock) and prove nothing either way."""
import hashlib, sys
from pathlib import Path

HERE = Path(__file__).resolve().parent


def load(n):
    d = {}
    for b in sorted((HERE / "nav" / n).rglob("walk_*.bmp")):
        d[b.name] = hashlib.sha256(b.read_bytes()).hexdigest()[:8]
    return d


ca, cb, na, nb = (load(x) for x in sys.argv[1:5])
names = sorted(set(ca) | set(cb) | set(na) | set(nb))
inf = same = diff = noisy = 0
for n in names:
    a, b, c, d = ca.get(n), cb.get(n), na.get(n), nb.get(n)
    if a != b:
        noisy += 1
        tag = "NOISY (controls differ)"
    else:
        inf += 1
        if c == a and d == a:
            same += 1
            tag = "SAME"
        else:
            diff += 1
            tag = "DIFF"
    print(f"{n:34s} ctl={a},{b} new={c},{d}  {tag}")
print(f"screens={len(names)} informative={inf} (SAME {same}, DIFF {diff}) noisy={noisy}")
sys.exit(1 if diff else 0)
