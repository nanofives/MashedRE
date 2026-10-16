"""txdscope: byte-compare the standalone draw files of two run13.py arms.

    py -3.12 verify\\txdscope_20261016\\compare_draws.py <dirA> <dirB> [--tracks 01_Egypt,10_Dump]

For every <track>\\stand\\d1r2\\stand_row_*.draws.json present in BOTH dirs: SHA-256 equal or not. Prints one line per
track (files compared, identical, different) and the names of the different files. Exit 0 always (measurement only).
"""
import argparse
import hashlib
from pathlib import Path


def sha(p: Path) -> str:
    return hashlib.sha256(p.read_bytes()).hexdigest()


def main() -> int:
    ap = argparse.ArgumentParser()
    ap.add_argument("a"); ap.add_argument("b"); ap.add_argument("--tracks", default="")
    a = ap.parse_args()
    A, B = Path(a.a), Path(a.b)
    want = [t for t in a.tracks.split(",") if t]
    tot = same = 0
    for td in sorted(p for p in A.iterdir() if p.is_dir()):
        if want and td.name not in want:
            continue
        fa = sorted((td / "stand" / "d1r2").glob("stand_row_*.draws.json"))
        n = s = 0
        diff = []
        for f in fa:
            g = B / td.name / "stand" / "d1r2" / f.name
            if not g.exists():
                diff.append(f.name + " (missing in B)")
                n += 1
                continue
            n += 1
            if sha(f) == sha(g):
                s += 1
            else:
                diff.append(f.name)
        tot += n; same += s
        print(f"{td.name:<16} files={n} identical={s} different={n - s} {diff if diff else ''}")
    print(f"TOTAL files={tot} identical={same} different={tot - same}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
