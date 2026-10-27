"""D4-WSB-C230-RECHECK: per-boot signature sets of 0x0055c230 from racediff sample files.

  py -3.12 verify/wsbc230_20261027/boot_sets.py <arm_dir> [<arm_dir> ...]

Each arm dir holds c4_racediff_samples.json = {"off": {rva: {sig: count}}, "on": {...}}. A control arm's "on" slot
is its first original boot. Prints one line per boot (arm, slot, calls, set) and whether every printed set is equal.
"""
import json, sys
from pathlib import Path

KEY = str(0x0055c230)
sets = []
for d in sys.argv[1:]:
    p = Path(d) / "c4_racediff_samples.json"
    s = json.loads(p.read_text(encoding="utf-8")) if p.exists() else {}
    for slot in ("on", "off"):
        m = (s.get(slot) or {}).get(KEY) or {}
        sets.append(frozenset(m))
        print(f"{Path(d).name}\t{slot}\tcalls={sum(m.values())}\tsigs={len(m)}")
        for sig in sorted(m):
            print(f"    {m[sig]:>3} x {sig[:160]}")
eq = len(set(sets)) == 1 and all(sets)
print(f"BOOTS={len(sets)} DISTINCT_SETS={len(set(sets))} ALL_EQUAL_NONEMPTY={eq}")
