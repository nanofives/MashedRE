import csv, json, glob, collections
R = r"C:/Users/maria/Desktop/Proyectos/Mashed/"
naked = {}
for p in glob.glob(R + "re/tools/naked_specs/*.json"):
    for f in json.load(open(p))["funcs"]:
        naked.setdefault(int(f["start"], 16), p.replace("\\", "/").split("/")[-1])
hooks = {}
for r in csv.DictReader(open(R + "hooks.csv", encoding="utf-8", errors="replace")):
    try: hooks[int(r["rva"], 16)] = r
    except Exception: pass
p = R + "verify/d3_bulk_20261011/executed.tsv"
rows = list(csv.DictReader(open(p, encoding="utf-8"), delimiter="\t"))
c = collections.Counter()
for r in rows:
    v = int(r["rva"], 16)
    r["naked_spec"] = naked.get(v, "")
    ef = (hooks.get(v) or {}).get("exe_file", "")
    if r["body"] == "exe": b = "exe-body"
    elif v in naked: b = "exe-naked"
    elif ef: b = "exe_file-col-only"
    elif r["body"] == "asi-only": b = "asi-only"
    else: b = "none"
    r["port"] = b; c[b] += 1
w = csv.DictWriter(open(p, "w", newline="", encoding="utf-8"), fieldnames=list(rows[0].keys()), delimiter="\t", lineterminator="\n")
w.writeheader(); w.writerows(rows)
print(c)
