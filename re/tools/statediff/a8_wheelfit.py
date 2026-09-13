#!/usr/bin/env python3
"""A8 slip-deficit: PER-WHEEL lateral coefficient, fitted per side from record fields.

Model (Integrate2.cpp block #4, transcribed from A6a FUN_00467650):
    F_w = a_w * lat_w + b_w * axis_w
    lat_w = u - (u . bodyfwd) * axis_w      u = unit velocity, axis_w = p[0x1f..0x21]
    a_w   = the lateral coefficient; the port computes it as
            lbc = p[0x15] * p[0x1b] * g_suspScale * min(le4, 1024) * 0.0009766
Both a_w and b_w are fitted by 2-D (x,z) least squares per wheel and per speed band,
with the median relative residual, so a wheel whose force is NOT a constant times its
lateral vector shows up as a large residual and a band-dependent a_w.

ORIGINAL: MSD1 capture; per-wheel force +0x70/+0x78, axis +0x7c/+0x84, base
          0x1a4 + w*0xc4; p[0x1b] at +0x6c.
PORT:     motion_diag.log with the [A8-ORIENT] fields wf=[..] wax=[..] wle4=[..] wld4=[..]
          (VehiclePhysicsRun.cpp, 2026-09-13). Older logs lack them and are refused.
Regime: speed >= --min-speed (1500), grounded, steer held (per-side threshold),
consecutive frames not required (this is per-frame, not an integration step).
Read-only. Does not execute the game.
"""
import argparse, math, re, struct, sys, os
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import a8_momentum as m  # noqa: E402

WB = [0x1a4 + w * 0xc4 for w in range(4)]
BANDS = ((1500, 1800), (1800, 2100), (2100, 2400), (2400, 2800))


def orig_frames(path, min_speed, steer_min):
    _, _, frames = m.load_msd(path)
    f = lambda p, o: struct.unpack_from("<f", p, o)[0]
    out = []
    for idx in sorted(frames):
        p = frames[idx]
        sp, st, g = f(p, 0x9e4), f(p, 0x1a8), f(p, 0x9e0)
        if sp < min_speed or (steer_min is not None and st < steer_min) or g < 3.5:
            continue
        v = (f(p, 0x9b0), f(p, 0x9b4), f(p, 0x9b8)); n = math.sqrt(sum(c * c for c in v))
        if n < 1e-3:
            continue
        u = tuple(c / n for c in v); bf = (f(p, 0x9d4), f(p, 0x9d8), f(p, 0x9dc))
        lc0 = sum(a * b for a, b in zip(u, bf))
        wheels = []
        for b in WB:
            ax = (f(p, b + 0x7c), f(p, b + 0x80), f(p, b + 0x84))
            lat = tuple(u[i] - lc0 * ax[i] for i in range(3))
            F = (f(p, b + 0x70), f(p, b + 0x74), f(p, b + 0x78))
            wheels.append(dict(lat=lat, ax=ax, F=F, p1b=f(p, b + 0x6c), le4=float("nan"), ld4=math.sqrt(sum(c * c for c in lat))))
        out.append(dict(sp=sp, wheels=wheels))
    return out


RE_WF = re.compile(r"wf=\[([^\]]*)\] wax=\[([^\]]*)\] wle4=\[([^\]]*)\] wld4=\[([^\]]*)\]")
RE_KV = re.compile(r"\b(sp|horiz|velH|bodyH|steer|gnd|reseed)=([-+0-9.eE]+)")
RE_W1B = re.compile(r"w1b=\[([^\]]*)\]")


def port_frames(path, min_speed, steer_min):
    out = []; lacking = 0
    for line in open(path, errors="replace"):
        if "velH=" not in line:
            continue
        mw = RE_WF.search(line)
        if not mw:
            lacking += 1; continue
        kv = {k: float(v) for k, v in RE_KV.findall(line)}
        if kv.get("horiz", 0) < min_speed or (steer_min is not None and kv.get("steer", 0) < steer_min) or kv.get("gnd", 0) < 3.5 or int(kv.get("reseed", 0)):
            continue
        wf = [float(x) for x in mw.group(1).split(",")]; wax = [float(x) for x in mw.group(2).split(",")]
        wle4 = [float(x) for x in mw.group(3).split(",")]; wld4 = [float(x) for x in mw.group(4).split(",")]
        w1b = [float(x) for x in RE_W1B.search(line).group(1).split(",")]
        vh = kv["velH"]; u = (math.cos(vh), 0.0, math.sin(vh)); bh = kv["bodyH"]; bf = (math.cos(bh), 0.0, math.sin(bh))
        lc0 = sum(a * b for a, b in zip(u, bf))
        wheels = []
        for w in range(4):
            ax = (wax[2 * w], 0.0, wax[2 * w + 1])
            lat = tuple(u[i] - lc0 * ax[i] for i in range(3))
            F = (wf[2 * w], 0.0, wf[2 * w + 1])
            wheels.append(dict(lat=lat, ax=ax, F=F, p1b=w1b[w], le4=wle4[w], ld4=wld4[w]))
        out.append(dict(sp=kv["horiz"], wheels=wheels))
    if lacking and not out:
        sys.exit(f"{path}: {lacking} lines without the [A8-ORIENT] wf/wax fields -- an older log")
    return out


def fit(rows, w):
    S = [[0.0, 0.0], [0.0, 0.0]]; T = [0.0, 0.0]
    for r in rows:
        d = r["wheels"][w]
        for i in (0, 2):
            S[0][0] += d["lat"][i] ** 2; S[0][1] += d["lat"][i] * d["ax"][i]; S[1][1] += d["ax"][i] ** 2
            T[0] += d["lat"][i] * d["F"][i]; T[1] += d["ax"][i] * d["F"][i]
    S[1][0] = S[0][1]
    det = S[0][0] * S[1][1] - S[0][1] * S[1][0]
    if abs(det) < 1e-12:
        return float("nan"), float("nan"), float("nan")
    a = (T[0] * S[1][1] - S[0][1] * T[1]) / det; b = (S[0][0] * T[1] - S[1][0] * T[0]) / det
    res = []
    for r in rows:
        d = r["wheels"][w]
        pr = [a * d["lat"][i] + b * d["ax"][i] for i in range(3)]
        fm = math.hypot(d["F"][0], d["F"][2])
        if fm > 1:
            res.append(math.hypot(d["F"][0] - pr[0], d["F"][2] - pr[2]) / fm)
    res.sort()
    return a, b, (res[len(res) // 2] if res else float("nan"))


def med(v):
    v = sorted(v); return v[len(v) // 2] if v else float("nan")


def report(rows, label):
    print(f"\n=== {label}: {len(rows)} frames ===")
    print(f"  {'wheel':<6} {'band':<10} {'n':>4} {'a(lat)':>9} {'b(axis)':>8} {'resid':>6} {'p1b':>7} {'le4':>7} {'ld4':>6} {'a/p1b':>7}")
    res = {}
    for w in range(4):
        a, b, r = fit(rows, w)
        p1b = med([x["wheels"][w]["p1b"] for x in rows])
        print(f"  w{w:<5} {'all':<10} {len(rows):>4} {a:>9.0f} {b:>8.0f} {r:>6.3f} {p1b:>7.1f} {med([x['wheels'][w]['le4'] for x in rows]):>7.1f} {med([x['wheels'][w]['ld4'] for x in rows]):>6.3f} {a/p1b if p1b else float('nan'):>7.1f}")
        res[(w, 'all')] = a
        for lo, hi in BANDS:
            sel = [x for x in rows if lo <= x["sp"] < hi]
            if len(sel) < 20:
                continue
            a2, b2, r2 = fit(sel, w)
            print(f"  {'':<6} {f'{lo}-{hi}':<10} {len(sel):>4} {a2:>9.0f} {b2:>8.0f} {r2:>6.3f} {'':>7} {med([x['wheels'][w]['le4'] for x in sel]):>7.1f} {med([x['wheels'][w]['ld4'] for x in sel]):>6.3f}")
            res[(w, (lo, hi))] = a2
    return res


def main():
    ap = argparse.ArgumentParser(description=__doc__.split("\n")[0])
    ap.add_argument("--orig"); ap.add_argument("--port")
    ap.add_argument("--min-speed", type=float, default=1500.0)
    ap.add_argument("--orig-steer-min", type=float, default=33.0)
    ap.add_argument("--port-steer-min", type=float, default=0.9)
    a = ap.parse_args()
    o = p = {}
    if a.orig:
        o = report(orig_frames(a.orig, a.min_speed, a.orig_steer_min), f"ORIGINAL {a.orig}")
    if a.port:
        p = report(port_frames(a.port, a.min_speed, a.port_steer_min), f"PORT     {a.port}")
    if o and p:
        print("\n=== lateral coefficient a_w, port / original ===")
        for w in range(4):
            cells = []
            for key in ['all'] + list(BANDS):
                if (w, key) in o and (w, key) in p and abs(o[(w, key)]) > 1:
                    cells.append(f"{str(key):>12}: {p[(w, key)] / o[(w, key)]:.3f}")
            print(f"  w{w}: " + "  ".join(cells))


if __name__ == "__main__":
    main()
