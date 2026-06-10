#!/usr/bin/env python3
"""R0 section re-file for STUBS.md (Phase R0, 2026-06-09).

Automated appends bypassed the section structure: ~1,022 table rows sit after
'## Conventions'. This script re-files every table row into:
  - '## Resolved stubs' if the row is struck (~~), contains **cleared**, or has
    a 'resolved' status cell;
  - '## Active stubs' otherwise.
Rows already inside Active/Resolved keep their relative order; appended rows
follow them. The Conventions section keeps only its prose. Row IDs are NOT
renamed (some appended rows carry U- prefixes — known drift, left intact and
reported). Invariant: total table-row count conserved; refuses to write
otherwise. Backup to log/backups/STUBS.md.pre_r0.
"""
import re
import shutil
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
PATH = ROOT / "STUBS.md"
BACKUP = ROOT / "log" / "backups" / "STUBS.md.pre_r0"

ROW = re.compile(r"^\|\s*~{0,2}[SUD]-\d+")


def is_resolved(line: str) -> bool:
    return ("~~" in line) or ("**cleared**" in line) or re.search(
        r"\|\s*resolved\s*\|", line) is not None


def main() -> int:
    lines = PATH.read_text(encoding="utf-8").splitlines()

    idx_active = lines.index("## Active stubs")
    idx_resolved = next(i for i, l in enumerate(lines)
                        if l.startswith("## Resolved stubs"))
    idx_conv = lines.index("## Conventions")

    head = lines[:idx_active]
    sec_active = lines[idx_active:idx_resolved]
    sec_resolved = lines[idx_resolved:idx_conv]
    sec_conv = lines[idx_conv:]

    total_before = sum(1 for l in lines if ROW.match(l))

    def split(section: list[str]):
        rows = [l for l in section if ROW.match(l)]
        scaffold = [l for l in section if not ROW.match(l)]
        return scaffold, rows

    act_scaffold, act_rows = split(sec_active)
    res_scaffold, res_rows = split(sec_resolved)
    conv_scaffold, conv_rows = split(sec_conv)

    moved_to_res = [r for r in conv_rows if is_resolved(r)]
    moved_to_act = [r for r in conv_rows if not is_resolved(r)]
    # also re-file misfiled rows already inside Active/Resolved
    act_stay = [r for r in act_rows if not is_resolved(r)]
    act_to_res = [r for r in act_rows if is_resolved(r)]
    res_stay = [r for r in res_rows if is_resolved(r)]
    res_to_act = [r for r in res_rows if not is_resolved(r)]

    wrong_prefix = sum(1 for r in conv_rows if re.match(r"^\|\s*~{0,2}[UD]-", r))

    # trim trailing blank scaffold lines so sections butt up cleanly
    def trimmed(s):
        while s and s[-1].strip() == "":
            s.pop()
        return s

    out = (head
           + trimmed(act_scaffold) + act_stay + res_to_act + moved_to_act + [""]
           + trimmed(res_scaffold) + res_stay + act_to_res + moved_to_res + [""]
           + trimmed(conv_scaffold) + [""])

    total_after = sum(1 for l in out if ROW.match(l))
    if total_after != total_before:
        print(f"FATAL: row count drift {total_before} -> {total_after}")
        return 1

    BACKUP.parent.mkdir(parents=True, exist_ok=True)
    shutil.copy2(PATH, BACKUP)
    PATH.write_text("\n".join(out) + "\n", encoding="utf-8")
    print(f"OK rows={total_before} active={len(act_stay)+len(res_to_act)+len(moved_to_act)} "
          f"resolved={len(res_stay)+len(act_to_res)+len(moved_to_res)} "
          f"moved_from_conventions={len(conv_rows)} "
          f"(to_active={len(moved_to_act)}, to_resolved={len(moved_to_res)}) "
          f"refiled_within_sections={len(act_to_res)+len(res_to_act)} "
          f"wrong_prefix_rows_left_intact={wrong_prefix}")
    return 0


if __name__ == "__main__":
    sys.exit(main())
