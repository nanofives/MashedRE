#!/usr/bin/env python3
"""Extract plate text from each .md file in bucket_0052df70."""
import os
import re
import json
import sys

BUCKET_DIR = r"C:/Users/maria/Desktop/Proyectos/Mashed/re/analysis/bucket_0052df70/"
DATE_PREFIX = "[C1 2026-05-20] "

def extract_first_unit(md_text):
    """Find ## Mechanical description, then first bullet or first sentence."""
    m = re.search(r"##\s+Mechanical description\s*\n(.*?)(?:\n##\s|\Z)", md_text, re.S)
    if not m:
        return None
    body = m.group(1).strip()
    if not body:
        return None
    lines = body.split("\n")
    # find first non-empty line
    first = None
    for ln in lines:
        s = ln.strip()
        if s:
            first = s
            break
    if first is None:
        return None
    if first.startswith("-") or first.startswith("*"):
        # bullet - capture this line plus continuation lines (lines that don't start with - or *)
        idx = lines.index(ln)
        collected = [first.lstrip("-* \t")]
        for j in range(idx + 1, len(lines)):
            nxt = lines[j]
            ns = nxt.strip()
            if not ns:
                break
            if ns.startswith("-") or ns.startswith("*"):
                break
            collected.append(ns)
        text = " ".join(collected).strip()
        return text
    else:
        # first sentence: up to first '. ' or end
        # but must continue across lines until paragraph break
        para_lines = []
        idx = lines.index(ln)
        for j in range(idx, len(lines)):
            ns = lines[j].strip()
            if not ns:
                break
            para_lines.append(ns)
        para = " ".join(para_lines).strip()
        # first sentence
        sm = re.search(r"^(.*?\.)(\s|$)", para)
        if sm:
            return sm.group(1).strip()
        return para

def truncate_120(text):
    if len(text) <= 120:
        return text
    # truncate at last word boundary <= 120 (chars before "…")
    # need room for the ellipsis. Cut at <= 119 chars then add …
    cut = text[:119]
    sp = cut.rfind(" ")
    if sp > 0:
        cut = cut[:sp]
    return cut.rstrip() + "…"

def build_plate(text):
    full = DATE_PREFIX + text
    return truncate_120(full)

def main():
    files = sorted(os.listdir(BUCKET_DIR))
    out = []
    for fname in files:
        if not fname.endswith(".md"):
            continue
        m = re.match(r"0x([0-9a-fA-F]+)\.md$", fname)
        if not m:
            continue
        rva = "0x" + m.group(1).lower()
        path = os.path.join(BUCKET_DIR, fname)
        try:
            with open(path, "r", encoding="utf-8") as f:
                txt = f.read()
        except Exception as e:
            out.append({"rva": rva, "error": f"read failed: {e}"})
            continue
        unit = extract_first_unit(txt)
        if unit is None:
            out.append({"rva": rva, "error": "no mechanical description"})
            continue
        plate = build_plate(unit)
        out.append({"rva": rva, "plate": plate, "raw": unit})
    print(json.dumps(out, indent=2))

if __name__ == "__main__":
    main()
