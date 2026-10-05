#!/usr/bin/env python3
"""Triage the asi-only TUs: which could join the shipping exe cheaply, which are
blocked, and which are dev-only instruments by design.

WHY. `mashed_re.exe` is the deliverable; `mashed_re_dev.asi` is dev-only and never
ships (CLAUDE.md). 255 of 422 ported TUs are in `asi_sources.rsp` ONLY, carrying the
majority of the project's C3/C4 rows -- i.e. verified work that is not in the
deliverable. Before spending a campaign on it, measure which ones are cheap.

THE DISCRIMINATOR, and it comes from the build rather than from taste. The exe is
compiled with `/DMASHED_STANDALONE` (`mashedmod/build.bat:210`, `:212`) and dual-target
TUs use `#ifdef MASHED_STANDALONE` to swap absolute original-image VAs for private
storage -- `Gameplay/PickupPoolSpawn.cpp:79,118,141,149` is the worked example. So what
stops a TU joining the exe is the set of absolute original-image addresses it still
touches in CODE:

  0x00400000..0x004fffff  .text of the original. UNMAPPED in the standalone
                          (`Compat/StandaloneRvaThunks.h:7`; an access AVs, recorded at
                          `Frontend/MenuButtonDetect.cpp:71`). A CALL here is a hard
                          blocker needing an exe-side body or a registered thunk.
  0x00500000..0x009fffff  VirtualAlloc-mapped BLANK by `exe_main.cpp:54`, so reads
                          return 0 and writes go nowhere observable. Not a crash --
                          an INERTNESS risk (U-9187's root cause).

COMMENTS ARE STRIPPED FIRST, and that is not a detail: every ported function cites its
RVA in a `// 0x00xxxxxx` comment, so an unstripped scan reports ~100% of TUs as
reaching into the original image. The counts below are code references only.

Usage:
  py -3.12 re/tools/asi_only_triage.py [--csv OUT.csv] [--verbose] [--class CLASS]
"""
import argparse
import collections
import csv
import io
import os
import re
import sys

ROOT = os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))

LO_CODE, HI_CODE = 0x00400000, 0x004fffff      # original .text -- unmapped, AV
LO_BLANK, HI_BLANK = 0x00500000, 0x009fffff    # blank-mapped -- reads zero

ADDR = re.compile(r'0[xX]00[0-9a-fA-F]{6}\b')
# a function-pointer cast onto an absolute address == a CALL into the original image
FNCAST = re.compile(r'reinterpret_cast\s*<[^>]*\(\s*\*\s*\)[^>]*>\s*\(\s*(0[xX]00[0-9a-fA-F]{6})',
                    re.S)
FNCAST2 = re.compile(r'reinterpret_cast\s*<\s*fn[_A-Za-z0-9]*\s*>\s*\(\s*(0[xX]00[0-9a-fA-F]{6})',
                     re.S)
# markers that a TU exists to observe the ORIGINAL, not to replace it
DEVONLY = (
    'asi-only', 'asi only', 'dev-only', 'dev only', 'never shipped',
    'reads live globals', 'diff instrument', 'instrumentation only',
    'original-side', 'for verification only',
)


def strip_comments(s):
    s = re.sub(r'/\*.*?\*/', ' ', s, flags=re.S)
    s = re.sub(r'//[^\n]*', ' ', s)
    s = re.sub(r'"(?:\\.|[^"\\])*"', '""', s)      # string literals too
    return s


def tu_map(p):
    s = io.open(os.path.join(ROOT, p), encoding='utf-8', errors='replace').read()
    out = {}
    for m in re.findall(r'[A-Za-z0-9_./\\-]+\.cpp', s):
        out[m.replace('\\', '/').split('/')[-1].lower()] = m.replace('\\', '/')
    return out


def find_file(rel):
    cand = os.path.join(ROOT, 'mashedmod', rel)
    if os.path.exists(cand):
        return cand
    base = os.path.basename(rel)
    for dp, _dn, fn in os.walk(os.path.join(ROOT, 'mashedmod', 'src')):
        if base in fn:
            return os.path.join(dp, base)
    return None


def code_addrs(code):
    """Original-.text addresses referenced in CODE, EXCLUDING hook-registration
    arguments.

    MEASURED 2026-10-05, and it changed this tool's verdict: of 662 such references
    across the 197 TUs first classed BLOCKED-CODEADDR, **471 (71.1 %) are
    `RH_ScopedInstall` arguments** and **163 of the 197 TUs have no other kind**. A
    hook registration names the ORIGINAL address a body replaces -- it is the
    `.asi` install mechanism and is meaningless in a standalone exe that has nothing
    to hook. Counting it as a blocker overstated the blocked pool by ~4x, so it is
    excluded here and reported separately as `n_install_only`.
    """
    keep, install = [], []
    for m in ADDR.finditer(code):
        a = int(m.group(0), 16)
        if not (LO_CODE <= a <= HI_CODE):
            continue
        before = re.sub(r'\s+', ' ', code[max(0, m.start() - 90):m.start()])
        (install if 'RH_ScopedInstall' in before else keep).append(a)
    return sorted(set(keep)), sorted(set(install))


def classify(path):
    raw = io.open(path, encoding='utf-8', errors='replace').read()
    code = strip_comments(raw)
    low = raw.lower()

    addrs = [int(a, 16) for a in ADDR.findall(code)]
    codeb, installs = code_addrs(code)
    blank = sorted({a for a in addrs if LO_BLANK <= a <= HI_BLANK})
    calls = sorted({int(m, 16) for m in FNCAST.findall(code) + FNCAST2.findall(code)
                    if LO_CODE <= int(m, 16) <= HI_CODE})

    guarded = 'MASHED_STANDALONE' in code
    devonly = any(k in low for k in DEVONLY)

    if devonly and not guarded:
        cls = 'DEV-ONLY-BY-DESIGN'
    elif calls:
        cls = 'BLOCKED-CALLOUT'
    elif codeb:
        cls = 'BLOCKED-CODEADDR'
    elif blank:
        cls = 'NEEDS-STORAGE'
    else:
        cls = 'CHEAP'
    return {'class': cls, 'n_code_addr': len(codeb), 'n_blank_addr': len(blank),
            'n_callouts': len(calls), 'n_install_only': len(installs),
            'guarded': guarded, 'devonly_marker': devonly,
            'callouts': ['0x%08x' % a for a in calls[:6]],
            'blank': ['0x%08x' % a for a in blank[:6]]}


def main(argv=None):
    ap = argparse.ArgumentParser()
    ap.add_argument('--csv')
    ap.add_argument('--verbose', action='store_true')
    ap.add_argument('--class', dest='only_class')
    a = ap.parse_args(argv)

    exe, asi = (tu_map('mashedmod/exe_sources.rsp'),
                tu_map('mashedmod/asi_sources.rsp'))
    only = {k: v for k, v in asi.items() if k not in exe}

    rows = list(csv.DictReader(io.open(os.path.join(ROOT, 'hooks.csv'),
                                       encoding='utf-8', errors='replace')))
    by_tu = collections.defaultdict(list)
    for r in rows:
        b = (r.get('file') or '').replace('\\', '/').split('/')[-1].lower()
        if b.endswith('.cpp'):
            by_tu[b].append(r)

    out, missing = [], []
    for base, rel in sorted(only.items()):
        p = find_file(rel)
        if not p:
            missing.append(rel)
            continue
        c = classify(p)
        hr = by_tu.get(base, [])
        c['tu'] = rel
        c['rows'] = len(hr)
        for lv in ('C1', 'C2', 'C3', 'C4'):
            c[lv] = sum(1 for r in hr if r.get('confidence') == lv)
        out.append(c)

    print('asi-only TUs: %d (exe %d, asi %d)%s'
          % (len(only), len(exe), len(asi),
             '   [%d source files not found]' % len(missing) if missing else ''))
    print()
    order = ['CHEAP', 'NEEDS-STORAGE', 'BLOCKED-CODEADDR', 'BLOCKED-CALLOUT',
             'DEV-ONLY-BY-DESIGN']
    print('%-20s %5s %6s %6s %6s %6s %6s' % ('class', 'TUs', 'rows', 'C2', 'C3', 'C4', 'guarded'))
    for cl in order:
        g = [c for c in out if c['class'] == cl]
        if not g:
            continue
        print('%-20s %5d %6d %6d %6d %6d %6d'
              % (cl, len(g), sum(c['rows'] for c in g), sum(c['C2'] for c in g),
                 sum(c['C3'] for c in g), sum(c['C4'] for c in g),
                 sum(1 for c in g if c['guarded'])))
    tot = [c for c in out]
    print('%-20s %5d %6d %6d %6d %6d %6d'
          % ('TOTAL', len(tot), sum(c['rows'] for c in tot), sum(c['C2'] for c in tot),
             sum(c['C3'] for c in tot), sum(c['C4'] for c in tot),
             sum(1 for c in tot if c['guarded'])))

    print()
    print('verified (C3+C4) rows by class:')
    for cl in order:
        g = [c for c in out if c['class'] == cl]
        if g:
            print('  %-20s %4d' % (cl, sum(c['C3'] + c['C4'] for c in g)))

    if a.verbose or a.only_class:
        print()
        for c in sorted(out, key=lambda x: -(x['C3'] + x['C4'])):
            if a.only_class and c['class'] != a.only_class:
                continue
            print('%-22s %-44s rows %3d  C3+C4 %3d  code %2d blank %3d callouts %2d%s'
                  % (c['class'], c['tu'][-44:], c['rows'], c['C3'] + c['C4'],
                     c['n_code_addr'], c['n_blank_addr'], c['n_callouts'],
                     '  [guarded]' if c['guarded'] else ''))
            if c['callouts']:
                print('      callouts: %s' % ', '.join(c['callouts']))

    if a.csv:
        with io.open(a.csv, 'w', newline='', encoding='utf-8') as fh:
            w = csv.DictWriter(fh, fieldnames=[
                'tu', 'class', 'rows', 'C1', 'C2', 'C3', 'C4', 'n_code_addr',
                'n_blank_addr', 'n_callouts', 'n_install_only', 'guarded', 'devonly_marker'],
                extrasaction='ignore')
            w.writeheader()
            for c in sorted(out, key=lambda x: (x['class'], -(x['C3'] + x['C4']))):
                w.writerow(c)
        print('\nwrote %s' % a.csv)
    return 0


if __name__ == '__main__':
    raise SystemExit(main())
