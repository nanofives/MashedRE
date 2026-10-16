"""Keep the capture evidence small before the commit: sha8 + line count of every full capture CSV / exe dump go to CAPTURES_SHA.txt,
the full seek / seekobj CSVs are replaced by their informative rows (*.kept.csv), and the MB-sized exe dumps in arms/ are deleted
(their sha8 also stay in the run_arm logs).

  seek rows kept:    every row where gate 1 passes (0.5 <= held <= 30.0 or NaN) or ret != 0
  seekobj rows kept: every row where the state is not 0 in or out, or any DAT_0088ff50 word is non-zero, or any mode word is 10
"""
import csv
import hashlib
import math
import struct
from pathlib import Path

HERE = Path(__file__).resolve().parent


def f(u) -> float:
    return struct.unpack("<f", struct.pack("<I", int(u) & 0xFFFFFFFF))[0]


def sha8(p: Path) -> str:
    return hashlib.sha256(p.read_bytes()).hexdigest()[:8]


out = []
targets = sorted(HERE.glob("O*.msd.seek*.csv")) + sorted((HERE / "w1").glob("H*.msd.seek*.csv"))
for p in targets:
    lines = sum(1 for _ in p.open("rb"))
    out.append(f"{p.relative_to(HERE).as_posix()} lines={lines} sha8={sha8(p)}")
    rows = list(csv.DictReader(p.open(newline="")))
    if not rows:
        continue
    if p.name.endswith(".seek.csv"):
        keep = [r for r in rows if r["ret"] != "0" or (lambda h: math.isnan(h) or 0.5 <= h <= 30.0)(f(r["de0"]))]
    else:
        keep = [r for r in rows if r["state_in"] != "0" or r["state_out"] != "0"
                or any(r[f"ff50_{v}"] != "0" for v in range(4)) or any(r.get(f"mode{v}") == "10" for v in range(4))]
    kp = p.with_name(p.name.replace(".csv", ".kept.csv"))
    with kp.open("w", newline="") as fo:
        w = csv.DictWriter(fo, fieldnames=list(rows[0].keys()))
        w.writeheader()
        w.writerows(keep)
    out.append(f"  -> {kp.name} rows={len(keep)}")
    p.unlink()
for p in sorted((HERE / "arms").glob("*")):
    out.append(f"arms/{p.name} lines={sum(1 for _ in p.open('rb'))} sha8={sha8(p)}")
    p.unlink()
(HERE / "CAPTURES_SHA.txt").write_text("\n".join(out) + "\n", encoding="utf-8")
print("\n".join(out[:12]), "\n...", len(out), "lines")
