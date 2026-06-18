#!/usr/bin/env python3
r"""Extract per-call (input -> output) pairs for a target function from a TTD trace
to CSV, for an offline BIT-EXACT diff against the reimplementation.

For each recorded call to the target RVA, seek to entry (read the argument) and to
exit (read the return), capturing RAW BITS (not decimal) so the comparison is
bit-exact -- decimal formatting from `dx`/`r` would mask low-bit divergence.

ABI modelled here: __cdecl, single float32 arg at [esp+4], float return in st0
(FastSqrt 0x004c3b30, FastInvSqrt 0x004c3b90, Vec3Magnitude 0x004c3ac0, ...).
Other ABIs become new --abi handlers (vec3_ptr, thiscall_struct, etc.).

The x87 st0 is read as its raw 80-bit extended (sign:exp:mantissa) via `r st0` and
converted to the float32 the function actually returns (round-to-float32, lossless
because the leaf loads a float32 into st0).

Usage:
    py -3.12 scripts/ttd/extract_calls.py <trace.run> [--rva 0x4c3b30]
             [--count 128] [--out log/ttd/calls.csv] [--validate sqrt|invsqrt|none]
"""
import argparse, csv, os, re, struct, sys
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from ttd_query import query

# st0 raw form from `r st0`:  (sign:biased15exp:mantissa64)  e.g. (0:4004:b3997c0000000000)
ST0_RE   = re.compile(r"\(([01]):([0-9a-fA-F]{4}):([0-9a-fA-F]{16})\)")
INPUT_RE = re.compile(r"\(\(unsigned int \*\)@esp\)\[1\]\s*:\s*(0x[0-9a-fA-F]+|\d+)")
COUNT_RE = re.compile(r"\.Count\(\)\s*:\s*(0x[0-9a-fA-F]+|\d+)")

def ext_to_value(sign, exp, mant):
    """x87 80-bit extended -> Python float."""
    if exp == 0 and mant == 0:
        return -0.0 if sign else 0.0
    val = (mant / float(1 << 63)) * (2.0 ** (exp - 16383))   # explicit integer bit at 63
    return -val if sign else val

def f32_bits(value):
    try:
        b = struct.pack("<f", value)
    except OverflowError:
        b = struct.pack("<f", float("inf") if value > 0 else float("-inf"))
    return struct.unpack("<I", b)[0]

def u32_to_f32(bits):
    return struct.unpack("<f", struct.pack("<I", bits & 0xffffffff))[0]

def build_cmds(rva, n):
    rhex = f"0x{rva:x}"
    cmds = [f"dx @$calls = @$cursession.TTD.Calls({rhex})",   # materialize once (avoid re-scan)
            "dx @$calls.Count()"]
    for i in range(n):
        cmds += [
            f".echo @@CALL {i}",
            f"dx @$calls[{i}].TimeStart.SeekTo()",
            "dx ((unsigned int *)@esp)[1]",          # input float32 bits @ [esp+4]
            f"dx @$calls[{i}].TimeEnd.SeekTo()",
            "r st0",                                  # return float in st0
        ]
    return cmds

def parse(text):
    """Positional zip: the i-th input read pairs with the i-th st0 read. Robust to the
    echoed-marker doubling that confuses split-on-marker parsing."""
    inputs = [int(m.group(1), 0) & 0xffffffff for m in INPUT_RE.finditer(text)]
    st0s   = ST0_RE.findall(text)
    n = min(len(inputs), len(st0s))
    rows = []
    for i in range(n):
        in_bits = inputs[i]
        sign, exp, mant = int(st0s[i][0]), int(st0s[i][1], 16), int(st0s[i][2], 16)
        out_val = ext_to_value(sign, exp, mant)
        rows.append({"i": i,
                     "in_bits":  f"0x{in_bits:08x}", "in_f32":  u32_to_f32(in_bits),
                     "out_bits": f"0x{f32_bits(out_val):08x}", "out_f32": out_val})
    return rows, len(inputs), len(st0s)

VALIDATORS = {
    "sqrt":    lambda i, o: (i >= 0) and (abs(o*o - i) <= 1e-3 * max(1.0, abs(i))),
    "invsqrt": lambda i, o: (i >  0) and (abs(o*o*i - 1.0) <= 1e-3),
    "none":    lambda i, o: True,
}

def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("trace")
    ap.add_argument("--rva", default="0x4c3b30")
    ap.add_argument("--count", type=int, default=128)
    ap.add_argument("--out", default=None)
    ap.add_argument("--validate", choices=list(VALIDATORS), default="sqrt")
    a = ap.parse_args()
    rva = int(a.rva, 16)
    out = a.out or os.path.join("log", "ttd", f"calls_{rva:08x}.csv")

    text = query(a.trace, build_cmds(rva, a.count))
    m = COUNT_RE.search(text)
    total = int(m.group(1), 0) if m else None
    rows, n_in, n_out = parse(text)

    os.makedirs(os.path.dirname(out), exist_ok=True)
    with open(out, "w", newline="") as fh:
        w = csv.DictWriter(fh, fieldnames=["i", "in_bits", "in_f32", "out_bits", "out_f32"])
        w.writeheader(); w.writerows(rows)

    chk = VALIDATORS[a.validate]
    passed = sum(1 for r in rows if chk(r["in_f32"], r["out_f32"]))
    print(f"RVA 0x{rva:08x}  total calls in trace: {total}")
    print(f"extracted {len(rows)} (inputs={n_in}, st0s={n_out}) -> {out}")
    if rows:
        print(f"validate[{a.validate}]: {passed}/{len(rows)} pass")
        for r in rows[:8]:
            print(f"  [{r['i']:>4}] in {r['in_bits']} ({r['in_f32']:>14.6f})  ->  "
                  f"out {r['out_bits']} ({r['out_f32']:>14.6f})")
    if rows and passed != len(rows):
        sys.exit(1)

if __name__ == "__main__":
    main()
