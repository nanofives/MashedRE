#!/usr/bin/env py -3.12
"""promote_frontier.py — self-refreshing "promotable frontier" for /promote-round.

WHY THIS EXISTS
---------------
The /promote-round loop wasted ~15 tool-calls per promotion, most of it on
re-deriving facts that can be precomputed once (see
memory/feedback-promote-round-efficiency.md). The dominant waste was decompiling
functions that turn out to be already-C3/C4, zero-caller (gate-fail), or non-leaf.
This tool emits the set of functions that are actually promotable RIGHT NOW so a
round can go straight to author+diff.

DEFINITION (the memo's frontier, computed self-contained from the binary):
  frontier = status==C2 in hooks.csv
           ∩ FIRST-PARTY (subsystem has no 'third-party' tag)
           ∩ ZERO-CALLEE  (no direct E8 call, no indirect call, no tail-jmp to
                            another function — a true leaf)
           ∩ body < ~MAX_BODY bytes
           ∩ has >=1 caller that is C2+ in hooks.csv  (the C3 caller gate)

DATA SOURCE: original/MASHED.exe.unpatched (the SHA-anchored original) + hooks.csv.
NO Ghidra dependency — capstone disassembles .text directly, which also correctly
handles the "early-return-then-continue" false positive that bit the byte-scan
heuristic (it tracks the max forward branch target so a mid-body `ret` on an
early-return path is NOT mistaken for the function end). The memo warns the narrow
opcode-pattern scan gave a false "drained at 129"; this graph-level finder is the
fix.

OUTPUT: re/analysis/plans/promote_frontier.tsv  (re-run after every round).
  columns: rva  name  subsystem  size  n_callers_c2plus  best_caller  shape_hint

Run:  py -3.12 scripts/promote_frontier.py
"""
import csv
import os
import sys
from collections import defaultdict

import pefile
import capstone

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
EXE = os.path.join(ROOT, "original", "MASHED.exe.unpatched")
HOOKS = os.path.join(ROOT, "hooks.csv")
OUT = os.path.join(ROOT, "re", "analysis", "plans", "promote_frontier.tsv")

MAX_BODY = 400            # frontier body-size ceiling (bytes); raised 260->400 (2026-06-15) to surface medium clean leaves in the tail phase
MIN_BODY = 5              # an inline-JMP hook is E9+rel32 = 5 bytes; a body < 5
#                          bytes gets its 5-byte patch clobbered PAST the function
#                          boundary into the next function -> install-time crasher
#                          even when the bit-identity diff is GREEN (e.g. the 3-byte
#                          0x005c9d00 GetRaceEndTrigger, demoted C3->C2 2026-05-22).
PROMOTABLE_CONF = {"C2", "C3", "C4"}   # a caller at this level satisfies the gate

RET_OPS = {0xC3, 0xC2, 0xCB, 0xCA}     # ret / retn imm16 / retf / retf imm16


def load_hooks():
    """rva_int -> dict(row). Normalizes the ~21 file-offset util rows (<0x400000)."""
    rows = {}
    with open(HOOKS, newline="", encoding="utf-8", errors="replace") as f:
        for r in csv.DictReader(f):
            rva = (r.get("rva") or "").strip()
            if not rva or rva.startswith("#"):
                continue
            try:
                v = int(rva, 16)
            except ValueError:
                continue
            if v < 0x400000:           # file-offset convention -> real VA
                v += 0x400000
            r["_va"] = v
            rows[v] = r
    return rows


# Statically-linked third-party library bands (FidDB-attested; never first-party
# port work — reproducing them inflates the count with library internals). RVAs in
# these ranges are excluded from the frontier even when the hooks.csv subsystem
# tag is generic (e.g. CRT __cfltcvt table init carries subsystem "boot").
#   CRT band        0x004a0000..0x004b3fff  (lever3 calibration; MSVC CRT)
#   D3DX9 PSGP band 0x004ec000..0x004fc9e0  (Microsoft PSGP SSE/SSE2 dispatch)
#   qhull / RW-Phys 0x0057c5b0..0x005a5820  (qhull-2002.1 via RenderWare Physics 3.7)
#   libpng+zlib     0x00516000..0x00529fff  (statically-linked PNG/zlib: png_*, z*; user
#                                            ruling 2026-06-15 = library-skip, do NOT promote)
#   MSVC CRT rt     0x005c0000..0x005c3fff  (CRT runtime: __ftol/__setjmp3/wcs*/SEH/__ctrandisp2;
#                                            flagged round 156, added 2026-06-15 per library-skip.
#                                            NARROWED 2026-07-27 from 0x005c8000 — see U-9023.)
LIBRARY_BANDS = (
    (0x004a0000, 0x004b3fff),
    (0x004ec000, 0x004fc9e0),
    (0x0057c5b0, 0x005a5820),
    (0x00516000, 0x00529fff),
    (0x005c0000, 0x005c3fff),
)

# U-9023 (2026-07-27) — the 0x005c0000..0x005c8000 band was OVER-BROAD and had been
# silently excluding first-party rows from the frontier. Evidence:
#   * its own source batch note says only 56 of 80 rows are CRT library-residue;
#   * scripts/bulk_add_library_residue.py:64 uses a DIFFERENT upper bound (0x5d0000)
#     for the same named band — the two disagreed with each other;
#   * 0x005c4d30 CondGet5c4d30 was already C3 with a clean Frida diff INSIDE the band
#     (promoted 2026-06-13, two days BEFORE the band was added on 2026-06-15), as were
#     0x005c7500 AudioMixerRateCompute and 0x005c75b0 AudioVoiceField8cGet;
#   * re/analysis/bucket_005c1d63/0x005c47e0.md records kind: GAME / library_match:
#     (none — game code) for FUN_005c47e0, which calls first-party render leaves.
#
# A NARROWER BAND ALONE CANNOT FIX IT: CRT and non-CRT interleave through the range
# (28 alternations in address order — e.g. FUN_005c1da2 sits between _fseek and __ftol),
# so no contiguous bound separates them. The band is therefore paired with a name test.
#
# Why keep any band at all: the original rationale still holds — Ghidra's FID under-tags
# CRT here, so unnamed CRT helpers carry subsystem "boot" and neither the tag nor the name
# test would catch them. Below 0x005c4000 those unnamed helpers are dense and interleaved
# with named CRT, so the band stays. From 0x005c4000 up, the CRT residue is FID-named
# (_gmtime 0x5c4440, ___mbtowc_mt 0x5c4547, _mbtowc 0x5c4607) and thus caught by name,
# while the unnamed rows there are the game dynarray/chunk readers and the audio cluster.
#
# Net effect: 37 rows released to the frontier (12 audio + 25 boot), 92 still excluded.
# NOTE this is a CANDIDATE-GENERATION filter only — every released row still faces the
# callee-gate and the full C3 evidence gate. So a false ADMIT costs a little screening
# time, while a false EXCLUDE silently removes coverage forever. That asymmetry is why
# this leans permissive.
_CRT_NAME_PREFIXES = ("_", "FID_conflict:")
_CRT_NAME_EXACT = ("shortsort", "strtoxl")
# RenderWare internals ALSO take a leading underscore (_rwDeviceSystemFn, _rp*, _rt*).
# RW is the engine being ported (D2 Option B = RW-subset verbatim), so it is FIRST-PARTY
# work and must NOT be swept up by the CRT name test. Caught in review: without this
# exemption the change would have silently dropped 0x004c7a70 _rwDeviceSystemFn (render)
# out of the frontier — a regression in the opposite direction from the one being fixed.
_RW_NAME_PREFIXES = ("_rw", "_Rw", "_rp", "_Rp", "_rt", "_Rt")


def is_library_name(name):
    """True when the symbol is FID-attested CRT residue, regardless of address band."""
    n = (name or "").strip()
    if not n:
        return False
    if n.startswith(_RW_NAME_PREFIXES):
        return False
    return n.startswith(_CRT_NAME_PREFIXES) or n in _CRT_NAME_EXACT


def _rva_of(row):
    try:
        return int(str(row.get("rva", "")).strip().replace("0x", ""), 16)
    except ValueError:
        return -1


def is_first_party(row):
    if "third-party" in (row.get("subsystem") or ""):
        return False
    if is_library_name(row.get("name")):
        return False
    rva = _rva_of(row)
    return not any(lo <= rva <= hi for lo, hi in LIBRARY_BANDS)


def shape_hint(code, off):
    """Coarse opcode-shape tag for the first bytes — bridges into the classifier."""
    b = code[off:off + 8]
    if not b:
        return "?"
    if b[0] == 0xA1:
        return "read_global_u32"          # mov eax, [imm32]; ret
    if b[0] == 0xD9 and len(b) > 1 and b[1] == 0x05:
        return "read_global_f32"          # fld dword [imm32]; ret
    if b[0] == 0xC7 and len(b) > 1 and b[1] == 0x05:
        return "const_setter"             # mov dword [imm32], imm32; ret
    if b[0] == 0xB8:
        return "const_return"             # mov eax, imm32; ret
    if b[0] in (0x33, 0x31) and len(b) > 1 and b[1] in (0xC0, 0xC9):
        return "xor_clear"                # xor eax,eax / xor ecx,ecx ...
    if b[:4] == bytes((0x8B, 0x44, 0x24, 0x04)):
        # mov eax,[esp+4]; ...  -> single-arg getter family
        nb = code[off + 4:off + 8]
        if nb[:3] == bytes((0x8B, 0x04, 0x85)):
            return "abs_table_get_idx4"   # mov eax,[eax*4+tbl]
        if nb[:2] == bytes((0x8B, 0x40)):
            return "field_get_cdecl"      # mov eax,[eax+off]
        return "arg_getter"
    if b[0] == 0x8B and len(b) > 1 and b[1] == 0x41:
        return "field_get_thiscall"       # mov eax,[ecx+off]
    if b[:5] == bytes((0x8B, 0x44, 0x24, 0x04)) and False:
        return "?"
    return "other"


def analyze():
    pe = pefile.PE(EXE, fast_load=True)
    base = pe.OPTIONAL_HEADER.ImageBase
    text = next(s for s in pe.sections if s.Name.rstrip(b"\x00") == b".text")
    text_va = base + text.VirtualAddress
    text_end = text_va + text.Misc_VirtualSize
    code = text.get_data()  # raw bytes of .text

    def va_to_off(va):
        return va - text_va

    hooks = load_hooks()
    # Function-start set = every hooks.csv row whose VA lands in .text (any
    # confidence — we need complete boundaries for the call graph), plus we
    # treat them as the universe of known function entries.
    starts = sorted(v for v in hooks if text_va <= v < text_end)
    start_set = set(starts)

    md = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_32)
    md.detail = False  # we only need mnemonic + operand string + bytes

    size_of = {}
    has_call = {}
    direct_callees = defaultdict(set)
    callers = defaultdict(set)   # callee_va -> {caller_va}

    for i, start in enumerate(starts):
        nxt = starts[i + 1] if i + 1 < len(starts) else text_end
        region_end = min(nxt, text_end)
        off = va_to_off(start)
        end_off = va_to_off(region_end)
        maxbranch = start          # furthest forward intra-fn branch target seen
        fcall = False
        fsize = region_end - start  # default if no clean ret found
        addr = start
        try:
            for insn in md.disasm(code[off:end_off], start):
                addr = insn.address
                nextaddr = insn.address + insn.size
                mn = insn.mnemonic
                op = insn.op_str
                if mn == "call":
                    fcall = True
                    if op.startswith("0x"):
                        try:
                            tgt = int(op, 16)
                            if tgt in start_set:
                                direct_callees[start].add(tgt)
                                callers[tgt].add(start)
                        except ValueError:
                            pass
                    # indirect call (reg/mem) just sets fcall
                elif mn == "jmp":
                    if op.startswith("0x"):
                        try:
                            tgt = int(op, 16)
                        except ValueError:
                            tgt = None
                        if tgt is not None:
                            if start <= tgt < region_end:
                                if tgt > maxbranch:
                                    maxbranch = tgt
                                # intra-fn jump: keep going
                            elif tgt in start_set:
                                # tail-call to another function == has a callee
                                fcall = True
                                direct_callees[start].add(tgt)
                                callers[tgt].add(start)
                                fsize = nextaddr - start
                                break
                            else:
                                # jump out of region to non-function (jump table
                                # / switch thunk) — stop, treat as non-leaf-safe
                                fcall = True
                                fsize = nextaddr - start
                                break
                    else:
                        # indirect jmp (jump table dispatch) -> not a clean leaf
                        fcall = True
                        fsize = nextaddr - start
                        break
                elif mn.startswith("j"):  # conditional branch
                    if op.startswith("0x"):
                        try:
                            tgt = int(op, 16)
                            if start <= tgt < region_end and tgt > maxbranch:
                                maxbranch = tgt
                        except ValueError:
                            pass
                elif insn.bytes and insn.bytes[0] in RET_OPS:
                    if nextaddr > maxbranch:
                        fsize = nextaddr - start
                        break
                    # else: early-return path; body continues past maxbranch
            else:
                # ran off region_end without a terminating ret
                fsize = region_end - start
        except capstone.CsError:
            fsize = region_end - start
        size_of[start] = fsize
        has_call[start] = fcall

    return dict(base=base, code=code, text_va=text_va, hooks=hooks,
                size_of=size_of, has_call=has_call, callers=callers,
                direct_callees=direct_callees, start_set=start_set)


def main():
    a = analyze()
    hooks, size_of, has_call, callers = (a["hooks"], a["size_of"],
                                         a["has_call"], a["callers"])
    code, text_va = a["code"], a["text_va"]

    frontier = []
    for va, row in hooks.items():
        if (row.get("confidence") or "").strip() != "C2":
            continue
        if not is_first_party(row):
            continue
        if va not in size_of:           # not in .text
            continue
        if has_call.get(va, True):      # must be a true leaf
            continue
        sz = size_of.get(va, 9999)
        if sz > MAX_BODY or sz < MIN_BODY:   # too big, or too small for a 5-byte E9
            continue
        notes = (row.get("notes") or "")
        if "DEMOTED" in notes and "crash" in notes.lower():   # known install-time crasher
            continue
        # caller gate: >=1 direct caller at C2+
        c2_callers = [c for c in callers.get(va, ())
                      if (hooks.get(c, {}).get("confidence") or "").strip() in PROMOTABLE_CONF]
        if not c2_callers:
            continue
        best = sorted(c2_callers)[0]
        frontier.append((va, row, size_of[va], len(c2_callers), best))

    frontier.sort(key=lambda x: x[0])
    os.makedirs(os.path.dirname(OUT), exist_ok=True)
    with open(OUT, "w", encoding="utf-8") as f:
        f.write("rva\tname\tsubsystem\tsize\tn_callers_c2plus\tbest_caller\tshape_hint\n")
        for va, row, sz, ncall, best in frontier:
            hint = shape_hint(code, va - text_va)
            f.write(f"{va:08x}\t{row.get('name','')}\t{row.get('subsystem','')}\t"
                    f"{sz}\t{ncall}\t{best:08x}\t{hint}\n")

    # summary
    from collections import Counter
    by_sub = Counter(row.get("subsystem", "?") for _, row, _, _, _ in frontier)
    by_hint = Counter(shape_hint(code, va - text_va) for va, _, _, _, _ in frontier)
    print(f"wrote {OUT}")
    print(f"FRONTIER (C2 & first-party & leaf & <{MAX_BODY}B & C2+ caller): "
          f"{len(frontier)} functions")
    print("\nby subsystem:")
    for s, n in by_sub.most_common():
        print(f"  {s:16s} {n}")
    print("\nby shape_hint:")
    for s, n in by_hint.most_common():
        print(f"  {s:20s} {n}")


if __name__ == "__main__":
    sys.exit(main())
