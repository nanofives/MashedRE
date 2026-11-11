"""RWC-DEVICE: compare the [ORIG_TOP] / [RW_BIND] lines of two knob-boot logs with run-specific values masked.

    py -3.12 verify/rwcdevice_20261111/logcmp.py <a.log> <b.log>

Masked: bare 8-digit hex tokens (heap pointers, HWNDs), 0x-prefixed 8-digit values outside the original image range
0x00400000..0x009fffff (heap pointers written with 0x; original addresses stay), and
decimal numbers followed by ms / s (timings). Prints the line counts, the number of differing lines and the first 20.
"""
import re
import sys

PTR = re.compile(r"(?<![0-9A-Fa-fx])[0-9A-Fa-f]{8}(?![0-9A-Fa-f])")
HEAP0X = re.compile(r"0x([0-9A-Fa-f]{8})")


def mask0x(m):
    v = int(m.group(1), 16)
    return m.group(0) if 0x00400000 <= v < 0x00A00000 else "0x<p>"


TIME = re.compile(r"\b\d+(?:\.\d+)?\s*(?:ms|s)\b")


def lines(p):
    out = []
    for ln in open(p, encoding="latin-1"):
        if ln.startswith("[ORIG_TOP]") or ln.startswith("[RW_BIND]"):
            out.append(TIME.sub("<t>", PTR.sub("<p>", HEAP0X.sub(mask0x, ln.rstrip()))))
    return out


def main():
    a, b = lines(sys.argv[1]), lines(sys.argv[2])
    diff = [(i, x, y) for i, (x, y) in enumerate(zip(a, b)) if x != y]
    print("lines %d / %d, differing %d (+%d unpaired)" % (len(a), len(b), len(diff), abs(len(a) - len(b))))
    for i, x, y in diff[:20]:
        print("%5d < %s\n      > %s" % (i + 1, x, y))


if __name__ == "__main__":
    main()
