"""PORT-TOP-04D: BINDINGS.tsv rows for 0x004924f0, its callees, the RwGlobals record and the two RW-free callers that run
again. Runs update_bindings_top04c.py first (it rebuilds the AppDestroy tree from gen_top04c_tus.py). Idempotent.
usage: py -3.12 verify/porttop_20261108/update_bindings_top04d.py
"""
import os, subprocess, sys
from collections import Counter

HERE = os.path.dirname(os.path.abspath(__file__))
TSV = os.path.join(HERE, 'BINDINGS.tsv')
T = 'PORT-TOP-04D: '
W = 0x00492370

ROWS = {  # rva -> (name or None, kind, symbol, note, level, caller)
    0x004924f0: ('DataZeroFill', 'matched', 'DataZeroFill', T + 'Boot/SubsystemInit.cpp under MASHED_STANDALONE = re/match/topm01/004924f0.c (464 B byte-identical); the .asi keeps the raw passthrough', 1, W),
    0x00431ae0: (None, 'matched', 'DefaultParam_SetField04', T + 'Match/MatchS07_00431ae0.cpp (exe body already)', 2, 0x004924f0),
    0x00431af0: (None, 'matched', 'DefaultParam_SetField08', T + 'Match/MatchS07_00431af0.cpp (exe body already)', 2, 0x004924f0),
    0x00431b00: (None, 'matched', 'DefaultParam_SetField00', T + 'Match/MatchS07_00431b00.cpp (exe body already)', 2, 0x004924f0),
    0x00431b10: (None, 'matched', 'BootParamSet2', T + 'Match/MatchS07_00431b10.cpp, now also in the exe list', 2, 0x004924f0),
    0x00431d00: (None, 'matched', 'CarSelectReset', T + 'Match/MatchS07_00431d00.cpp (exe body already)', 2, 0x004924f0),
    0x004114c0: (None, 'matched', 'VtableTeardown_114c0', T + 'Match/MatchS02_004114c0.cpp: runs on the knob path since the RwGlobals record (its +0x10c free goes to librw)', 2, 0x00402a40),
}


def main():
    subprocess.check_call([sys.executable, os.path.join(HERE, 'update_bindings_top04c.py')])
    lines = open(TSV, encoding='utf-8').read().splitlines()
    head, rows = lines[0], [ln.split('\t') for ln in lines[1:] if ln.strip()]
    idx = {int(r[0], 16): r for r in rows}
    for rva, (name, kind, sym, note, lvl, caller) in ROWS.items():
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
