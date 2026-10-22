"""dcleaf tracker transaction (re-classify): the vehicle-side dual copies 0x0046dbe0 / 0x00468b40 / 0x00442c80 /
0x00442ce0 / 0x0046b1c0 are one body each. 0x00442ce0 C2 -> C4, 0x0046b1c0 C3 -> C4; the other three keep their level.
New U-9787 (coverage). Line endings kept."""
import csv, io, os
W = os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
R = 'verify/dcleaf_20261022/RESULT_dcleaf.md'
D = '2026-10-22'
V = 'mashedmod/src/mashed_re/Vehicle/'
LEAVES = V + 'ForceIntegratorLeaves.cpp'
GATES = ("Exe gates knob-free on the final exe: every reference unchanged (G-CTRL c3bfcef3 / 4ed4849b / 4c3548a1 x2, B "
         "4985eb3b / b6e3d073 / 6717c15e, (b) 9/9, (e) 3/3, D2 3/3, 13 tracks). ")

# rva: (old C, new C, new file or None, new exe_file, new frida_diff or None, note)
ROWS = {
    '0046dbe0': ('C4', 'C4', None, V + 'VehicleState.cpp', None,
                 "dcleaf " + D + ": ONE body, Vehicle/VehicleState.cpp:VehicleRacePositionGet in both targets (record base "
                 "through VehRecord); the exe's private copy ForceIntegratorLeaves.cpp:CarContactCount deleted, its one "
                 "consumer RubberBandGrip (0x00442d28 / 0x00442d35) calls this body. The .asi's code is instruction-identical "
                 "to the one the C4 racediff measured (imul [esp+4],0xd04 / mov eax,[eax+0x8815a8] / ret); path1 8/8 GREEN "
                 "again. " + GATES + R + " | "),
    '00468b40': ('C3', 'C3', None, V + 'VehicleState.cpp', None,
                 "dcleaf " + D + ": ONE body, Vehicle/VehicleState.cpp:VehicleContactHistoryLookup in both targets; the "
                 "exe's private copy Collision/CarWorldContacts.cpp:ContactHistoryLookup deleted, Collision::"
                 "ContactHistoryLookup (ContactSolvers.h) forwards to this body. .asi code instruction-identical before and "
                 "after; path1 7/7 GREEN again. " + GATES + R + " | "),
    '00442c80': ('C4', 'C4', LEAVES, LEAVES, None,
                 "dcleaf " + D + ": ONE body, Vehicle/ForceIntegratorLeaves.cpp:RubberBandGate in both rsp files (target "
                 "thunks only for the mode word, DAT_007f0fd0 and refdist 0x008989b0); the .asi installs it through "
                 "RubberBandGateEntry, Util/UtilMid.cpp:ModeGatedPlayerCheck deleted. In-process A/B, hook live, canonical "
                 "race (MASHED_LEAF_SELFTEST, original run twice + the shared body per call): 6 races (track 0 x4, Warzone x2), at least "
                 "177000 calls (last summary line of each run), noo 0, ndiff 0, 9961 return 1. path1 GREEN. "
                 "Exe input U-9784 (game-mode word). " + GATES + R + " | "),
    '00442ce0': ('C2', 'C4', LEAVES, LEAVES, 'D:/MashedWT/dcleaf_data/leaf_ab (leaf_selftest logs)',
                 "dcleaf " + D + ": C2 -> C4. ONE body, Vehicle/ForceIntegratorLeaves.cpp:RubberBandGrip in both rsp files; "
                 "the .asi installs it (RubberBandGripEntry, new). In-process A/B, hook live, canonical race: the same 6 races, at "
                 "least 86000 calls, noo 0, ndiff 0, all on the default path (refdist == 0 "
                 "seen, record +0x08 never on a tier pair); tier branches by in-place seeding (MASHED_LEAF_SELFTEST_SWEEP, "
                 "16 (i1, i2) pairs x 8 band values incl. inf / denormal / FLT_MAX): 1536 cases, noo 0, ndiff 0, 516 on a "
                 "tier value. No stub in the body: the exe's Fi_GameModeTick no-op stands for FUN_0040e340, a getter "
                 "(mov eax,[0x008a94d0]; ret) whose result 0x00442d06 discards. Natural tier coverage: U-9787. "
                 + GATES + R + " | "),
    '0046b1c0': ('C3', 'C4', V + 'VehicleContactHull.cpp', V + 'VehicleContactHull.cpp', None,
                 "dcleaf " + D + ": C3 -> C4, U-9155's split folded. ONE body, Vehicle/VehicleContactHull.cpp:"
                 "VehicleBuildContactHull in both rsp files (record base through VehRecord, returns the original's 1 / 0); "
                 "the .asi installs it through the export VehicleSlotAabbExpand, the naked x87 copy "
                 "Vehicle/VehicleSlotAabbExpand.cpp deleted. In-process A/B, hook live, canonical race (snapshot of the "
                 "0xa8 output bytes, original twice + the shared body): track 0 20 calls, Warzone 24 calls (both call sites, "
                 "every spawn), noo 0, ndiff 0 on all 42 dwords and the return. path1 7/7 GREEN on the shared body. The box "
                 "input's producer stays U-9155 (exe input, not the body). " + GATES + R + " | "),
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
    assert s.count(m) == 1 and '  dcleaf:' not in s
    tail = " dcleaf: ONE body, both targets; exe gates knob-free, every reference unchanged. " + R
    lines = [
        D + "  0046b1c0  VehicleSlotAabbExpand  C3->C4  U-9155 split folded (VehicleContactHull.cpp both targets); in-process A/B 44/44, path1 7/7." + tail,
        D + "  00442ce0  VehicleRubberBandSpeedModifier  C2->C4  installed in the .asi (shared ForceIntegratorLeaves.cpp); in-process A/B 86000 natural calls + 1536 seeded tier cases, 0 diff; natural tier coverage U-9787." + tail,
        D + "  00442c80  ModeGatedPlayerCheck  C4 (kept)  body now the shared RubberBandGate; in-process A/B 177000 calls 0 diff; UtilMid copy deleted; plate markers resolved." + tail,
        D + "  00468b40  VehicleContactHistoryLookup  C3 (kept)  exe copy (CarWorldContacts.cpp) deleted; .asi code unchanged." + tail,
        D + "  0046dbe0  VehicleRacePositionGet  C4 (kept)  exe copy (CarContactCount) deleted; record base via VehRecord; .asi code unchanged." + tail,
        D + "  0040e350  GetRenderSubMode  C3 (kept)  allowlist key dropped: Vehicle/PhysicsChainHooks.cpp:118 was a false anchor (call-through), comment reworded." + tail,
    ]
    return s.replace(m, m + nl.join(lines) + nl, 1)


def unc(s, nl):
    hdr = '|----|------|-------|-----------|------------------|--------------------|--------|' + nl
    assert s.count(hdr) >= 1 and '| U-9787 |' not in s
    row = ("| U-9787 | coverage | dcleaf's in-process A/B of `0x00442ce0` (shared `Vehicle/ForceIntegratorLeaves.cpp:RubberBandGrip`) | "
           "No natural call reached a tier branch: 86000 calls over track 0 (4 runs) and Warzone (2 runs) all took the default "
           "`0*band + 1` path (refdist `0x008989b0[car]` == 0 seen, but record `+0x08` of the car and of the reference car "
           "`0x008989c8` was 0 / 0 or 1 / 1, never a tier pair; `band` was 1.0 on every call). The six tier branches "
           "(`0x00442d41..0x00442dc7`) are covered only by in-place seeding (1536 cases, 0 diff). | a natural race where record "
           "`+0x08` differs between the leader and the reference car; the writer of `+0x08` is FUN_0046dc00, called only from "
           "`0x0043dd4c..0x0043fac2` | identify the mode / event that drives FUN_0046dc00 with distinct values per car "
           "(`scenario_launch.py --watch-write` on `0x008815a8`), then rerun `MASHED_LEAF_SELFTEST` there | nothing (C4 "
           "stands on the canonical default path; this row is the natural-coverage gap) |" + nl)
    return s.replace(hdr, hdr + row, 1)


rw('hooks.csv', hooks)
rw('re/analysis/CHANGELOG.md', changelog)
rw('UNCERTAINTIES.md', unc)
print('ok')
