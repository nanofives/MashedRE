#!/usr/bin/env python3
r"""Extract per-call struct STATE (entry vs exit) for a mutating function from a TTD
trace, to CSV, for an offline BIT-EXACT diff vs the reimplementation.

FastSqrt-style leaves return a value (extract_calls.py handles those). Camera/physics
functions instead MUTATE a struct in place (vehicle record / camera struct) and return
nothing useful — so the observable is the struct fields BEFORE vs AFTER the call. For
each recorded call to the target RVA this seeks to entry (TimeStart) and exit (TimeEnd)
and reads a configured field set at a fixed struct base, capturing RAW BITS so the
comparison is bit-exact. The (state_in -> state_out) rows are what you diff the standalone
reimpl against on the same inputs.

Default field set + base are the player-car-0 vehicle record (DAT_008815a0), verbatim
from re/frida/phys_c4_telemetry.py / vehicle.md — so this is the offline-replay,
full-coverage equivalent of that Frida telemetry lane.

Usage:
    py -3.12 scripts/ttd/extract_struct.py <trace.run> --rva 0x470670 --count 64
    py -3.12 scripts/ttd/extract_struct.py <trace.run> --rva 0x470670 --base 0x8815a0
"""
import argparse, csv, os, re, struct, sys
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from ttd_query import query

# (byte offset into struct base, label, type)  — vehicle record DAT_008815a0
# (phys_c4_telemetry.py FIELDS; vehicle.md). type: f=float i=int32 x=raw hex(grounded sentinel)
VEHICLE_FIELDS = [
    (0x1a8, "driveTorque", "f"), (0x26c, "angTorque", "f"),
    (0x9b0, "velX", "f"), (0x9b4, "velY", "f"), (0x9b8, "velZ", "f"),
    (0x9d4, "fwdX", "f"), (0x9d8, "fwdY", "f"), (0x9dc, "fwdZ", "f"),
    (0x9bc, "angVelX", "f"), (0x9c0, "angVelY", "f"), (0x9c4, "angVelZ", "f"),
    (0x9e0, "groundedCnt", "x"), (0x9e4, "speed", "f"),
    (0xb0c, "slideMeasure", "f"), (0xb14, "boostX", "f"),
    (0xb24, "filtAccel", "i"), (0xb28, "filtBrake", "i"),
    (0x9f0, "motionState", "i"),
]

# camera struct DAT_00897fe0 — the camera director's OUTPUT fields (re/frida/camera_probe.py).
# Unlike the vehicle leaf, FUN_00446520 WRITES these within its own call, so entry-vs-exit
# shows the camera moving (genuine within-call mutation).
CAMERA_FIELDS = [
    (0x34, "elev", "f"), (0x38, "azim", "f"),
    (0x40, "posX", "f"), (0x44, "posY", "f"), (0x48, "posZ", "f"),
    (0x4c, "tgtX", "f"), (0x50, "tgtY", "f"), (0x54, "tgtZ", "f"),
    (0x994, "pairA", "i"), (0x998, "pairB", "i"), (0x9a0, "zoom", "f"),
]

FIELDSETS = {"vehicle": (VEHICLE_FIELDS, 0x008815a0), "camera": (CAMERA_FIELDS, 0x00897fe0)}

# field-read line: "*(unsigned int *)(0x8815a0 + 0x9b0) : 0x........"  (dx prints uint in hex)
FIELD_RE = re.compile(r"unsigned int \*\)\(0x[0-9a-fA-F]+ \+ (0x[0-9a-fA-F]+)\)\s*:\s*(0x[0-9a-fA-F]+|\d+)")

def interp(bits, t):
    if t == "f": return struct.unpack("<f", struct.pack("<I", bits))[0]
    if t == "i": return bits - 0x100000000 if bits & 0x80000000 else bits
    return bits                      # x = raw

def build_cmds(rva, n, base, offsets):
    b = f"0x{base:x}"
    cmds = [f"dx @$calls = @$cursession.TTD.Calls(0x{rva:x})", "dx @$calls.Count()"]
    for i in range(n):
        cmds.append(f"dx @$calls[{i}].TimeStart.SeekTo()")
        cmds += [f"dx *(unsigned int *)({b} + 0x{off:x})" for off, _, _ in offsets]
        cmds.append(f"dx @$calls[{i}].TimeEnd.SeekTo()")
        cmds += [f"dx *(unsigned int *)({b} + 0x{off:x})" for off, _, _ in offsets]
    return cmds

def parse(text, offsets):
    flat = [(int(m.group(1), 16), int(m.group(2), 0) & 0xffffffff) for m in FIELD_RE.finditer(text)]
    N = len(offsets); per = 2 * N
    rows = []
    for c in range(len(flat) // per):
        seg = flat[c * per:(c + 1) * per]
        ins, outs = seg[:N], seg[N:]
        for k, (off, label, t) in enumerate(offsets):
            ib, ob = ins[k][1], outs[k][1]
            rows.append({"call": c, "off": f"0x{off:x}", "field": label, "type": t,
                         "in_bits": f"0x{ib:08x}", "in_val": interp(ib, t),
                         "out_bits": f"0x{ob:08x}", "out_val": interp(ob, t),
                         "changed": int(ib != ob)})
    return rows, len(flat)

def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("trace")
    ap.add_argument("--rva", default="0x470670")
    ap.add_argument("--fields", choices=list(FIELDSETS), default="vehicle")
    ap.add_argument("--base", default=None, help="struct base addr (default per --fields)")
    ap.add_argument("--count", type=int, default=64)
    ap.add_argument("--out", default=None)
    a = ap.parse_args()
    rva = int(a.rva, 16)
    offsets, default_base = FIELDSETS[a.fields]
    base = int(a.base, 16) if a.base else default_base
    out = a.out or os.path.join("log", "ttd", f"struct_{rva:08x}.csv")

    text = query(a.trace, build_cmds(rva, a.count, base, offsets))
    m = re.search(r"\.Count\(\)\s*:\s*(0x[0-9a-fA-F]+|\d+)", text)
    total = int(m.group(1), 0) if m else None
    rows, nflat = parse(text, offsets)

    os.makedirs(os.path.dirname(out), exist_ok=True)
    with open(out, "w", newline="") as fh:
        w = csv.DictWriter(fh, fieldnames=["call", "off", "field", "type",
                                           "in_bits", "in_val", "out_bits", "out_val", "changed"])
        w.writeheader(); w.writerows(rows)

    ncalls = (max((r["call"] for r in rows), default=-1) + 1)
    changed_calls = sorted({r["call"] for r in rows if r["changed"]})
    print(f"RVA 0x{rva:08x}  base 0x{base:x}  total calls in trace: {total}")
    print(f"extracted {ncalls} calls x {len(offsets)} fields ({nflat} reads) -> {out}")
    print(f"calls that MUTATED >=1 field: {len(changed_calls)}/{ncalls}"
          f" (proves the captured state evolves -> diffable)")
    for c in changed_calls[:3]:
        deltas = [r for r in rows if r["call"] == c and r["changed"]]
        print(f"  call[{c}] changed: " + ", ".join(
            f"{d['field']} {d['in_val']:.4g}->{d['out_val']:.4g}" if d['type'] == 'f'
            else f"{d['field']} {d['in_val']}->{d['out_val']}" for d in deltas[:6]))

if __name__ == "__main__":
    main()
