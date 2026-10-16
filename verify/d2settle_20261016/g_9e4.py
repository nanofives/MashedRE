"""G-9E4 (PREREG_d2settle.md section 3): on a PLAYERTRACE log, +0x9e4 (v9e4=) must read exactly 0.0 on every
pre-UpdateRace line before the player's first line with vel != (0,0,0) or c9ec > 0.
usage: py -3.12 g_9e4.py <ptrace.log> [...]"""
import re, sys
R = re.compile(r'^f=(\d+) .*? vel=\(([^,]+),([^,]+),([^)]+)\) .*? v9e4=(\S+) .*? c9ec=(-?\d+) ')
for path in sys.argv[1:]:
    n = bad = 0; first_move = None; vals = set()
    for line in open(path):
        m = R.match(line)
        if not m:
            continue
        f = int(m.group(1)); vel = [float(m.group(i)) for i in (2, 3, 4)]
        v9e4 = float(m.group(5)); c9ec = int(m.group(6))
        if any(v != 0.0 for v in vel) or c9ec > 0:
            first_move = f; break
        n += 1; vals.add(m.group(5))
        if v9e4 != 0.0:
            bad += 1
    print(f'G-9E4 {path}: at-rest frames {n} (until f={first_move}), v9e4 != 0 on {bad}, distinct {sorted(vals)[:4]} '
          f'-> {"PASS" if n > 0 and bad == 0 else "FAIL"}')
