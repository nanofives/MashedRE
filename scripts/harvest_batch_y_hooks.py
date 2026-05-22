#!/usr/bin/env python3
"""Harvest batch_y plate frontmatter -> hooks.csv C1 rows.

Run AFTER ghidra-sweep-20260519-1818 has drained the 6 SCRIBE_QUEUE rows
into master Ghidra. Each plate becomes one hooks.csv row at C1 / mapped.

Buckets covered:
- batch-y-s1 bucket_00449ba0 (80 plates +1 padding) -> gameplay
    (powerup-bezier-adjacent + per-player-audio-slot-subdomain)
    Skip padding-byte 0x004660fa (listing-level only, not a real function).
- batch-y-s2 bucket_00466100 (80 plates)            -> gameplay
    MIXED: vehicle-physics-core + scripted-entity-bytecode +
    scenery-actor-pool + WorldDestroy. All three -> gameplay.
- batch-y-s3 bucket_004d7ac0 (80 plates)            -> render
    RW frame/light/world (many plates carry RW-catalog-derived names).
- batch-y-s4 bucket_00554010 (80 plates +1 padding +1 extra) -> gameplay
    Subsystem REFUTED (was font-canvas-context hypothesis); actually
    collision-bvh-octree + physics-math tail.
    Skip padding-byte 0x00565cce.
- batch-y-s5 bucket_00565d50 (80 plates)            -> gameplay
    physics-engine-deep-guts (geometry+collision+island+broadphase+
    solver+velocity). Queue RVAs are stale - disk is ground truth.
- batch-y-s6 bucket_005a5020 (80 plates)            -> TRIMODAL
    * 4 plates 0x005a5020..0x005a5820 -> third-party-library[qhull-2002.1]
    * 5 plates 0x005a5910..0x005a5f60 -> audio (refcount/ctor helpers)
    * 71 plates 0x005a6030..0x005b81fe -> audio (stream + EA-XA + IMA ADPCM)

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

# Padding-byte plates to skip entirely (not real functions; listing-level only).
PADDING_PLATES: set[str] = {
    "004660fa",  # batch_y s1
    "00565cce",  # batch_y s4
}

# Per-plate s6 classification: qhull range vs audio range.
# Inclusive RVA bounds (lower-hex, no prefix).
S6_QHULL_RVAS = {"005a5020", "005a52c0", "005a56e0", "005a5820"}
S6_AUDIO_REFCOUNT_RVAS = {
    "005a5910", "005a5b30", "005a5d60", "005a5d90", "005a5f60"
}

S6_QHULL_NOTE = (
    "library_residue qhull-2002.1 (s6 batch_y; print/input residue tail "
    "0x005a5020..0x005a5820); static-linked third-party qhull"
)
S6_AUDIO_REFCOUNT_NOTE = (
    "batch_y s6 generic refcount/ctor-chain helpers (0x005a5910..0x005a5f60); "
    "support glue for the audio cluster that follows"
)
S6_AUDIO_STREAM_NOTE = (
    "batch_y s6 audio stream + codec pipeline (EA-XA + IMA ADPCM); main "
    "audio backend body 0x005a6030..0x005b81fe"
)


def classify_s6(rva: str) -> tuple[str, str]:
    """Returns (subsystem, note) for a single s6 plate."""
    if rva in S6_QHULL_RVAS:
        return ("third-party-library[qhull-2002.1]", S6_QHULL_NOTE)
    if rva in S6_AUDIO_REFCOUNT_RVAS:
        return ("audio", S6_AUDIO_REFCOUNT_NOTE)
    return ("audio", S6_AUDIO_STREAM_NOTE)


PLATE_BUCKETS: dict[str, tuple[str, str, str]] = {
    "bucket_00449ba0": (
        "batch-y-s1",
        "gameplay",
        "batch_y s1 powerup-bezier-adjacent + per-player-audio-slot-subdomain; "
        "gameplay effect/audio glue around player slots",
    ),
    "bucket_00466100": (
        "batch-y-s2",
        "gameplay",
        "batch_y s2 MIXED gameplay: vehicle-physics-core "
        "(0x00466100..0x0046dd90) + scripted-entity-bytecode "
        "(0x00471430..0x00474890) + scenery-actor-pool + WorldDestroy "
        "(0x0047b480..0x0047fc33)",
    ),
    "bucket_004d7ac0": (
        "batch-y-s3",
        "render",
        "batch_y s3 RW frame/light/world; many plates carry RW-catalog-derived "
        "names from the agent (preserved via name_in_ghidra/proposed_name)",
    ),
    "bucket_00554010": (
        "batch-y-s4",
        "gameplay",
        "batch_y s4 collision-bvh-octree + physics-math tail (Jacobi "
        "eigenvalue, Givens, covariance-to-quaternion, bounding-sphere "
        "merge); pre-batch font-canvas-context hypothesis refuted",
    ),
    "bucket_00565d50": (
        "batch-y-s5",
        "gameplay",
        "batch_y s5 physics-engine deep guts (geometry+collision+"
        "island-builder+broadphase+solver+velocity); per-plate "
        "subsystem_observed frontmatter has finer-grained category",
    ),
    "bucket_005a5020": (
        "batch-y-s6",
        "MIXED",  # sentinel -> classify_s6 per plate
        "",
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
    print(f"hooks.csv: {len(existing)} unique mapped RVAs (pre-batch_y)")

    added: list[list[str]] = []
    per_bucket_added: dict[str, int] = {}
    s6_breakdown: dict[str, int] = {}
    drift_higher: list[tuple[str, str, str]] = []
    drift_equal_or_lower: list[tuple[str, str, str]] = []
    skipped_padding: list[tuple[str, str]] = []
    no_data: list[Path] = []

    def emit(row: list[str], bucket_tag: str) -> None:
        added.append(row)
        per_bucket_added[bucket_tag] = per_bucket_added.get(bucket_tag, 0) + 1

    for bucket, (session_id, subsystem, default_note) in PLATE_BUCKETS.items():
        bdir = ROOT / "re" / "analysis" / bucket
        if not bdir.exists():
            print(f"  bucket missing: {bdir}")
            continue
        plates = sorted(
            p for p in bdir.glob("*.md")
            if not p.name.startswith("_") and p.name.lower() != "readme.md"
        )
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

            if rva in PADDING_PLATES:
                skipped_padding.append((rva, bucket))
                continue

            if rva in existing:
                ec = existing_conf.get(rva, "")
                if ec in ("C2", "C3", "C4"):
                    drift_higher.append((rva, bucket, ec))
                    continue
                drift_equal_or_lower.append((rva, bucket, ec))
                continue

            if subsystem == "MIXED":
                sub, note = classify_s6(rva)
                s6_breakdown[sub] = s6_breakdown.get(sub, 0) + 1
            else:
                sub, note = subsystem, default_note

            name = (
                data.get("proposed_name")
                or data.get("name_in_ghidra")
                or data.get("name")
                or f"FUN_{rva}"
            )

            file_col = f"re/analysis/{bucket}/{plate.name}"
            row = [
                rva, name, sub, "C1", "mapped",
                file_col, session_id, "", note,
            ]
            emit(row, bucket)

    # Dedupe newly-added against itself.
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

    if s6_breakdown:
        print(f"\n=== s6 trimodal breakdown ===")
        for k in sorted(s6_breakdown):
            print(f"  {k}: {s6_breakdown[k]}")

    if skipped_padding:
        print(f"\n=== Skipped padding-byte plates ({len(skipped_padding)}) ===")
        for rva, bucket in skipped_padding:
            print(f"  0x{rva} (in {bucket}) -> SKIP (listing-level padding)")

    if drift_higher:
        print(f"\n=== Drift cases - already at C2+ ({len(drift_higher)}) ===")
        for rva, bucket, ec in drift_higher[:20]:
            print(f"  0x{rva} (in {bucket}, existing={ec}) -> SKIP")
        if len(drift_higher) > 20:
            print(f"  ... + {len(drift_higher) - 20} more")
    if drift_equal_or_lower:
        print(f"\n=== Drift cases - already at <=C1 ({len(drift_equal_or_lower)}) ===")
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
        backup = HOOKS.with_suffix(".csv.bak.batch_y")
        shutil.copy2(HOOKS, backup)
        new_path = HOOKS.with_suffix(".csv.new")
        with open(HOOKS, encoding="utf-8-sig") as src, open(
            new_path, "w", encoding="utf-8", newline=""
        ) as dst:
            dst.write(src.read())
            w = csv.writer(dst, lineterminator="\n")
            for r in deduped:
                w.writerow(r)
        new_path.replace(HOOKS)
        print(f"\nAppended {len(deduped)} rows to {HOOKS.relative_to(ROOT)}")
        print(f"Backup at {backup.relative_to(ROOT)}")
    else:
        print("\nNo rows appended.")

    new_existing, _ = load_existing_rvas()
    pct_pre = 100.0 * len(existing) / 5800
    pct_post = 100.0 * len(new_existing) / 5800
    print(f"\nCoverage: {len(existing)} -> {len(new_existing)} unique RVAs of 5800")
    print(f"          {pct_pre:.1f}% -> {pct_post:.1f}% mapped")

    return 0


if __name__ == "__main__":
    sys.exit(main())
