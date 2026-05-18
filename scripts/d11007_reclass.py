"""
D-11007 drift cleanup: per-row subsystem reclass + dup consolidation
for the batch_t residue (4 groups identified by sweep-20260518-0247).

This script is one-shot. It reads hooks.csv as line-list, applies
surgical edits keyed on (rva, current_subsystem, plate_path) tuples,
and writes back. Refuses to touch C3/C4 rows (exclusion rule).

Groups handled:
  1. render_midrva (40 RVAs) — subsystem reclass per plate's subsystem_observed
  2. cluster_004d RW error reporter (0x004d7ff0, 0x004d8480) — audio->render
  3. vehicle_lowrva (9 RVAs) — dup consolidation; C2 row already canonical
  4. cluster_0049 — audited sample, no reclass needed

Group 4 produces no edits; group 1+2+3 do.

Edit policy:
- Update $3 (subsystem) to plate-observed value
- Append note in $9 explaining reclass evidence + date
- For duplicates (multiple C1 rows same RVA): mark non-canonical rows as
  [DUP-REMOVED-2026-05-18] via comment line replacing the row, keep canonical
- Refuse silently on any C3/C4 row encountered (per exclusion rule)

Run with:  py -3.12 scripts/d11007_reclass.py
"""
from __future__ import annotations
import csv
import io
import re
import sys
from pathlib import Path

REPO = Path(__file__).resolve().parent.parent
HOOKS = REPO / "hooks.csv"
DATE = "2026-05-18"

# ---- Group 1: render_midrva 40-RVA reclass map ----------------------------
# (rva, plate_subsystem_observed) — from re/analysis/promote_c2_render_midrva/*.md frontmatter
# Hooks.csv in-use subsystem values include: ai, audio, boot, camera, frontend,
# gameplay, hud, input, io, particle, physics, powerups, race_state, render,
# save, track, unknown, util, vehicle, video. Plate prose values map directly.
# Special-case: 004715a0 plate says "physics (AI-graph + collision scenario
# builder; misclassified as render)" — function expands DAT_0086cbcc poly
# groups into AI node entries (pathfinding), so subsystem=ai is the right
# call per plate's actual mechanical description.
RENDER_MIDRVA = {
    "004715a0": "ai",          # plate says physics-prose but mechanically AI-graph builder
    "00471ac0": "camera",      # camera-anim event manager
    "00471cf0": "camera",      # camera-anim state reset
    "00471ec0": "camera",      # camera-anim trigger checker
    "00472380": "render",      # RW plugin/extension reg — already render
    "004725c0": "render",      # paired-slot render dispatch — already render
    "00472650": "util",        # uniform-float-in-range RNG helper
    "00472690": "util",        # uniform-int-in-range RNG helper
    "00474f30": "render",      # RpClump callback
    "00474fb0": "render",      # RpClump iter wrapper
    "00474fd0": "render",      # atomic-search wrapper
    "00478200": "track",       # course-load fatal error
    "004783f0": "ai",          # AI polygon heading
    "004785e0": "util",        # vec3 cross product
    "00478660": "ai",          # AI polygon bounding builder
    "00478cf0": "track",       # course-asset destructor
    "00479030": "track",       # post-load callback for course
    "004790e0": "track",       # post-load thunk for course
    "00479330": "track",       # main course asset loader
    "0047a0f0": "track",       # course Lua init root
    "0047a1e0": "track",       # COURSE.LUA Sky_Filename handler
    "0047ab30": "track",       # COURSE.LUA Setup_Fog handler
    "0047abd0": "track",       # COURSE.LUA Modify_Fog handler
    "0047b9e0": "track",       # Lua wrapper thunk
    "0047bcc0": "track",       # half-edge adj builder (track collision)
    "0047be80": "track",       # per-tri normal calc (track collision)
    "0047bf70": "track",       # per-sector record builder
    "0047c0b0": "track",       # zero-clear collision tables
    "0047c0f0": "track",       # collision sector loader
    "0047c160": "camera",      # camera-path node containment walk
    "0047cea0": "physics",     # explosion/impulse application
    "0047d080": "physics",     # body enable/disable
    "0047d100": "physics",     # body secondary-enable
    "0047f4c0": "physics",     # physics world ctor
    "0047f840": "physics",     # physics world lazy-init
    "0047f940": "physics",     # cylinder body creator
    "0047fc40": "physics",     # box body creator
    "0047fe00": "physics",     # sphere body creator
    "0047ff70": "physics",     # cone/capsule body creator
    "00480100": "physics",     # physics body post-init
}

# ---- Group 2: cluster_004d RW error reporter ------------------------------
CLUSTER_004D = {
    "004d7ff0": "render",  # was audio
    "004d8480": "render",  # was audio
}

# ---- Group 3: vehicle_lowrva 9 RVAs with C2 canonical + C1 sibling dups ---
# Canonical row is the one with file path under promote_c2_vehicle_lowrva/.
# Strategy: keep C2 row as-is (already labeled with subsystem_observed boundary
# in notes); mark all C1 sibling rows pointing to other plate dirs as
# [DUP-REMOVED-2026-05-18] (replace whole row with a # comment line).
VEHICLE_LOWRVA = [
    "00408a50", "00408a70", "0040e340", "0040e350", "0040e370",
    "00422fd0", "0042c280", "00432080", "004331a0",
]

EXCLUDED_CONFIDENCE = {"C3", "C4"}

def parse_row(line: str) -> list[str] | None:
    """Parse one CSV line into 9 fields. Returns None for non-data rows."""
    line = line.rstrip("\r\n")
    if not line or line.startswith("#") or line.startswith("﻿"):
        return None
    # Skip the header rows
    if line.startswith("rva,"):
        return None
    # Use csv module on a single line to handle quoted fields
    reader = csv.reader(io.StringIO(line))
    try:
        row = next(reader)
    except StopIteration:
        return None
    if len(row) < 9:
        return None
    return row

def quote_field(s: str) -> str:
    """Quote a field if it contains comma, quote, or newline."""
    if "," in s or '"' in s or "\n" in s:
        return '"' + s.replace('"', '""') + '"'
    return s

def emit_row(fields: list[str]) -> str:
    return ",".join(quote_field(f) for f in fields)

def main() -> int:
    # Read raw bytes to detect BOM, then decode normally
    raw_bytes = HOOKS.read_bytes()
    has_bom = raw_bytes.startswith(b"\xef\xbb\xbf")
    if has_bom:
        raw_bytes = raw_bytes[3:]
    raw = raw_bytes.decode("utf-8")
    lines = raw.splitlines(keepends=False)

    out_lines: list[str] = []
    g1_reclassed = 0
    g1_kept = 0
    g2_dup_removed = 0
    g2_canonical_kept = 0
    g3_dup_removed = 0
    refused_c3 = []

    # Track which RVAs we've already seen for each group so dup-handling is deterministic
    g3_canonical_seen: dict[str, bool] = {rva: False for rva in VEHICLE_LOWRVA}

    for line_no, line in enumerate(lines, 1):
        row = parse_row(line)
        if row is None:
            out_lines.append(line)
            continue
        rva = row[0].strip()
        subsystem = row[2].strip()
        confidence = row[3].strip()
        file_path = row[5].strip()
        notes = row[8] if len(row) > 8 else ""

        # ----- Group 1: render_midrva -----
        if rva in RENDER_MIDRVA:
            if confidence in EXCLUDED_CONFIDENCE:
                refused_c3.append((rva, confidence, "render_midrva"))
                out_lines.append(line)
                continue
            target = RENDER_MIDRVA[rva]
            if subsystem == target:
                # Already correct — add a note explaining audit
                if "d11007-audit" not in notes:
                    new_notes = (notes + "; " if notes else "") + (
                        f"d11007-audit {DATE}: subsystem={target} confirmed via "
                        f"re/analysis/promote_c2_render_midrva/{rva}.md plate (subsystem_observed)"
                    )
                    row[8] = new_notes
                    out_lines.append(emit_row(row))
                    g1_kept += 1
                    continue
                out_lines.append(line)
                g1_kept += 1
                continue
            # Reclass
            old_sub = subsystem
            row[2] = target
            new_notes = (notes + "; " if notes else "") + (
                f"d11007-reclass {DATE}: subsystem {old_sub}->{target} per "
                f"re/analysis/promote_c2_render_midrva/{rva}.md (subsystem_observed)"
            )
            row[8] = new_notes
            out_lines.append(emit_row(row))
            g1_reclassed += 1
            continue

        # ----- Group 2: cluster_004d RW error reporter -----
        # Strategy: canonical row is rw_engine_init_cont1 (correctly identifies
        # function as RW driver/error infra). All other rows (audio_rws_loader_*)
        # are dup discovery paths.
        if rva in CLUSTER_004D:
            if confidence in EXCLUDED_CONFIDENCE:
                refused_c3.append((rva, confidence, "cluster_004d"))
                out_lines.append(line)
                continue
            is_canonical = "rw_engine_init_cont1" in file_path
            if is_canonical:
                # Canonical row — keep, add audit note
                if "d11007-audit" not in notes:
                    new_notes = (notes + "; " if notes else "") + (
                        f"d11007-audit {DATE}: canonical RW driver/error infra row; "
                        f"sibling C1 rows under audio_rws_loader_* removed"
                    )
                    row[8] = new_notes
                    out_lines.append(emit_row(row))
                    g2_canonical_kept += 1
                    continue
                out_lines.append(line)
                g2_canonical_kept += 1
                continue
            # Non-canonical row — was audio-tagged sibling; mark dup-removed
            out_lines.append(
                f"# {rva} duplicate C1 row removed {DATE} d11007-drift-cleanup: "
                f"canonical at re/analysis/rw_engine_init_cont1/{rva}.md "
                f"(was subsystem={subsystem}, file={file_path}); "
                f"RW driver/error infra — render attribution per FUN_004c2c90 C2"
            )
            g2_dup_removed += 1
            continue

        # ----- Group 3: vehicle_lowrva dup consolidation -----
        if rva in VEHICLE_LOWRVA:
            if confidence in EXCLUDED_CONFIDENCE:
                refused_c3.append((rva, confidence, "vehicle_lowrva"))
                out_lines.append(line)
                continue
            # Canonical row: C2, file under promote_c2_vehicle_lowrva/
            is_canonical = (
                confidence == "C2"
                and "promote_c2_vehicle_lowrva" in file_path
            )
            if is_canonical:
                if not g3_canonical_seen[rva]:
                    g3_canonical_seen[rva] = True
                    if "d11007-audit" not in notes:
                        new_notes = (notes + "; " if notes else "") + (
                            f"d11007-audit {DATE}: canonical row; "
                            f"sibling C1 rows under other subsystems removed"
                        )
                        row[8] = new_notes
                        out_lines.append(emit_row(row))
                        continue
                    out_lines.append(line)
                    continue
                # Second canonical row for same RVA? Keep but flag.
                out_lines.append(line)
                continue
            # Non-canonical C1 sibling row — replace with dup-removed comment
            out_lines.append(
                f"# {rva} duplicate C1 row removed {DATE} d11007-drift-cleanup: "
                f"canonical C2 at re/analysis/promote_c2_vehicle_lowrva/0x{rva}.md "
                f"(was subsystem={subsystem}, file={file_path})"
            )
            g3_dup_removed += 1
            continue

        # ----- Default: pass through unchanged -----
        out_lines.append(line)

    # Write back — preserve BOM + CRLF line endings to match original file
    out = "\r\n".join(out_lines) + "\r\n"
    out_bytes = (b"\xef\xbb\xbf" if has_bom else b"") + out.encode("utf-8")
    HOOKS.write_bytes(out_bytes)

    print(f"Group 1 (render_midrva): reclassed {g1_reclassed}, audited+kept {g1_kept}")
    print(f"Group 2 (cluster_004d):  {g2_canonical_kept} canonical kept, {g2_dup_removed} dup C1 rows removed")
    print(f"Group 3 (vehicle_lowrva): {g3_dup_removed} duplicate C1 rows removed")
    if refused_c3:
        print(f"Refused (C3+): {len(refused_c3)}")
        for rva, conf, group in refused_c3:
            print(f"  {rva} {conf} {group}")
    return 0

if __name__ == "__main__":
    sys.exit(main())
