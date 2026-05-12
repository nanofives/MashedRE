"""Read a bucket's per-RVA .md files and emit one plate per RVA.

Usage: py -3.12 re/tools/sweep_plate_gen.py <bucket_dir> <date>

Output: <rva>\t<confidence>\t<plate>\n  (one row per .md file)

Plate format: "[<C-level> <date>] <first paragraph of ## Purpose>", truncated
at last word boundary <= 120 chars total, with "..." appended if cut.

Notes:
- Files come in two name conventions: 0x<rva>.md OR <rva>_FUN_<rva>.md.
- Section header may be "## Mechanical description" (canonical) or "## Purpose"
  (some analysis sessions used this synonym). We accept either.
- Confidence and rva come from YAML front-matter.
"""
import os
import re
import sys
import yaml

sys.stdout.reconfigure(encoding="utf-8")


def parse_md(path):
    txt = open(path, encoding="utf-8").read()
    # Front matter
    m = re.match(r"^---\n(.*?)\n---\n", txt, re.S)
    if not m:
        return None
    fm_block = m.group(1)
    # Try YAML; fall back to regex if it has a quirk.
    fm = {}
    try:
        parsed = yaml.safe_load(fm_block)
        if isinstance(parsed, dict):
            fm = parsed
    except Exception:
        pass
    # Always regex-extract the two fields we need, to tolerate quirky values.
    rm = re.search(r"^rva:\s*(\S+)", fm_block, re.M)
    if rm:
        fm["rva"] = rm.group(1)
    cm = re.search(r"^confidence:\s*(\S+)", fm_block, re.M)
    if cm:
        fm["confidence"] = cm.group(1)
    body = txt[m.end():]
    # Find Purpose or Mechanical description section.
    # Match the entire header line (handles "## Mechanical description (full decomp)")
    # then start the body after the trailing newline.
    sect = None
    for header in ("## Mechanical description", "## Purpose"):
        m2 = re.search(r"^" + re.escape(header) + r".*\n", body, re.M)
        if not m2:
            continue
        sub = body[m2.end():]
        nxt = sub.find("\n## ")
        if nxt != -1:
            sub = sub[:nxt]
        sect = sub.strip()
        break
    if not sect:
        return None
    # First "bullet" — for bulleted lists, the first '- ' line; for paragraphs,
    # the first paragraph until blank line.
    first = None
    lines = sect.splitlines()
    # If first non-blank starts with "- ", take that one line.
    for i, ln in enumerate(lines):
        if ln.strip().startswith("- "):
            first = ln.strip()[2:].strip()
            break
        if ln.strip():
            # Paragraph: collect lines until blank
            para = []
            for ln2 in lines[i:]:
                if not ln2.strip():
                    break
                para.append(ln2.strip())
            first = " ".join(para)
            break
    if not first:
        return None
    return fm, first


def truncate_at_word(text, limit):
    if len(text) <= limit:
        return text, False
    cut = text[: limit - 1]  # leave room for ellipsis
    sp = cut.rfind(" ")
    if sp > 0:
        cut = cut[:sp]
    return cut + "…", True


def main():
    bucket_dir = sys.argv[1]
    date = sys.argv[2]
    files = sorted(
        f for f in os.listdir(bucket_dir)
        if f.endswith(".md") and re.match(r"^(0x)?[0-9a-fA-F]+", f)
    )
    for f in files:
        full = os.path.join(bucket_dir, f)
        result = parse_md(full)
        if not result:
            sys.stderr.write(f"SKIP {f}: no parseable purpose section\n")
            continue
        fm, first = result
        rva = fm.get("rva")
        if isinstance(rva, int):
            rva = f"0x{rva:08x}"
        if isinstance(rva, str) and not rva.startswith("0x"):
            # Convert decimal-looking string from YAML to hex
            try:
                rva = f"0x{int(rva):08x}"
            except ValueError:
                pass
        conf = fm.get("confidence", "C1")
        prefix = f"[{conf} {date}] "
        plate, cut = truncate_at_word(prefix + first, 120)
        # Print RVA\tconfidence\tplate
        sys.stdout.write(f"{rva}\t{conf}\t{plate}\n")


if __name__ == "__main__":
    main()
