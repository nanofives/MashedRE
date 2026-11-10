"""PORT-OT-RENDER-2 (L-111 step 1, L-115): BINDINGS.tsv rows for the RENDER leftovers of PORT-OT-RENDER on the MASHED_ORIG_TOP
path: 0x00475a00 on its L-115 near-match body (93.1%, SIB base/index order only, port axis only), its teardown 0x00475d90 on
the matched body, and the reasons 0x00490000 / 0x0048ff20 stay stand-ins and 0x0041a980 does not run.

Chain (no script runs another tip's main, which would re-run othud2 and reset the other tip's rows): run
update_bindings_othud2.py once, apply the FIXED tables of update_bindings_otother2.py and update_bindings_otpickuppool.py in
that order, then the rows below. `--check-base` stops after the two tables and compares the result with the BINDINGS.tsv
that was there before the run (it must be byte-identical on orch/integration 7bed7a482), then restores it. Idempotent.
usage: py -3.12 verify/portorigtop_20261109/update_bindings_otrender2.py [--check-base]
"""
import importlib.util, os, subprocess, sys
from collections import Counter

HERE = os.path.dirname(os.path.abspath(__file__))
TOP = os.path.join(os.path.dirname(HERE), 'porttop_20261108')
TSV = os.path.join(TOP, 'BINDINGS.tsv')
T = 'PORT-OT-RENDER-2: '

FIXED = {  # rva -> (name or None, kind, symbol, note, level, caller)
    0x00475a00: (None, 'near-match', 'Orig_00475a00',
                 T + 'L-115 near-match body, NOT matched (port axis only, hooks C2 unchanged): Match/PortTop_00475a00.cpp = '
                 're/match/s12/00475a00.c, 93.1% (regs 100, ops 100, 29/29 ins), match.py --bytes 2 differing bytes of 91, '
                 'both SIB: 0x00475a41 orig 17 [edi+edx] / DDK 3a [edx+edi], 0x00475a49 orig 07 [edi+eax] / DDK 38 '
                 '[eax+edi] (scale 1, no displacement, no EBP/ESP base: same address, segment and value). relocheck OK 9/9. '
                 'Record DAT_00691608[n] by 0x004770c0(rec, 0x80c, cap, tex), DAT_00691604[n] = 0x00474db0(cap, 0x50), '
                 'DAT_00691610[n] = cap, n + 1. Called twice by 0x0040bbb0 ("scorch" 0x18, "wfall" 0x10)', 3, 0x0040bbb0),
    0x00475d90: (None, 'matched', 'Orig_00475d90',
                 T + 'Match/PortTop_00475d90.cpp = re/match/s12/00475d90.c (BYTE-IDENTICAL 184 B, relocheck OK 18/18; '
                 'C++ code typedef under #ifdef __cplusplus, the 004768c0.c form): 0x004768c0 on the DAT_00691600 records '
                 'and RwGlobals +0x10c on DAT_00691604[i] and the four arrays. Records built by 0x00475a00, arrays by '
                 '0x004759b0, both from 0x0040bbb0 on this path', 3, 0x0040bd00),
    0x004759b0: (None, 'matched', 'Orig_004759b0',
                 'PORT-OT-RENDER: Match/PortTop_004759b0.cpp = re/match/s12/004759b0.c: DAT_00691600 = n, four 0x00474db0 '
                 'arrays at 0x00691604 / 08 / 10 / 14. ORIGTOP-ONLY CROSS-TARGET vs .asi-only Match/MatchS12_004759b0.cpp. '
                 + T + 'its teardown 0x00475d90 now runs on the matched body (records from the 0x00475a00 near-match body)',
                 3, 0x0040bbb0),
    0x00490000: (None, 'stand-in', 'PortTop_StandIn04C_00490000',
                 'PORT-TOP-04C: Match/PortTop_StandIns04C.cpp (logs, does nothing): matched, not compiled (level 2). ' + T +
                 'kept: 0x0086a4ac (+0x0c of the 30 0x50-byte slots at 0x0086a4a0, BSS) is written only by the callback '
                 '0x0048fee0 (mov [ecx+0xc], eax at 0x0048fee8), which 0x0048fef0 passes to 0x004e89a0 for the slot built by '
                 '0x0048ff20 (a stand-in). That chain cannot run on matched bodies: 0x0048fed0 / 0x0048fee0 have no source '
                 'and no hooks.csv row, 0x004e89a0 is 76.7% (not the L-115 class) and unbound, 0x0048fef0 reads RW 3.x '
                 'atomic +0x18 while the RpClumpForAllAtomics binding hands callbacks the librw atomic (geometry at +0x14), '
                 'and the file callees 0x004b65a0 / 0x004b65b0 / 0x004b3b70 have no knob-path binding. So *0x0086a4ac is 0 '
                 'and 0x004e8090 reads *0', 3, 0x0040cfd0),
    0x0048ff20: (None, 'stand-in', 'PortTop_StandIn04G_0048ff20',
                 'PORT-TOP-04G: stand-in in Match/PortTop_0040bbb0.cpp (logs its first call, does nothing): file-backed '
                 'table loader. ' + T + 'kept (matched 100%, but its record build needs the unsourced callbacks 0x0048fed0 '
                 '/ 0x0048fee0, an RW-layout atomic view for 0x0048fef0 and bindings for 0x004e89a0 / 0x004b65a0 / '
                 '0x004b65b0 / 0x004b3b70: the 0x00490000 row)', 3, 0x0040bbb0),
    0x0041a980: (None, 'matched', 'Orig_0041a980',
                 'PORT-OT-RENDER: Match/PortTop_0041a980.cpp = re/match/s03/raw_0041a980.c (preamble now MATCH-S02, '
                 'BYTE-IDENTICAL): RpClumpDestroy on 0x0063c7b8[0..1], then zero them. Runs only when 0x0041f320(i) != 0. '
                 + T + 'not run on this path: 0x0063d830[0..3] (BSS) gets a non-zero value only at 0x00420839 in 0x00420420 '
                 '(79.8%, C2), called only by 0x0040d110 <- 0x0040d270 <- 0x0040d440 <- 0x004929d0, which the knob path '
                 'binds to the hand GameModeMachine_Tick. 0x0041ec00 stores 0 there (0x0041eca6). No exe TU writes it',
                 3, 0x0040cf80),
}


def load(name):
    spec = importlib.util.spec_from_file_location(name, os.path.join(HERE, name + '.py'))
    m = importlib.util.module_from_spec(spec)
    spec.loader.exec_module(m)
    return m


def apply(table):
    lines = open(TSV, encoding='utf-8').read().splitlines()
    head, rows = lines[0], [ln.split('\t') for ln in lines[1:] if ln.strip()]
    idx = {int(r[0], 16): r for r in rows}
    for rva, (name, kind, sym, note, lvl, caller) in table.items():
        r = idx.get(rva)
        if r is None:
            r = ['0x%08x' % rva, name or 'FUN_%08x' % rva, '', '', '', '', '']
            rows.append(r)
            idx[rva] = r
        if name:
            r[1] = name
        r[2], r[3], r[4], r[5], r[6] = kind, sym, note, str(lvl), '0x%08x' % caller
    open(TSV, 'w', encoding='utf-8', newline='\n').write('\n'.join([head] + ['\t'.join(r) for r in rows]) + '\n')
    return rows


def main():
    check = '--check-base' in sys.argv
    before = open(TSV, 'rb').read()
    subprocess.check_call([sys.executable, os.path.join(HERE, 'update_bindings_othud2.py')])
    apply(load('update_bindings_otother2').FIXED)
    rows = apply(load('update_bindings_otpickuppool').FIXED)
    if check:
        # the checkout is CRLF under core.autocrlf, the scripts write LF (= the git blob)
        same = open(TSV, 'rb').read() == before.replace(b'\r\n', b'\n')
        open(TSV, 'wb').write(before)
        print('base chain reproduces BINDINGS.tsv byte for byte:', same)
        sys.exit(0 if same else 1)
    rows = apply(FIXED)
    print(len(rows), dict(Counter(r[2] for r in rows)))


if __name__ == '__main__':
    main()
