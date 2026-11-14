# SOAK-P0: decode the --tick-raw columns of an unedited detorig run (ticks.bin + meta.json).
# Raw order (run_chain.sh RAW): 0x0063ba8c (1 dword), then car 0..3 position floats (+0x958, +0x95c, +0x960).
# py -3.12 u_decode.py <out dir> [--every N]
import json, math, struct, sys
from pathlib import Path

d = Path(sys.argv[1])
every = int(sys.argv[sys.argv.index("--every") + 1]) if "--every" in sys.argv else 300
m = json.loads((d / "meta.json").read_text())
t = m["ticks"]
recw, nblk, total = t["recw"], t["nblk"], t["total"]
buf = (d / t["file"]).read_bytes()
print("ticks", total, "recw", recw, "nblk", nblk, "race anchor", m.get("race", {}).get("tick"))
rows = []
for i in range(total):
    o = (i * recw + 2 + nblk) * 4
    ui = struct.unpack_from("<i", buf, o)[0]
    pos = [struct.unpack_from("<3f", buf, o + 4 + 12 * k) for k in range(4)]
    tick = struct.unpack_from("<I", buf, i * recw * 4)[0]
    rows.append((tick, ui, pos))
phases = t.get("phases") or m.get("tick_phases")
race = [k for k, (tick, ui, pos) in enumerate(rows) if phases is None or True]
r0 = None
prev = None
for tick, ui, pos in rows:
    if r0 is None and ui != 0:
        r0 = pos
    if ui != prev:
        print(f"ui -> {ui} at tick {tick}")
        prev = ui
r0 = r0 or rows[0][2]
for tick, ui, pos in rows[::every] + [rows[-1]]:
    print(f"tick {tick} ui {ui} disp0..3 " + " ".join("%.2f" % math.dist(r0[k], pos[k]) for k in range(4)))
print("phase runs", phases)
