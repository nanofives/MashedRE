"""dcphys tracker transaction (re-classify): A3 0x0046b540 C2 -> C4 on the shared body, A6a 0x00467650 stays C2 (clean only
with the default-OFF MASHED_A6A_DOT_ORDER), 0x0046b1c0's exe_file moved; U-9783 filed; three CHANGELOG lines. Line endings kept."""
import csv, io, os
W = os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
R = 'verify/dcphys_20261021/RESULT_dcphys.md'
D = '2026-10-21'


def rw(path, fn):
    p = os.path.join(W, path)
    s = open(p, encoding='utf-8', newline='').read()
    open(p, 'w', encoding='utf-8', newline='').write(fn(s, '\r\n' if '\r\n' in s else '\n'))


A3_NOTE = ("dcphys " + D + ": ONE body, Vehicle/VehicleInit.cpp in BOTH rsp lists (exe_file == file); the .asi's verbatim "
           "copy (C4 24/24) deleted. C4 on the shared body: hook installed live (A3_Entry inline-JMP @0x0046b540, "
           "MASHED_HOOK_ONLY), canonical race (track 0) and Warzone (track 11), in-process A/B against the live original "
           "(OrigA3 trampoline) over the full 0xd04 record + 0x00613108/14/30/3c + return: 52/52 bit-identical over 4 races, "
           "no-op control (original run twice) 0 on every call. Exe changes, all knob-free and G-CTRL / (b) / (e) / D2 "
           "unchanged: original order (handling walk before the slot check 0x0046b5e2), full 13-entry table from the "
           "image (0x00613140..0x00613244; tag 18 spring 42000), _DAT_005cea44 bits 0x3e8e38e4 (was 0.277778f = 0x3e8e38eb), "
           "hull builder 0x0046b1c0 moved to the spawn loop before A3 (0x0040ed62 / 0x0040ed68 order). Target thunks only: "
           "record base, selector input (standalone keeps the measured type 6, U-9783), table pointer, handling-global "
           "sink. " + R + " | ")
A6A_NOTE = ("dcphys " + D + ": ONE body, Vehicle/Integrate2.cpp in BOTH rsp lists (exe_file == file); the .asi's verbatim "
            "copy (C4 72/72, 14 naked shims) deleted. Shared body == verbatim copy on every sampled call (2 x 1376, "
            "track 0). BOTH missed the original on 174/1440 and 25/1376 calls (x20 quotas; the 72-call C4 sample fell "
            "inside the first ~100 calls, all clean), +0x9b0..+0x9c4 only: the three 3-term dots in Ghidra's print "
            "order. MASHED_A6A_DOT_ORDER=1 (default OFF; 0x00467be9..0x00467c0c, 0x0046837d..0x00468394, "
            "0x00468771..0x00468793) closes it: 6551/6551 bit-identical, hook live, tracks 0 and 11 (+ --boost), no-op "
            "control 0. STAYS C2: the shipping default is the knob-OFF body; C4 evidence exists for the knob-ON body, "
            "pending Mariano's flip. Airborne branch not sampled in this lane (air=0 in every run). Dead +0x478 clamp "
            "removed (gear 0..4 by its writers 0x00467982 / 0x0046799a / 0x0046bc66). " + R + " | ")
HULL_NOTE = ("dcphys " + D + ": exe body moved verbatim to Vehicle/VehicleContactHull.cpp (VehicleSpawnHull, called by "
             "the spawn loop before A3) so that VehicleInit.cpp can be A3's one shared TU; the U-9155 split by target is "
             "unchanged. " + R + " | ")


def hooks(s, nl):
    # splice only the three edited lines; every other row stays byte-identical
    lines = s.split(nl)
    h = next(csv.reader([lines[0]]))
    ix = {k: i for i, k in enumerate(h)}
    seen = set()
    for li, line in enumerate(lines[1:], 1):
        if not line[:9] in ('0046b540,', '00467650,', '0046b1c0,'):
            continue
        r = next(csv.reader([line]))
        if r[0] == '0046b540':
            assert r[ix['confidence']] == 'C2'
            r[ix['confidence']] = 'C4'
            r[ix['file']] = r[ix['exe_file']] = 'mashedmod/src/mashed_re/Vehicle/VehicleInit.cpp'
            r[ix['notes']] = A3_NOTE + r[ix['notes']]
            seen.add(r[0])
        elif r[0] == '00467650':
            assert r[ix['confidence']] == 'C2'
            r[ix['file']] = r[ix['exe_file']] = 'mashedmod/src/mashed_re/Vehicle/Integrate2.cpp'
            r[ix['notes']] = A6A_NOTE + r[ix['notes']]
            seen.add(r[0])
        elif r[0] == '0046b1c0':
            r[ix['exe_file']] = 'mashedmod/src/mashed_re/Vehicle/VehicleContactHull.cpp'
            r[ix['notes']] = HULL_NOTE + r[ix['notes']]
            seen.add(r[0])
        out = io.StringIO(newline='')
        csv.writer(out, lineterminator='').writerow(r)
        lines[li] = out.getvalue()
    assert seen == {'0046b540', '00467650', '0046b1c0'}, seen
    return nl.join(lines)


def uncertainties(s, nl):
    assert '| U-9783 |' not in s
    hdr = '|----|------|-------|-----------|------------------|--------------------|--------|' + nl
    assert s.count(hdr) == 1
    row = ("| U-9783 | structural | A3 `0x0046b540`'s handling-type selector input in the standalone "
           "(`Vehicle/VehicleInit.cpp` `HandlingTypeIndex`) | The original's type is "
           "`(*(int**)0x005f2770)[v][1]` with `v = [0x0067f17c]`, or `[0x0067ea94]` when `[0x0067e9fc] == 10` "
           "(0x0046b56c..0x0046b585). The standalone has no producer for the `0x005f2770` object array, so it uses the "
           "type measured live on the reference scenario (6: `_DAT_00613108` = 105.0, U-9147) for every car and track. "
           "The .asi reads the live chain, and A3's shared body is bit-identical there (52/52, dcphys). | which object "
           "array `0x005f2770` points to, who fills it, and its `+4` value per car / track | find the writer of "
           "`[0x005f2770]` (`--watch-write`) and peek `(*(int**)0x005f2770)[v][1]` over several car and track selections; "
           "then port the producer or table it | the standalone's handling type off the reference car (A3's input, not "
           "its body: A3 is C4 on the shared body) |" + nl)
    return s.replace(hdr, hdr + row, 1)


def changelog(s, nl):
    m = '<!-- ENTRIES -->' + nl
    assert s.count(m) == 1 and '  dcphys  ' not in s
    lines = [
        D + "  0046b540  VehicleSpawnInit  C2->C4  dcphys: ONE body (Vehicle/VehicleInit.cpp in both rsp lists, .asi verbatim "
        "copy deleted, CROSS-TARGET:0046b540 off the allowlist). Hook live, canonical race + Warzone, in-process full-state "
        "A/B (record + 4 handling globals + return) 52/52 bit-identical, no-op control 0. Knob-free exe changes (original "
        "order, 13-entry image table, _DAT_005cea44 bits, hull before A3 in the spawn loop) moved no gate. U-9783 filed "
        "(standalone selector input). " + R,
        D + "  00467650  VehicleWheelDrivetrainUpdate  C2 (no change)  dcphys: ONE body (Vehicle/Integrate2.cpp in both rsp "
        "lists, .asi verbatim copy + 14 shims deleted, CROSS-TARGET:00467650 off the allowlist). Shared == verbatim on every "
        "sampled call; both missed the original on 174/1440 and 25/1376 calls (three 3-term dot associations). New "
        "MASHED_A6A_DOT_ORDER (default OFF) closes it, 6551/6551. Stays C2 at the knob-OFF default; flip is Mariano's. " + R,
        D + "  0046b1c0  VehicleSlotAabbExpand  C3 (no change)  dcphys: exe_file moved to Vehicle/VehicleContactHull.cpp "
        "(verbatim move, allowlist key updated; U-9155 split unchanged). " + R,
    ]
    return s.replace(m, m + nl.join(lines) + nl, 1)


rw('hooks.csv', hooks)
rw('UNCERTAINTIES.md', uncertainties)
rw('re/analysis/CHANGELOG.md', changelog)
print('applied')
