"""Row-level 3-way merge for hooks.csv, keyed by rva (HB-HOOKSCSV-ROWMERGE, 2026-11-10).

`git merge-file` merges hooks.csv line by line, so two branches that edit *adjacent* rows (or rows
inside one diff hunk) conflict although they touched different functions. PORT-OT-FRONTEND bounced
on that in the merge train on 2026-11-10. This merge treats every data row as one record keyed by its
`rva` cell and merges records, not lines.

Rules
- Header (first record): kept from ours. If one side's header is the other's plus trailing columns
  (a branch that predates `port`), the wider header wins and narrower rows get the missing cells
  from the version that has them (see "missing columns"). Any other header difference halts.
- Comment / blank records (`#...`, `"#...`, empty): grouped by the data row they follow (the group
  before the first data row is anchored to TOP). Each group is merged as a whole: ours == base takes
  theirs, theirs == base takes ours, ours == theirs takes either, else CONFLICT.
- Data rows, key = rva cell lowercased:
  * changed on one side only (edit or delete) -> that side.
  * changed on both sides identically -> clean.
  * changed on both sides differently -> CONFLICT, the differing field names are printed
    (`port` alone means the difference is only in the derived column).
  * deleted on one side and edited on the other -> CONFLICT.
  * added on one side -> inserted right after the nearest preceding data row of that side that is in
    the output, or at TOP when there is none. hooks.csv is not rva-sorted (556 inversions on 2026-11-10), so rva order is not
    used. Added on both sides: identical -> one copy at ours' position, different -> CONFLICT.
- Row order: ours' order, plus the inserts above.
- Bytes: records keep their own line ending (split on `\\n`, `\\r` kept). A row taken from theirs is
  written with ours' line ending. A row that is not changed is written byte-identical to ours.
- Missing columns: when a version lacks a column the output has, that version's value for it is taken
  as "not changed": a side takes base's value (then the other side's), base takes ours' (then
  theirs'). So a branch from
  before the `port` column merges by its other cells only.
- Malformed rows (cell count != header width) are compared as raw text, reported as field `<raw>`.

On CONFLICT the output carries git-style markers around each conflicting record and the exit code is
1. Exit 0 = clean.

Use
- git merge driver: `py -3.12 scripts/backfill_port.py --merge-driver %O %A %B` (the registered
  `hooksport` driver, .gitattributes `/hooks.csv merge=hooksport`) delegates here; or directly
  `py -3.12 scripts/hookscsv_merge.py --driver %O %A %B`. The result is written to %A.
- standalone: `py -3.12 scripts/hookscsv_merge.py BASE OURS THEIRS [-o OUT]` (stdout when no -o).
- self-test: `py -3.12 scripts/test_hookscsv_merge.py`.
"""
from __future__ import annotations

import csv
import pathlib
import sys

TOP = "\x00TOP"
TAG = "[hookscsv-merge]"


class Rec:
    __slots__ = ("raw", "body", "eol", "cells", "kind", "key")

    def __init__(self, raw: str):
        self.raw = raw
        self.body = raw.rstrip("\r\n")
        self.eol = raw[len(self.body):]
        self.cells = next(csv.reader([self.body])) if self.body else []
        c0 = self.cells[0].strip() if self.cells else ""
        if not self.body.strip() or self.body.startswith("#") or c0.startswith("#"):
            self.kind, self.key = "c", None
        else:
            self.kind, self.key = "d", c0.lower()


def split_records(text: str) -> list[str]:
    """Physical lines (each keeps its `\\r\\n` / `\\n`), joined while a quote is left open."""
    out, cur = [], ""
    for line in text.splitlines(keepends=True):
        cur += line
        if cur.count('"') % 2 == 0:
            out.append(cur)
            cur = ""
    if cur:
        out.append(cur)
    return out


class Doc:
    def __init__(self, text: str):
        recs = [Rec(r) for r in split_records(text)]
        self.header = recs[0] if recs else Rec("")
        self.cols = self.header.cells
        self.rows: dict[str, Rec] = {}
        self.order: list[str] = []                    # data keys in file order
        self.groups: dict[str, list[Rec]] = {TOP: []}  # comment group after each key
        anchor = TOP
        for r in recs[1:]:
            if r.kind == "c":
                self.groups.setdefault(anchor, []).append(r)
                continue
            if r.key in self.rows:
                raise ValueError(f"duplicate rva {r.key}")
            self.rows[r.key] = r
            self.order.append(r.key)
            self.groups.setdefault(r.key, [])
            anchor = r.key
        self.eol = self.header.eol or "\n"

    def value(self, key: str, col: str):
        r = self.rows[key]
        if len(r.cells) != len(self.cols):
            return None
        return r.cells[self.cols.index(col)] if col in self.cols else None


def _fields(doc: Doc, key: str, cols, fallbacks) -> dict | str:
    """Row as {col: value} over the output columns, or its raw body when malformed."""
    r = doc.rows[key]
    if len(r.cells) != len(doc.cols):
        return r.body
    out = {}
    for c in cols:
        if c in doc.cols:
            out[c] = r.cells[doc.cols.index(c)]
        else:
            out[c] = next((f.value(key, c) for f in fallbacks
                           if key in f.rows and c in f.cols and f.value(key, c) is not None), "")
    return out


def _render(doc: Doc, key: str, cols, filled: dict | str, eol: str) -> str:
    """The row's own bytes when its columns already match the output, else its body plus the cells it
    lacks (only trailing columns can be missing, see the header rule)."""
    r = doc.rows[key]
    if isinstance(filled, str) or list(doc.cols) == list(cols):
        return r.body + eol
    extra = [filled[c] for c in cols[len(doc.cols):]]
    tail = ",".join(f'"{v}"' if ("," in v or '"' in v) else v for v in extra)
    return f"{r.body},{tail}{eol}"


def _diff_fields(a, b) -> list[str]:
    if isinstance(a, str) or isinstance(b, str):
        return ["<raw>"]
    return [c for c in a if a[c] != b.get(c)]


def merge(base_t: str, ours_t: str, theirs_t: str):
    """Return (text, conflicts). conflicts = list of human-readable lines; empty = clean."""
    O, A, B = Doc(base_t), Doc(ours_t), Doc(theirs_t)
    conflicts: list[str] = []
    eol = A.eol

    # header
    ca, cb = list(A.cols), list(B.cols)
    if ca == cb or cb[:len(ca)] == ca or ca[:len(cb)] == cb:
        cols = ca if len(ca) >= len(cb) else cb
        header = A.header.raw if cols == ca else B.header.body + eol
    else:
        cols, header = ca, A.header.raw
        conflicts.append(f"header: ours {ca} theirs {cb}")

    def F(doc, key, others):
        return _fields(doc, key, cols, others)

    out_rows: dict[str, str] = {}      # key -> rendered text (may hold markers)
    keep: list[str] = []
    for k in A.order:
        fa = F(A, k, [O, B])
        in_o, in_b = k in O.rows, k in B.rows
        if not in_o:
            if in_b:
                fb = F(B, k, [O, A])
                if fa != fb:
                    fl = _diff_fields(fa, fb)
                    conflicts.append(f"0x{k}: added on both sides, differ in {','.join(fl)}")
                    out_rows[k] = _marker(A.rows[k].body, B.rows[k].body, eol)
                    keep.append(k)
                    continue
            out_rows[k] = _render(A, k, cols, fa, eol)
            keep.append(k)
            continue
        fo = F(O, k, [A, B])
        if not in_b:                          # theirs deleted
            if fa == fo:
                continue
            conflicts.append(f"0x{k}: deleted by theirs, edited by ours ({','.join(_diff_fields(fo, fa))})")
            out_rows[k] = _marker(A.rows[k].body, "", eol)
            keep.append(k)
            continue
        fb = F(B, k, [O, A])
        if fb == fo or fa == fb:
            out_rows[k] = _render(A, k, cols, fa, eol)
        elif fa == fo:
            out_rows[k] = _render(B, k, cols, fb, eol)
        else:
            fl = _diff_fields(fa, fb)
            detail = ""
            if not isinstance(fa, str) and not isinstance(fb, str) and not isinstance(fo, str):
                detail = " -- " + "; ".join(f"{c}: base={fo[c]!r} ours={fa[c]!r} theirs={fb[c]!r}" for c in fl)
            conflicts.append(f"0x{k}: both sides changed, differ in {','.join(fl)}{detail}")
            out_rows[k] = _marker(A.rows[k].body, B.rows[k].body, eol)
        keep.append(k)
    # rows ours deleted that theirs edited
    for k in O.order:
        if k not in A.rows and k in B.rows:
            fo, fb = F(O, k, [B]), F(B, k, [O])
            if fb != fo:
                conflicts.append(f"0x{k}: deleted by ours, edited by theirs ({','.join(_diff_fields(fo, fb))})")
                out_rows[k] = _marker("", B.rows[k].body, eol)
                _insert_after(keep, B, k, set(keep))
    # rows only theirs added
    present = set(keep)
    for k in B.order:
        if k not in O.rows and k not in A.rows:
            out_rows[k] = _render(B, k, cols, F(B, k, [O, A]), eol)
            _insert_after(keep, B, k, present)
            present.add(k)

    # comment groups (anchored to TOP or to a data key)
    def grp(doc, a):
        return [r.body for r in doc.groups.get(a, [])]

    def take(doc, a):
        return "".join(r.body + eol if doc is not A else r.raw for r in doc.groups.get(a, []))

    group_text: dict[str, str] = {}
    for a in [TOP] + keep:
        ga, gb, go = grp(A, a), grp(B, a), grp(O, a)
        if gb == go or ga == gb:
            group_text[a] = take(A, a)
        elif ga == go:
            group_text[a] = take(B, a)
        else:
            conflicts.append(f"comment group after {'TOP' if a == TOP else '0x' + a}: both sides changed")
            group_text[a] = _marker(eol.join(ga), eol.join(gb), eol)
    # a group whose anchor row is not in the output: a side that edited it would lose the edit
    kept = set(keep)
    for a in set(A.groups) | set(B.groups) | set(O.groups):
        if a != TOP and a not in kept:
            ga, gb, go = grp(A, a), grp(B, a), grp(O, a)
            if (ga and ga != go) or (gb and gb != go):
                conflicts.append(f"comment group after deleted row 0x{a}: edited on one side")
    parts = [header, group_text[TOP]]
    for k in keep:
        parts.append(out_rows[k])
        parts.append(group_text.get(k, ""))
    text = "".join(parts)
    if not ours_t.endswith(("\n",)) and text.endswith(eol):
        text = text[: -len(eol)]
    return text, conflicts


def _insert_after(keep: list[str], B: Doc, k: str, present: set):
    """Place theirs' row `k` right after the nearest preceding theirs data row that is in the output
    (TOP when none). Comment groups are anchored to the row before them and merged per anchor, so a
    group that followed the new row in theirs is anchored to the new row there and moves with it."""
    i = B.order.index(k)
    for j in range(i - 1, -1, -1):
        p = B.order[j]
        if p in present:
            keep.insert(keep.index(p) + 1, k)
            return
    keep.insert(0, k)


def _marker(a: str, b: str, eol: str) -> str:
    s = f"<<<<<<< ours{eol}"
    if a:
        s += a.rstrip("\r\n") + eol
    s += f"======={eol}"
    if b:
        s += b.rstrip("\r\n") + eol
    return s + f">>>>>>> theirs{eol}"


def _read(p) -> str:
    return pathlib.Path(p).read_bytes().decode("utf-8")


def driver(o: str, a: str, b: str) -> int:
    text, conflicts = merge(_read(o), _read(a), _read(b))
    pathlib.Path(a).write_bytes(text.encode("utf-8"))
    for c in conflicts:
        print(f"{TAG} CONFLICT {c}", file=sys.stderr)
    if conflicts:
        print(f"{TAG} hooks.csv: {len(conflicts)} conflict(s)", file=sys.stderr)
        return 1
    return 0


def main(argv) -> int:
    if argv and argv[0] == "--driver":
        return driver(*argv[1:4])
    out = None
    if "-o" in argv:
        i = argv.index("-o")
        out = argv[i + 1]
        argv = argv[:i] + argv[i + 2:]
    if len(argv) != 3:
        print(__doc__)
        return 2
    text, conflicts = merge(*(_read(p) for p in argv))
    if out:
        pathlib.Path(out).write_bytes(text.encode("utf-8"))
    else:
        sys.stdout.buffer.write(text.encode("utf-8"))
    for c in conflicts:
        print(f"{TAG} CONFLICT {c}", file=sys.stderr)
    return 1 if conflicts else 0


if __name__ == "__main__":
    sys.exit(main(sys.argv[1:]))
