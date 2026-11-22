"""T2-GATE-RANK (run 53, text-only): which stops and stand-ins gate the T2 path today, ranked by the census functions
they release.

  py -3.12 verify/t2gaterank_20261121/t2_gate_rank.py [--exe <MASHED.exe.unpatched>]

Static, read-only, no build. Inputs (all on this tree):
  graph      verify/portorigtop_20261109/origtop_size.py: function entries/bounds (re/console/pc_funcs.csv), direct E8
             edges (re/console/pc_features.csv) and relocation edges (address-taken + pointer tables) of the original.
             Restricted to the functions the original executed in the census (any phase), so static over-reach through
             tables the original never used on ref1 is cut.
  census     verify/census_20261116/census_T2-RACETICK-CENSUS.tsv. T2 census function = an rva with calls > 0 in phase
             race-load or race-tick; its T2 calls = race-load + race-tick calls.
  route      binding state of each rva on the MASHED_ORIG_TOP route at this head, same precedence as
             verify/census_20261116/census_join.py binding(): plan first (raceload plan
             re/tools/bind/state/raceload/plan.json (BIND-GATES-ALLKNOBS run 55: was the pre-BIND-TOOL copy
             verify/ro_20261116/rltools/plan_rl.json, frozen at PORT-OT-RACE-BIND-PROV4), then the audio plan
             re/tools/bind/state/audio/plan.json), then verify/porttop_20261108/BINDINGS.tsv.
               run   (body compiled, callees followed): plan bind / reuse, BINDINGS matched / near-match
               lib   (supplied, not followed): BINDINGS bound / crt / shim
               hand  (hand exe body, not followed): BINDINGS port
               stop  plan stop
               stand BINDINGS stand-in / under-stand-in (hand stand-in)
               unbound  everything else (BINDINGS missing / rw, or no row): the route has no body for it
Executed path R = closure from 0x00492370 following `run` nodes only. Gate = a node reached by R in state stop /
stand / unbound. Per gate:
  released_alone  T2 census rvas outside R reached when only this gate becomes `run` (it, plus run bodies behind it)
  released_tool   the same, also following every c4=match bindable node (what a porttop_bind.py write of this gate
                  would bind for free: C4 c4=match, a re/match source, not cpp-unclean) and ADMITTED.tsv rows
  behind          T2 census rvas the gate dominates (dominator tree from 0x00492370 over the graph following every
                  node except lib / hand): every path from the top to them passes through the gate
Ranking: released_tool, then its T2 calls, then behind.
Output: verify/t2gaterank_20261121/T2_GATE_RANK.tsv (every gate with behind > 0 or released_tool > 0), summary on stdout.

--all-knobs (run 55, GATERANK-ALLKNOBS): the route view with every default-OFF sub-knob ON, not only MASHED_OT_RACELOAD=1.
Default output is unchanged. The all-knobs view writes T2_GATE_RANK.tsv into --out (default
log/orch/run55_gaterank/all_knobs), never into the committed verify/ copy. See KNOBS below for each knob's file:line.
"""
from __future__ import annotations

import collections
import csv
import json
import pathlib
import re
import sys

HERE = pathlib.Path(__file__).resolve().parent
ROOT = HERE.parents[1]
sys.path.insert(0, str(ROOT / "verify" / "portorigtop_20261109"))
sys.path.insert(0, str(ROOT / "re" / "tools"))
import origtop_size as O          # noqa: E402
import port_closed_sets as P      # noqa: E402

TOP = 0x00492370
T2PH = ("race-load", "race-tick")
EXE_DEF = pathlib.Path(r"C:\Users\maria\Desktop\Proyectos\Mashed\original\MASHED.exe.unpatched")


# knob -> (the rva(s) it switches from a stand-in or stop to a bound body, wrapper source file:line of the getenv)
KNOBS = [
    ("MASHED_OT_RACELOAD=1", "0x0040d270 -> Orig_0040d270", "Match/PortTop_StandInsRL.cpp:22 (also Match/PortTop_00492370.cpp:137)"),
    ("MASHED_OT_RACEFRAME=1", "0x004111c0 -> PortTop_Bound_004111c0 (raceframe plan.json)", "Match/PortTop_RaceFrameKnob.cpp:26"),
    ("MASHED_OT_RENDERFRAME=1 (+ MASHED_RWC_FRAME)", "direct callees of 0x00492e90 -> walked bodies; root 0x00492e90 = RwCore_00492e90 (renderframe plan.json)",
     "Match/PortTop_RenderFrameSel.cpp:14; root route Match/PortTop_00492370.cpp:313 (RwCore_FrameRoute)"),
    ("MASHED_OT_RL_D110=1", "0x0040d110 -> PortTop_Bound_0040d110 (bulk plan.json)", "Match/PortTop_StandInsRL.cpp:191"),
    ("MASHED_OT_TITLETICK=1", "0x00429290 -> PortTop_Bound_00429290 (renderframe plan.json)", "Match/PortTop_00402750_Pieces.cpp:239"),
]
KNOB_FORCE = {0x004111c0: "MASHED_OT_RACEFRAME=1", 0x00492e90: "MASHED_OT_RENDERFRAME=1", 0x0040d110: "MASHED_OT_RL_D110=1",
              0x00429290: "MASHED_OT_TITLETICK=1"}
# re/tools/bind/state/<profile>/plan.json of every profile other than audio / raceload (read above). BIND-GATES-ALLKNOBS
# (run 55): was the fixed tuple ("bulk", "raceframe", "renderframe"), which missed a new profile (gates).
EXTRA_PLANS = tuple(sorted(p.parent.name for p in (ROOT / "re/tools/bind/state").glob("*/plan.json")
                           if p.parent.name not in ("audio", "raceload")))


def h8(a):
    return "0x%08x" % a


def tsv_rows(path):
    with open(path, newline="", encoding="utf-8") as f:
        lines = [l for l in f if not l.startswith("#")]
    return list(csv.DictReader(lines, delimiter="\t"))


def main(argv):
    exe = pathlib.Path(argv[argv.index("--exe") + 1]) if "--exe" in argv else EXE_DEF
    allk = "--all-knobs" in argv
    outdir = HERE
    if allk:
        outdir = pathlib.Path(argv[argv.index("--out") + 1]) if "--out" in argv else ROOT / "log/orch/run55_gaterank/all_knobs"
        outdir.mkdir(parents=True, exist_ok=True)
        print("ALL-KNOBS view; knobs on:")
        for k, what, where in KNOBS:
            print("  %s: %s  [%s]" % (k, what, where))
    ent, bounds = O.load_bounds()
    fset = set(ent)
    dedge = O.direct_edges()
    redge, ntab = O.reloc_edges(exe, ent, bounds)

    # ---- census
    exec_all, t2calls, phases = set(), collections.Counter(), collections.defaultdict(set)
    for r in tsv_rows(ROOT / "verify/census_20261116/census_T2-RACETICK-CENSUS.tsv"):
        a = int(r["rva"], 16)
        c = int(r["calls"] or 0)
        if c <= 0:
            continue
        exec_all.add(a)
        phases[a].add(r["phase"])
        if r["phase"] in T2PH:
            t2calls[a] += c
    T2 = set(t2calls)

    edges = collections.defaultdict(set)
    for src in (dedge, redge):
        for a, cs in src.items():
            if a in exec_all:
                edges[a] |= {c for c in cs if c in fset and c != a and c in exec_all}

    # ---- RE status
    H = P.load_hooks()
    _files, by_rva = P.load_matched()
    ledger = {int(r["rva"], 16): r for r in tsv_rows(ROOT / "re/analysis/match_ledger.tsv") if r["rva"].startswith("0x")}
    admitted = {int(r["rva"], 16) for r in tsv_rows(ROOT / "re/match/ADMITTED.tsv") if r.get("rva", "").startswith("0x")}
    prov = {int(r["rva"], 16): r for r in tsv_rows(ROOT / "re/match/PROVISIONAL.tsv") if r.get("rva", "").startswith("0x")}
    insitu = {int(r["rva"], 16): r["insitu_verdict"] for r in tsv_rows(ROOT / "re/census/t2_route_table.tsv")}
    unclean = {}
    for p in (ROOT / "re/tools/bind/state/raceload/cpp_unclean.txt", ROOT / "verify/ro_20261116/rltools/cpp_unclean.txt"):
        for l in p.read_text(encoding="utf-8", errors="replace").splitlines():
            if l and not l.startswith("#"):
                k, _, why = l.partition("\t")
                unclean[int(k, 16)] = why.strip()[:80]

    def c4tag(a):
        r = H.get(a)
        if not r or r["confidence"] != "C4":
            return ""
        m = re.search(r"c4=([a-z-]+)", r.get("notes") or "")
        return m.group(1) if m else "untagged"

    def source(a):
        if a in ledger and ledger[a]["source"]:
            return ledger[a]["source"], ledger[a]["best_pct"]
        if a in by_rva:
            return by_rva[a][0], ""
        if a in prov:
            return prov[a]["source"], prov[a]["best_pct"]
        return "", ""

    # ---- route state at this head
    st, why = {}, {}
    for a, k in O.load_bindings().items():
        st[a] = {"matched": "run", "near-match": "run", "bound": "lib", "crt": "lib", "shim": "lib", "port": "hand",
                 "stand-in": "stand", "under-stand-in": "stand"}.get(k, "unbound")
        why[a] = "BINDINGS " + k
    for p in (ROOT / "re/tools/bind/state/audio/plan.json", ROOT / "re/tools/bind/state/raceload/plan.json"):
        for k, v in json.loads(p.read_text(encoding="utf-8")).items():
            a = int(k, 16)
            st[a] = "stop" if v[0] == "stop" else "run"
            why[a] = "%s %s%s" % (p.parent.name if p.parent.name != "rltools" else "raceload", v[0],
                                  (" " + v[2]) if v[0] == "bind" else (" " + v[1][:60] if len(v) > 1 and v[1] else ""))

    if allk:   # extra plans: a plan `run` entry wins; a plan `stop` only sets an rva no earlier layer made run
        nplan = collections.Counter()
        for prof in EXTRA_PLANS:
            pp = ROOT / "re/tools/bind/state" / prof / "plan.json"
            for k, v in json.loads(pp.read_text(encoding="utf-8")).items():
                a = int(k, 16)
                s2 = "stop" if v[0] == "stop" else "run"
                if s2 == "stop" and st.get(a) == "run":
                    continue
                if st.get(a) != s2:
                    nplan[prof] += 1
                st[a] = s2
                why[a] = "%s %s%s" % (prof, v[0], (" " + v[2]) if v[0] == "bind" else (" " + v[1][:60] if len(v) > 1 and v[1] else ""))
        for a, kn in KNOB_FORCE.items():
            st[a], why[a] = "run", "knob %s -> bound body (%s)" % (kn, why.get(a, "plan"))
        print("all-knobs plan layer: states changed per profile", dict(nplan))

    # third layer, only for rvas the plans and BINDINGS do not name: the `#define FUN_<rva> <target>` aliases of the
    # ORIGTOP wrapper TUs in mashedmod/exe_sources.rsp (how a compiled matched body's callee is wired). Knob-on view:
    # MASHED_OT_RACELOAD=1, so 0x0040d270 is Orig_0040d270 (Match/PortTop_StandInsRL.cpp PortTop_Bound_0040d270).
    rank_ = {"run": 3, "hand": 2, "stand": 1, "stop": 0}
    dstate, dwhy, nconf = {}, {}, 0
    rsp = [l.strip().strip('"') for l in (ROOT / "mashedmod/exe_sources.rsp").read_text(encoding="utf-8").splitlines()]
    for p in rsp:
        if "PortTop_" not in p:
            continue
        fp = ROOT / "mashedmod/src/mashed_re" / p.replace("\\", "/")
        if not fp.exists():
            continue
        for m in re.finditer(r"^\s*#define\s+FUN_([0-9a-fA-F]{8})\s+([A-Za-z_]\w*)", fp.read_text(encoding="utf-8", errors="replace"), re.M):
            a, t = int(m.group(1), 16), m.group(2)
            if "Stop" in t:
                s = "stop"
            elif "StandIn" in t:
                s = "stand"
            elif t.startswith(("PortTop_Bound_", "Orig_", "PortTop_RL_", "RwCore_")):
                s = "run"
            else:
                s = "hand"
            if a in dstate and dstate[a] != s:
                nconf += 1
            if a not in dstate or rank_[s] > rank_[dstate[a]]:
                dstate[a], dwhy[a] = s, "define %s in %s" % (t, fp.name)
    nover = 0
    for a, s in dstate.items():
        if a not in st:
            st[a], why[a] = s, dwhy[a]
            nover += 1
    st[0x0040d270], why[0x0040d270] = "run", "knob MASHED_OT_RACELOAD=1 -> Orig_0040d270 (PortTop_StandInsRL.cpp)"
    print("define layer: %d rvas aliased, %d added (not in plans/BINDINGS), %d alias conflicts across TUs (highest kept)" % (
        len(dstate), nover, nconf))

    def state(a):
        return st.get(a, "unbound")

    def bindable(a):   # what porttop_bind.py binds by itself: c4=match with a source, compiles as C++, or admitted
        return (c4tag(a) == "match" and source(a)[0] and a not in unclean) or a in admitted

    def cls(a):
        s = state(a)
        r = H.get(a)
        tag = c4tag(a)
        src, pct = source(a)
        fam = O.family_of(r) if r else ""
        if s == "stand":
            return "hand stand-in" + (" (run-loop piece)" if a in O.RACE else "")
        if tag == "match" and src:
            return "c4=match cpp-unclean" if a in unclean else "c4=match bindable"
        if tag:
            return "C4 no bindable source (%s)" % tag if not (tag == "frida-replay" and src) else "C4 frida-replay with source"
        if src:
            lv = r["confidence"] if r else "none"
            return "%s with a source (%s%%%s)" % (lv, pct or "?", (", in-situ " + insitu[a]) if insitu.get(a) else "")
        lv = r["confidence"] if r else "none (no hooks row)"
        return "no source (%s%s)" % (lv, ", " + fam if fam else "")

    def closure(starts, follow):
        seen, stk = set(starts), list(starts)
        while stk:
            a = stk.pop()
            if not follow(a):
                continue
            for c in edges.get(a, ()):
                if c not in seen:
                    seen.add(c)
                    stk.append(c)
        return seen

    R = closure([TOP], lambda a: state(a) == "run")
    gates = sorted(a for a in R if state(a) in ("stop", "stand", "unbound"))

    # dominators over the full-follow graph (all but lib / hand)
    full_follow = lambda a: state(a) not in ("lib", "hand")
    reach = closure([TOP], full_follow)
    order, seen = [], set()
    stk = [(TOP, iter(sorted(edges.get(TOP, ()))))]
    seen.add(TOP)
    while stk:     # iterative DFS postorder
        a, it = stk[-1]
        nxt = None
        if full_follow(a):
            for c in it:
                if c not in seen:
                    nxt = c
                    break
        if nxt is None:
            order.append(a)
            stk.pop()
        else:
            seen.add(nxt)
            stk.append((nxt, iter(sorted(edges.get(nxt, ())))))
    rpo = order[::-1]
    num = {a: i for i, a in enumerate(rpo)}
    preds = collections.defaultdict(list)
    for a in reach:
        if full_follow(a):
            for c in edges.get(a, ()):
                if c in num:
                    preds[c].append(a)
    idom = {TOP: TOP}

    def inter(x, y):
        while x != y:
            while num[x] > num[y]:
                x = idom[x]
            while num[y] > num[x]:
                y = idom[y]
        return x
    changed = True
    while changed:
        changed = False
        for b in rpo[1:]:
            ps = [p for p in preds[b] if p in idom]
            if not ps:
                continue
            new = ps[0]
            for p in ps[1:]:
                new = inter(p, new)
            if idom.get(b) != new:
                idom[b] = new
                changed = True
    kids = collections.defaultdict(list)
    for b, d in idom.items():
        if b != d:
            kids[d].append(b)

    def domset(g):
        out, stk2 = {g}, [g]
        while stk2:
            for c in kids.get(stk2.pop(), ()):
                out.add(c)
                stk2.append(c)
        return out

    rows = []
    for g in gates:
        alone = closure([g], lambda a: a == g or (state(a) == "run")) - R
        tool = closure([g], lambda a: a == g or state(a) == "run" or bindable(a)) - R
        beh = domset(g) if g in idom else {g}
        ra, rt, bh = alone & T2, tool & T2, beh & T2
        if not (rt or bh):
            continue
        nstops = sorted(a for a in tool if a != g and not (state(a) == "run" or bindable(a)) and state(a) not in ("lib", "hand"))
        callers = sorted(a for a in R if state(a) == "run" and g in edges.get(a, ()))
        r = H.get(g) or {}
        rows.append(dict(rva=h8(g), name=r.get("name", "?"), subsystem=r.get("subsystem", ""),
                         level=r.get("confidence", "none"), route_state=state(g), route_src=why.get(g, "no binding row"),
                         cls=cls(g), source=source(g)[0], best_pct=source(g)[1], insitu=insitu.get(g, ""),
                         g_t2=int(g in T2), g_calls=t2calls.get(g, 0),
                         rel_alone=len(ra), rel_alone_calls=sum(t2calls[a] for a in ra),
                         rel_tool=len(rt), rel_tool_calls=sum(t2calls[a] for a in rt),
                         behind=len(bh), behind_calls=sum(t2calls[a] for a in bh),
                         next_stops=len(nstops), next_stop_list=" ".join(h8(a) for a in nstops[:12]),
                         callers_on_path=" ".join(h8(a) for a in callers[:6])))
    rows.sort(key=lambda d: (-d["rel_tool"], -d["rel_tool_calls"], -d["behind"], d["rva"]))
    cols = ["rank", "rva", "name", "cls", "rel_tool", "rel_tool_calls", "rel_alone", "rel_alone_calls", "behind",
            "behind_calls", "g_t2", "g_calls", "route_state", "route_src", "level", "subsystem", "source", "best_pct",
            "insitu", "next_stops", "next_stop_list", "callers_on_path"]
    out = outdir / "T2_GATE_RANK.tsv"
    with open(out, "w", newline="", encoding="utf-8") as f:
        f.write("\t".join(cols) + "\n")
        for i, d in enumerate(rows, 1):
            d["rank"] = i
            f.write("\t".join(str(d[c]) for c in cols) + "\n")
    inR2 = R & T2
    print("graph: executed functions %d, T2 census %d (calls %d), reloc tables %d" % (len(exec_all), len(T2), sum(t2calls.values()), ntab))
    print("route states at head:", dict(collections.Counter(state(a) for a in exec_all)))
    print("executed path R: %d functions, T2 census in R %d (calls %d)" % (len(R), len(inR2), sum(t2calls[a] for a in inR2)))
    print("T2 census reachable from top in the full-follow graph: %d; not attached to the top: %d" % (len(reach & T2), len(T2 - reach)))
    print("gates on R: %d (stop %d, stand %d, unbound %d); with census behind or released: %d" % (
        len(gates), sum(state(a) == "stop" for a in gates), sum(state(a) == "stand" for a in gates),
        sum(state(a) == "unbound" for a in gates), len(rows)))
    hreach = closure([TOP], lambda a: state(a) != "lib")
    print("T2 census reachable only through hand exe bodies (BINDINGS port / define to a named exe body), not ranked: %d" % (
        len((hreach - reach) & T2)))
    print("hand exe bodies on R:", " ".join(h8(a) for a in sorted(a for a in R if state(a) == "hand")))
    for d in rows:
        if d["rva"] in ("0x0047b860",):
            print("runtime first stop %s (PORT-OT-RACE-BIND-PROV4 exit code): rank %d, %s" % (d["rva"], d["rank"], d["cls"]))
    print("class tally of ranked gates:", dict(collections.Counter(re.sub(r" \(.*", "", d["cls"]) for d in rows)))
    for d in rows[:40]:
        print("%3d %s %-28s %-48s tool=%d/%d alone=%d behind=%d/%d" % (d["rank"], d["rva"], d["name"][:28], d["cls"][:48],
              d["rel_tool"], d["rel_tool_calls"], d["rel_alone"], d["behind"], d["behind_calls"]))
    return 0


if __name__ == "__main__":
    sys.exit(main(sys.argv[1:]))
