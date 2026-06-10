# Stricter classifier: post-process existing diff_*.csv files.
# Counts ACTUAL row-by-row orig vs reimpl equality (ignoring crash_equal_ok
# masking). Flags silent-divergence cases that a2_triage missed.
from __future__ import annotations
import csv, sys
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent.parent
sys.path.insert(0, str(ROOT / 're' / 'frida'))
from hooks_registry import HOOKS

def strict_classify(csv_path: Path) -> str:
    if not csv_path.exists(): return 'NO-CSV'
    rows = list(csv.DictReader(open(csv_path)))
    if not rows: return 'EMPTY-CSV'
    n = len(rows)
    # Count rows where orig and reimpl bytes match (regardless of match flag).
    real_match = 0
    null_match = 0
    diverge = 0
    for r in rows:
        o = r['original']; ri = r['reimpl']
        if not o and not ri:
            null_match += 1  # both crashed — banned per phase-a1 rule
        elif o == ri:
            real_match += 1
        else:
            diverge += 1
    # Distinct orig values (for variation check).
    distinct_origs = len({r['original'] for r in rows if r['original']})
    if diverge > 0:
        return f'SILENT-DIVERGE ({diverge}/{n} orig!=reimpl)'
    if null_match == n:
        return f'AV-AV ({n}/{n})'
    if real_match == n:
        if distinct_origs == 1:
            v = rows[0]['original']
            return f'GREEN-UNIFORM ({n}/{n} all={v})'
        return f'GREEN-VARYING ({n}/{n} {distinct_origs} distinct)'
    # Mixed: some real matches + some null matches
    return f'MIXED ({real_match} real, {null_match} av-av, {diverge} divergent / {n})'

def main():
    # Read RVA/Export/Key lines from triage output on stdin
    print('RVA,Export,RegistryKey,Verdict,StrictClass')
    for line in sys.stdin:
        line = line.strip()
        if not line or line.startswith('RVA,'): continue
        parts = line.split(',', 3)
        if len(parts) < 4: continue
        rva, export, key, verdict = parts
        if not key:
            print(f'{rva},{export},,NO-ENTRY,NO-ENTRY')
            continue
        csv_path = ROOT / 'log' / f'diff_{key}.csv'
        strict = strict_classify(csv_path)
        print(f'{rva},{export},{key},{verdict.strip()},{strict}')

if __name__ == '__main__':
    main()
