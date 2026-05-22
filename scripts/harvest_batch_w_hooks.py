#!/usr/bin/env python3
"""Harvest batch_w plate frontmatter -> hooks.csv C1 rows.

Run AFTER ghidra-sweep-20260519-0330 has drained the 6 SCRIBE_QUEUE rows
into master Ghidra. Each plate becomes one hooks.csv row at C1 / mapped.
All six batch_w buckets are pure plate buckets (no HALT-only buckets this
round), so this is simpler than the batch_v harvester — no HALT-RVA
candidate lists, no spot-confirmed-name overrides, no bimodal splits.

Buckets covered:
- batch-w-s1 bucket_0041e140 (80 plates) -> gameplay (MIXED[player-slot-state-accessors+gameplay-math-leaves])
- batch-w-s2 bucket_00474d80 (80 plates) -> render   (MIXED[gfx-particle-emitter+decal-shockwave-trail+geometry-plane-clip+anim-keyframe-interp+script-vm-handler-bank])
- batch-w-s3 bucket_004bdcc0 (80 plates) -> third-party-library[lua-5.0] (lparser+lundump+lfunc+lcode)
- batch-w-s4 bucket_004fcb51 (80 plates) -> third-party-library[d3dx9-shader-compiler] (preprocessor+byacc-parser+lexer+diagnostics+symbol-hashtable)
- batch-w-s5 bucket_00557fb0 (80 plates) -> gameplay (MIXED[physics-broad-phase-and-rigid-body+text-renderer-2d+actor-update-scheduler])
- batch-w-s6 bucket_005ae170 (80 plates) -> audio    (audio-rws-source+sequencer-segment+custom-allocator-arena)

Output:
- Appends to hooks.csv (one row per new plated RVA)
- Prints summary: per-bucket added / drift-skipped / no-frontmatter
"""

from __future__ import annotations

import csv
import re
import shutil
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
HOOKS = ROOT / "hooks.csv"

# -----------------------------------------------------------------------------
# Per-bucket (session_id, subsystem token, default note). The note is shared
# across all plates in the bucket; per-RVA detail lives in the plate file itself.
# -----------------------------------------------------------------------------

PLATE_BUCKETS: dict[str, tuple[str, str, str]] = {
    "bucket_0041e140": (
        "batch-w-s1",
        "gameplay",
        "batch_w s1 MIXED[player-slot-state-accessors+gameplay-math-leaves]; "
        "small getters/setters + arithmetic leaves around player/vehicle slot state",
    ),
    "bucket_00474d80": (
        "batch-w-s2",
        "render",
        "batch_w s2 MIXED[gfx-particle-emitter+decal-shockwave-trail+geometry-plane-clip+"
        "anim-keyframe-interp+script-vm-handler-bank]; render-heavy mixed bucket",
    ),
    "bucket_004bdcc0": (
        "batch-w-s3",
        "third-party-library[lua-5.0]",
        "library_residue lua-5.0 (s3 batch_w; lparser+lundump+lfunc+lcode); "
        "static-linked third-party; per-plate canonical proposed_name where attested",
    ),
    "bucket_004fcb51": (
        "batch-w-s4",
        "third-party-library[d3dx9-shader-compiler]",
        "library_residue d3dx9-shader-compiler (s4 batch_w; HLSL preprocessor+byacc-parser+"
        "lexer+diagnostics+symbol-hashtable); static-linked third-party",
    ),
    "bucket_00557fb0": (
        "batch-w-s5",
        "gameplay",
        "batch_w s5 MIXED[physics-broad-phase-and-rigid-body+text-renderer-2d+"
        "actor-update-scheduler]; physics core + 2D text glue",
    ),
    "bucket_005ae170": (
        "batch-w-s6",
        "audio",
        "batch_w s6 audio-rws-source+sequencer-segment+custom-allocator-arena; "
        "RenderWare audio path + sequencer + arena allocator",
    ),
}

# -----------------------------------------------------------------------------
# Plate parser
# -----------------------------------------------------------------------------


def parse_plate(path: Path) -> dict | None:
    try:
        txt = path.read_text(encoding="utf-8")
    except Exception:
        return None
    if not txt.startswith("---"):
        return None
    end = txt.find("\n---", 3)
    if end == -1:
        return None
    fm = txt[3:end].strip()
    data: dict[str, str] = {}
    for line in fm.splitlines():
        line = line.rstrip()
        if not line or line.startswith("-") or line.startswith(" "):
            continue
        if ":" in line:
            k, _, v = line.partition(":")
            data[k.strip()] = v.strip()
    return data


def load_existing_rvas() -> tuple[set[str], dict[str, str]]:
    """Returns (set of RVA, map of RVA -> existing confidence)."""
    seen: set[str] = set()
    conf: dict[str, str] = {}
    hex8 = re.compile(r"^(?:0x)?([0-9a-fA-F]{8})$")
    with open(HOOKS, encoding="utf-8-sig") as f:
        rdr = csv.reader(f)
        for r in rdr:
            if not r:
                continue
            m = hex8.match(r[0].strip())
            if m:
                rva = m.group(1).lower()
                seen.add(rva)
                if len(r) >= 4:
                    conf[rva] = r[3].strip()
    return seen, conf


def main() -> int:
    existing, existing_conf = load_existing_rvas()
    print(f"hooks.csv: {len(existing)} unique mapped RVAs (pre-batch_w)")

    added: list[list[str]] = []
    per_bucket_added: dict[str, int] = {}
    drift_higher: list[tuple[str, str, str]] = []  # (rva, bucket, existing_conf)
    drift_equal_or_lower: list[tuple[str, str, str]] = []
    no_data: list[Path] = []

    def emit(row: list[str], bucket_tag: str) -> None:
        added.append(row)
        per_bucket_added[bucket_tag] = per_bucket_added.get(bucket_tag, 0) + 1

    for bucket, (session_id, subsystem, default_note) in PLATE_BUCKETS.items():
        bdir = ROOT / "re" / "analysis" / bucket
        if not bdir.exists():
            print(f"  bucket missing: {bdir}")
            continue
        plates = sorted(p for p in bdir.glob("*.md") if not p.name.startswith("_"))
        for plate in plates:
            data = parse_plate(plate)
            if not data:
                no_data.append(plate)
                continue
            rva_raw = data.get("rva", "")
            m = re.search(r"([0-9a-fA-F]{8})", rva_raw)
            if not m:
                no_data.append(plate)
                continue
            rva = m.group(1).lower()
            if rva in existing:
                ec = existing_conf.get(rva, "")
                # Skip if already at C2+ (don't demote).
                if ec in ("C2", "C3", "C4"):
                    drift_higher.append((rva, bucket, ec))
                    continue
                # Equal/lower (C0/C1/blank): note as drift-promote-equal but skip
                # since a row already exists. We don't try to mutate existing rows
                # in this pass (precedent: batch_v had 0 drift; this is defensive).
                drift_equal_or_lower.append((rva, bucket, ec))
                continue

            # Prefer proposed_name if present (s3 lua bucket attaches lparser_fornum
            # etc.); else name_in_ghidra; else fall back to FUN_<rva>.
            name = (
                data.get("proposed_name")
                or data.get("name_in_ghidra")
                or data.get("name")
                or f"FUN_{rva}"
            )

            file_col = f"re/analysis/{bucket}/{plate.name}"
            row = [
                rva, name, subsystem, "C1", "mapped",
                file_col, session_id, "", default_note,
            ]
            emit(row, bucket)

    # Dedupe newly-added against itself (defense against accidental dup).
    seen_added: set[str] = set()
    deduped: list[list[str]] = []
    for r in added:
        if r[0] in seen_added:
            continue
        seen_added.add(r[0])
        deduped.append(r)

    print(f"\n=== Per-bucket added counts ===")
    for k in sorted(per_bucket_added):
        print(f"  {k}: {per_bucket_added[k]}")
    total = sum(per_bucket_added.values())
    print(f"  TOTAL: {total} (deduped-internal: {len(deduped)})")

    if drift_higher:
        print(f"\n=== Drift cases — already at C2+ ({len(drift_higher)}) ===")
        for rva, bucket, ec in drift_higher[:20]:
            print(f"  0x{rva} (in {bucket}, existing={ec}) -> SKIP")
        if len(drift_higher) > 20:
            print(f"  ... + {len(drift_higher) - 20} more")
    if drift_equal_or_lower:
        print(f"\n=== Drift cases — already at <=C1 ({len(drift_equal_or_lower)}) ===")
        for rva, bucket, ec in drift_equal_or_lower[:20]:
            print(f"  0x{rva} (in {bucket}, existing={ec or 'blank'}) -> SKIP (row exists)")
        if len(drift_equal_or_lower) > 20:
            print(f"  ... + {len(drift_equal_or_lower) - 20} more")
    if no_data:
        print(f"\n=== No-frontmatter / missing plates ({len(no_data)}) ===")
        for p in no_data[:5]:
            try:
                print(f"  {p.relative_to(ROOT)}")
            except ValueError:
                print(f"  {p}")

    if deduped:
        # Atomic write via tempfile + replace.
        backup = HOOKS.with_suffix(".csv.bak.batch_w")
        shutil.copy2(HOOKS, backup)
        new_path = HOOKS.with_suffix(".csv.new")
        # Read original, write to .new, append rows.
        with open(HOOKS, encoding="utf-8-sig") as src, open(
            new_path, "w", encoding="utf-8", newline=""
        ) as dst:
            dst.write(src.read())
            w = csv.writer(dst, lineterminator="\n")
            for r in deduped:
                w.writerow(r)
        # Replace (Windows-safe via os.replace under the hood for Path.replace).
        new_path.replace(HOOKS)
        print(f"\nAppended {len(deduped)} rows to {HOOKS.relative_to(ROOT)}")
        print(f"Backup at {backup.relative_to(ROOT)}")
    else:
        print("\nNo rows appended.")

    # Post-write coverage stats.
    new_existing, _ = load_existing_rvas()
    pct_pre = 100.0 * len(existing) / 5800
    pct_post = 100.0 * len(new_existing) / 5800
    print(f"\nCoverage: {len(existing)} -> {len(new_existing)} unique RVAs of 5800")
    print(f"          {pct_pre:.1f}% -> {pct_post:.1f}% mapped")

    return 0


if __name__ == "__main__":
    sys.exit(main())
