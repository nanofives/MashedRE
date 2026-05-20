#!/usr/bin/env python3
"""Bulk-add hooks.csv rows for unmapped functions inside calibrated library bands.

Phase 1 exit criterion 1 demands every function in the binary have a row in
hooks.csv. The four-batches v..z have calibrated 12 library bands across
MASHED.exe (MSVC CRT main + tail, Lua 5.0, D3DX9 HLSL + backward + FX, libpng
+ zlib, qhull 2002.1 + continuation + more + print, DirectShow strmbase).
Worker sessions correctly REFUSE to plate library residue as game code, so
these functions are unmapped not because they're undiscovered but because the
discovery process intentionally leaves them library-tagged.

This script appends one row per unmapped RVA inside a calibrated band, with
confidence=C0 (no per-function decomp evidence; band-level classification
only) and a deterministic library-residue note. After it runs, criterion 1
goes from 4810/5800 -> 5634/5800 mapped (assuming all 888 library-band
unmapped RVAs append cleanly).

Idempotent: dedupes against existing hooks.csv RVAs.
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

# Calibrated library bands (lower inclusive, upper exclusive, name, source-batch).
# Order matters for overlap precedence (earlier bands win on tie).
BANDS = [
    (0x0049dcb0, 0x004a0000, "directshow-strmbase",
     "batch-v-s2 finding"),
    (0x004a0000, 0x004b4000, "msvc-crt-main",
     "FidDB-attested; calibrated pre-batch-s"),
    (0x004b4a80, 0x004c4000, "lua-5.0",
     "batch-x-s2 extended (lstate/lgc/lvm/lapi/ltable/lstring/lmem/llex/"
     "lobject/lopcodes/lzio/ldebug/lparser/lundump/lfunc/lcode)"),
    (0x004fcb51, 0x005112ca, "d3dx9-hlsl-shader-compiler",
     "batch-w-s4 preprocessor+byacc-parser+lexer+diagnostics+symbol-hashtable "
     "+ pre-batch breadth_unmapped_005xx assembler+math-primitives"),
    (0x005115aa, 0x00516a71, "d3dx9-backward-extension",
     "batch-z-s4 halt (ShaderCompiler::AST namespace)"),
    (0x00516bb0, 0x0052df40, "libpng-zlib-image-loader",
     "batch-v-s4 + batch-t-s4 halt"),
    (0x0057c5b0, 0x005913c1, "qhull-2002.1",
     "batch-v-s5 (220 callers in qhull cluster); s_qhull_input_*/qh_* anchors"),
    (0x00591710, 0x0059b455, "qhull-2002.1-continuation",
     "batch-z-s1 halt (qh_partitionpoint..qh_appendvertex span)"),
    (0x0059b460, 0x0059f5d1, "qhull-2002.1-more",
     "batch-z-s2 Region B (extends qhull past pre-batch bound 0x0057c5b0)"),
    (0x005a0e00, 0x005a5820, "qhull-2002.1-print",
     "batch-y-s6 qhull-print residue tail (qh_printcenter+qhull_input_*)"),
    (0x005b8200, 0x005b8681, "d3dx9-fx-framework",
     "batch-z-s2 Region C (effect-state-block builder + bit-pack cascade)"),
    (0x005c0000, 0x005d0000, "msvc-crt-tail",
     "batch-v-s6 finding (33 FidDB + 23 helpers)"),
]


def band_of(addr: int) -> tuple[str, int, int, str] | None:
    for lo, hi, name, source in BANDS:
        if lo <= addr < hi:
            return (name, lo, hi, source)
    return None


def load_existing_rvas() -> set[str]:
    seen: set[str] = set()
    hex8 = re.compile(r"^(?:0x)?([0-9a-fA-F]{8})$")
    with open(HOOKS, encoding="utf-8-sig") as f:
        for r in csv.reader(f):
            if not r:
                continue
            m = hex8.match(r[0].strip())
            if m:
                seen.add(m.group(1).lower())
    return seen


def load_inventory_rvas() -> set[str]:
    seen: set[str] = set()
    hex8 = re.compile(r"^(?:0x)?([0-9a-fA-F]{8})$")
    with open(INVENTORY, encoding="utf-8-sig") as f:
        for r in csv.reader(f):
            if not r:
                continue
            m = hex8.match(r[0].strip())
            if m:
                seen.add(m.group(1).lower())
    return seen


def main() -> int:
    existing = load_existing_rvas()
    inventory = load_inventory_rvas()
    unmapped = inventory - existing
    print(f"inventory:        {len(inventory)}")
    print(f"mapped (pre-run): {len(existing)}")
    print(f"unmapped:         {len(unmapped)}")

    per_band_added: dict[str, int] = {}
    per_band_skipped_unbanded: list[str] = []
    rows_to_append: list[list[str]] = []

    for rva in sorted(unmapped):
        addr = int(rva, 16)
        b = band_of(addr)
        if b is None:
            per_band_skipped_unbanded.append(rva)
            continue
        name, lo, hi, source = b
        subsystem = f"third-party-library[{name}]"
        note = (
            f"library_residue band={name} span=0x{lo:08x}..0x{hi:08x} "
            f"source={source}; C0 band-level classification, do NOT promote"
        )
        rows_to_append.append([
            f"0x{rva}",
            f"FUN_{rva}",
            subsystem,
            "C0",
            "mapped",
            "",  # file
            "",  # scenario
            "",  # frida_diff
            note,
        ])
        per_band_added[name] = per_band_added.get(name, 0) + 1

    print()
    print("=== Per-band rows to append ===")
    for k in sorted(per_band_added, key=lambda k: -per_band_added[k]):
        print(f"  {k}: {per_band_added[k]}")
    print(f"  TOTAL: {sum(per_band_added.values())}")

    if per_band_skipped_unbanded:
        print()
        print(
            f"=== Skipped (unmapped but NOT in any calibrated band) "
            f"-> game-code candidates for batch_aa "
            f"({len(per_band_skipped_unbanded)}) ==="
        )

    if not rows_to_append:
        print("\nNothing to append.")
        return 0

    backup = HOOKS.with_suffix(".csv.bak.library_residue")
    shutil.copy2(HOOKS, backup)
    print(f"\nBackup at {backup.relative_to(ROOT)}")

    new_path = HOOKS.with_suffix(".csv.new")
    with open(HOOKS, encoding="utf-8-sig") as src, open(
        new_path, "w", encoding="utf-8", newline=""
    ) as dst:
        dst.write(src.read())
        w = csv.writer(dst, lineterminator="\n")
        for r in rows_to_append:
            w.writerow(r)
    new_path.replace(HOOKS)
    print(f"Appended {len(rows_to_append)} rows to {HOOKS.relative_to(ROOT)}")

    post = load_existing_rvas()
    pct_pre = 100.0 * len(existing) / len(inventory)
    pct_post = 100.0 * len(post) / len(inventory)
    print(f"\nCoverage: {len(existing)} -> {len(post)} unique RVAs of {len(inventory)}")
    print(f"          {pct_pre:.1f}% -> {pct_post:.1f}% mapped")
    print(
        f"Remaining unmapped (real game-code candidates): "
        f"{len(post) and (len(inventory) - len(post)) or 0}"
    )

    return 0


if __name__ == "__main__":
    sys.exit(main())
