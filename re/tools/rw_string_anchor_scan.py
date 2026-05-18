#!/usr/bin/env python3
"""
Lever 1 of the C1 brute-force strategy — RW string-anchor sweep.

Scans MASHED.exe for null-terminated strings shaped like RenderWare 3.x API
names (`Rw*`, `Rp*`, `Rt*`, `_rw*`, `_rt*`, `rwID_*`). For each such string,
brute-greps the .text section for `push imm32` and `mov reg, imm32`
references whose imm32 equals the string's VA, then maps each reference to
the containing function via `re/analysis/function_inventory.csv`.

Output: re/analysis/plans/rw_string_anchor_proposals.tsv
  function_rva  current_name  suggested_name  string  string_va  ref_count  ref_offsets

The "suggested_name" is the RW string itself — chosen on the well-known RW
convention that each public API function emits an error string carrying its
own name (`_rwError("RwEngineInit", ...)`). The strongest evidence is "the
ONLY RW-named string this function references" — those are 1:1 matches.
Multi-string functions are emitted with all candidates; reviewer picks.

This tool produces a *proposal* TSV. NO Ghidra mutation. Apply via a
follow-up `ghidra-sweep` after manual review.

Usage:
    py -3.12 re/tools/rw_string_anchor_scan.py
"""

from __future__ import annotations

import bisect
import csv
import re
import struct
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
MASHED = ROOT / "original" / "MASHED.exe"
INVENTORY = ROOT / "re" / "analysis" / "function_inventory.csv"
HOOKS = ROOT / "hooks.csv"
OUTPUT = ROOT / "re" / "analysis" / "plans" / "rw_string_anchor_proposals.tsv"

# RW API names embedded in log/error strings. The key insight (verified
# 2026-05-18 against MASHED.exe rdata): RW names rarely appear bare —
# they're wrapped in messages like "Calling RwEngineInit\n",
# "RtFSManagerRegister failed\n", "RpHAnimAnimationCreate: bad arg".
# We extract the API name token via regex and infer the emitting function
# from the log-message phrasing.
#
# Patterns (in priority order):
#   PATTERN_CALLING = "Calling X(\n?)"        -> emitter is CALLER of X
#   PATTERN_STATUS  = "X (failed|successful|error|done)" -> emitter IS X
#   PATTERN_PREFIX  = "X: ..." (RW-shaped X)             -> emitter likely IS X
#   PATTERN_BARE    = standalone token "X"               -> low-conf reference
#
# The emitter is the function in .text that pushes the string's VA. For
# PATTERN_CALLING, the called function (X) is at the target of the next
# CALL instruction after the push.
RW_TOKEN = r"(?:Rw|Rp|Rt|_rw|_rt|rwID_|RpHAnim|Rt2d|Rwp)[A-Za-z0-9_]+"
PATTERN_CALLING = re.compile(rb"Calling\s+(" + RW_TOKEN.encode() + rb")\b")
PATTERN_STATUS  = re.compile(rb"\b(" + RW_TOKEN.encode() + rb")\s+(?:failed|successful|succeeded|error|done|registered)\b", re.IGNORECASE)
PATTERN_PREFIX  = re.compile(rb"\b(" + RW_TOKEN.encode() + rb"):")
PATTERN_BARE    = re.compile(rb"^(" + RW_TOKEN.encode() + rb")$")
# RCS/$Header-style __FILE__ source-file paths. The full string typically
# looks like "Physics/Rwp37Active/src/core/RwpMgr.c#2 $". Extract the module
# basename (e.g., RwpMgr) — every function referencing this string belongs
# to the same translation unit.
PATTERN_FILE    = re.compile(rb"(?:[A-Za-z0-9_/-]+/)?(" + RW_TOKEN.encode() + rb")\.c(?:#\d+)?(?:\s+\$)?")

MIN_NAME_LEN = 6
MAX_NAME_LEN = 64


def parse_pe(data: bytes):
    """Return (image_base, sections). Each section is a dict with name,
    virtual_size, virtual_address, raw_size, raw_offset."""
    e_lfanew = struct.unpack_from("<I", data, 0x3c)[0]
    assert data[e_lfanew:e_lfanew + 4] == b"PE\x00\x00", "not a PE file"
    file_hdr = e_lfanew + 4
    num_sections = struct.unpack_from("<H", data, file_hdr + 2)[0]
    opt_hdr_size = struct.unpack_from("<H", data, file_hdr + 16)[0]
    opt_hdr = file_hdr + 20
    magic = struct.unpack_from("<H", data, opt_hdr)[0]
    assert magic == 0x10b, f"PE32+ unsupported, magic={magic:#x}"
    image_base = struct.unpack_from("<I", data, opt_hdr + 28)[0]
    sections_start = opt_hdr + opt_hdr_size
    sections = []
    for i in range(num_sections):
        off = sections_start + i * 40
        name = data[off:off + 8].rstrip(b"\x00").decode("ascii", errors="replace")
        vsize = struct.unpack_from("<I", data, off + 8)[0]
        va = struct.unpack_from("<I", data, off + 12)[0]
        raw_size = struct.unpack_from("<I", data, off + 16)[0]
        raw_offset = struct.unpack_from("<I", data, off + 20)[0]
        sections.append({
            "name": name, "vsize": vsize, "va": va,
            "raw_size": raw_size, "raw_offset": raw_offset,
        })
    return image_base, sections


def find_section(sections, name):
    for s in sections:
        if s["name"] == name:
            return s
    return None


def classify_string(s: bytes) -> tuple[str, str] | None:
    """Return (rw_name, evidence_pattern) if s is an RW-related log string,
    else None. evidence_pattern is one of CALLING/STATUS/PREFIX/BARE."""
    # Strip trailing whitespace + newlines for matching
    stripped = s.rstrip(b"\r\n\t \x00")
    if not stripped:
        return None
    m = PATTERN_CALLING.search(stripped)
    if m:
        name = m.group(1).decode("ascii", errors="replace")
        if MIN_NAME_LEN <= len(name) <= MAX_NAME_LEN:
            return name, "CALLING"
    m = PATTERN_STATUS.search(stripped)
    if m:
        name = m.group(1).decode("ascii", errors="replace")
        if MIN_NAME_LEN <= len(name) <= MAX_NAME_LEN:
            return name, "STATUS"
    m = PATTERN_PREFIX.search(stripped)
    if m:
        name = m.group(1).decode("ascii", errors="replace")
        if MIN_NAME_LEN <= len(name) <= MAX_NAME_LEN:
            return name, "PREFIX"
    m = PATTERN_BARE.match(stripped)
    if m:
        name = m.group(1).decode("ascii", errors="replace")
        if MIN_NAME_LEN <= len(name) <= MAX_NAME_LEN:
            return name, "BARE"
    m = PATTERN_FILE.search(stripped)
    if m:
        name = m.group(1).decode("ascii", errors="replace")
        if MIN_NAME_LEN <= len(name) <= MAX_NAME_LEN:
            return name, "FILE"
    return None


def scan_rdata_strings(data: bytes, rdata):
    """Walk rdata for null-terminated strings; return list of
    (raw_string, rw_name, evidence, rdata_offset)."""
    base = rdata["raw_offset"]
    end = base + rdata["raw_size"]
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


def load_function_entries():
    """Load (sorted) list of function-entry RVAs from function_inventory.csv."""
    rvas = []
    names = {}
    with open(INVENTORY, encoding="utf-8-sig") as f:
        rdr = csv.reader(f)
        next(rdr)
        for r in rdr:
            if not r or not r[0].strip():
                continue
            rva = int(r[0].strip(), 16)
            rvas.append(rva)
            names[rva] = r[1].strip() if len(r) > 1 else "?"
    rvas.sort()
    return rvas, names


def load_hooks_names():
    """Map RVA(int) -> (name, confidence) from hooks.csv."""
    out = {}
    if not HOOKS.exists():
        return out
    with open(HOOKS, encoding="utf-8-sig") as f:
        rdr = csv.reader(f)
        next(rdr)
        for r in rdr:
            if len(r) < 4:
                continue
            try:
                rva = int(r[0].strip().lstrip("0x"), 16)
            except ValueError:
                continue
            out[rva] = (r[1].strip(), r[3].strip())
    return out


def find_next_calls(data, text, image_base: int, after_va: int,
                     max_distance: int = 64, max_calls: int = 2):
    """After a push-imm32 at `after_va` (the imm32 byte position), scan
    forward up to max_distance bytes for `call rel32` (opcode 0xe8) and
    return the absolute target addresses. Used for CALLING-pattern strings:
    `push "Calling X"; call print_fn; ...; call X` — the LAST call in this
    sequence is typically X."""
    text_base_va = image_base + text["va"]
    raw_base = text["raw_offset"]
    # find file-offset of after_va within text
    file_idx = raw_base + (after_va - text_base_va) + 4  # +4 to skip the imm32
    if file_idx >= raw_base + text["raw_size"]:
        return []
    targets: list[int] = []
    cursor = file_idx
    limit = min(file_idx + max_distance, raw_base + text["raw_size"])
    while cursor < limit and len(targets) < max_calls:
        if data[cursor] == 0xe8 and cursor + 5 <= limit:
            rel = struct.unpack("<i", data[cursor + 1:cursor + 5])[0]
            # absolute target = (instruction_addr + 5) + rel32
            instr_va = text_base_va + (cursor - raw_base)
            target = (instr_va + 5 + rel) & 0xFFFFFFFF
            targets.append(target)
            cursor += 5
        else:
            cursor += 1
    return targets


def find_text_refs(data, text, image_base: int, target_va: int):
    """Return list of (absolute_va, opcode) within .text where target_va is
    referenced by a `push imm32` (0x68 ..) or `mov reg, imm32`
    (0xb8..0xbf ..). The test is a simple bytes-grep; false positives are
    rare for VAs in the rdata range because that exact 4-byte pattern is
    unlikely to occur as embedded-data."""
    needle = struct.pack("<I", target_va)
    base = text["raw_offset"]
    end = base + text["raw_size"]
    section = data[base:end]
    offsets = []
    start = 0
    while True:
        idx = section.find(needle, start)
        if idx == -1:
            break
        prev = section[idx - 1] if idx > 0 else 0
        # 0x68 = push imm32; 0xb8..0xbf = mov r32, imm32
        if prev == 0x68 or 0xb8 <= prev <= 0xbf:
            va = image_base + text["va"] + idx
            offsets.append((va, prev))
        start = idx + 1
    return offsets


def containing_function(rvas, va: int):
    """Return the largest RVA <= va, or None."""
    i = bisect.bisect_right(rvas, va) - 1
    if i < 0:
        return None
    return rvas[i]


def main() -> int:
    if not MASHED.exists():
        print(f"ERROR: {MASHED} not found")
        return 1
    if not INVENTORY.exists():
        print(f"ERROR: {INVENTORY} not found")
        return 1

    data = MASHED.read_bytes()
    image_base, sections = parse_pe(data)
    print(f"ImageBase: 0x{image_base:08x}")
    print(f"Sections:")
    for s in sections:
        print(f"  {s['name']:8s}  va=0x{s['va']:08x}  vsize=0x{s['vsize']:x}  raw=0x{s['raw_offset']:x}+0x{s['raw_size']:x}")

    rdata = find_section(sections, ".rdata")
    text = find_section(sections, ".text")
    if rdata is None or text is None:
        print("ERROR: .rdata or .text section missing")
        return 1

    # Pull RW-shaped strings from .rdata
    raw_strings = scan_rdata_strings(data, rdata)
    print(f"\n{len(raw_strings)} RW-related log strings in .rdata")
    if not raw_strings:
        return 0

    # Stats by pattern type
    from collections import Counter
    by_pattern = Counter(ev for _, _, ev, _ in raw_strings)
    for k, n in by_pattern.most_common():
        print(f"  {k}: {n}")

    # Load function entries and existing names
    fn_rvas, ghidra_names = load_function_entries()
    hooks_names = load_hooks_names()
    print(f"\n{len(fn_rvas)} function entries in inventory")
    print(f"{len(hooks_names)} hooks.csv rows")

    # For each RW string, find references in .text and group by containing function.
    # For CALLING-pattern strings, ALSO walk forward to the next CALL targets —
    # those addresses are the API being called (RwEngineInit etc.).
    by_fn: dict[int, list[tuple[str, str, str, int, int, int]]] = {}
    # Separate ledger for "function X is at RVA Y" inferences from CALLING patterns.
    callee_inferences: list[tuple[int, str, str, int]] = []  # (target_rva, rw_name, evidence, emitter_ref_va)
    refless_strings = 0
    for raw_s, rw_name, evidence, rdata_off in raw_strings:
        s_va = image_base + rdata["va"] + rdata_off
        s_repr = raw_s.decode("ascii", errors="replace").rstrip("\r\n\t \x00")
        refs = find_text_refs(data, text, image_base, s_va)
        if not refs:
            refless_strings += 1
            continue
        for ref_va, opcode in refs:
            owner = containing_function(fn_rvas, ref_va)
            if owner is None:
                continue
            by_fn.setdefault(owner, []).append(
                (s_repr, rw_name, evidence, s_va, ref_va, opcode)
            )
            if evidence == "CALLING":
                # The next call within ~64 bytes is typically the API target.
                # If there are 2 calls (printf-style log + API call), the
                # second is the API. Emit BOTH as candidates; reviewer picks.
                next_calls = find_next_calls(data, text, image_base, ref_va)
                for target in next_calls:
                    if 0x00401000 <= target < 0x005ca000:
                        callee_inferences.append((target, rw_name, "CALLING-callsite", ref_va))

    print(f"\n{len(by_fn)} distinct functions reference RW log strings")
    print(f"{refless_strings} RW strings have no detectable .text reference (dead?)")
    print(f"{len(callee_inferences)} CALLING-callsite inferences from forward-call scan")

    OUTPUT.parent.mkdir(parents=True, exist_ok=True)
    with open(OUTPUT, "w", encoding="utf-8", newline="") as f:
        w = csv.writer(f, delimiter="\t", lineterminator="\n")
        w.writerow([
            "function_rva", "current_name", "current_confidence",
            "suggested_name", "evidence_pattern", "raw_string",
            "string_va", "ref_count", "ref_offsets",
        ])
        for fn_rva in sorted(by_fn.keys()):
            entries = by_fn[fn_rva]
            # Group by suggested name (a function may emit many strings)
            per_name: dict[str, list[tuple[str, str, int, int]]] = {}
            for s_repr, rw_name, evidence, s_va, ref_va, opcode in entries:
                per_name.setdefault(rw_name, []).append(
                    (s_repr, evidence, s_va, ref_va)
                )
            for rw_name, hits in sorted(per_name.items()):
                # Pick the strongest evidence pattern among hits for this name
                evidence_priority = {"STATUS": 0, "PREFIX": 1, "BARE": 2, "CALLING": 3}
                hits_sorted = sorted(hits, key=lambda h: evidence_priority.get(h[1], 9))
                s_repr, evidence, s_va, _ = hits_sorted[0]
                ref_offsets = ",".join(f"0x{h[3]:08x}" for h in hits)
                current_name = ghidra_names.get(fn_rva, "?")
                if fn_rva in hooks_names:
                    current_name = hooks_names[fn_rva][0]
                    confidence = hooks_names[fn_rva][1]
                else:
                    confidence = "(unmapped)"
                w.writerow([
                    f"0x{fn_rva:08x}", current_name, confidence,
                    rw_name, evidence, s_repr,
                    f"0x{s_va:08x}", len(hits), ref_offsets,
                ])

    # Append callee-inference rows (functions named via "Calling X" callsite walks).
    with open(OUTPUT, "a", encoding="utf-8", newline="") as f:
        w = csv.writer(f, delimiter="\t", lineterminator="\n")
        # Deduplicate by target_rva: a single API may be called from multiple
        # log sites. Keep the strongest evidence (any STATUS> CALLING-callsite).
        by_target: dict[int, list[tuple[str, str, int]]] = {}
        for target_rva, rw_name, evidence, emitter_ref_va in callee_inferences:
            by_target.setdefault(target_rva, []).append(
                (rw_name, evidence, emitter_ref_va)
            )
        for target_rva in sorted(by_target.keys()):
            hits = by_target[target_rva]
            names = sorted({h[0] for h in hits})
            # The function we name MUST be in the inventory as an entry.
            # Find the nearest entry RVA <= target_rva — typically it IS target_rva.
            owner = containing_function(fn_rvas, target_rva)
            if owner is None or owner != target_rva:
                # Not at a function entry — likely a label or mid-function jump.
                # Skip; the caller of "Calling X" landed in the middle of X.
                continue
            current_name = ghidra_names.get(owner, "?")
            if owner in hooks_names:
                current_name = hooks_names[owner][0]
                confidence = hooks_names[owner][1]
            else:
                confidence = "(unmapped)"
            ref_offsets = ",".join(f"0x{h[2]:08x}" for h in hits)
            for rw_name in names:
                w.writerow([
                    f"0x{owner:08x}", current_name, confidence,
                    rw_name, "CALLING-callsite",
                    "(inferred from 'Calling X' string callsite)",
                    "(no string)", len(hits), ref_offsets,
                ])

    # Highest-confidence proposals: STATUS pattern (function reports its
    # own name), single rw_name per function.
    high_conf_fns = set()
    for fn_rva, entries in by_fn.items():
        names = {e[1] for e in entries}
        if len(names) == 1:
            ev = entries[0][2]
            if ev in ("STATUS", "PREFIX"):
                high_conf_fns.add(fn_rva)

    print(f"\nSummary:")
    print(f"  high-confidence rename candidates (STATUS/PREFIX + single name): {len(high_conf_fns)}")
    print(f"  multi-name functions or CALLING-only: {len(by_fn) - len(high_conf_fns)}")
    print(f"\nWrote: {OUTPUT}")
    return 0


if __name__ == "__main__":
    import sys
    sys.exit(main())
