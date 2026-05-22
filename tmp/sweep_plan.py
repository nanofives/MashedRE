"""Dump a JSON work plan for the sweep: per bucket -> [(rva, plate_text)].

Parses re/SCRIBE_QUEUE.md, finds Queued rows, and resolves plate text for each
RVA via sweep_helper.first_bullet_of_mech_desc + truncate.
"""

from __future__ import annotations
import json
import os
import re
import sys
import importlib.util

ROOT = os.path.abspath(os.path.join(os.path.dirname(__file__), ".."))
QUEUE_PATH = os.path.join(ROOT, "re", "SCRIBE_QUEUE.md")

# Load sweep_helper
spec = importlib.util.spec_from_file_location("sweep_helper", os.path.join(os.path.dirname(__file__), "sweep_helper.py"))
sweep_helper = importlib.util.module_from_spec(spec)
spec.loader.exec_module(sweep_helper)


def parse_queued_rows(text: str):
    """Return list of {bucket, rvas[], level, pool, note_preview} from Queued section."""
    # Slice between "## Queued" and "## Drained"
    m_q = re.search(r"^##\s+Queued\s*$", text, flags=re.MULTILINE)
    m_d = re.search(r"^##\s+Drained\s*$", text, flags=re.MULTILINE)
    if not m_q:
        return []
    body = text[m_q.end(): m_d.start() if m_d else len(text)]
    rows = []
    for line in body.splitlines():
        line = line.strip()
        if not line or line.startswith("#"):
            continue
        if " bucket=" not in line:
            continue
        # Parse: <date>  <sid>  bucket=<bucket>  rvas=<list>  level=<l>  pool=<p>  ...
        # Use regex
        m_b = re.search(r"\bbucket=([^\s]+)", line)
        m_r = re.search(r"\brvas=([^\s]+)", line)
        m_l = re.search(r"\blevel=([^\s]+)", line)
        m_p = re.search(r"\bpool=([^\s]+)", line)
        if not (m_b and m_r):
            continue
        bucket = m_b.group(1)
        rvas_raw = m_r.group(1)
        if rvas_raw.startswith("HALTED"):
            rvas = []
        else:
            rvas = [x.strip() for x in rvas_raw.split(",") if x.strip()]
        rows.append({
            "bucket": bucket,
            "rvas": rvas,
            "level": m_l.group(1) if m_l else "",
            "pool": m_p.group(1) if m_p else "",
            "row_line": line,
        })
    return rows


def main():
    with open(QUEUE_PATH, "r", encoding="utf-8") as f:
        text = f.read()
    rows = parse_queued_rows(text)
    plan = []
    missing = []
    for row in rows:
        bucket = row["bucket"]
        bucket_dir = os.path.join(ROOT, "re", "analysis", bucket)
        bucket_entry = {
            "bucket": bucket,
            "bucket_dir": bucket_dir.replace("\\", "/"),
            "level": row["level"],
            "pool": row["pool"],
            "rvas": [],
        }
        for rva in row["rvas"]:
            rva_clean = rva.lower().lstrip("0x").rjust(8, "0")
            plate_path = sweep_helper.find_plate(bucket_dir, rva_clean)
            if not plate_path:
                plate_path = sweep_helper.find_plate(bucket_dir, rva.lower())
            if not plate_path:
                missing.append({"bucket": bucket, "rva": rva})
                bucket_entry["rvas"].append({"rva": rva, "plate_text": None, "missing": True})
                continue
            bullet = sweep_helper.first_bullet_of_mech_desc(plate_path)
            if not bullet:
                missing.append({"bucket": bucket, "rva": rva, "reason": "no-first-bullet"})
                bucket_entry["rvas"].append({"rva": rva, "plate_text": None, "missing": True})
                continue
            text_full = f"[C1 {sweep_helper.TODAY}] {bullet}"
            text_trunc = sweep_helper.truncate_at_word(text_full)
            bucket_entry["rvas"].append({
                "rva": rva,
                "plate_text": text_trunc,
                "plate_file": os.path.basename(plate_path),
            })
        plan.append(bucket_entry)
    out = {"plan": plan, "missing": missing}
    out_path = os.path.join(os.path.dirname(__file__), "sweep_plan.json")
    with open(out_path, "w", encoding="utf-8") as f:
        json.dump(out, f, ensure_ascii=False, indent=1)
    print(f"wrote {out_path}")


if __name__ == "__main__":
    main()
