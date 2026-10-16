"""Build the path1 / path2 vectors for registry key 'ai_powerup_seek' (0x00414f00, cache_setter_observe + fold_ret) from the
original-side capture O3.msd.seek.csv (Warzone, rule 8), plus contrived rows for the branches the capture does not reach.

    py -3.12 make_vectors.py > vectors.py.txt

Seeds per vector (every seeded VA is snapshotted and restored by the handler):
  0x00684de0 = held float bits, 0x0088ff50 + v*4 = predicate word, 0x00684dac = S (S + 0x30 = the target row),
  record v: 0x00881f48 + v*0xd04 = 0 (ring index), 0x00881ef8/efc/f00 + v*0xd04 = own row x/y/z,
  car 0: 0x008815a4 = alive word, 0x00881ef8 / 0x00881f00 = car 0 x / z (v != 0 only; for v == 0 the own row is car 0's),
  cand 0x007f1040 / 0x007f1044 = 0xaaaaaaaa sentinel, observed after the call with EAX folded in front.
Args: (0, 0x007f1050, 0x007f1040, v); param_1 / param_2 are never read by the body.
"""
import csv
import struct
from pathlib import Path

HERE = Path(__file__).resolve().parent
S = 0x007f1000
CAND = 0x007f1040
STRIDE = 0xd04


def fb(x: float) -> int:
    return struct.unpack("<I", struct.pack("<f", x))[0]


def bf(u: int) -> float:
    return struct.unpack("<f", struct.pack("<I", u & 0xFFFFFFFF))[0]


def vec(de0, ff50v, t, own, alive0, car0, v, note):
    seed = [(0x00684de0, de0), (0x0088ff50 + v * 4, ff50v), (0x00684dac, S),
            (S + 0x30, t[0]), (S + 0x34, t[1]), (S + 0x38, t[2]),
            (0x00881f48 + v * STRIDE, 0),
            (0x00881ef8 + v * STRIDE, own[0]), (0x00881efc + v * STRIDE, own[1]), (0x00881f00 + v * STRIDE, own[2]),
            (0x008815a4, alive0), (CAND, 0xAAAAAAAA), (CAND + 4, 0xAAAAAAAA)]
    if v != 0:
        seed += [(0x00881f48, 0), (0x00881ef8, car0[0]), (0x00881f00, car0[1])]
    s = ", ".join("{'addr': '0x%08x', 'val': 0x%08x}" % (a, x & 0xFFFFFFFF) for a, x in seed)
    return ("            # %s\n            {'seed': [%s],\n             'args': [0, 0x007f1050, 0x%08x, %d], "
            "'obs': ['0x%08x', '0x%08x']},") % (note, s, CAND, v, CAND, CAND + 4)


def main() -> None:
    rows = list(csv.DictReader((HERE / "O3.msd.seek.csv").open()))
    I = lambda r, k: int(r[k]) & 0xFFFFFFFF
    out = []
    r1 = [r for r in rows if r["ret"] == "1"]
    for r in r1:
        out.append(vec(I(r, "de0"), I(r, "ff50v"), (I(r, "tx"), I(r, "ty"), I(r, "tz")), (I(r, "ox"), I(r, "oy"), I(r, "oz")),
                       I(r, "alive0"), (I(r, "x0"), I(r, "z0")), int(r["v"]),
                       "O3 n=%s v=%s: natural ret=1 (expect cand = target x/z)" % (r["n"], r["v"])))
    g2 = [r for r in rows if r["ret"] == "0" and 0.5 <= bf(I(r, "de0")) <= 30.0 and I(r, "ff50v") == 0][:8]
    for r in g2:
        out.append(vec(I(r, "de0"), 0, (I(r, "tx"), I(r, "ty"), I(r, "tz")), (I(r, "ox"), I(r, "oy"), I(r, "oz")),
                       I(r, "alive0"), (I(r, "x0"), I(r, "z0")), int(r["v"]),
                       "O3 n=%s v=%s: natural gate-2 fail (predicate 0), expect ret 0, sentinel kept" % (r["n"], r["v"])))
    g1 = [r for r in rows if r["ret"] == "0" and I(r, "de0") == 0][:4]
    for r in g1:
        out.append(vec(0, 1, (I(r, "tx"), I(r, "ty"), I(r, "tz")), (I(r, "ox"), I(r, "oy"), I(r, "oz")),
                       I(r, "alive0"), (I(r, "x0"), I(r, "z0")), int(r["v"]),
                       "O3 n=%s v=%s: held 0.0 < 0.5 with the predicate FORCED 1, expect ret 0" % (r["n"], r["v"])))
    # contrived rows on a real ret=1 row's target
    b = r1[0]
    t = (I(b, "tx"), I(b, "ty"), I(b, "tz"))
    tx, ty, tz = bf(t[0]), bf(t[1]), bf(t[2])
    car0 = (fb(tx - 7.25), fb(tz + 3.5))
    near = lambda dx, dz: (fb(tx + dx), fb(ty), fb(tz + dz))
    h = fb(1.0)
    out.append(vec(h, 1, t, near(0.5, 0.5), 1, car0, 2, "CONTRIVED |d| ~0.707 in (0.25, 1.5], car 0 alive: expect cand = car 0 x/z"))
    out.append(vec(h, 1, t, near(0.5, 0.5), 0, car0, 2, "CONTRIVED |d| ~0.707, car 0 word 0: expect ret 0"))
    out.append(vec(h, 1, t, near(0.1, 0.1), 1, car0, 2, "CONTRIVED |d| ~0.141 <= 0.25: expect ret 0"))
    out.append(vec(h, 1, t, near(1.5, 0.0), 1, car0, 3, "CONTRIVED |d| = LUT sqrt(2.25), the 1.5 boundary"))
    out.append(vec(h, 1, t, near(0.25, 0.0), 1, car0, 1, "CONTRIVED |d| = LUT sqrt(0.0625), the 0.25 boundary"))
    out.append(vec(h, 1, t, near(3.0, -4.0), 1, car0, 1, "CONTRIVED |d| = 5: expect cand = target x/z"))
    out.append(vec(fb(0.5), 1, t, near(3.0, -4.0), 1, car0, 1, "CONTRIVED held exactly 0.5 passes (not <)"))
    out.append(vec(0x3EFFFFFF, 1, t, near(3.0, -4.0), 1, car0, 1, "CONTRIVED held one ulp below 0.5: expect ret 0"))
    out.append(vec(fb(30.0), 1, t, near(3.0, -4.0), 1, car0, 1, "CONTRIVED held exactly 30.0 passes (not >)"))
    out.append(vec(0x41F00001, 1, t, near(3.0, -4.0), 1, car0, 1, "CONTRIVED held one ulp above 30.0: expect ret 0"))
    out.append(vec(0x7FC00000, 1, t, near(3.0, -4.0), 1, car0, 1, "CONTRIVED held NaN: passes both x87 compares (jnp / jne)"))
    out.append(vec(h, 1, t, near(0.5, 0.5), 1, car0, 0, "CONTRIVED v = 0 (own row is car 0's), |d| ~0.707: expect cand = own x/z"))
    print("\n".join(out))
    print("# %d vectors: %d natural ret=1, %d natural gate-2, %d gate-1, 12 contrived" % (len(out), len(r1), len(g2), len(g1)))


if __name__ == "__main__":
    main()
