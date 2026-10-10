"""D-11072 P3 pre-registration: OPEN-LOOP model of FUN_00414a70 (+ FUN_00414300) on a port capture.

Prediction tool only (PREREG_P3.md section 3). float32 arithmetic from the decomp
(verify/d3_modes37_20261009/decomp_chain.txt), NOT a bit-exact transcription: the exe port is the
naked x87 body; this model only predicts what that body returns on the inputs a knob-ON capture
WITHOUT P3 already carries. Open-loop: once a 1/2 return changes a car's control in the real run,
the trajectories diverge, so these counts are a prediction, not a reference.

Inputs per AI call (v, frame), from <name>.step.csv / <name>.gates.csv / <name>.ptrace.log:
  own xz            step own_x/own_z            (FUN_0046d4a0 +0x30/+0x38)
  lookahead         step look_x/look_z          (param_2 = &local_24 from FUN_004161e0)
  own fwd           step rec_9d4/rec_9dc        (FUN_0046d510: (0,0,1) by the +0x928 matrix)
  own speed         step rec_9e4                (FUN_0046d6d0, 0x00881f84 = record +0x9e4)
  other xz          gates ptr_x/ptr_z           (FUN_0046d4a0 for the other car)
  progress          gates prog_96e8             (FUN_00408a50, 0x008a96e8)
  slot state        gates slot_state            (FUN_0040e370 / FUN_0040e470)
  other speed       step rec_9e4 (AI) / ptrace v9e4 at f = frame + 1 (player) [approximation]
  node count        30                          (DAT_0066d6d8, Training)
Usage: py -3.12 model_14a70.py <dir> <name> [--n 220]
"""
import csv, sys, collections
import numpy as np

F = np.float32
K0, K1500, K15, K1, K2 = F(0.0), F(1500.0), F(1.5), F(1.0), F(2.0)


def f414300(own, tgt, oth, r, fwd):
    # 0x00414300; returns (ret, cand)
    ax, az = own; bx, bz = tgt; px, pz = oth
    if F(ax - bx) == K0:
        fx, fz = ax, pz
    else:
        m = F(F(az - bz) / F(ax - bx))
        if m == K0:
            fx, fz = px, az
        else:
            c = F(az - F(m * ax))
            q = F(-F(K1 / m))
            fx = F(F(F(pz - F(q * px)) - c) / F(m - q))
            fz = F(F(m * fx) + c)
    dx = F(px - fx); dz = F(pz - fz)
    d2 = F(F(dx * dx) + F(dz * dz))
    if d2 < F(r * r):
        if K0 < F(F(fwd[1] * F(pz - az)) + F(fwd[0] * F(px - ax))):
            if F(0.001) < d2:
                inv = F(K1 / F(np.sqrt(d2)))
                return 1, (F(px - F(F(dx * inv) * r)), F(pz - F(F(inv * dz) * r)))
            return 1, (F(px - F(dx * r)), F(pz - F(dz * r)))
    return 0, None


def f414a70(v, own, tgt, fwd, sp, oxz, prog, st, osp, nodes=30):
    best, bd = -1, K2
    for i in range(4):
        if i == v or st[i] == 0:
            continue
        d = F(prog[i] - prog[v])
        if d <= K0 and F(float(int(F(5 - nodes)))) <= d:
            continue
        if d < bd:
            best, bd = i, d
    if best == -1:
        return 0
    tx, tz = oxz[best]
    dist = F(np.sqrt(F(F(F(tx - own[0]) ** 2) + F(F(tz - own[1]) ** 2))))
    if K1500 < F(sp - osp[best]) or sp < K1500:
        r, _ = f414300(own, tgt, (tx, tz), K1, fwd)
        if r:
            return 1
    if st[best] != 1 and dist < K15:
        return 2
    return 0


def main(d, name, n=220):
    step = list(csv.DictReader(open(f"{d}/{name}.step.csv", newline="")))
    gates = {(int(r["frame"]), int(r["v"])): r for r in csv.DictReader(open(f"{d}/{name}.gates.csv", newline=""))}
    psp = {}
    try:
        for ln in open(f"{d}/{name}.ptrace.log"):
            if ln.startswith("f=") and " v9e4=" in ln:
                psp[int(ln.split()[0][2:])] = F(float(ln.split(" v9e4=")[1].split()[0]))
    except FileNotFoundError:
        pass
    aisp = {(int(r["frame"]), int(r["v"])): F(float(r["rec_9e4"])) for r in step}
    out = collections.defaultdict(list)
    for r in step:
        fr, v = int(r["frame"]), int(r["v"])
        g = [gates.get((fr, i)) for i in range(4)]
        if any(x is None for x in g):
            out[v].append((r, None)); continue
        oxz = [(F(float(x["ptr_x"])), F(float(x["ptr_z"]))) for x in g]
        prog = [F(float(x["prog_96e8"])) for x in g]
        st = [int(x["slot_state"]) for x in g]
        osp = [psp.get(fr + 1, F(0.0))] + [aisp.get((fr, i), F(0.0)) for i in (1, 2, 3)]
        ret = f414a70(v, (F(float(r["own_x"])), F(float(r["own_z"]))),
                      (F(float(r["look_x"])), F(float(r["look_z"]))),
                      (F(float(r["rec_9d4"])), F(float(r["rec_9dc"]))),
                      F(float(r["rec_9e4"])), oxz, prog, st, osp)
        out[v].append((r, ret))
    for v in sorted(out):
        rows = out[v]
        i0 = next((i for i, (x, _) in enumerate(rows) if int(x["c4"]) != 0), None)
        if i0 is None:
            print(f"v{v}: no window"); continue
        w = rows[i0:i0 + n]
        c = collections.Counter(ret for _, ret in w)
        whole = collections.Counter(ret for _, ret in rows)
        print(f"v{v}: window start frame {w[0][0]['frame']} n={len(w)}  ret1={c[1]} ret2={c[2]} ret0={c[0]} "
              f"nogates={c[None]}   whole run: ret1={whole[1]} ret2={whole[2]} calls={len(rows)}")


if __name__ == "__main__":
    a = sys.argv[1:]
    n = int(a[a.index("--n") + 1]) if "--n" in a else 220
    main(a[0], a[1], n)
