#!/usr/bin/env python3
"""Generic generator for Opus 1M C1-brute-force batch files.

Usage:
    py -3.12 scripts/gen_batch_letter.py <letter>

Writes batch_<letter>.txt at project root. Letter must be unused.
"""
from __future__ import annotations

import csv
import re
import sys
from pathlib import Path
from datetime import date

ROOT = Path(__file__).resolve().parents[1]

CRT_LO = 0x004a0000
CRT_HI = 0x004b4000


def main(letter: str) -> int:
    letter = letter.strip().lower()
    if not re.fullmatch(r"[a-z]", letter):
        print(f"ERROR: letter must be a-z, got {letter!r}")
        return 1

    out = ROOT / f"batch_{letter}.txt"
    if out.exists():
        print(f"ERROR: {out.name} already exists — pick a different letter")
        return 2

    # Inputs
    inv: list[dict] = []
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

    unmapped = sorted(
        [f for f in inv if f["rva_int"] not in mapped
                       and not (CRT_LO <= f["rva_int"] < CRT_HI)],
        key=lambda f: f["rva_int"],
    )

    N_SESSIONS = 6
    K_PER_SESS = 80
    TOTAL = N_SESSIONS * K_PER_SESS

    if len(unmapped) < TOTAL:
        print(f"WARNING: only {len(unmapped)} unmapped fns; batch may be partial")
        # still proceed

    step = max(1, (len(unmapped) - K_PER_SESS) // (N_SESSIONS - 1))
    sessions = []
    for i in range(N_SESSIONS):
        start = min(i * step, len(unmapped) - K_PER_SESS)
        chunk = unmapped[start:start + K_PER_SESS]
        if not chunk:
            print(f"WARNING: session {i+1} has no candidates; pool exhausted")
            continue
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

    L: list[str] = []
    def w(line: str = "") -> None:
        L.append(line)

    w("=" * 78)
    w(f"batch_{letter}.txt  —  Mashed C1 brute-force  —  {TODAY}")
    w("=" * 78)
    w()
    w("Mode:           Opus 1M first-pass C0/unmapped -> C1 discovery")
    w(f"Sessions:       {N_SESSIONS}")
    w(f"RVAs per sess:  {K_PER_SESS}  (Opus 1M hard cap 100; default 80)")
    w(f"Total RVAs:     {sum(len(s['chunk']) for s in sessions)}")
    w(f"Unmapped pool:  {len(unmapped)} (after CRT-band exclusion)")
    w("Pool slots:     Mashed_pool1..Mashed_pool6 (verify free at launch)")
    w("Anchor binary:  original/MASHED.exe.unpatched SHA-256 (patched MASHED.exe")
    w("                differs by design; anchor lives on .unpatched per CLAUDE.md)")
    w("                BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E")
    w("Pool dir:       C:/Users/maria/Desktop/Proyectos/Mashed/mashed_pool")
    w()
    w("Read this BEFORE starting any session:")
    w("- .claude/skills/discover-c1-batch/SKILL.md  (Opus 1M caps section)")
    w("- .claude/skills/ghidra-pool/SKILL.md         (acquire/release flow)")
    w("- re/CONFIDENCE.md                            (C0 -> C1 evidence rubric)")
    w("- re/SESSION_RULES.md (search for scribe-queue) (queue-row format)")
    w()
    w("RW reference catalog (876 RW names by module — use during decomp):")
    w("  re/analysis/plans/rw_function_catalog.tsv")
    w()
    w("Already-named library-residue ranges (DON'T re-discover):")
    w("  0x004a0000..0x004b3fff  — MSVC CRT (FidDB-attested, pre-excluded)")
    w("  0x0051e6f0..0x0052c49d  — libpng + zlib (batch_t s4 halt; library-tagged)")
    w("  0x00583f10..0x005913c0  — qhull 2002.1 (batch_t s5; library-tagged)")
    w("  0x005a0e00..0x005a5017  — qhull print* (Ghidra FidDB; library-tagged)")
    w()
    w("If your bucket lands inside one of these ranges, halt per the STOP-AND-ASK")
    w("library-residue clause.")
    w()

    for s in sessions:
        idx = s["idx"]
        pool = s["pool"]
        bucket = s["bucket_name"]
        w()
        w("=" * 78)
        w(f"Session {idx} — {bucket}  (FIRST-PASS, {len(s['chunk'])}-RVA discovery)")
        w("=" * 78)
        w()
        w(f"Pool slot:    Mashed_pool{pool} (pre-assigned)")
        w(f"Model:        Opus 4.7 (claude-opus-4-7[1m]) — hard cap 100 RVAs (this session: {len(s['chunk'])})")
        w(f"Skill:        discover-c1-batch (first-pass; Opus 1M mode)")
        w(f"Bucket dir:   re/analysis/{bucket}/")
        w(f"Drains to:    re/SCRIBE_QUEUE.md (Queued); ghidra-sweep picks up later")
        w(f"Address span: 0x{s['rva_first']:08x} .. 0x{s['rva_last']:08x}  (~{s['total_size']:,} bytes of code)")
        w()
        w("# Mission")
        w(f"Discover and write C0->C1 plates for {len(s['chunk'])} unmapped functions in the")
        w(f"contiguous-RVA bucket anchored at 0x{s['rva_first']:08x}. These functions have")
        w("no row in hooks.csv and no analysis note. Working hypothesis on subsystem:")
        w("infer from xrefs during decomp; report in the queue row's `subsystem_observed=`.")
        w()
        w("# Pre-flight")
        w("```bash")
        w(f'export POOL_SLOT_FILE=".pool_slot_batch_{letter}_s{idx}"')
        w(f'bash scripts/ghidra_pool.sh acquire   # should return Mashed_pool{pool} if free')
        w('sha256sum original/MASHED.exe.unpatched  # MUST equal BDCAE0...EFD3C0E')
        w("```")
        w()
        w("# Workflow (Opus 1M — bulk-coherent per session)")
        w("Read all candidate RVAs first; then decomp in clusters of ~10-15 at a time.")
        w("Opus 1M context keeps all 80 functions' decomp + cross-refs in scope")
        w("simultaneously — exploit this for cross-function naming (recurring callees,")
        w("shared globals, repeated string patterns).")
        w()
        w("Per RVA:")
        w("  1. mcp__ghidra__function_at <rva>     (or listing_code_unit_at if no fn)")
        w("  2. mcp__ghidra__decomp_function       (the body)")
        w("  3. mcp__ghidra__function_callees      (depth-1 callees)")
        w("  4. mcp__ghidra__function_callers      (subsystem inference)")
        w("  5. Cross-ref the RW catalog: signature/calls/constants matching an RW")
        w("     API -> name and cite catalog row")
        w(f"  6. Write re/analysis/{bucket}/0x<rva>.md with:")
        w("     - frontmatter (rva, name_in_ghidra, size_bytes, confidence=C1,")
        w(f"       callees_depth1, callers_noted, opened_in_slot=Mashed_pool{pool},")
        w(f"       session_date={TODAY})")
        w("     - ## Mechanical description (bullets; no speculation; cite addresses)")
        w("     - ## Constants (table; cite RVA for each)")
        w("     - ## Uncertainties (bare [UNCERTAIN] markers; sweep assigns U-IDs)")
        w("     - ## Stubs encountered (bare [STUB] markers; sweep assigns S-IDs)")
        w()
        w("# Candidates")
        w("| # | RVA | Ghidra name | size |")
        w("|---|-----|-------------|------|")
        for i, f in enumerate(s["chunk"], 1):
            w(f"| {i} | 0x{f['rva_int']:08x} | {f['name']} | {f['size']} |")
        w()
        w("# Output (this session ONLY produces these — does NOT touch hooks.csv)")
        w(f"- {len(s['chunk'])} per-RVA .md plates in re/analysis/{bucket}/")
        w("- ONE queue row appended to re/SCRIBE_QUEUE.md (Queued section):")
        w(f"    {TODAY}  batch-{letter}-s{idx}  rvas=0x...,0x...  bucket=re/analysis/{bucket}")
        w("                  subsystem_observed=<X>  notes=<one-line>")
        w()
        w("# NO COMMITS, NO hooks.csv WRITES")
        w("- Do NOT mutate hooks.csv, STUBS.md, UNCERTAINTIES.md, or DEFERRED.md.")
        w("  Those are the sweep's / orchestrator's responsibility.")
        w(f"- Do NOT git commit. The orchestrating session commits after all {N_SESSIONS}")
        w("  sessions return.")
        w()
        w("# STOP-AND-ASK")
        w("- function_at returns null AND listing_code_unit_at also null -> skip + note")
        w("- Bucket-range turns out to be library residue (any of the 4 already-known")
        w("  ranges above OR newly-discovered FidDB-attested cluster) -> halt + report.")
        w("  Sessions should NOT promote library residue.")
        w("- RW catalog match found but signature mismatches Ghidra decomp -> flag, do")
        w("  NOT auto-name; defer to a re-classify pass.")
        w("- More than 5 candidates have catalogued [UNCERTAIN] markers above C2 -> halt")
        w("- Adjacent functions form a known struct shape (>= 3 share offsets) ->")
        w("  optionally produce re/analysis/structs/<name>.md alongside the plates")
        w()

    out.write_text("\n".join(L), encoding="utf-8")
    print(f"Wrote: {out}")
    print(f"Sessions: {len(sessions)} x {K_PER_SESS} RVAs = {sum(len(s['chunk']) for s in sessions)} total")
    print()
    print("Session anchors:")
    for s in sessions:
        print(f"  s{s['idx']}: 0x{s['rva_first']:08x}..0x{s['rva_last']:08x}  "
              f"bytes={s['total_size']:>6,}  pool=Mashed_pool{s['pool']}")
    return 0


if __name__ == "__main__":
    if len(sys.argv) != 2:
        print("Usage: gen_batch_letter.py <letter>")
        sys.exit(1)
    sys.exit(main(sys.argv[1]))
