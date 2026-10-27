r"""D4-FX-REBASE: fA (verify/fxslot2_20261027, the car-1-survivor rep) car 1 FX calls after its first round, against the
FUN_0040ca00 rec+0xcfc != 0 branch (0x0040cabe jne 0x0040cb42: no speed gate, skid iff roll >= 0.3 [0x005cc99c] at
0x0040cb4e, three FUN_0040c690 calls per skid frame). Expected gated share of frames on that branch: 1 - 0.3 = 0.7.
usage: py -3.12 fa_cfc_check.py <fA.json> <t_from>"""
import json, sys
rows = json.load(open(sys.argv[1]))['rows']; t0 = int(sys.argv[2])
a = next(r for r in rows if r['t'] == t0); b = rows[-1]
for c in range(4):
    dn = b['calls'][2 * c] - a['calls'][2 * c]; ds = b['calls'][2 * c + 1] - a['calls'][2 * c + 1]; fr = b['frames'] - a['frames']
    print('car %d t=%d..%d frames=%d non-skid calls=%d skid calls=%d skid frames=%d share=%.4f' % (c, t0, b['t'], fr, dn, ds, ds // 3, (ds / 3) / fr))
