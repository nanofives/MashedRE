"""RO-CTL-WARP-N: per-call-site clock-call counts per run, boot segment (calllog_boot.bin, install to tick 1) and load
segment (calllog.bin, warp to race anchor), side by side. A row whose count differs between runs is a source of the
N difference. Site = return address + the next two MASHED.exe .text dwords on the stack (detorig_calllog.key).

Usage: py -3.12 warpn_sites.py <data_dir> <run> [<run> ...]
"""
import json, struct, sys
from collections import Counter
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parents[2] / "re" / "frida"))
import detorig_calllog as dc  # noqa: E402

KIND = {0: "tgt", 1: "qpc"}


def seg(d, name):
    p = d / (name + ".bin")
    if not p.exists():
        return None
    b = p.read_bytes()
    out = []
    for o in range(0, len(b) - 63, 64):
        w = struct.unpack_from("<16I", b, o)
        frames = [x for x in w[2:] if dc.TEXT_LO <= x < dc.TEXT_HI]
        out.append((w[1], (w[2],) + tuple(frames[1:3])))
    return out


def main():
    dd = Path(sys.argv[1]); runs = sys.argv[2:]
    for name in ("calllog_boot", "calllog"):
        cs = {}
        for r in runs:
            s = seg(dd / r, name)
            if s is not None:
                cs[r] = Counter((KIND.get(k, k), site) for k, site in s)
        if not cs:
            continue
        keys = sorted(set().union(*cs.values()), key=lambda k: (-max(c[k] for c in cs.values()), k))
        print(f"\n== {name}: " + "  ".join(f"{r}={sum(c.values())}" for r, c in cs.items()))
        print("  " + " ".join(f"{r:>5}" for r in cs) + "  kind  site")
        for k in keys:
            vals = [cs[r][k] for r in cs]
            mark = "*" if len(set(vals)) > 1 else " "
            print(f"{mark} " + " ".join(f"{v:5d}" for v in vals) + f"  {k[0]}   " + "  <-  ".join(dc.site(x) for x in k[1]))
        for r in cs:
            m = json.loads((dd / r / "meta.json").read_text())
            if name == "calllog_boot":
                print(f"  {r}: tgt {sum(v for (k, _), v in cs[r].items() if k == 'tgt')} qpc {sum(v for (k, _), v in cs[r].items() if k == 'qpc')}"
                      f" warp timer.before {m['warp']['pins']['timer']['before']}")
            else:
                print(f"  {r}: dn_warp {m['race']['dn_warp']}")


if __name__ == "__main__":
    main()
