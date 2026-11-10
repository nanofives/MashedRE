"""Gate one closed-set batch switch ON and OFF, and bisect it when ON moves a gate (PORT-BATCH-TOOL, D4).

Run it inside the test slot and the machine lock (test-slot OUTSIDE, mashed_lock INSIDE):
  test-slot bash -c 'py -3.12 <orch-wt>/re/orchestrator/mashed_lock.py run --label PORT-BATCH-TOOL --ttl 14400 -- \
      py -3.12 re/tools/port_batch_bisect.py --tag HUD --heads 0x0041d870,0x0041e850'

One gate pass = verify/savelink_20261023/run_arms.py (H_<stack>, B_<stack>_K0 / K1 / K26) and the 12 nav-demo
captures (verify/porttop_20261108/knob_boot.py, MASHED_NAV_DEMO=1 MASHED_DETERMINISTIC=1 MASHED_DET_MPG_PIN=0, the PORT-TOP-04E settings
baseline restored first). The OFF pass uses the DEF stack; an ON pass uses the BATCH stack with
A4_BATCH_KNOBS='MASHED_BATCH_<TAG>=1 [MASHED_BATCH_<TAG>_ROWS=<heads>]'. Hashes: G-CTRL = sha8 of the H arm's
step.csv / gates.csv / ptrace.log, B = sha8 of each B arm's step.csv (the merge train's definition,
re/orchestrator/merge_train.json), captures = sha256/16 of walk_NN_*.bmp (the cmp_caps.py list of PORT-TOP-04E..G).

Order: OFF, ON, OFF, ON (all heads). A side whose two passes differ is UNSTABLE and nothing is bisected. If both
sides repeat and ON != OFF anywhere, the heads are split in halves and each half that still moves is
split again, down to single heads (a half that moves only together with the other is reported as one unit). The
responsible heads are then excluded and the rest is gated ON again. Every pass goes to <data>/<label>/ and one line of
<data>/passes.tsv; the final table is written to verify/portbatch_20261109/bisect_<TAG>.tsv.

Options: --data <dir> (default D:/MashedWT/PORT-BATCH-TOOL_data), --root <gate root>, --rootk <nav root>,
--no-nav, --no-off (reuse nothing: OFF is not run, ON is compared with the refs only), --label-prefix <p>.
"""
from __future__ import annotations

import hashlib
import json
import os
import pathlib
import shutil
import subprocess
import sys
import time

ROOT = pathlib.Path(__file__).resolve().parents[2]
NAV_REF = dict(walk_01='83c8cbb913899494', walk_02='24249b644e3e8057', walk_03='feb2c803554cd028',
               walk_04='2871487f1fcef51c', walk_05='9f02cb91d09e0500', walk_06='b1f095c07072b28d',
               walk_07='3465159db8526481', walk_08='843e3fc49d166754', walk_09='5db0164fb2c6df8c',
               walk_10='ca2f64dc5e7a9407', walk_11='536f710d21e50619', walk_12='6d41bc3ac8ad8d8a')
SETTINGS = 'D:/MashedWT/PORT-TOP-04E_data/settings_baseline.bin'


def opt(argv, k, d=None):
    return argv[argv.index(k) + 1] if k in argv else d


def sha8(p):
    return hashlib.sha256(open(p, 'rb').read()).hexdigest()[:8] if os.path.exists(p) else 'MISSING'


class Gate:
    def __init__(self, argv):
        self.tag = opt(argv, '--tag')
        self.data = pathlib.Path(opt(argv, '--data', 'D:/MashedWT/PORT-BATCH-TOOL_data'))
        self.root = opt(argv, '--root', 'D:/MashedWT/PORT-BATCH-TOOL_root')
        self.rootk = opt(argv, '--rootk', 'D:/MashedWT/PORT-BATCH-TOOL_rootK')
        self.nav = '--no-nav' not in argv
        self.prefix = opt(argv, '--label-prefix', '')
        mt = json.load(open(ROOT / 're' / 'orchestrator' / 'merge_train.json'))
        self.ref = dict(gctrl=mt['gctrl']['ref'], b=mt['b']['ref'], nav=[NAV_REF[k] for k in sorted(NAV_REF)])
        self.data.mkdir(parents=True, exist_ok=True)
        self.n = 0

    def knobs(self, heads):
        if heads is None:
            return []
        k = ['MASHED_BATCH_%s=1' % self.tag]
        if heads != 'all':
            k.append('MASHED_BATCH_%s_ROWS=%s' % (self.tag, ','.join('0x%08x' % h for h in heads)))
        return k

    def run(self, label, heads):
        """heads: None = OFF, 'all' = every head, list = those heads. -> {gctrl:[3], b:[3], nav:[12]}"""
        self.n += 1
        label = '%s%02d_%s' % (self.prefix, self.n, label)
        out = self.data / label
        (out / 'arms').mkdir(parents=True, exist_ok=True)
        kn = self.knobs(heads)
        stack = 'BATCH' if kn else 'DEF'
        env = dict(os.environ, A4_ROOT=self.root, A4_OUT=str(out / 'arms'), A4_BATCH_KNOBS=' '.join(kn),
                   PYTHONWARNINGS='ignore')
        arms = ['H_%s' % stack] + ['B_%s_K%s' % (stack, k) for k in ('0', '1', '26')]
        t0 = time.time()
        print('--- pass %s knobs=%s' % (label, kn or 'none'), flush=True)
        rc = subprocess.call([sys.executable, 'verify/savelink_20261023/run_arms.py'] + arms, cwd=ROOT, env=env)
        a = out / 'arms'
        res = dict(label=label, knobs=' '.join(kn), rc=rc,
                   gctrl=[sha8(a / ('H_%s.%s' % (stack, x))) for x in ('step.csv', 'gates.csv', 'ptrace.log')],
                   b=[sha8(a / ('B_%s_K%s.step.csv' % (stack, k))) for k in ('0', '1', '26')], nav=[])
        if self.nav:
            nav = out / 'nav'
            nav.mkdir(exist_ok=True)
            shutil.copyfile(SETTINGS, os.path.join(self.rootk, 'mashed_re_settings.bin'))
            e2 = dict(os.environ, PORTTOP_OUT=str(out), PYTHONWARNINGS='ignore')
            subprocess.call([sys.executable, 'verify/porttop_20261108/knob_boot.py', 'navdemo', self.rootk, '300'] + kn +
                            ['MASHED_NAV_DEMO=1', 'MASHED_DETERMINISTIC=1', 'MASHED_DET_MPG_PIN=0',   # HB-CAPTURE-MPGPIN
                             'MASHED_VERIFY_OUT=' + str(nav)],
                            cwd=ROOT, env=e2)
            for k in sorted(NAV_REF):
                f = [x for x in os.listdir(nav) if x.startswith(k + '_') and x.endswith('.bmp')]
                res['nav'].append(hashlib.sha256(open(nav / f[0], 'rb').read()).hexdigest()[:16] if f else 'MISSING')
        res['secs'] = int(time.time() - t0)
        with open(self.data / 'passes.tsv', 'a') as fh:
            fh.write('\t'.join([label, res['knobs'] or '-', '/'.join(res['gctrl']), '/'.join(res['b']),
                                ' '.join(res['nav']) or '-', str(res['secs'])]) + '\n')
        json.dump(res, open(out / 'pass.json', 'w'), indent=1)
        print('    G-CTRL %s  B %s  nav %s' % ('/'.join(res['gctrl']), '/'.join(res['b']),
                                             '%d/12 ref' % sum(x == y for x, y in zip(res['nav'], self.ref['nav']))
                                             if res['nav'] else '-'), flush=True)
        return res

    def key(self, r):
        return (tuple(r['gctrl']), tuple(r['b']), tuple(r['nav']))

    def moved(self, r, base):
        return self.key(r) != self.key(base)


def bisect(g, heads, base, on_res):
    """-> list of responsible units (each a list of heads)."""
    if len(heads) == 1:
        return [heads]
    h1, h2 = heads[:len(heads) // 2], heads[len(heads) // 2:]
    r1 = g.run('bis_%s' % '_'.join('%x' % h for h in h1)[:60], h1)
    r2 = g.run('bis_%s' % '_'.join('%x' % h for h in h2)[:60], h2)
    out = []
    if g.moved(r1, base):
        out += bisect(g, h1, base, r1)
    if g.moved(r2, base):
        out += bisect(g, h2, base, r2)
    if not out:
        out = [heads]       # moves only when both halves are on: one unit
    return out


def main(argv):
    g = Gate(argv)
    heads = [int(x, 16) for x in opt(argv, '--heads').replace(' ', ',').split(',') if x]
    table = []
    off = off2 = on2 = None
    verdict = ''
    # OFF, ON, OFF, ON (the PORT-TOP-04G capture order): a difference must repeat on both sides before it is
    # bisected. Run 1 of this step found 11 of 12 OFF captures off the 04E list on the first nav run in a fresh
    # root copy while ON matched 12/12; one OFF pass alone would have sent that to the bisect.
    if '--no-off' not in argv:
        off = g.run('off', None)
        table.append(('OFF', '-', off))
    on = g.run('on_all', 'all')
    table.append(('ON', 'all', on))
    if '--no-off' not in argv:
        off2 = g.run('off2', None)
        table.append(('OFF repeat', '-', off2))
        on2 = g.run('on_all2', 'all')
        table.append(('ON repeat', 'all', on2))
    base = off2 or dict(gctrl=g.ref['gctrl'], b=g.ref['b'], nav=g.ref['nav'] if g.nav else [])
    responsible = []
    stable = True
    if off2 is not None:
        if g.moved(off, off2) or g.moved(on, on2):
            stable = False
            verdict = 'UNSTABLE: a side does not repeat (OFF %s, ON %s); not bisected' % (
                'repeats' if not g.moved(off, off2) else 'differs', 'repeats' if not g.moved(on, on2) else 'differs')
    if stable and g.moved(on, base):
        responsible = bisect(g, heads, base, on)
        keep = [h for h in heads if not any(h in u for u in responsible)]
        if keep:
            again = g.run('on_excl', keep)
            table.append(('ON excluding %s' % ' '.join('0x%08x' % h for u in responsible for h in u),
                          ' '.join('0x%08x' % h for h in keep), again))
    ref = dict(gctrl=g.ref['gctrl'], b=g.ref['b'], nav=g.ref['nav'])
    lines = ['pass\theads\tG-CTRL\tB\tnav_vs_04E\tG-CTRL_eq_ref\tB_eq_ref\tlabel']
    for name, hs, r in table:
        nav = ('%d/12' % sum(x == y for x, y in zip(r['nav'], ref['nav']))) if r['nav'] else '-'
        lines.append('\t'.join([name, hs, '/'.join(r['gctrl']), '/'.join(r['b']), nav,
                                str(r['gctrl'] == ref['gctrl']), str(r['b'] == ref['b']), r['label']]))
    lines.append('responsible\t%s' % (' | '.join(' '.join('0x%08x' % h for h in u) for u in responsible) or 'none'))
    if verdict:
        lines.append('verdict\t' + verdict)
    txt = '\n'.join(lines) + '\n'
    print(txt)
    dst = ROOT / 'verify' / 'portbatch_20261109' / ('bisect_%s.tsv' % g.tag)
    dst.parent.mkdir(parents=True, exist_ok=True)
    dst.write_text(txt)
    (g.data / ('bisect_%s.tsv' % g.tag)).write_text(txt)
    return 0


if __name__ == '__main__':
    sys.exit(main(sys.argv[1:]))
