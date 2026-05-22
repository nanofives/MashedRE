# BulkPlateBookmark.py — Ghidra script (PyGhidra)
# Bulk-write PLATE comments and "first-pass" bookmarks for a sweep manifest,
# and (when the function comment carries a single "Library Function:" attribution
# and the Ghidra function name is still FUN_XXXXXXXX) rename to that symbol.
#
# Usage (from ghidra_headless_mcp):
#   ghidra_script(path="scripts/ghidra/BulkPlateBookmark.py",
#                 script_args=[<manifest_json_path>])
#
# Manifest schema (subset used here):
#   { "date": "YYYY-MM-DD",
#     "buckets": [ {"bucket": "bucket_xxxxxxxx",
#                   "rvas": [ {"rva": "0xXXXXXXXX", "plate": "[C1 ...] ..."} ] } ] }
#
# Per-protocol semantics (ghidra-sweep SKILL.md):
#   - PLATE comment_type = "plate".
#   - bookmark category = "first-pass", comment = "C1 YYYY-MM-DD session:<bucket>".
#   - If function_at(rva) is None: try listing.getCodeUnitAt — write plate-as-EOL
#     at listing address (C0 fallback); no bookmark (no function).
#   - If function plate-comment has prefix "Library Function:" and matches a
#     single concrete symbol AND Ghidra function name still starts with FUN_:
#     rename to that exact symbol name.
#
# Output is structured JSON on stdout; the MCP wrapper returns it via 'stdout'.

import json
import re
import sys
import traceback

# Ghidra-provided globals: currentProgram, monitor, getScriptArgs(), askString,
# createBookmark, getFunctionAt, getSymbolAt, etc.
from ghidra.program.model.listing import CodeUnit  # type: ignore
from ghidra.program.model.address import Address  # type: ignore
from ghidra.util.exception import CancelledException  # type: ignore

# ---------------------------------------------------------------------------

LIBPREFIX_RE = re.compile(r"^Library Function:\s*([A-Za-z_][A-Za-z0-9_:?@$]*)\s*$")

def _addr(rva_hex):
    # rva_hex is "0xXXXXXXXX" — Ghidra image base is 0x00400000 already encoded.
    af = currentProgram.getAddressFactory()
    return af.getAddress(rva_hex)

def _has_library_attestation(existing_plate):
    """Return the single symbol name if the existing plate is exactly a
    'Library Function: <name>' single-line attestation, else None."""
    if not existing_plate:
        return None
    lines = [l for l in existing_plate.strip().splitlines() if l.strip()]
    if len(lines) != 1:
        return None
    m = LIBPREFIX_RE.match(lines[0].strip())
    if not m:
        return None
    return m.group(1)

def process(manifest_path):
    with open(manifest_path, "r", encoding="utf-8") as f:
        manifest = json.load(f)

    date = manifest.get("date", "")
    listing = currentProgram.getListing()
    af = currentProgram.getAddressFactory()
    symtab = currentProgram.getSymbolTable()

    summary = {"date": date, "buckets": []}
    grand = {"plates": 0, "bookmarks": 0, "renames": 0,
             "missing_rvas": [], "missing_md": [], "errors": []}

    for b in manifest["buckets"]:
        bucket = b["bucket"]
        bsum = {"bucket": bucket, "plates": 0, "bookmarks": 0, "renames": 0,
                "missing_rvas": [], "c0_listing_plates": [], "errors": []}
        bookmark_comment = "C1 %s session:%s" % (date, bucket)

        for entry in b["rvas"]:
            rva = entry["rva"]
            plate = entry.get("plate", "")
            if not plate:
                grand["missing_md"].append(rva)
                continue
            try:
                addr = af.getAddress(rva)
                if addr is None:
                    bsum["missing_rvas"].append(rva)
                    grand["missing_rvas"].append(rva)
                    continue

                fn = getFunctionAt(addr)
                if fn is not None:
                    # Read existing comments BEFORE we overwrite the plate.
                    # FidDB attestations from Ghidra's analyzer typically land
                    # on the function's repeatable comment or the function
                    # comment (Function.getComment()) — not on the PLATE_COMMENT
                    # we are about to set. Scan all relevant slots.
                    pre_fn_comment = fn.getComment() or ""
                    lib_candidates = set()
                    cand = _has_library_attestation(pre_fn_comment)
                    if cand: lib_candidates.add(cand)
                    for ct in (CodeUnit.PRE_COMMENT, CodeUnit.POST_COMMENT,
                               CodeUnit.EOL_COMMENT, CodeUnit.REPEATABLE_COMMENT,
                               CodeUnit.PLATE_COMMENT):
                        c = listing.getComment(ct, addr)
                        if c:
                            cand = _has_library_attestation(c)
                            if cand: lib_candidates.add(cand)

                    # PLATE on function (PLATE_COMMENT)
                    listing.setComment(addr, CodeUnit.PLATE_COMMENT, plate)
                    bsum["plates"] += 1
                    grand["plates"] += 1

                    # Bookmark
                    createBookmark(addr, "first-pass", bookmark_comment)
                    bsum["bookmarks"] += 1
                    grand["bookmarks"] += 1

                    # FidDB rename — only if a single concrete symbol attested
                    # AND the function still has a generic FUN_ name.
                    if len(lib_candidates) == 1:
                        target = list(lib_candidates)[0]
                        cur_name = fn.getName()
                        if cur_name.startswith("FUN_"):
                            try:
                                fn.setName(target, ghidra.program.model.symbol.SourceType.USER_DEFINED)
                                bsum["renames"] += 1
                                grand["renames"] += 1
                            except Exception as e:
                                bsum["errors"].append("rename %s -> %s: %s" % (rva, target, str(e)))
                                grand["errors"].append("rename %s -> %s: %s" % (rva, target, str(e)))
                else:
                    # C0 fallback: try listing.getCodeUnitAt
                    cu = listing.getCodeUnitAt(addr)
                    if cu is None:
                        bsum["missing_rvas"].append(rva)
                        grand["missing_rvas"].append(rva)
                        continue
                    # Write plate at listing-level
                    listing.setComment(addr, CodeUnit.PLATE_COMMENT, plate)
                    bsum["plates"] += 1
                    grand["plates"] += 1
                    bsum["c0_listing_plates"].append(rva)
            except Exception as e:
                tb = traceback.format_exc()
                bsum["errors"].append("rva=%s err=%s tb=%s" % (rva, str(e), tb))
                grand["errors"].append("rva=%s err=%s" % (rva, str(e)))

        summary["buckets"].append(bsum)

    summary["totals"] = grand
    print("==SWEEP_RESULT_JSON_BEGIN==")
    print(json.dumps(summary, indent=2))
    print("==SWEEP_RESULT_JSON_END==")


# Entrypoint -----------------------------------------------------------------
import ghidra.program.model.symbol  # ensure SourceType is available  # noqa: E402

args = getScriptArgs()
if not args:
    print("ERROR: manifest path required as script arg")
    sys.exit(1)

process(args[0])
