# PROV-INSITU-REF1-WS2: one row per IDENTICAL-FOR-600 body of PROV-INSITU-REF1 that is not promoted (WS_CUT.tsv legs
# other than pass, plus the 3 long-race holds), with its write-set failure class and the WS2 runs that name it.
# Inputs: WS_CUT.tsv, WRITESET_TABLE.tsv, VERDICTS.tsv, PROMOTIONS.tsv, ws2_STRIDE_PLAN.tsv,
# D:/MashedWT/PROV-INSITU-REF1-WS2_data/esc_detail_ref1groups.json (ws2_classes.py) and ws2_state.jsonl.
#   py -3.12 ws2_table.py          -> WS2_CLASSES.tsv, class counts on stdout
import collections, csv, json, re
from pathlib import Path

H = Path(__file__).resolve().parent
D = Path("D:/MashedWT/PROV-INSITU-REF1-WS2_data")
rd = lambda f: list(csv.DictReader(open(H / f), delimiter="\t"))
cut = {r["rva"]: r for r in rd("WS_CUT.tsv")}
ver = {r["rva"]: r for r in rd("VERDICTS.tsv")}
prom = {r["rva"] for r in rd("PROMOTIONS.tsv")}
E = json.loads((D / "esc_detail_ref1groups.json").read_text())
state = [json.loads(l) for l in (D / "ws2_state.jsonl").read_text().splitlines() if l.strip()]
single = {x["rvas"][0]: x for x in state if x["label"].startswith("s_")}
wcut = {r["rva"]: r for r in rd("WS2_CUT.tsv")} if (H / "WS2_CUT.tsv").exists() else {}
PROM2 = {r["rva"] for r in csv.DictReader(open(H.parents[1] / "re/promotions/PROV-INSITU-REF1-WS2.tsv"), delimiter="	")}
HELD2 = {"0x0047def0": "not promoted: open STUB S-3728 names it (no-STUB leg)", "0x00469df0": "not promoted: open STUB S-2622 names it (no-STUB leg)", "0x0046ef70": "not promoted: standing long-race DIVERGED@408 on the unchanged source (PROVISIONAL.tsv refused)"}
HOLD = {"0x00469aa0", "0x0046e9e0", "0x004709a0"}
PRE = {"0x004ddcb0", "0x005abd30", "0x005ab410", "0x004e51e0", "0x005b2080", "0x005b04b0", "0x00405540"}
DIRECT = re.compile(r"\[(esp|ebp)( [+-] 0x[0-9a-f]+)?\]")


def esc_class(rv):
    e = E.get(rv)
    if not e:
        return "escapes (no detail)"
    s = e["caller_stack_sites"]
    d = [k for k in s if DIRECT.search(k)]
    parts = []
    if s:
        parts.append("caller-stack own-arg-slot (esp/ebp-relative)" if len(d) == len(s) else
                     "caller-stack through a pointer" if not d else "caller-stack both")
    if e["other_regions"]:
        parts.append("heap/other")
    if e["masked_blocks"]:
        parts.append("image-masked")
    if e["blob_offsets"]:
        parts.append("blob")
    return " + ".join(parts)


rows = []
for rv, v in ver.items():
    if not v["verdict"].startswith("IDENTICAL") or rv in prom:
        continue
    c = cut.get(rv)
    leg = c["leg"] if c else "static covered / dynamic pass"
    if rv in HOLD:
        cls, sub = "A long-race hold", "standing long-race DIVERGED on the current source (RESULT_PROV-INSITU-REF1)"
    elif leg == "escapes":
        cls, sub = "B escapes", esc_class(rv)
    elif leg.startswith("covered, frame only"):
        cls, sub = "C frame only (L-40)", ("all calls before the race anchor or every call already stepped" if rv in PRE
                                            else "race calls: stride re-step planned (ws2_STRIDE_PLAN.tsv)")
    elif leg == "covered+image, run diverged":
        cls, sub = "D covered+image, stepped run diverged", ""
    elif leg == "not-run":
        cls, sub = "E not-run", ("ws_g05 write log full (2,097,152 records, 826,682 dropped; 0x004df120 took 2,083,048 steps)"
                                 if c and c["ws_run"] == "ws_g05" else "not in a ws group (wired later, NOT-WIRED leg)"
                                 if c and c["ws_run"] == "?" else "no main-thread call stepped")
    else:
        cls, sub = "?", leg
    s = single.get(rv)
    rows.append({"rva": rv, "name": v["name"], "class": v["class"], "ref1_ws_run": c["ws_run"] if c else "",
                 "ref1_leg": leg, "ws2_class": cls, "ws2_subclass": sub,
                 "ws2_single": s["label"] if s else "", "ws2_single_replay": f"{s['verdict']} {s.get('identical_for', '')}".strip() if s else "",
                 "ws2_single_frames": (s.get("wlog") or {}).get("frames", "") if s else "",
                 "ws2_single_other_thread_steps": (s.get("wlog") or {}).get("other_thread_steps", "") if s else "",
                 "ws2_best_run": wcut.get(rv, {}).get("run", ""), "ws2_best_run_replay": wcut.get(rv, {}).get("run_replay", ""),
                 "ws2_calls_stepped": wcut.get(rv, {}).get("calls_stepped", ""), "ws2_image": wcut.get(rv, {}).get("image", ""),
                 "ws2_leg": wcut.get(rv, {}).get("leg", "not re-stepped"),
                 "ws2_outcome": "promoted (PROV-INSITU-REF1-WS2.tsv)" if rv in PROM2 else HELD2.get(rv, "")})
with open(H / "WS2_CLASSES.tsv", "w", newline="") as f:
    w = csv.DictWriter(f, fieldnames=list(rows[0]), delimiter="\t"); w.writeheader(); w.writerows(rows)
print(len(rows), "bodies")
print(collections.Counter((r["ws2_class"].split()[0], r["ws2_leg"]) for r in rows))
for k, n in sorted(collections.Counter((r["ws2_class"], r["ws2_subclass"]) for r in rows).items()):
    print(n, k)
