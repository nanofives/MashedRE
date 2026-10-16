# Regenerates HASHES.md: sha8 + line count of every <run>.step.csv / .gates.csv in this directory.
# The CSVs themselves are not committed (several MB each); the recipe in run_gctrl.py reproduces them.
import hashlib
from pathlib import Path

HERE = Path(__file__).resolve().parent


def sha8(p):
    return hashlib.sha256(p.read_bytes()).hexdigest()[:8]


rows = []
for s in sorted(HERE.glob("*.step.csv")):
    name = s.name[:-len(".step.csv")]
    g = HERE / f"{name}.gates.csv"
    n = len(s.read_text(errors="replace").splitlines())
    rows.append((name, n, sha8(s), sha8(g) if g.exists() else "-"))

out = ["# step.csv / gates.csv fingerprints (G-CTRL recipe, run_gctrl.py)", "",
       "Run names: `base*` = baseline exe (HEAD 1caf1a10). `post*`/`commit*`/`final*` = full deletion. "
       "`hwy_*` = Highway, 4000 frames. `all_*`/`vs_*` = per-track sweeps. `lc*`/`nc_*` = no-op controls. "
       "Training default (14400 frames) expected 991a5fff / be0d47ea.", "",
       "| run | step lines | step sha8 | gates sha8 |", "|---|---:|---|---|"]
for name, n, s8, g8 in rows:
    out.append(f"| {name} | {n} | {s8} | {g8} |")
(HERE / "HASHES.md").write_text("\n".join(out) + "\n", encoding="utf-8")
print(f"HASHES.md: {len(rows)} runs")
