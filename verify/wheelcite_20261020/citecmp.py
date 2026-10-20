# [wheelcite] P3: per-row cite before (diff_*.json, the run's own scoring) vs after (wc_*.json, C27b)
import json, sys
from collections import Counter
from pathlib import Path
for root in sys.argv[1:]:
    root = Path(root); ch = Counter(); n = 0; bad = []
    for fa in sorted(root.glob("*/diff_*.json")):
        fb = fa.with_name("wc_" + fa.name[5:])
        ra, rb = json.loads(fa.read_text())["rows"], json.loads(fb.read_text())["rows"]
        assert len(ra) == len(rb), fa
        for a, b in zip(ra, rb):
            n += 1
            ca, cb = a.get("cite", "RED"), b.get("cite", "RED")
            if ca != cb:
                ch[(ca, cb)] += 1
                if cb.startswith("C27b") and not (b.get("dpos", 1) == 0.0 and b.get("tris") == [96]):
                    bad.append((fa.parent.name, fa.stem, b.get("dpos"), b.get("tris")))
                print(f"  {fa.parent.name} {fa.stem[5:]} {a['kind']} {a['tex']}: {ca} -> {cb} dpos={b.get('dpos')} tris={b.get('tris')}")
    print(f"{root}: {n} rows, changes {dict(ch)}, C27b rows with dpos != 0 or tris != [96]: {bad}")
