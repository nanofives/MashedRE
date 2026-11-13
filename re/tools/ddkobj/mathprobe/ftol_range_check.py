"""For __ftol probe outputs: classify every differing (int)double result as in-range or out-of-range of int32.
usage: ftol_range_check.py <ddk.bin> <other.bin>"""
import math
import struct
import sys

REC = 24
a = open(sys.argv[1], "rb").read()
b = open(sys.argv[2], "rb").read()
n = len(a) // REC // 2
for k, pc in enumerate(("PC_24", "PC_53")):
    inr = outr = 0
    ex = []
    for i in range(n):
        o = (k * n + i) * REC
        d = struct.unpack_from("<d", a, o + 4)[0]
        ra = struct.unpack_from("<i", a, o + 12)[0]
        rb = struct.unpack_from("<i", b, o + 12)[0]
        if ra != rb:
            if not math.isnan(d) and not math.isinf(d) and -2147483649.0 < d < 2147483648.0:
                inr += 1
            else:
                outr += 1
                ex.append((d, ra, rb))
    print(pc, "differ in range:", inr, "differ out of int32 range / NaN / Inf:", outr)
    for d, ra, rb in ex:
        print("   ", repr(d), "ddk", ra, "vs", rb)
