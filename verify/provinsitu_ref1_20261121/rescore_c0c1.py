# PROV-INSITU-REF1: re-score every DIVERGED run against the same-boot-class no-body pair c0 / c1 (the body runs carry
# a different kernel32 pointer at 0x00773d60 from ref1 / ref2, MASK:os). Writes RESCORE_C0C1.tsv.
import csv, json, sys
from pathlib import Path
H = Path(__file__).resolve().parent
ROOT = H.parents[1]
sys.path.insert(0, str(ROOT / "re" / "frida"))
from prov_compare import verdict  # noqa: E402
D = Path("D:/MashedWT/PROV-INSITU-REF1_data")
runs = []
for nm in ("b1", "b2", "b3", "rep", "rep2", "nw", "x", "xrep"):
    f = D / f"{nm}_state.jsonl"
    if f.exists():
        for ln in f.read_text().splitlines():
            x = json.loads(ln)
            if x["verdict"] == "DIVERGED":
                runs.append((x["rva"], x["run"], x["n"]))
with open(H / "RESCORE_C0C1.tsv", "w", newline="") as f:
    w = csv.writer(f, delimiter="\t"); w.writerow(["rva", "run", "n_vs_ref1ref2", "verdict_vs_c0c1", "n_vs_c0c1", "boot_class_match"])
    for rva, run, n in runs:
        v = verdict(D / run, D / "c0", D / "c1")
        w.writerow([rva, run, n, v["verdict"], v["identical_for"], v.get("boot_class_match")])
        print(rva, run, n, v["verdict"], v["identical_for"], v.get("boot_class_match"), flush=True)
