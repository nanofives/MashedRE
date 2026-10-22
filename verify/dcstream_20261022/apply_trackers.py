"""dcstream tracker transaction (re-classify, Mariano's go 2026-10-22): 0x004cbd30 RwStreamRead and 0x004cc050 RwStreamSkip
C2 -> C3 (one body, Render/RwStreamCore.cpp); U-0111, U-0112, U-1275 resolved from the disassembly. Line endings kept."""
import csv, io, os
W = os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
R = 'verify/dcstream_20261022/RESULT_dcstream.md'
D = '2026-10-22'
F = 'mashedmod/src/mashed_re/Render/RwStreamCore.cpp'
FD = 'D:/MashedWT/dcstream_data/leaf_ab (leaf_selftest logs; verify/dcstream_20261022/leaf_ab_summary.txt)'
GATES = ("Exe gates knob-free on the final exe f2703ca2: every reference unchanged (G-CTRL bacfc747 / 8dd1c004 / 6f9932b6 x2, B "
         "f9deb9b9 / d3601fcc / 9da9a26a, (b) 9/9, (e) 3/3, D2 3/3, 13 tracks identical). The exe body is unreachable on the "
         "default build (RwWorldLoad_FromBytes has no caller), so C4 is not claimed. ")

ROWS = {
    '004cbd30': ("dcstream " + D + ": C2 -> C3, the 2026-09-29 demotion reason is gone. ONE body, Render/RwStreamCore.cpp:"
                 "RwStreamCoreRead in both rsp files, read from the disassembly (jump table 0x004cbe70; file err 5 / 0x8000001a "
                 "after FUN_00550af0, memory clamp + err 5 at 0x004cbdd8, callback [s+0x10]([s+0x1c], dst, n), default err 0xe). "
                 "The old .asi copy emitted no error at all. In-process A/B, hook live (MASHED_LEAF_SELFTEST, original twice through "
                 "a trampoline for the pure class, rewind for the file class): track 0 45568 reads, Warzone 50176 reads, noo 0, "
                 "ndiff 0 (3 short file reads per run not re-run); synthetic sweep 116 cases (memory overrun / wrap, unknown types, "
                 "callback stubs, error slot compared) 0 / 0. No natural callback stream. " + GATES + R + " | "),
    '004cc050': ("dcstream " + D + ": C2 -> C3, the 2026-09-29 demotion reason is gone. ONE body, Render/RwStreamCore.cpp:"
                 "RwStreamCoreSkip in both rsp files (n == 0 -> stream before the type, 0x004cc05a; memory overrun writes pos = "
                 "size + err 5, 0x004cc0db; callback NON-zero -> stream, 0x004cc118). The old .asi copy had the callback test "
                 "inverted, no n == 0 early-out, no pos clamp, no errors. In-process A/B: 16 natural skips per run (track 0, "
                 "Warzone) 0 / 0; the sweep covers the overrun, wrap and callback cases 0 / 0. " + GATES + R + " | "),
}

UNC = {
    'U-0111': "**RESOLVED " + D + " (dcstream, " + R + " section 2):** read from the disassembly of MASHED.exe.unpatched: "
              "push 5 at 0x004cbd81 (file short read, EOF check non-zero) and 0x004cbdd8 (memory clamp), push 0x8000001a at "
              "0x004cbda3 (file short read, EOF check zero), push 0xe at 0x004cbe44 (default). ",
    'U-0112': "**RESOLVED " + D + " (dcstream, " + R + " section 2):** push 5 at 0x004cc09d (file seek failed, EOF check "
              "non-zero) and 0x004cc0d9 (memory overrun), push 0xe at 0x004cc123 (default). ",
    'U-1275': "**RESOLVED " + D + " (dcstream, " + R + " section 2):** FUN_004cc050 advances by n: memory np = n + pos "
              "(0x004cc0d0), pos = np (0x004cc103) or pos = size + err 5 on overrun (0x004cc0db); file FUN_005509b0(h, n, 1) "
              "(0x004cc084). ",
}


def rw(path, fn):
    p = os.path.join(W, path)
    s = open(p, encoding='utf-8', newline='').read()
    open(p, 'w', encoding='utf-8', newline='').write(fn(s, '\r\n' if '\r\n' in s else '\n'))


def hooks(s, nl):
    lines = s.split(nl)
    h = next(csv.reader([lines[0]]))
    ix = {k: i for i, k in enumerate(h)}
    hit = set()
    for li, line in enumerate(lines[1:], 1):
        k = line[:8]
        if k not in ROWS or line[8:9] != ',':
            continue
        r = next(csv.reader([line]))
        assert r[ix['confidence']] == 'C2', (k, r[ix['confidence']])
        r[ix['confidence']] = 'C3'
        r[ix['file']] = F
        r[ix['exe_file']] = F
        r[ix['frida_diff']] = FD
        r[ix['notes']] = ROWS[k] + r[ix['notes']]
        out = io.StringIO(newline='')
        csv.writer(out, lineterminator='').writerow(r)
        lines[li] = out.getvalue()
        hit.add(k)
    assert hit == set(ROWS), hit
    return nl.join(lines)


def changelog(s, nl):
    m = '<!-- ENTRIES -->' + nl
    assert s.count(m) == 1 and '  dcstream:' not in s
    tail = " dcstream: ONE body, both targets; exe gates knob-free, every reference unchanged. " + R
    lines = [
        D + "  004cbd30  RwStreamRead  C2->C3  shared Render/RwStreamCore.cpp (4-type dispatch + error emits from the disassembly); "
            "in-process A/B 95744 reads 0 diff (track 0 + Warzone), sweep 116/0/0; U-0111 resolved." + tail,
        D + "  004cc050  RwStreamSkip  C2->C3  shared Render/RwStreamCore.cpp (zero early-out, pos clamp, callback non-zero = ok); "
            "32 natural skips + sweep 0 diff; U-0112, U-1275 resolved." + tail,
    ]
    return s.replace(m, m + nl.join(lines) + nl, 1)


def unc(s, nl):
    lines = s.split(nl)
    hit = set()
    for i, line in enumerate(lines):
        for uid, text in UNC.items():
            if line.startswith('| ' + uid + ' |'):
                cells = line.split(' | ')
                assert cells[1] == 'structural', (uid, cells[1])
                cells[1] = '~~structural~~ **RESOLVED ' + D + '** (dcstream)'
                cells[3] = text + cells[3]
                cells[-1] = 'nothing |' if cells[-1].endswith('|') else cells[-1]
                lines[i] = ' | '.join(cells)
                hit.add(uid)
    assert hit == set(UNC), hit
    return nl.join(lines)


rw('hooks.csv', hooks)
rw('re/analysis/CHANGELOG.md', changelog)
rw('UNCERTAINTIES.md', unc)
print('ok')
