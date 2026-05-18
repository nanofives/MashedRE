#!/usr/bin/env python3
"""
Lever 2 of the C1 brute-force strategy — RenderWare function-name catalog.

Mines gta-reversed-modern/source/game_sa/RenderWare/ and librw/src/ for all
RW API function declarations (Rw*/Rp*/Rt*/_rw*/_rt*) and emits a catalog
TSV that worker sessions can use during decomp inspection. For each Mashed
function the worker is reading, they can compare:

  - The function's signature shape (param count, return type) against the
    catalog;
  - The function's callees against the catalog (e.g., if a Mashed function
    calls another that matches RwEngineOpen, the caller is likely an
    RW-bootstrap orchestrator).
  - Any internal strings or constants against catalog entries' headers.

Output: re/analysis/plans/rw_function_catalog.tsv
  rw_name  return_type  arg_signature  module  source_file

This is research-input, NOT auto-rename data. The catalog feeds into worker
sessions during C0->C1 / C1->C2 work — the SCRIBE_QUEUE rows from those
sessions will cite catalog evidence by name + file.

Usage:
    py -3.12 re/tools/rw_catalog_extract.py
"""

from __future__ import annotations

import csv
import re
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
GTA_RW = ROOT / "re" / "prior_art" / "renderware" / "gta-reversed-modern" / \
          "source" / "game_sa" / "RenderWare"
LIBRW = ROOT / "re" / "prior_art" / "renderware" / "librw" / "src"
OUTPUT = ROOT / "re" / "analysis" / "plans" / "rw_function_catalog.tsv"

# Function-declaration regex. Catches:
#   `RwBool RwEngineInit(RwMemoryFunctions *memFuncs, ...);`
#   `RwTexture *RwTextureCreate(RwRaster *raster);`
#   `void* RwMalloc(RwUInt32 size, RwUInt32 hint);`
#   `static RwBool _rwError(...)`  (also catches static helpers)
#
# Group 1 = return type (loose; can have multiple words + pointer)
# Group 2 = function name (must start with RW prefix)
# Group 3 = arg list (no nested parens)
DECL_RE = re.compile(
    r"^[ \t]*"                                    # line start
    r"(?:RWEXPORT\s+|extern\s+(?:\"C\"\s+)?|static\s+|inline\s+|RWFUNCTION\s+)*"
    r"((?:const\s+)?(?:struct\s+)?[\w]+(?:\s*\*+)?)\s+"   # return type
    r"((?:Rw|Rp|Rt|_rw|_rt|rwID_)[A-Z]\w+)\s*"            # function name
    r"\(([^)]*)\)",                                       # arg list
    re.MULTILINE,
)

# Skip lines that are obviously not declarations (defines, comments, typedefs).
SKIP_RE = re.compile(r"^[ \t]*(?://|#|typedef\b|/\*)")

# Module classifier: map a function name prefix to its broad RW module.
MODULE_MAP = [
    ("RwEngine",   "engine"),
    ("RwStream",   "stream"),
    ("RwMatrix",   "matrix"),
    ("RwFrame",    "frame"),
    ("RwCamera",   "camera"),
    ("RwLight",    "light"),
    ("RwTexture",  "texture"),
    ("RwImage",    "image"),
    ("RwRaster",   "raster"),
    ("RwTexDict",  "texdict"),
    ("RwIm2D",     "im2d"),
    ("RwIm3D",     "im3d"),
    ("RwResources","resources"),
    ("RwMaterial", "material"),
    ("RwRenderState", "renderstate"),
    ("RwError",    "error"),
    ("RwOs",       "os"),
    ("RwSky",      "sky"),
    ("RpWorld",    "world"),
    ("RpClump",    "clump"),
    ("RpAtomic",   "atomic"),
    ("RpGeometry", "geometry"),
    ("RpMaterial", "material"),
    ("RpLight",    "light"),
    ("RpHAnim",    "hanim"),
    ("RpAnim",     "anim"),
    ("RpSkin",     "skin"),
    ("RpMatFX",    "matfx"),
    ("RpMorph",    "morph"),
    ("RtFSManager","fsmanager"),
    ("RtCharset",  "charset"),
    ("Rt2d",       "rt2d"),
    ("_rw",        "internal-rw"),
    ("_rp",        "internal-rp"),
    ("_rt",        "internal-rt"),
]


def classify_module(name: str) -> str:
    for prefix, module in MODULE_MAP:
        if name.startswith(prefix):
            return module
    return "other"


def extract_from_file(path: Path) -> list[dict]:
    """Return a list of declaration dicts for one source file."""
    try:
        txt = path.read_text(encoding="utf-8", errors="replace")
    except Exception:
        return []
    decls = []
    seen = set()
    for line_idx, line in enumerate(txt.splitlines()):
        if SKIP_RE.match(line):
            continue
        for m in DECL_RE.finditer(line):
            ret = m.group(1).strip()
            name = m.group(2).strip()
            args = m.group(3).strip()
            # Filter false positives — return type "if"/"else"/etc.
            if ret in {"if", "else", "for", "while", "switch", "return",
                        "case", "default"}:
                continue
            # Dedup: a function declared in both .h and .cpp shows up twice;
            # keep only the first per file but we'll dedup across files later.
            key = (name, len(args.split(",")))
            if key in seen:
                continue
            seen.add(key)
            decls.append({
                "name": name,
                "return_type": ret,
                "args": args,
                "module": classify_module(name),
                "source_file": str(path.relative_to(ROOT)),
                "line": line_idx + 1,
            })
    return decls


def walk_source_dir(root: Path) -> list[dict]:
    if not root.exists():
        return []
    all_decls: list[dict] = []
    for p in root.rglob("*"):
        if not p.is_file():
            continue
        if p.suffix.lower() not in {".h", ".hpp", ".cpp", ".cc", ".c"}:
            continue
        all_decls.extend(extract_from_file(p))
    return all_decls


def main() -> int:
    print(f"Scanning: {GTA_RW}")
    gta_decls = walk_source_dir(GTA_RW)
    print(f"  {len(gta_decls)} declarations")
    print(f"Scanning: {LIBRW}")
    librw_decls = walk_source_dir(LIBRW)
    print(f"  {len(librw_decls)} declarations")

    all_decls = gta_decls + librw_decls
    print(f"\n{len(all_decls)} total declarations before dedup")

    # Dedup by (name, arg_count). Keep the first source file for evidence;
    # additional sources go into a `also_in` field.
    by_name: dict[str, dict] = {}
    for d in all_decls:
        key = d["name"]
        if key in by_name:
            existing = by_name[key]
            # If the other source has a longer arg list, prefer it (more
            # informative signature).
            if len(d["args"]) > len(existing["args"]):
                existing["return_type"] = d["return_type"]
                existing["args"] = d["args"]
            # Always track all source paths for cross-reference.
            existing.setdefault("also_in", []).append(d["source_file"])
        else:
            by_name[key] = d

    print(f"{len(by_name)} unique RW function names\n")

    # Stats by module
    from collections import Counter
    by_module = Counter(d["module"] for d in by_name.values())
    print("By module:")
    for mod, n in by_module.most_common():
        print(f"  {mod:18s}  {n}")

    # Emit catalog
    OUTPUT.parent.mkdir(parents=True, exist_ok=True)
    with open(OUTPUT, "w", encoding="utf-8", newline="") as f:
        w = csv.writer(f, delimiter="\t", lineterminator="\n")
        w.writerow([
            "rw_name", "module", "return_type", "arg_signature",
            "source_file", "line", "also_in",
        ])
        for name in sorted(by_name.keys()):
            d = by_name[name]
            also = ";".join(d.get("also_in", []))
            w.writerow([
                d["name"], d["module"], d["return_type"],
                d["args"], d["source_file"], d["line"], also,
            ])

    print(f"\nWrote: {OUTPUT}")

    # Cross-reference: how many catalog entries match MASHED.exe substrings?
    mashed_path = ROOT / "original" / "MASHED.exe"
    if mashed_path.exists():
        data = mashed_path.read_bytes()
        matches = sum(1 for n in by_name.keys() if n.encode() in data)
        print(f"\nMASHED.exe substring presence: {matches} of {len(by_name)} "
              f"catalog names ({100*matches/len(by_name):.1f}%)")
        # Show a sample of matches
        sample = [n for n in sorted(by_name.keys()) if n.encode() in data][:20]
        print("Sample matches:")
        for n in sample:
            print(f"  {n:35s}  module={by_name[n]['module']}")

    return 0


if __name__ == "__main__":
    import sys
    sys.exit(main())
