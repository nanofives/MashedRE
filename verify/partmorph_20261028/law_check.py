r"""D5-PARTMORPH G4 / G5 checker (PREREG_D5-PARTMORPH.md section 3).

usage: py -3.12 law_check.py <arm>.pm.csv [--vtx <arm>.pm.csv.vtx --piz <car.piz> --entry <DFF>] [--dormant]

Replays the logged rows of MASHED_DBG_PARTMORPH against the section 1 decode, in float32 steps:
  moved  = clamp(((dy*dy + dx*dx) + dz*dz) * 25, 0, 1), from the logged position and previous position
  prev   = the previous row of the same slot's position (0 before the first)
  state 0: morph = clamp(steer0 * 0.05, -1, 1) * 0.5 + 0.5,  a = FUN_00422470(phase0, 0xff) - 0.5,
           phase1 = moved * 0.0625 + phase0
  state != 0: phase1 = phase0
  phase0 = the previous row's phase1 (one global DAT_0063e4a4 across the four cars)
--vtx: every logged vertex 0 = (v1 - v0) * pos + v0 per component, and (v0, v1) is a vertex pair of the
       DFF's part 0x12 morph targets 0 / 1 to 1e-5 (the un-bake round trip of RwSceneBuild).
--dormant: G5, pass when no row has state 0.
"""
import argparse
import os
import struct
import sys

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from noise_ref import f32, noise_00422470  # noqa: E402


def clamp_moved(d):
    if d >= 1.0:
        return 1.0
    if not (d >= 0.0):
        return 0.0
    return d


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('pm')
    ap.add_argument('--vtx')
    ap.add_argument('--piz')
    ap.add_argument('--entry')
    ap.add_argument('--dormant', action='store_true')
    a = ap.parse_args()
    rows = []
    with open(a.pm) as f:
        hdr = f.readline().strip().split(',')
        for line in f:
            v = line.strip().split(',')
            if len(v) != len(hdr):
                continue
            rows.append({k: (int(x) if k in ('slot', 'state', 'part14') else float(x)) for k, x in zip(hdr, v)})
    n0 = sum(1 for r in rows if r['state'] == 0)
    print(f'rows={len(rows)} state0_rows={n0} slots={sorted(set(r["slot"] for r in rows))}')
    if a.dormant:
        print('G5', 'PASS' if n0 == 0 and rows else 'FAIL')
        return 0 if n0 == 0 and rows else 1
    bad = {}

    def miss(k, i):
        bad.setdefault(k, []).append(i)

    prev = {}
    phase = None
    for i, r in enumerate(rows):
        p = prev.get(r['slot'], (0.0, 0.0, 0.0))
        if (r['prevx'], r['prevy'], r['prevz']) != tuple(f32(x) for x in p):
            miss('prev', i)
        dx, dy, dz = f32(r['px'] - r['prevx']), f32(r['py'] - r['prevy']), f32(r['pz'] - r['prevz'])
        d = f32(f32(f32(dy * dy) + f32(dx * dx)) + f32(dz * dz))
        d = clamp_moved(f32(d * 25.0))
        if d != r['moved']:
            miss('moved', i)
        if phase is not None and r['phase0'] != phase:
            miss('phase_chain', i)
        if r['state'] == 0:
            v = f32(r['steer0'] * f32(0.05))
            if v < 1.0:
                if v < -1.0:
                    v = -1.0
            else:
                v = 1.0
            if f32(f32(v * 0.5) + 0.5) != r['morph']:
                miss('morph', i)
            if f32(noise_00422470(r['phase0']) - 0.5) != r['a']:
                miss('a', i)
            if f32(f32(r['moved'] * 0.0625) + r['phase0']) != r['phase1']:
                miss('phase_step', i)
        elif r['phase1'] != r['phase0']:
            miss('phase_hold', i)
        prev[r['slot']] = (r['px'], r['py'], r['pz'])
        phase = r['phase1']
    for k, v in sorted(bad.items()):
        print(f'  MISMATCH {k}: {len(v)} rows, first {v[:5]}')
    ok = not bad and n0 >= 600
    if a.vtx:
        ok = check_vtx(a) and ok
    print('G4', 'PASS' if ok else 'FAIL')
    return 0 if ok else 1


def check_vtx(a):
    lines = [l.strip().split(',') for l in open(a.vtx) if l.strip()]
    nbad = 0
    for v in lines:
        s, v0, v1, out = float(v[2]), [float(x) for x in v[3:6]], [float(x) for x in v[6:9]], [float(x) for x in v[9:12]]
        exp = [f32(f32(f32(v1[k] - v0[k]) * s) + v0[k]) for k in range(3)]
        nbad += exp != out
    print(f'vtx lines={len(lines)} lerp_mismatch={nbad}')
    ok = nbad == 0 and len(lines) > 0
    if a.piz and a.entry and lines:
        pairs = dff_part12_pairs(a.piz, a.entry)
        v0, v1 = [float(x) for x in lines[0][3:6]], [float(x) for x in lines[0][6:9]]
        best = min(max(max(abs(p[0][k] - v0[k]) for k in range(3)), max(abs(p[1][k] - v1[k]) for k in range(3)))
                   for p in pairs)
        print(f'vtx[0] (v0, v1) nearest DFF part 0x12 pair: max abs diff {best:.3g} over {len(pairs)} pairs')
        ok = ok and best <= 1e-5
    return ok


def dff_part12_pairs(piz, entry):
    sys.path.insert(0, os.path.join(os.path.dirname(os.path.abspath(__file__)), '..', '..', 're', 'tools'))
    import piz_extract
    from pathlib import Path
    data, _v, _a, ents, _m = piz_extract.read_archive(Path(piz))
    d = next(data[o:o + l] for (n, o, l, _i) in ents if n.upper() == entry.upper())

    def rc(o):
        t, s, _ = struct.unpack_from('<III', d, o)
        return t, s, o + 12
    t, sz, p = rc(0)
    end = p + sz
    q = p
    out = []
    while q + 12 <= end:
        t, s, pp = rc(q)
        if t == 0x1A:
            _t, s2, gsp = rc(pp)
            ng = struct.unpack_from('<i', d, gsp)[0]
            g = gsp + s2
            for _gi in range(ng):
                _tg, gsz, gp = rc(g)
                _ts, ss, sp = rc(gp)
                flags, nt, nv, nm = struct.unpack_from('<Iiii', d, sp)
                blob = d[gp:gp + gsz]
                k = blob.find(b'tv_part_id\x00')
                part = struct.unpack_from('<iii', blob, k + 11)[2] if k >= 0 else None
                if part == 0x12 and nm >= 2 and not flags & 0x01000000:
                    r = sp + 16
                    if flags & 0x08:
                        r += nv * 4
                    nuv = (flags >> 16) & 0xff or (2 if flags & 0x80 else (1 if flags & 0x04 else 0))
                    r += nuv * nv * 8 + nt * 8
                    tgt = []
                    for _mt in range(nm):
                        r += 16
                        hv, hn = struct.unpack_from('<ii', d, r)
                        r += 8
                        tgt.append([struct.unpack_from('<3f', d, r + 12 * i) for i in range(nv)] if hv else None)
                        r += (nv * 12 if hv else 0) + (nv * 12 if hn else 0)
                    if tgt[0] and tgt[1]:
                        out += list(zip(tgt[0], tgt[1]))
                g = gp + gsz
        q = pp + s
    return out


if __name__ == '__main__':
    sys.exit(main())
