#!/usr/bin/env python3
"""U-9147: replay A6a block #4 (FUN_00467650) on EITHER side's recorded inputs and
compare its outputs quantity by quantity.

WHY THIS REPLACES a8_wheelfit.py's CROSS-SIDE FIT
-------------------------------------------------
a8_wheelfit.py fitted `F = a*lat + b*axis` per wheel on each side and compared the
fitted `a`. That was withdrawn on 2026-09-29 (re/analysis/D2_REOPEN_2026-09-29.md
section 6.0, third correction): its PORT side rebuilt `lat` from a 2-D velocity
heading, `u = (cos velH, 0, sin velH)`, and ignored the `wld4` the port logged, so
the two sides' fit bases were different vectors in every frame. The arithmetic tell
was that with p[-1] == 0 the applied lateral scale obeys `f5 <= lbc` always
(Integrate2.cpp:442-457), so the reported `a/lbc = 1.840` was impossible.

This tool does not fit anything. It TRANSCRIBES the law once, from
mashedmod/src/mashed_re/Vehicle/Integrate2.cpp block #4 (RVA 0x00467650), and then:

  SELF-CHECK 1 (port, exact)   replay(act.* inputs)  vs  A6a's OWN logged outputs.
        The port now logs both (MASHED_A6ADUMP, VehiclePhysicsRun.cpp). If the
        replay does not reproduce a.le / a.lat / a.ld4 / a.lbc / a.f5 / a.dF to
        float32 precision, the replay is wrong and nothing else here may be read.

  SELF-CHECK 2 (port, phase)   replay(snap.* inputs) vs  A6a's OWN logged outputs.
        `snap.*` is the same record read at the render-tick phase -- the ONLY phase
        the original's .msd capture has. This measures what the phase difference
        costs. A snapshot-driven cross-side comparison is sound only if this is
        small, and that was never checked for a8_wheelfit.py.

  SELF-CHECK 3 (invariant)     f5 <= lbc whenever p[-1] == 0, on both sides.

  CROSS (original)             replay(.msd record fields) vs the original's OWN
        recorded per-wheel force +0x70/+0x74/+0x78. Same law, the original's data.
        A residual here means the ported law disagrees with the original's actual
        behaviour, and the per-quantity table says which quantity first.

g_suspScale (_DAT_0088e5f0) is NOT in the 0xd04 record, so it must be supplied for
the original side with --orig-susp (measure it, do not guess).

Read-only. Does not execute the game.
"""
import argparse, math, re, struct, sys, os
import numpy as np

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import a8_momentum as m  # noqa: E402

F32 = np.float32


def cf(bits):
    return F32(struct.unpack("<f", struct.pack("<I", bits))[0])


# --- Integrate2.cpp:90-118 constants, exact .rdata bit patterns ---
K_ANG_LO    = cf(0xb727c5ac)   # 0x005cea1c
K_ANG_HI    = cf(0x3727c5ac)   # 0x005cc990
K_0P008     = cf(0x3c03126f)   # 0x005cea14
K_8         = F32(8.0)         # 0x005cc9f4
K_0P019877  = cf(0x3ca30eac)   # 0x005cea18
K_360       = F32(360.0)       # 0x005ccac4
K_270       = F32(270.0)
K_1024      = cf(0x44800000)   # 0x005cea10
K_0P0009766 = cf(0x3a800000)   # 0x005cea0c
K_SPEEDMIN  = cf(0x38d1b717)   # 0x005cd03c
K_50        = F32(50.0)
K_0P02      = cf(0x3ca3d70a)   # Integrate2.cpp k0p02
K_0P85      = cf(0x3f59999a)   # 0x005ce264
K_0P0019531 = cf(0x3b000000)   # 0x005cea08
K_1P1       = F32(1.1)
K_3         = F32(3.0)
WB = [0x1a4 + w * 0xc4 for w in range(4)]
BANDS = ((1500, 2000), (2000, 2600))


def rot270(av):
    """Rw_MatrixFromAxisAngle(m, av, 270, mode 0) followed by Rw_TransformPoints,
    i.e. the row-vector Rodrigues form transcribed in
    mashedmod/src/mashed_re/Math/RwMatrixRotateInner.cpp:11-16 (body 0x004c4a50).
    Validated, not assumed: SELF-CHECK 1 compares the resulting `le` against the
    port's own a.le."""
    n = math.sqrt(float(av[0]) ** 2 + float(av[1]) ** 2 + float(av[2]) ** 2)
    if n < 1e-20:
        return np.eye(3, dtype=np.float32)
    x, y, z = (float(c) / n for c in av)
    a = math.radians(270.0)
    s, omc = math.sin(a), 1.0 - math.cos(a)
    return np.array([
        [1.0 - (1.0 - x * x) * omc,  z * s + y * x * omc,        z * x * omc - y * s],
        [y * x * omc - z * s,        1.0 - (1.0 - y * y) * omc,  s * x + y * z * omc],
        [y * s + z * x * omc,        y * z * omc - s * x,        1.0 - (1.0 - z * z) * omc],
    ], dtype=np.float32)


def replay(fr, wl):
    """fr: dict(vel, av, bf, sp, angsp, gc, susp).  wl: dict(off, ax, p15, p16, p1b, pm1).
    Returns the block-#4 chain, or None with a reason if the gate at :428 refuses."""
    av, vel = fr["av"], fr["vel"]
    spin = any(c < K_ANG_LO or K_ANG_HI < c for c in av)
    if spin:
        R = rot270(av)
        src = np.array(wl["off"], dtype=np.float32)
        dst = np.array([src[0] * R[0][i] + src[1] * R[1][i] + src[2] * R[2][i]
                        for i in range(3)], dtype=np.float32)
        f = F32(fr["sp"] * K_0P008)
        if f < K_8:
            f = K_8
        f = F32(F32(F32(f * F32(fr["angsp"])) * K_0P019877) * K_360)
        le = np.array([F32(dst[i] * f) - F32(vel[i]) for i in range(3)], dtype=np.float32)
        le4raw = F32(math.sqrt(float(le[0]) ** 2 + float(le[1]) ** 2 + float(le[2]) ** 2))
    else:
        le = np.array([-F32(vel[0]), -F32(vel[1]), -F32(vel[2])], dtype=np.float32)
        le4raw = F32(fr["sp"])
    out = dict(spin=1 if spin else 0, le=le, le4raw=le4raw)
    if not (K_SPEEDMIN < le4raw):
        return out, "le4 <= kSpeedMin"
    if struct.pack("<f", float(fr["gc"])) != struct.pack("<f", 4.0):
        return out, "gc != 4.0"
    inv = F32(F32(1.0) / le4raw)                     # :430, BEFORE the 1024 cap
    s0, s1 = F32(le[0] * inv), F32(le[1] * inv)
    le4 = K_1024 if K_1024 < le4raw else le4raw      # :432
    susp, p1b = F32(fr["susp"]), F32(wl["p1b"])
    lbc = F32(F32(F32(F32(F32(wl["p15"]) * p1b) * susp) * le4) * K_0P0009766)
    bf = fr["bf"]
    lc0 = F32(F32(F32(bf[2] * inv) * le[2]) + F32(F32(bf[0] * s0) + F32(bf[1] * s1)))
    ax = wl["ax"]
    lc8, lc4, lc0z = F32(lc0 * ax[0]), F32(lc0 * ax[1]), F32(lc0 * ax[2])
    lat = np.array([F32(s0 - lc8), F32(s1 - lc4), F32(F32(inv * le[2]) - lc0z)], dtype=np.float32)
    ld4 = F32(math.sqrt(float(lat[0]) ** 2 + float(lat[1]) ** 2 + float(lat[2]) ** 2))
    out.update(inv=inv, le4=le4, lbc=lbc, lc0=lc0, lat=lat, ld4=ld4)
    l94 = wl["pm1"] & 0xFFFFFFFF
    if (l94 & 0x100) == 0:
        f5, l98, l9c, la0 = lbc, F32(0), F32(0), F32(0)
        if l94 == 0:
            l98 = F32(F32(F32(wl["p16"]) * p1b) * susp)
            la0, l9c, l98 = F32(lc8 * l98), F32(lc4 * l98), F32(l98 * lc0z)
            f4 = F32(ld4 * F32(fr["sp"]))
            if f4 < K_50:
                f5 = F32(F32(f4 * K_0P02) * lbc)
        dF = np.array([F32(F32(lat[0] * f5) + la0), F32(F32(lat[1] * f5) + l9c),
                       F32(F32(f5 * lat[2]) + l98)], dtype=np.float32)
        out.update(f5=f5, dF=dF, arm=0)
    else:
        f5 = F32(lbc * K_0P85)
        f4 = F32(F32(F32(F32(F32(p1b * K_3) * susp) * F32(float(l94))) * K_0P0019531) * K_1P1)
        dF = np.array([F32(F32(lc8 * f4) + F32(lat[0] * f5)),
                       F32(F32(lc4 * f4) + F32(lat[1] * f5)),
                       F32(F32(lc0z * f4) + F32(f5 * lat[2]))], dtype=np.float32)
        out.update(f5=f5, dF=dF, arm=1)
    return out, None


# ---------------------------------------------------------------- port dump parsing
RE_F = re.compile(r"f=(\d+) ")


def _kv(seg):
    d = {}
    for k, v in re.findall(r"([a-zA-Z0-9_.]+)=(-?[0-9][^ ]*)", seg):
        d[k] = v
    return d


def load_port(path):
    rows = []
    for line in open(path, errors="replace"):
        parts = line.rstrip("\n").split(" |")
        if len(parts) != 5:
            continue
        h = _kv(parts[0])
        v3 = lambda s: [F32(x) for x in s.split(",")]
        row = dict(f=int(h["f"]))
        for pre in ("snap", "act"):
            row[pre] = dict(vel=v3(h[pre + ".vel"]), av=v3(h[pre + ".av"]),
                            bf=v3(h[pre + ".bf"]), sp=F32(h[pre + ".sp"]),
                            angsp=F32(h[pre + ".angsp"]), gc=F32(h[pre + ".gc"]),
                            susp=F32(h[pre + ".susp"]))
        row["steer"] = float(h["snap.steer"])
        row["c6"] = dict(m18c=float(h["snap.m18c"]), m2c=int(h["snap.m2c"]),
                         m34=int(h["snap.m34"]), tid=int(h["snap.tid"]),
                         d00=int(h["snap.d00"]))
        row["c6act"] = dict(l60=float(h["act.l60"]), m18c=float(h["act.m18c"]),
                            speed=float(h["act.speed"]), grip=float(h["act.grip"]),
                            kvel=float(h["act.kvel"]), kav=float(h["act.kav"]),
                            arm=int(h["act.arm"]), ran=int(h["act.clamp"]))
        ws = []
        for w in range(4):
            d = _kv(parts[1 + w])
            ws.append(dict(
                fired=int(d["fired"]), spin=int(d["spin"]),
                snap=dict(off=v3(d["s.off"]), ax=v3(d["s.ax"]), p15=F32(d["s.p15"]),
                          p16=F32(d["s.p16"]), p1b=F32(d["s.p1b"]), pm1=int(d["s.pm1"]),
                          pm3=int(d["s.pm3"]), pmb=F32(d["s.pmb"]), F=v3(d["s.F"])),
                act=dict(off=v3(d["a.off"]), ax=v3(d["a.ax"]), p15=F32(d["a.p15"]),
                         p16=F32(d["a.p16"]), p1b=F32(d["a.p1b"]), pm1=int(d["a.pm1"]),
                         le=v3(d["a.le"]), le4raw=F32(d["a.le4raw"]), le4=F32(d["a.le4"]),
                         lat=v3(d["a.lat"]), ld4=F32(d["a.ld4"]), lbc=F32(d["a.lbc"]),
                         f5=F32(d["a.f5"]), dF=v3(d["a.dF"]))))
        row["w"] = ws
        rows.append(row)
    return rows


def relerr(a, b):
    a, b = float(a), float(b)
    d = abs(a - b)
    s = max(abs(a), abs(b))
    return 0.0 if s == 0.0 else d / s


def vrel(a, b):
    na = math.sqrt(sum(float(x) ** 2 for x in a))
    nb = math.sqrt(sum(float(x) ** 2 for x in b))
    dn = math.sqrt(sum((float(x) - float(y)) ** 2 for x, y in zip(a, b)))
    s = max(na, nb)
    return 0.0 if s == 0.0 else dn / s


def med(v):
    v = sorted(v)
    return v[len(v) // 2] if v else float("nan")


SCALARS = ("le4raw", "le4", "ld4", "lbc", "f5")
VECS = ("le", "lat", "dF")


def selfcheck(rows, phase, min_speed, steer_min, label):
    """replay(rows[phase]) vs A6a's own logged act.* outputs."""
    acc = {k: [] for k in SCALARS + VECS}
    nfit = nskip = 0
    for r in rows:
        if float(r["snap"]["sp"]) < min_speed or r["steer"] < steer_min:
            continue
        for w in range(4):
            W = r["w"][w]
            if not W["fired"]:
                continue
            src = dict(r[phase])
            wl = W[phase]
            if phase == "snap" and wl["pm3"] == 0:
                nskip += 1
                continue
            got, why = replay(src, wl)
            if why:
                nskip += 1
                continue
            nfit += 1
            for k in SCALARS:
                acc[k].append(relerr(got[k], W["act"][k]))
            for k in VECS:
                acc[k].append(vrel(got[k], W["act"][k]))
    print(f"\n--- SELF-CHECK  {label}  (replay from `{phase}.*` vs A6a's logged act.*) ---")
    print(f"    samples={nfit}  refused-by-gate={nskip}")
    if not nfit:
        return None
    print(f"    {'quantity':<8} {'median rel':>12} {'p95 rel':>12} {'max rel':>12}")
    worst = 0.0
    for k in SCALARS + VECS:
        v = sorted(acc[k])
        p95 = v[int(0.95 * (len(v) - 1))]
        print(f"    {k:<8} {med(v):>12.3e} {p95:>12.3e} {max(v):>12.3e}")
        worst = max(worst, max(v))
    return worst


def invariant(rows, min_speed, steer_min):
    bad = n = 0
    for r in rows:
        if float(r["snap"]["sp"]) < min_speed or r["steer"] < steer_min:
            continue
        for w in range(4):
            W = r["w"][w]
            if not W["fired"] or W["act"]["pm1"] != 0:
                continue
            n += 1
            if float(W["act"]["f5"]) > float(W["act"]["lbc"]) * (1 + 1e-6):
                bad += 1
    print(f"\n--- SELF-CHECK 3  invariant f5 <= lbc when p[-1] == 0 (Integrate2.cpp:442-457) ---")
    print(f"    samples={n}  violations={bad}   {'PASS' if bad == 0 and n else 'FAIL/EMPTY'}")
    return bad, n


def orig_rows(path, susp, min_speed, steer_min):
    _, _, frames = m.load_msd(path)
    g = lambda p, o: struct.unpack_from("<f", p, o)[0]
    gi = lambda p, o: struct.unpack_from("<i", p, o)[0]
    out = []
    for idx in sorted(frames):
        p = frames[idx]
        if g(p, 0x9e4) < min_speed or g(p, 0x1a8) < steer_min or g(p, 0x9e0) < 3.5:
            continue
        fr = dict(vel=[F32(g(p, 0x9b0 + 4 * i)) for i in range(3)],
                  av=[F32(g(p, 0x9bc + 4 * i)) for i in range(3)],
                  bf=[F32(g(p, 0x9d4 + 4 * i)) for i in range(3)],
                  sp=F32(g(p, 0x9e4)), angsp=F32(g(p, 0x9e8)), gc=F32(g(p, 0x9e0)),
                  susp=F32(susp))
        ws = []
        for b in WB:
            ws.append(dict(off=[F32(g(p, b - 0x24 + 4 * i)) for i in range(3)],
                           ax=[F32(g(p, b + 0x7c + 4 * i)) for i in range(3)],
                           p15=F32(g(p, b + 0x54)), p16=F32(g(p, b + 0x58)),
                           p1b=F32(g(p, b + 0x6c)), pm1=gi(p, b - 0x04),
                           pm3=gi(p, b - 0x0c), pmb=F32(g(p, b - 0x2c)),
                           F=[F32(g(p, b + 0x70 + 4 * i)) for i in range(3)]))
        out.append(dict(idx=idx, fr=fr, w=ws, sp=float(g(p, 0x9e4)),
                        c6=dict(m18c=g(p, 0x18c), m2c=gi(p, 0x2c), m34=gi(p, 0x34),
                                tid=gi(p, 0x1f0), d00=gi(p, 0xd00))))
    return out


def orig_rows_all(path):
    """every frame, unfiltered -- the angular check needs consecutive pairs."""
    _, _, frames = m.load_msd(path)
    g = lambda p, o: struct.unpack_from("<f", p, o)[0]
    out = []
    for idx in sorted(frames):
        p = frames[idx]
        ws = [dict(off=[F32(g(p, b - 0x24 + 4 * i)) for i in range(3)],
                   pmb=F32(g(p, b - 0x2c)),
                   F=[F32(g(p, b + 0x70 + 4 * i)) for i in range(3)]) for b in WB]
        out.append(dict(idx=idx, w=ws, sp=float(g(p, 0x9e4)),
                        av=[g(p, 0x9bc + 4 * i) for i in range(3)],
                        bf=[g(p, 0x9d4 + 4 * i) for i in range(3)],
                        vel=[g(p, 0x9b0 + 4 * i) for i in range(3)],
                        steer=float(g(p, 0x1a8)), gc=float(g(p, 0x9e0))))
    return out


def cross(rows, label, getfr, getw, getF, nrows, lag=0, susp_used=None):
    """replay(record snapshot at frame n) vs the recorded per-wheel force at frame n+lag.

    `lag` exists because the two sides run A6a at a different point in the frame
    relative to the render-tick snapshot the record is read at:
      ORIGINAL (FUN_00470c70): step 3 = A4 -> A5/A6a/A6b, step 5 = the substep loop,
        then the snapshot. So the snapshot at frame n is A6a(n)'s output AFTER a whole
        substep loop has moved the inputs -- and it is the best available proxy for
        A6a(n+1)'s INPUTS. lag=1 is the matched pairing there.
      PORT (VehiclePhysicsRun.cpp:847): the substep loop runs first and A4 last, so the
        snapshot sits immediately after A6a and lag=0 is the matched pairing. Running
        the port at lag=1 is the control that shows the lag is not free.

    `suspEff` is the recovered g_suspScale. Block #4's whole output is LINEAR in
    g_suspScale (it enters only through lbc at :434 and l98 at :454), so
    suspEff = susp_used * |F| / |dF| is a one-parameter recovery, not a fit of the
    law's shape. The law's shape is checked separately by `dir cos`.
    """
    print(f"\n=== CROSS  {label}: {nrows} frames, lag={lag} ===")
    print(f"    {'wheel':<6} {'n':>5} {'|dF|med':>12} {'|F|med':>12} {'|dF|/|F|':>9} "
          f"{'dir cos':>8} {'vec rel':>9} {'ld4':>7} {'lbc':>10} {'f5/lbc':>7} {'suspEff':>9}")
    res = {}
    for w in range(4):
        rows_w, ratios, coss, vrels, ld4s, lbcs, f5r = [], [], [], [], [], [], []
        ndFs, nFs, susps = [], [], []
        for k, r in enumerate(rows):
            if k + lag >= len(rows):
                break
            fr, wl = getfr(r), getw(r, w)
            if wl["pm3"] == 0:
                continue
            got, why = replay(fr, wl)
            if why:
                continue
            Fv = getF(rows[k + lag], w)
            nF = math.sqrt(sum(float(x) ** 2 for x in Fv))
            ndF = math.sqrt(sum(float(x) ** 2 for x in got["dF"]))
            if nF < 1.0 or ndF < 1.0:
                continue
            rows_w.append(1)
            ndFs.append(ndF); nFs.append(nF)
            ratios.append(ndF / nF)
            coss.append(sum(float(a) * float(b) for a, b in zip(got["dF"], Fv)) / (nF * ndF))
            vrels.append(vrel(got["dF"], Fv))
            ld4s.append(float(got["ld4"]))
            lbcs.append(float(got["lbc"]))
            f5r.append(float(got["f5"]) / float(got["lbc"]) if float(got["lbc"]) else float("nan"))
            su = susp_used if susp_used is not None else float(fr["susp"])
            susps.append(su * nF / ndF)
        if not rows_w:
            print(f"    w{w:<5} {'0':>5}")
            continue
        print(f"    w{w:<5} {len(rows_w):>5} {med(ndFs):>12.1f} {med(nFs):>12.1f}"
              f" {med(ratios):>9.4f} {med(coss):>8.4f} {med(vrels):>9.4f}"
              f" {med(ld4s):>7.4f} {med(lbcs):>10.1f} {med(f5r):>7.4f} {med(susps):>9.1f}")
        res[w] = dict(ratio=med(ratios), cos=med(coss), vrel=med(vrels),
                      ld4=med(ld4s), lbc=med(lbcs), n=len(rows_w), susp=med(susps))
    return res


K_NORMACCUM = cf(0x4248f5c3)   # 0x005cea04  50.24


def block5(wheels):
    """A6a block #5 (Integrate2.cpp:482-512, from 0x00467650): per-wheel force ->
    the torque accumulators l_78/l_74/l_70 that the angular integration at :516-521
    applies as `av += l_7x * dt * record[0x5c] * 9.2598e-7`.

    Every input is a RECORD field on both sides (per-wheel force +0x70.., offset
    -0x24.., |offset| -0x2c), so this predicts d(av) per frame on either side without
    any hook. The yaw component is l_74 -> +0x9c0."""
    l78 = l74 = l70 = 0.0
    for wl in wheels:
        F = [float(x) for x in wl["F"]]
        if math.sqrt(sum(c * c for c in F)) <= float(K_SPEEDMIN):
            continue
        pmb = float(wl["pmb"])
        if pmb == 0.0:
            continue
        inv = 1.0 / pmb
        d = [float(wl["off"][i]) * inv for i in range(3)]
        dot = d[2] * F[2] + d[1] * F[1] + d[0] * F[0]
        c = [d[i] * dot for i in range(3)]
        a = [F[i] - c[i] for i in range(3)]
        x84 = a[2] * c[1] - a[1] * c[2]
        x80 = a[0] * c[2] - a[2] * c[0]
        x7c = a[1] * c[0] - a[0] * c[1]
        if dot > 0.0:
            x84, x80, x7c = -x84, -x80, -x7c
        m15 = math.sqrt(sum((a[i] * pmb) ** 2 for i in range(3)))
        m16 = math.sqrt(x84 * x84 + x80 * x80 + x7c * x7c)
        if m16 > float(K_SPEEDMIN):
            r = (m15 * float(K_NORMACCUM)) / m16
            l78 -= x84 * r
            l74 -= x80 * r
            l70 -= x7c * r
    return l78, l74, l70


def angular(rows, label, getwheels, getav, getidx, bands, getsp):
    """The ANGULAR analogue of a8_momentum's effective-dt check, per side.

    Predict the torque from the frame's own recorded per-wheel forces (block5), read
    the observed d(av.y) between consecutive frames, and report the ratio as an
    effective `adt`. Both halves are MEASURED quantities on the same side, so this is
    a self-consistency check, not a restatement: if the two sides agree on adt the
    torque -> angular-velocity application is identical and the divergence is upstream;
    if the port's adt is larger it over-rotates for the torque it computes."""
    print(f"\n=== ANGULAR  {label} ===")
    print(f"    {'band':<12} {'n':>5} {'d(av.y)/fr':>11} {'l74 pred':>13} "
          f"{'eff adt':>11} {'av.y':>8} {'l78':>13} {'l70':>13}")
    out = {}
    for lo, hi in bands:
        dys, l74s, effs, avys, l78s, l70s = [], [], [], [], [], []
        for k in range(len(rows) - 1):
            sp = getsp(rows[k])
            if not (lo <= sp < hi):
                continue
            if getidx(rows[k + 1]) != getidx(rows[k]) + 1:   # consecutive frames only
                continue
            l78, l74, l70 = block5(getwheels(rows[k]))
            if abs(l74) < 1e-6:
                continue
            dy = float(getav(rows[k + 1])[1]) - float(getav(rows[k])[1])
            dys.append(dy)
            l74s.append(l74)
            l78s.append(l78)
            l70s.append(l70)
            avys.append(float(getav(rows[k])[1]))
            effs.append(dy / l74)
        if len(dys) < 8:
            print(f"    {f'{lo}-{hi}':<12} {len(dys):>5}")
            continue
        print(f"    {f'{lo}-{hi}':<12} {len(dys):>5} {med(dys):>11.5f} {med(l74s):>13.4g} "
              f"{med(effs):>11.4e} {med(avys):>8.3f} {med(l78s):>13.4g} {med(l70s):>13.4g}")
        out[(lo, hi)] = dict(dy=med(dys), l74=med(l74s), eff=med(effs), n=len(dys))
    return out


def geom(rows, label, getw, getbf, getsp, bands):
    """Per-wheel GEOMETRY, the inputs block #4 is handed: the steered axis heading
    relative to the body forward (+0x9d4/+0x9dc), the wheel offset (-0x24..) and its
    length (-0x2c). These are A5's output, not A6a's, and they set the operating
    point the tire law is evaluated at. Record fields on both sides."""
    print(f"\n=== GEOMETRY  {label} ===")
    print(f"    {'band':<12} {'wheel':<6} {'n':>5} {'ax-bodyH':>10} {'|off|':>8} "
          f"{'off.x':>8} {'off.z':>8} {'ax.y':>9}")
    out = {}
    for lo, hi in bands:
        for w in range(4):
            dh, lo_, ox, oz, axy = [], [], [], [], []
            for r in rows:
                sp = getsp(r)
                if not (lo <= sp < hi):
                    continue
                bf = getbf(r)
                wl = getw(r, w)
                bh = math.atan2(float(bf[2]), float(bf[0]))
                ah = math.atan2(float(wl["ax"][2]), float(wl["ax"][0]))
                d = ah - bh
                while d > math.pi:
                    d -= 2 * math.pi
                while d < -math.pi:
                    d += 2 * math.pi
                dh.append(d)
                lo_.append(float(wl["pmb"]))
                ox.append(float(wl["off"][0]))
                oz.append(float(wl["off"][2]))
                axy.append(float(wl["ax"][1]))
            if len(dh) < 8:
                continue
            print(f"    {f'{lo}-{hi}':<12} w{w:<5} {len(dh):>5} {med(dh):>+10.4f} "
                  f"{med(lo_):>8.4f} {med(ox):>+8.4f} {med(oz):>+8.4f} {med(axy):>+9.5f}")
            out[(lo, hi, w)] = med(dh)
    return out


K_32768     = F32(32768.0)     # 0x005ce9fc
K_1E7       = F32(1.0e7)       # 0x005ce9f8
K_9P9998E8  = cf(0x33d6bf95)   # 0x005ce9f4
K_3P0518E5  = cf(0x38000000)   # 0x005ce9f0


def clamp6(l60, m18c, m2c, m34, tid, d00, speed):
    """A6a's trailing speed-limit grip-clamp #6 (Integrate2.cpp:635-722, from
    0x00467650). It is the DIRECT slip suppressor: it removes `lateral_velocity * k`
    from +0x9b0.. and scales the angular velocity by `1 - k`. Every input is a record
    field except l_60, which is the block-#4 accumulation sum(ld4*le4) and is replayed.

    Returns (grip, arm, k_vel, k_av)."""
    if m18c == 0.0:
        return float("nan"), "?", float("nan"), float("nan")
    grip = F32(l60 / m18c)
    if tid == -0x5f7f80:
        grip = F32(grip * F32(1.5))
    else:
        dbl = (tid == -0x557f80)
        if not dbl:
            if tid in (-0x69e1a6, -0xe17f4c):
                grip = F32(grip * (F32(3.0) if d00 == 0 else F32(1.5)))
            if tid == -0x37e1a6:
                dbl = True
        if dbl:
            grip = F32(grip + grip)
    if m2c != 0:
        grip = F32(F32(F32(float(m2c)) * F32(0.01) + F32(1.0)) * grip)
    if m34 != 0:
        grip = F32(F32(F32(float(m34)) * F32(0.1) + F32(1.0)) * grip)
    grip = F32(grip * F32(speed))
    if K_32768 < grip:
        k = F32(F32(K_1E7 - grip) * K_9P9998E8)
        if k < 0.0:
            k = F32(0.0)
        k = F32(F32(k * F32(0.1)) + F32(k * F32(0.1)))
        return float(grip), "hi", float(k), float(F32(1.0) - k)
    k = F32(F32(K_32768 - grip) * K_3P0518E5)
    if k < F32(0.1):
        k = F32(0.1)
    kv = k
    if k < F32(0.5):
        k = F32(0.5)
    return float(grip), "lo", float(kv), float(F32(1.0) - k)


def replay_l60(fr, wheels):
    """l_60 = sum(ld4 * le4) over the wheels block #4 fired on (Integrate2.cpp:450).
    Replayed, since the record carries the inputs but not the accumulator."""
    t = 0.0
    for wl in wheels:
        if wl.get("pm3", 1) == 0:
            continue
        got, why = replay(fr, wl)
        if why:
            continue
        t += float(got["ld4"]) * float(got["le4"])
    return t if t > 0.0 else None


def selfcheck4(rows, min_speed, steer_min):
    """clamp6() transcription vs A6a's own logged grip / k. Port only."""
    eg, ek, ea, n, arms = [], [], [], 0, set()
    for r in rows:
        if float(r["snap"]["sp"]) < min_speed or r["steer"] < steer_min:
            continue
        A = r["c6act"]
        if not A["ran"]:
            continue
        g, arm, kv, kav = clamp6(A["l60"], A["m18c"], r["c6"]["m2c"], r["c6"]["m34"],
                                 r["c6"]["tid"], r["c6"]["d00"], A["speed"])
        n += 1
        arms.add((arm, "hi" if A["arm"] == 1 else "lo"))
        eg.append(relerr(g, A["grip"]))
        ek.append(relerr(kv, A["kvel"]))
        ea.append(relerr(kav, A["kav"]))
    print("\n--- SELF-CHECK 4  clamp6() transcription vs A6a's own logged grip/k ---")
    print(f"    samples={n}  arm agreement={'OK' if all(a == b for a, b in arms) else 'MISMATCH ' + str(arms)}")
    if not n:
        return
    print(f"    grip  median {med(eg):.3e}  max {max(eg):.3e}")
    print(f"    k_vel median {med(ek):.3e}  max {max(ek):.3e}")
    print(f"    1-k   median {med(ea):.3e}  max {max(ea):.3e}")


def heading(rows, label, getbf, getvel, getidx, getsp, bands):
    """The two halves of the scored metric, separated: how fast the BODY heading
    (+0x9d4/+0x9dc) turns per frame, how fast the VELOCITY heading (+0x9b0/+0x9b8)
    turns per frame, and the standing angle between them (= a8_slip_axis's
    `slip vs fwd`). Record fields on both sides; no replay, no fit."""
    print(f"\n=== HEADINGS  {label} ===")
    print(f"    {'band':<12} {'n':>5} {'d(bodyH)/fr':>12} {'d(velH)/fr':>12} "
          f"{'ratio b/v':>10} {'beta':>8} {'|vel|':>9}")
    out = {}
    for lo, hi in bands:
        db, dv, be, vm = [], [], [], []
        for k in range(len(rows) - 1):
            if not (lo <= getsp(rows[k]) < hi):
                continue
            if getidx(rows[k + 1]) != getidx(rows[k]) + 1:
                continue
            b0, b1 = getbf(rows[k]), getbf(rows[k + 1])
            v0, v1 = getvel(rows[k]), getvel(rows[k + 1])
            n0 = math.hypot(float(v0[0]), float(v0[2]))
            if n0 < 1.0:
                continue
            bh0 = math.atan2(float(b0[2]), float(b0[0]))
            bh1 = math.atan2(float(b1[2]), float(b1[0]))
            vh0 = math.atan2(float(v0[2]), float(v0[0]))
            vh1 = math.atan2(float(v1[2]), float(v1[0]))
            wrap_ = lambda a: (a + math.pi) % (2 * math.pi) - math.pi
            db.append(wrap_(bh1 - bh0))
            dv.append(wrap_(vh1 - vh0))
            be.append(abs(wrap_(vh0 - bh0)))
            vm.append(n0)
        if len(db) < 8:
            print(f"    {f'{lo}-{hi}':<12} {len(db):>5}")
            continue
        r = med(db) / med(dv) if med(dv) else float('nan')
        print(f"    {f'{lo}-{hi}':<12} {len(db):>5} {med(db):>+12.5f} {med(dv):>+12.5f} "
              f"{r:>10.4f} {med(be):>8.4f} {med(vm):>9.1f}")
        out[(lo, hi)] = dict(db=med(db), dv=med(dv), beta=med(be))
    return out


def clamp6_table(rows, label, getl60, getfields, getsp, bands):
    print(f"\n=== CLAMP #6  {label}  (Integrate2.cpp:635-722; the direct slip bleed) ===")
    print(f"    {'band':<12} {'n':>5} {'l_60':>10} {'+0x18c':>9} {'+0x2c':>6} {'+0x34':>6} "
          f"{'+0x1f0':>11} {'+0xd00':>7} {'speed':>8} {'grip':>12} {'arm':>4} "
          f"{'k_vel':>8} {'1-k_av':>8}")
    out = {}
    for lo, hi in bands:
        L, M, S, G, K, A, arms = [], [], [], [], [], [], []
        f2c = f34 = ftid = fd00 = None
        for r in rows:
            sp = getsp(r)
            if not (lo <= sp < hi):
                continue
            l60 = getl60(r)
            if l60 is None:
                continue
            fl = getfields(r)
            g, arm, kv, kav = clamp6(l60, fl["m18c"], fl["m2c"], fl["m34"],
                                     fl["tid"], fl["d00"], sp)
            L.append(l60); M.append(fl["m18c"]); S.append(sp)
            G.append(g); K.append(kv); A.append(kav); arms.append(arm)
            f2c, f34, ftid, fd00 = fl["m2c"], fl["m34"], fl["tid"], fl["d00"]
        if len(L) < 8:
            print(f"    {f'{lo}-{hi}':<12} {len(L):>5}")
            continue
        arm = max(set(arms), key=arms.count)
        print(f"    {f'{lo}-{hi}':<12} {len(L):>5} {med(L):>10.1f} {med(M):>9.4g} "
              f"{f2c:>6} {f34:>6} {ftid:>11} {fd00:>7} {med(S):>8.1f} {med(G):>12.5g} "
              f"{arm:>4} {med(K):>8.5f} {med(A):>8.5f}")
        out[(lo, hi)] = dict(l60=med(L), grip=med(G), k=med(K), kav=med(A), arm=arm)
    return out


def main():
    ap = argparse.ArgumentParser(description=__doc__.split("\n")[0])
    ap.add_argument("--port", help="a6a dump written by MASHED_A6ADUMP")
    ap.add_argument("--orig", help="MSD1 capture")
    ap.add_argument("--orig-susp", type=float,
                    help="the ORIGINAL's g_suspScale (_DAT_0088e5f0). MEASURE it.")
    ap.add_argument("--min-speed", type=float, default=1500.0)
    ap.add_argument("--orig-steer-min", type=float, default=33.0)
    ap.add_argument("--port-steer-min", type=float, default=0.9)
    ap.add_argument("--lag", type=int, action="append", default=[],
                    help="frames between the replayed record snapshot and the force it "
                         "is compared against; repeatable. See cross(). Default 0 and 1.")
    a = ap.parse_args()
    if not a.lag:
        a.lag = [0, 1]

    if a.port:
        rows = load_port(a.port)
        print(f"PORT {a.port}: {len(rows)} frames")
        w1 = selfcheck(rows, "act", a.min_speed, a.port_steer_min,
                       "1 (exact: A6a's own inputs)")
        w2 = selfcheck(rows, "snap", a.min_speed, a.port_steer_min,
                       "2 (phase: the render-tick snapshot the .msd sees)")
        invariant(rows, a.min_speed, a.port_steer_min)
        if w1 is not None:
            print(f"\n    SELF-CHECK 1 worst relative error over all quantities: {w1:.3e}"
                  f"   -> {'SOUND' if w1 < 1e-5 else 'REPLAY IS WRONG -- STOP'}")
        if w2 is not None:
            print(f"    SELF-CHECK 2 worst relative error (phase cost):        {w2:.3e}")
        sel = [r for r in rows
               if float(r["snap"]["sp"]) >= a.min_speed and r["steer"] >= a.port_steer_min]
        for lag in a.lag:
            cross(sel, f"PORT {a.port} (snapshot phase)",
                  lambda r: r["snap"], lambda r, w: r["w"][w]["snap"],
                  lambda r, w: r["w"][w]["snap"]["F"], len(sel), lag=lag)
        geom(sel, f"PORT {a.port}", lambda r, w: r["w"][w]["snap"],
             lambda r: r["snap"]["bf"], lambda r: float(r["snap"]["sp"]), BANDS)
        selfcheck4(rows, a.min_speed, a.port_steer_min)
        heading(rows, f"PORT {a.port}", lambda r: r["snap"]["bf"],
                lambda r: r["snap"]["vel"], lambda r: r["f"],
                lambda r: float(r["snap"]["sp"]), BANDS)
        clamp6_table(sel, f"PORT {a.port} (replayed l_60 from the snapshot)",
                     lambda r: replay_l60(r["snap"], [r["w"][w]["snap"] for w in range(4)]),
                     lambda r: r["c6"], lambda r: float(r["snap"]["sp"]), BANDS)
        clamp6_table(sel, f"PORT {a.port} (A6a's OWN l_60, ground truth)",
                     lambda r: r["c6act"]["l60"],
                     lambda r: r["c6"], lambda r: float(r["snap"]["sp"]), BANDS)
        angular(rows, f"PORT {a.port}",
                lambda r: [r["w"][w]["snap"] for w in range(4)],
                lambda r: r["snap"]["av"], lambda r: r["f"], BANDS,
                lambda r: float(r["snap"]["sp"]))

    if a.orig:
        # g_suspScale is NOT in the 0xd04 record. It is not guessed here: block #4 is
        # LINEAR in it, so it is RECOVERED as the suspEff column and its consistency
        # across wheels is part of the test. --orig-susp only sets the reference value
        # the ratio column is expressed against.
        susp = 1.0 if a.orig_susp is None else a.orig_susp
        orows = orig_rows(a.orig, susp, a.min_speed, a.orig_steer_min)
        print(f"\nORIGINAL {a.orig}: {len(orows)} frames, g_suspScale assumed={susp}"
              f"  (suspEff column = the RECOVERED value)")
        for lag in a.lag:
            cross(orows, f"ORIGINAL {a.orig}",
                  lambda r: r["fr"], lambda r, w: r["w"][w],
                  lambda r, w: r["w"][w]["F"], len(orows), lag=lag)
        geom(orows, f"ORIGINAL {a.orig}", lambda r, w: r["w"][w],
             lambda r: r["fr"]["bf"], lambda r: r["sp"], BANDS)
        clamp6_table(orows, f"ORIGINAL {a.orig} (replayed l_60 from the record)",
                     lambda r: replay_l60(r["fr"], r["w"]),
                     lambda r: r["c6"], lambda r: r["sp"], BANDS)
        oall = orig_rows_all(a.orig)
        heading(oall, f"ORIGINAL {a.orig}", lambda r: r["bf"], lambda r: r["vel"],
                lambda r: r["idx"], lambda r: r["sp"], BANDS)
        angular(oall, f"ORIGINAL {a.orig}",
                lambda r: r["w"], lambda r: r["av"], lambda r: r["idx"], BANDS,
                lambda r: r["sp"])


if __name__ == "__main__":
    main()
