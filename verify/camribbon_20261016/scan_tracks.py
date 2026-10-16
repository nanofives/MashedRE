"""camribbon G-SCAN: per track, new (default) ribbon vs opt-out (stream-order) ribbon, plus the nodes
with no LED elevation. Reads log/camribbon/dump/scan_<track>/ and scanopt_<track>/ (run_cam.py dump).

    py -3.12 scan_tracks.py [out.txt]
"""
import struct
import sys
from pathlib import Path

WT = Path(__file__).resolve().parents[2]
D = WT / "log" / "camribbon" / "dump"
TRACKS = ["Arctic", "City", "dump", "Egypt", "Forest", "Highway", "Neustein", "rouabout", "sands",
          "Storm", "SuperG", "training", "Warzone"]


def f32(x):
    return struct.unpack("<f", struct.pack("<f", float(x)))[0]


def rd(p):
    out = []
    for ln in open(p):
        if ln.strip():
            v = [f32(x) for x in ln.split()]
            out.append((v[0:3], v[3:6], v[6:9]))
    return out


def ribbon(p):
    head, rows = "", []
    for ln in open(p):
        if ln.startswith("#"):
            head = ln.strip()
        elif ln.strip():
            i, h = ln.split()
            rows.append((int(i), int(h)))
    return head, rows


def main():
    out = []
    out.append(f"{'track':9s} {'cid':>3s} {'nodes':>5s} {'dir!=':>6s} {'c0!=':>5s} {'c3!=':>5s} "
               f"{'noLEDelev':>9s}  no-LED nodes (first 12) / of which c3 changed")
    for t in TRACKS:
        k = t.lower()
        n_new = D / f"scan_{k}" / "cam_dump.csv.nodes.txt"
        n_old = D / f"scanopt_{k}" / "cam_dump.csv.nodes.txt"
        r_new = D / f"scan_{k}" / "cam_dump.csv.ribbon.txt"
        if not (n_new.exists() and n_old.exists() and r_new.exists()):
            out.append(f"{t:9s} MISSING")
            continue
        a, b = rd(n_new), rd(n_old)
        head, rows = ribbon(r_new)
        cid = head.split("course_id=")[1].split()[0]
        nd = sum(1 for x, y in zip(a, b) if x[0] != y[0])
        n0 = sum(1 for x, y in zip(a, b) if x[1] != y[1])
        n3 = sum(1 for x, y in zip(a, b) if x[2] != y[2])
        noled = [i for i, h in rows if h == 0]
        ch = [i for i in noled if a[i][2] != b[i][2]]
        out.append(f"{t:9s} {cid:>3s} {len(a):5d} {nd:6d} {n0:5d} {n3:5d} {len(noled):9d}  "
                   f"{noled[:12]} / c3 changed on {len(ch)}")
        if len(a) != len(b):
            out.append(f"   NODE COUNT DIFFERS {len(a)} vs {len(b)}")
    txt = "\n".join(out)
    print(txt)
    if len(sys.argv) > 1:
        Path(sys.argv[1]).write_text(txt + "\n")


if __name__ == "__main__":
    main()
