"""DDKOBJ-REGCONV arm 3: knob-ON log diff, variant vs VS2022 (PREREG arm 3).

    py -3.12 verify/ddkobj_20261112/logdiff_on.py <vs_on.log> <ddk_on.log>

Two passes:
  1. lines outside [ORIG_TOP] / [RW_BIND]: B17 region-map bodies dropped (they flip run to run for one exe, see
     RESULT section 3), thread ids, pointers, 8-digit hex, timings and floats masked;
  2. [ORIG_TOP] / [RW_BIND] lines: timings, thread ids, bare 8-digit hex and 0x pointers outside 0x004xxxxx..0x007xxxxx
     masked (original-address RVAs and cells are kept, so a changed binding or RVA still shows).
Prints the unified diff of each pass.
"""
import difflib
import re
import sys


def norm_rest(p):
    out, skip = [], False
    for l in open(p, encoding="utf-8", errors="replace"):
        l = l.rstrip("\n")
        if l.startswith("=== B17 region map"):
            skip = True
            out.append(l)
            continue
        if skip:
            if l.lstrip().startswith("--- totals"):
                skip = False
            continue
        if "[ORIG_TOP]" in l or "[RW_BIND]" in l:
            continue
        l = re.sub(r"tid=\d+", "tid=N", l)
        l = re.sub(r"0x[0-9a-fA-F]{6,16}", "PTR", l)
        l = re.sub(r"\b[0-9a-fA-F]{8}\b", "HEX8", l)
        l = re.sub(r"\d+(\.\d+)?\s*(ms|s|us)\b", "T", l)
        l = re.sub(r"\d+\.\d+", "F", l)
        out.append(l)
    return out


def norm_top(p):
    def m(l):
        l = re.sub(r"\d+\.\d+ s", "T", l)
        l = re.sub(r"(?<![0-9A-Fa-fx])([0-9A-Fa-f]{8})(?![0-9A-Fa-f])", "HEX8", l)
        l = re.sub(r"0x(?!00[4-7])[0-9a-fA-F]{8}", "PTR", l)
        return re.sub(r"tid=\d+", "tid=N", l)
    return [m(l.rstrip()) for l in open(p, encoding="utf-8", errors="replace") if "[ORIG_TOP]" in l or "[RW_BIND]" in l]


def main():
    vs, ddk = sys.argv[1], sys.argv[2]
    for name, f in (("other lines", norm_rest), ("[ORIG_TOP]/[RW_BIND] lines", norm_top)):
        a, b = f(vs), f(ddk)
        d = list(difflib.unified_diff(a, b, "vs_on", "ddk_on", n=0, lineterm=""))
        print(f"== {name}: vs {len(a)} ddk {len(b)}, diff lines {len(d)}")
        print("\n".join(d))


if __name__ == "__main__":
    main()
