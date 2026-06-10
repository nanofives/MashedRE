# Phase A2 triage helper: map disabled exports → registry names, run diff,
# classify result.
#
# Reads stdin lines of form "0xRVA  ExportName", looks up the registry hook
# whose 'export' matches, runs run_diff.py, and prints a single CSV line:
#   RVA,Export,RegistryKey,Verdict,DiffSummary,Notes
#
# Verdicts (one of):
#   GREEN-REAL    — fingerprint match with input-dependent variation
#   GREEN-WEAK    — match but all rows produced same output (potential fake)
#   AV-AV-BANNED  — both crashed identically; user banned this pattern
#   AV-DIVERGE    — both crashed differently → real reimpl divergence
#   RED-MIXED     — partial mismatch
#   TIMEOUT       — diff harness exceeded deadline
#   NO-ENTRY      — no registry entry for this export
#   NO-EXPORT     — registry entry present but export not in .asi
#
# This is offline analysis — does not modify any file.

from __future__ import annotations
import csv, io, sys, subprocess
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent.parent
sys.path.insert(0, str(ROOT / 're' / 'frida'))
from hooks_registry import HOOKS

def find_hook_by_export(export_name: str) -> str | None:
    for key, cfg in HOOKS.items():
        if cfg.get('export') == export_name:
            return key
    return None

def run_diff_capture(hook_key: str, timeout_s: int = 90) -> tuple[int, str, str]:
    log_csv = ROOT / 'log' / f'diff_{hook_key}.csv'
    try:
        proc = subprocess.run(
            ['py', '-3.12', str(ROOT / 're' / 'frida' / 'run_diff.py'), hook_key],
            capture_output=True, text=True, timeout=timeout_s, cwd=str(ROOT))
        return proc.returncode, proc.stdout, proc.stderr
    except subprocess.TimeoutExpired:
        return 99, '', 'TIMEOUT'

def classify_csv(csv_path: Path) -> str:
    if not csv_path.exists(): return 'NO-CSV'
    rows = list(csv.DictReader(open(csv_path)))
    if not rows: return 'EMPTY-CSV'
    matches  = [r for r in rows if r['match'] == 'True']
    mismatch = [r for r in rows if r['match'] == 'False']
    n_total  = len(rows)
    n_match  = len(matches)
    n_miss   = len(mismatch)
    # Check err patterns
    errs_orig = set(r['err_original'] for r in rows if r['err_original'])
    errs_reim = set(r['err_reimpl']   for r in rows if r['err_reimpl'])
    all_null  = all(not r['original'] and not r['reimpl'] for r in rows)
    if n_match == n_total:
        # All match — check for input-dependent variation
        origs = set(r['original'] for r in rows)
        if all_null:
            return f'AV-AV-BANNED  (all None/None, {len(errs_orig)} err strings)'
        if len(origs) == 1:
            return f'GREEN-WEAK    ({n_total}/{n_total} all={list(origs)[0]})'
        return f'GREEN-REAL    ({n_total}/{n_total} {len(origs)} distinct vals)'
    if n_match == 0 and all_null:
        if errs_orig == errs_reim:
            return f'AV-AV-BANNED  ({n_total} crashes, same err)'
        return f'AV-DIVERGE    ({n_total} crashes, orig={list(errs_orig)[:1]}, reim={list(errs_reim)[:1]})'
    return f'RED-MIXED     ({n_miss}/{n_total} mismatches)'

def main():
    print('RVA,Export,RegistryKey,Verdict')
    for line in sys.stdin:
        line = line.strip()
        if not line: continue
        try:
            rva, export = line.split(None, 1)
        except ValueError: continue
        key = find_hook_by_export(export)
        if key is None:
            print(f'{rva},{export},,NO-ENTRY')
            continue
        rc, out, err = run_diff_capture(key)
        if rc == 99:
            print(f'{rva},{export},{key},TIMEOUT')
            continue
        if 'export not found in .asi' in out:
            print(f'{rva},{export},{key},NO-EXPORT')
            continue
        verdict = classify_csv(ROOT / 'log' / f'diff_{key}.csv')
        print(f'{rva},{export},{key},{verdict}', flush=True)

if __name__ == '__main__':
    main()
