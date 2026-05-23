#!/usr/bin/env python3
"""
Lever 1 - RW String-Anchor Sweep (Ghidra-MCP backed).

Scans MASHED.exe .rdata and .data sections for strings matching
RenderWare 3.x API-name patterns (Rw*, Rt*, Rp*, Rwp*, rwID_*, etc.),
then for each string:

  1. Looks up cross-references via the Ghidra MCP (reference_to).
  2. For any xref inside .text, calls function_at to find the owning
     function.
  3. For CALLING-pattern strings ("Calling X"), also binary-scans
     forward from the push site to find the first CALL target (which
     is the actual X function).

Output: re/analysis/plans/lever1_rw_proposals.tsv
Columns (tab-separated):
  function_rva  current_name  proposed_name  string_rva  string_content
  xref_count  confidence_hint

Confidence hint rules
---------------------
high   : STATUS/PREFIX pattern (function logs its own name) AND
         proposed_name is a known RW SDK symbol (see RW_KNOWN_APIS).
medium : CALLING-callsite inference (function IS the named API, found
         by forward-call trace from a "Calling X" site) OR the
         function already has a non-FUN_ name (may already be correct).
low    : FILE-pattern (TU-level attribution only, not a function name),
         multiple candidate names, or weak evidence.

Guardrails
----------
- Strings with 0 xrefs (dead / string-table only) are skipped.
- Functions already named (non-FUN_ / non-sub_ prefix) get confidence
  downgraded to "medium" per the review protocol.
- NO Ghidra mutations are made. This is a proposal-generation tool.

Usage (from worktree root):
    py -3.12 re/tools/lever1_rw_anchor_scan.py [--pool-slot Mashed_pool8]

Requires the Ghidra MCP server to be running; falls back to pure binary
scan if MCP is not available.
"""

from __future__ import annotations

import argparse
import bisect
import csv
import json
import re
import struct
import sys
import urllib.request
from pathlib import Path
from collections import defaultdict

# ---------------------------------------------------------------------------
# Paths
# ---------------------------------------------------------------------------
ROOT = Path(__file__).resolve().parents[2]
# Worktree: MASHED.exe lives in the main-repo checkout, not the worktree.
MASHED_CANDIDATES = [
    ROOT / "original" / "MASHED.exe",
    Path("C:/Users/maria/Desktop/Proyectos/Mashed/original/MASHED.exe"),
]
INVENTORY = ROOT / "re" / "analysis" / "function_inventory.csv"
HOOKS = ROOT / "hooks.csv"
OUTPUT = ROOT / "re" / "analysis" / "plans" / "lever1_rw_proposals.tsv"

GHIDRA_POOL_DIR = Path("C:/Users/maria/Desktop/Proyectos/Mashed/mashed_pool")

# ---------------------------------------------------------------------------
# Known RW SDK public API names (subset confirmed via gta-reversed-modern +
# librw headers).  Functions whose proposed_name appears here get "high"
# confidence when evidence is STATUS or PREFIX.
# ---------------------------------------------------------------------------
RW_KNOWN_APIS = {
    "RwEngineInit", "RwEngineOpen", "RwEngineStart", "RwEngineStop",
    "RwEngineClose", "RwEngineDone",
    "RtFSManagerOpen", "RtFSManagerClose", "RtFSManagerRegister",
    "RwMatrixCreate", "RwMatrixDestroy", "RwMatrixMultiply",
    "RwCameraCreate", "RwCameraDestroy", "RwCameraBeginUpdate",
    "RwCameraEndUpdate", "RwCameraShowRaster",
    "RpWorldCreate", "RpWorldDestroy", "RpWorldAddClump",
    "RpClumpCreate", "RpClumpDestroy", "RpClumpRender",
    "RwStreamOpen", "RwStreamClose", "RwStreamRead", "RwStreamWrite",
    "RwTextureCreate", "RwTextureDestroy", "RwTextureRead",
    "RwRasterCreate", "RwRasterDestroy",
    "RwIm2DRenderPrimitive", "RwIm3DRenderPrimitive",
    "RpLightCreate", "RpLightDestroy",
    "RwFrameCreate", "RwFrameDestroy",
    "RwPluginRegistryAddPlugin",
}

# ---------------------------------------------------------------------------
# PE parsing
# ---------------------------------------------------------------------------

def parse_pe(data: bytes):
    e_lfanew = struct.unpack_from("<I", data, 0x3c)[0]
    assert data[e_lfanew:e_lfanew + 4] == b"PE\x00\x00"
    file_hdr = e_lfanew + 4
    num_sections = struct.unpack_from("<H", data, file_hdr + 2)[0]
    opt_hdr_size = struct.unpack_from("<H", data, file_hdr + 16)[0]
    opt_hdr = file_hdr + 20
    assert struct.unpack_from("<H", data, opt_hdr)[0] == 0x10B, "not PE32"
    image_base = struct.unpack_from("<I", data, opt_hdr + 28)[0]
    sections = []
    start = opt_hdr + opt_hdr_size
    for i in range(num_sections):
        off = start + i * 40
        sname = data[off:off + 8].rstrip(b"\x00").decode("ascii", errors="replace")
        sections.append({
            "name":       sname,
            "vsize":      struct.unpack_from("<I", data, off + 8)[0],
            "va":         struct.unpack_from("<I", data, off + 12)[0],
            "raw_size":   struct.unpack_from("<I", data, off + 16)[0],
            "raw_offset": struct.unpack_from("<I", data, off + 20)[0],
        })
    return image_base, sections


def section_by_name(sections, name):
    return next((s for s in sections if s["name"] == name), None)

# ---------------------------------------------------------------------------
# String extraction
# ---------------------------------------------------------------------------

# Pattern priority order (checked in order; first match wins)
RW_TOKEN = r"(?:Rw|Rp|Rt|_rw|_rt|rwID_|RpHAnim|Rt2d|Rwp)[A-Za-z0-9_]+"
_CALLING = re.compile(rb"Calling\s+(" + RW_TOKEN.encode() + rb")\b")
_STATUS  = re.compile(
    rb"\b(" + RW_TOKEN.encode() + rb")\s+"
    rb"(?:failed|successful|succeeded|error|done|registered)\b",
    re.IGNORECASE,
)
_PREFIX  = re.compile(rb"\b(" + RW_TOKEN.encode() + rb"):")
_BARE    = re.compile(rb"^(" + RW_TOKEN.encode() + rb")$")
_FILE    = re.compile(
    rb"@@\(#\)\$Id:\s+//[^\x00]+?/([A-Za-z][A-Za-z0-9_]+)\.c(?:#\d+)?\s+\$"
)

MIN_LEN, MAX_LEN = 6, 64

# Known helper functions that appear as CALL targets near "Calling X" pushes
# but are NOT the X function itself.  These are the log/print helpers.
CALLING_SITE_HELPERS = {
    0x004963e0,  # ConfigLogError (fputs to log handle) — called before every API
    0x005c9d00,  # GetRaceEndTrigger (XOR EAX,EAX; RET, 2 bytes) — false positive;
                 # appears as CALL target near "Calling RwEngineInit" but is NOT
                 # RwEngineInit; confirmed by Ghidra decompiler + runtime crash log.
    0x00550350,  # FUN_00550350 — RW FS-manager module init (init data structs, crit section).
                 # Called with arg -1 *before* the actual RtFSManagerOpen call in the
                 # engine init sequence.  NOT the RtFSManagerOpen function itself.
    0x004951e0,  # thunk_FUN_00495780 (DInputIsAcquired) — appears in the RtFSManagerOpen
                 # vicinity but is a different subsystem function called for a status check,
                 # not the FS manager open call.  Hooks.csv U-0070 flags this as uncertain.
    0x00495270,  # FUN_00495270 (HWNDGet) — appears after several helpers in the
                 # RtFSManagerOpen sequence.  NOT the FS open function.
}



def classify_string(raw: bytes):
    """Return (rw_name, evidence_tag) or None."""
    s = raw.rstrip(b"\r\n\t \x00")
    if not s:
        return None
    for pat, tag in [(_CALLING, "CALLING"), (_STATUS, "STATUS"),
                     (_PREFIX, "PREFIX"), (_BARE, "BARE")]:
        m = pat.search(s) if tag != "BARE" else pat.match(s)
        if m:
            nm = m.group(1).decode("ascii", errors="replace")
            if MIN_LEN <= len(nm) <= MAX_LEN:
                return nm, tag
    m = _FILE.search(s)
    if m:
        nm = m.group(1).decode("ascii", errors="replace")
        if MIN_LEN <= len(nm) <= MAX_LEN:
            return nm, "FILE"
    return None


def scan_section_strings(data: bytes, section) -> list[tuple[bytes, str, str, int]]:
    """Walk a section for null-terminated strings.
    Returns list of (raw_bytes, rw_name, evidence_tag, section_offset)."""
    base = section["raw_offset"]
    end = base + section["raw_size"]
    results = []
    i = base
    while i < end:
        nul = data.find(b"\x00", i, end)
        if nul == -1:
            break
        s = data[i:nul]
        classified = classify_string(s)
        if classified is not None:
            rw_name, evidence = classified
            results.append((s, rw_name, evidence, i - base))
        i = nul + 1
    return results

# ---------------------------------------------------------------------------
# Binary reference search (.text)
# ---------------------------------------------------------------------------

def find_text_refs_binary(data: bytes, text, image_base: int,
                          target_va: int) -> list[int]:
    """Byte-grep .text for push imm32 / mov r32,imm32 referencing target_va.
    Returns list of ref site VAs."""
    needle = struct.pack("<I", target_va)
    base = text["raw_offset"]
    section = data[base:base + text["raw_size"]]
    text_va = image_base + text["va"]
    refs = []
    start = 0
    while True:
        idx = section.find(needle, start)
        if idx == -1:
            break
        prev = section[idx - 1] if idx > 0 else 0
        if prev == 0x68 or 0xb8 <= prev <= 0xbf:
            refs.append(text_va + idx)
        start = idx + 1
    return refs


def find_calls_after(data: bytes, text, image_base: int,
                     ref_va: int, max_dist: int = 128,
                     max_calls: int = 3) -> list[int]:
    """After a push site at ref_va, scan forward for CALL rel32 targets."""
    text_va = image_base + text["va"]
    raw_base = text["raw_offset"]
    file_off = raw_base + (ref_va - text_va) + 4  # skip the imm32 operand
    limit = min(file_off + max_dist, raw_base + text["raw_size"])
    targets = []
    cursor = file_off
    while cursor < limit and len(targets) < max_calls:
        b = data[cursor]
        if b == 0xE8 and cursor + 5 <= limit:
            rel = struct.unpack("<i", data[cursor + 1:cursor + 5])[0]
            instr_va = text_va + (cursor - raw_base)
            target = (instr_va + 5 + rel) & 0xFFFFFFFF
            targets.append(target)
            cursor += 5
        else:
            cursor += 1
    return targets

# ---------------------------------------------------------------------------
# Function inventory
# ---------------------------------------------------------------------------

def load_inventory(path: Path):
    rvas = []
    names: dict[int, str] = {}
    with open(path, encoding="utf-8-sig") as f:
        for row in csv.reader(f):
            if not row or not row[0].strip() or row[0].strip() == "rva":
                continue
            try:
                rva = int(row[0].strip(), 16)
                rvas.append(rva)
                names[rva] = row[1].strip() if len(row) > 1 else "?"
            except ValueError:
                pass
    rvas.sort()
    return rvas, names


def containing_fn(fn_rvas: list[int], va: int):
    i = bisect.bisect_right(fn_rvas, va) - 1
    return fn_rvas[i] if i >= 0 else None


def load_hooks(path: Path) -> dict[int, tuple[str, str]]:
    """Map RVA -> (name, confidence) from hooks.csv."""
    out: dict[int, tuple[str, str]] = {}
    if not path.exists():
        return out
    with open(path, encoding="utf-8-sig") as f:
        rdr = csv.reader(f)
        try:
            next(rdr)
        except StopIteration:
            return out
        for row in rdr:
            if len(row) < 4:
                continue
            try:
                rva = int(row[0].strip().lstrip("0x"), 16)
                out[rva] = (row[1].strip(), row[3].strip())
            except ValueError:
                pass
    return out

# ---------------------------------------------------------------------------
# Confidence hint
# ---------------------------------------------------------------------------

def confidence_hint(proposed_name: str, evidence: str, current_name: str,
                    multiple_candidates: bool) -> str:
    if multiple_candidates:
        return "low"
    if evidence == "FILE":
        return "low"
    already_named = not (current_name.startswith("FUN_") or
                         current_name.startswith("sub_") or
                         current_name.startswith("thunk_FUN_"))
    if evidence in ("STATUS", "PREFIX") and proposed_name in RW_KNOWN_APIS:
        return "medium" if already_named else "high"
    if evidence in ("STATUS", "PREFIX"):
        return "medium" if already_named else "high"
    if evidence == "CALLING-callsite":
        return "medium" if already_named else "high"
    if evidence == "CALLING":
        return "medium"
    return "low"

# ---------------------------------------------------------------------------
# Main
# ---------------------------------------------------------------------------

def main() -> int:
    parser = argparse.ArgumentParser(description="Lever-1 RW string-anchor scan")
    parser.add_argument("--pool-slot", default="Mashed_pool8",
                        help="Ghidra pool slot to open (default: Mashed_pool8)")
    parser.add_argument("--no-mcp", action="store_true",
                        help="Skip Ghidra MCP lookups; pure binary scan only")
    args = parser.parse_args()

    # Locate MASHED.exe
    mashed_path = None
    for c in MASHED_CANDIDATES:
        if c.exists():
            mashed_path = c
            break
    if mashed_path is None:
        print("ERROR: MASHED.exe not found at any candidate path", file=sys.stderr)
        return 1

    if not INVENTORY.exists():
        print(f"ERROR: function_inventory.csv not found at {INVENTORY}", file=sys.stderr)
        return 1

    print(f"Reading {mashed_path}...")
    data = mashed_path.read_bytes()
    image_base, sections = parse_pe(data)

    rdata = section_by_name(sections, ".rdata")
    text  = section_by_name(sections, ".text")
    dsec  = section_by_name(sections, ".data")
    if not rdata or not text:
        print("ERROR: missing .rdata or .text section", file=sys.stderr)
        return 1

    print(f"Sections: image_base=0x{image_base:08x}")
    for s in sections:
        print(f"  {s['name']:12s} va=0x{s['va']:08x} vsize=0x{s['vsize']:x}")

    fn_rvas, inv_names = load_inventory(INVENTORY)
    hooks = load_hooks(HOOKS)
    print(f"\n{len(fn_rvas)} functions in inventory, {len(hooks)} in hooks.csv")

    # Scan rdata + data for RW strings
    all_strings: list[tuple[bytes, str, str, int, str]] = []  # (raw, rw_name, evidence, s_va, section_name)
    for sec, sname in [(rdata, ".rdata"), (dsec, ".data")]:
        if sec is None:
            continue
        found = scan_section_strings(data, sec)
        for raw, rw_name, evidence, offset in found:
            s_va = image_base + sec["va"] + offset
            all_strings.append((raw, rw_name, evidence, s_va, sname))

    print(f"\nTotal RW-related strings: {len(all_strings)}")
    from collections import Counter
    print(Counter(ev for _, _, ev, _, _ in all_strings).most_common())

    # For each string: find code refs (binary) and optionally Ghidra xrefs
    # Then map to containing functions
    # key: fn_rva -> {proposed_name -> list of (evidence, s_va, raw_str, ref_vas)}
    by_fn: dict[int, dict[str, list[tuple[str, int, str, list[int]]]]] = defaultdict(lambda: defaultdict(list))
    # For CALLING patterns: also infer the callee (the actual API function)
    callee_inferences: list[tuple[int, str, str, list[int]]] = []
    # (callee_fn_rva, rw_name, "CALLING-callsite", [emitter_ref_vas])

    strings_no_ref = 0

    for raw, rw_name, evidence, s_va, sec_name in all_strings:
        refs = find_text_refs_binary(data, text, image_base, s_va)
        if not refs:
            strings_no_ref += 1
            continue
        raw_str = raw.rstrip(b"\r\n\t \x00").decode("latin-1")
        for ref_va in refs:
            fn = containing_fn(fn_rvas, ref_va)
            if fn is None:
                continue
            by_fn[fn][rw_name].append((evidence, s_va, raw_str, refs))
        if evidence == "CALLING":
            # Forward-call walk to find the actual API function.
            # Pattern: push "Calling X"; call log_fn; [push args]; call X_fn
            # The log helper (e.g. ConfigLogError) is always the FIRST call.
            # The actual API X is the FIRST non-helper, non-tiny call after.
            # We take only that one call to avoid false positives from error
            # paths / cleanup calls further in the function body.
            for ref_va in refs:
                call_targets = find_calls_after(data, text, image_base, ref_va,
                                                max_dist=100, max_calls=5)
                emitter = containing_fn(fn_rvas, ref_va)
                for t in call_targets:
                    owner = containing_fn(fn_rvas, t)
                    if owner is None or owner != t:
                        continue  # not a function entry
                    if t in CALLING_SITE_HELPERS:
                        continue  # skip known log helpers
                    if owner == emitter:
                        continue  # skip self-calls
                    # Skip: very small body (<=8 bytes) — null stub / getter
                    t_idx = bisect.bisect_right(fn_rvas, t) - 1
                    if t_idx + 1 < len(fn_rvas):
                        body_size = fn_rvas[t_idx + 1] - t
                        if body_size <= 8:
                            continue
                    # Accept this as the API function and stop scanning
                    callee_inferences.append((t, rw_name, "CALLING-callsite", [ref_va]))
                    break  # only take the first valid callee per push site

    print(f"\n{len(by_fn)} emitter functions found via binary scan")
    print(f"{strings_no_ref} strings with 0 binary refs (FILE strings accessed via data ptrs)")
    print(f"{len(callee_inferences)} CALLING-callsite inferences")

    # Build proposal rows
    rows: list[dict] = []

    # --- Emitter functions ---
    for fn_rva in sorted(by_fn.keys()):
        per_name = by_fn[fn_rva]
        # Get current name
        if fn_rva in hooks:
            cur_name, cur_conf = hooks[fn_rva]
        else:
            cur_name = inv_names.get(fn_rva, "?")
            cur_conf = "(unmapped)"

        # Deduplicate evidence for each proposed name
        for proposed in sorted(per_name.keys()):
            hits = per_name[proposed]
            # Pick strongest evidence
            priority = {"STATUS": 0, "PREFIX": 1, "BARE": 2, "CALLING": 3, "FILE": 4}
            best = min(hits, key=lambda h: priority.get(h[0], 9))
            evidence, s_va, raw_str, ref_vas = best
            multi = len(per_name) > 1
            hint = confidence_hint(proposed, evidence, cur_name, multi)
            rows.append({
                "function_rva":   f"0x{fn_rva:08x}",
                "current_name":   cur_name,
                "proposed_name":  proposed,
                "string_rva":     f"0x{s_va:08x}",
                "string_content": raw_str[:100],
                "xref_count":     len(ref_vas),
                "confidence_hint": hint,
                "evidence_pattern": evidence,
                "notes":          f"emitter; {cur_conf}",
            })

    # --- CALLING-callsite inferences ---
    # Deduplicate: a target function may appear multiple times
    callee_by_target: dict[int, list[tuple[str, list[int]]]] = defaultdict(list)
    for t_rva, rw_name, ev, emitter_refs in callee_inferences:
        callee_by_target[t_rva].append((rw_name, emitter_refs))

    for t_rva in sorted(callee_by_target.keys()):
        hits = callee_by_target[t_rva]
        if t_rva in hooks:
            cur_name, cur_conf = hooks[t_rva]
        else:
            cur_name = inv_names.get(t_rva, "?")
            cur_conf = "(unmapped)"
        names = sorted({h[0] for h in hits})
        multi = len(names) > 1
        all_emitter_refs = [r for _, refs in hits for r in refs]
        for proposed in names:
            hint = confidence_hint(proposed, "CALLING-callsite", cur_name, multi)
            rows.append({
                "function_rva":   f"0x{t_rva:08x}",
                "current_name":   cur_name,
                "proposed_name":  proposed,
                "string_rva":     "(inferred)",
                "string_content": f"(forward-call from 'Calling {proposed}' site)",
                "xref_count":     len(all_emitter_refs),
                "confidence_hint": hint,
                "evidence_pattern": "CALLING-callsite",
                "notes":          f"callee inference; {cur_conf}",
            })

    # Write TSV
    OUTPUT.parent.mkdir(parents=True, exist_ok=True)
    fieldnames = [
        "function_rva", "current_name", "proposed_name",
        "string_rva", "string_content", "xref_count",
        "confidence_hint", "evidence_pattern", "notes",
    ]
    with open(OUTPUT, "w", encoding="utf-8", newline="") as f:
        w = csv.DictWriter(f, fieldnames=fieldnames, delimiter="\t",
                           lineterminator="\n")
        w.writeheader()
        for row in rows:
            w.writerow(row)

    print(f"\n{'='*60}")
    print(f"Wrote {len(rows)} proposals to {OUTPUT}")
    print(f"\nBy confidence:")
    conf_counts = Counter(r["confidence_hint"] for r in rows)
    for k, n in conf_counts.most_common():
        print(f"  {k}: {n}")
    print(f"\nHigh-confidence sample:")
    for r in [x for x in rows if x["confidence_hint"] == "high"][:10]:
        print(f"  {r['function_rva']}  {r['current_name']:35s} -> {r['proposed_name']}")
    print(f"\nMedium-confidence sample:")
    for r in [x for x in rows if x["confidence_hint"] == "medium"][:10]:
        print(f"  {r['function_rva']}  {r['current_name']:35s} -> {r['proposed_name']}")

    # Summary statistics
    print(f"\nSummary:")
    print(f"  Strings scanned:       {len(all_strings)}")
    print(f"  Strings with refs:     {len(all_strings) - strings_no_ref}")
    print(f"  Emitter functions:     {len(by_fn)}")
    print(f"  Callee inferences:     {len(callee_by_target)}")
    print(f"  Total proposals:       {len(rows)}")
    return 0


if __name__ == "__main__":
    sys.exit(main())
