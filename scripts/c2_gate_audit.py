#!/usr/bin/env python3
"""c2_gate_audit.py — classify the first-party C2 pool by promotion blocker.

Heuristic notes-text classifier over hooks.csv. Produces
re/analysis/C2_GATE_AUDIT_<date>.md with per-gate denominators so
promotion batches can be sized against the ~30% predicted-yield rule
(ROADMAP v2) instead of guessed.

IMPORTANT: this is process tooling over tracker prose, not RE evidence.
Every count is a keyword-heuristic ESTIMATE; the report says so. Rows can
match multiple gates; the PRIMARY gate is the first match in GATE_ORDER
(most-specific / most-blocking first).
"""

import csv
import datetime
import re
import sys
from collections import Counter, defaultdict
from pathlib import Path

REPO = Path(__file__).resolve().parent.parent
HOOKS_CSV = REPO / "hooks.csv"
OUT_DIR = REPO / "re" / "analysis"

# (gate, regex, meaning) — first match in this order is the primary gate.
GATES = [
    ("abi-limited",
     re.compile(r"naked|SEH_prolog|SEH_epilog|harness-limited|non-standard ABI|compiler-injected", re.I),
     "naked asm / compiler-helper ABI; JMP-hook unsafe or harness cannot drive"),
    ("demoted-needs-reimpl",
     re.compile(r"DEMOTED C[34]->C2.*(no active RH_ScopedInstall|NOT a registered hook|Needs a reimpl)", re.I | re.S),
     "former C3/C4 demoted in re-validation only because no installed reimpl exists; analysis already done"),
    ("demoted-crasher",
     re.compile(r"DEMOTED C[34]->C2.*(crash|corrupt)", re.I | re.S),
     "former C3/C4 demoted because the reimpl crashes the canonical scenario; needs reimpl repair"),
    ("com-directshow",
     re.compile(r"DirectShow|IGraphBuilder|IMediaControl|IBaseFilter|\bCOM\b|CoCreateInstance", re.I),
     "COM/DirectShow object lifecycle; needs a bespoke COM harness"),
    ("live-state",
     re.compile(r"live[- ]state|live arrays?|menu-attach|degenerate|pool unavailable|requires (a )?race|in-race state|runtime state|attach-time|zero at (the )?menu", re.I),
     "inputs/state are zero or absent at menu-attach; needs scenario-attach (race state)"),
    ("oversize",
     re.compile(r"oversize|\b\d{4,5}B\b|~?\d+(\.\d+)?\s?KB\b", re.I),
     "function too large for bit-identity reimpl in one session (>=~2KB)"),
    ("thiscall-getter",
     re.compile(r"(thiscall|__thiscall|ecx).{0,80}(getter|setter|accessor|field)|"
                r"(getter|setter|accessor).{0,40}(this\+|struct field|member)", re.I),
     "thiscall struct-ptr field accessor; unlocked wholesale by ONE new arg_type (pivot B)"),
    ("arg-shape",
     re.compile(r"arg_type|RW struct|Rw[A-Z]\w*\s?\*|struct[- ]ptr|vtable|callback|function pointer|fn[- ]ptr", re.I),
     "argument shape has no harness handler yet; needs an arg_type extension"),
    ("crt-fiddb",
     re.compile(r"FidDB|\bCRT\b|VS2003|VS MSVC", re.I),
     "CRT/compiler-runtime band; verbatim-port value is low, promote only on demand"),
]
GATE_ORDER = [g[0] for g in GATES] + ["unclassified"]

# Shape lanes — applied to rows with NO hard gate. Each maps to the harness
# lane that would test it. First match wins (most lane-specific first).
SHAPES = [
    ("struct-getter",
     re.compile(r"getter|setter|accessor|returns \*|reads offset|writes offset|"
                r"(this|param_1|ecx)\s*\+\s*0x", re.I),
     "field accessor over a struct/global — pivot-B arg_type (struct-ptr getter) lane"),
    ("pool-listop",
     re.compile(r"\bpool\b|arena|\bring\b|linked.list|list.op|free.list|slot alloc", re.I),
     "pool/list/arena op — hand-built-ring harness class (see audio list-op precedent)"),
    ("vtable-dispatch",
     re.compile(r"vtable|DrawIndexedPrimitive|DrawPrimitive|SetRenderState|SetRS", re.I),
     "D3D9/RW vtable dispatch — needs a live device; draw_quad_observe-style fingerprint lane"),
    ("rw-api-orchestrator",
     re.compile(r"\bRw[A-Z]|\bRp[A-Z]|\bRt[A-Z]|ForAll|RpWorldSector|RpAtomic|RpClump", re.I),
     "first-party code orchestrating RW APIs — RW-struct-ptr arg_types or nav/scenario harness"),
    ("scalar-leaf",
     re.compile(r"\bleaf\b|no callees|^\s*array zero|\b\d{1,3}B\b", re.I),
     "small scalar leaf — standard registry lane (pool believed drained; verify before batching)"),
]
SHAPE_ORDER = [s[0] for s in SHAPES]

GLOBAL_RX = re.compile(r"DAT_00[0-9a-f]{6}", re.I)

# Subsystems whose state is populated during a race — the scenario-attach
# lane's natural constituency (proxy axis, independent of keyword hits).
RACE_STATE_SUBSYSTEMS = {"gameplay", "vehicle", "ai", "camera", "race", "hud", "particle"}


def classify(notes: str):
    notes = notes or ""
    hits = [name for name, rx, _ in GATES if rx.search(notes)]
    if hits:
        return hits[0], hits, True
    for name, rx, _ in SHAPES:
        if rx.search(notes):
            return name, [name], False
    return "unclassified", [], False


def main():
    with open(HOOKS_CSV, newline="", encoding="utf-8", errors="replace") as f:
        rows = list(csv.DictReader(f))

    c2 = [r for r in rows
          if (r.get("confidence") or "").strip() == "C2"
          and "third-party" not in (r.get("subsystem") or "")]

    primary_count = Counter()
    all_hits_count = Counter()
    by_sub = defaultdict(Counter)
    gate_rows = defaultdict(list)
    status_x = Counter()
    globals_x = Counter()      # (primary, touches_globals) -> n
    hard_gated = 0

    for r in c2:
        notes = r.get("notes", "")
        primary, hits, is_hard = classify(notes)
        touches_globals = bool(GLOBAL_RX.search(notes or ""))
        r["_touches_globals"] = touches_globals
        if is_hard:
            hard_gated += 1
        primary_count[primary] += 1
        for h in hits:
            all_hits_count[h] += 1
        by_sub[(r.get("subsystem") or "?").strip()][primary] += 1
        gate_rows[primary].append(r)
        status_x[(primary, (r.get("status") or "?").strip())] += 1
        globals_x[(primary, touches_globals)] += 1

    today = datetime.date.today().isoformat()
    out = OUT_DIR / f"C2_GATE_AUDIT_{today}.md"

    lines = []
    w = lines.append
    w(f"# C2 gate audit — {today}")
    w("")
    w("Generated by `scripts/c2_gate_audit.py` (re-run any time; heuristic keyword")
    w("classifier over `hooks.csv` notes — **estimates, not evidence**; rows whose")
    w("notes don't mention their blocker land in `unclassified`).")
    w("")
    w(f"First-party C2 pool: **{len(c2)}** rows; {hard_gated} carry a recorded hard")
    w("gate (a past promotion attempt failed there); the rest are classified by code")
    w("shape into the harness lane that would test them.")
    w("")
    all_order = [g[0] for g in GATES] + SHAPE_ORDER + ["unclassified"]
    meanings = {name: meaning for name, _, meaning in GATES}
    meanings.update({name: meaning for name, _, meaning in SHAPES})
    meanings["unclassified"] = "notes carry no recognizable blocker or shape keyword — sample to subdivide"
    w("## Primary classification (hard gates first, then shape lanes)")
    w("")
    w("| Class | Rows | % | Lane / meaning |")
    w("|---|---:|---:|---|")
    for g in all_order:
        n = primary_count.get(g, 0)
        if n:
            kind = "gate" if g in [x[0] for x in GATES] else "shape"
            w(f"| {g} ({kind}) | {n} | {100*n/len(c2):.1f}% | {meanings[g]} |")
    w("")
    w("## Any-match counts (a row can hit several gates)")
    w("")
    w("| Gate | Rows mentioning it |")
    w("|---|---:|")
    for g, n in all_hits_count.most_common():
        w(f"| {g} | {n} |")
    w("")
    w("## Primary class × subsystem")
    w("")
    subs = sorted(by_sub, key=lambda s: -sum(by_sub[s].values()))
    gates_present = [g for g in all_order if primary_count.get(g)]
    w("| Subsystem | total | " + " | ".join(gates_present) + " |")
    w("|---|---:|" + "---:|" * len(gates_present))
    for s in subs:
        row = by_sub[s]
        w(f"| {s} | {sum(row.values())} | " + " | ".join(str(row.get(g, 0)) for g in gates_present) + " |")
    w("")
    w("## Touches-globals axis (scenario-attach relevance)")
    w("")
    w("A row that reads/writes `DAT_*` globals diffs degenerately when those globals")
    w("are zero at menu-attach; attaching at race state populates them.")
    w("")
    w("| Class | touches globals | doesn't |")
    w("|---|---:|---:|")
    for g in gates_present:
        w(f"| {g} | {globals_x.get((g, True), 0)} | {globals_x.get((g, False), 0)} |")
    w("")
    w("## Lane sizings")
    w("")
    reearn = gate_rows.get("demoted-needs-reimpl", [])
    w(f"- **Cheap re-earns (demoted-needs-reimpl): {len(reearn)} rows.** Analysis exists;")
    w("  each needs only a reimpl + standard diff. Highest value-per-effort bucket.")
    crash = gate_rows.get("demoted-crasher", [])
    w(f"- **Reimpl repairs (demoted-crasher): {len(crash)} rows.** Reimpl exists but broke")
    w("  the canonical scenario; root-cause work, not harness work.")
    tg = gate_rows.get("struct-getter", [])
    tg_hard = primary_count.get("thiscall-getter", 0)
    w(f"- **Pivot B (struct-ptr-getter arg_type): {len(tg)} shape-classified rows")
    w(f"  (+{tg_hard} hard-gated).** One arg_type unlocks the whole bucket — well past")
    w("  the ~30% predicted-yield bar if even a third fit the handler.")
    sa_pool = [r for r in c2 if r["_touches_globals"]
               and (r.get("subsystem") or "").strip() in RACE_STATE_SUBSYSTEMS]
    ls_any = all_hits_count.get("live-state", 0)
    w(f"- **Scenario-attach lane: {len(sa_pool)} rows touch `DAT_*` globals AND live in")
    w(f"  state-rich subsystems {sorted(RACE_STATE_SUBSYSTEMS)}** ({ls_any} additionally")
    w("  carry an explicit live-state failure note). These diff degenerately at")
    w("  menu-attach; attach at race state to populate their inputs.")
    w("")
    w("## Status × gate (does a reimpl already exist?)")
    w("")
    w("`impl`/`hooked`/`verified` status = code exists; `mapped`/`analyzed` = analysis only.")
    w("")
    w("| Class | mapped | analyzed | impl | hooked | verified | other |")
    w("|---|---:|---:|---:|---:|---:|---:|")
    statuses = ["mapped", "analyzed", "impl", "hooked", "verified"]
    for g in gates_present:
        cells = [status_x.get((g, s), 0) for s in statuses]
        other = primary_count.get(g, 0) - sum(cells)
        w(f"| {g} | " + " | ".join(str(c) for c in cells) + f" | {other} |")
    w("")
    w("## Scenario-attach candidate RVAs (globals × state-rich subsystem)")
    w("")
    w(", ".join(f"`{r['rva']}`" for r in sorted(sa_pool, key=lambda r: r["rva"])))
    w("")
    w("## Row lists (RVAs per gate, for batch curation)")
    w("")
    for g in gates_present:
        rs = gate_rows.get(g, [])
        if not rs or g == "unclassified":
            continue
        w(f"### {g} ({len(rs)})")
        w("")
        w(", ".join(f"`{r['rva']}`" for r in sorted(rs, key=lambda r: r["rva"])))
        w("")
    unc = gate_rows.get("unclassified", [])
    if unc:
        w(f"### unclassified ({len(unc)}) — first 40 for sampling")
        w("")
        for r in sorted(unc, key=lambda r: r["rva"])[:40]:
            note = (r.get("notes") or "").replace("|", "/")[:100]
            w(f"- `{r['rva']}` {r.get('subsystem','?')} — {note}")
        w("")

    out.write_text("\n".join(lines), encoding="utf-8")
    print(f"wrote {out}  ({len(c2)} C2 rows)")
    for g in all_order:
        if primary_count.get(g):
            print(f"  {g:24s} {primary_count[g]:5d}")


if __name__ == "__main__":
    sys.exit(main())
