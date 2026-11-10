"""PORT-OT-PICKUPPOOL (L-111 (a) step 1): BINDINGS.tsv rows for the two pickup-family teardowns of 0x0045b930 swapped onto
matched bodies on the MASHED_ORIG_TOP path (0x00458880, the pickup pool 0x0068b198, made single-owner first, and
0x004593b0, run only with MASHED_ORIG_TOP_RWMOD=1), the exe-only matched callee 0x004c5770, the record selector the two
use for 0x004768c0, and the kept 0x00412010 with its reason. Runs update_bindings_othud2.py first (the chain tip on
orch/integration 7e6eeff6c: it runs the GAMEPLAY, VEHICLE and OTHER tips, each of which runs the rest), then writes the
rows below. Idempotent.
usage: py -3.12 verify/portorigtop_20261109/update_bindings_otpickuppool.py
"""
import os, subprocess, sys
from collections import Counter

HERE = os.path.dirname(os.path.abspath(__file__))
TOP = os.path.join(os.path.dirname(HERE), 'porttop_20261108')
TSV = os.path.join(TOP, 'BINDINGS.tsv')
T = 'PORT-OT-PICKUPPOOL: '
SEL = ('0x004768c0 through the record selector PortTop_PKP_Call004768c0 (Match/PortTop_00458880.cpp): Orig_004768c0 when '
       'the record +0x04 word is non-zero, else logged and skipped')

FIXED = {  # rva -> (name or None, kind, symbol, note, level, caller)
    0x00458880: (None, 'matched', 'Orig_00458880 (via PortTop_0045b930_Call00458880)',
                 T + 'Match/PortTop_00458880.cpp = re/match/s09/00458880.c (BYTE-IDENTICAL 55 B, relocheck OK 7/7): '
                 'RpClumpDestroy on the 25 pool records at 0x0068b198, ' + SEL + ' (record 0x0068b968, built only by '
                 '0x004587a0 under the stand-in 0x0045bae0), RwTexDictionaryDestroy(DAT_0068b9ac). The call-site forwarder '
                 'in Match/PortTop_0045b930.cpp first runs PickupsOrig_ReleasePool (Gameplay/PickupTick.cpp): the hand '
                 'pickup code zeroes the +0x00 words that hold its own fake objects, so the pool has one owner at a time',
                 3, 0x0045b930),
    0x004593b0: (None, 'matched', 'Orig_004593b0 (via PortTop_0045b930_Call004593b0)',
                 T + 'Match/PortTop_004593b0.cpp = re/match/s09/004593b0.c (BYTE-IDENTICAL 68 B, relocheck OK 7/7): '
                 'RpClumpDestroy(rec[7]) and 0x004c5770(rec[0]) on the four records 0x0068ba30..0x0068bb90, ' + SEL +
                 ' (record 0x0068b9b8, built only by 0x00459290 under the stand-in 0x0045bae0), RwTexDictionaryDestroy '
                 '(DAT_0068bb58) when non-zero. Run only with MASHED_ORIG_TOP_RWMOD=1 (RwGlobals +0x11c), selector '
                 'PortTop_0045b930_Call004593b0, the stand-in PortTop_StandIn04C_004593b0 otherwise', 3, 0x0045b930),
    0x004c5770: (None, 'matched', 'Orig_004c5770',
                 T + 'Match/PortTop_004c5770.cpp = re/match/s19/004c5770.c (BYTE-IDENTICAL 35 B, relocheck OK 2/2): '
                 'RwGlobals +0x11c (RwGlobalsFreeListFree with RWMOD) on *(RwGlobals + [0x007d4028]) and param_1, returns 1. '
                 'Reached only from the RWMOD arm of 0x004593b0', 4, 0x004593b0),
    0x004768c0: (None, 'matched', 'Orig_004768c0',
                 'PORT-OT-HUD: Match/PortTop_004768c0.cpp = re/match/s12/004768c0.c (BYTE-IDENTICAL 211 B) from 0x00413bb0 '
                 '(record 0x0063bd50). PORT-OT-HUD-2: also from 0x004210f0 (record 0x0063e548, built by the matched '
                 '0x004210b0); the stand-in PortTop_StandIn04C_004768c0 is removed. ' + T + 'also from 0x00458880 / '
                 '0x004593b0 through the record selector PortTop_PKP_Call004768c0, which skips it for an unbuilt record '
                 '(0x0068b968 / 0x0068b9b8 on this path)', 4, 0x004210f0),
    0x00412010: (None, 'stand-in', 'PortTop_StandIn04C_00412010',
                 'PORT-TOP-04C: Match/PortTop_StandIns04C.cpp (logs, does nothing): matched; reads DAT_0063bb38[i] +0x3c '
                 'with no null check (state of 0x00402750). ' + T + 'kept: the six records at 0x0063bb38 (BSS) are written '
                 'only by 0x00411f30 (caller 0x0045bae0, a stand-in), 0x00412100 (caller 0x0040bde0) and 0x00412620 (caller '
                 '0x0040ce00), none on this path, and no exe code writes them, so the first read is at 0x0000003c', 3, 0x0045b930),
}


def main():
    subprocess.check_call([sys.executable, os.path.join(HERE, 'update_bindings_othud2.py')])
    lines = open(TSV, encoding='utf-8').read().splitlines()
    head, rows = lines[0], [ln.split('\t') for ln in lines[1:] if ln.strip()]
    idx = {int(r[0], 16): r for r in rows}
    for rva, (name, kind, sym, note, lvl, caller) in FIXED.items():
        r = idx.get(rva)
        if r is None:
            r = ['0x%08x' % rva, name or 'FUN_%08x' % rva, '', '', '', '', '']
            rows.append(r)
            idx[rva] = r
        if name:
            r[1] = name
        r[2], r[3], r[4], r[5], r[6] = kind, sym, note, str(lvl), '0x%08x' % caller
    open(TSV, 'w', encoding='utf-8', newline='\n').write('\n'.join([head] + ['\t'.join(r) for r in rows]) + '\n')
    print(len(rows), dict(Counter(r[2] for r in rows)))


if __name__ == '__main__':
    main()
