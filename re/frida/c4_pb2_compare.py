# Summarise c4_pb2_scenario.py runs (group R: 0x00423ee0 / 0x00423f60 / 0x00423ff0 / 0x00424070).
#
# Per run and getter: game calls, calls by return address, mmOrig (return != byte model of the original),
# mmOld (return != pre-2e3cfead stride-4 model), disc (calls where the two models differ, i.e. calls that
# can tell the fix from the defect), consumer checks (FUN_0042c300 / FUN_0042c3c0 args == the getter
# returns that fed them), hook-live witness (first byte + E9 target module), crash / survival.
# Verdict per hook-live arm and getter: GREEN when calls > 0, disc > 0, mmOrig == 0 and consumer bad == 0.
# A control arm is RED when mmOrig > 0 on a getter with disc > 0.
#
# Cross-run channel (--ref REF.json): per phase, the ordered game-call sequence (getter, param, return
# address, return) and the press page walk are compared with the reference run. The original's own
# run-to-run noise is one value: f50 of player 0 (0x00899f70, "cleanest driving line"), which differs
# between original runs; returns equal to that run's own f50[0] are written as 'F' before comparing.
# Consumer vs original: each FUN_0042c300 / FUN_0042c3c0 draw call's value args are checked against
# the byte model m_orig of the getter calls that fed them (cons_orig_bad).
#
# Usage: py -3.12 re/frida/c4_pb2_compare.py <run.json> [...] [--ref REF.json] [--csv out.csv]
import json, sys, csv
from pathlib import Path

G = ["f00", "f04", "f50", "f08"]
RVA = {"f00": "0x00423ee0", "f04": "0x00423f60", "f50": "0x00423ff0", "f08": "0x00424070"}


def main():
    av = sys.argv[1:]
    out = None; ref = None
    if "--ref" in av:
        i = av.index("--ref"); ref = json.loads(Path(av[i + 1]).read_text(encoding="utf-8")); av = av[:i] + av[i + 2:]
    if "--csv" in av:
        i = av.index("--csv"); out = av[i + 1]; av = av[:i] + av[i + 2:]
    rows = []
    for p in av:
        d = json.loads(Path(p).read_text(encoding="utf-8"))
        st = (d.get("dump") or {}).get("st") or {}
        ev = (d.get("dump") or {}).get("ev") or []
        # Pre-attach witness from armg(): after our attach the off/stock arms also show 0xE9 (Frida's own
        # trampoline), so modinfo_at_arm is not a hook-live witness.
        mi = ((d.get("arm_result") or {}).get("g")) or {}
        pages = sorted({e["page"] for e in ev if "page" in e and "g" in e})
        phases = []
        if (st.get("phases") or {}).get("nat"):
            phases = [("nat", st["phases"]["nat"]), ("poke", st)]
        elif d.get("poke_prev") is not None:
            # Runs off1/on1 predate the closephase RPC: rebuild both phases from the per-call events,
            # split at the first event carrying the poked f00/f04 values (natural values were all 0).
            phases = split_by_events(ev, d)
        else:
            phases = [("all", st)]
        for ph, cst in phases:
          for g in G:
            r = row(d, ph, cst, g, mi, ev, pages)
            r.update(extra(d, ph, g, ref))
            rows.append(r)
    emit(rows, out)


def poke_t(d):
    ev = d["dump"]["ev"]
    if d.get("poke_t_ms"):
        return d["poke_t_ms"]
    for e in ev:
        if e.get("g") in ("f00", "f04") and e.get("s") and any(e["s"]["f"]):
            return e["t"]
    return None


def seqs(d):
    """Per phase: [(g, p, ra, sym(ret))], consumer-vs-model counts per getter, and the page walk."""
    F = d["inputs_results"]["f"]["f50"][0]
    tp = poke_t(d)
    sq = {"nat": [], "poke": []}; co = {"nat": {}, "poke": {}}; last = []
    for e in d["dump"]["ev"]:
        ph = "poke" if (tp is not None and e["t"] >= tp) else "nat"
        if "cons" in e:
            want = [x["mo"] for x in last[-len(e["exp"]):]] if e.get("exp") else None
            b = co[ph].setdefault(e.get("g"), {"ok": 0, "bad": 0})
            vals = e["args"][:1] if e["cons"] == "c300" else e["args"][:2]
            b["ok" if want == vals else "bad"] += 1
            last = []
        elif "g" in e and e.get("s"):
            r = e["ret"]; sym = "F" if (e["g"] == "f50" and r == F and F != 0) else r
            sq[ph].append((e["g"], e["p"], e["ra"], sym)); last.append(e)
    pw = [(x["page0"], x["page1"]) for x in d.get("presses", [])]
    return sq, co, pw


def extra(d, ph, g, ref):
    p = ph.rstrip("*")
    if p not in ("nat", "poke") or not d.get("inputs_results"):
        return {}
    sq, co, pw = seqs(d)
    out = {"cons_orig_ok": (co[p].get(g) or {}).get("ok", 0), "cons_orig_bad": (co[p].get(g) or {}).get("bad", 0)}
    if ref is not None:
        rs, _, rpw = seqs(ref)
        a = [x for x in sq[p] if x[0] == g]; b = [x for x in rs[p] if x[0] == g]
        n = max(len(a), len(b)); mm = sum(1 for i in range(n) if i >= len(a) or i >= len(b) or a[i] != b[i])
        out.update({"ref": ref.get("tag"), "xrun_calls": len(a), "xrun_ref_calls": len(b), "xrun_mismatch": mm,
                    "pagewalk_same": pw == rpw})
    return out


def split_by_events(ev, d):
    pk = {a: v for a, v in d["poke"]}
    tpk = None
    for e in ev:
        if e.get("g") in ("f00", "f04") and e.get("s") and any(e["s"]["f"]):
            tpk = e["t"]; break
    out = []
    for name, sel in (("nat", lambda e: tpk is None or e["t"] < tpk), ("poke", lambda e: tpk is not None and e["t"] >= tpk)):
        c = {"calls": {}, "mmOrig": {}, "mmOld": {}, "disc": {}, "byRa": {}, "consBy": {}, "c510by": {}}
        for e in ev:
            if not sel(e): continue
            if "cons" in e:
                b = c["consBy"].setdefault(e.get("g"), {"ok": 0, "bad": 0}); b["ok" if e["ok"] else "bad"] += 1
            elif "g" in e:
                g = e["g"]; c["calls"][g] = c["calls"].get(g, 0) + 1
                c["byRa"].setdefault(g, {}); c["byRa"][g][e["ra"]] = c["byRa"][g].get(e["ra"], 0) + 1
                if e.get("s"):
                    c["mmOrig"][g] = c["mmOrig"].get(g, 0) + (e["mo"] != e["ret"])
                    c["mmOld"][g] = c["mmOld"].get(g, 0) + (e["mold"] != e["ret"])
                    c["disc"][g] = c["disc"].get(g, 0) + (e["mo"] != e["mold"])
            elif "c510" in e:
                c["c510by"][e["c510"]] = c["c510by"].get(e["c510"], 0) + 1
        out.append((name + "*", c))
    return out


def row(d, ph, st, g, mi, ev, pages):
    if True:
        if True:
            calls = (st.get("calls") or {}).get(g, 0)
            mo = (st.get("mmOrig") or {}).get(g, 0); mold = (st.get("mmOld") or {}).get(g, 0)
            disc = (st.get("disc") or {}).get(g, 0)
            gev = [e for e in ev if e.get("g") == g and e.get("s") and "cons" not in e]
            tgs = sorted({e["s"]["tg"] for e in gev}); modes = sorted({e["s"]["mode"] for e in gev})
            t16 = sorted({tuple(e["s"]["t16"]) for e in gev}); fz = sum(1 for e in gev if any(e["s"]["f"]))
            arm = d.get("arm")
            hook_live = mi.get(g, {}).get("b0") == 0xE9
            if arm in ("on", "ctl"):
                if calls == 0: v = "NO-CALLS"
                elif disc == 0: v = "NON-DISCRIMINATING"
                elif mo == 0 and not (((st.get("consBy") or {}).get(g) or {}).get("bad")): v = "GREEN"
                else: v = "RED"
            else:
                v = "REF-OK" if (calls > 0 and mo == 0) else ("REF-NO-CALLS" if calls == 0 else "MODEL-WRONG")
            return {"run": d.get("tag"), "phase": ph, "arm": arm, "getter": g, "rva": RVA[g], "verdict_run": d.get("verdict"),
                         "b0_at_arm": mi.get(g, {}).get("b0"), "e9_target": mi.get(g, {}).get("mod"), "hook_live": hook_live,
                         "calls": calls, "by_ra": json.dumps((st.get("byRa") or {}).get(g, {}), sort_keys=True),
                         "disc": disc, "mmOrig": mo, "mmOld": mold, "nonzero_field_calls": fz,
                         "tg": tgs, "mode": modes, "teams16": [list(t) for t in t16],
                         "consOk_run": st.get("consOk", 0), "consBad_run": st.get("consBad", 0),
                         "cons_ok": ((st.get("consBy") or {}).get(g) or {}).get("ok"), "cons_bad": ((st.get("consBy") or {}).get(g) or {}).get("bad"),
                         "c510_by_ra": json.dumps(st.get("c510by", {}), sort_keys=True),
                         "pages": pages, "crash": len(d.get("crash", [])), "alive_at_end": d.get("alive_at_end"),
                         "exit_code": d.get("exit_code"), "result": v}


def emit(rows, out):
    keys = []
    for r in rows:
        for k in r:
            if k not in keys: keys.append(k)
    for r in rows:
        print(f"{r['run']:>8} {r['phase']:>4} {r['arm']:>5} {r['getter']} live={int(r['hook_live'])} calls={r['calls']:>4} disc={r['disc']:>4} "
              f"mmOrig={r['mmOrig']:>4} mmOld={r['mmOld']:>4} cons={r['cons_ok']}/{r['cons_bad']} nzf={r['nonzero_field_calls']} "
              f"corig={r.get('cons_orig_ok')}/{r.get('cons_orig_bad')} xrun[{r.get('ref')}]={r.get('xrun_mismatch')}/{r.get('xrun_calls')} pw={r.get('pagewalk_same')} "
              f"crash={r['crash']} -> {r['result']}")
    if out:
        with open(out, "w", newline="", encoding="utf-8") as f:
            w = csv.DictWriter(f, fieldnames=keys); w.writeheader()
            for r in rows: w.writerow({k: (json.dumps(v) if isinstance(v, list) else v) for k, v in r.items()})
        print("->", out)


if __name__ == "__main__":
    main()
