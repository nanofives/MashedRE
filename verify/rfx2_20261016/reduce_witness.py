"""rfx2: keep one sample of each camera_probe --fx-out witness (the one the offline checks read) for
the evidence folder. The full files stay in log/rfx2/fxw*_<track>/fx.json.

    py -3.12 reduce_witness.py <fx.json> <out.json> [--pick mid|last]
"""
import json
import sys


def main() -> int:
    src, dst = sys.argv[1], sys.argv[2]
    pick = sys.argv[sys.argv.index("--pick") + 1] if "--pick" in sys.argv else "mid"
    fx = json.load(open(src))
    hl = [x for x in fx if x.get("hl") and x["hl"].get("nq", 0) > 0]
    obj = [x for x in fx if x.get("n")]
    lf = [x for x in fx if x.get("lf")]
    keep = {}
    if obj:
        keep["objshad_sample"] = obj[len(obj) // 2]
    if hl:
        keep["headlight_sample"] = hl[len(hl) // 2]
    if lf:
        keep["lensflare_sample"] = lf[-1] if pick == "last" else lf[len(lf) // 2]
        keep["lensflare_k_by_row"] = [[x["row"], sorted({round(e["k"], 4) for e in x["lf"]})] for x in lf]
    keep["source"] = src
    keep["samples"] = len(fx)
    json.dump(keep, open(dst, "w"), indent=0)
    print(dst, {k: (v if isinstance(v, (int, str)) else "...") for k, v in keep.items()})
    return 0


if __name__ == "__main__":
    sys.exit(main())
