"""Compare the two probe outputs: per function and precision mode, how many results differ as raw doubles and after
rounding to float (the game stores float), and the largest difference in float ULPs.
usage: compare.py <ddk.bin> <vs.bin>"""
import struct
import sys

NAMES = ["sin (intrinsic __CIsin)", "cos (intrinsic __CIcos)", "atan2 (intrinsic __CIatan2)", "sqrt (intrinsic)",
         "sin (function _sin)", "cos (function _cos)"]
REC = 4 + 8 * 6
N = 4096


def f32(x):
    return struct.unpack("<f", struct.pack("<f", x))[0]


def ulp(a, b):
    ia = struct.unpack("<i", struct.pack("<f", a))[0]
    ib = struct.unpack("<i", struct.pack("<f", b))[0]
    return abs(ia - ib)


def main():
    a = open(sys.argv[1], "rb").read()
    b = open(sys.argv[2], "rb").read()
    print("| function | x87 PC | calls | differ (double) | differ (float) | max float ULP |")
    print("|---|---|---|---|---|---|")
    for k, pc in enumerate(("PC_24", "PC_53")):
        for j, nm in enumerate(NAMES):
            dd = df = mu = 0
            for i in range(N):
                o = (k * N + i) * REC
                ra = struct.unpack_from("<d", a, o + 4 + 8 * j)[0]
                rb = struct.unpack_from("<d", b, o + 4 + 8 * j)[0]
                if struct.pack("<d", ra) != struct.pack("<d", rb):
                    dd += 1
                fa, fb = f32(ra), f32(rb)
                if struct.pack("<f", fa) != struct.pack("<f", fb):
                    df += 1
                    mu = max(mu, ulp(fa, fb))
            print(f"| {nm} | {pc} | {N} | {dd} | {df} | {mu} |")


if __name__ == "__main__":
    main()
