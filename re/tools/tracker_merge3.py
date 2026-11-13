#!/usr/bin/env python3
"""Keyed 3-way merge for the line-per-record trackers (HB-TRACKER-AT-MERGE, folds in HB-DRAIN-VERDICT's recommendation).

  tracker_merge3.py hooks  <ours> <base> <theirs>     hooks.csv, one row per RVA (first field = 8 hex digits)
  tracker_merge3.py allow  <ours> <base> <theirs>     re/tools/dual_copy_allowlist.txt, one entry per line
                                                      (key = the first whitespace-delimited token of a non-# line)

Same argument order as `git merge-file`: the merged result is written over <ours>. Exit 0 = merged cleanly,
1 = conflict (<ours> untouched, the conflicting keys are printed one per line on stdout), 2 = usage / unreadable.

Rule per key (a row is the whole line):
  ours == theirs                -> keep
  theirs == base                -> keep ours (an ours-only change or delete stands)
  ours == base                  -> take theirs (a theirs-only change or delete stands)
  all three differ              -> CONFLICT (same key edited differently on both sides; hand merge)
  key only in theirs            -> inserted after the nearest preceding key of theirs that is in the result
  key only in ours              -> kept
Lines with no key (header, comments, blanks) are compared as a block: unchanged on one side = the other side's
change stands (theirs' added lines are appended at the end); changed differently on both sides = CONFLICT `<unkeyed>`.
Line endings of <ours> are kept (CRLF stays CRLF).
"""
import re
import sys

HEX8 = re.compile(r"[0-9a-fA-F]{8}$")


def key_hooks(line):
    k = line.split(",", 1)[0]
    return k.lower() if HEX8.match(k) else None


def key_allow(line):
    s = line.strip()
    if not s or s.startswith("#"):
        return None
    return s.split()[0]


def parse(raw, keyf):
    eol = "\r\n" if "\r\n" in raw else "\n"
    lines = raw.split(eol)
    rows, order, other = {}, [], []
    for ln in lines:
        k = keyf(ln)
        if k is None:
            other.append(ln)
        else:
            if k in rows:
                raise ValueError(f"duplicate key {k}")
            rows[k] = ln
            order.append(k)
    return eol, lines, rows, order, other


def merge(ours_raw, base_raw, theirs_raw, keyf):
    eol, olines, orows, oorder, oother = parse(ours_raw, keyf)
    _, _, brows, _, bother = parse(base_raw, keyf)
    _, _, trows, torder, tother = parse(theirs_raw, keyf)
    conflicts = []
    # unkeyed block
    if tother != bother and oother != bother and tother != oother:
        conflicts.append("<unkeyed>")
    # per-key decisions
    result = {}
    for k in set(orows) | set(trows) | set(brows):
        o, b, t = orows.get(k), brows.get(k), trows.get(k)
        if o == t:
            res = o
        elif t == b:
            res = o
        elif o == b:
            res = t
        else:
            conflicts.append(k)
            continue
        if res is not None:
            result[k] = res
    if conflicts:
        return None, conflicts
    # rebuild in ours' line order, then place theirs-only keys
    out = []
    placed = set()
    for ln in olines:
        k = keyf(ln)
        if k is None:
            out.append(ln)
        elif k in result:
            out.append(result[k])
            placed.add(k)
        # else: dropped by theirs (ours == base)
    # theirs-only unkeyed lines (ours == base on that block): not expected for these trackers; keep theirs' block
    if oother == bother and tother != bother:
        added = [l for l in tother if l not in oother]
        # insert before the trailing empty string produced by a final newline
        at = len(out) - 1 if out and out[-1] == "" else len(out)
        out[at:at] = added
    # keys new on theirs' side (or changed into existence): insert after their predecessor in theirs
    prev = None
    for k in torder:
        if k in result and k not in placed:
            idx = None
            if prev is not None and prev in placed:
                idx = max((i for i, l in enumerate(out) if keyf(l) == prev), default=None)
            if idx is None:
                at = len(out) - 1 if out and out[-1] == "" else len(out)
            else:
                at = idx + 1
            out.insert(at, result[k])
            placed.add(k)
        prev = k
    return eol.join(out), []


def read(p):
    with open(p, encoding="utf-8", newline="") as f:
        return f.read()


def main(argv):
    if len(argv) != 5 or argv[1] not in ("hooks", "allow"):
        print(__doc__)
        return 2
    keyf = key_hooks if argv[1] == "hooks" else key_allow
    try:
        merged, conflicts = merge(read(argv[2]), read(argv[3]), read(argv[4]), keyf)
    except (OSError, ValueError, UnicodeDecodeError) as e:
        print(f"tracker_merge3: {e}", file=sys.stderr)
        return 2
    if conflicts:
        for c in sorted(conflicts):
            print(c)
        return 1
    with open(argv[2], "w", encoding="utf-8", newline="") as f:
        f.write(merged)
    return 0


if __name__ == "__main__":
    sys.exit(main(sys.argv))
