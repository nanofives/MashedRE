#!/usr/bin/env python3
"""Harvest batch_z plate frontmatter -> hooks.csv C1 rows.

Run AFTER ghidra-sweep-20260520-0001 has drained the 5 SCRIBE_QUEUE rows
into master Ghidra. Each plate becomes one hooks.csv row at C1 / mapped.

Buckets covered:
- batch-z-s2 bucket_00565cd0 (80 plates)            -> TRIMODAL
    * 47 plates 0x00565cd0..0x0057ae30 -> gameplay
        physics-engine narrow-phase (GJK+EPA+SAT+conservative-advancement+
        contact-clipping+manifold-reduction+rigid-body-mass-properties)
    * 31 plates 0x0059b460..0x0059f5d0 -> third-party-library[qhull-2002.1]
        qhull-2002.1 LIBRARY RESIDUE extending past pre-batch bound
    * 2  plates 0x005b8200..0x005b8680 -> third-party-library[d3dx9-fx]
        D3DX9 / FX framework effect-state-block builder residue
- batch-z-s6 bucket_0041dc30 (80 plates)            -> frontend
    HUD/frontend/race-state core (HUD-glyph emitter + per-player score
    aggregation + menu input + copter asset loader + race state machine
    + screen-dim fade). Library-residue check NEGATIVE.
- batch-z-s5 bucket_004f022d (80 plates)            -> render
    RenderWare 3.x D3D9-backend (quaternion/matrix math + RW driver-
    interface init/teardown + RpGeometry/RpAtomic NATIVE-D3D9 stream R/W
    + NvTriStrip strip generator + material-list + D3D9 vertex-format
    converters + view/projection matrix infrastructure). NO D3DX9 spillover.
- batch-z-s3 bucket_0052df70 (80 plates)            -> render
    RenderWare-atomic / animation-glue (vertex-format allocator + pipeline
    binding + AABB BVH collision walker + material/raster installer +
    animation-effect controller). ORPHAN-queue-row reconstructed pre-sweep.

The batch-z-s1 bucket_00591710 row was a FULL HALT (qhull-2002.1 library
residue, 0 plates produced) — nothing to harvest from it.

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

# No padding-byte plates expected in batch_z (none of the buckets contained
# alignment bytes in the queue-row notes).
PADDING_PLATES: set[str] = set()

# Per-plate s2 classification: physics game-code vs qhull residue vs D3DX9-FX residue.
S2_QHULL_LOWER = 0x0059b460
S2_QHULL_UPPER = 0x0059f5d0
S2_D3DX9_LOWER = 0x005b8200
S2_D3DX9_UPPER = 0x005b8680

S2_GAME_NOTE = (
    "batch_z s2 physics-engine narrow-phase pipeline (GJK distance + EPA "
    "penetration + SAT axis solver + polygon contact clipping + manifold "
    "reduction + conservative-advancement TOI + Mirtich-style mass-properties "
    "integrator); REGION A 0x00565cd0..0x0057ae30"
)
S2_QHULL_NOTE = (
    "library_residue qhull-2002.1 (batch_z s2 REGION B 0x0059b460..0x0059f5d0; "
    "extends qhull island past pre-batch bound 0x0057c5b0); static-linked "
    "third-party qhull, do NOT promote"
)
S2_D3DX9_NOTE = (
    "library_residue D3DX9 / FX framework effect-state-block builder + "
    "bit-pack descriptor cascade (batch_z s2 REGION C 0x005b8200..0x005b8680; "
    "separate from D3DX9 HLSL band 0x004fcb51..0x005112c9)"
)


def classify_s2(rva: str) -> tuple[str, str]:
    """Returns (subsystem, note) for a single s2 plate."""
    addr = int(rva, 16)
    if S2_QHULL_LOWER <= addr <= S2_QHULL_UPPER:
        return ("third-party-library[qhull-2002.1]", S2_QHULL_NOTE)
    if S2_D3DX9_LOWER <= addr <= S2_D3DX9_UPPER:
        return ("third-party-library[d3dx9-fx]", S2_D3DX9_NOTE)
    return ("gameplay", S2_GAME_NOTE)


PLATE_BUCKETS: dict[str, tuple[str, str, str]] = {
    "bucket_00565cd0": (
        "batch-z-s2",
        "MIXED",  # sentinel -> classify_s2 per plate
        "",
    ),
    "bucket_0041dc30": (
        "batch-z-s6",
        "frontend",
        "batch_z s6 HUD/frontend/race-state core (HUD-glyph emitter + "
        "per-player score aggregation + menu input + copter asset loader "
        "+ race state machine + screen-dim fade); per-player score struct "
        "stride 0x4e at DAT_00899a40, HUD-glyph slot stride 0xd ints at "
        "DAT_00898ac0",
    ),
    "bucket_004f022d": (
        "batch-z-s5",
        "render",
        "batch_z s5 RenderWare 3.x D3D9-backend (quaternion/matrix math + "
        "RW driver init/teardown + RpGeometry/RpAtomic NATIVE-D3D9 stream "
        "R/W + NvTriStrip strip generator + material-list + D3D9 vertex-"
        "format converters + view/projection matrix infra)",
    ),
    "bucket_0052df70": (
        "batch-z-s3",
        "render",
        "batch_z s3 RenderWare-atomic/animation-glue (vertex-format "
        "allocator + pipeline binding + AABB BVH collision walker + "
        "material/raster installer + animation-effect controller); orphan-"
        "queue-row reconstructed pre-sweep, opened in Mashed_pool11",
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
    print(f"hooks.csv: {len(existing)} unique mapped RVAs (pre-batch_z)")

    added: list[list[str]] = []
    per_bucket_added: dict[str, int] = {}
    s2_breakdown: dict[str, int] = {}
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
                sub, note = classify_s2(rva)
                s2_breakdown[sub] = s2_breakdown.get(sub, 0) + 1
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

    if s2_breakdown:
        print(f"\n=== s2 trimodal breakdown ===")
        for k in sorted(s2_breakdown):
            print(f"  {k}: {s2_breakdown[k]}")

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
        backup = HOOKS.with_suffix(".csv.bak.batch_z")
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
