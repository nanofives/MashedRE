# T2-RACEPATH-CENSUS: join the measured call counts (detorig --prov-count / --prov-snap, prov.json) with each RVA's
# RE status and its top-down binding.
#   py -3.12 verify/census_20261116/census_join.py <prov.json> <out_dir>
# Writes <out_dir>/census_T2-RACEPATH-CENSUS.tsv (one row per rva x phase with calls > 0) and census_counts.json.
#
# Phases, from the counter snapshots (all threads; detorig.js provInstall + provSnapInstall):
#   boot        install (PE entry) .. first tick, minus the audio init
#   audio-init  first enter of 0x004669b0 .. its first leave (split at the first enter of 0x0045d460: pre45d / post45d)
#   menu        first tick .. the warp (tick 300 of the ref1 recipe)
#   race-load   warp .. race anchor (first main-thread clock call with DAT_00771968 == 3)
#   race-tick   race anchor .. end of run (600 hashed ticks)
# reached_today: the original had called the function at least once before its first call of 0x005aa3d0, the point
# where both exe builds stop today (RESULT_PORT-TOP-AUDIOINIT-SPLIT-5A9970). A proxy that assumes the exe follows the
# original's call order up to that stop.
import csv, json, re, sys
from collections import Counter, defaultdict
from pathlib import Path

WT = Path(__file__).resolve().parent.parent.parent
prov, out = Path(sys.argv[1]), Path(sys.argv[2])
P = json.loads(prov.read_text())
rows = P["rows"]; S = P["snaps"]; stage = P.get("stage", "end")
print("report stage", stage)
names = [r["rva"].lower() for r in rows]

def snap(key):
    s = S.get(key)
    if not s or "counts" not in s:
        raise SystemExit(f"snap {key} missing: {s}")
    return s["counts"]

a, b, m45, stop = snap("0x004669b0:enter"), snap("0x004669b0:leave"), snap("0x0045d460:enter"), snap("0x005aa3d0:enter")
for k in ("0x004669b0:enter", "0x004669b0:leave", "0x0045d460:enter", "0x005aa3d0:enter"):
    print(k, {x: S[k][x] for x in ("n", "tick", "tid")})
if not (S["0x004669b0:leave"]["tick"] in (0, None)):
    print("WARNING: audio init leave is not before the first tick; boot split assumes it is")

# ---- status sources
def rv(x): return "0x%08x" % int(x, 16)
hooks = {}; subsys = {}
reach = {l.split("	")[0]: l.split("	")[1].strip() for l in (WT / "verify/census_20261116/reach_top_static.tsv").read_text().splitlines()[1:]}
with open(WT / "hooks.csv", newline="", encoding="utf-8") as f:
    for r in csv.DictReader(f):
        try: k = rv(r["rva"])
        except Exception: continue
        m = re.search(r"c4=([a-z-]+)", r.get("notes") or "")
        hooks[k] = (r["confidence"], m.group(1) if m and r["confidence"] == "C4" else "", r["name"])
        subsys[k] = r["subsystem"]
def tsv(path, key="rva"):
    with open(path, newline="", encoding="utf-8") as f:
        lines = [l for l in f if not l.startswith("#")]
    return {rv(r[key]): r for r in csv.DictReader(lines, delimiter="\t") if r.get(key, "").startswith("0x")}
admitted = tsv(WT / "re/match/ADMITTED.tsv")
provis = tsv(WT / "re/match/PROVISIONAL.tsv")
prov_rwc = tsv(WT / "verify/rwccorerun_20261111/PROVISIONAL_RWC.tsv")
bindings = tsv(WT / "verify/porttop_20261108/BINDINGS.tsv")
routed = tsv(WT / "verify/rwccorerun_20261111/ROUTED.tsv")
plan = {("0x" + k.lower()): v for k, v in json.loads((WT / "verify/portorigtop_20261113/tools/plan.json").read_text()).items()}

BK = {"matched": "matched", "bound": "rw-lib", "crt": "crt-lib", "shim": "shim", "port": "hand-port",
      "near-match": "provisional", "stand-in": "stand-in", "under-stand-in": "under-stand-in", "missing": "missing",
      "rw": "unbound"}
RK = {"match100": "matched", "provisional": "provisional", "lift": "lift", "wrapper": "wrapper", "missing": "missing"}
def binding(k):
    c4 = hooks.get(k, ("", "", ""))[1]
    if k in plan:
        v = plan[k]
        if v[0] == "bind": return {"match": "matched", "replay": "replay", "admitted": "admitted", "provisional": "provisional"}[v[2]], "plan.json"
        if v[0] == "stop": return "stop", "plan.json"
        if v[0] == "reuse":
            if k in bindings: return BK.get(bindings[k]["kind"], bindings[k]["kind"]), "plan.json reuse + BINDINGS.tsv"
            return ("matched" if c4 in ("match", "libid") else "hand-port"), "plan.json reuse " + v[1]
    if k in bindings: return BK.get(bindings[k]["kind"], bindings[k]["kind"]), "BINDINGS.tsv"
    if k in routed and routed[k]["verdict"] == "routed": return RK.get(routed[k]["kind"], routed[k]["kind"]), "ROUTED.tsv (MASHED_RWC_CORE)"
    if k in reach: return ("matched" if c4 in ("match", "libid") else "exe-body"), "static reach " + reach[k][:60]
    return "unbound", ""

def status(k):
    conf, c4, _ = hooks.get(k, ("none", "", ""))
    s = conf + (":" + c4 if c4 else "")
    if k in admitted: s += " admitted"
    if k in provis: s += f" prov({provis[k]['status']} {provis[k]['best_pct']})"
    if k in prov_rwc: s += f" prov_rwc({prov_rwc[k]['best_pct']})"
    return s

# ---- per-rva phase counts
ph = {}
for i, r in enumerate(rows):
    t1, w, an, tot = r["calls_boot"], r["calls_before_warp"], r["calls_to_anchor"], r["calls_total"]
    au = b[i] - a[i]
    if stage == "warp": an = tot = w            # partial report sent at the warp: no race data
    elif stage == "anchor": tot = an = tot       # partial report sent at the anchor: no race-tick data
    ph[names[i]] = {"boot": t1 - au, "audio-init": au, "menu": w - t1, "race-load": an - w, "race-tick": tot - an,
                    "_pre45d": m45[i] - a[i], "_reached": stop[i] > 0, "_window": stop[i] == 0 and m45[i] > 0}
ORDER = ["boot", "audio-init", "menu", "race-load", "race-tick"]
out.mkdir(parents=True, exist_ok=True)
tsvp = out / "census_T2-RACEPATH-CENSUS.tsv"
cnt = defaultdict(Counter); cnt_b = defaultdict(Counter); exec_any = set()
info = {}
with open(tsvp, "w", newline="", encoding="utf-8") as f:
    w = csv.writer(f, delimiter="\t", lineterminator="\n")
    w.writerow(["rva", "name", "phase", "calls", "status", "binding", "binding_src", "reached_today", "reach_static", "subsystem", "first_phase", "audio_pre45d460_calls"])
    for k in names:
        c = ph[k]
        if not any(c[p] for p in ORDER): continue
        exec_any.add(k)
        st, (bd, src) = status(k), binding(k)
        first = next(p for p in ORDER if c[p])
        info[k] = (st, bd, first, c)
        for p in ORDER:
            if c[p]:
                w.writerow([k, hooks.get(k, ("", "", ""))[2], p, c[p], st, bd, src, int(c["_reached"]), int(k in reach), subsys.get(k, ""), first, c["_pre45d"]])
                cnt[p][st.split(" ")[0]] += 1; cnt_b[p][bd] += 1
res = {"executed_distinct": len(exec_any), "instrumented": len(names),
       "by_phase_status": {p: dict(cnt[p]) for p in ORDER}, "by_phase_binding": {p: dict(cnt_b[p]) for p in ORDER},
       "reached_today": sum(1 for k in exec_any if info[k][3]["_reached"])}
BAD = ("exe-body", "provisional", "stand-in", "under-stand-in", "missing", "unbound", "stop", "hand-port", "shim", "rw-lib", "lift", "wrapper")
res["race_tick_nonmatched"] = sorted(([k, info[k][3]["race-tick"], info[k][1], info[k][0], hooks.get(k, ("", "", ""))[2], subsys.get(k, "")]
                                      for k in exec_any if info[k][3]["race-tick"] and info[k][1] in BAD), key=lambda x: -x[1])
res["race_load_nonmatched"] = sorted(([k, info[k][3]["race-load"], info[k][1], info[k][0], hooks.get(k, ("", "", ""))[2], subsys.get(k, "")]
                                      for k in exec_any if info[k][3]["race-load"] and info[k][1] in BAD), key=lambda x: -x[1])
res["audio_pre45d460"] = sorted(([k, info[k][3]["_pre45d"], info[k][1], info[k][0], int(info[k][3]["_reached"])]
                                 for k in exec_any if info[k][3]["_pre45d"] > 0), key=lambda x: x[0])
# first called after the exe's stop point (first enter of 0x005aa3d0) and up to the first enter of 0x0045d460, plus
# 0x005aa3d0 itself: what the exe still has to pass before it reaches 0x0045d460
STOPK = ("stop", "unbound", "missing", "stand-in", "under-stand-in")
win = [k for k in exec_any if info[k][3]["_window"] or k == "0x005aa3d0"]
res["window_5aa3d0_to_45d460"] = sorted([k, info[k][1], info[k][0], hooks.get(k, ("", "", ""))[2]] for k in win)
res["window_binding"] = dict(Counter(info[k][1] for k in win))
res["stops_before_45d460"] = sum(1 for k in win if info[k][1] in STOPK)
res["stage"] = stage
res["unbound_by_subsystem"] = {p: dict(Counter(subsys.get(k, "none") for k in exec_any if info[k][3][p] and info[k][1] == "unbound").most_common(12)) for p in ORDER}
res["first_phase_binding"] = {p: dict(Counter(info[k][1] for k in exec_any if info[k][2] == p)) for p in ORDER}
(out / "census_counts.json").write_text(json.dumps(res, indent=1))
print(json.dumps({k: v for k, v in res.items() if k not in ("race_tick_nonmatched", "race_load_nonmatched", "audio_pre45d460", "window_5aa3d0_to_45d460")}, indent=1))
print("race-tick non-matched", len(res["race_tick_nonmatched"]), "audio pre-45d460 executed", len(res["audio_pre45d460"]))
