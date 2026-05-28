#!/usr/bin/env python3
"""
c3_filter_v4: fixes 4 over-permissiveness bugs surfaced by the c3_batch_j
post-mortem (25% real promotion yield vs 70% filter-pass rate, 2026-05-18).

Carries the v3 filter set (commit 803a7ce, file c3_batch_j_filter.py):
  - inline `[UNCERTAIN]` regex matching both bare and U-id forms
  - catalogued-U-ID exemption
  - section-scoped callee extraction
  - "Resolves [UNCERTAIN]" prose skip
  - stale-impl drift skip
  - arg_type-present compat check (kept as fallback)

v4 ADDITIONS / REPLACEMENTS
---------------------------

(a) Signature-vs-harness mapping check.
    v3 only verified that any `arg_type:` frontmatter field, if present,
    appeared in diff_template.js' supported set. Notes without explicit
    frontmatter passed silently. v4 also scans the note body for explicit
    signature phrases that v3 cannot diff today (__fastcall with ECX/EDX
    args, EAX-implicit `this`/arg, ESI-implicit out-ptr, FPU/ST0 input,
    5+ arg shapes, paired-global setup+observe). These are the c3_batch_j
    s3+s4 refusal patterns; rejecting at filter time prevents zero-yield
    worker sessions.

(b) Live-state side-effect detector.
    Synthetic Frida calls into the live process. A candidate that opens
    files (fopen/fwrite/fclose), closes handles (CloseHandle), deletes
    files (DeleteFile/_unlink), pops modal Win32 dialogs
    (DialogBoxParam(A)), or terminates the process (ExitProcess) cannot
    be diffed safely — calling it once corrupts game state. v4 rejects
    these at filter time. c3_batch_j s6 lost 4 candidates this way.

(c) Tighter callee-gate.
    v3's RVA regex `\b(?:0x)?([0-9a-fA-F]{8})\b` does NOT match RVAs
    embedded in `FUN_004625b0` because `_` and `0` are both word chars,
    so the leading \b never fires. The 0x004669b0 note (`## Mechanical
    description` lists 4 callees as `FUN_xxxxxxxx`) had ZERO callees
    extracted, trivially passing the callee-gate. v4's regex matches
    FUN_/DAT_/LAB_/loc_/sub_ prefixes AND 0xHHHHHHHH. We also filter to
    the Mashed text-section range [0x00400000, 0x00800000) to drop data
    constants like `0x3f800000` that the new regex would otherwise hit.

(d) `Blocks: <C-level>` column honoring.
    UNCERTAINTIES.md has a 7-column table; column 7 is `Blocks`.
    Possible values seen: `none`, `C3`, `C4`. v3 blanket-allowed any
    catalogued U-ID. U-0125 is catalogued but `Blocks: C3`; c3-batch-i-s1
    refused 0x005aea00 for this reason at worker-session time — v4
    enforces it at filter time. We parse the table and, for each
    `[UNCERTAIN U-xxxx]` marker in the candidate body, look up the
    row whose `Where` cell mentions the candidate's RVA; if that row's
    `Blocks` ≥ C3, refuse. If no row's `Where` mentions the candidate
    RVA, fall back to the row with the strongest `Blocks` (conservative).
"""

from __future__ import annotations

import argparse
import csv
import re
import sys
from pathlib import Path

ROOT = Path(r"C:\Users\maria\Desktop\Proyectos\Mashed")
HOOKS = ROOT / "hooks.csv"
UNCERT = ROOT / "UNCERTAINTIES.md"
DIFF_TEMPLATE = ROOT / "re" / "frida" / "diff_template.js"

# Mashed .text section RVA window. MASHED.exe ImageBase=0x00400000, code lives
# up to ~0x005FXXXX; we use 0x00800000 as a generous upper bound so anything
# above is treated as data (e.g., the 0x3f800000 IEEE-754 1.0f constant which
# would otherwise pass as a callee RVA).
TEXT_LO = 0x00400000
TEXT_HI = 0x00800000

# Filter 1: inline UNCERTAIN. Matches `[UNCERTAIN]` and `[UNCERTAIN U-xxxx]`.
UNCERTAIN_RE = re.compile(r"\[UNCERTAIN[ \]]")

# Filter 4: explicit Resolves-prose skip.
RESOLVES_PROSE_RE = re.compile(r"Resolve[ds]?\s+\[UNCERTAIN", re.IGNORECASE)

# Callee-extraction is restricted to these section headings.
ALLOWED_CALLEE_SECTIONS = {
    "## decompilation",
    "## callees",
    "## mechanical description",
    "## mechanical",
    # added in v4 — `## Analysis` / `## Decomp` / `## Body` show up in older
    # plate-format notes (e.g., re/analysis/audio_sfx_dispatch/004669b0.md).
    "## analysis",
    "## decomp",
    "## body",
}

# v4 RVA extractor. Captures three forms:
#   FUN_xxxxxxxx / DAT_xxxxxxxx / LAB_xxxxxxxx / loc_xxxxxxxx / sub_xxxxxxxx
#   0xHHHHHHHH  (with non-word boundary BEFORE the 0)
#   bare HHHHHHHH (8 hex chars, word-boundary on both sides)
# Each alternative captures the 8-hex tail into group(1)/(2)/(3); collapse via
# a helper.
RVA_RE = re.compile(
    r"(?:(?:FUN|DAT|LAB|loc|sub)_([0-9a-fA-F]{8}))"
    r"|(?:(?<![\w])0x([0-9a-fA-F]{8}))"
    r"|(?:\b([0-9a-fA-F]{8})\b)"
)

# (a) Signature shapes the harness can't diff today.
# Patterns chosen from the c3_batch_j s3+s4 refusal evidence (commits ac546be,
# 3ac3610, 677aac5):
#   * __fastcall  -> ECX/EDX in args (we currently call via __cdecl exports)
#   * EAX-implicit -> caller passes 'this' or arg in EAX; harness has only
#                     1 stub today, and it's hot-path-specific
#   * ESI-implicit -> caller writes via ESI; v3 has no shim
#   * FPU / ST0 input -> caller seeds ST0 with a float before the call
#   * 5+ arg shapes -> diff_template.js maxes out at 4-arg today
SIG_UNSUPPORTED_PATTERNS = [
    (r"(?i)\b__fastcall\b",                                 "fastcall ECX/EDX args"),
    (r"(?i)\bEAX[- ]?implicit\b",                           "EAX-implicit arg"),
    (r"(?i)\(implicit\s+['`]?this['`]?\)",                  "implicit-this arg"),
    # NOTE: bare `EAX = X` / `ESI = X` patterns were tried but false-flag the
    # x86 return-value convention (e.g., `_strlen` note: `Return: EAX = ptr...`).
    # The explicit-phrase patterns below (__fastcall, EAX-implicit, in_EAX,
    # EAX+ESI) catch every c3_batch_j s3/s4 refusal without ambiguity.
    (r"(?i)\bin_EAX\b",                                     "in_EAX arg"),
    (r"(?i)\bin_ESI\b",                                     "in_ESI arg"),
    (r"(?i)\bin_ST0\b",                                     "in_ST0 (FPU) arg"),
    (r"(?i)\bST0\s+input\b",                                "ST0 input"),
    (r"(?i)\bimplicit\s+ST0\b",                             "implicit ST0"),
    (r"(?i)\bFPU\s+input\b",                                "FPU input"),
    (r"(?i)\bEAX\+ESI\b",                                   "EAX+ESI dual"),
    (r"(?i)\b(?:5|five)[- ]?arg(?:ument)?\b",               "5-arg shape"),
    (r"(?i)\b(?:5|five)\s+out[- ]?pointers?\b",             "5-out-ptr shape"),
    (r"(?i)paired[- ]global\s+(?:setup|observe)",           "paired-global setup/observe"),
]

# (b) Live-state side-effect mutators. If any matches in the note body,
# the candidate cannot be safely diffed via synthetic Frida calls. Patterns
# are taken from c3_batch_j s6 refusal evidence (commit e8d1439).
LIVE_STATE_PATTERNS = [
    (r"\bDialogBoxParam(?:A|W)?\b",       "modal Win32 dialog (DialogBoxParam)"),
    (r"\bDialogBox(?:A|W)?\s*\(",         "modal Win32 dialog (DialogBox)"),
    (r"\bMessageBox(?:A|W)?\b",           "blocking message box"),
    (r"\b_?fopen\b",                       "fopen — live file I/O"),
    (r"\b_?fwrite\b",                      "fwrite — live disk write"),
    (r"\b_?fclose\b",                      "fclose on live FILE*"),
    (r"\bCloseHandle\b",                   "CloseHandle on live handle"),
    (r"\b_?unlink\b",                      "_unlink on live path"),
    (r"\b_?remove\s*\(",                   "remove() on live path"),
    (r"\bDeleteFile(?:A|W)?\b",            "DeleteFile"),
    (r"\bMoveFile(?:A|W)?\b",              "MoveFile"),
    (r"\bSetCurrentDirectory(?:A|W)?\b",   "chdir mutates live cwd"),
    (r"\bExitProcess\b",                   "ExitProcess terminates host"),
    (r"\b_exit\s*\(",                      "_exit() terminates host"),
    (r"\bRegSetValue(?:Ex)?(?:A|W)?\b",    "registry write"),
    (r"\bRegDeleteKey(?:A|W)?\b",          "registry delete"),
    # COM vtable calls on live DirectX globals — calling these via synthetic
    # Frida corrupts the live device (DInput, D3D, DSound). c3_batch_j s6
    # 0x004960e0 leaked this way: `vtable[7] = Acquire`, `Unacquire(DAT_…)`.
    (r"\bIDirectInput(?:Device)?\d*\b",    "DirectInput vtable on live device"),
    (r"\bIDirect3D(?:Device)?\d*\b",       "Direct3D vtable on live device"),
    (r"\bIDirectSound(?:Buffer)?\d*\b",    "DirectSound vtable on live device"),
    (r"vtable\[\d+\]\s*=\s*(?:Acquire|Unacquire|Release)\b",
                                            "live COM Acquire/Unacquire/Release"),
]

ARG_TYPE_RE = re.compile(
    r"^(?:arg_type|signature_arg_type):\s*([a-z_0-9]+)\s*$", re.MULTILINE
)

CONF_PRI = {"C0": 0, "C1": 1, "C2": 2, "C3": 3, "C4": 4}


# ----------------------------------------------------------------------------
# loaders
# ----------------------------------------------------------------------------

def load_hooks() -> dict[str, dict]:
    rows: dict[str, dict] = {}
    with open(HOOKS, encoding="utf-8-sig") as f:
        reader = csv.reader(f)
        next(reader, None)
        for r in reader:
            if len(r) < 6:
                continue
            rva = r[0].strip().lower()
            rva = rva[2:] if rva.startswith("0x") else rva
            row = {
                "rva": rva,
                "name": r[1].strip(),
                "subsystem": r[2].strip(),
                "confidence": r[3].strip(),
                "status": r[4].strip(),
                "file": r[5].strip(),
            }
            existing = rows.get(rva)
            if existing is None:
                rows[rva] = row
            else:
                if CONF_PRI.get(row["confidence"], -1) > CONF_PRI.get(
                    existing["confidence"], -1
                ):
                    rows[rva] = row
    return rows


def parse_uncertainties_blocks() -> dict[str, list[tuple[str, str]]]:
    """
    Parse UNCERTAINTIES.md table rows. Returns:
        { 'U-0125': [(where_text_lowercase, blocks_value), ...], ... }

    The table format is `| ID | Type | Where | Statement | Evidence missing |
    Path to resolution | Blocks |`. Some rows (e.g., U-0648) appear to have
    fewer columns (5 instead of 7) — for those we conservatively assume
    `Blocks=none` because the column shift can't be safely auto-corrected.
    """
    out: dict[str, list[tuple[str, str]]] = {}
    if not UNCERT.exists():
        return out

    table_row_re = re.compile(r"^\|\s*(U-\d{3,5})\s*\|(.*)\|\s*$")
    for raw in UNCERT.read_text(encoding="utf-8", errors="replace").splitlines():
        m = table_row_re.match(raw)
        if not m:
            continue
        uid = m.group(1)
        cells = [c.strip() for c in m.group(2).split("|")]
        # Expected 6 trailing cells: Type, Where, Statement, Evidence,
        # Path, Blocks.
        if len(cells) >= 6:
            where = cells[1].lower()
            blocks = cells[-1]
        elif len(cells) == 5:
            # column-shift in some older rows (e.g., U-0648).
            where = cells[0].lower()
            blocks = "none"
        else:
            where = ""
            blocks = "none"
        out.setdefault(uid, []).append((where, blocks))
    return out


def load_supported_arg_types() -> set[str]:
    txt = DIFF_TEMPLATE.read_text(encoding="utf-8", errors="replace")
    return set(re.findall(r"arg_type === '([a-z_0-9]+)'", txt))


def read_note(path: Path) -> str:
    if not path.exists():
        return ""
    return path.read_text(encoding="utf-8", errors="replace")


def section_iter(note_body: str):
    current = "(preamble)"
    buf: list[str] = []
    for line in note_body.splitlines():
        if line.startswith("## "):
            yield current, "\n".join(buf)
            current = line.strip().lower()
            buf = []
        else:
            buf.append(line)
    yield current, "\n".join(buf)


# ----------------------------------------------------------------------------
# Filter 1+2+4: inline UNCERTAIN (now honoring Blocks)
# ----------------------------------------------------------------------------

def _blocks_at_or_above_c3(blocks_value: str) -> bool:
    v = blocks_value.strip().upper()
    if not v or v == "NONE":
        return False
    return v in {"C3", "C4"}


def _u_id_blocks_for(uid: str, candidate_rva: str,
                     blocks_map: dict) -> tuple[bool, str]:
    """
    Returns (blocks_c3, reason). If the U-ID is not in the catalog at all,
    returns (False, "uncatalogued") — that case is caught earlier as a
    refusal anyway.

    If we can match the candidate RVA against one of the U-ID's rows, use
    that row's Blocks. Otherwise fall back to the strongest Blocks across
    all rows for the U-ID (conservative — favors refusing).
    """
    rows = blocks_map.get(uid, [])
    if not rows:
        return False, "uncatalogued"
    rva_norm = candidate_rva.lower().lstrip("0x").zfill(8)
    rva_padded = "0x" + rva_norm
    matched = [
        (where, blk) for where, blk in rows
        if rva_norm in where or rva_padded in where
    ]
    pool = matched if matched else rows
    for where, blk in pool:
        if _blocks_at_or_above_c3(blk):
            scope = "matched-RVA" if matched else "fallback-strongest"
            return True, f"Blocks={blk} ({scope})"
    return False, "Blocks=none"


def inline_uncertain_blocks(body: str, blocks_map: dict,
                            candidate_rva: str) -> str | None:
    """
    v4 semantics:
      * Markers OUTSIDE `## Uncertainties` — bare `[UNCERTAIN]` (no U-ID) or
        catalogued U-IDs with `Blocks: none` are refused as inline-uncatalogued;
        only catalogued U-IDs with `Blocks: none` are exempted (= can stay).
      * Markers INSIDE `## Uncertainties` — normally legitimate (the catalog
        of filed holes). BUT a catalogued U-ID with `Blocks: C3+` is ALWAYS
        a refusal, regardless of section, because the rubric forbids C3
        promotion while such an uncertainty is open (see CONFIDENCE.md).
    """
    for heading, section in section_iter(body):
        in_uncertainties = heading.startswith("## uncertainties")
        # Scrub Resolves-prose lines first.
        scrubbed = "\n".join(
            line for line in section.splitlines()
            if not RESOLVES_PROSE_RE.search(line)
        )

        for m in UNCERTAIN_RE.finditer(scrubbed):
            tail = scrubbed[m.start() : m.start() + 32]
            uid_m = re.search(r"\[UNCERTAIN\s+(U-\d{3,5})\]", tail)
            if not uid_m:
                # bare [UNCERTAIN] — legitimate inside ## Uncertainties,
                # refused everywhere else.
                if in_uncertainties:
                    continue
                return f"inline bare [UNCERTAIN] (heading={heading})"
            uid = uid_m.group(1)

            # Blocks check runs in BOTH section locations — a C3 block is
            # a hard refusal even if the marker lives in the catalog section.
            if uid in blocks_map:
                blocks_c3, why = _u_id_blocks_for(uid, candidate_rva, blocks_map)
                if blocks_c3:
                    return (
                        f"inline [UNCERTAIN {uid}] catalogued but {why} "
                        f"(heading={heading})"
                    )

            # Below this point: Blocks!=C3 (or U-ID uncatalogued).
            if in_uncertainties:
                # Filed-and-non-blocking markers in the catalog section are OK.
                continue

            if uid not in blocks_map:
                return (
                    f"inline [UNCERTAIN {uid}] not in UNCERTAINTIES.md "
                    f"(heading={heading})"
                )
            # catalogued + Blocks!=C3 + outside ## Uncertainties -> exempt.
    return None


# ----------------------------------------------------------------------------
# Filter 3: section-scoped callee extraction (v4 regex)
# ----------------------------------------------------------------------------

def _rva_from_match(m: re.Match) -> str | None:
    """Collapse the 3 alternatives of RVA_RE; return lowercase 8-hex or None."""
    for g in m.groups():
        if g:
            return g.lower()
    return None


def extract_callees(body: str, self_rva: str) -> list[str]:
    rvas: set[str] = set()
    self_norm = self_rva.lower().lstrip("0x").zfill(8)
    for heading, section in section_iter(body):
        if heading not in ALLOWED_CALLEE_SECTIONS:
            continue
        scrubbed = "\n".join(
            line for line in section.splitlines()
            if not RESOLVES_PROSE_RE.search(line)
        )
        for m in RVA_RE.finditer(scrubbed):
            rva = _rva_from_match(m)
            if rva is None or rva == self_norm:
                continue
            # Filter to MASHED .text range — drops 0x3f800000-class data
            # constants that the v4 regex would otherwise pick up.
            try:
                v = int(rva, 16)
            except ValueError:
                continue
            if not (TEXT_LO <= v < TEXT_HI):
                continue
            rvas.add(rva)
    return sorted(rvas)


def callee_gate(callees, hooks) -> tuple[bool, list[str]]:
    blocking: list[str] = []
    for c in callees:
        row = hooks.get(c)
        if row is None:
            # Unmapped — assume leaf/CRT, give benefit of doubt. (v3 behavior
            # retained; v4's reach is via the RVA regex fix, not the gate
            # strictness.)
            continue
        conf = row["confidence"]
        if CONF_PRI.get(conf, -1) < 2:
            blocking.append(f"{c}={conf}")
    return (not blocking), blocking


# ----------------------------------------------------------------------------
# v4 new filters
# ----------------------------------------------------------------------------

# v4 (e): byte-faithful evidence markers. A note must contain at least one of
# these to be authorable for a bit-identical reimpl. Notes that are pure
# control-flow SUMMARIES (e.g. the hud_frontend_d2 cluster: `## Control flow`,
# `## Globals read`, no verbatim decomp) cannot be authored without violating
# NO-GUESSING — c3_batch_t-s4 proved 0/10 on exactly that cluster.
DECOMP_EVIDENCE_RE = re.compile(
    r"(?im)^\s*```c\b"                        # verbatim C decompilation fence
    r"|^\s*##\s*mechanical description\b"     # mechanical-transcript section
    r"|^\s*##\s*decompilation\b"              # decompilation section
    r"|^\s*##\s*body\s*\(mechanical"          # body (mechanical transcript)
    r"|mechanical\s+transcript"               # inline phrase
)


def shape_only_note(body: str) -> str | None:
    """v4 (e): reject control-flow-SUMMARY notes that lack any verbatim decomp
    transcript. Authoring a bit-identical reimpl from a summary is a
    NO-GUESSING violation. Byte-faithful notes carry a ```c block, a
    `## Mechanical description`, a `## Decompilation`, or a
    `## Body (mechanical transcript)` section."""
    if DECOMP_EVIDENCE_RE.search(body):
        return None
    return "shape-only note (no verbatim decomp / mechanical-transcript section)"


def signature_unsupported(body: str) -> str | None:
    """v4 bug (a): detect signature phrases the harness cannot diff today."""
    for pat, label in SIG_UNSUPPORTED_PATTERNS:
        if re.search(pat, body):
            return f"signature-unsupported: {label}"
    return None


def live_state_side_effects(body: str) -> str | None:
    """v4 bug (b): detect live-state mutators that synthetic Frida cannot
    safely call. Search only OUTSIDE the ## Uncertainties section (the
    catalog there describes mutations as questions, not as observations)."""
    for heading, section in section_iter(body):
        if heading.startswith("## uncertainties"):
            continue
        for pat, label in LIVE_STATE_PATTERNS:
            if re.search(pat, section):
                return f"live-state-side-effect: {label} (in {heading})"
    return None


# ----------------------------------------------------------------------------
# stale-impl drift filter (v3-identical)
# ----------------------------------------------------------------------------

def stale_impl_check(rva: str) -> str | None:
    src = ROOT / "mashedmod" / "src" / "mashed_re"
    log = ROOT / "log"
    short = rva.lstrip("0x").lower().zfill(8)

    impl_hits: list[str] = []
    if src.exists():
        for p in src.rglob("*.cpp"):
            try:
                txt = p.read_text(encoding="utf-8", errors="replace")
            except Exception:
                continue
            if short in txt.lower():
                impl_hits.append(str(p.relative_to(ROOT)))
                break

    diff_hits: list[str] = []
    if log.exists():
        for p in log.glob(f"diff_*{short}*.csv"):
            diff_hits.append(str(p.relative_to(ROOT)))

    if impl_hits and diff_hits:
        return f"DRIFT impl={impl_hits[0]} diff={diff_hits[0]}"
    return None


# ----------------------------------------------------------------------------
# arg_type frontmatter check (v3-identical, kept as fallback)
# ----------------------------------------------------------------------------

def arg_type_present(body: str, supported: set[str]) -> tuple[str, bool]:
    m = ARG_TYPE_RE.search(body)
    if not m:
        return "unknown", True
    name = m.group(1)
    return name, name in supported


# ----------------------------------------------------------------------------
# main
# ----------------------------------------------------------------------------

def main() -> int:
    ap = argparse.ArgumentParser(description="c3 promotion filter v4")
    ap.add_argument(
        "--subsystems",
        default="audio,save,vehicle,input,boot",
        help="comma-separated subsystem names",
    )
    ap.add_argument(
        "--out-prefix",
        default="c3_batch_k",
        help="output filename prefix (writes <prefix>_passed.tsv etc.)",
    )
    args = ap.parse_args()
    target_subs = {s.strip() for s in args.subsystems.split(",") if s.strip()}

    print(f"c3_filter_v4 — subsystems={sorted(target_subs)}")
    print("Loading inputs...")
    hooks = load_hooks()
    blocks_map = parse_uncertainties_blocks()
    supported = load_supported_arg_types()
    print(f"  hooks.csv rows:        {len(hooks)}")
    print(f"  UNCERTAINTIES rows:    {len(blocks_map)}")
    blocks_c3_count = sum(
        1 for u, rows in blocks_map.items()
        if any(_blocks_at_or_above_c3(b) for _, b in rows)
    )
    print(f"  U-IDs with Blocks>=C3: {blocks_c3_count}")
    print(f"  supported arg_types:   {len(supported)}")
    print()

    candidates: list[dict] = []
    for rva, row in hooks.items():
        if row["confidence"] != "C2":
            continue
        if row["subsystem"] not in target_subs:
            continue
        if not row["file"].endswith(".md"):
            continue
        candidates.append(row)

    print(f"=== Initial pull: {len(candidates)} C2 candidates ===")
    by_sub: dict[str, int] = {}
    for c in candidates:
        by_sub[c["subsystem"]] = by_sub.get(c["subsystem"], 0) + 1
    for sub, n in sorted(by_sub.items()):
        print(f"  {sub}: {n}")
    print()

    elim = {
        "no_note": 0,
        "stale_drift": 0,
        "inline_uncertain": 0,
        "shape_only_note": 0,
        "callee_gate": 0,
        "signature_unsupported": 0,
        "live_state_side_effect": 0,
        "arg_type_unsupported": 0,
    }
    passed: list[dict] = []
    rejected: list[tuple[dict, str]] = []

    for c in candidates:
        rva = c["rva"]
        body = read_note(ROOT / c["file"])
        if not body:
            elim["no_note"] += 1
            rejected.append((c, "no-note"))
            continue

        drift = stale_impl_check(rva)
        if drift:
            elim["stale_drift"] += 1
            rejected.append((c, drift))
            continue

        unc = inline_uncertain_blocks(body, blocks_map, rva)
        if unc:
            elim["inline_uncertain"] += 1
            rejected.append((c, unc))
            continue

        shape_bad = shape_only_note(body)
        if shape_bad:
            elim["shape_only_note"] += 1
            rejected.append((c, shape_bad))
            continue

        sig_bad = signature_unsupported(body)
        if sig_bad:
            elim["signature_unsupported"] += 1
            rejected.append((c, sig_bad))
            continue

        live_bad = live_state_side_effects(body)
        if live_bad:
            elim["live_state_side_effect"] += 1
            rejected.append((c, live_bad))
            continue

        callees = extract_callees(body, rva)
        passes, blocking = callee_gate(callees, hooks)
        if not passes:
            elim["callee_gate"] += 1
            rejected.append((c, f"callee-gate: {','.join(blocking)}"))
            continue

        atype, ok = arg_type_present(body, supported)
        if not ok:
            elim["arg_type_unsupported"] += 1
            rejected.append((c, f"arg_type={atype} not supported"))
            continue

        c["arg_type"] = atype
        c["callees"] = callees
        passed.append(c)

    print("=== Filter eliminations ===")
    for k, v in elim.items():
        print(f"  {k:30s} {v:3d}")
    print()
    print(f"=== Passed all filters: {len(passed)} ===")
    by_sub = {}
    for c in passed:
        by_sub[c["subsystem"]] = by_sub.get(c["subsystem"], 0) + 1
    for sub, n in sorted(by_sub.items()):
        print(f"  {sub}: {n}")

    out_dir = ROOT / "re" / "analysis" / "plans"
    out_passed = out_dir / f"{args.out_prefix}_passed.tsv"
    out_rejected = out_dir / f"{args.out_prefix}_rejected.tsv"
    with open(out_passed, "w", encoding="utf-8") as f:
        f.write("rva\tname\tsubsystem\targ_type\tnote\tcallees\n")
        for c in passed:
            f.write(
                "\t".join([
                    c["rva"], c["name"], c["subsystem"],
                    c.get("arg_type", ""), c["file"],
                    ",".join(c.get("callees", [])),
                ]) + "\n"
            )
    with open(out_rejected, "w", encoding="utf-8") as f:
        f.write("rva\tname\tsubsystem\treason\n")
        for c, reason in rejected:
            f.write("\t".join([c["rva"], c["name"], c["subsystem"], reason]) + "\n")

    print(f"\nWrote: {out_passed}")
    print(f"Wrote: {out_rejected}")
    return 0


if __name__ == "__main__":
    sys.exit(main())
