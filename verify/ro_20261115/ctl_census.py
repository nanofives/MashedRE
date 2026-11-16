"""RO-REF-CONTROL-NOISE: original-vs-original census over several detorig boot-mode runs.

py -3.12 verify/ro_20261115/ctl_census.py NAME=DIR [NAME=DIR ...] [--vas 0x0068bc2c,...] [--out census.txt]

For every pair: the blocks whose per-tick hash (ticks.bin, detorig_firstdiv.load_run layout) differs at any record, with the
first record, the record count and the last record. Then, per run: warp / race clock-call counts N from meta.json, the warp
timer 'before' words, and the value of each --vas dword in the tick-1 and tick-600 checkpoint dumps (base 0x005ea000).
Block hashes are the raw tick hashes (heap rule already applied in the hash), no mask file: this lists what moves, and
detorig_firstdiv.py gives the masked verdict.
"""
import argparse, ast, itertools, pathlib, sys
import numpy as np

ROOT = pathlib.Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / "re" / "frida"))
import detorig_firstdiv as fd  # noqa: E402

DATA_LO = 0x005ea000


def s32(v):
    v = int(v)
    return v - (1 << 32) if v & 0x80000000 else v


def field(m, k):
    v = m.get(k)
    if isinstance(v, str):
        try:
            v = ast.literal_eval(v)
        except Exception:
            return None
    return v


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("runs", nargs="+")
    ap.add_argument("--vas", default="")
    ap.add_argument("--out", default="")
    a = ap.parse_args()
    runs = {}
    for s in a.runs:
        n, d = s.split("=", 1)
        runs[n] = fd.load_run(d)
    vas = [int(x, 16) for x in a.vas.split(",") if x]
    out = []
    P = out.append
    for n, r in runs.items():
        m = r["meta"]
        w, rc = field(m, "warp") or {}, field(m, "race") or {}
        tim = (w.get("pins") or {}).get("timer") or {}
        P(f"run {n}: dir {r['dir']} pid {m.get('pid')} watch_hits {m.get('watch_hits', 0)} records {len(r['recs'])} "
          f"warp N {w.get('n')} race N {rc.get('n')} dn_warp {rc.get('dn_warp')} timer.before {tim.get('before')} "
          f"stats_end N {(m.get('stats_end') or {}).get('N')}")
        for t in (1, 600):
            if t in r["dumps"]:
                wd = fd.read_dump(r["dumps"][t])
                vals = []
                for va in vas:
                    i = (va - DATA_LO) // 4
                    v = int(wd[i]) if 0 <= i < wd.size else None
                    vals.append(f"0x{va:08x}=" + ("n/a" if v is None else f"0x{v:08x}({s32(v)})"))
                P(f"    tick {t}: " + " ".join(vals))
    P("")
    for (na, A), (nb, B) in itertools.combinations(runs.items(), 2):
        nblk = A["t"]["nblk"]
        k = min(len(A["recs"]), len(B["recs"]))
        ha = A["recs"][:k, 2:2 + nblk]
        hb = B["recs"][:k, 2:2 + nblk]
        dif = ha != hb
        cols = np.nonzero(dif.any(axis=0))[0]
        P(f"pair {na} vs {nb}: {k} records, {len(cols)} blocks differ at some record; blocks differing at record 0: "
          f"{int(dif[0].sum())}")
        for c in cols:
            rr = np.nonzero(dif[:, c])[0]
            P(f"    block 0x{DATA_LO + int(c) * A['t']['block']:08x}: first rec {int(rr[0])} last {int(rr[-1])} count {len(rr)}")
    txt = "\n".join(out) + "\n"
    print(txt)
    if a.out:
        pathlib.Path(a.out).write_text(txt, encoding="utf-8")


if __name__ == "__main__":
    main()
