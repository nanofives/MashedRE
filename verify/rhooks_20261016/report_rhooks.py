"""rhooks: per-track base vs final-arm table (matched draws, matched triangle %, rows) from the score JSONs."""
import json, sys, collections
from pathlib import Path
HERE = Path(__file__).resolve().parent
arms = [("base", "base_"), ("arm_d", "dr_")]
print("| track | frame | matched draws base -> D | matched tri % base -> D | rows base -> D |")
print("|---|---:|---|---|---|")
tot = collections.Counter()
for td in sorted(p for p in (HERE / "base").iterdir() if p.is_dir()):
    for f in sorted(td.glob("base_*.json")):
        if f.name.startswith("cites"): continue
        row = f.stem[len("base_"):]
        B = json.loads(f.read_text()); D = json.loads((HERE / "arm_d" / td.name / f"dr_{row}.json").read_text())
        pb = 100.0 * B["tri_a"][1] / B["tri_a"][0] if isinstance(B["tri_a"], list) else None
        def pct(X):
            m = sum(1 for _ in [])
            return None
        print(f"| {td.name[3:]} | {int(row)} | {B['matched']} -> {D['matched']} of {B['na']} | "
              f"{100.0*B['matched_tri']/B['tri_a'] if 'matched_tri' in B else 0:.1f} | {len(B['rows'])} -> {len(D['rows'])} |")
        tot["rows_b"] += len(B["rows"]); tot["rows_d"] += len(D["rows"]); tot["m_b"] += B["matched"]; tot["m_d"] += D["matched"]; tot["na"] += B["na"]
print(dict(tot))
