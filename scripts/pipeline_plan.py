#!/usr/bin/env py -3.12
"""pipeline_plan.py — PLANNER tier of the autonomous C2->C3 promotion pipeline.

This is the brain of one round. It is deterministic, cheap, and spawns NO
MASHED.exe (pure file analysis), so it is safe to run unattended every round.

Pipeline architecture (see re/pipeline/README.md):
    Plan (this script)  ->  Execute (worker agents)  ->  Collect (merge+diff)

INPUTS (all produced by the existing recon scripts; run them first):
  - re/analysis/recon_c2/enrich_runnable_today.tsv   (promote_enrich.py)  [supply]
  - re/analysis/recon_c2/enrich_needs_argtype.tsv    (promote_enrich.py)  [ceiling]
  - hooks.csv                                          [drift / already-C3 guard]
  - re/PROMOTION_QUEUE.md                              [already-queued guard]
  - re/pipeline/rounds/                                [round counter + prior plans]

OUTPUT:
  - re/pipeline/rounds/round_<N>_plan.json   the round's batches + arg_type-gap report

The planner does the candidate-selection filtering that promote-c3-batch's
post-mortem says MUST run at plan time, not in the worker session:
  (1) drop RVAs already C3/C4 in hooks.csv               (drift)
  (2) drop RVAs already present in PROMOTION_QUEUE        (in-flight)
  (3) rank the runnable-today supply by a yield/safety score
  (4) emit N sessions x K candidates (default 6 x 5, Opus)
  (5) emit the top arg_type gaps from the needs-argtype pool, so a future
      meta-planner round can build the arg_type that unblocks the most C2.

Run:  py -3.12 scripts/pipeline_plan.py [--sessions 6] [--per-session 5]
                                        [--model claude-opus-4-8[1m]]
                                        [--round N]   (default: auto-increment)
"""
import argparse
import csv
import json
import os
import re
import sys

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
RUNNABLE = os.path.join(ROOT, "re", "analysis", "recon_c2", "enrich_runnable_today.tsv")
NEEDS_AT = os.path.join(ROOT, "re", "analysis", "recon_c2", "enrich_needs_argtype.tsv")
HOOKS = os.path.join(ROOT, "hooks.csv")
QUEUE = os.path.join(ROOT, "re", "PROMOTION_QUEUE.md")
ROUNDS_DIR = os.path.join(ROOT, "re", "pipeline", "rounds")

# arg_types that workers must NOT invent (the integration sweep RED's on a
# non-existent arg_type that locally falls through to a passing default).
# A "runnable" candidate uses one of these; anything else is needs-argtype.


def norm_rva(s):
    s = (s or "").strip().lower()
    if not s:
        return None
    if s.startswith("0x"):
        s = s[2:]
    s = s.lstrip("0") or "0"
    return s.zfill(8)


def load_tsv(path):
    if not os.path.exists(path):
        return []
    with open(path, newline="", encoding="utf-8") as f:
        rows = list(csv.DictReader(f, delimiter="\t"))
    return rows


def load_blocked_rvas():
    """RVAs we must NOT schedule: already >=C3 in hooks.csv, or already queued."""
    blocked = {}  # rva -> reason
    # (1) hooks.csv confidence >= C3 anywhere (drift / already done)
    if os.path.exists(HOOKS):
        with open(HOOKS, newline="", encoding="utf-8") as f:
            for row in csv.DictReader(f):
                conf = (row.get("confidence") or "").strip().upper()
                rva = norm_rva(row.get("rva"))
                if not rva:
                    continue
                if conf in ("C3", "C4"):
                    blocked.setdefault(rva, "already_" + conf.lower())
    # (2) any RVA mentioned in PROMOTION_QUEUE (in-flight or merged)
    if os.path.exists(QUEUE):
        with open(QUEUE, encoding="utf-8") as f:
            for m in re.finditer(r"0x([0-9A-Fa-f]{6,8})", f.read()):
                rva = norm_rva(m.group(1))
                if rva:
                    blocked.setdefault(rva, "in_queue")
    return blocked


# --- exclusion classes the manual process DEFERS (round-1 post-mortem 2026-06-25) --
# A round-1 batch of 30 was 24 thunks + several destructive teardowns; ALL 12
# worker-"GREEN" verdicts RED'd in the real integration diff (uniform ~61s
# timeout) because:
#   * thunks = 5-byte JMP trampolines into (often already-C3) targets — no new
#     first-party behavior, and a hook on the thunk double-installs with the target.
#   * destructive teardown/free fns free live heap when the synthetic A/B harness
#     calls them => hang/crash/timeout, never a clean bit-identity diff.
#   * arg_type=='none' (0-arg) fns have NOTHING to feed and only global side
#     effects to observe — undiffable by the scalar A/B harness.
# What DOES diff GREEN here: getters / computed-returns with a real arg_type.
THUNK_RE = re.compile(r"\bthunk\b|^thunk_", re.I)
DESTRUCTIVE_RE = re.compile(
    r"teardown|free|kill|release|destroy|shutdown|dtor|delete|close|unlock|"
    r"reset|dealloc|cleanup", re.I)


def exclude_reason(row):
    """Return a drop-reason string if this row is structurally undiffable, else None."""
    name = (row.get("name") or "")
    if THUNK_RE.search(name):
        return "thunk"
    if DESTRUCTIVE_RE.search(name):
        return "destructive"
    if (row.get("arg_type_today") or "none") == "none":
        return "no_argtype"        # 0-arg: nothing to feed/observe
    if (row.get("ret") or "").lower() in ("void", ""):
        return "void_ret"          # no computed return = no diff observable
    try:
        if int(row.get("nargs") or 0) < 1:
            return "no_args"
    except ValueError:
        pass
    return None


def score(row):
    """Higher = more likely to produce a clean bit-identity Frida diff.

    Mechanical, decompiler-shape only (NO-GUESSING): every term cites a column
    emitted by promote_enrich.py. Rows reaching score() have already passed
    exclude_reason() (real arg_type, non-void ret, not thunk/destructive).
    """
    s = 0.0
    why = []
    # a real, existing arg_type is the single best diffability signal
    at = row.get("arg_type_today") or "none"
    s += 3; why.append(f"arg:{at}")
    # getter shape: derefs an arg or reads a global and returns it
    if (row.get("derefs_arg") or "0") not in ("0", "-", ""):
        s += 1; why.append("deref-arg")
    if (row.get("reads_global") or "0") not in ("0", "-", ""):
        s += 1; why.append("reads-global")
    sig = (row.get("sig_conf") or "").lower()
    if sig == "high":
        s += 2; why.append("sig:high")
    elif sig in ("med", "medium"):
        s += 1; why.append("sig:med")
    else:
        why.append("sig:low")
    try:
        ncall = int(row.get("n_callees") or 0)
    except ValueError:
        ncall = 0
    if ncall == 0:
        s += 1; why.append("leaf")
    try:
        size = int(row.get("size") or 0)
    except ValueError:
        size = 0
    if 8 <= size <= 120:
        s += 1; why.append("right-size")
    elif size > 250:
        s -= 2; why.append("huge")
    if (row.get("fp") or "0") in ("0", "-", ""):
        s += 1; why.append("no-fp")
    else:
        s -= 1; why.append("fp-risk")
    if (row.get("com_d3d") or "0") in ("0", "-", ""):
        s += 1
    else:
        s -= 2; why.append("com")          # COM/D3D/DSound/DInput state = unsafe to diff
    if (row.get("rw_orch") or "0") in ("0", "-", ""):
        s += 1
    else:
        why.append("rw-orch")
    return s, ",".join(why)


def load_prior_scheduled():
    """RVAs any prior round already scheduled (landed OR queued OR failed).

    Avoids re-burning tokens re-attempting the same candidates every round: a
    candidate a prior round scheduled but didn't land was already determined
    queued/failed for THIS arg_type set. NOTE: clear/ignore this after building a
    NEW arg_type (the meta-action) — previously-queued "needs arg_type X"
    candidates may then become runnable and deserve a retry.
    """
    rvas = set()
    if not os.path.isdir(ROUNDS_DIR):
        return rvas
    for fn in os.listdir(ROUNDS_DIR):
        if not re.match(r"round_\d+_plan\.json$", fn):
            continue
        try:
            p = json.load(open(os.path.join(ROUNDS_DIR, fn), encoding="utf-8"))
        except Exception:
            continue
        for b in p.get("batches", []):
            for c in b.get("candidates", []):
                rv = norm_rva(c.get("rva"))
                if rv:
                    rvas.add(rv)
    return rvas


def next_round_number():
    if not os.path.isdir(ROUNDS_DIR):
        return 1
    mx = 0
    for fn in os.listdir(ROUNDS_DIR):
        m = re.match(r"round_(\d+)_plan\.json$", fn)
        if m:
            mx = max(mx, int(m.group(1)))
    return mx + 1


def argtype_gaps(needs_rows, top=12):
    """Rank the unmet arg_type needs by how many C2 they would unblock.

    Groups the needs-argtype pool by (conv, nargs, ret, fp) — the signature
    shape a bespoke arg_type would have to cover — and counts members. This is
    the meta-planner's input: build the arg_type at the top of this list and the
    next round's runnable-today supply grows by `count`.
    """
    buckets = {}
    for r in needs_rows:
        key = (
            (r.get("conv") or "?").strip(),
            (r.get("nargs") or "?").strip(),
            (r.get("ret") or "?").strip(),
            "fp" if (r.get("fp") or "0") not in ("0", "-", "") else "int",
        )
        b = buckets.setdefault(key, {"count": 0, "examples": []})
        b["count"] += 1
        if len(b["examples"]) < 4:
            b["examples"].append(norm_rva(r.get("rva")))
    out = []
    for (conv, nargs, ret, fp), b in sorted(
        buckets.items(), key=lambda kv: -kv[1]["count"]
    )[:top]:
        out.append({
            "shape": f"{conv} nargs={nargs} ret={ret} {fp}",
            "would_unblock": b["count"],
            "example_rvas": b["examples"],
        })
    return out


def cluster_candidates(picked, max_size=6, prox_gap=0x800, model=""):
    """Group score-picked candidates into coherent clusters for cluster workers.

    Pass 1: sort by RVA; cut a new cluster when subsystem changes, the RVA gap to
    the previous member exceeds prox_gap, or the cluster hits max_size. This yields
    coherent address-adjacent families (same source TU => shared structs/globals).
    Pass 2: families of >=3 stay as coherent clusters; leftover singletons/pairs are
    packed into 'mixed' groups of ~max_size so a worker still amortizes its boot +
    build across several functions even when they aren't a family.
    """
    by_rva = sorted(picked, key=lambda c: int(c["rva"], 16))
    raw = []
    cur = []
    for c in by_rva:
        rva = int(c["rva"], 16)
        if cur and (c["sub"] != cur[-1]["sub"]
                    or rva - int(cur[-1]["rva"], 16) > prox_gap
                    or len(cur) >= max_size):
            raw.append(cur)
            cur = []
        cur.append(c)
    if cur:
        raw.append(cur)

    families = [g for g in raw if len(g) >= 3]
    loose = [c for g in raw if len(g) < 3 for c in g]
    mixed = [loose[i:i + max_size] for i in range(0, len(loose), max_size)]

    clusters = []
    cid = 0
    for g in families:
        cid += 1
        clusters.append({"session": cid, "model": model, "kind": "family",
                         "subsystem": g[0]["sub"], "candidates": g})
    for g in mixed:
        cid += 1
        clusters.append({"session": cid, "model": model, "kind": "mixed",
                         "subsystem": "mixed", "candidates": g})
    return clusters


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--sessions", type=int, default=6)
    ap.add_argument("--per-session", type=int, default=5)
    ap.add_argument("--model", default="claude-opus-4-8[1m]")
    ap.add_argument("--round", type=int, default=None)
    ap.add_argument("--min-score", type=float, default=2.0,
                    help="drop candidates below this yield score")
    ap.add_argument("--peek", action="store_true",
                    help="print the plan but do NOT write a round file or consume candidates")
    ap.add_argument("--cluster-max", type=int, default=6,
                    help="max functions per cluster/worker")
    ap.add_argument("--cluster-gap", type=lambda s: int(s, 0), default=0x800,
                    help="max RVA gap (bytes) to keep two functions in one family cluster")
    args = ap.parse_args()

    runnable = load_tsv(RUNNABLE)
    needs = load_tsv(NEEDS_AT)
    if not runnable:
        print(f"FATAL: {RUNNABLE} missing/empty — run promote_enrich.py first",
              file=sys.stderr)
        return 2
    blocked = load_blocked_rvas()
    prior = load_prior_scheduled()

    scored = []
    dropped = {"blocked_c3": 0, "in_queue": 0, "low_score": 0, "dup": 0,
               "thunk": 0, "destructive": 0, "no_argtype": 0, "void_ret": 0,
               "no_args": 0, "prior_round": 0}
    seen = set()
    for r in runnable:
        rva = norm_rva(r.get("rva"))
        if not rva:
            continue
        if rva in seen:
            dropped["dup"] += 1
            continue
        seen.add(rva)
        if rva in blocked:
            reason = blocked[rva]
            dropped["in_queue" if reason == "in_queue" else "blocked_c3"] += 1
            continue
        if rva in prior:
            dropped["prior_round"] += 1
            continue
        ex = exclude_reason(r)
        if ex:
            dropped[ex] += 1
            continue
        sc, why = score(r)
        if sc < args.min_score:
            dropped["low_score"] += 1
            continue
        scored.append({
            "rva": "0x" + rva,
            "name": r.get("name") or "",
            "sub": r.get("sub") or "",
            "size": r.get("size") or "",
            "conv": r.get("conv") or "",
            "nargs": r.get("nargs") or "",
            "ret": r.get("ret") or "",
            "arg_type": r.get("arg_type_today") or "none",
            "sig_conf": r.get("sig_conf") or "",
            "score": round(sc, 1),
            "why": why,
        })
    scored.sort(key=lambda c: -c["score"])

    # Take the top-scored candidates, then CLUSTER them by address proximity +
    # subsystem so each worker reimplements a coherent FAMILY (same source region
    # => shared structs/globals/helpers). The compiler emits a translation unit's
    # functions adjacently, so RVA-adjacency ~= same .c file. Reimplementing a
    # family in one worker context amortizes the shared understanding + one build
    # across the whole cluster (the reimpl-first strategy, cluster-generalized).
    n = args.sessions * args.per_session
    picked = scored[:n]
    batches = cluster_candidates(picked, max_size=args.cluster_max,
                                 prox_gap=args.cluster_gap, model=args.model)

    rnd = args.round if args.round is not None else next_round_number()
    plan = {
        "round": rnd,
        "supply": {
            "runnable_today": len(runnable),
            "needs_argtype": len(needs),
            "scored_eligible": len(scored),
            "scheduled": len(picked),
        },
        "dropped": dropped,
        "batches": batches,
        "argtype_gaps": argtype_gaps(needs),
    }

    if args.peek:
        out = "(peek — not written)"
    else:
        os.makedirs(ROUNDS_DIR, exist_ok=True)
        out = os.path.join(ROUNDS_DIR, f"round_{rnd}_plan.json")
        with open(out, "w", encoding="utf-8") as f:
            json.dump(plan, f, indent=2)
        out = os.path.relpath(out, ROOT)

    fam = sum(1 for b in batches if b.get("kind") == "family")
    mix = sum(1 for b in batches if b.get("kind") == "mixed")
    print(f"round {rnd}: {len(picked)} scheduled across {len(batches)} clusters "
          f"({fam} family + {mix} mixed) "
          f"(eligible {len(scored)} / runnable {len(runnable)}; "
          f"argtype-blocked {len(needs)})")
    print(f"  dropped: {dropped}")
    print(f"  top arg_type gap: {plan['argtype_gaps'][0] if plan['argtype_gaps'] else 'none'}")
    print(f"  wrote {out}")
    return 0


if __name__ == "__main__":
    sys.exit(main())
