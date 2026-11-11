"""Record-level 3-way merge drivers for the generated ORIG_TOP files (HB-GENFILES-NOCONFLICT, 2026-11-10).

Two ORIG_TOP rows in flight both edit the same three files, and git's line merge bounced every such pair in the merge
train (HUDINIT-A twice on 2026-11-10):
  verify/porttop_20261108/BINDINGS.tsv                  rows appended / rewritten by each row's updater
  verify/portstate_20261108/SEED_CANDIDATES.tsv         the port-state seed inventory (address-sorted rows)
  mashedmod/src/mashed_re/Compat/PortStateSeed.inc      generated from that inventory (re/tools/gen_portstate_seed.py)
Each file is a set of records with a key, so these drivers merge records, not lines (the scripts/hookscsv_merge.py
precedent for hooks.csv).

Common rule, per key: changed on one side only -> that side; changed identically on both -> clean; changed differently
on both -> CONFLICT (git-style markers around the record, exit 1); deleted on one side and edited on the other ->
CONFLICT. A real disagreement therefore stops the merge loudly, it never picks a side.

bindings  BINDINGS.tsv (tab-separated, key = `rva` lowercased). Fields merge one by one (a field changed on one side
          only is taken from that side), so two rows that edit different columns of one rva merge. The `note` column has
          one more clean case: when both sides only appended to base's note, the result is base + ours' suffix +
          theirs' suffix (two rows each appending a note to the same rva). Rows ours added stay where ours has them.
          Rows theirs added after its last base row are appended at the end, after ours' own new rows (both sides'
          updaters append, and theirs is merged after ours); a row theirs added earlier goes right after its nearest
          preceding row present in the output. Header from ours (a different header halts).
seedtsv   SEED_CANDIDATES.tsv (key = `addr` as an integer). Output is address-sorted, which is how the inventory is
          kept. Header from ours (a different header halts).
seedinc   PortStateSeed.inc. Every entry (address, bytes, name, section, readers) is parsed from the three versions,
          merged by address, and the file is rendered again with re/tools/gen_portstate_seed.py's own render(), so the
          result is byte-identical to running the generator on the merged inventory. Two entries that overlap after the
          merge halt (the generator refuses overlaps too).

Line endings: the output uses ours' line ending.

Use
- git merge drivers (.gitattributes + one-time config, see the RESULT / install below):
    py -3.12 scripts/genfiles_merge.py --driver bindings|seedtsv|seedinc %O %A %B
  The result is written to %A, exit 0 clean, 1 conflict, 2 halt (unusable input).
- install the three drivers into this clone's git config: py -3.12 scripts/genfiles_merge.py --install
- standalone: py -3.12 scripts/genfiles_merge.py KIND BASE OURS THEIRS [-o OUT]
Self-test: py -3.12 -m pytest scripts/test_genfiles_merge.py
"""
from __future__ import annotations

import importlib.util
import os
import pathlib
import re
import subprocess
import sys

HERE = pathlib.Path(__file__).resolve().parent
REPO = HERE.parent
DRIVERS = {
    "bindings": ("genbindings", "/verify/porttop_20261108/BINDINGS.tsv"),
    "seedtsv": ("genseedtsv", "/verify/portstate_20261108/SEED_CANDIDATES.tsv"),
    "seedinc": ("genseedinc", "/mashedmod/src/mashed_re/Compat/PortStateSeed.inc"),
}


class Halt(Exception):
    pass


def _eol(text: str) -> str:
    return "\r\n" if "\r\n" in text else "\n"


def _lines(text: str) -> list[str]:
    t = text.replace("\r\n", "\n")
    if t.endswith("\n"):
        t = t[:-1]
    return t.split("\n") if t else []


def _marker(ours: str, theirs: str) -> list[str]:
    return ["<<<<<<< ours", ours, "=======", theirs, ">>>>>>> theirs"]


# ------------------------------------------------------------------------------------------------- keyed TSV records
def _table(text: str, key_of):
    ls = _lines(text)
    if not ls:
        return "", [], {}
    head, rows = ls[0], [l for l in ls[1:] if l.strip()]
    order, recs = [], {}
    for r in rows:
        k = key_of(r)
        if k in recs:
            raise Halt("duplicate key %r" % (k,))
        order.append(k)
        recs[k] = r
    return head, order, recs


def _merge_fields(b: str | None, o: str, t: str, note_col: int | None):
    """Field-wise 3-way merge of one tab-separated row. Returns (row, None) or (None, reason)."""
    if b is None:
        return (o, None) if o == t else (None, "added differently on both sides")
    bf, of, tf = b.split("\t"), o.split("\t"), t.split("\t")
    if not (len(bf) == len(of) == len(tf)):
        return (None, "column count differs")
    out = []
    for i, (x, y, z) in enumerate(zip(bf, of, tf)):
        if y == z or z == x:
            out.append(y)
        elif y == x:
            out.append(z)
        elif i == note_col and y.startswith(x) and z.startswith(x):
            out.append(x + y[len(x):] + z[len(x):])
        else:
            return (None, "column %d changed on both sides" % i)
    return ("\t".join(out), None)


def merge_bindings(base: str, ours: str, theirs: str):
    key = lambda r: r.split("\t", 1)[0].strip().lower()
    hb, ob, rb = _table(base, key)
    ho, oo, ro = _table(ours, key)
    ht, ot, rt = _table(theirs, key)
    if ho != ht or (hb and hb != ho):
        if not (hb == ho or hb == ht):
            raise Halt("header differs on both sides")
    head = ho if ho == ht else (ht if ho == hb else ho)
    cols = head.split("\t")
    note_col = cols.index("note") if "note" in cols else None
    conflicts = 0
    out: dict[str, list[str]] = {}

    def resolve(k):
        nonlocal conflicts
        b, o, t = rb.get(k), ro.get(k), rt.get(k)
        if o == t:
            return [o] if o is not None else []
        if t == b:
            return [o] if o is not None else []
        if o == b:
            return [t] if t is not None else []
        if o is None or t is None:
            conflicts += 1
            return _marker(o or "(deleted)", t or "(deleted)")
        row, why = _merge_fields(b, o, t, note_col)
        if row is not None:
            return [row]
        conflicts += 1
        return _marker(o, t)

    keys = []
    for k in oo:
        keys.append(k)
    present = set(oo)
    # rows theirs has that ours does not (added by theirs, or deleted by ours)
    base_in_t = [k for k in ot if k in rb]
    last_base_t = ot.index(base_in_t[-1]) if base_in_t else -1
    tail, inserts = [], []
    for i, k in enumerate(ot):
        if k in present:
            continue
        if k in rb:          # deleted by ours: kept deleted unless theirs edited it (resolve() decides)
            if rt[k] == rb[k]:
                continue
            inserts.append((i, k))
            continue
        if i > last_base_t:
            tail.append(k)
        else:
            inserts.append((i, k))
    for i, k in inserts:
        prev = next((ot[j] for j in range(i - 1, -1, -1) if ot[j] in keys), None)
        pos = keys.index(prev) + 1 if prev is not None else 0
        keys.insert(pos, k)
    keys += tail
    body = []
    for k in keys:
        body += resolve(k)
    eol = _eol(ours)
    text = eol.join([head] + body) + eol
    return text, conflicts


def merge_seed_tsv(base: str, ours: str, theirs: str):
    key = lambda r: int(r.split("\t", 1)[0], 16)
    hb, _, rb = _table(base, key)
    ho, _, ro = _table(ours, key)
    ht, _, rt = _table(theirs, key)
    if ho != ht:
        raise Halt("SEED_CANDIDATES.tsv header differs")
    conflicts, body = 0, []
    for k in sorted(set(rb) | set(ro) | set(rt)):
        b, o, t = rb.get(k), ro.get(k), rt.get(k)
        if o == t:
            r = [o] if o is not None else []
        elif t == b:
            r = [o] if o is not None else []
        elif o == b:
            r = [t] if t is not None else []
        else:
            conflicts += 1
            r = _marker(o or "(deleted)", t or "(deleted)")
        body += r
    eol = _eol(ours)
    return eol.join([ho] + body) + eol, conflicts


# ------------------------------------------------------------------------------------------------- PortStateSeed.inc
def _gen():
    spec = importlib.util.spec_from_file_location("gen_portstate_seed", REPO / "re" / "tools" / "gen_portstate_seed.py")
    m = importlib.util.module_from_spec(spec)
    spec.loader.exec_module(m)
    return m


_ENT_HDR = re.compile(r"^    // 0x([0-9a-f]{8}) (\S*) \(([^,]*), (\d+)\)$")
_ENT_ROW = re.compile(r"^    \{ 0x([0-9a-f]{8})u,\s*(\d+)u,\s*(\d+)u \},   // (\S*), read by (.*)$")


def parse_seed_inc(text: str):
    """{addr: (bytes, glob, section, rows)} plus the inventory path recorded in the header."""
    g = _gen()
    ents, _ = g.parse_inc(text.replace("\r\n", "\n"))
    meta, readers = {}, {}
    inv = None
    for line in _lines(text):
        m = _ENT_HDR.match(line)
        if m:
            meta[int(m.group(1), 16)] = (m.group(2), m.group(3))
        m = _ENT_ROW.match(line)
        if m:
            readers[int(m.group(1), 16)] = m.group(5)
        if line.startswith("// inventory: "):
            inv = line[len("// inventory: "):].split(" sha256 ")[0]
    out = {}
    for a, d in ents:
        glob, sec = meta.get(a, ("", ""))
        rows = readers.get(a, "-")
        out[a] = (bytes(d), glob, sec, "" if rows == "-" else rows)
    return out, inv


def render_seed_inc(ents: dict, inv_rel: str, bin_sha: str) -> str:
    g = _gen()
    entries = [(a, ents[a][0], dict(glob=ents[a][1], section=ents[a][2], rows=ents[a][3])) for a in sorted(ents)]
    prev_end = 0
    for a, d, r in entries:
        if a < prev_end:
            raise Halt("0x%08x %s overlaps the previous seed range after the merge" % (a, r["glob"]))
        prev_end = a + len(d)
    text, _ = g.render(entries, os.path.join(g.REPO, inv_rel), None, bin_sha)
    return text + "\n" if not text.endswith("\n") else text


def merge_seed_inc(base: str, ours: str, theirs: str):
    rb, _ = parse_seed_inc(base)
    ro, inv = parse_seed_inc(ours)
    rt, _ = parse_seed_inc(theirs)
    m = re.search(r"sha256 ([0-9A-F]{64})", ours.split("// binary:", 1)[1]) if "// binary:" in ours else None
    if not m:
        raise Halt("PortStateSeed.inc: no binary sha256 line")
    conflicts, merged, clash = 0, {}, []
    for k in sorted(set(rb) | set(ro) | set(rt)):
        b, o, t = rb.get(k), ro.get(k), rt.get(k)
        if o == t or t == b:
            v = o
        elif o == b:
            v = t
        else:
            conflicts += 1
            clash.append(k)
            continue
        if v is not None:
            merged[k] = v
    if conflicts:
        lines = ["<<<<<<< ours"] + _lines(ours) + ["=======", ] + _lines(theirs) + [">>>>>>> theirs",
                 "// genfiles_merge: seed entries changed differently on both sides: " +
                 " ".join("0x%08x" % k for k in clash)]
        return _eol(ours).join(lines) + _eol(ours), conflicts
    text = render_seed_inc(merged, inv or "verify/portstate_20261108/SEED_CANDIDATES.tsv", m.group(1))
    return text.replace("\n", _eol(ours)), 0


MERGERS = {"bindings": merge_bindings, "seedtsv": merge_seed_tsv, "seedinc": merge_seed_inc}


def _read(p) -> str:
    return open(p, "rb").read().decode("utf-8")


def driver(kind: str, o: str, a: str, b: str) -> int:
    try:
        text, conflicts = MERGERS[kind](_read(o), _read(a), _read(b))
    except Halt as e:
        print("[genfiles_merge %s] HALT: %s" % (kind, e))
        return 2
    open(a, "wb").write(text.encode("utf-8"))
    if conflicts:
        print("[genfiles_merge %s] %d conflicting record(s)" % (kind, conflicts))
        return 1
    return 0


def install() -> int:
    for kind, (name, _) in DRIVERS.items():
        # A branch that predates this script has no scripts/genfiles_merge.py: fall back to git's line merge there.
        cmd = ("if [ -f scripts/genfiles_merge.py ]; then py -3.12 scripts/genfiles_merge.py --driver %s %%O %%A %%B; "
               "else git merge-file %%A %%O %%B; fi" % kind)
        subprocess.check_call(["git", "config", "merge.%s.name" % name, "genfiles_merge %s (HB-GENFILES-NOCONFLICT)" % kind])
        subprocess.check_call(["git", "config", "merge.%s.driver" % name, cmd])
        print("merge.%s.driver = %s" % (name, cmd))
    return 0


def main(argv) -> int:
    if argv[:1] == ["--install"]:
        return install()
    if argv[:1] == ["--driver"]:
        return driver(argv[1], argv[2], argv[3], argv[4])
    if len(argv) >= 4 and argv[0] in MERGERS:
        kind, b, o, t = argv[:4]
        try:
            text, conflicts = MERGERS[kind](_read(b), _read(o), _read(t))
        except Halt as e:
            print("HALT: %s" % e, file=sys.stderr)
            return 2
        if "-o" in argv:
            open(argv[argv.index("-o") + 1], "wb").write(text.encode("utf-8"))
        else:
            sys.stdout.write(text)
        return 1 if conflicts else 0
    print(__doc__)
    return 2


if __name__ == "__main__":
    sys.exit(main(sys.argv[1:]))
