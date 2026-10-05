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

[FIXED 2026-10-05, same day. The KA now PASSES — see the box below for what was
wrong and `verify/d4_save_20261005/RESULT_STALESWEEP2.md` for the survey that
produced the fix. No schema change was needed: UNCERTAINTIES.md already carries TWO
status mechanisms and the fix was to read both, plus report a third bucket the
first version silently dropped.

  * a RESOLVED SECTION exists (header `| ID | Type | Resolved date | Resolution |`)
    — but holds only 60 of 3125 rows, so it is ~98 % unused.
  * a marker-in-the-Type-cell convention (`~~structural~~ **RESOLVED …**`) — 1362
    ACTIVE rows carry one.
  * and 168 further ACTIVE rows read as resolved IN PROSE ONLY, with no marker.
    **U-3559 is one of those, which is precisely why the first version missed it.**
    These are now reported as a separate LOW-CONFIDENCE bucket instead of being
    dropped.]

┌───── ORIGINAL FAILURE, KEPT SO THE MISTAKE IS NOT REPEATED ───────────────────┐
│ THE FIRST VERSION FAILED ITS OWN KNOWN-ANSWER CHECK.                          │
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
│ I concluded "the real fix is a schema change". **That was wrong** — see the   │
│ note above. The file already had the mechanisms; I had not surveyed it.       │
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

# A SYSTEMATIC BENIGN CLASS, measured: scripts/reclassify_batch_*.py carry lines like
# "Mint U-IDs from U-8000 for every bare [UNCERTAIN] marker in the 156 plates". That
# is MINT-TIME PROVENANCE describing what the batch created, not a claim that the row
# is still open. It accounted for 9 of 19 hits on the first clean run — nearly half
# the output, all noise. Skipped by default; --include-batch-scripts keeps them.
BATCH_PROVENANCE = re.compile(r'reclassify_batch_\w+\.py$')


# A row resolved in PROSE with no marker. Deliberately narrow and deliberately
# low-confidence: these phrases are how U-3559 ("Tail [...] IS written by ...") and
# U-3560 ("HEX CORRECTED") state an answer without any marker word. Hits from this
# bucket are reported SEPARATELY and must not be mixed with marker hits.
PROSE_RESOLVED = re.compile(
    r'\bIS written by\b|\bANSWERED\b|\bNARROWED\b|\bCONFIRMED\b|\bREFUTED\b|'
    r'\bno second writer\b|\bHEX CORRECTED\b|\bCORRECTED\b', re.I)

# the Resolved-section header; rows after it are resolved by filing, not by wording
RESOLVED_SECTION_HDR = re.compile(r'^\|\s*ID\s*\|\s*Type\s*\|\s*Resolved date\s*\|')


def load_rows():
    """Classify every U-row into one of three states.

    UNCERTAINTIES.md carries TWO status mechanisms already (surveyed 2026-10-05):
    a Resolved SECTION holding 60 of 3125 rows, and a marker-in-Type convention on
    1362 ACTIVE rows. A third group, 168 ACTIVE rows, states its answer in prose
    only. The first version of this tool read only the marker and so missed U-3559
    and its whole class.
    """
    p = os.path.join(ROOT, 'UNCERTAINTIES.md')
    rows = {}
    in_resolved_section = False
    for line in io.open(p, encoding='utf-8', errors='replace'):
        if RESOLVED_SECTION_HDR.match(line):
            in_resolved_section = True
            continue
        m = re.match(r'\|\s*U-(\d{3,5})\s*\|', line)
        if not m:
            continue
        if in_resolved_section:
            state = 'section'          # highest confidence: filed as resolved
        elif RESOLVED_MARK.search(line):
            state = 'marker'           # high: explicit RESOLVED/CLOSED/~~ marker
        elif PROSE_RESOLVED.search(line):
            state = 'prose'            # LOW: reads resolved, no marker. Needs a human.
        else:
            state = 'open'
        rows[m.group(1)] = {'state': state,
                            'resolved_marked': state in ('section', 'marker'),
                            'line': line.rstrip('\n')}
    return rows


def scan(roots, rows, include_md, include_batch=False):
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
                if not include_batch and BATCH_PROVENANCE.search(rel):
                    continue
                try:
                    txt = io.open(path, encoding='utf-8', errors='replace').read()
                except OSError:
                    continue
                for m in UREF.finditer(txt):
                    uid = m.group(1)
                    row = rows.get(uid)
                    if not row or row['state'] == 'open':
                        continue
                    ctx = txt[max(0, m.start() - 160): m.end() + 160]
                    framed_open = bool(OPEN_WORDS.search(ctx))
                    framed_res = bool(RESOLVED_WORDS.search(ctx))
                    if not framed_open or framed_res:
                        continue
                    line_no = txt.count('\n', 0, m.start()) + 1
                    snippet = re.sub(r'\s+', ' ', ctx).strip()
                    hits.append({'file': rel, 'line': line_no, 'uid': 'U-' + uid,
                                 'row_state': row['state'],
                                 'snippet': snippet[:230]})
    return hits


def main(argv=None):
    ap = argparse.ArgumentParser()
    ap.add_argument('--roots', default='re/tools,re/frida,mashedmod/src,scripts')
    ap.add_argument('--all', action='store_true', help='also scan re/analysis/*.md')
    ap.add_argument('--include-batch-scripts', action='store_true',
                    help='keep scripts/reclassify_batch_*.py mint-time provenance (noise)')
    ap.add_argument('--json', action='store_true')
    a = ap.parse_args(argv)

    rows = load_rows()
    roots = [r.strip() for r in a.roots.split(',') if r.strip()]
    if a.all:
        roots.append('re/analysis')
    hits = scan(roots, rows, include_md=a.all, include_batch=a.include_batch_scripts)

    import collections
    st = collections.Counter(r['state'] for r in rows.values())
    if a.json:
        print(json.dumps({'rows_total': len(rows), 'row_states': dict(st),
                          'roots': roots, 'hits': hits}, indent=1))
        return 0

    print('UNCERTAINTIES.md rows: %d' % len(rows))
    print('  resolved by SECTION filing : %d  (highest confidence)' % st['section'])
    print('  resolved by Type MARKER    : %d  (high)' % st['marker'])
    print('  reads resolved in PROSE    : %d  (LOW -- needs a human read)' % st['prose'])
    print('  no resolution signal       : %d' % st['open'])
    print('roots scanned: %s%s' % (', '.join(roots),
                                   '' if a.all else '   (code only; --all adds re/analysis)'))

    for state, label in (('section', 'RESOLVED-SECTION'), ('marker', 'MARKER-RESOLVED'),
                         ('prose', 'PROSE-ONLY (LOW CONFIDENCE)')):
        sub = [h for h in hits if h['row_state'] == state]
        print('\n=== citations framing a %s row as OPEN: %d ===' % (label, len(sub)))
        if state == 'prose' and sub:
            print('    this is the bucket the first version dropped; U-3559 lives here')
        byf = {}
        for h in sub:
            byf.setdefault(h['file'], []).append(h)
        for f in sorted(byf):
            print('  %s' % f)
            for h in byf[f]:
                print('     :%-5d %-8s %s' % (h['line'], h['uid'], h['snippet']))
    print('\n(a search, not a verdict -- every hit needs a human read)')
    return 0


if __name__ == '__main__':
    raise SystemExit(main())
