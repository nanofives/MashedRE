# PROV-INSITU-REF1: split the linked bodies into batches by score tier; bodies wrong on 2026-11-11 (PROV-INSITU, not
# IDENTICAL) run as singles, the rest in groups (sizes as PROV-INSITU: higher score, bigger group).
import json, csv
from pathlib import Path
H = Path(__file__).resolve().parent
b = [x for x in json.load(open("D:/MashedWT/PROV-INSITU-REF1_data/bodies309.json"))["bodies"] if x["status"] == "OK"]
ins = {r["rva"]: r["insitu_1111"] for r in csv.DictReader(open(H / "sweep309.tsv"), delimiter="\t")}
tier = lambda p: "hi" if p >= 80 else "mid" if p >= 50 else "lo"
out = {}
for x in b:
    v = ins[x["rva"]]
    bad = bool(v) and not v.startswith("IDENT")
    out.setdefault(f"{tier(x['pct'])}_{'bad' if bad else 'rest'}", []).append(x["rva"])
for k, v in sorted(out.items()):
    (H / f"batch_{k}.txt").write_text(",".join(v))
    print(k, len(v))
