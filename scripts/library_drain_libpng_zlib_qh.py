#!/usr/bin/env python3
"""Library-tag drain: add C1 rows for the libpng+zlib bucket (s4 halt) and
the qhull function set OUTSIDE s5's bucket. All entries land at C1 mapped
with subsystem=util and scenario=library-drain-<lib>.

s4 (HALT, bucket_0051e6f0) identified the 0x0051e6f0..0x0052c49d range as
statically-linked libpng + zlib. 12 functions had spot-confirmed canonical
names; the remaining ~68 stay as FUN_XXX.

Master Ghidra also has 18 qh_* functions in 0x005a0e00..0x005a5017
(Ghidra-FidDB-attested qhull names from an earlier session). Those need
hooks.csv rows if they're not already there.

Output:
- Appends to hooks.csv (one row per library function not previously rowed)
- Reports added vs already-present counts
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

# Spot-confirmed canonical names from s4 halt report (libpng + zlib).
LIBPNG_ZLIB_NAMES: dict[str, str] = {
    "0051e6f0": "png_write_hIST",
    "0051e7f0": "png_check_keyword",
    "0051fa50": "png_write_find_filter",
    "005207c0": "png_create_struct_2",
    "00520850": "png_destroy_struct",
    "00520930": "png_free",
    "00523110": "png_get_uint_32",
    "00523140": "png_crc_read",
    "00525870": "png_handle_sPLT",
    "00528e00": "zcalloc",
    "00528e20": "zcfree",
    "0052ab00": "inflate",
    "0052bff0": "inflate_table",
}

LIBPNG_ZLIB_RANGE = (0x0051e6f0, 0x0052c49d + 1)  # half-open
QHULL_PRINT_RANGE = (0x005a0e00, 0x005a5017 + 1)  # ghidra-FidDB qh_* span

# Qhull names from Ghidra master function_list query (qh_ prefix).
QHULL_NAMES: dict[str, str] = {
    "005a0e00": "qh_printcenter",
    "005a1300": "qh_printend",
    "005a1720": "qh_printincidences",
    "005a1810": "qh_printextremes_2d",
    "005a1990": "qh_printextremes",
    "005a1aa0": "qh_printfacet",
    "005a1ad0": "qh_printfacet3vertex",
    "005a2500": "qh_printfacet2math",
    "005a2a40": "qh_printfacetNvertex_nonsimplicial",
    "005a34e0": "qh_printfacetridges",
    "005a3ea0": "qh_printhelp_singularity",
    "005a3f80": "qh_printhelp_degenerate",
    "005a4740": "qh_printneighborhood",
    "005a48d0": "qh_printpoints_out",
    "005a4c60": "qh_printridge",
    "005a4d40": "qh_printvdiagram",
    "005a4e00": "qh_eachvoronoi_all",
    "005a4ed0": "qh_printvertex",
}


def load_inventory() -> list[dict]:
    rows = []
    with open(INVENTORY, encoding="utf-8-sig") as f:
        rdr = csv.reader(f)
        next(rdr)
        for r in rdr:
            if r and r[0].strip():
                rows.append({
                    "rva_int": int(r[0].strip(), 16),
                    "rva": r[0].strip().lower(),
                    "name": r[1].strip(),
                    "size": int(r[2].strip()),
                })
    return rows


def load_existing_rvas() -> set[str]:
    hex8 = re.compile(r"^(?:0x)?([0-9a-fA-F]{8})$")
    seen: set[str] = set()
    with open(HOOKS, encoding="utf-8-sig") as f:
        rdr = csv.reader(f)
        for r in rdr:
            if not r:
                continue
            m = hex8.match(r[0].strip())
            if m:
                seen.add(m.group(1).lower())
    return seen


def main() -> int:
    inv = load_inventory()
    existing = load_existing_rvas()
    print(f"Inventory: {len(inv)} functions")
    print(f"hooks.csv: {len(existing)} unique RVAs already mapped")

    added: list[list[str]] = []
    already: list[str] = []

    # --- libpng+zlib drain ---
    print(f"\n=== libpng+zlib drain (range 0x{LIBPNG_ZLIB_RANGE[0]:08x}..0x{LIBPNG_ZLIB_RANGE[1] - 1:08x}) ===")
    lpz_count = 0
    for f in inv:
        if not (LIBPNG_ZLIB_RANGE[0] <= f["rva_int"] < LIBPNG_ZLIB_RANGE[1]):
            continue
        lpz_count += 1
        if f["rva"] in existing:
            already.append(f"0x{f['rva']} (libpng/zlib)")
            continue
        canonical = LIBPNG_ZLIB_NAMES.get(f["rva"])
        name = canonical if canonical else f["name"]
        # libpng split is below ~0x00528200; zlib above
        lib_tag = "libpng" if f["rva_int"] < 0x00528200 else "zlib"
        note = (f"library_residue {lib_tag} (s4 batch_t halt); "
                f"static-linked third-party; bulk-tag via Ghidra FidDB or "
                f"dedicated rename pass")
        row = [
            f["rva"], name, "util", "C1", "mapped",
            "re/analysis/bucket_0051e6f0/_BUCKET_HALT.md",
            "library-drain-libpng-zlib", "", note,
        ]
        added.append(row)
    print(f"  In range: {lpz_count}")
    print(f"  Added:    {sum(1 for r in added if 'libpng-zlib' in r[6])}")

    # --- qhull (outside s5's bucket) ---
    print(f"\n=== qhull drain (range 0x{QHULL_PRINT_RANGE[0]:08x}..0x{QHULL_PRINT_RANGE[1] - 1:08x}) ===")
    qh_count = 0
    qh_added_now = 0
    for f in inv:
        if not (QHULL_PRINT_RANGE[0] <= f["rva_int"] < QHULL_PRINT_RANGE[1]):
            continue
        qh_count += 1
        if f["rva"] in existing:
            already.append(f"0x{f['rva']} (qhull)")
            continue
        canonical = QHULL_NAMES.get(f["rva"])
        name = canonical if canonical else f["name"]
        note = ("library_residue qhull-2002.1 (Ghidra-FidDB-attested name); "
                "static-linked third-party; sibling of s5 bucket_00583f10")
        row = [
            f["rva"], name, "util", "C1", "mapped",
            "re/analysis/library_residue/qhull.md",  # placeholder; no per-RVA plate
            "library-drain-qhull", "", note,
        ]
        added.append(row)
        qh_added_now += 1
    print(f"  In range: {qh_count}")
    print(f"  Added:    {qh_added_now}")

    # Summary
    print(f"\n--- Summary ---")
    print(f"  Total new rows: {len(added)}")
    print(f"  Already present: {len(already)}")
    if already:
        for entry in already[:8]:
            print(f"    {entry}")
        if len(already) > 8:
            print(f"    ... + {len(already) - 8} more")

    if added:
        shutil.copy2(HOOKS, str(HOOKS) + ".bak.library_drain")
        with open(HOOKS, "a", encoding="utf-8", newline="") as f:
            w = csv.writer(f, lineterminator="\n")
            for r in added:
                w.writerow(r)
        print(f"\nAppended {len(added)} rows. Backup at hooks.csv.bak.library_drain")
    else:
        print("\nNo new rows appended.")

    return 0


if __name__ == "__main__":
    sys.exit(main())
