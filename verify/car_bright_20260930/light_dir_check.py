# Reproduce, from the shipped LIGHTS.DFF assets alone, BOTH directional-light
# world vectors for every track:
#   CORRECT : the light frame's at-vector composed up the chain starting at the
#             frame's PARENT (RW semantics: rot[6..8] is already the frame's own
#             at axis expressed in its parent's space).
#   AS-BUILT: what mashedmod/src/mashed_re/D3d9Render/TrackRenderer.cpp:853
#             (and the twin at :751) actually computes -- the loop starts at
#             `fi = pending_frame`, i.e. the light frame's OWN rotation is
#             applied to its OWN at-vector, one composition too many.
#
# The port then uses L = -normalize(dir) as the "toward the light" vector
# (TrackRenderer.cpp:1218-1226). L[1] > 0 means the sun is ABOVE the horizon.
#
# Run: py -3.12 verify/car_bright_20260930/light_dir_check.py <dir-with-extracted-LIGHTS.DFF-per-track>
# or with no argument to extract from original/TOASTART/TRACKS/*.piz via re/tools/piz_extract.py.
import struct, sys, math, subprocess, tempfile, os
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent.parent


def parse(buf):
    """Return (frames, lights). frames: list of dict(rot[9], pos[3], parent).
       lights: list of dict(type, rgb, frame)."""
    def u32(o): return struct.unpack_from("<I", buf, o)[0]
    def i32(o): return struct.unpack_from("<i", buf, o)[0]
    def f32(o): return struct.unpack_from("<f", buf, o)[0]

    assert u32(0) == 0x10, "not a CLUMP"
    end = 12 + u32(4)
    assert u32(12) == 0x01
    o = 24 + u32(16)
    frames = []
    if u32(o) == 0x0E:                                   # FRAMELIST
        flsz, flb = u32(o + 4), o + 12
        assert u32(flb) == 0x01
        fstb = flb + 12
        nf = i32(fstb)
        q = fstb + 4
        for _ in range(nf):
            rot = [f32(q + k * 4) for k in range(9)]
            pos = [f32(q + 36 + k * 4) for k in range(3)]
            frames.append(dict(rot=rot, pos=pos, parent=i32(q + 48)))
            q += 0x38
        o = flb + flsz
    lights, pending = [], -1
    while o + 12 <= end:
        t, sz = u32(o), u32(o + 4)
        body = o + 12
        if t == 0x01 and sz == 4:
            pending = i32(body)
        elif t == 0x12 and u32(body) == 0x01:
            sb = body + 12
            lights.append(dict(rgb=(f32(sb + 4), f32(sb + 8), f32(sb + 12)),
                               type=u32(sb + 20) >> 16, frame=pending))
            pending = -1
        o = body + sz
    return frames, lights


def apply(m, v):
    return [m[0]*v[0] + m[3]*v[1] + m[6]*v[2],
            m[1]*v[0] + m[4]*v[1] + m[7]*v[2],
            m[2]*v[0] + m[5]*v[1] + m[8]*v[2]]


def norm(v):
    l = math.sqrt(sum(c * c for c in v))
    return [c / l for c in v] if l > 1e-6 else v


def compose(frames, fidx, start_at_self):
    """start_at_self=True reproduces TrackRenderer.cpp:853 as built."""
    v = list(frames[fidx]['rot'][6:9])
    fi = fidx if start_at_self else frames[fidx]['parent']
    while fi >= 0:
        v = apply(frames[fi]['rot'], v)
        pa = frames[fi]['parent']
        if pa == fi or pa < 0:
            break
        fi = pa
    return norm(v)


def main():
    tdir = ROOT / "original" / "TOASTART" / "TRACKS"
    tmp = Path(tempfile.mkdtemp(prefix="lights_"))
    rows = []
    for piz in sorted(tdir.glob("*.piz")):
        out = tmp / piz.stem
        out.mkdir(parents=True, exist_ok=True)
        subprocess.run([sys.executable, str(ROOT / "re" / "tools" / "piz_extract.py"),
                        "extract", str(piz), "-o", str(out), "--filter", "LIGHTS.DFF"],
                       stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)
        cand = list(out.rglob("LIGHTS.DFF"))
        if not cand:
            print(f"{piz.stem:<10} NO LIGHTS.DFF")
            continue
        frames, lights = parse(cand[0].read_bytes())
        for li in lights:
            if li['type'] != 1:
                continue
            fi = li['frame']
            good = compose(frames, fi, False)
            built = compose(frames, fi, True)
            Lg = [-c for c in good]
            Lb = [-c for c in built]
            same = all(abs(a - b) < 1e-4 for a, b in zip(good, built))
            rows.append((piz.stem, good, built, Lg, Lb, same))
            print(f"{piz.stem:<10} frame={fi} parent={frames[fi]['parent']}")
            print(f"   dir CORRECT  = ({good[0]:+.6f},{good[1]:+.6f},{good[2]:+.6f})"
                  f"   -> L=({Lg[0]:+.4f},{Lg[1]:+.4f},{Lg[2]:+.4f})  sun_elev_y={Lg[1]:+.4f}")
            print(f"   dir AS-BUILT = ({built[0]:+.6f},{built[1]:+.6f},{built[2]:+.6f})"
                  f"   -> L=({Lb[0]:+.4f},{Lb[1]:+.4f},{Lb[2]:+.4f})  sun_elev_y={Lb[1]:+.4f}"
                  f"   {'IDENTICAL' if same else '*** DIVERGES ***'}")
            # what an up-facing panel (N=(0,1,0)) receives, amb from the AMBIENT light
            amb = next((l['rgb'][0] for l in lights if l['type'] == 2), 0.0)
            sun = li['rgb'][0]
            up_good = min(1.0, amb + sun * max(0.0, Lg[1]))
            up_built = min(1.0, amb + sun * max(0.0, Lb[1]))
            print(f"   up-facing panel (N=+Y): amb={amb:.3f} sun={sun:.3f}"
                  f"  correct={up_good:.4f}  as-built={up_built:.4f}"
                  f"  ratio(as-built/correct)={up_built/up_good:.4f}")
    n_div = sum(1 for r in rows if not r[5])
    n_below = sum(1 for r in rows if r[4][1] <= 0)
    n_below_ok = sum(1 for r in rows if r[3][1] <= 0)
    print(f"\nSUMMARY: {len(rows)} directional lights, {n_div} diverge between "
          f"CORRECT and AS-BUILT.")
    print(f"  suns below the horizon (L.y<=0): CORRECT={n_below_ok}  AS-BUILT={n_below}")


if __name__ == "__main__":
    main()
