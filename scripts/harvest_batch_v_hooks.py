#!/usr/bin/env python3
"""Harvest batch_v plate frontmatter + HALT-bucket ranges -> hooks.csv C1 rows.

Run AFTER ghidra-sweep-20260518-1729 has drained the 6 SCRIBE_QUEUE rows
into master Ghidra. Each plate or HALT-range RVA becomes one hooks.csv row
at C1 / mapped. Mirrors `harvest_batch_t_hooks.py` for the per-plate buckets
and `library_drain_libpng_zlib_qh.py` for the HALT-residue buckets, but
collapses both into a single batch_v follow-up pass.

Buckets covered:
- batch-v-s1 bucket_00412130 (80 plates)  -> gameplay (hud-weapons-effects-and-particle-emitters)
- batch-v-s2 bucket_00489940 (80 plates)  -> particle (22 vfx + 49 directshow + 9 drift-skip)
- batch-v-s3 bucket_004ddfb0 (80 plates)  -> render   (renderware-d3d9-driver)
- batch-v-s4 bucket_00516bb0 (HALT, 80 RVAs from batch_v.txt) -> util (libpng+zlib+image-library)
- batch-v-s5 bucket_0057bf30 (8 plates + 72 HALT RVAs) -> rw-cluster game + util qhull library
- batch-v-s6 bucket_005c1d63 (80 plates)  -> mixed (33 fiddb-crt + 23 crt-internal + 24 game)

Output:
- Appends to hooks.csv (one row per new plated / HALT-tagged RVA)
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
INVENTORY = ROOT / "re" / "analysis" / "function_inventory.csv"

# -----------------------------------------------------------------------------
# Plate-based buckets — walk re/analysis/<bucket>/*.md frontmatter.
#
# Per-bucket (session_id, subsystem token, default note). The note is shared
# across all plates in the bucket; per-RVA detail lives in the plate file itself.
# -----------------------------------------------------------------------------

PLATE_BUCKETS: dict[str, tuple[str, str, str]] = {
    "bucket_00412130": (
        "batch-v-s1",
        "gameplay",
        "batch_v s1 hud-weapons-effects-and-particle-emitters; wheel-trail-decals + "
        "vehicle-icon sprite-batch + decal-color-grid + weapons subsystem (4 launchers + "
        "4 missiles + 64-puff smoke + 2 reticules) + 2 ghost-vehicle slots + 4 ParticleEmitter classes",
    ),
    "bucket_00489940": (
        "batch-v-s2",
        "particle",
        "batch_v s2 TRIMODAL[vfx-trail-spark(22)+directshow-strmbase-static-link(49)+drift-skip(9)]; "
        "directshow half is candidate library-residue, awaiting FidDb confirmation",
    ),
    "bucket_004ddfb0": (
        "batch-v-s3",
        "render",
        "batch_v s3 renderware-d3d9-driver; rwimage-raster-format-conv + rwheap-allocator + "
        "csl-pipeline-nodes + rpgeometry/rpworld/rpatomic stream-codec + d3d9-vertex-decl-build",
    ),
    "bucket_005c1d63": (
        "batch-v-s6",
        "boot",
        "batch_v s6 MIXED[msvc-crt-fiddb(33)+msvc-crt-internal-helpers(23)+game-dynarray-and-chunk-readers(15)+"
        "audio-mixer-voice-init(1)+game-collision-or-rwmath-zero-callers(8)]; "
        "56/80 are CRT library-residue (FidDB-attested or canonical-body-match)",
    ),
}

# RVAs we know to be drift cross-references pointing into cluster_004b4_first_pass.
# For these we rewrite the file column to the canonical cluster plate.
S2_DRIFT_RVAS: set[str] = {
    "004b4000", "004b4010", "004b4050", "004b4080",
    "004b40c0", "004b40f0", "004b4120", "004b4130", "004b4140",
}
S2_DRIFT_CANONICAL_DIR = "re/analysis/cluster_004b4_first_pass"

# -----------------------------------------------------------------------------
# batch-v-s5 bucket_0057bf30 — bimodal (8 plates + 72 HALT RVAs).
#
# Plates 1..8 are game/RW code; treat as a plate bucket but with its own
# subsystem token + note. Plates 9..80 are HALT-row library residue.
# -----------------------------------------------------------------------------

S5_PLATE_RVAS: set[str] = {
    "0057bf30", "0057c0d0", "0057c1b0", "0057c2b0",
    "0057c370", "0057c420", "0057c440", "0057c550",
}
S5_PLATE_SUBSYS = "render"  # RW-object cluster (refcount + RW vtable + memory-vtable)
S5_PLATE_NOTE = (
    "batch_v s5 RW-object cluster (8 game plates of bimodal bucket; remaining 72 RVAs are "
    "qhull-2002.1 library-residue tracked separately); 0x54-byte struct, refcount@+0x50, "
    "RW memory-vtable DAT_007d3ff8+0x108/+0x10c"
)

# 72 qhull RVAs from candidates #9..#80 in batch_v.txt (s5 section).
S5_HALT_QHULL_RVAS: list[str] = [
    "0057c5b0", "0057c650", "0057c670", "0057ca30", "0057cae0",
    "0057cb30", "0057ce90", "0057d370", "0057d580", "0057d990",
    "0057de60", "0057e750", "0057e820", "0057ec70", "0057ed80",
    "0057ee00", "0057f030", "0057f080", "0057f0c0", "0057f200",
    "0057f4e0", "0057f750", "0057f860", "0057f940", "0057f970",
    "00580090", "005803a0", "005808a0", "00580ac0", "00580ea0",
    "00580fc0", "00581120", "00581170", "005811e0", "00581290",
    "00581320", "00581540", "005815f0", "00581680", "005817f0",
    "00581930", "00581a40", "00581ff0", "00582220", "005822f0",
    "00582390", "00582420", "005824a0", "00582550", "00582680",
    "005826d0", "005827f0", "00582840", "00582930", "00582a20",
    "00582a60", "00582ac0", "00582b10", "00582bf0", "00582ce0",
    "00582d40", "00582da0", "00582e10", "00582e70", "00582ed0",
    "00582f00", "00582f30", "00582f70", "00583070", "005830a0",
    "00583150", "00583330",
]

# 4 qhull spot-confirms from s5 HALT report (only 1 has a canonical name; 3 are
# tagged descriptors of where they sit in qhull's source layout).
S5_QHULL_NAMES: dict[str, str] = {
    "0057c5b0": "qh_fprintf",
    # 0057c650 is described as "qh_fprintf buf-adapter" (no canonical name); keep FUN_XXX
    # 0057cb30 is qh_check_bestdist
    "0057cb30": "qh_check_bestdist",
    # 0057ca30 is the qh_new_qhull adapter (uses "qhull s Pp" default-options string)
    # but the s5 HALT only attests it as adapter — keep FUN_XXX rather than name an adapter.
    # 0057c670 is described as the qhull-to-game triangulation output formatter — keep FUN_XXX.
}

S5_HALT_BUCKET_FILE = "re/analysis/bucket_0057bf30/_BUCKET_HALT.md"
S5_HALT_NOTE = (
    "library_residue qhull-2002.1 (s5 batch_v halt; bimodal-bucket residue half); "
    "static-linked third-party; extends batch-t-s5 0x00583f10..0x005913c0 backwards; "
    "bulk-tag via Ghidra FidDB"
)

# -----------------------------------------------------------------------------
# batch-v-s4 bucket_00516bb0 — pure HALT-row (libpng + zlib + image-library).
#
# 80 RVAs sourced from the candidate table in batch_v.txt.
# -----------------------------------------------------------------------------

S4_HALT_RVAS: list[str] = [
    "00516bb0", "00516bd0", "00516bf0", "00516c40", "00516cc0",
    "00516d20", "00516d30", "00516d50", "00516da0", "00516de0",
    "005171a0", "00517200", "00517270", "005172b0", "005172f0",
    "00517380", "005173d0", "005173f0", "005175d0", "00517940",
    "00517da0", "00517e00", "00518080", "00518140", "005183a0",
    "005183c0", "005184b0", "005184f0", "00518570", "00518580",
    "00518590", "005185a0", "00519270", "005196f0", "00519820",
    "005199f0", "00519af0", "00519bc0", "00519e30", "00519f70",
    "0051a6c0", "0051b750", "0051bb40", "0051bd70", "0051c100",
    "0051c220", "0051c9f0", "0051ca10", "0051ca60", "0051ca90",
    "0051cb40", "0051cb70", "0051cbe0", "0051cfc0", "0051d0f0",
    "0051d1b0", "0051d240", "0051d340", "0051d400", "0051d560",
    "0051d800", "0051d900", "0051db30", "0051dca0", "0051e230",
    "0051e4a0", "0052c4a0", "0052c630", "0052c660", "0052c7a0",
    "0052cb00", "0052d170", "0052d7e0", "0052d980", "0052da20",
    "0052daf0", "0052ddc0", "0052ddf0", "0052de60", "0052df40",
]

# 14 spot-confirmed canonical names from the s4 HALT report.
S4_LIBPNG_ZLIB_IMG_NAMES: dict[str, str] = {
    "00516bb0": "png_write_data",
    "00516bd0": "png_flush_data",
    "00516bf0": "png_set_write_fn",
    "00516c40": "png_sig_cmp",
    "00516cc0": "png_zalloc",            # or png_calloc per HALT
    "00516d20": "thunk_png_free",        # thunk_FUN_00520930 in inventory
    "00516d30": "png_reset_crc",
    "00516d50": "png_calculate_crc",
    "00516da0": "png_create_info_struct",
    "00516de0": "png_free_data",
    "005185a0": "png_do_background",
    "00519270": "png_do_read_transformations",
    "0051a6c0": "png_do_expand_palette",
    "0051c220": "png_build_gamma_table",
    "0051e230": "png_write_tRNS",
    "0052c4a0": "inflate_fast_huffman_setup",
    "0052cb00": "image_library_bmp_loader",
    "0052daf0": "image_library_format_handler_registry",
}

S4_HALT_BUCKET_FILE = "re/analysis/bucket_00516bb0/_BUCKET_HALT.md"


def s4_split_tag(rva: str) -> str:
    """Return per-RVA sub-library label based on the 3-segment split documented
    in the s4 HALT report: libpng <0x0051e6f0, zlib 0x0052c4a0..~0x0052d52a,
    image-library 0x0052cb00..0x0052df40 (the zlib + image-library segments
    overlap at the 0x0052cb00 boundary; the HALT report places the boundary
    BETWEEN zlib's inflate_fast machinery and the image-library's BMP loader,
    so we treat the BMP loader RVA and beyond as image-library).
    """
    rva_int = int(rva, 16)
    if rva_int < 0x0051e6f0:
        return "libpng"
    if rva_int < 0x0052cb00:
        return "zlib"
    return "image-library"


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


def load_existing_rvas() -> set[str]:
    seen: set[str] = set()
    hex8 = re.compile(r"^(?:0x)?([0-9a-fA-F]{8})$")
    with open(HOOKS, encoding="utf-8-sig") as f:
        rdr = csv.reader(f)
        for r in rdr:
            if not r:
                continue
            m = hex8.match(r[0].strip())
            if m:
                seen.add(m.group(1).lower())
    return seen


def load_inventory_names() -> dict[str, str]:
    """rva -> name from function_inventory.csv (for HALT entries that need
    a name beyond raw FUN_XXX, e.g. thunk_FUN_*)."""
    names: dict[str, str] = {}
    with open(INVENTORY, encoding="utf-8-sig") as f:
        rdr = csv.reader(f)
        next(rdr)
        for r in rdr:
            if r and r[0].strip():
                names[r[0].strip().lower()] = r[1].strip()
    return names


def main() -> int:
    existing = load_existing_rvas()
    inv_names = load_inventory_names()
    print(f"hooks.csv: {len(existing)} unique mapped RVAs (pre-batch_v)")
    print(f"function_inventory.csv: {len(inv_names)} rows")

    added: list[list[str]] = []
    per_bucket_added: dict[str, int] = {}
    drift: list[tuple[str, str]] = []
    no_data: list[Path] = []

    def emit(row: list[str], bucket_tag: str) -> None:
        added.append(row)
        per_bucket_added[bucket_tag] = per_bucket_added.get(bucket_tag, 0) + 1

    # ---- 1. plate-based buckets (s1, s2, s3, s6) ----
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
                drift.append((rva, bucket))
                continue
            name = data.get("name_in_ghidra") or data.get("name") or f"FUN_{rva}"

            # s2 special-case: drift cross-refs into cluster_004b4_first_pass.
            # Rewrite the `file` column to the canonical plate.
            if bucket == "bucket_00489940" and rva in S2_DRIFT_RVAS:
                file_col = f"{S2_DRIFT_CANONICAL_DIR}/{rva}.md"
                note = (
                    "batch_v s2 drift-cross-ref into cluster_004b4_first_pass "
                    "(canonical plate from batch-t-s1); first hooks.csv row landed here"
                )
            else:
                file_col = f"re/analysis/{bucket}/{plate.name}"
                note = default_note

            row = [
                rva, name, subsystem, "C1", "mapped",
                file_col, session_id, "", note,
            ]
            emit(row, bucket)

    # ---- 2. batch-v-s5 game plates (8) ----
    s5_bucket = "bucket_0057bf30"
    s5_bdir = ROOT / "re" / "analysis" / s5_bucket
    if s5_bdir.exists():
        for rva in sorted(S5_PLATE_RVAS):
            plate = s5_bdir / f"0x{rva}.md"
            if not plate.exists():
                no_data.append(plate)
                continue
            data = parse_plate(plate)
            if not data:
                no_data.append(plate)
                continue
            if rva in existing:
                drift.append((rva, s5_bucket + "[plate]"))
                continue
            name = data.get("name_in_ghidra") or f"FUN_{rva}"
            row = [
                rva, name, S5_PLATE_SUBSYS, "C1", "mapped",
                f"re/analysis/{s5_bucket}/0x{rva}.md",
                "batch-v-s5", "",
                S5_PLATE_NOTE,
            ]
            emit(row, s5_bucket + "[plate]")

    # ---- 3. batch-v-s5 qhull HALT residue (72) ----
    for rva in S5_HALT_QHULL_RVAS:
        if rva in existing:
            drift.append((rva, s5_bucket + "[halt-qhull]"))
            continue
        canonical = S5_QHULL_NAMES.get(rva)
        name = canonical if canonical else inv_names.get(rva, f"FUN_{rva}")
        row = [
            rva, name, "util", "C1", "mapped",
            S5_HALT_BUCKET_FILE,
            "library-drain-qhull-batch-v",
            "",
            S5_HALT_NOTE,
        ]
        emit(row, s5_bucket + "[halt-qhull]")

    # ---- 4. batch-v-s4 libpng+zlib+image-library HALT residue (80) ----
    s4_bucket = "bucket_00516bb0"
    for rva in S4_HALT_RVAS:
        if rva in existing:
            drift.append((rva, s4_bucket + "[halt]"))
            continue
        canonical = S4_LIBPNG_ZLIB_IMG_NAMES.get(rva)
        if canonical:
            name = canonical
        else:
            name = inv_names.get(rva, f"FUN_{rva}")
        lib_tag = s4_split_tag(rva)
        note = (
            f"library_residue {lib_tag} (s4 batch_v halt; extends batch-t-s4 "
            f"0x0051e6f0..0x0052c49d island both directions); static-linked "
            f"third-party; bulk-tag via Ghidra FidDB"
        )
        row = [
            rva, name, "util", "C1", "mapped",
            S4_HALT_BUCKET_FILE,
            "library-drain-libpng-zlib-image",
            "",
            note,
        ]
        emit(row, s4_bucket + "[halt]")

    # ---- 5. dedupe newly-added against itself (defense against accidental dup) ----
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

    if drift:
        print(f"\n=== Drift cases ({len(drift)} RVAs already in hooks.csv) ===")
        for rva, bucket in drift[:20]:
            print(f"  0x{rva} (in {bucket})")
        if len(drift) > 20:
            print(f"  ... + {len(drift) - 20} more")
    if no_data:
        print(f"\n=== No-frontmatter / missing plates ({len(no_data)}) ===")
        for p in no_data[:5]:
            try:
                print(f"  {p.relative_to(ROOT)}")
            except ValueError:
                print(f"  {p}")

    if deduped:
        shutil.copy2(HOOKS, str(HOOKS) + ".bak.batch_v")
        with open(HOOKS, "a", encoding="utf-8", newline="") as f:
            w = csv.writer(f, lineterminator="\n")
            for r in deduped:
                w.writerow(r)
        print(f"\nAppended {len(deduped)} rows to {HOOKS.relative_to(ROOT)}")
        print(f"Backup at hooks.csv.bak.batch_v")
    else:
        print("\nNo rows appended.")

    # ---- 6. post-write coverage stats ----
    new_existing = load_existing_rvas()
    pct_pre = 100.0 * len(existing) / 5800
    pct_post = 100.0 * len(new_existing) / 5800
    print(f"\nCoverage: {len(existing)} -> {len(new_existing)} unique RVAs of 5800")
    print(f"          {pct_pre:.1f}% -> {pct_post:.1f}% mapped")

    return 0


if __name__ == "__main__":
    sys.exit(main())
