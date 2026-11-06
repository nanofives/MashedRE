"""W-C4 / W-HOLD scorer for D4-DC-77b0-ST5-C4ACCEL (PREREG in this folder).

Reads the [st5hold] lines of a MASHED_ST5_HOLD_DIAG arm log (B_ST5L2D_K0.log) and checks, per non-exit call:
  W-C4   blk byte 4 == 0xff iff c29b8 < T(cd), T = (cd + 0x40) * 100 (cd >= 0) or (0x4a - cd) * 100 (cd < 0)
         (FUN_00418560 0x004185b5..0x004185dd)
  W-HOLD each AI car's position identical on calls 2..last non-exit call, equal to ST5-HOLD's held x/z.
Prints per-car 0xff counts and the bf4 seen on the last call. Usage: py -3.12 score_c4.py <arm.log>
"""
import re
import sys

HELD = {1: (-1.10121727, -0.901938975), 2: (1.1367228, 0.20244953), 3: (-0.465431273, 1.29945505)}
PAT = re.compile(r"\[st5hold\] call=(\d+) car=(\d+) slot=\d+ blk=([0-9a-f]{16}) pos=\(([^,]+),([^,]+),([^)]+)\)"
                 r" cd=(-?\d+) c29b8=(-?\d+) bf4=(-?\d+)")


def thr(cd):
    return (cd + 0x40) * 100 if cd >= 0 else (0x4a - cd) * 100


def main(path):
    rows = []
    with open(path, errors="replace") as f:
        for ln in f:
            m = PAT.search(ln)
            if m:
                rows.append((int(m[1]), int(m[2]), bytes.fromhex(m[3]), float(m[4]), float(m[6]),
                             int(m[7]), int(m[8]), int(m[9])))
    if not rows:
        print("no [st5hold] lines with cd/c29b8/bf4 (old exe?)")
        return 2
    last = max(r[0] for r in rows)
    # The exit call runs at DAT_0063ba8c = 6 (leg 1 stores 6 at 0x004104e3): excluded from W-C4 / W-HOLD.
    body = [r for r in rows if r[0] < last]
    bad_c4, bad_hold, ff = [], [], {1: 0, 2: 0, 3: 0}
    for call, car, blk, x, z, cd, c29, bf4 in body:
        want = 0xff if c29 < thr(cd) else 0
        if blk[4] != want:
            bad_c4.append((call, car, blk[4], want, cd, c29))
        if blk[4] == 0xff:
            ff[car] += 1
        if call >= 2 and (x, z) != HELD.get(car, (x, z)):
            bad_hold.append((call, car, x, z))
    print(f"calls={last} (exit call {last} excluded)")
    for car in (1, 2, 3):
        cr = [r for r in rows if r[1] == car]
        if cr:
            print(f"car {car}: cd={cr[0][5]} T={thr(cr[0][5])} c29b8 first={cr[0][6]} last-nonexit="
                  f"{[r for r in cr if r[0] < last][-1][6]} ff_calls={ff[car]} bf4_at_exit_call={cr[-1][7]}")
    print(f"W-C4 mismatches={len(bad_c4)}", bad_c4[:5])
    print(f"W-HOLD mismatches={len(bad_hold)}", bad_hold[:5])
    return 0 if not bad_c4 and not bad_hold else 1


if __name__ == "__main__":
    sys.exit(main(sys.argv[1]))
