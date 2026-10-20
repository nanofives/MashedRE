"""u9774: A5 (FUN_0046ddb0) per call on car 0, original --a5-probe rows vs the port's MASHED_A5GDIAG rows, aligned on each side's
first call with a nonzero entry speed. Prints entry speed and out/in ratio (orig) vs sEnt / sigma (port) for a call range.
usage: a5cmp.py <orig.a5probe.csv> <port.a5g.log> k0 k1"""
import csv, struct, sys, re, math
hx = lambda s: struct.unpack('<f', struct.pack('<I', int(s, 16)))[0]
O = list(csv.DictReader(open(sys.argv[1])))
P = []
for l in open(sys.argv[2]):
    m = re.match(r'i=(\d+) car=0 sEnt=(\S+) sp=(\S+) .* base=(\S+) l70=(\S+) sigma=(\S+)', l)
    if m: P.append(dict(i=int(m[1]), sEnt=float(m[2]), sp=float(m[3]), base=float(m[4]), l70=float(m[5]), sigma=float(m[6])))
def osp(r): return math.sqrt(sum(hx(r['v%d' % j]) ** 2 for j in range(3)))
def oout(r): return math.sqrt(sum(hx(r['o%d' % j]) ** 2 for j in range(3)))
o0 = next(i for i, r in enumerate(O) if osp(r) > 0.01); p0 = next(i for i, r in enumerate(P) if r['sEnt'] > 0.01)
print('orig first moving A5 row sdframe', O[o0]['sdframe'], ' port first moving a5g i', P[p0]['i'])
for k in range(int(sys.argv[3]), int(sys.argv[4])):
    if o0 + k >= len(O) or p0 + k >= len(P): break
    r, q = O[o0 + k], P[p0 + k]
    vin, vo = osp(r), oout(r)
    print('%4d  orig |v| %.6f out/in %.9f s9e4 %.6f | port sEnt %.6f sigma %.9f sp %.6f l70 %.6g %s' % (
        k, vin, vo / vin if vin else 0, hx(r['s9e4']), q['sEnt'], q['sigma'], q['sp'], q['l70'],
        '' if abs(vin - q['sEnt']) < 5e-6 and abs((vo / vin if vin else 0) - q['sigma']) < 2e-8 else '<<'))
