# PROV-INSITU-REF1: markdown tables for the RESULT from VERDICTS.tsv and the extra-batch state (prints to stdout).
import csv, json, collections
from pathlib import Path
H = Path(__file__).resolve().parent
D = Path("D:/MashedWT/PROV-INSITU-REF1_data")
rows = list(csv.DictReader(open(H / "VERDICTS.tsv"), delimiter="\t"))
k = lambda v: "IDENTICAL" if v.startswith("IDENT") else v.split("@")[0]
batch = lambda r: {"b1": "1 (80+)", "b2": "2 (50-80)", "b3": "3 (<50)", "nw": "NOT-WIRED leg"}.get(r["run"][:2], "-")
CL = ("game", "RW-Physics", "RW core")
print("### Totals (309)\n\n| class | IDENTICAL-FOR-600 | DIVERGED | CRASH | NOT-EXECUTED | NOT-WIRED | total |\n|---|---|---|---|---|---|---|")
tot = collections.Counter()
for c in CL:
    t = collections.Counter(k(r["verdict"]) for r in rows if r["class"] == c); tot.update(t)
    print(f"| {c} | {t['IDENTICAL']} | {t['DIVERGED']} | {t['CRASH']} | {t['NOT-EXECUTED']} | {t['NOT-WIRED']} | {sum(t.values())} |")
print(f"| all | {tot['IDENTICAL']} | {tot['DIVERGED']} | {tot['CRASH']} | {tot['NOT-EXECUTED']} | {tot['NOT-WIRED']} | {sum(tot.values())} |\n")
print("### Per batch\n\n| batch | class | tested | IDENTICAL | DIVERGED | CRASH | NOT-EXECUTED |\n|---|---|---|---|---|---|---|")
for b in ("1 (80+)", "2 (50-80)", "3 (<50)", "NOT-WIRED leg"):
    for c in CL:
        t = collections.Counter(k(r["verdict"]) for r in rows if r["class"] == c and batch(r) == b)
        if sum(t.values()):
            print(f"| {b} | {c} | {sum(t.values())} | {t['IDENTICAL']} | {t['DIVERGED']} | {t['CRASH']} | {t['NOT-EXECUTED']} |")
print("\n### DIVERGED and CRASH (every row confirmed by a repeat run)\n\n| rva | name | class | link % | verdict | first tick (segment) and blocks / fault | runs | 11-11 |\n|---|---|---|---|---|---|---|---|")
for r in sorted((r for r in rows if k(r["verdict"]) in ("DIVERGED", "CRASH")), key=lambda r: (k(r["verdict"]), r["class"], r["rva"])):
    det = r["detail"].split("; ", 1)
    conf, rest = det[0], (det[1] if len(det) > 1 else "")
    rest = rest.replace("; ; ", "").strip("; ").replace("|", "/")
    print(f"| `{r['rva']}` | {r['name']} | {r['class']} | {r['link_pct']} | {r['verdict']} | {rest} | {conf} | {r['insitu_1111'] or '-'} |")
print("\n### NOT-WIRED (after the symmap additions)\n\n| rva | class | unresolved |\n|---|---|---|")
for r in rows:
    if r["verdict"] == "NOT-WIRED":
        print(f"| `{r['rva']}` | {r['class']} | {r['detail']} |")
print("\n### Extra batch (lead: 0x0048ae00, MATCH-NOSRC-A, MATCH-NOSRC-B)\n\n| rva | source | link % | verdict | calls | detail |\n|---|---|---|---|---|---|")
bx = {b["rva"]: b for b in json.load(open(D / "bodies_extra.json"))["bodies"]}
st = {}
for nm in ("x_state.jsonl", "nw_state.jsonl", "xrep_state.jsonl"):
    for ln in (D / nm).read_text().splitlines():
        x = json.loads(ln)
        if x["rva"] in bx:
            st.setdefault(x["rva"], []).append(x)
for rva, b in bx.items():
    s = st.get(rva, [])
    if b["status"] != "OK" and not s:
        print(f"| `{rva}` | {b.get('source')} | {b.get('pct')} | NOT-WIRED | - | {','.join(b.get('unresolved', []))} |"); continue
    s0 = s[0]
    v = {"IDENTICAL-FOR-N": f"IDENTICAL-FOR-{s0['n']}", "DIVERGED": f"DIVERGED@{s0['n']}"}.get(s0["verdict"], s0["verdict"])
    det = ""
    if s0.get("first"):
        det = f"tick {s0['first']['tick']} ({s0['first'].get('segment')}), blocks {[x[0] for x in s0['first']['blocks']][:6]}"
    if len(s) > 1:
        det += f"; repeat {s[1]['verdict']} ({s[1]['run']})"
    print(f"| `{rva}` | {b.get('source')} | {b.get('pct')} | {v} | {s0.get('calls_total')} | {det} {s0['run']} |")
