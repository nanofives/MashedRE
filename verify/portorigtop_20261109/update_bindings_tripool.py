"""PORT-OT-TRIPOOL (L-111 step 1): BINDINGS.tsv rows for the Tri.dff slot table 0x0086a4a0: 0x0048ff20 (builder),
0x0048ffd0 (teardown) and 0x00490000 (texture clear) on their matched bodies, the two matched callbacks 0x0048fed0 /
0x0048fee0, 0x0048fef0 with its RW-layout atomic view, and RpGeometryForAllMaterials 0x004e89a0 bound to librw.

Runs update_bindings_powerupinit_a.py first, then the FIXED table of update_bindings_otrender2.py, then this row's FIXED
rows (the chain of orch/integration 05ea6cb1e). --check-base runs only the base chain and compares it with the committed BINDINGS.tsv (LF-normalised).

usage: py -3.12 verify/portorigtop_20261109/update_bindings_tripool.py [--check-base]
"""
import importlib.util, os, subprocess, sys
from collections import Counter

HERE = os.path.dirname(os.path.abspath(__file__))
TOP = os.path.join(os.path.dirname(HERE), 'porttop_20261108')
TSV = os.path.join(TOP, 'BINDINGS.tsv')
T = 'PORT-OT-TRIPOOL: '
EV = ' (verify/portorigtop_20261109/RESULT_PORT-OT-TRIPOOL.md)'
B = 0x0048ff20

FIXED = {  # rva -> (name or None, kind, symbol, note, caller); level = caller's level + 1
    0x0048ff20: (None, 'matched', 'Orig_0048ff20 (via PortTop_TriPool_Call0048ff20)',
                 T + 'Match/PortTop_0048ff20.cpp = re/match/s15/0048ff20.c ("Tri.dff" a copy under MATCH_PORT): 30 slots '
                 'at 0x0086a4a0, +0x04 clump (0x004b3e40), +0x08 atomic (0x0048fed0), +0x0c material (0x0048fee0 under '
                 '0x0048fef0). The forwarder logs the slot counts. One owner: this body creates the 30 clumps, 0x0048ffd0 '
                 'destroys them' + EV, 0x0040bbb0),
    0x0048fed0: (None, 'matched', 'Orig_0048fed0', T + 'Match/PortTop_0048fed0.cpp = re/match/s15/0048fed0.c (new, '
                 'BYTE-IDENTICAL 12 B): slot +0x08 = the librw atomic RpClumpForAllAtomics hands over' + EV, B),
    0x0048fef0: (None, 'matched', 'Orig_0048fef0 (via PortTop_TriPool_Call0048fef0)',
                 T + 'Match/PortTop_0048fef0.cpp = re/match/s15/0048fef0.c, given an RW-layout view of the slot atomic '
                 '(MashedRw_AtomicView_0048fef0, LibRw/RwApiBind.cpp MashedRwAtomicView, geometry view +0x08 = flags): '
                 'RW 3.x reads the geometry at atomic +0x18, librw has it at +0x14' + EV, B),
    0x0048fee0: (None, 'matched', 'Orig_0048fee0', T + 'Match/PortTop_0048fee0.cpp = re/match/s15/0048fee0.c (new, '
                 'BYTE-IDENTICAL 12 B): slot +0x0c = the librw material (texture at +0x00 as in RW 3.x), the only writer '
                 'of 0x0086a4ac..' + EV, 0x0048fef0),
    0x004e89a0: ('RpGeometryForAllMaterials', 'rw', 'MashedRw_GeometryForAllMaterials_004e89a0',
                 T + 'LibRw/RwApiBind.cpp: cb(material, data) over Geometry::matList in order, stop on 0, returns the '
                 'pointer given. A geometry view of MashedRwAtomicView is resolved to its librw geometry and its +0x08 flags '
                 'written back first (0x0048fef0 ORs 0x40 into the view). The matched source re/match/rwl02/004e89a0.c is '
                 '76.7%, not the L-115 class' + EV, 0x0048fef0),
    0x0048ffd0: (None, 'matched', 'Orig_0048ffd0 (via PortTop_TriPool_Call0048ffd0)',
                 T + 'Match/PortTop_0048ffd0.cpp = re/match/s15/0048ffd0.c: RpMaterialSetTexture(slot +0x0c, 0) '
                 '(Orig_004e8090) and RpClumpDestroy(slot +0x04) x30. The forwarder runs it when all 30 slots hold a clump '
                 'and a material, logs either way' + EV, 0x0040bd00),
    0x00490000: (None, 'matched', 'Orig_00490000 (via PortTop_TriPool_Call00490000)',
                 T + 'Match/PortTop_00490000.cpp = re/match/s15/00490000.c: RpMaterialSetTexture(slot +0x0c, 0) x30, same '
                 'guard as 0x0048ffd0. 0x0040cfd0 runs in no check of this row, so it is compiled and bound, not run' + EV,
                 0x0040cfd0),
}


def main():
    subprocess.check_call([sys.executable, os.path.join(HERE, 'update_bindings_powerupinit_a.py')])
    # then the FIXED table of update_bindings_otrender2.py (its main is not called: it would re-run othud2 and drop
    # the POWERUPINIT-A rows). This is the chain of orch/integration 05ea6cb1e, where both rows merged.
    spec = importlib.util.spec_from_file_location('otrender2', os.path.join(HERE, 'update_bindings_otrender2.py'))
    r2 = importlib.util.module_from_spec(spec)
    spec.loader.exec_module(r2)
    r2.apply(r2.FIXED)
    lines = open(TSV, encoding='utf-8').read().splitlines()
    if '--check-base' in sys.argv:
        blob = subprocess.check_output(['git', 'show', 'HEAD:verify/porttop_20261108/BINDINGS.tsv'], cwd=HERE)
        same = blob.decode('utf-8').replace('\r\n', '\n').splitlines() == lines
        print('base chain reproduces BINDINGS.tsv byte for byte:', same)
        return
    head, rows = lines[0], [ln.split('\t') for ln in lines[1:] if ln.strip()]
    idx = {int(r[0], 16): r for r in rows}
    for rva, (name, kind, sym, note, caller) in FIXED.items():
        r = idx.get(rva)
        if r is None:
            r = ['0x%08x' % rva, name or 'FUN_%08x' % rva, '', '', '', '', '']
            rows.append(r)
            idx[rva] = r
        if name:
            r[1] = name
        lvl = int(idx[caller][5]) + 1 if caller in idx and idx[caller][5].strip().isdigit() else (int(r[5]) if r[5].strip().isdigit() else 3)
        r[2], r[3], r[4], r[5], r[6] = kind, sym, note, str(lvl), '0x%08x' % caller
    open(TSV, 'w', encoding='utf-8', newline='\n').write('\n'.join([head] + ['\t'.join(r) for r in rows]) + '\n')
    print(len(rows), dict(Counter(r[2] for r in rows)))


if __name__ == '__main__':
    main()
