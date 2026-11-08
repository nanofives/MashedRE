"""PORT-RWP-01 reachability: for every RenderWare-Physics-3.7 row with an exe body, which exe symbol
carries it, whether WinMain reaches it, and if not, which exe symbols reference it and which
top-level unreached ancestors its caller closure ends at.

Uses scripts/backfill_port.py's own graph (rel32 call/jmp + base relocations), symbol resolution
and WinMain walk, so the verdicts are the port column's verdicts, with the caller side added.

  py -3.12 verify/portrwp_20261108/reach.py [--build-dir mashedmod/build]
Writes verify/portrwp_20261108/REACH.tsv and prints a summary.
"""
import csv, pathlib, sys
from collections import deque, Counter

HERE = pathlib.Path(__file__).resolve().parent
ROOT = HERE.parents[1]
sys.path.insert(0, str(ROOT / "scripts"))
import backfill_port as P  # noqa: E402

SUB = "RenderWare-Physics-3.7"


def main(argv):
    build = ROOT / "mashedmod" / "build"
    if "--build-dir" in argv:
        build = pathlib.Path(argv[argv.index("--build-dir") + 1]).resolve()
    exe, mp = build / "mashed_re.exe", build / "mashed_re.map"
    ents, names_at, edges, nrel, (t_lo, t_hi) = P.build_graph(exe, mp)
    prev, barriers = P.walk(ents, names_at, edges)
    bodies = P.exe_bodies()
    res, by_key = P.resolve(bodies, ents, t_lo, t_hi)

    rev: dict[int, set[int]] = {}
    for s, ts in edges.items():
        for t in ts:
            rev.setdefault(t, set()).add(s)

    def nm(va):
        ns = names_at.get(va, [])
        return P.base_name(ns[0]) if ns else f"0x{va:08x}"

    def is_init(va):
        # CRT dynamic initialisers (the no-op RH_ScopedInstall registrations live there)
        return any("dynamic initializer" in s or s.startswith("??__E") for s in names_at.get(va, []))

    def chain(va):
        out = []
        while va is not None:
            out.append(nm(va))
            va = prev.get(va)
        return " < ".join(out)

    def tops(va, limit=4000):
        """Unreached ancestors with no non-initialiser caller: where the caller closure ends."""
        seen, q, top = {va}, deque([va]), set()
        while q and len(seen) < limit:
            a = q.popleft()
            callers = [c for c in rev.get(a, ()) if not is_init(c)]
            if not callers and a != va:
                top.add(a)
            for c in callers:
                if c not in seen:
                    seen.add(c)
                    q.append(c)
        return top, len(seen) - 1

    rows = [r for r in csv.DictReader(open(ROOT / "hooks.csv", encoding="utf-8"))
            if SUB in r["subsystem"] and r["exe_file"].strip()]
    out = []
    stat = Counter()
    for r in rows:
        rva = int(r["rva"], 16)
        items = res.get(rva, [])
        vas = sorted({va for _r, _n, lst, _h in items for va in lst})
        syms = ";".join(sorted({n for _r, n, _l, _h in items})) or "-"
        if not vas:
            hit = None
            for rel, n, _l, _h in items:
                hit = P.inlined_into(rel, n, by_key, prev)
                if hit is not None:
                    break
            reached = "inlined-reached" if hit is not None else "unresolved"
            out.append([r["rva"], r["confidence"], r["port"], r["exe_file"].split("/")[-1], syms, reached,
                        chain(hit) if hit else "-", "-", "-", "-"])
            stat[reached] += 1
            continue
        hit = [va for va in vas if va in prev]
        if hit:
            out.append([r["rva"], r["confidence"], r["port"], r["exe_file"].split("/")[-1], syms, "yes",
                        chain(hit[0]), "-", "-", "-"])
            stat["yes"] += 1
            continue
        callers = sorted({nm(c) for va in vas for c in rev.get(va, ()) if not is_init(c)})
        top_all, n_anc = set(), 0
        for va in vas:
            t, n = tops(va)
            top_all |= t
            n_anc += n
        out.append([r["rva"], r["confidence"], r["port"], r["exe_file"].split("/")[-1], syms, "no", "-",
                    ",".join(callers[:8]) + (f",+{len(callers) - 8}" if len(callers) > 8 else "") or "(none)",
                    ",".join(sorted(nm(t) for t in top_all)[:6]) or "(none)", str(n_anc)])
        stat["no"] += 1

    # The job each row does in the original, and the exe function that does it on the default path.
    # Bridge helpers are the ones the exe's VehicleCouplingBridge_impl calls by absolute address (ABS_CALLS.tsv).
    bridge = set()
    if (HERE / "ABS_CALLS.tsv").exists():
        for l in open(HERE / "ABS_CALLS.tsv", encoding="utf-8"):
            p = l.rstrip("\n").split("\t")
            if len(p) > 3 and p[3] in ("VehicleCouplingBridge_impl", "Bridge_SelfTest"):
                bridge.add(p[0])
    EXE_JOB = ("none: the exe builds no RWP world; car pose comes from VehiclePhysics_StepCar/StepCarsOuter "
               "(UpdateCar < RenderFrame < WinMain), whose StepBegin starts at 0x00470e1a, after the "
               "0x00470e15 CALL FUN_0047eb30")
    for o in out:
        tops_ = o[8]
        if o[0].lower() in bridge:
            job = "bridge helper (FUN_0047eb30 per tick)"
        elif "PhysicsCollisionBodyCreate" in tops_:
            job = "world build (FUN_0047d3c0, load time)"
        elif tops_ not in ("-", "(none)"):
            job = "solver island (FUN_0047ea40 -> FUN_0047e9c0 per tick)"
        else:
            job = "no exe caller and no relocated exe pointer to it" if o[5] == "no" else "-"
        o += [job, EXE_JOB]

    hdr = ["rva", "confidence", "port", "exe_file", "exe_symbol", "reached_by_WinMain", "winmain_chain",
           "exe_callers(non-init)", "caller_closure_tops", "n_ancestors", "original_job", "exe_function_doing_the_job"]
    with open(HERE / "REACH.tsv", "w", encoding="utf-8", newline="") as f:
        f.write("\t".join(hdr) + "\n")
        for o in out:
            f.write("\t".join(o) + "\n")
    print("jobs:", Counter(o[10] for o in out))
    print(f"rows {len(out)}  {dict(stat)}  reloc edges {nrel}")
    tc = Counter()
    for o in out:
        if o[5] == "no":
            for t in o[8].split(","):
                tc[t] += 1
    print("closure tops:", tc.most_common(15))
    return 0


if __name__ == "__main__":
    sys.exit(main(sys.argv[1:]))
