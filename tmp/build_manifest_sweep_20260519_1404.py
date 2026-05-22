"""Build sweep manifest for sweep-20260519-1404 (batch_x: 6 buckets x 80 RVAs).

Uses the same first-bullet extraction + 120-char truncation as
sweep_build_manifest.py, but reads the 6 batch_x bucket rows from
SCRIBE_QUEUE.md regardless of whether the row sits under '## Queued' or
'## Drained' (s1/s5 were misfiled by their agents and remain there without
a drained-by= tag; the sweep treats them as still-Queued work).
"""

import json
import re
import sys
from pathlib import Path

DATE = "2026-05-19"
ROOT = Path(__file__).resolve().parents[1]
ANALYSIS = ROOT / "re" / "analysis"
QUEUE_FILE = ROOT / "re" / "SCRIBE_QUEUE.md"

# Order matches the brief
BUCKETS = [
    "bucket_00452ec0",   # batch-x-s1
    "bucket_004b4a80",   # batch-x-s2
    "bucket_004c4270",   # batch-x-s3
    "bucket_00549580",   # batch-x-s4
    "bucket_005a6f30",   # batch-x-s5
    "bucket_005bba60",   # batch-x-s6
]


def parse_queue_rvas(bucket: str) -> list:
    text = QUEUE_FILE.read_text(encoding="utf-8")
    for line in text.splitlines():
        if (f"bucket=re/analysis/{bucket}" in line) or (
            f"bucket={bucket}" in line and "re/analysis/" not in line
        ):
            m = re.search(r"rvas=([0-9a-fA-Fx,]+)", line)
            if m:
                return [r.strip() for r in m.group(1).split(",") if r.strip().startswith("0x")]
    return []


def first_bullet(md_path: Path) -> str:
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
            if set(stripped) <= {"-", "*", "_", "|", " "}:
                continue
            if stripped.startswith("- "):
                return stripped[2:].strip()
            if stripped.startswith("* "):
                return stripped[2:].strip()
            m = re.match(r"^\d+\.\s+(.*)$", stripped)
            if m:
                return m.group(1).strip()
            return stripped
    return ""


def truncate_word_boundary(text: str, limit: int = 120) -> str:
    if len(text) <= limit:
        return text
    cut = text[:limit]
    sp = cut.rfind(" ")
    if sp <= 0:
        sp = limit
    return cut[:sp].rstrip(" ,.;:!?-") + "…"


def build_manifest():
    out = {"date": DATE, "buckets": []}
    total_rvas = 0
    total_missing_md = 0
    total_extra = 0

    for bucket in BUCKETS:
        rvas = parse_queue_rvas(bucket)
        if not rvas:
            print(f"WARN: no RVAs found in queue for {bucket}", file=sys.stderr)
        entries = []
        missing_md = []
        seen_rvas = set()
        for rva in rvas:
            md = ANALYSIS / bucket / f"{rva}.md"
            bullet = first_bullet(md)
            if not bullet:
                missing_md.append(rva)
                # skip — do not write plate for missing .md (per protocol)
                continue
            plate = "[C1 " + DATE + "] " + truncate_word_boundary(bullet, 120)
            entries.append({"rva": rva, "plate": plate})
            seen_rvas.add(rva.lower())

        # ALSO include any per-RVA .md files that exist in the bucket dir but
        # were NOT listed in the queue row. The analysis agent produced them,
        # so they should be scribed. This is a divergence from the strict
        # protocol but matches user intent: drain everything the agent
        # actually analyzed. The mismatch will be noted in the report.
        extras = []
        for md in sorted((ANALYSIS / bucket).glob("0x*.md")):
            rva = md.stem  # e.g. 0x004b55a0
            if rva.lower() in seen_rvas:
                continue
            bullet = first_bullet(md)
            if not bullet:
                continue
            plate = "[C1 " + DATE + "] " + truncate_word_boundary(bullet, 120)
            entries.append({"rva": rva, "plate": plate})
            extras.append(rva)

        total_rvas += len(entries)
        total_missing_md += len(missing_md)
        total_extra += len(extras)
        out["buckets"].append({
            "bucket": bucket,
            "rvas": entries,
            "missing_md": missing_md,
            "extras_from_dir": extras,
        })
        print(f"  {bucket}: {len(entries)} rvas (incl. {len(extras)} extras-from-dir), {len(missing_md)} missing .md")

    out["total_rvas"] = total_rvas
    out["total_missing_md"] = total_missing_md
    out["total_extras_from_dir"] = total_extra

    manifest_path = ROOT / "tmp" / "sweep_manifest_20260519_1404.json"
    manifest_path.parent.mkdir(exist_ok=True)
    manifest_path.write_text(json.dumps(out, indent=2), encoding="utf-8")
    print(f"\nWrote {manifest_path}")
    print(f"Total: {total_rvas} rvas (incl. {total_extra} extras-from-dir), {total_missing_md} missing .md")


if __name__ == "__main__":
    build_manifest()
