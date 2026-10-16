"""Hooked-run witness (H1/H2: Warzone rule 8, --hooks 0x00414f00, obj-only seek probe).

    py -3.12 h_summary.py w1/H1 [w1/H2 ...]

Per run: the 0x00414f00 entry dword at arm / at drain (E9.. = the .asi JMP is live), platform-object state transitions, frames with
DAT_0088ff50[v] != 0, and frames whose committed mode word DAT_0089a52c + v*0x74 is 10. The mode-10 block (0x00416484 /
FUN_00417da0's twin) is the only writer of mode 10, and with the JMP live FUN_00414f00's 1 can only come from the port, so a mode-10
frame is the port's body returning 1 in the live race. Each mode-10 frame is checked against the gates the body tests: the
predicate word of that car != 0 and 0.5 <= held <= 30.0 in the SAME frame row (the row is read at FUN_00451d80's leave, i.e. the
object tick of that frame; the AI step runs in the same frame).
"""
import csv
import re
import struct
import sys
from pathlib import Path

HERE = Path(__file__).resolve().parent


def f(u) -> float:
    return struct.unpack("<f", struct.pack("<I", int(u) & 0xFFFFFFFF))[0]


for arg in sys.argv[1:]:
    base = HERE / arg
    log = base.parent / f"{base.name}_warzone_rule8_hooked.txt"
    txt = log.read_text(encoding="utf-8", errors="replace") if log.exists() else ""
    m = re.search(r"seek-probe agent: (\{.*\})", txt)
    print(f"=== {arg}  agent={m.group(1) if m else 'MISSING'}")
    p = base.parent / f"{base.name}.msd.seekobj.csv"
    rows = list(csv.DictReader(p.open())) if p.exists() else []
    print(f"  obj rows {len(rows)}")
    if not rows:
        continue
    trans = [(r["frame"], r["state_in"], r["state_out"]) for r in rows if r["state_in"] != r["state_out"]]
    print(f"  state transitions: {trans}")
    for v in range(4):
        ff = sum(1 for r in rows if r[f"ff50_{v}"] != "0")
        m10 = [r for r in rows if r[f"mode{v}"] == "10"]
        ok = sum(1 for r in m10 if r[f"ff50_{v}"] != "0" and 0.5 <= f(r["c8"]) <= 30.0)
        print(f"  car {v}: ff50!=0 frames {ff}, committed mode 10 frames {len(m10)}, of which gate-consistent {ok}")
    modes = {}
    for r in rows:
        for v in range(4):
            modes[r[f"mode{v}"]] = modes.get(r[f"mode{v}"], 0) + 1
    print(f"  committed mode histogram (all cars x frames): {dict(sorted(modes.items(), key=lambda kv: -kv[1]))}")
