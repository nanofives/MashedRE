"""FUN_00414a70 inputs for car 2 at the start of driving, original (--tgt-probe) vs exe (step dump), read-only.

usage: py -3.12 grid_inputs.py <o_tg1.msd.tgt.csv> <exe step.csv> [ncalls=4]
Original: the car-2 calls from the first with its own speed > 0 (the probe row carries all four cars' x, z, speed,
progress 0x008a96e8 (pr) and slot word (st), plus d6d8 = [0x0066d6d8]).
Exe: the same per-car fields from the step dump rows of the same frame (own_x / own_z / rec_9e4 / prog).
"""
import csv, struct, sys
def f(u): return struct.unpack('<f', struct.pack('<I', int(u) & 0xffffffff))[0]
n = int(sys.argv[3]) if len(sys.argv) > 3 else 4
O = [x for x in csv.DictReader(open(sys.argv[1], newline='')) if x['v'] == '2']
i0 = next(i for i, x in enumerate(O) if f(x['sp2']) > 0.0) - 1
print('ORIGINAL (o_tg1), car-2 calls from the last zero-speed call:')
for x in O[i0:i0 + n]:
    cars = ' | '.join(f'c{j} x {f(x["x%d" % j]):.3f} z {f(x["z%d" % j]):.3f} sp {f(x["sp%d" % j]):.0f} pr {f(x["pr%d" % j]):.3f} st {x["st%d" % j]}'
                      for j in range(4))
    print(f'  n {x["n"]} ret {x["ret"]} d6d8 {x["d6d8"]} :: {cars}')
E = list(csv.DictReader(open(sys.argv[2], newline='')))
byf = {}
for x in E: byf.setdefault(int(x['frame']), {})[int(x['v'])] = x
print('EXE, frames 0..%d (cars 1..3 from the step dump; prog = 0x008a96e8 under PATHPROG):' % (n - 1))
for fr in range(n):
    row = byf.get(fr, {})
    cars = ' | '.join(f'c{j} x {float(row[j]["own_x"]):.3f} z {float(row[j]["own_z"]):.3f} sp {float(row[j]["rec_9e4"]):.0f} '
                      f'pr {row[j].get("prog", "")} ret {row[j].get("ret14a70", "-")}' for j in (1, 2, 3) if j in row)
    print(f'  frame {fr} :: {cars}')
