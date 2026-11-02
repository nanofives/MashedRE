#!/usr/bin/env python3
"""Standing guard for D5 shim removal (step 3): find MASHED.exe spawners that rely on
the dinput8 proxy to load mashed_re_dev.asi.

A file is a SPAWNER when it contains both a process-spawn token (.spawn(, Popen,
Start-Process) and a MASHED.exe reference
(MASHED.exe, original_exe, MASHED_EXE). A spawner is COVERED when it mentions
MASHED_RE_NO_AUTO_HOOK or dev_asi. Anything else is UNCOVERED.

Scope: re/ and scripts/ (*.py, *.ps1; prior_art and analysis excluded) are live and
gate the exit code. verify/ (*.py, *.ps1, *.bat) is frozen evidence: reported, never
fails. Allowlist: re/tools/spawner_audit_allowlist.txt, one repo-relative path per line
(forward slashes, '#' comments, trailing '# reason' allowed).

Usage: py -3.12 re/tools/spawner_audit.py [--root DIR] [--list-verify] [--quiet]
Exit 1 when a live UNCOVERED spawner is not allowlisted, else 0.
"""
from __future__ import annotations

import argparse
import re
import sys
from pathlib import Path

SPAWN = re.compile(r'\.spawn\(|\bPopen\b|Start-Process', re.I)
TARGET = re.compile(r'MASHED\.exe|original_exe|MASHED_EXE')
COVER = re.compile(r'MASHED_RE_NO_AUTO_HOOK|dev_asi')

LIVE_DIRS = ('re', 'scripts')
LIVE_EXT = ('.py', '.ps1')
LIVE_SKIP = ('re/prior_art/', 're/analysis/')
VERIFY_EXT = ('.py', '.ps1', '.bat')
ALLOWLIST = 're/tools/spawner_audit_allowlist.txt'


def classify(path: Path):
    """Return None (not a spawner), 'covered' or 'uncovered'."""
    try:
        text = path.read_text(encoding='utf-8', errors='replace')
    except OSError:
        return None
    if not (SPAWN.search(text) and TARGET.search(text)):
        return None
    return 'covered' if COVER.search(text) else 'uncovered'


def scan(root: Path, top: str, exts, skip=()):
    out = []
    base = root / top
    if not base.is_dir():
        return out
    for p in sorted(base.rglob('*')):
        if not p.is_file() or p.suffix.lower() not in exts:
            continue
        rel = p.relative_to(root).as_posix()
        if any(rel.startswith(s) for s in skip):
            continue
        c = classify(p)
        if c:
            out.append((rel, c))
    return out


def load_allowlist(root: Path):
    f = root / ALLOWLIST
    if not f.is_file():
        # --root pointed at a bare archive of another branch: use the list beside this script
        f = Path(__file__).resolve().with_name(Path(ALLOWLIST).name)
    if not f.is_file():
        return set()
    names = set()
    for line in f.read_text(encoding='utf-8').splitlines():
        line = line.split('#', 1)[0].strip()
        if line:
            names.add(line.replace('\\', '/'))
    return names


def main() -> int:
    ap = argparse.ArgumentParser(description=__doc__.split('\n')[0])
    ap.add_argument('--root', default=str(Path(__file__).resolve().parents[2]))
    ap.add_argument('--list-verify', action='store_true', help='print every uncovered verify/ file')
    ap.add_argument('--quiet', action='store_true')
    a = ap.parse_args()
    root = Path(a.root)
    allow = load_allowlist(root)

    live = []
    for d in LIVE_DIRS:
        live += scan(root, d, LIVE_EXT, LIVE_SKIP)
    ver = scan(root, 'verify', VERIFY_EXT)

    bad = [r for r, c in live if c == 'uncovered' and r not in allow]
    allowed = [r for r, c in live if c == 'uncovered' and r in allow]
    n_cov = sum(1 for _, c in live if c == 'covered')
    v_unc = [r for r, c in ver if c == 'uncovered']
    v_cov = len(ver) - len(v_unc)

    print(f'live (re/, scripts/): {len(live)} spawners, {n_cov} covered, '
          f'{len(allowed)} allowlisted, {len(bad)} FLAGGED')
    print(f'verify/ (report only): {len(ver)} spawners, {v_cov} covered, {len(v_unc)} uncovered')
    if not a.quiet:
        for r in bad:
            print(f'FLAG {r}')
        for r in allowed:
            print(f'allow {r}')
    if a.list_verify:
        for r in v_unc:
            print(f'verify-uncovered {r}')
    return 1 if bad else 0


if __name__ == '__main__':
    sys.exit(main())
