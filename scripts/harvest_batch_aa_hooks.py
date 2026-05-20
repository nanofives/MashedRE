#!/usr/bin/env python3
"""Harvest batch_aa plate frontmatter -> hooks.csv C1 rows.

Run AFTER ghidra-sweep-20260520-0604 has drained the 3 SCRIBE_QUEUE rows
into master Ghidra. Each plate becomes one hooks.csv row at C1 / mapped.

Buckets covered:
- batch-aa-s1 bucket_00489450 (19 plates)  -> MIXED gameplay
    Animation-track cubic-spline subsystem (0x00489xxx) +
    RpWorld iteration-wrapper family (0x004b41xx) +
    util-math singleton 0x004c4220 (3x3 determinant, sibling to FastMath
    Phase-4 C3 trio). 37 RVAs in this bucket halted as D3DX9 PSGP library
    residue (handled by bulk_add_library_residue.py band d3dx9-psgp).
- batch-aa-s2 bucket_004ee461 (0 plates)   -> SKIP
    Full library-residue halt; 56 RVAs are D3DX9 PSGP (handled by bulk-add).
- batch-aa-s3 bucket_004fcac3 (54 plates) -> MIXED rendering+audio
    RW D3D9 rendering + spline/curve evaluation + matrix-to-quaternion +
    Möller-Trumbore ray-triangle intersection + DSound audio source
    management.

Output:
- Appends to hooks.csv (one row per new plated RVA)
- Skips files prefixed with `_` (halt markers like _LIBRARY_RESIDUE_*.md)
"""

from __future__ import annotations

import csv
import re
import shutil
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
HOOKS = ROOT / "hooks.csv"

PADDING_PLATES: set[str] = set()

# Per-plate s1 classification: animation+util-math vs RW-iteration-glue.
# Cluster boundaries from queue-row note.
S1_RW_LOWER = 0x004b4150
S1_RW_UPPER = 0x004b48e0

S1_ANIM_NOTE = (
    "batch_aa s1 animation-track cubic-spline subsystem (FUN_00489500 cubic-"
    "spline keyframe evaluator + arm/tick/disarm triplet + 0x30-byte slot "
    "setters; uses _DAT_007d3ff8 vtable+0x10c track-manager singleton)"
)
S1_RW_NOTE = (
    "batch_aa s1 RpWorld iteration-wrapper family (RpWorldForAllWorldSectors, "
    "RpWorldForAllAtomics, 2 unnamed RW iterators sharing LAB_004b3f20 "
    "callback) + 4-resource compound ctor 0x004b48e0 + locked singleton-fill "
    "0x004b4150 + Vec3Lerp 0x004b4650 (C3 candidate)"
)
S1_UTIL_NOTE = (
    "batch_aa s1 util-math singleton 0x004c4220 = 3x3 determinant from 4x4 "
    "layout (68B, pure-leaf, sibling to landed Phase-4 C3 FastMath cluster "
    "at 0x004c3ac0/3b30/3b90)"
)


def classify_s1(rva: str) -> tuple[str, str]:
    """Returns (subsystem, note) for a single s1 plate."""
    addr = int(rva, 16)
    if 0x00489000 <= addr < 0x0048a000:
        return ("gameplay", S1_ANIM_NOTE)
    if S1_RW_LOWER <= addr <= S1_RW_UPPER:
        return ("render", S1_RW_NOTE)
    if addr == 0x004c4220:
        return ("gameplay", S1_UTIL_NOTE)
    return ("gameplay", "batch_aa s1 misc")


# Per-plate s3 classification: 3 sub-clusters per the queue-row note.
S3_RW_LOWER = 0x004fcac3
S3_RW_UPPER = 0x005493e0
S3_AUDIO_LOWER = 0x005b8be0
S3_AUDIO_UPPER = 0x005bff40

S3_RW_NOTE = (
    "batch_aa s3 RW D3D9 rendering + spline/curve evaluation + math primitives "
    "(RwMatrixMultiply 0x004fcac3, draw-call 3-tier dispatcher, RW pipeline "
    "init/teardown, Catmull-Rom curve evaluators, Shoemake matrix-to-quaternion "
    "triplet, Möller-Trumbore ray-tri intersect)"
)
S3_AUDIO_NOTE = (
    "batch_aa s3 DirectSound/software-mixer audio source state machine "
    "(hardware vs software branches on caps[+0x50]&8; 16-bit PCM saturated "
    "2-voice mix at 0x005bb5b0); NOT D3DX9-FX backward extension"
)


def classify_s3(rva: str) -> tuple[str, str]:
    """Returns (subsystem, note) for a single s3 plate."""
    addr = int(rva, 16)
    if S3_AUDIO_LOWER <= addr <= S3_AUDIO_UPPER:
        return ("audio", S3_AUDIO_NOTE)
    return ("render", S3_RW_NOTE)


PLATE_BUCKETS: dict[str, tuple[str, str, str]] = {
    "bucket_00489450": ("batch-aa-s1", "MIXED_S1", ""),
    "bucket_004fcac3": ("batch-aa-s3", "MIXED_S3", ""),
}


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
        for r in csv.reader(f):
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
    print(f"hooks.csv: {len(existing)} unique mapped RVAs (pre-batch_aa)")

    added: list[list[str]] = []
    per_bucket_added: dict[str, int] = {}
    drift_higher: list[tuple[str, str, str]] = []
    drift_equal_or_lower: list[tuple[str, str, str]] = []
    no_data: list[Path] = []

    def emit(row: list[str], bucket_tag: str) -> None:
        added.append(row)
        per_bucket_added[bucket_tag] = per_bucket_added.get(bucket_tag, 0) + 1

    for bucket, (session_id, subsystem, _default) in PLATE_BUCKETS.items():
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
                continue

            if rva in existing:
                ec = existing_conf.get(rva, "")
                if ec in ("C2", "C3", "C4"):
                    drift_higher.append((rva, bucket, ec))
                    continue
                drift_equal_or_lower.append((rva, bucket, ec))
                continue

            if subsystem == "MIXED_S1":
                sub, note = classify_s1(rva)
            elif subsystem == "MIXED_S3":
                sub, note = classify_s3(rva)
            else:
                sub, note = subsystem, ""

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
        print(f"\n=== Drift cases - already at C2+ ({len(drift_higher)}) ===")
        for rva, bucket, ec in drift_higher[:20]:
            print(f"  0x{rva} (in {bucket}, existing={ec}) -> SKIP")
    if drift_equal_or_lower:
        print(f"\n=== Drift cases - already at <=C1 ({len(drift_equal_or_lower)}) ===")
        for rva, bucket, ec in drift_equal_or_lower[:20]:
            print(f"  0x{rva} (in {bucket}, existing={ec or 'blank'}) -> SKIP")

    if no_data:
        print(f"\n=== No-frontmatter / missing plates ({len(no_data)}) ===")
        for p in no_data[:5]:
            try:
                print(f"  {p.relative_to(ROOT)}")
            except ValueError:
                print(f"  {p}")

    if deduped:
        backup = HOOKS.with_suffix(".csv.bak.batch_aa")
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

    new_existing, _ = load_existing_rvas()
    pct_pre = 100.0 * len(existing) / 5800
    pct_post = 100.0 * len(new_existing) / 5800
    print(f"\nCoverage: {len(existing)} -> {len(new_existing)} unique RVAs of 5800")
    print(f"          {pct_pre:.1f}% -> {pct_post:.1f}% mapped")

    return 0


if __name__ == "__main__":
    sys.exit(main())
