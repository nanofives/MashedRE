"""Tracker lint for the merge train (HB-TRAIN-HOOKSCSV-TEXTONLY).

hooks.csv is text-only in merge_train.json (no build input and no gate scorer reads it), so a
re-classify row that only touches trackers skips the build and the gates. This lint is what still
catches a malformed tracker row on such a pass. merge_train.ps1 runs it on every batch.

Checks, each producing a finding key that does not depend on line numbers:
  hooks.csv          DECODE  not UTF-8
                     PARSE   csv.Error under strict parsing (e.g. an unterminated quote)
                     HEADER  header row differs from the base's header
                     NCOL    data row with a column count other than the header's
                     RVA     rva cell not 8 lowercase hex digits
                     CONF    confidence cell not one of C0..C4
                     DUP     the same rva on two data rows
  UNCERTAINTIES.md   UDUP    the same U-id on two table rows (lines starting with "| U-")

Rows whose first cell starts with "#" are the comment block at the top of hooks.csv.
Findings already present in --base are pre-existing (hooks.csv carries 4 NCOL rows as of
78be531e1) and do not fail the lint. Only NEW findings fail it.

usage: py -3.12 tracker_lint.py --repo <tree> [--base <git-ref>]
exit: 0 no NEW finding, 1 NEW finding(s), 2 usage / missing file.
"""
from __future__ import annotations

import argparse
import collections
import csv
import io
import pathlib
import re
import subprocess
import sys

CONF = {"C0", "C1", "C2", "C3", "C4"}
RVA_RE = re.compile(r"[0-9a-f]{8}")
UID_RE = re.compile(r"^\| (U-\d+) \|")


def lint_hooks(raw: bytes | None, base_header: list[str] | None) -> tuple[set[str], int, list[str] | None]:
    if raw is None:
        return set(), 0, None
    out: set[str] = set()
    try:
        text = raw.decode("utf-8")
    except UnicodeDecodeError as e:
        return {f"DECODE hooks.csv byte {e.start}"}, 0, None
    try:
        rows = list(csv.reader(io.StringIO(text, newline=""), strict=True))
    except csv.Error as e:
        return {f"PARSE hooks.csv: {e}"}, 0, None
    if not rows:
        return {"HEADER hooks.csv is empty"}, 0, None
    header = rows[0]
    if base_header is not None and header != base_header:
        out.add(f"HEADER {','.join(header)}")
    seen: collections.Counter[str] = collections.Counter()
    n = 0
    for r in rows[1:]:
        if not r or r[0].startswith("#"):
            continue
        n += 1
        rva = r[0]
        if len(r) != len(header):
            out.add(f"NCOL rva={rva} n={len(r)}")
        if not RVA_RE.fullmatch(rva):
            out.add(f"RVA {rva!r}")
        conf = r[3] if len(r) > 3 else ""
        if conf not in CONF:
            out.add(f"CONF rva={rva} confidence={conf!r}")
        seen[rva.lower()] += 1
    out |= {f"DUP rva={k} x{v}" for k, v in seen.items() if v > 1}
    return out, n, header


def lint_uncertainties(raw: bytes | None) -> tuple[set[str], int]:
    if raw is None:
        return set(), 0
    text = raw.decode("utf-8", errors="replace")
    ids = collections.Counter(m.group(1) for ln in text.splitlines() if (m := UID_RE.match(ln)))
    return {f"UDUP {k} x{v}" for k, v in ids.items() if v > 1}, len(ids)


def base_blob(repo: pathlib.Path, ref: str, path: str) -> bytes | None:
    r = subprocess.run(["git", "-C", str(repo), "show", f"{ref}:{path}"], capture_output=True)
    return r.stdout if r.returncode == 0 else None


def main() -> int:
    ap = argparse.ArgumentParser()
    ap.add_argument("--repo", required=True)
    ap.add_argument("--base", default=None)
    a = ap.parse_args()
    repo = pathlib.Path(a.repo)
    hp, up = repo / "hooks.csv", repo / "UNCERTAINTIES.md"
    if not hp.is_file():
        print(f"[tracker-lint] FATAL -- no hooks.csv under {repo}")
        return 2
    b_hooks = base_blob(repo, a.base, "hooks.csv") if a.base else None
    b_unc = base_blob(repo, a.base, "UNCERTAINTIES.md") if a.base else None
    bh_find, _, bh_header = lint_hooks(b_hooks, None)
    bu_find, _ = lint_uncertainties(b_unc)
    h_find, h_rows, _ = lint_hooks(hp.read_bytes(), bh_header)
    u_find, u_ids = lint_uncertainties(up.read_bytes() if up.is_file() else None)
    base_all, head_all = bh_find | bu_find, h_find | u_find
    new = sorted(head_all - base_all)
    pre = len(head_all & base_all)
    for f in new:
        print(f"[tracker-lint] NEW {f}")
    tail = (f"hooks.csv {h_rows} rows, UNCERTAINTIES.md {u_ids} ids, pre-existing={pre} NEW={len(new)}"
            f" (base {a.base or 'none'})")
    if new:
        print(f"[tracker-lint] FAIL -- {tail}")
        return 1
    print(f"[tracker-lint] OK -- {tail}")
    return 0


if __name__ == "__main__":
    sys.exit(main())
