"""Report-only: reference / other index of the refdist producer by driving-call index, original o_rd3 vs the exe's
rdtrace (M_OFF / M_ON). Original driving window as score_u9683.py. Usage: py -3.12 ref_compare.py [first last]"""
import struct
import sys
from pathlib import Path

D = Path(__file__).resolve().parent
O = D.parent / "d3_u9219_20261013"
hexf = lambda h: struct.unpack("<f", struct.pack("<I", int(h, 16)))[0]


def load(path):
    rows = [l.rstrip("\n").split("\t") for l in open(path) if l.strip()]
    return {k: i for i, k in enumerate(rows[0])}, rows[1:]


ix, rows = load(O / "o_rd3.msd.rd.tsv")
orig, on = [], False
for r in rows:
    ba8c = int(r[ix["ba8c"]]); cars = [c.split(":") for c in r[ix["cars"]].split(";")]
    st = [int(c[2], 16) for c in cars]
    if not on:
        if ba8c == 6 and not any(st): on = True
        else: continue
    if ba8c != 6 or any(st): break
    orig.append(dict(clk=int(r[ix["clk"]]), a8=int(r[ix["a8"]]), c8=int(r[ix["c8"]]),
                     pct=[hexf(c[4]) for c in cars], rd=[hexf(x) for x in r[ix["rd"]].split(";")]))

ARM_OFF, ARM_ON = (sys.argv[3], sys.argv[4]) if len(sys.argv) > 4 else ("M_OFF", "M_ON")
exe = {}
for arm in (ARM_OFF, ARM_ON):
    ex, er = load(D / (arm + ".rdtrace.tsv"))
    exe[arm] = [dict(clk=int(r[ex["clk"]]), ref=int(r[ex["ref"]]), other=int(r[ex["other"]]),
                     rd=[hexf(x) for x in r[ex["rd"]].split(";")], ord=int(r[ex["ord"]]))
                for r in er if r[ex["kind"]] == "P"]   # the exe's 0x0063ba8c is not the original's sub-state: every race frame is a driving frame

print("original W: %d producer calls, clk %d..%d, spacing %s; exe OFF: %d P rows, clk %d..%d, spacing %s" % (
    len(orig), orig[0]["clk"], orig[-1]["clk"], sorted({orig[i + 1]["clk"] - orig[i]["clk"] for i in range(len(orig) - 1)}),
    len(exe[ARM_OFF]), exe[ARM_OFF][0]["clk"], exe[ARM_OFF][-1]["clk"],
    sorted({exe[ARM_OFF][i + 1]["clk"] - exe[ARM_OFF][i]["clk"] for i in range(min(300, len(exe[ARM_OFF]) - 1))})))
lo = int(sys.argv[1]) if len(sys.argv) > 1 else 10
hi = int(sys.argv[2]) if len(sys.argv) > 2 else 24
print("driving producer call i: original (clk, ref a8, other c8, race_pct x4, refdist x4) | exe M_OFF (ref, other, refdist x4)"
      " | exe M_ON (ref, other)")
for i in range(lo, hi + 1):
    o = orig[i]
    f = exe[ARM_OFF][i]; n = exe[ARM_ON][i]
    print("%3d  orig clk %5d ref %d other %d pct %s rd %s | OFF ref %d other %d rd %s | ON ref %d other %d" % (
        i, o["clk"], o["a8"], o["c8"], ["%.3f" % p for p in o["pct"]], ["%.3f" % x for x in o["rd"]],
        f["ref"], f["other"], ["%.3f" % x for x in f["rd"]], n["ref"], n["other"]))
same = sum(1 for i in range(min(len(orig), len(exe[ARM_OFF]))) if orig[i]["a8"] == exe[ARM_OFF][i]["ref"])
n = min(len(orig), len(exe[ARM_OFF]))
print("reference index equal on %d / %d driving producer calls (OFF); ON equal on %d / %d" % (
    same, n, sum(1 for i in range(n) if orig[i]["a8"] == exe[ARM_ON][i]["ref"]), n))
