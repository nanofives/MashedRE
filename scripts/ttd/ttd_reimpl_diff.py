#!/usr/bin/env python3
r"""Diff a reimplementation against TTD-captured ORIGINAL behavior (the in->out CSV).

extract_calls.py captures the original's (input_bits -> output_bits) from a real
canonical run. This harness feeds each captured input to a reimpl backend and compares
its output to the captured original output BIT-EXACTLY. That's the canonical-scenario
reimpl diff -- C4 evidence -- but sourced from real captured inputs instead of synthetic
vectors, and run fully offline.

Backends here are pure-Python stand-ins. `sqrt` models the STANDALONE's no-RW-LUT path
(std::sqrt fallback, see WS-PHYS-CRASH-FIX), so diffing it against the captured original
RW fast-sqrt LUT output quantifies the standalone's residual, per call, to the bit/ULP.
For a TRUE reimpl diff, add an `asi:<export>` backend that Frida-calls the .asi export
with MASHED + the live LUT (the diff_scoring_adder.py pattern) -- the comparison logic
below is unchanged.

Usage:
    py -3.12 scripts/ttd/ttd_reimpl_diff.py log/ttd/calls_004c3b30.csv --reimpl sqrt
"""
import argparse, csv, math, struct, sys

def to_f32(v):
    try: return struct.unpack("<f", struct.pack("<f", v))[0]
    except OverflowError: return float("inf") if v > 0 else float("-inf")

def f32_bits(v):
    try: b = struct.pack("<f", v)
    except OverflowError: b = struct.pack("<f", float("inf") if v > 0 else float("-inf"))
    return struct.unpack("<I", b)[0]

BACKENDS = {
    "sqrt":     lambda x: math.sqrt(x) if x >= 0 else 0.0,          # standalone std::sqrt path
    "invsqrt":  lambda x: 1.0 / math.sqrt(x) if x > 0 else 0.0,
    "identity": lambda x: x,
}

def ulp_distance(a, b):
    """Signed-magnitude float32 bits -> monotonic key; distance = representable steps apart."""
    key = lambda u: u if not (u & 0x80000000) else (0x80000000 - (u & 0x7fffffff))
    return abs(key(a) - key(b))

def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("csv")
    ap.add_argument("--reimpl", choices=list(BACKENDS), default="sqrt")
    a = ap.parse_args()
    fn = BACKENDS[a.reimpl]

    rows = list(csv.DictReader(open(a.csv)))
    if not rows or "in_f32" not in rows[0]:
        sys.exit("CSV has no in_f32/out_bits columns (scalar extract_calls.py output expected)")

    exact, divs, first = 0, [], None
    for r in rows:
        in_f = float(r["in_f32"])
        orig_bits = int(r["out_bits"], 16)
        re_bits = f32_bits(to_f32(fn(in_f)))
        if re_bits == orig_bits:
            exact += 1
        else:
            u = ulp_distance(re_bits, orig_bits)
            divs.append(u)
            if first is None:
                first = (r["i"], r["in_f32"], r["out_bits"], f"0x{re_bits:08x}", u)

    n = len(rows)
    print(f"reimpl='{a.reimpl}'  vs  TTD-captured original  ({a.csv})")
    print(f"  {exact}/{n} bit-identical")
    if divs:
        print(f"  divergent: {len(divs)}   max {max(divs)} ULP   mean {sum(divs)/len(divs):.1f} ULP")
        print(f"  first diff: call[{first[0]}]  in={first[1]}  orig={first[2]}  reimpl={first[3]}  ({first[4]} ULP)")
        print("  -> reimpl does NOT reproduce the original bit-exactly (expected for std::sqrt vs the RW LUT)")
    else:
        print("  ALL bit-identical -> reimpl reproduces the original on every captured input")
        print("     (strong canonical-scenario evidence; promote via re-classify, never auto-C4)")
    # exit 0 always: this is a measurement tool, not a gate

if __name__ == "__main__":
    main()
