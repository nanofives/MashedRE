"""fpu_score.py <probe.json> [...]  -- score fpu_probe.py output against the two float models (PREREG_FPU.md).

For every record 0..29 of every sample the before-state is pushed through re/match/s08/004325c0.c's switch twice:
model F (each float op rounded to f32 = SSE2, = x87 at PC_24) and model D (x87 at PC_53: products and the f8 sum held in
double, f8 stored to f32). A record is DISCRIMINATING when F and D give different (state, f8, cnt). Each observed run
(original at its own CW, original under forced PC_53, twin at own CW, twin under PC_53) is counted per record as
=F, =D, both (non-discriminating and equal), or neither.
"""
import json, math, struct, sys
from collections import Counter

GROUP = {0xff000000, 0xff100000, 0xff110000, 0xff120000, 0xff130000, 0xff230000}


def f32(x):
    return struct.unpack('<f', struct.pack('<f', x))[0]


def bf(u):
    return struct.unpack('<f', struct.pack('<I', u & 0xffffffff))[0]


def fb(x):
    return struct.unpack('<I', struct.pack('<f', x))[0]


def step(rec, dt, speed, model):
    typ, st, f8b, cnt = rec
    def prod(K):
        if model == 'F':
            return math.trunc(f32(f32(dt * speed) * K))
        return math.trunc(dt * speed * K)
    if typ == 0xff040000 or typ in GROUP:
        kp, kn = (1000.0, -2000.0) if typ == 0xff040000 else (500.0, -800.0)
        if st != 0 and st != 2:
            cnt = (cnt + prod(kp)) & 0xffffffff
            cnt = cnt - (1 << 32) if cnt & 0x80000000 else cnt
            if cnt >= 400:
                cnt, st = 0x1ff, 0x1000
        else:
            cnt = cnt + prod(kn)
            if cnt <= 0:
                if st == 2:
                    cnt, st = 0x1ff, 0x1000
                else:
                    cnt, st = 0, 1
        return [typ, st, f8b, cnt], (kp if (rec[1] != 0 and rec[1] != 2) else kn)
    f8 = bf(f8b)
    if model == 'F':
        v = f32(f32(dt * 300.0) + f8)
    else:
        v = f32(dt * 300.0 + f8)
    if v >= 180.0:
        v, st = 180.0, 0x1000
    return [typ, st, fb(v), cnt], 'f8'


def score(samples, runs):
    """samples: list of (b, dt_bits, ph, {run: after})."""
    tot = {r: Counter() for r in runs}
    disc_by_k = Counter()
    resid = []
    for b, dtb, ph, outs in samples:
        dt = bf(dtb)
        speed = 2.0 if ph >= 4 else 1.0
        for k in range(30):
            pf, kk = step(b[k], dt, speed, 'F')
            pd, _ = step(b[k], dt, speed, 'D')
            disc = pf != pd
            if disc:
                disc_by_k[(kk, speed)] += 1
            for r in runs:
                if r not in outs:
                    continue
                o = outs[r][k]
                if disc:
                    c = '=F' if o == pf else ('=D' if o == pd else 'neither')
                else:
                    c = 'both' if o == pf else 'neither'
                tot[r][('disc ' if disc else 'non ') + c] += 1
                if c == 'neither' and len(resid) < 20:
                    resid.append((r, hex(dtb), ph, k, b[k], o, pf, pd))
    return tot, disc_by_k, resid


def main():
    for path in sys.argv[1:]:
        d = json.load(open(path))
        print(f'== {path}  pid={d.get("pid")} asi={d.get("asi")}')
        cw = d.get('cw', [])
        print(f'arm1: {len(cw)} entry rows; CW values {dict(Counter(hex(r[0]) for r in cw))}; '
              f'MXCSR {dict(Counter(hex(r[1]) for r in cw))}; phase {dict(Counter(r[3] for r in cw))}')
        dts = Counter(hex(r[2]) for r in cw)
        print(f'      dt values (top 6) {dts.most_common(6)}')
        nat = d.get('nat', [])
        if nat:
            print(f'arm2: {len(nat)} calls; CW read before the call {dict(Counter(hex(r["cw"]) for r in nat))}; '
                  f'distinct dt {len(set(r["dt"] for r in nat))}')
            tot, disc, resid = score([(r['b'], r['dt'], r['ph'], {'orig': r['a']}) for r in nat], ['orig'])
            print('      orig', dict(tot['orig']), ' discriminating by (K,speed):', dict(disc))
            for x in resid:
                print('      residual', x)
        ctl = d.get('ctl', [])
        if ctl:
            smp = []
            for rnd in ctl:
                for s in rnd['samples']:
                    outs = {k: s[k] for k in ('o_own', 'o_53', 't_own', 't_53') if k in s}
                    smp.append((s['b'], s['dt'], s['ph'], outs))
            cws = Counter((hex(s['cw']), hex(s['cw_after'])) for rnd in ctl for s in rnd['samples'])
            print(f'arm3: {len(ctl)} rounds, {len(smp)} samples; (cw, cw_after) {dict(cws)}')
            runs = ['o_own', 'o_53', 't_own', 't_53']
            tot, disc, resid = score(smp, runs)
            print('      discriminating records by (K,speed):', dict(disc))
            for r in runs:
                if tot[r]:
                    print(f'      {r:6s}', dict(tot[r]))
            for x in resid:
                print('      residual', x)
            # per-dt table for the original at its own CW
            per = Counter()
            for b, dtb, ph, outs in smp:
                dt = bf(dtb); sp = 2.0 if ph >= 4 else 1.0
                for k in range(30):
                    pf, kk = step(b[k], dt, sp, 'F'); pd, _ = step(b[k], dt, sp, 'D')
                    if pf != pd:
                        o = outs['o_own'][k]
                        per[(hex(dtb), sp, kk, '=F' if o == pf else '=D' if o == pd else 'neither')] += 1
            for key in sorted(per, key=str):
                print('      o_own dt/speed/K', key, per[key])


if __name__ == '__main__':
    main()
