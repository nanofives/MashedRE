"""dcmix tracker transaction (re-classify, Mariano's go 2026-10-22): the mixed dual-copy residue. 0x0040e180 C2 -> C4
(new U-9788, coverage), 0x0045baa0 C2 -> C3; 0x0055dec0 and 0x0055e200 keep their level. Line endings kept."""
import csv, io, os
W = os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
R = 'verify/dcmix_20261022/RESULT_dcmix.md'
D = '2026-10-22'
S = 'mashedmod/src/mashed_re/'
GATES = ("Exe gates knob-free on the final exe 7ea8a2e2: every reference unchanged (G-CTRL c3bfcef3 / 4ed4849b / 4c3548a1 x2, B "
         "4985eb3b / b6e3d073 / 6717c15e, (b) 9/9, (e) 3/3, D2 3/3, 13 tracks identical). ")

# rva: (old C, new C, new file or None, new exe_file, new frida_diff or None, note)
ROWS = {
    '0040e180': ('C2', 'C4', S + 'Race/MostSepPair.cpp', S + 'Race/MostSepPair.cpp', 'D:/MashedWT/dcmix_data/leaf_ab (leaf_selftest logs)',
                 "dcmix " + D + ": C2 -> C4. ONE body, Race/MostSepPair.cpp:RaceCamera::MostSeparatedPair in both rsp files "
                 "(magnitude per target: exe Vec3Magnitude, .asi the original 0x004c3ac0, as before). The .asi export "
                 "MostSepPair_0040e180 fills RaceCamCar[4] from the original getters and calls it. In-process A/B, hook live, "
                 "canonical race (MASHED_LEAF_SELFTEST, original twice through a trampoline + the shared body per call): track 0 "
                 "60 s 13000 calls, Warzone 120 s 32000 calls (incl. the one-survivor tail), noo 0, ndiff 0. Coverage gap "
                 "(dying-but-alive input not shown): U-9788. The 'exe uses std::sqrt' note below is stale since dualcopy4. "
                 + GATES + R + " | "),
    '0045baa0': ('C2', 'C3', None, S + 'Powerup/PowerupTypeSearch.cpp', 'log/diff_search_45baa0.csv (dcmix, early_window_leaf_diff)',
                 "dcmix " + D + ": C2 -> C3, the 2026-09-29 demotion reason is gone. ONE body, Powerup/PowerupTypeSearch.cpp in "
                 "both rsp files (entry or NULL, count<=0 -> NULL, as 0x0045baa0..0x0045babf); the .asi Search45baa0 passes "
                 "0x005f9998 / [0x005f9bd8] / 0x40, the exe Lookup passes s_table / 9 / sizeof(TypeEntry) and maps the entry back "
                 "to the index (9 distinct codes). path1 early_window_leaf_diff GREEN, 3 seeded cases, 0 mismatches. "
                 + GATES + R + " | "),
    '0055dec0': ('C3', 'C3', None, S + 'Util/UtilLeaves_ab6.cpp', None,
                 "dcmix " + D + ": ONE body, Util/UtilLeaves_ab6.cpp:UtilDeref55dec0 in both targets; RwpBuildExterns.cpp's "
                 "FUN_0055dec0 deleted, PhysicsWorldBuild.cpp (FUN_0047d3c0's 4-arg call, 0x0047d5ee) calls the shared one. "
                 ".asi code unchanged. " + GATES + R + " | "),
    '0055e200': ('C4', 'C4', None, S + 'Collision/RwpIntegrator.cpp', None,
                 "dcmix " + D + ": the RwpSolverCore23.cpp 'body' was an extern declaration (scanner fixed). Note: the original "
                 "copies through x87 (fld [esp+8] / fstp [eax], 0x0055e200..0x0055e20a), the port copies 32 bits: equal for "
                 "every non-NaN single, x87 quiets a signalling NaN; callers pass 0x358637bd (0x00449e28, 0x00449e7d) or "
                 "FUN_0047e9c0's arg 2. " + R + " | "),
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
        oldc, newc, f, ef, fd, note = ROWS[k]
        r = next(csv.reader([line]))
        assert r[ix['confidence']] == oldc, (k, r[ix['confidence']])
        r[ix['confidence']] = newc
        if newc == 'C4' and oldc != 'C4':
            r[ix['status']] = 'verified'
        if f:
            r[ix['file']] = f
        r[ix['exe_file']] = ef
        if fd:
            r[ix['frida_diff']] = fd
        r[ix['notes']] = note + r[ix['notes']]
        out = io.StringIO(newline='')
        csv.writer(out, lineterminator='').writerow(r)
        lines[li] = out.getvalue()
        hit.add(k)
    assert hit == set(ROWS), hit
    return nl.join(lines)


def changelog(s, nl):
    m = '<!-- ENTRIES -->' + nl
    assert s.count(m) == 1 and '  dcmix:' not in s
    tail = " dcmix: ONE body, both targets; exe gates knob-free, every reference unchanged. " + R
    lines = [
        D + "  0040e180  MostSeparatedPair  C2->C4  shared Race/MostSepPair.cpp; in-process A/B 45000 calls 0 diff (track 0 + Warzone); coverage U-9788." + tail,
        D + "  0045baa0  PowerupTypeLookup  C2->C3  shared Powerup/PowerupTypeSearch.cpp (entry or NULL); path1 GREEN 3/3." + tail,
        D + "  0055dec0  FUN_0055dec0  C3 (kept)  RwpBuildExterns.cpp copy deleted; .asi code unchanged." + tail,
        D + "  0055e200  RwpSolverContextSet  C4 (kept)  allowlist key dropped: the second 'body' was an extern declaration (scanner fixed); SNaN note." + tail,
        D + "  0042d3e0  MenuEntryArrayInit  level kept  allowlist key dropped: the finding bound a static_assert line (scanner fixed); one body since dualcopy4." + tail,
    ]
    return s.replace(m, m + nl.join(lines) + nl, 1)


def unc(s, nl):
    hdr = '|----|------|-------|-----------|------------------|--------------------|--------|' + nl
    assert s.count(hdr) >= 1 and '| U-9788 |' not in s
    row = ("| U-9788 | coverage | dcmix's in-process A/B of `0x0040e180` (shared `Race/MostSepPair.cpp`) | The leaf log records "
           "the pairs (original x2 and the shared body), not the per-car inputs, so no logged call is shown to have a car that is "
           "active and alive (`FUN_0046c7b0` == 1) with the `FUN_0046cbb0` dead flag set. 45000 calls, 0 diff, pairs (0,1) (0,2) "
           "(0,3) (1,2) (1,3) and the one-survivor tail. | a call with a dying-but-alive car | add the four per-car flags to the "
           "leaf line and rerun on a race with an elimination (Warzone `--hold 120`) | nothing (C4 stands on the canonical runs; "
           "this row is the natural-coverage gap) |" + nl)
    return s.replace(hdr, hdr + row, 1)


rw('hooks.csv', hooks)
rw('re/analysis/CHANGELOG.md', changelog)
rw('UNCERTAINTIES.md', unc)
print('ok')
