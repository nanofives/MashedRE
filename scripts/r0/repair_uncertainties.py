#!/usr/bin/env python3
"""R0 repair for UNCERTAINTIES.md (Phase R0, 2026-06-09). Two operations:

1. Duplicate U-ID renumbering: where two+ rows share a numeric ID, the first
   occurrence (document order) keeps it; later occurrences get fresh IDs
   allocated past the current max, with a breadcrumb appended in their last
   cell ("renumbered from U-XXXX 2026-06-09 R0"). Map written to
   log/backups/uncertainties_renumber_map.txt.
2. Section re-file: every table row after '## Conventions' (and any misfiled
   within sections) goes to '## Resolved (audit trail)' if struck (~~) or
   carrying an uppercase RESOLVED token, else to '## Active uncertainties'.
   NOTE: lowercase 'resolved' ("...not resolved this pass" boilerplate) is
   deliberately NOT a resolved signal. '## Types' section preserved.

Invariants enforced before write: row count conserved; no duplicate numeric
IDs remain; no rows left after Conventions. Backup to
log/backups/UNCERTAINTIES.md.pre_r0.
"""
import re
import shutil
import sys
from collections import Counter
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
PATH = ROOT / "UNCERTAINTIES.md"
BACKUP = ROOT / "log" / "backups" / "UNCERTAINTIES.md.pre_r0"
MAP_OUT = ROOT / "log" / "backups" / "uncertainties_renumber_map.txt"

# ID cell variants: U-1234, ~~U-1234~~, U-4183a (letter sub-ID),
# "U-4309 (RESOLVED 2026-05-25)" (inline annotation).
ROW = re.compile(r"^\|\s*(~{0,2})U-(\d+)([a-z]?)\s*(\(RESOLVED[^)|]*\))?(~{0,2})\s*\|")


def is_resolved(line: str) -> bool:
    return line.lstrip("| ").startswith("~~") or re.search(r"\bRESOLVED\b", line)


def main() -> int:
    lines = PATH.read_text(encoding="utf-8").splitlines()
    total_before = sum(1 for l in lines if ROW.match(l))

    # --- Pass 1: dedup IDs (key = numeric id + optional letter suffix) ---
    ids = [int(m.group(2)) for l in lines if (m := ROW.match(l))]
    next_id = max(ids) + 1
    seen: set[tuple[int, str]] = set()
    renumber_map: list[tuple[str, int]] = []
    out1: list[str] = []
    for l in lines:
        m = ROW.match(l)
        if not m:
            out1.append(l)
            continue
        key = (int(m.group(2)), m.group(3))
        if key in seen:
            new = next_id
            next_id += 1
            old_tok = f"U-{m.group(2)}{m.group(3)}"
            anno = (" " + m.group(4)) if m.group(4) else ""
            new_tok = f"| {m.group(1)}U-{new:04d}{anno}{m.group(5)} |"
            l2 = ROW.sub(new_tok, l, count=1)
            if l2.rstrip().endswith("|"):
                body = l2.rstrip()[:-1].rstrip()
                l2 = body + f"; renumbered from {old_tok} 2026-06-09 R0 |"
            renumber_map.append((old_tok, new))
            out1.append(l2)
            seen.add((new, ""))
        else:
            seen.add(key)
            out1.append(l)

    # --- Pass 2: section re-file ---
    idx_active = out1.index("## Active uncertainties")
    idx_resolved = next(i for i, l in enumerate(out1)
                        if l.startswith("## Resolved"))
    idx_types = out1.index("## Types")
    idx_conv = out1.index("## Conventions")

    def split(section):
        return ([l for l in section if not ROW.match(l)],
                [l for l in section if ROW.match(l)])

    head = out1[:idx_active]
    act_scaffold, act_rows = split(out1[idx_active:idx_resolved])
    res_scaffold, res_rows = split(out1[idx_resolved:idx_types])
    types_sec = out1[idx_types:idx_conv]
    conv_scaffold, conv_rows = split(out1[idx_conv:])

    act_stay = [r for r in act_rows if not is_resolved(r)]
    act_to_res = [r for r in act_rows if is_resolved(r)]
    res_stay = [r for r in res_rows if is_resolved(r)]
    res_to_act = [r for r in res_rows if not is_resolved(r)]
    tail_to_act = [r for r in conv_rows if not is_resolved(r)]
    tail_to_res = [r for r in conv_rows if is_resolved(r)]

    def trimmed(s):
        while s and s[-1].strip() == "":
            s.pop()
        return s

    out = (head
           + trimmed(act_scaffold) + act_stay + res_to_act + tail_to_act + [""]
           + trimmed(res_scaffold) + res_stay + act_to_res + tail_to_res + [""]
           + trimmed(types_sec) + [""]
           + trimmed(conv_scaffold) + [""])

    # --- Invariants ---
    total_after = sum(1 for l in out if ROW.match(l))
    keys_after = [(int(m.group(2)), m.group(3)) for l in out
                  if (m := ROW.match(l))]
    dups_after = [k for k, v in Counter(keys_after).items() if v > 1]
    if total_after != total_before:
        print(f"FATAL: row count drift {total_before} -> {total_after}")
        return 1
    if dups_after:
        print(f"FATAL: duplicate IDs remain: {dups_after[:10]}")
        return 1

    BACKUP.parent.mkdir(parents=True, exist_ok=True)
    shutil.copy2(PATH, BACKUP)
    with open(MAP_OUT, "a", encoding="utf-8") as f:
        f.write("".join(f"{a} -> U-{b:04d}\n" for a, b in renumber_map))
    PATH.write_text("\n".join(out) + "\n", encoding="utf-8")

    span = (f"(new ids U-{renumber_map[0][1]:04d}..U-{renumber_map[-1][1]:04d}) "
            if renumber_map else "")
    print(f"OK rows={total_before} renumbered={len(renumber_map)} {span}"
          f"active={len(act_stay)+len(res_to_act)+len(tail_to_act)} "
          f"resolved={len(res_stay)+len(act_to_res)+len(tail_to_res)} "
          f"from_tail(to_act={len(tail_to_act)}, to_res={len(tail_to_res)}) "
          f"refiled_within={len(act_to_res)+len(res_to_act)}")
    return 0


if __name__ == "__main__":
    sys.exit(main())
