"""
sweep_build_manifest.py — read 6 batch_w bucket dirs, extract first-bullet plate
text per RVA, and emit a JSON manifest the Ghidra-side script consumes.

Plate format (skill literal):
  "[C1 2026-05-19] " + first_bullet_verbatim
Truncated at last word boundary <=120 chars, append "..." if cut.
No paraphrasing.
"""

import json
import re
import sys
from pathlib import Path

DATE = "2026-05-19"
ROOT = Path(__file__).resolve().parents[1]
ANALYSIS = ROOT / "re" / "analysis"

# Order matches brief (batch_y sweep — sweep-20260519-1818)
BUCKETS = [
    "bucket_00449ba0",
    "bucket_00466100",
    "bucket_004d7ac0",
    "bucket_00554010",
    "bucket_00565d50",
    "bucket_005a5020",
]

# Map bucket -> queue-row rvas (from SCRIBE_QUEUE.md) to ensure we drain exactly
# what was queued, in queue-row order. Derived inline below by parsing.
QUEUE_FILE = ROOT / "re" / "SCRIBE_QUEUE.md"


def parse_queue_rvas(bucket: str) -> list[str]:
    text = QUEUE_FILE.read_text(encoding="utf-8")
    # Find line that contains "bucket=re/analysis/<bucket>" or "bucket=<bucket>"
    for line in text.splitlines():
        if (f"bucket=re/analysis/{bucket}" in line) or (f"bucket={bucket}" in line and "re/analysis/" not in line):
            m = re.search(r"rvas=([0-9a-fA-Fx,]+)", line)
            if m:
                return [r.strip() for r in m.group(1).split(",") if r.strip().startswith("0x")]
    return []


def first_bullet(md_path: Path) -> str:
    """Return the first piece of plate content under '## Mechanical description'.

    Sessions vary: most write a leading '- ' bullet; some write '* '; some write
    prose, sometimes followed by a '1. ' numbered list. Take the first non-empty,
    non-divider line — bullet marker stripped if present — as the plate seed.
    """
    if not md_path.exists():
        return ""
    in_mech = False
    for raw in md_path.read_text(encoding="utf-8", errors="replace").splitlines():
        line = raw.rstrip()
        if line.startswith("## Mechanical description"):
            in_mech = True
            continue
        if in_mech:
            if line.startswith("##"):
                break
            stripped = line.lstrip()
            if not stripped:
                continue
            # Skip horizontal rules / table separators
            if set(stripped) <= {"-", "*", "_", "|", " "}:
                continue
            # Strip bullet/numbered list marker if present, take verbatim text
            if stripped.startswith("- "):
                return stripped[2:].strip()
            if stripped.startswith("* "):
                return stripped[2:].strip()
            m = re.match(r"^\d+\.\s+(.*)$", stripped)
            if m:
                return m.group(1).strip()
            # Plain prose
            return stripped
    return ""


def truncate_word_boundary(text: str, limit: int = 120) -> str:
    if len(text) <= limit:
        return text
    cut = text[:limit]
    # backtrack to last space
    sp = cut.rfind(" ")
    if sp <= 0:
        sp = limit
    return cut[:sp].rstrip(" ,.;:!?-") + "…"


def build_manifest():
    out = {"date": DATE, "buckets": []}
    total_rvas = 0
    total_missing_md = 0

    for bucket in BUCKETS:
        queue_rvas = parse_queue_rvas(bucket)
        if not queue_rvas:
            print(f"WARN: no RVAs found in queue for {bucket}", file=sys.stderr)
        # Pick up .md files that exist in the bucket dir even if not in queue
        # (batch-y agents drifted from candidate lists; precedent: batch-x-s2
        # extras-from-dir handling in sweep-20260519-1404).
        bucket_dir = ANALYSIS / bucket
        dir_rvas = []
        if bucket_dir.is_dir():
            for p in sorted(bucket_dir.glob("0x*.md")):
                dir_rvas.append(p.stem)  # "0xXXXXXXXX"
        # Build a stable order: queue order first, then any dir-only extras
        # in address order; the union is the plate set.
        seen = set()
        ordered_rvas = []
        for r in queue_rvas:
            if r not in seen:
                seen.add(r)
                ordered_rvas.append(r)
        extras = []
        for r in dir_rvas:
            if r not in seen:
                seen.add(r)
                extras.append(r)
        ordered_rvas.extend(extras)
        entries = []
        missing_md = []
        for rva in ordered_rvas:
            md = ANALYSIS / bucket / f"{rva}.md"
            bullet = first_bullet(md)
            if not bullet:
                missing_md.append(rva)
                plate = ""
            else:
                plate = "[C1 " + DATE + "] " + truncate_word_boundary(bullet, 120)
            entries.append({"rva": rva, "plate": plate})
        total_rvas += len(entries)
        total_missing_md += len(missing_md)
        out["buckets"].append({
            "bucket": bucket,
            "rvas": entries,
            "missing_md": missing_md,
            "extras_from_dir": extras,
        })
        print(f"  {bucket}: {len(entries)} rvas ({len(extras)} extras from dir), {len(missing_md)} missing .md")

    out["total_rvas"] = total_rvas
    out["total_missing_md"] = total_missing_md

    manifest_path = ROOT / "tmp" / "sweep_manifest_20260519_1818.json"
    manifest_path.parent.mkdir(exist_ok=True)
    manifest_path.write_text(json.dumps(out, indent=2), encoding="utf-8")
    print(f"\nWrote {manifest_path}")
    print(f"Total: {total_rvas} rvas, {total_missing_md} missing .md")


if __name__ == "__main__":
    build_manifest()
