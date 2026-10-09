"""FUN_00414180 port scorer (PREREG_414180.md G-DRAW + the ring part of G-EXIT). Read-only.

usage: py -3.12 score_p414.py <arm> [<arm> ...]   (run from verify/d3_gatefire_20261008)

For every '[substate] FUN_00414180' line in <arm>.log: re-simulate the verbatim FUN_00534870 ring
(p414_cdpred.py law) to the logged draws_before, take 4 draws for row (1,5), and compare with the
logged cd. With the MASHED_SUBSTATE_CD override the G-EXIT prediction uses
the override value (it is written after the body). Round 1 must also equal the registered draws_before=0, cd=3,1,4,2, pre368=0, pre370=0.
G-EXIT per AI car (1000 + 100*cd / 1) is scored by score_leg1b_gates.py's first-state-6 rows,
printed here next to the predicted values.
"""
import csv
import re
import sys
import numpy as np

M = 0xffffffff


def ring_draws(n_skip, n):
    ring = [0x9a319039]
    for _ in range(1, 31):
        ring.append((ring[-1] * 0x41c64e6d + 0x3039) & M)
    st = {"p": 3, "q": 0}

    def nxt():
        p, q = st["p"], st["q"]
        ring[p] = (ring[p] + ring[q]) & M
        u = ring[p] >> 1
        p += 1
        if p >= 31:
            st["p"], st["q"] = 0, q + 1
            return u
        q += 1
        st["p"], st["q"] = p, (0 if q >= 31 else q)
        return u

    for _ in range(0x136 + n_skip):
        nxt()
    f = np.float32
    return [int(f(f(f(4) * f(nxt() & 0x7fffffff)) * f(4.656612873e-10)) + f(1)) for _ in range(n)]


LINE = re.compile(r"FUN_00414180 draws_before=(\d+) pre368=(-?\d+) pre370=0x([0-9a-f]+) idx=(-?\d+) "
                  r"cd=(-?\d+),(-?\d+),(-?\d+),(-?\d+) rec0=(-?\d+) fd0=(-?\d+)")
ok_all = True
for arm in sys.argv[1:]:
    print("==", arm)
    lines = [LINE.search(l) for l in open(arm + ".log", encoding="utf-8", errors="replace")]
    lines = [m for m in lines if m]
    text = open(arm + ".log", encoding="utf-8", errors="replace").read()
    ov = re.search(r"MASHED_SUBSTATE_CD=(-?\d+) seeded", text)   # override applied AFTER the body
    segs = []
    with open(arm + ".inputs.csv") as fh:
        rows = list(csv.DictReader(fh))
    prev_pre = False
    for i, r in enumerate(rows):
        pre = float(r["countdown"]) > 0
        if prev_pre and not pre:
            segs.append(r)
        prev_pre = pre
    for k, m in enumerate(lines):
        n, p368, p370, idx = int(m[1]), int(m[2]), int(m[3], 16), int(m[4])
        cd = [int(m[i]) for i in range(5, 9)]
        # draws_before counts RwRandomOpen's 310 discards too (they go through RwRandomNext), so
        # once the ring is open the real offset past the discards is n - 310 (0 = not yet opened).
        sim = ring_draws(n - 0x136 if n >= 0x136 else n, 4)
        ok = (sim == cd) and idx == 2 and int(m[9]) == 2 and int(m[10]) == 0
        if k == 0:
            ok = ok and n == 0 and cd == [3, 1, 4, 2] and p368 == 0 and p370 == 0
        ok_all &= ok
        exit_row = segs[k] if k < len(segs) else None
        got = ([exit_row["bf4r%d" % v] + "/" + exit_row["bf8r%d" % v] for v in range(4)]
               if exit_row else "no state-6 row")
        eff = [int(ov[1])] * 4 if ov else cd
        pred = ["0/0"] + ["%d/1" % (1000 + 100 * c) for c in eff[1:]]
        ex_ok = exit_row is not None and got == pred
        ok_all &= ex_ok
        print("  round %d: draws_before=%d pre368=%d pre370=0x%08x idx=%d cd=%s ring=%s  %s | "
              "G-EXIT pred %s got %s  %s" % (k + 1, n, p368, p370, idx, cd, sim, "OK" if ok else "FAIL",
                                             pred, got, "OK" if ex_ok else "FAIL"))
print("ALL", "PASS" if ok_all else "FAIL")
