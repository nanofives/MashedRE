# RO-CTL-OSWORD: detorig_ticks.py verdict before (--block-masks-only) and after (per-dword masks) over the
# RO-REF-CONTROL-NOISE runs. Prints one table row per pair and mode, plus each pair's masked / kept NEW blocks.
#   py -3.12 verify/ro_20261115/osword_pairs.py [DATA_DIR] [--json OUT]
import itertools, json, subprocess, sys, tempfile
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
TOOL = ROOT / "re/frida/detorig_ticks.py"
MASKS = f"{ROOT / 're/frida/warp_normalize.tsv'},{ROOT / 're/frida/replay_masks.tsv'}"
args = [a for a in sys.argv[1:] if not a.startswith("--")]
D = Path(args[0] if args else "D:/MashedWT/RO-REF-CONTROL-NOISE_data")
out_json = sys.argv[sys.argv.index("--json") + 1] if "--json" in sys.argv else ""
runs = ["c1", "c2", "c3", "c4", "c5"]
pairs = list(itertools.combinations(runs, 2)) + [("c1", "cw1"), ("c1", "cw2"), ("cw1", "cw2")]
modes = [("before", ["--block-masks-only"]), ("after", []), ("before n-ok", ["--block-masks-only", "--n-ok"]),
         ("after n-ok", ["--n-ok"])]
rows = []
tmp = Path(tempfile.mkdtemp())
total = len(pairs) * len(modes)
k = 0
for a, b in pairs:
    for name, extra in modes:
        k += 1
        j = tmp / f"{a}_{b}_{name.replace(' ', '_')}.json"
        subprocess.run([sys.executable, str(TOOL), str(D / a), str(D / b), "--masks", MASKS, "--json", str(j), *extra],
                       check=True, capture_output=True, text=True)
        r = json.loads(j.read_text())
        # a raw-route block can still be NEW at a later tick: new_blocks already reflects its dropped ticks
        masked_new = {m[0] for m in r["masked_blocks"]}
        counted = [x[:2] for x in r["new_blocks"] if x[0] not in masked_new]
        rows.append({"pair": f"{a}:{b}", "mode": name, "ident": r["identical_for_ticks"], "n": r["ticks_compared"],
                     "nmis": r["n_misalign_at"], "counted_new": counted,
                     "masked": [m[:3] for m in r["masked_blocks"]], "raw_masked": r.get("raw_masked_blocks", {}),
                     "raw_race": r.get("raw_race", {}), "raw_inconsistent": r.get("raw_inconsistent", {}),
                     "dump_unmasked": r.get("dump_unmasked", {})})
        print(f"[{k}/{total}] {a}:{b} {name:<12} IDENTICAL-FOR-N {r['identical_for_ticks']:>3}/{r['ticks_compared']}"
              f"  N-mis {r['n_misalign_at']}  counted NEW {counted}", flush=True)
if out_json:
    Path(out_json).write_text(json.dumps(rows, indent=1))
