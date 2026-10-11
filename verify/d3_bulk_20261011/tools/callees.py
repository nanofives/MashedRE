import sys, json, csv, glob
sys.path.insert(0, r"C:\Users\maria\Desktop\Proyectos\Mashed\scripts")
import rva_body_scan as S
R = "C:/Users/maria/Desktop/Proyectos/Mashed/"; SP = "C:/Users/maria/AppData/Local/Temp/claude/C--Users-maria--claude-account3-spares-spare-d5Fgde/789b5c2d-9291-49f9-b481-ca9cfeff820e/scratchpad/"
sc = S.scan_all(); exe = S.rva_to_tus(sc, "exe")
naked = {int(f["start"], 16) for p in glob.glob(R + "re/tools/naked_specs/*.json") for f in json.load(open(p))["funcs"]}
hooks = {}
for r in csv.DictReader(open(R + "hooks.csv", encoding="utf-8", errors="replace")):
    try: hooks[int(r["rva"], 16)] = r
    except Exception: pass
ex = {int(r["rva"], 16): r for r in csv.DictReader(open(R + "verify/d3_bulk_20261011/executed.tsv", encoding="utf-8"), delimiter="\t")}
inp = json.load(open(SP + "inputs.json"))
for a in sys.argv[1:]:
    f = inp[a]; out = []
    for nm, ent in f["callees"]:
        v = int(ent, 16); h = hooks.get(v, {})
        st = "exe" if v in exe else ("naked" if v in naked else ("exe_file" if h.get("exe_file") else "-"))
        out.append("%s[%s,%s,%s]" % (ent[2:].lstrip("0"), h.get("confidence", "?"), st, "X" if v in ex else ""))
    print(a, " ".join(out))
