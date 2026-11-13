"""Compare __ftol probe outputs: per x87 precision mode and per conversion form, how many int results differ.
usage: compare_ftol.py <ddk.bin> <other.bin> [label]"""
import struct
import sys

REC = 4 + 8 + 12
FORMS = ["(int)double", "(int)float", "(int)(d*3+.25)"]


def main():
    a = open(sys.argv[1], "rb").read()
    b = open(sys.argv[2], "rb").read()
    label = sys.argv[3] if len(sys.argv) > 3 else "other"
    assert len(a) == len(b), (len(a), len(b))
    n = len(a) // REC // 2
    print(f"| form | PC | calls | DDK vs {label} differ | first differing input |")
    print("|---|---|---|---|---|")
    for k, pc in enumerate(("PC_24", "PC_53")):
        for j, nm in enumerate(FORMS):
            diff = 0
            first = ""
            for i in range(n):
                o = (k * n + i) * REC
                ra = struct.unpack_from("<i", a, o + 12 + 4 * j)[0]
                rb = struct.unpack_from("<i", b, o + 12 + 4 * j)[0]
                if ra != rb:
                    diff += 1
                    if not first:
                        d = struct.unpack_from("<d", a, o + 4)[0]
                        first = f"{d!r} ddk={ra} {label}={rb}"
            print(f"| {nm} | {pc} | {n} | {diff} | {first} |")


if __name__ == "__main__":
    main()
