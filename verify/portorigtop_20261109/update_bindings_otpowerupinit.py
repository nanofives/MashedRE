"""PORT-OT-POWERUPINIT (L-111 (a) step 1): BINDINGS.tsv chain fix, and the place for this row's own rows.

update_bindings_otother2.py and update_bindings_otpickuppool.py both run update_bindings_othud2.py first, so they are
siblings, not a chain: running either one alone drops the other's rows. This updater runs the HUD-2 tip once, then
applies the OTHER-2 rows, then the PICKUPPOOL rows (both imported from their scripts, not re-run, so the HUD-2 tip runs
exactly once), then this row's FIXED rows.

--check-chain: stop after the three inherited steps, so `git diff verify/porttop_20261108/BINDINGS.tsv` shows whether
they reproduce the committed file byte for byte.

usage: py -3.12 verify/portorigtop_20261109/update_bindings_otpowerupinit.py [--check-chain]
"""
import importlib.util, os, subprocess, sys
from collections import Counter

HERE = os.path.dirname(os.path.abspath(__file__))
TOP = os.path.join(os.path.dirname(HERE), 'porttop_20261108')
TSV = os.path.join(TOP, 'BINDINGS.tsv')
T = 'PORT-OT-POWERUPINIT: '

FIXED = {  # rva -> (name or None, kind, symbol, note, level, caller). Empty: this row swapped nothing (scope report).
}


def _fixed_of(script):
    spec = importlib.util.spec_from_file_location(script[:-3], os.path.join(HERE, script))
    mod = importlib.util.module_from_spec(spec)
    spec.loader.exec_module(mod)
    return mod.FIXED


def _apply(fixed):
    lines = open(TSV, encoding='utf-8').read().splitlines()
    head, rows = lines[0], [ln.split('\t') for ln in lines[1:] if ln.strip()]
    idx = {int(r[0], 16): r for r in rows}
    for rva, (name, kind, sym, note, lvl, caller) in fixed.items():
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
    subprocess.check_call([sys.executable, os.path.join(HERE, 'update_bindings_othud2.py')])
    _apply(_fixed_of('update_bindings_otother2.py'))
    rows = _apply(_fixed_of('update_bindings_otpickuppool.py'))
    if '--check-chain' not in sys.argv[1:]:
        rows = _apply(FIXED)
    print(len(rows), dict(Counter(r[2] for r in rows)))


if __name__ == '__main__':
    main()
