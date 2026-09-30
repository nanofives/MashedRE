#!/usr/bin/env python3
"""loop_plot.py - overlay the ORIGINAL's and the PORT's world loops on a track's
collision mesh, and mark every port off-mesh recovery.

READ-ONLY. Does not execute the game.

Sources
-------
ORIGINAL  an MSD1 capture (`scenario_launch.py --statediff-out`). World position is
          the translation row of the RwMatrix at record +0x928, i.e. record
          +0x958/+0x95c/+0x960 (x/y/z). That matrix is the object A4 computes once at
          0x00470699 (`record + [record+0x9a8]*0x40 + 0x928`) and hands to A5
          (0x00470918) and A6b (0x0047093b); see D2_REOPEN_2026-09-29.md 4.1-4.2.
          Validated here rather than asserted: --check prints the row norms and the
          spawn point so it can be compared against the track's own start line.
PORT      `player_trace.log` from MASHED_PLAYERTRACE=1 (`pos=(x,y,z)` per sim step).
MESH      COLLISIONS.BSP out of the track .piz, parsed with re/tools/track_dump.py -
          the same triangle soup TrackRenderer flattens into col_verts_/col_tris_
          (TrackRenderer.cpp:1009-1032).
FIRES     the MASHED_OFFMESH_LOG lines (`from=(x,y,z)`).

Usage
  py -3.12 re/tools/statediff/loop_plot.py --png out.png \
      [--orig <cap.msd>] [--orig-track <piz>] \
      [--port <player_trace.log>] [--port-track <piz>] \
      [--offmesh <offmesh.log>] [--check]

With two different --orig-track/--port-track the plot is drawn as two panels, one
per track, which is the only honest way to show a cross-track pair.
"""
import argparse
import math
import struct
import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
import track_dump           # noqa: E402
import piz_extract          # noqa: E402


# ---------------------------------------------------------------- sources
def orig_xyz(path):
    """(frame_idx, x, y, z) per captured frame, from record +0x958..+0x960."""
    b = open(path, 'rb').read()
    assert b[:4] == b'MSD1', 'not MSD1'
    rec, _base, _ = struct.unpack_from('<III', b, 4)
    off, out = 16, []
    while off + 4 + rec <= len(b):
        fi, = struct.unpack_from('<I', b, off)
        p = b[off + 4: off + 4 + rec]
        m = struct.unpack_from('<16f', p, 0x928)
        out.append((fi, m[12], m[13], m[14], m))
        off += 4 + rec
    return out


def port_xyz(path):
    out = []
    for line in open(path, errors='replace'):
        i = line.find('pos=(')
        if i < 0:
            continue
        j = line.find(')', i)
        x, y, z = (float(v) for v in line[i + 5:j].split(','))
        f = line.split('f=', 1)[1].split(' ', 1)[0]
        out.append((int(f), x, y, z))
    return out


def fires_xz(path):
    out = []
    for line in open(path, errors='replace'):
        i = line.find('from=(')
        if i < 0:
            continue
        j = line.find(')', i)
        x, y, z = (float(v) for v in line[i + 6:j].split(','))
        out.append((x, z))
    return out


def mesh_tris(piz_path):
    """XZ triangles of the track's COLLISIONS.BSP (the port's col_tris_ soup)."""
    data, _v, _a, entries, _m = piz_extract.read_archive(Path(piz_path))
    blob = None
    for (name, off, length, _fid) in entries:
        u = name.upper()
        if u.startswith('COLL') and u.endswith('.BSP'):
            blob = data[off:off + length]
            break
    if blob is None:
        sys.exit(f'no COLL*.BSP in {piz_path}')
    _hdr, sectors = track_dump.parse_world(blob)
    # track_dump gives verts as (x,y,z) tuples and tris as (mat, v0, v1, v2) --
    # the same field order TrackRenderer.cpp:1023-1030 reads (element 0 = material).
    tris = []
    for s in sectors:
        v = s['verts']
        for (_mat, i0, i1, i2) in s['tris']:
            tris.append(((v[i0][0], v[i0][2]),
                         (v[i1][0], v[i1][2]),
                         (v[i2][0], v[i2][2])))
    return tris


def on_mesh(tris, x, z):
    """HeightOnSoup's XZ barycentric test (TrackRenderer.cpp:2090-2107)."""
    for (ax, az), (bx, bz), (cx, cz) in tris:
        d00x, d00z = bx - ax, bz - az
        d01x, d01z = cx - ax, cz - az
        den = d00x * d01z - d01x * d00z
        if -1e-9 < den < 1e-9:
            continue
        px, pz = x - ax, z - az
        u = (px * d01z - d01x * pz) / den
        v = (d00x * pz - px * d00z) / den
        if u < 0.0 or v < 0.0 or u + v > 1.0:
            continue
        return True
    return False


# ---------------------------------------------------------------- plot
# PIL, not matplotlib: matplotlib is not installed for this interpreter, and
# re/tools/track_dump.py:286-306 already draws its top-down X/Z wireframe with
# PIL/ImageDraw. Same projection and the same colour for the mesh.
PANEL = 1000            # px per panel
PAD = 30

def _extent(tris, rows, fires):
    """Bounds covering the whole mesh plus anything plotted on it."""
    xs = [v[0] for t in tris for v in t] + [r[1] for r in rows] + [q[0] for q in fires]
    zs = [v[1] for t in tris for v in t] + [r[3] for r in rows] + [q[1] for q in fires]
    return min(xs), max(xs), min(zs), max(zs)


def _panel(tris, loops, fires, title, extent):
    """One top-down X/Z panel. `loops` is [(rows, rgb, label, marker)]."""
    from PIL import Image, ImageDraw
    x0, x1, z0, z1 = extent
    span = max(x1 - x0, z1 - z0) or 1.0
    W = H = PANEL

    def px(x, z):
        return (int((x - x0) / span * (W - 2 * PAD)) + PAD,
                int((z - z0) / span * (H - 2 * PAD)) + PAD)

    img = Image.new('RGB', (W, H + 46), (10, 10, 16))
    dr = ImageDraw.Draw(img)
    for a, b, c in tris:
        dr.line([px(*a), px(*b), px(*c), px(*a)], fill=(52, 96, 66), width=1)
    legend = [title, f'mesh COLLISIONS.BSP tris={len(tris)}   '
                     f'x {x0:.1f}..{x1:.1f}  z {z0:.1f}..{z1:.1f}']
    for rows, rgb, label, _mk in loops:
        if not rows:
            continue
        pts = [px(r[1], r[3]) for r in rows]
        dr.line(pts, fill=rgb, width=2)
        s = pts[0]
        dr.ellipse([s[0] - 6, s[1] - 6, s[0] + 6, s[1] + 6], outline=(255, 255, 255),
                   fill=rgb, width=2)
        legend.append(f'{label}: n={len(rows)} spawn=({rows[0][1]:.2f}, {rows[0][3]:.2f})')
    for fx, fz in fires:
        q = px(fx, fz)
        dr.line([(q[0] - 5, q[1] - 5), (q[0] + 5, q[1] + 5)], fill=(230, 60, 90), width=2)
        dr.line([(q[0] - 5, q[1] + 5), (q[0] + 5, q[1] - 5)], fill=(230, 60, 90), width=2)
    if fires:
        legend.append(f'red x = RecoverOffMesh fire, n={len(fires)}')
    for i, line in enumerate(legend):
        dr.text((8, H + 2 + i * 11), line, fill=(220, 220, 230))
    return img


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('--orig'); ap.add_argument('--orig-track')
    ap.add_argument('--port'); ap.add_argument('--port-track')
    ap.add_argument('--offmesh')
    ap.add_argument('--png')
    ap.add_argument('--check', action='store_true')
    a = ap.parse_args()

    o = orig_xyz(a.orig) if a.orig else []
    p = port_xyz(a.port) if a.port else []
    f = fires_xz(a.offmesh) if a.offmesh else []
    # frame 0 of a capture is the pre-valid all-zero record (FORMAT.md) - drop it
    o = [r for r in o if not (r[1] == 0.0 and r[2] == 0.0 and r[3] == 0.0)]

    if a.check:
        if o:
            bad = 0
            for _fi, _x, _y, _z, m in o:
                for r in (0, 1, 2):
                    n = math.sqrt(sum(m[r * 4 + c] ** 2 for c in range(3)))
                    if abs(n - 1.0) > 0.01:
                        bad += 1
            print(f'ORIG {a.orig}: {len(o)} frames, +0x928 rows off unit norm {bad}/{3*len(o)}')
            print(f'  spawn      ({o[0][1]:+.4f}, {o[0][2]:+.4f}, {o[0][3]:+.4f})')
            print(f'  x {min(r[1] for r in o):+.3f}..{max(r[1] for r in o):+.3f}'
                  f'   y {min(r[2] for r in o):+.3f}..{max(r[2] for r in o):+.3f}'
                  f'   z {min(r[3] for r in o):+.3f}..{max(r[3] for r in o):+.3f}')
        if p:
            print(f'PORT {a.port}: {len(p)} steps')
            print(f'  spawn      ({p[0][1]:+.4f}, {p[0][2]:+.4f}, {p[0][3]:+.4f})')
            print(f'  x {min(r[1] for r in p):+.3f}..{max(r[1] for r in p):+.3f}'
                  f'   y {min(r[2] for r in p):+.3f}..{max(r[2] for r in p):+.3f}'
                  f'   z {min(r[3] for r in p):+.3f}..{max(r[3] for r in p):+.3f}')
        # containment: is each side's loop on the OTHER side's collision soup?
        for lbl, rows, trk in (('ORIG', o, a.orig_track), ('PORT', p, a.port_track)):
            if not rows or not trk:
                continue
            for tlbl, ttrk in (('orig-track', a.orig_track), ('port-track', a.port_track)):
                if not ttrk:
                    continue
                tris = mesh_tris(ttrk)
                step = max(1, len(rows) // 200)
                sel = rows[::step]
                hits = sum(1 for r in sel if on_mesh(tris, r[1], r[3]))
                print(f'  {lbl} loop on {tlbl} ({Path(ttrk).stem}) mesh: '
                      f'{hits}/{len(sel)} sampled points on-mesh')

    if not a.png:
        return
    from PIL import Image
    ORIG_RGB, PORT_RGB = (90, 160, 255), (255, 150, 40)
    panels = []
    if a.orig_track and a.port_track and a.orig_track == a.port_track:
        # one track: both loops share the panel and the same projection
        tris = mesh_tris(a.orig_track)
        ext = _extent(tris, o + p, f)
        panels.append(_panel(tris, [(o, ORIG_RGB, 'ORIGINAL (blue)', 'o'),
                                    (p, PORT_RGB, 'PORT (orange)', 's')], f,
                             f'BOTH ARMS - {Path(a.orig_track).stem} COLLISIONS.BSP', ext))
    else:
        # two tracks: two panels. Drawing a cross-track pair in one frame would
        # imply a shared world the two arms do not share.
        if a.orig_track:
            tris = mesh_tris(a.orig_track)
            panels.append(_panel(tris, [(o, ORIG_RGB, 'ORIGINAL (blue)', 'o')], [],
                                 f'ORIGINAL arm - {Path(a.orig_track).stem} '
                                 f'COLLISIONS.BSP', _extent(tris, o, [])))
        if a.port_track:
            tris = mesh_tris(a.port_track)
            panels.append(_panel(tris, [(p, PORT_RGB, 'PORT (orange)', 's')], f,
                                 f'PORT arm - {Path(a.port_track).stem} '
                                 f'COLLISIONS.BSP', _extent(tris, p, f)))
    W = sum(im.width for im in panels)
    H = max(im.height for im in panels)
    out = Image.new('RGB', (W, H), (10, 10, 16))
    x = 0
    for im in panels:
        out.paste(im, (x, 0)); x += im.width
    out.save(a.png)
    print('wrote', a.png)


if __name__ == '__main__':
    main()
