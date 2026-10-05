#!/usr/bin/env python3
"""Find places that cite a U-NNNN as OPEN which UNCERTAINTIES.md has since RESOLVED.

WHY. On 2026-10-05 `re/tools/gamesave_parse.py`'s header described a save-file region
as "not written by Save::SerializeToBuffer; who writes it is unresolved (UNCERTAIN
U-3559)". U-3559 had been RESOLVED since 2026-05-22 — the region IS written by it,
by two named stores. The stale header misled three consecutive legs
(verify/d4_save_20261005/PREREG_SAVE.md, RESULT_SAVE.md, PREREG_SPAN.md), and the
same header also carried a transposed digit in a LIVE constant. A corrected fact
lived in the tracker while the stale version lived in the file people read.

THIS IS A SEARCH WITH MANUAL ADJUDICATION, NOT A GATE. It cannot know whether a
citation is stale; it finds citations whose SURROUNDING WORDS frame the row as open
while the row itself carries a resolution marker. Every hit needs a human read.
Stated before running so the output cannot be presented as a verdict.

┌───────────────────────────────────────────────────────────────────────────────┐
│ THIS TOOL FAILS ITS OWN KNOWN-ANSWER CHECK. READ BEFORE USING ITS OUTPUT.     │
│                                                                               │
│ Run against the PRE-FIX `gamesave_parse.py` (git e3e60fec~1), the exact case  │
│ it was written for, it reports **0 hits**.                                    │
│                                                                               │
│ Cause, and it is on the ROW side not the citation side: U-3559's row states   │
│ its answer in PROSE — "Tail [...] IS written by Save::SerializeToBuffer" —    │
│ with no RESOLVED/CLOSED/~~ marker anywhere, so `resolved_marked` is False and │
│ the citation is never considered. U-3558 is the same shape ("Field position   │
│ ANSWERED" inside a row typed plain `structural`).                             │
│                                                                               │
│ CONSEQUENCE: what this finds is a WEAKER class than the one that motivated    │
│ it — rows that are explicitly marked resolved AND cited as open. The          │
│ dangerous class, a row quietly answered in prose while a tool header still    │
│ says "unresolved", is exactly what it misses. Do not read a clean run as      │
│ "no stale citations".                                                         │
│                                                                               │
│ THE REAL FIX IS A SCHEMA CHANGE, not a better regex: UNCERTAINTIES.md has no  │
│ machine-readable status field. 1369 of 3125 rows carry a marker word; the     │
│ rest cannot be classified by any text rule that does not also mis-fire. A     │
│ `status:` column (open / narrowed / resolved / retracted) would make this     │
│ sweep reliable and is an owner decision, not a mechanical edit.               │
└───────────────────────────────────────────────────────────────────────────────┘

DETECTION RULE, fixed before the first run:
  * a U-row is "RESOLVED-marked" if its line contains any of RESOLVED, CLOSED,
    RETRACTED, ANSWERED, SETTLED, or a struck-through type (~~...~~).
    NOTE this is lossy in both directions: UNCERTAINTIES.md has heterogeneous row
    formats, and some rows (U-3559 is exactly one) state their answer in prose
    without any marker word. Those are MISSED. Under-reporting is the safer error
    for an audit whose output is a worklist.
  * a citation is "framed open" if within +/-160 chars it carries any of
    UNCERTAIN, unresolved, not resolved, open question, TBD, pending, unknown,
    "who writes", "not known", "not established", "still to", "needs".
  * a citation is "framed resolved" if it carries RESOLVED/CLOSED/RETRACTED/
    corrected/"no longer"/superseded — those are reported separately and are
    usually FINE.

Usage:
  py -3.12 re/tools/stale_uncertain_refs.py [--roots re/tools,re/frida,mashedmod/src]
                                            [--all] [--json]
"""
import argparse
import io
import json
import os
import re

ROOT = os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
UREF = re.compile(r'\bU-(\d{3,5})\b')
RESOLVED_MARK = re.compile(r'RESOLVED|CLOSED|RETRACTED|ANSWERED|SETTLED|~~', re.I)
OPEN_WORDS = re.compile(
    r'UNCERTAIN|unresolved|not resolved|open question|\bTBD\b|pending|unknown|'
    r'who writes|not known|not established|still to|needs\b', re.I)
RESOLVED_WORDS = re.compile(
    r'RESOLVED|CLOSED|RETRACTED|corrected|no longer|superseded|amended', re.I)
EXT = ('.py', '.cpp', '.h', '.hpp', '.bat', '.ps1', '.js', '.md')
SKIP_DIRS = {'__pycache__', '.git', 'build', 'obj', 'prior_art', 'archive'}


def load_rows():
    p = os.path.join(ROOT, 'UNCERTAINTIES.md')
    rows = {}
    for line in io.open(p, encoding='utf-8', errors='replace'):
        m = re.match(r'\|\s*U-(\d{3,5})\s*\|', line)
        if m:
            rows[m.group(1)] = {'resolved_marked': bool(RESOLVED_MARK.search(line)),
                                'line': line.rstrip('\n')}
    return rows


def scan(roots, rows, include_md):
    hits = []
    for root in roots:
        base = os.path.join(ROOT, root)
        if not os.path.isdir(base):
            continue
        for dp, dn, fn in os.walk(base):
            dn[:] = [d for d in dn if d not in SKIP_DIRS]
            for f in fn:
                if not f.endswith(EXT):
                    continue
                if f.endswith('.md') and not include_md:
                    continue
                path = os.path.join(dp, f)
                rel = os.path.relpath(path, ROOT).replace('\\', '/')
                if rel == 'UNCERTAINTIES.md':
                    continue
                try:
                    txt = io.open(path, encoding='utf-8', errors='replace').read()
                except OSError:
                    continue
                for m in UREF.finditer(txt):
                    uid = m.group(1)
                    row = rows.get(uid)
                    if not row or not row['resolved_marked']:
                        continue
                    ctx = txt[max(0, m.start() - 160): m.end() + 160]
                    framed_open = bool(OPEN_WORDS.search(ctx))
                    framed_res = bool(RESOLVED_WORDS.search(ctx))
                    if not framed_open or framed_res:
                        continue
                    line_no = txt.count('\n', 0, m.start()) + 1
                    snippet = re.sub(r'\s+', ' ', ctx).strip()
                    hits.append({'file': rel, 'line': line_no, 'uid': 'U-' + uid,
                                 'snippet': snippet[:230]})
    return hits


def main(argv=None):
    ap = argparse.ArgumentParser()
    ap.add_argument('--roots', default='re/tools,re/frida,mashedmod/src,scripts')
    ap.add_argument('--all', action='store_true', help='also scan re/analysis/*.md')
    ap.add_argument('--json', action='store_true')
    a = ap.parse_args(argv)

    rows = load_rows()
    roots = [r.strip() for r in a.roots.split(',') if r.strip()]
    if a.all:
        roots.append('re/analysis')
    hits = scan(roots, rows, include_md=a.all)

    nres = sum(1 for r in rows.values() if r['resolved_marked'])
    if a.json:
        print(json.dumps({'rows_total': len(rows), 'rows_resolved_marked': nres,
                          'roots': roots, 'hits': hits}, indent=1))
        return 0

    print('UNCERTAINTIES.md rows: %d, of which RESOLVED-marked: %d' % (len(rows), nres))
    print('roots scanned: %s%s' % (', '.join(roots), '' if a.all else '   (code only; --all adds re/analysis)'))
    print('\ncitations that frame a RESOLVED-marked row as OPEN: %d' % len(hits))
    print('(a search, not a verdict -- every hit needs a human read)\n')
    byf = {}
    for h in hits:
        byf.setdefault(h['file'], []).append(h)
    for f in sorted(byf):
        print('%s' % f)
        for h in byf[f]:
            print('   :%-5d %-8s %s' % (h['line'], h['uid'], h['snippet']))
        print()
    return 0


if __name__ == '__main__':
    raise SystemExit(main())
