#!/usr/bin/env python3
"""R0 C4-ledger repair (Phase R0, 2026-06-09). See re/analysis/C4_REVALIDATION.md.

Three operations, each verified per-row before mutating (NO-GUESSING):
  A. Population B (24 escapee RVAs): tag C4-EVIDENCE-SUSPECT IF the row is C4,
     notes/scenario cite in-window 2026-05-23 evidence, and no post-fix
     evidence marker is present. Otherwise: report + skip.
  B. Population C (Vec3 trio): tag with borderline wording IF C4 and no
     post-fix evidence.
  C. Rows NOT at C4 still carrying the C4-EVIDENCE-SUSPECT token: rewrite the
     token to C4-EVIDENCE-SUSPECT-CLEARED(2026-06-09,demoted) so the standard
     `grep C4-EVIDENCE-SUSPECT:` converges to the open population while the
     history stays readable.

Prints every decision. Backup to log/backups/hooks.csv.pre_c4tag.
"""
import csv
import io
import shutil
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
CSV_PATH = ROOT / "hooks.csv"
BACKUP = ROOT / "log" / "backups" / "hooks.csv.pre_c4tag"

ESCAPEES = {
    "00428590", "00431ae0", "00431af0", "00431b00", "004921d0", "00492270",
    "004938c0", "00495120", "00495270", "00498c00", "004c2d90", "004c5800",
    "004c5820", "004c5a00", "004c5ae0", "004c5b50", "004c5c80", "004c9eb0",
    "004c9f50", "004c9f60", "004cbc60", "004cbc70", "004cbc80", "004cc820",
}
TRIO = {"004c3ac0", "004c3b30", "004c3b90"}

IN_WINDOW = ("2026-05-23 c4-lift", "boot-to-menu-canonical-r3",
             "mass-canonical-wave2", "texture_loader_d3_cont1",
             "mass_canonical_wave2", "boot-to-menu-canonical-wave")
POST_FIX = ("post_loaderfix", "install_observe", "leafdiff", "sidediff",
            "installed 0xE9", "install-reconfirmed", "2026-06-0")

TAG_ESCAPEE = ("C4-EVIDENCE-SUSPECT: canonical evidence in broken-loader window "
               "(2026-05-15..24); escaped 6e359fe8 tagging; tagged 2026-06-09 R0 "
               "audit. C4 RETAINED pending installed-hook canonical re-validation.")
TAG_TRIO = ("C4-EVIDENCE-SUSPECT: evidence log/observe_hooks_at_menu.txt "
            "timestamped 2026-05-15 00:57 = day one of broken-loader window; "
            "pre/post-breakage order unestablished; tagged 2026-06-09 R0 audit. "
            "C4 RETAINED pending installed-hook canonical re-validation.")


def main() -> int:
    lines = CSV_PATH.read_text(encoding="utf-8").splitlines()
    header, body = lines[0], lines[1:]
    out: list[str] = [header]
    stats = {"tagged_escapee": 0, "tagged_trio": 0, "cleared": 0,
             "skipped": [], "already_tagged": 0}

    for ln in body:
        if ln.startswith("#") or not ln.strip():
            out.append(ln)
            continue
        row = next(csv.reader(io.StringIO(ln)))
        if len(row) != 9:
            print(f"FATAL: row without 9 fields: {ln[:80]}")
            return 1
        rva, conf, scenario, notes = row[0], row[3], row[6], row[8]
        blob = (scenario + " " + notes)

        if rva in ESCAPEES or rva in TRIO:
            label = "trio" if rva in TRIO else "escapee"
            if "C4-EVIDENCE-SUSPECT:" in notes:
                print(f"SKIP {rva} ({label}): already tagged")
                stats["already_tagged"] += 1
            elif conf != "C4":
                print(f"SKIP {rva} ({label}): confidence={conf}, not C4")
                stats["skipped"].append(rva)
            elif any(m in blob for m in POST_FIX):
                hit = [m for m in POST_FIX if m in blob]
                print(f"SKIP {rva} ({label}): post-fix evidence present {hit}")
                stats["skipped"].append(rva)
            elif rva in ESCAPEES and not any(m in blob for m in IN_WINDOW):
                print(f"SKIP {rva} (escapee): no in-window marker found")
                stats["skipped"].append(rva)
            else:
                tag = TAG_TRIO if rva in TRIO else TAG_ESCAPEE
                row[8] = notes.rstrip() + " | " + tag
                stats["tagged_trio" if rva in TRIO else "tagged_escapee"] += 1
                print(f"TAG  {rva} ({label})")
        elif conf != "C4" and "C4-EVIDENCE-SUSPECT:" in notes:
            row[8] = notes.replace(
                "C4-EVIDENCE-SUSPECT:",
                "C4-EVIDENCE-SUSPECT-CLEARED(2026-06-09,demoted):")
            stats["cleared"] += 1
            print(f"CLR  {rva}: stale tag on {conf} row rewritten")

        buf = io.StringIO()
        csv.writer(buf, lineterminator="").writerow(row)
        out.append(buf.getvalue())

    BACKUP.parent.mkdir(parents=True, exist_ok=True)
    shutil.copy2(CSV_PATH, BACKUP)
    CSV_PATH.write_text("\n".join(out) + "\n", encoding="utf-8")

    print(f"\nDONE tagged_escapee={stats['tagged_escapee']} "
          f"tagged_trio={stats['tagged_trio']} cleared={stats['cleared']} "
          f"already_tagged={stats['already_tagged']} "
          f"skipped={stats['skipped']}")
    return 0


if __name__ == "__main__":
    sys.exit(main())
