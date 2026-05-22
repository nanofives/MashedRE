#!/usr/bin/env python3
"""Extract plate text for each .md file in the bucket and write to a JSONL."""
import os
import re
import json

BUCKET_DIR = r"C:/Users/maria/Desktop/Proyectos/Mashed/re/analysis/bucket_0041dc30/"
OUT = r"C:/Users/maria/Desktop/Proyectos/Mashed/tmp/plates_0041dc30.jsonl"

def extract_plate(path):
    with open(path, 'r', encoding='utf-8') as f:
        content = f.read()
    # Find "## Mechanical description" section
    m = re.search(r"##\s*Mechanical description\s*\n(.+?)(?=\n##|\Z)", content, re.DOTALL)
    if not m:
        return None
    section = m.group(1).strip()
    # First content unit
    lines = section.split('\n')
    first_line = lines[0].strip()
    if first_line.startswith('-') or first_line.startswith('*'):
        text = re.sub(r'^[-*]\s*', '', first_line).strip()
    else:
        # paragraph - first sentence up to ". " or whole line
        # Collect until blank line or section end
        para_lines = []
        for line in lines:
            if not line.strip():
                break
            para_lines.append(line.strip())
        para = ' '.join(para_lines)
        dot_idx = para.find('. ')
        if dot_idx != -1:
            text = para[:dot_idx + 1]
        else:
            text = para
    return text

def make_plate(text):
    prefix = "[C1 2026-05-20] "
    full = prefix + text
    if len(full) <= 120:
        return full
    # Truncate at last word boundary <= 120 chars (including ellipsis)
    # We need space for the ellipsis char "…" (1 char)
    limit = 120 - 1
    truncated = full[:limit]
    # last space
    sp = truncated.rfind(' ')
    if sp > len(prefix):
        truncated = truncated[:sp]
    return truncated + "…"

results = []
for fn in sorted(os.listdir(BUCKET_DIR)):
    if not fn.endswith('.md'):
        continue
    rva = fn[:-3]  # strip .md
    path = os.path.join(BUCKET_DIR, fn)
    text = extract_plate(path)
    if text is None:
        results.append({"rva": rva, "plate": None, "error": "no Mechanical description"})
        continue
    plate = make_plate(text)
    results.append({"rva": rva, "plate": plate})

with open(OUT, 'w', encoding='utf-8') as f:
    for r in results:
        f.write(json.dumps(r) + '\n')

print(f"Wrote {len(results)} plates to {OUT}")
for r in results[:5]:
    print(r)
print("...")
for r in results[-3:]:
    print(r)
