"""kfdot tracker transaction (re-classify, Mariano's go 2026-10-23): 0x00532b80 RpHAnimStdKeyFrameInterpolate C2 -> C3
(one body, Track/KfInterpolate.cpp, Dump train's legacy copy deleted); the 0x00532b80 half of U-9741 / D-11079 resolved,
the 0x004c4680 half stays open. Line endings kept."""
import csv, io, os, sys
W = os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
R = 'verify/kfdot_20261022/RESULT_kfdot.md'
D = '2026-10-23'
F = 'mashedmod/src/mashed_re/Track/KfInterpolate.cpp'
FD = 'D:/MashedWT/kfdot_data/leaf_ab (leaf_selftest logs; verify/kfdot_20261022/leaf_ab_summary.txt)'
EXE = sys.argv[1] if len(sys.argv) > 1 else '?'

NOTE = ("kfdot " + D + ": C2 -> C3. ONE body, Track/KfInterpolate.cpp:KfInterpolateFields in both rsp files (moved from "
        "Track/WarzonePontoon.cpp); the Dump train's sampler (TrackRenderer.cpp:LinearAnimMatrix, FUN_004b47e0) now calls "
        "it, and its copy (dot summed reversed, std::sqrt where 0x00532dc9 calls the LUT root FUN_004c3b30) is deleted "
        "(Mariano 2026-10-23; MASHED_KF_INTERP measured first, PREREG_kfdot.md). In-process A/B, hook live "
        "(MASHED_LEAF_SELFTEST, original twice through a trampoline + the shared body): Dump 3328 calls (all lerp), "
        "Warzone 4 (2 through the acos), sweep 48 synthetic cases over every branch, noo 0, ndiff 0. Knob ON was "
        "byte-identical to OFF on every gate and on all 39 13-track draw dumps and frames; final exe " + EXE + " gates "
        "equal the rpct references. C4 not claimed: natural Dump calls never leave the lerp branch. " + R + " | ")
RES = ("**0x00532b80 half RESOLVED " + D + " (kfdot, " + R + "):** one body Track/KfInterpolate.cpp, the Dump copy "
       "deleted, every gate and the Dump rows unchanged. The 0x004c4680 half stays open. ")


def rw(path, fn):
    p = os.path.join(W, path)
    s = open(p, encoding='utf-8', newline='').read()
    open(p, 'w', encoding='utf-8', newline='').write(fn(s, '\r\n' if '\r\n' in s else '\n'))


def hooks(s, nl):
    lines = s.split(nl)
    h = next(csv.reader([lines[0]]))
    ix = {k: i for i, k in enumerate(h)}
    hit = 0
    for li, line in enumerate(lines[1:], 1):
        if not line.startswith('00532b80,'):
            continue
        r = next(csv.reader([line]))
        assert r[ix['confidence']] == 'C2', r[ix['confidence']]
        r[ix['name']] = 'RpHAnimStdKeyFrameInterpolate'
        r[ix['confidence']] = 'C3'
        r[ix['status']] = 'impl'
        r[ix['file']] = F
        r[ix['exe_file']] = F
        r[ix['frida_diff']] = FD
        r[ix['notes']] = NOTE + r[ix['notes']]
        out = io.StringIO(newline='')
        csv.writer(out, lineterminator='').writerow(r)
        lines[li] = out.getvalue()
        hit += 1
    assert hit == 1, hit
    return nl.join(lines)


def changelog(s, nl):
    m = '<!-- ENTRIES -->' + nl
    assert s.count(m) == 1 and '  kfdot:' not in s
    line = (D + "  00532b80  RpHAnimStdKeyFrameInterpolate  C2->C3  shared Track/KfInterpolate.cpp; in-process A/B 3332 natural "
            "calls + 48 sweep 0 diff; Dump copy deleted (dot order + LUT root). kfdot: ONE body, both targets; exe gates "
            "unchanged; U-9741 / D-11079 half resolved. " + R)
    return s.replace(m, m + line + nl, 1)


def unc(s, nl):
    lines = s.split(nl)
    hit = 0
    for i, line in enumerate(lines):
        if line.startswith('| U-9741 |'):
            cells = line.split(' | ')
            cells[3] = RES + cells[3]
            lines[i] = ' | '.join(cells)
            hit += 1
    assert hit == 1
    return nl.join(lines)


def deferred(s, nl):
    lines = s.split(nl)
    hit = 0
    for i, line in enumerate(lines):
        if line.startswith('| D-11079 |'):
            cells = line.split(' | ')
            cells[1] = RES + cells[1]
            lines[i] = ' | '.join(cells)
            hit += 1
    assert hit == 1
    return nl.join(lines)


rw('hooks.csv', hooks)
rw('re/analysis/CHANGELOG.md', changelog)
rw('UNCERTAINTIES.md', unc)
rw('DEFERRED.md', deferred)
print('ok')
