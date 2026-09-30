#!/usr/bin/env python3
"""a9_fixup.py -- the per-CONTACT cross-side table for VehicleContactFixup 0x0046ef70,
registered in re/analysis/D2_REOPEN_2026-09-29.md section 22.1 reading 4.

Section 22.1's scan named C4 (the post-bounce horizontal speed) as the first diverging term
at d = 0, and reading 4 points at the last-contact damp inside 0x0046ef70:

    fVar5 = local_74 * 3.0            0x0046f5ba   (kFx_Three,  _DAT_005cc31c)
    if (0.9 < fVar5) fVar5 = 0.9      0x0046f5c0   (kFx_NyThresh)
    vel *= fVar5                      ContactFixup.cpp:288-292

with local_74 = 1 - min(1, |m|/speed) and |m| the slot impulse magnitude at S+0x38. So the
damp has a KNEE: |m|/speed <= 0.7 keeps the capped 90%, above it the retention falls as
3*(1 - |m|/speed) and reaches 0 head-on.

ORIGINAL: scenario_launch.py --fixup-probe, <msd>.fixupprobe.csv. site 0 = the 0x0046ef70
          ENTRY (pre-fixup velocity + the live slot set, which the .msd cannot see), site 1 =
          the substep entry 0x004709a0. The first site-1 row after a site-0 row is the
          POST-fixup velocity: the original's substep body 0x004709a0 runs
          0x0046e9e0 -> 0x0046f6c0 -> 0x00469aa0 -> 0x0046ef70, so nothing writes +0x9b0
          between the fixup and the next substep entry.
PORT:     MASHED_WORLD_CONTACT_LOG (pre->post velocity and the slot set, one line per fixup,
          VehiclePhysicsRun.cpp:960-985) joined by ordinal to MASHED_FIXUP_LOG, which
          carries `sp` = the record +0x9e4 the damp divides by (ContactFixup.cpp:249-252).

Reported per contact, both sides: pre-speed, |m|, |m|/speed, the damp that implies, whether
it is on the 0.9 cap, the post/pre speed retention, and the velocity's own |cos| to the wall
normal before and after.

Usage:
  py -3.12 re/tools/statediff/a9_fixup.py --orig <msd>.fixupprobe.csv
        [--port-dir <dir with world_contact.log + fixup.log>] [--first N]
Read-only. Does not execute the game.
"""
import argparse, csv, math, os, re, sys

CAP = 0.9       # kFx_NyThresh, the damp cap  @0x0046f5c0
THREE = 3.0     # kFx_Three,    _DAT_005cc31c @0x0046f5ba


def damp_of(m, sp):
    if sp <= 0:
        return float('nan'), float('nan')
    ratio = min(1.0, abs(m) / sp)
    d = THREE * (1.0 - ratio)
    return ratio, (CAP if d > CAP else d)


def h3(v):
    return math.sqrt(v[0] * v[0] + v[1] * v[1] + v[2] * v[2])


def cos_to_normal(v, n):
    """|cos| between the velocity and the contact normal -- the quantity |m|/speed is."""
    a = h3(v)
    b = h3(n)
    if a < 1e-9 or b < 1e-9:
        return float('nan')
    return abs((v[0] * n[0] + v[1] * n[1] + v[2] * n[2]) / (a * b))


def load_orig(path):
    rows = list(csv.DictReader(open(path)))
    out = []
    for i, r in enumerate(rows):
        if r['site'] != '0':
            continue
        # The post-fixup velocity is the NEXT substep entry, but only if no A6a (site 2)
        # intervened: A6a runs once per frame before the substep loop and writes +0x9b0, so
        # a site-2 row in between means a frame boundary crossed and the pair is not clean.
        post, clean = None, True
        for q in rows[i + 1:]:
            if q['site'] == '2':
                clean = False
            if q['site'] == '1':
                post = q
                break
        def f(d, k):
            v = d[k]
            return float(v) if v not in ('', 'nan') else float('nan')
        pre = (f(r, 'velx'), f(r, 'vely'), f(r, 'velz'))
        nrm = (f(r, 's0_nx'), f(r, 's0_ny'), f(r, 's0_nz'))
        out.append(dict(seq=int(r['seq']), frame=int(r.get('frame', -1)), clean=clean,
                        sp=f(r, 'speed'),
                        pre=pre,
                        post=((f(post, 'velx'), f(post, 'vely'), f(post, 'velz'))
                              if post else (float('nan'),) * 3),
                        m=f(r, 's0_mag'), nrm=nrm, depth=f(r, 's0_depth'),
                        nslot=int(r['nslot']), idx=r['slotidx'],
                        acc=(f(r, 'a144'), f(r, 'a148'), f(r, 'a14c')),
                        gnd=f(r, 'gnd'), c9ec=int(r['c9ec'])))
    return out


WC = re.compile(r"slot=(\d+) pass=(\d+) n=(\d+) ret=(-?\d+) pos=\(([^)]*)\) "
                r"vel=\(([^)]*)\)->\(([^)]*)\)")
WCS = re.compile(r"\| s(\d+) d=(\S+) n=\((\S+),(\S+),(\S+)\) m=(\S+) "
                 r"arm=\((\S+),(\S+),(\S+)\) sl=(\S+) ayz=(\S+)")
FX = re.compile(r"t6c=\((\S+),(\S+),(\S+)\) acc=\((\S+),(\S+),(\S+)\) cnt=(\d+) sp=(\S+)")


def load_port(d):
    wc, fx = [], []
    for line in open(os.path.join(d, 'world_contact.log'), errors='replace'):
        m = WC.search(line)
        if not m:
            continue
        pre = tuple(float(x) for x in m[6].split(','))
        post = tuple(float(x) for x in m[7].split(','))
        sl = WCS.findall(line)
        wc.append(dict(nslot=int(m[3]), ret=int(m[4]), pre=pre, post=post,
                       idx='|'.join(s[0] for s in sl),
                       m=float(sl[0][5]) if sl else float('nan'),
                       nrm=(tuple(float(sl[0][k]) for k in (2, 3, 4)) if sl
                            else (float('nan'),) * 3),
                       depth=float(sl[0][1]) if sl else float('nan')))
    for line in open(os.path.join(d, 'fixup.log'), errors='replace'):
        m = FX.search(line)
        if m:
            fx.append(dict(t6c=tuple(float(m[k]) for k in (1, 2, 3)),
                           acc=tuple(float(m[k]) for k in (4, 5, 6)),
                           cnt=int(m[7]), sp=float(m[8])))
    out = []
    for i, w in enumerate(wc):
        f = fx[i] if i < len(fx) else None
        out.append(dict(seq=i, frame=-1, clean=True,
                        sp=(f['sp'] if f else float('nan')), pre=w['pre'],
                        post=w['post'], m=w['m'], nrm=w['nrm'], depth=w['depth'],
                        nslot=w['nslot'], idx=w['idx'],
                        acc=(f['acc'] if f else (float('nan'),) * 3),
                        gnd=float('nan'), c9ec=w['nslot']))
    # sanity: world_contact.log and fixup.log must agree on the contact count per fixup
    bad = sum(1 for i, w in enumerate(wc) if i < len(fx) and fx[i]['cnt'] != w['nslot'])
    return out, ('world_contact/fixup join: %d rows each side, cnt disagrees on %d'
                 % (min(len(wc), len(fx)), bad))


def report(label, rows, first):
    print('=== %s: %d fixup calls ===' % (label, len(rows)))
    print('  #   seq   frm  sp/|v|   pre_sp   pre_h    |m|   |m|/sp   damp  cap?  post_h  '
          'keep   |cos|v,n| pre->post  nslot idx')
    for k, r in enumerate(rows[:first]):
        ratio, d = damp_of(r['m'], r['sp'])
        preh = math.hypot(r['pre'][0], r['pre'][2])
        posth = math.hypot(r['post'][0], r['post'][2])
        keep = posth / preh if preh > 1e-6 else float('nan')
        c0 = cos_to_normal(r['pre'], r['nrm'])
        c1 = cos_to_normal(r['post'], r['nrm'])
        p3 = h3(r['pre'])
        print('  %-3d %5d %5d %7.5f %8.2f %8.2f %7.1f %7.5f %6.4f %4s %8.2f %6.4f%1s  '
              '%6.4f -> %6.4f   %d %s'
              % (k, r['seq'], r['frame'], (r['sp'] / p3 if p3 > 1e-6 else float('nan')),
                 r['sp'], preh, abs(r['m']), ratio, d,
                 'CAP' if d >= CAP - 1e-9 else '-', posth, keep,
                 '' if r['clean'] else '*', c0, c1, r['nslot'], r['idx']))
    n_cap = sum(1 for r in rows if damp_of(r['m'], r['sp'])[1] >= CAP - 1e-9)
    print('  on the 0.9 CAP: %d of %d fixups (%.1f%%)   -- the knee is |m|/speed <= 0.7'
          % (n_cap, len(rows), 100.0 * n_cap / max(1, len(rows))))
    rr = sorted(r['sp'] / h3(r['pre']) for r in rows if h3(r['pre']) > 1e-3)
    if rr:
        print('  +0x9e4 / |velocity| at the fixup entry: median %.6f  min %.6f  max %.6f'
              '   (the original holds this at 1.000000)' % (rr[len(rr) // 2], rr[0], rr[-1]))
    dirty = sum(1 for r in rows[:first] if not r['clean'])
    if dirty:
        print('  * = a frame boundary (A6a 0x00467650) fell between the fixup and the next '
              'substep entry, so that post/keep pair is NOT the fixup output: %d of the %d '
              'rows above' % (dirty, min(first, len(rows))))


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('--orig', required=True)
    ap.add_argument('--port-dir', default=None)
    ap.add_argument('--first', type=int, default=12)
    a = ap.parse_args()
    report('ORIGINAL ' + a.orig, load_orig(a.orig), a.first)
    if a.port_dir:
        print()
        rows, note = load_port(a.port_dir)
        print('  %s' % note)
        report('PORT ' + a.port_dir, rows, a.first)


if __name__ == '__main__':
    main()
