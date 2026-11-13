"""PROV-DIVERGED-18 (2026-11-11): the differing dwords of one run against one reference at the first full-image
checkpoint at or after a tick, inside the given blocks (or anywhere when none differ there).

    py -3.12 re/tools/prov/prov_ckpt_dwords.py <run> <ref> <tick> [block_va ...]
"""
import pathlib, struct, sys

HERE = pathlib.Path(__file__).resolve().parent
sys.path.insert(0, str(HERE))
sys.path.insert(0, str(HERE.parents[2] / "re" / "frida"))
from prov_dwords import ckpt_ticks  # noqa: E402
from detorig import DATA_LO, is_ptr_pair  # noqa: E402
from prov_compare import MASKED  # noqa: E402

run, ref, tick = pathlib.Path(sys.argv[1]), pathlib.Path(sys.argv[2]), int(sys.argv[3])
blocks = [int(x, 16) for x in sys.argv[4:]]
cr, ca = ckpt_ticks(run), ckpt_ticks(ref)
k, t, _f = next(c for c in cr if c[1] is not None and c[1] >= tick)
a = (run / f"ckpt{k:02d}.data.bin").read_bytes()
b = (ref / f"ckpt{k:02d}.data.bin").read_bytes()


def diffs(lo, hi):
    out = []
    for off in range(max(0, lo - DATA_LO), min(len(a), len(b), hi - DATA_LO), 4):
        x, y = struct.unpack_from("<I", a, off)[0], struct.unpack_from("<I", b, off)[0]
        va = DATA_LO + off
        if x != y and va not in MASKED and not is_ptr_pair(x, y):
            out.append((va, x, y))
    return out


d = [r for blk in blocks for r in diffs(blk, blk + 0x400)] or diffs(DATA_LO, DATA_LO + len(a))
print(f"checkpoint {k} at tick {t}: {len(d)} dwords")
for va, x, y in d[:16]:
    fx, fy = struct.unpack("<f", struct.pack("<I", x))[0], struct.unpack("<f", struct.pack("<I", y))[0]
    sx, sy = struct.unpack("<i", struct.pack("<I", x))[0], struct.unpack("<i", struct.pack("<I", y))[0]
    print(f"  {va:#010x} body {x:#010x} ({sx} / {fx:.7g}) orig {y:#010x} ({sy} / {fy:.7g})")
