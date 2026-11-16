"""RO-CTL-WARP-N: per-tick clock-call count N and the 0x007f1000 / 0x007d7000 words around the warp, per run.

Usage: py -3.12 warpn_census.py <data_dir> <run> [<run> ...]
Reads <data_dir>/<run>/meta.json, ticks.bin (record k: [tick, N, block hashes..., raw words...]) and the tick
checkpoints ckpt2NN.data.bin (image 0x005ea000..0x00915008, ckpt-ticks order from cfg).
"""
import json, struct, sys
from pathlib import Path

DATA_LO = 0x005ea000


def load(d):
    m = json.loads((d / "meta.json").read_text())
    t = m["ticks"]
    recw = t["recw"]
    raw = (d / "ticks.bin").read_bytes()
    n = len(raw) // (recw * 4)
    recs = [struct.unpack_from("<2I", raw, k * recw * 4) for k in range(n)]
    return m, recs


def word(blob, va):
    return struct.unpack_from("<I", blob, va - DATA_LO)[0]


def main():
    dd = Path(sys.argv[1]); runs = sys.argv[2:]
    M = {r: load(dd / r) for r in runs}
    print("run  N@rec0  tick@rec0  warpN  raceN  dN(rec0->rec299)  dN(rec299->rec300)")
    for r, (m, recs) in M.items():
        print(f"{r:4} {recs[0][1]:6} {recs[0][0]:6} {m['warp']['n']:6} {m['race']['n']:6} {recs[299][1]-recs[0][1]:8} {recs[300][1]-recs[299][1]:6}")
    # per-tick deltas: where do runs differ
    base = runs[0]
    dl = {r: [recs[k + 1][1] - recs[k][1] for k in range(len(recs) - 1)] for r, (m, recs) in M.items()}
    print("\nper-tick dN (N[k+1]-N[k]) histogram per run, and ticks whose dN differs from", base)
    for r in runs:
        h = {}
        for v in dl[r]:
            h[v] = h.get(v, 0) + 1
        diff = [k for k in range(min(len(dl[r]), len(dl[base]))) if dl[r][k] != dl[base][k]]
        print(f"{r:4} hist {dict(sorted(h.items()))}  differs at {diff[:20]}{' ...' if len(diff) > 20 else ''} ({len(diff)})")
    print("\nN - N@rec0 at selected records (offset form)")
    for k in (0, 1, 2, 5, 17, 18, 100, 299, 300, 301, 302, 310, 400, 599):
        print(f"rec {k:3}: " + "  ".join(f"{r}={M[r][1][k][1]-M[r][1][0][1]}" for r in runs if k < len(M[r][1])))
    # checkpoint words
    ck = M[base][0]["cfg"].get("ckptTicks") or []
    print("\nckpt ticks", ck)
    for blk in (0x007f1000, 0x007d7000):
        for i, t in enumerate(ck):
            blobs = {}
            for r in runs:
                p = dd / r / f"ckpt{200 + i}.data.bin"
                if p.exists():
                    blobs[r] = p.read_bytes()
            if len(blobs) < 2:
                continue
            rows = []
            for off in range(0, 0x400, 4):
                vals = [word(blobs[r], blk + off) for r in blobs]
                if len(set(vals)) > 1:
                    rows.append((blk + off, vals))
            print(f"\nblock 0x{blk:08x} tick {t}: {len(rows)} differing dwords  runs {list(blobs)}")
            for va, vals in rows[:40]:
                print(f"  0x{va:08x} " + " ".join(f"{v:>11}" for v in vals))


if __name__ == "__main__":
    main()
