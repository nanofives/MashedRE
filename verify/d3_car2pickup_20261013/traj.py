"""Car trajectories near the opening pickup, both sides.
orig: <x>.rd.tsv (FUN_00442a60 entry rows, per car x/y/z hex).  exe: <x>.registry.csv (frame,slot,type,radius,x,y,z,...)."""
import sys, csv, struct, math

PK = (1.88176906, 0.669412076, -3.09718895)  # pool 4 (orig) == exe pool slot 1, both sides


def hf(h):
    return struct.unpack('<f', struct.pack('<I', int(h, 16)))[0]


def orig_rows(path):
    out = []
    with open(path, newline='') as f:
        rd = csv.DictReader(f, delimiter='\t')
        for row in rd:
            cars = []
            for c in row['cars'].split(';'):
                p = c.split(':')
                cars.append((hf(p[5]), hf(p[6]), hf(p[7])))
            out.append((int(row['n']), int(row['clk']), int(row['ba8c']), cars))
    return out


def exe_rows(path):
    by = {}
    with open(path, newline='') as f:
        for row in csv.DictReader(f):
            t = int(row['type'], 16)
            if 0xff00 <= t <= 0xff03:
                by.setdefault(int(row['frame']), {})[t - 0xff00] = (float(row['x']), float(row['y']), float(row['z']))
    return [(fr, by[fr]) for fr in sorted(by)]


if __name__ == '__main__':
    kind, path = sys.argv[1], sys.argv[2]
    car = int(sys.argv[3]) if len(sys.argv) > 3 else 2
    lim = int(sys.argv[4]) if len(sys.argv) > 4 else 80
    if kind == 'orig':
        rows = orig_rows(path)
        i6 = next(i for i, r in enumerate(rows) if r[2] == 6)
        print('first ba8c==6 row n=%d clk=%d (index %d)' % (rows[i6][0], rows[i6][1], i6))
        for k, (n, clk, b, cars) in enumerate(rows[max(0, i6 - 3): i6 + lim]):
            p = cars[car]
            print(k - 3, n, clk, b, '%.6f %.6f %.6f' % p, 'd=%.6f' % math.dist(p, PK))
    else:
        rows = exe_rows(path)
        for fr, cars in rows[:lim]:
            p = cars.get(car)
            if p:
                print(fr, '%.6f %.6f %.6f' % p, 'd=%.6f' % math.dist(p, PK))
