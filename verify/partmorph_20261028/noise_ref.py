r"""D5-PARTMORPH reference model of FUN_00422470 @ 0x00422470 (and its callees), written from the
disassembly in PREREG_D5-PARTMORPH.md section 1, independent of the C++ port. Float32 steps for every
x87 add / sub / mul (the race loop runs at PC_24); the cosine in double (fcos is not rounded by the
precision control).

usage: py -3.12 noise_ref.py <selftest.csv>     (lines "xbits,rbits" in hex, from MASHED_PARTMORPH_SELFTEST)
Prints the count of points, integer-hash mismatches (none possible here: the hash is recomputed), and the
max ulp distance between the port's output and this model. Exit 1 when any point is over 1 ulp.
"""
import math
import struct
import sys


def f32(v):
    return struct.unpack('<f', struct.pack('<f', v))[0]


def bits(v):
    return struct.unpack('<I', struct.pack('<f', v))[0]


def hash_004223f0(n, mask):
    n &= 0xffffffff
    if mask:
        n &= mask
    e = ((n << 13) ^ n) & 0xffffffff
    c = (e * e) & 0xffffffff
    c = (c * 0x3d73 + 0xc0ae5) & 0xffffffff
    c = (c * e - 0x2df722f3) & 0xffffffff
    c &= 0x7fffffff
    return f32(float(c) * 2.0 ** -31)          # fild exact, one rounding in the fmul


def noise_00422470(x, mask=0xff):
    i = int(math.floor(x))                      # FUN_004a3090 floor, FUN_004a2c48 __ftol
    fi = float(i) + (4294967296.0 if i < 0 else 0.0)
    t = f32(x - fi)                             # fsubr, one rounding
    r1 = hash_004223f0(i + 1, mask)
    r0 = hash_004223f0(i, mask)
    pt = f32(f32(3.14159274) * t)               # [0x005cd310] * t
    w = f32(1.0 - math.cos(pt))
    w = f32(w * 0.5)
    wa = f32(f32(1.0 - w) * r0)
    wb = f32(w * r1)
    return f32(wa + wb)


def ulp_dist(a, b):
    ia, ib = bits(a), bits(b)
    if ia >> 31:
        ia = 0x80000000 - ia
    if ib >> 31:
        ib = 0x80000000 - ib
    return abs(ia - ib)


def main():
    n = worst = over = 0
    for line in open(sys.argv[1]):
        xs, rs = line.strip().split(',')
        x = struct.unpack('<f', struct.pack('<I', int(xs, 16)))[0]
        r = struct.unpack('<f', struct.pack('<I', int(rs, 16)))[0]
        d = ulp_dist(r, noise_00422470(x))
        n += 1
        worst = max(worst, d)
        over += d > 1
    print(f'points={n} max_ulp={worst} over_1ulp={over}')
    sys.exit(1 if over or n == 0 else 0)


if __name__ == '__main__':
    main()
