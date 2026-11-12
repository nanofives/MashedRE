"""PROV-INSITU: name the dwords of a divergence (address + both values) from the full-image checkpoints that every
detorig run already dumps (15 per run, every 2,000 main-thread clock calls), so no extra game run is needed.

For each DIVERGED body in the state files: the first divergence tick T and blocks come from the per-tick hash compare
(prov_compare.py). The checkpoint used is the first one at or after tick T (its tick is found by matching the
checkpoint's N against the per-tick N column). Reported: the differing dwords inside the first-divergence blocks at
that checkpoint (heap-pointer pairs and MASK dwords dropped), body value and original (reference) value; when none of
those blocks still differs there, the first differing dwords anywhere in the image. A divergence that falls between
two checkpoints is named at the later one, and the report says which tick that is.

    py -3.12 re/tools/prov/prov_dwords.py --ref D:/.../r1 --state a.jsonl,b.jsonl --out dwords.json
"""
import argparse, json, pathlib, struct, sys

HERE = pathlib.Path(__file__).resolve().parent
ROOT = HERE.parents[2]
sys.path.insert(0, str(ROOT / "re" / "frida"))
from detorig import DATA_LO, is_ptr_pair  # noqa: E402
from detorig_ticks import load  # noqa: E402
from prov_compare import MASKED  # noqa: E402

DATA = pathlib.Path("D:/MashedWT/PROV-INSITU_data")


def ckpt_ticks(d):
    m, t, recs = load(d)
    ns = [r[1] for r in recs]
    out = []
    for c in m["ckpt"]:
        tick = next((i for i, n in enumerate(ns) if n >= c["n"]), None)
        out.append((c["k"], tick, c["file"]))
    return out


def words(run, ref, k, blocks):
    a = (pathlib.Path(run) / f"ckpt{k:02d}.data.bin").read_bytes()
    b = (pathlib.Path(ref) / f"ckpt{k:02d}.data.bin").read_bytes()
    diff = []
    for i in range(0, min(len(a), len(b)), 4):
        x, y = struct.unpack_from("<I", a, i)[0], struct.unpack_from("<I", b, i)[0]
        va = DATA_LO + i
        if x != y and va not in MASKED and not is_ptr_pair(x, y):
            diff.append((va, x, y))
    inb = [w for w in diff if any(lo <= w[0] < lo + 1024 for lo in blocks)]
    return inb, diff


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--ref", required=True)
    ap.add_argument("--state", required=True)
    ap.add_argument("--out", required=True)
    a = ap.parse_args()
    cref = ckpt_ticks(a.ref)
    res = {}
    for f in a.state.split(","):
        p = pathlib.Path(f)
        if not p.exists():
            continue
        for ln in p.read_text().splitlines():
            s = json.loads(ln)
            if s["verdict"] != "DIVERGED" or s["group"] != 1:
                continue
            run = DATA / s["run"]
            t0 = s["first"]["tick"]
            blocks = [int(b[0], 16) for b in s["first"]["blocks"]]
            try:
                crun = ckpt_ticks(run)
            except SystemExit:
                continue
            k = next((kk for kk, tick, _ in crun if tick is not None and tick >= t0), None)
            if k is None:
                res[s["rva"]] = {"tick": t0, "ckpt": None, "words": []}
                continue
            inb, alld = words(run, a.ref, k, blocks)
            ck_tick = next(tick for kk, tick, _ in crun if kk == k)
            sel = inb or alld
            res[s["rva"]] = {"run": s["run"], "tick": t0, "ckpt": k, "ckpt_tick": ck_tick, "in_first_blocks": bool(inb),
                             "n_diff_dwords": len(alld),
                             "words": [[f"{va:#010x}", f"{x:08x}", f"{y:08x}"] for va, x, y in sel[:12]]}
            w = res[s["rva"]]["words"][:3]
            print(f"{s['rva']} tick {t0} ckpt {k} (tick {ck_tick}) {len(alld)} dwords; {w}", flush=True)
    pathlib.Path(a.out).write_text(json.dumps(res, indent=1))


if __name__ == "__main__":
    main()
