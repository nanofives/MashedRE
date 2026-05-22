#!/usr/bin/env python3
"""Extract first bullet of '## Mechanical description' from each per-RVA .md.

Output: one TSV file per bucket: <rva>\t<plate_text_raw>
Plate text format: "[C1 2026-05-18] <bullet>" truncated at last word boundary <=120 chars with optional ...
"""
import os
import re
import sys

SESSION_DATE = "2026-05-18"
BASE = r"C:\Users\maria\Desktop\Proyectos\Mashed"
SWEEP = os.path.join(BASE, "tmp", "sweep")

BUCKETS = [
    "bucket_0057bf30",  # s5
    "bucket_00489940",  # s2
    "bucket_00412130",  # s1
    "bucket_004ddfb0",  # s3
    "bucket_005c1d63",  # s6
]


def extract_first_bullet(md_text: str) -> str:
    """Find '## Mechanical description' (any case) and return the first bullet."""
    # Find the heading line
    lines = md_text.splitlines()
    idx = None
    for i, ln in enumerate(lines):
        if re.match(r"^##\s+Mechanical\s+description", ln, re.IGNORECASE):
            idx = i
            break
    if idx is None:
        return ""
    # Look at subsequent lines for first bullet (- or *)
    bullet_lines = []
    in_bullet = False
    for j in range(idx + 1, len(lines)):
        ln = lines[j]
        if re.match(r"^##\s", ln):  # next section
            break
        m = re.match(r"^\s*[-*]\s+(.*)$", ln)
        if m and not in_bullet:
            in_bullet = True
            bullet_lines.append(m.group(1).rstrip())
            continue
        if in_bullet:
            # Continuation: indented non-bullet line
            if re.match(r"^\s*[-*]\s+", ln):
                break  # next bullet
            if ln.strip() == "":
                break  # blank line ends bullet
            if re.match(r"^\s{2,}\S", ln):
                bullet_lines.append(ln.strip())
                continue
            break
    return " ".join(bullet_lines).strip()


def truncate_plate(prefix: str, body: str, max_len: int = 120) -> str:
    # Ellipsis is "…" (1 codepoint). We want len(prefix+body[+ellipsis]) <= max_len.
    full = prefix + body
    if len(full) <= max_len:
        return full
    # Reserve 1 char for ellipsis
    budget = max_len - len(prefix) - 1
    if budget <= 0:
        return (prefix + "…")[:max_len]
    truncated = body[:budget]
    # Walk back to last whitespace (only if a space exists at all)
    sp = truncated.rfind(" ")
    if sp > 0:
        truncated = truncated[:sp]
    # Strip trailing punctuation/whitespace cleanly
    truncated = truncated.rstrip(" \t.,;:-")
    result = prefix + truncated + "…"
    # Defensive: if still over (unicode oddity), hard-cut
    if len(result) > max_len:
        result = result[: max_len - 1] + "…"
    return result


def main():
    for bucket in BUCKETS:
        rva_file = os.path.join(SWEEP, f"{bucket}.rvas")
        out_file = os.path.join(SWEEP, f"{bucket}.plates.tsv")
        bucket_dir = os.path.join(BASE, "re", "analysis", bucket)
        if not os.path.exists(rva_file):
            print(f"SKIP {bucket}: no rva file")
            continue
        with open(rva_file, encoding="utf-8") as fh:
            rvas = [ln.strip() for ln in fh if ln.strip()]
        out_rows = []
        missing = []
        for rva in rvas:
            md_path = os.path.join(bucket_dir, f"{rva}.md")
            if not os.path.exists(md_path):
                # try without 0x prefix
                bare = rva[2:] if rva.startswith("0x") else rva
                md_path = os.path.join(bucket_dir, f"{bare}.md")
                if not os.path.exists(md_path):
                    missing.append(rva)
                    continue
            with open(md_path, encoding="utf-8") as fh:
                txt = fh.read()
            bullet = extract_first_bullet(txt)
            if not bullet:
                bullet = "(no first bullet in Mechanical description)"
            plate = truncate_plate(f"[C1 {SESSION_DATE}] ", bullet, 120)
            # Sanitize: newlines/tabs -> spaces
            plate = re.sub(r"[\r\n\t]+", " ", plate)
            plate = re.sub(r"\s+", " ", plate).strip()
            out_rows.append((rva, plate))
        with open(out_file, "w", encoding="utf-8") as fh:
            for rva, plate in out_rows:
                fh.write(f"{rva}\t{plate}\n")
        print(f"{bucket}: {len(out_rows)} plates, {len(missing)} missing -> {out_file}")
        if missing:
            for m in missing:
                print(f"  missing: {m}")


if __name__ == "__main__":
    main()
