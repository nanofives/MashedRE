#!/usr/bin/env python3
"""One-shot generator for batch_t.txt — Opus 1M C1 brute-force batch."""
from __future__ import annotations

import csv
import re
from pathlib import Path
from datetime import date

ROOT = Path(__file__).resolve().parents[1]

# --- inputs ----------------------------------------------------------------

inv = []
with open(ROOT / "re/analysis/function_inventory.csv", encoding="utf-8-sig") as f:
    rdr = csv.reader(f)
    next(rdr)
    for r in rdr:
        if r and r[0].strip():
            inv.append({
                "rva_int": int(r[0].strip(), 16),
                "rva": r[0].strip(),
                "name": r[1].strip(),
                "size": int(r[2].strip()),
            })

mapped: set[int] = set()
hex8 = re.compile(r"^(?:0x)?([0-9a-fA-F]{8})$")
with open(ROOT / "hooks.csv", encoding="utf-8-sig") as f:
    rdr = csv.reader(f)
    for r in rdr:
        if not r:
            continue
        m = hex8.match(r[0].strip())
        if m:
            mapped.add(int(m.group(1), 16))

CRT_LO = 0x004a0000
CRT_HI = 0x004b4000
unmapped = sorted(
    [f for f in inv if f["rva_int"] not in mapped
                   and not (CRT_LO <= f["rva_int"] < CRT_HI)],
    key=lambda f: f["rva_int"],
)

N_SESSIONS = 6
K_PER_SESS = 80
TOTAL = N_SESSIONS * K_PER_SESS

# 6 evenly-spaced anchors. Each session takes 80 consecutive unmapped fns.
step = (len(unmapped) - K_PER_SESS) // (N_SESSIONS - 1)
sessions = []
for i in range(N_SESSIONS):
    start = i * step
    chunk = unmapped[start:start + K_PER_SESS]
    sessions.append({
        "idx": i + 1,
        "pool": i + 1,
        "chunk": chunk,
        "rva_first": chunk[0]["rva_int"],
        "rva_last": chunk[-1]["rva_int"],
        "total_size": sum(f["size"] for f in chunk),
        "bucket_name": f"bucket_{chunk[0]['rva_int']:08x}",
    })

TODAY = date.today().isoformat()

# --- output ---------------------------------------------------------------

L: list[str] = []
def w(line: str = "") -> None:
    L.append(line)

w("=" * 78)
w(f"batch_t.txt  —  Mashed C1 brute-force  —  {TODAY}")
w("=" * 78)
w()
w("Mode:           Opus 1M first-pass C0/unmapped -> C1 discovery")
w(f"Sessions:       {N_SESSIONS}")
w(f"RVAs per sess:  {K_PER_SESS}  (Opus 1M hard cap 100; default 80)")
w(f"Total RVAs:     {TOTAL}")
w("Pool slots:     Mashed_pool1..Mashed_pool6 (0,9,10-15 locked at batch-gen time)")
w("Anchor binary:  original/MASHED.exe SHA-256")
w("                BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E")
w("Pool dir:       C:/Users/maria/Desktop/Proyectos/Mashed/mashed_pool")
w()
w("Read this BEFORE starting any session:")
w("- .claude/skills/discover-c1-batch/SKILL.md  (Opus 1M caps section)")
w("- .claude/skills/ghidra-pool/SKILL.md         (acquire/release flow)")
w("- re/CONFIDENCE.md                            (C0 -> C1 evidence rubric)")
w("- re/SESSION_RULES.md (search for scribe-queue) (queue-row format)")
w()
w("RW reference catalog (use during decomp inspection — 876 RW names by module):")
w("  re/analysis/plans/rw_function_catalog.tsv")
w()
w("RW string-anchor proposals (audit existing names against boot-log strings):")
w("  re/analysis/plans/rw_string_anchor_proposals.tsv")
w()
w("Prior art (read-only, vendored 2026-05-18):")
w("  re/prior_art/renderware/librw/                 (RW 3.x reimpl, MIT)")
w("  re/prior_art/renderware/gta-reversed-modern/   (GTA SA RW reversed, source-level)")
w()

for s in sessions:
    idx = s["idx"]
    pool = s["pool"]
    bucket = s["bucket_name"]
    w()
    w("=" * 78)
    w(f"Session {idx} — {bucket}  (FIRST-PASS, {K_PER_SESS}-RVA discovery)")
    w("=" * 78)
    w()
    w(f"Pool slot:    Mashed_pool{pool} (pre-assigned)")
    w(f"Model:        Opus 4.7 (claude-opus-4-7[1m]) — hard cap 100 RVAs (this session: {K_PER_SESS})")
    w(f"Skill:        discover-c1-batch (first-pass; Opus 1M mode)")
    w(f"Bucket dir:   re/analysis/{bucket}/")
    w(f"Drains to:    re/SCRIBE_QUEUE.md (Queued); ghidra-sweep picks up later")
    w(f"Address span: 0x{s['rva_first']:08x} .. 0x{s['rva_last']:08x}  (~{s['total_size']:,} bytes of code)")
    w()
    w("# Mission")
    w(f"Discover and write C0->C1 plates for {K_PER_SESS} unmapped functions in the")
    w(f"contiguous-RVA bucket anchored at 0x{s['rva_first']:08x}. These functions have")
    w("no row in hooks.csv and no analysis note. Working hypothesis on subsystem: infer")
    w("from xrefs during decomp; report the observed subsystem in the queue row's")
    w("`subsystem_observed=` field.")
    w()
    w("# Pre-flight")
    w("```bash")
    w(f'export POOL_SLOT_FILE=".pool_slot_batch_t_s{idx}"')
    w(f'bash scripts/ghidra_pool.sh acquire   # should return Mashed_pool{pool} if free')
    w('sha256sum original/MASHED.exe        # MUST equal BDCAE0...EFD3C0E')
    w("```")
    w()
    w("# Workflow (Opus 1M — bulk-coherent per session)")
    w("Read all 80 candidate RVAs first; then decomp them in clusters of ~10-15 at a")
    w("time. Opus 1M context lets you keep 80 functions' decomp + cross-refs in scope")
    w("simultaneously — exploit this for cross-function naming (recurring callees,")
    w("shared globals, repeated string patterns identify clusters).")
    w()
    w("Per RVA:")
    w("  1. mcp__ghidra__function_at <rva>     (or listing_code_unit_at if no fn)")
    w("  2. mcp__ghidra__decomp_function       (the body)")
    w("  3. mcp__ghidra__function_callees      (depth-1 callees)")
    w("  4. mcp__ghidra__function_callers      (subsystem inference)")
    w("  5. Cross-ref the RW catalog: if the function's signature/calls/constants")
    w("     match an RW API entry, name accordingly and cite catalog row.")
    w(f"  6. Write re/analysis/{bucket}/<rva>.md with:")
    w("     - frontmatter (rva, name, size_bytes, confidence=C1, callees_depth1,")
    w(f"       callers_noted, opened_in_slot=Mashed_pool{pool}, session_date={TODAY})")
    w("     - ## Mechanical description (bullets; no speculation; cite addresses)")
    w("     - ## Constants (table; cite RVA for each)")
    w("     - ## Uncertainties (file U-IDs to UNCERTAINTIES.md)")
    w("     - ## Stubs encountered (file S-IDs to STUBS.md)")
    w()
    w("# Candidates")
    w("| # | RVA | Ghidra name | size |")
    w("|---|-----|-------------|------|")
    for i, f in enumerate(s["chunk"], 1):
        w(f"| {i} | 0x{f['rva_int']:08x} | {f['name']} | {f['size']} |")
    w()
    w("# Output (this session ONLY produces these — does NOT touch hooks.csv)")
    w(f"- {K_PER_SESS} per-RVA .md plates in re/analysis/{bucket}/")
    w("- ONE queue row appended to re/SCRIBE_QUEUE.md (Queued section):")
    w(f"    {TODAY}  batch-t-s{idx}  rvas=0x...,0x...  bucket=re/analysis/{bucket}")
    w("                  subsystem_observed=<X>  notes=<one-line>")
    w("")
    w("# NO COMMITS, NO hooks.csv WRITES")
    w("- Do NOT mutate hooks.csv, STUBS.md, UNCERTAINTIES.md, or DEFERRED.md.")
    w("  Those are the sweep's responsibility (ghidra-sweep runs after all 6")
    w("  sessions return). Writing them here would collide with the parallel")
    w("  agents in batch_t (shared working tree).")
    w("- Do NOT git commit. The orchestrating session commits everything once")
    w("  all 6 agents return.")
    w()
    w("# STOP-AND-ASK")
    w("- function_at returns null AND listing_code_unit_at also null -> skip + note")
    w("- Bucket-range turns out to be CRT residue (FidDB-attested Library tags) -> halt,")
    w("  report. batch_t sessions should NOT promote CRT - separate library-tag drain.")
    w("- RW catalog match found but signature mismatches Ghidra decomp -> flag, do NOT auto-name")
    w("- More than 5 candidates have catalogued [UNCERTAIN] markers above C2 threshold -> halt")
    w("- Adjacent functions in the bucket form a known struct shape (>= 3 share offsets) ->")
    w("  optionally produce re/analysis/structs/<name>.md alongside the plates")
    w()

out = ROOT / "batch_t.txt"
out.write_text("\n".join(L), encoding="utf-8")

print(f"Wrote: {out}")
print(f"Sessions: {N_SESSIONS} x {K_PER_SESS} RVAs = {TOTAL} total")
print()
print("Session anchors:")
for s in sessions:
    print(f"  s{s['idx']}: 0x{s['rva_first']:08x}..0x{s['rva_last']:08x}  "
          f"bytes={s['total_size']:>6,}  pool=Mashed_pool{s['pool']}")
