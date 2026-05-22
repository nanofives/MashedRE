#!/usr/bin/env python3
"""Extract first bullet of '## Mechanical description' from each .md plate file.
Outputs lines: <rva>\t<plate_text>
where plate_text is prefixed [C2 2026-05-20] and truncated <=120 chars at word boundary.
"""
import sys, os, re, json

BUCKETS = [
    ("re/analysis/promote_c1_low_ab1",
     "0x00402fb0,0x0040ce80,0x00417740,0x004215c0,0x004277a0,0x00427e00,0x00428760,0x00430790,0x0045ba00,0x0046c5c0,0x0046c790,0x00472c60,0x00472f40"),
    ("re/analysis/promote_c1_mid_ab2",
     "0x004730b0,0x00473870,0x004739f0,0x00473c20,0x00473ee0,0x00474890,0x00493f70,0x00493f80,0x00493fc0,0x00493fd0,0x00494460,0x00494480,0x00494fd0"),
    ("re/analysis/promote_c1_high_ab3",
     "0x00494a80,0x004967e0,0x004a2b60,0x004a2c48,0x004b5750,0x004c1a00,0x004c1bb0,0x004c1be0,0x004d7ff0,0x004d8480,0x00552d70,0x005555b0,0x00556e90"),
]

PREFIX = "[C2 2026-05-20] "
MAXLEN = 120

def first_bullet(md_text):
    # Find the "## Mechanical description" section
    m = re.search(r"^##\s*Mechanical description\s*$", md_text, re.MULTILINE | re.IGNORECASE)
    if not m:
        # fallback: try without "## " just the header
        m = re.search(r"Mechanical description\s*$", md_text, re.MULTILINE | re.IGNORECASE)
        if not m:
            return None
    after = md_text[m.end():]
    # Walk lines until first non-empty bullet line
    lines = after.splitlines()
    bullet = None
    for line in lines:
        s = line.strip()
        if not s:
            continue
        if s.startswith("- ") or s.startswith("* "):
            bullet = s[2:].strip()
            # continue collecting continuation lines (indented or not starting with new bullet/header)
            break
        else:
            # ran into prose before a bullet — use this line itself
            if s.startswith("#"):
                return None
            bullet = s
            break
    if bullet is None:
        return None
    return bullet

def truncate(text, maxlen):
    if len(text) <= maxlen:
        return text
    # find last whitespace boundary at or before maxlen-1 (leave room for ...)
    cut = text.rfind(" ", 0, maxlen - 1)
    if cut <= 0:
        cut = maxlen - 1
    return text[:cut].rstrip() + "…"

def main():
    out = []
    for bucket, rvas_str in BUCKETS:
        rvas = rvas_str.split(",")
        for rva in rvas:
            rva_lc = rva.lower()
            path = os.path.join(bucket, f"{rva_lc}.md")
            if not os.path.exists(path):
                # try without 0x
                alt = os.path.join(bucket, f"{rva_lc[2:]}.md")
                if os.path.exists(alt):
                    path = alt
                else:
                    out.append({"bucket": bucket, "rva": rva, "status": "MISSING_MD", "plate": None})
                    continue
            with open(path, "r", encoding="utf-8") as f:
                txt = f.read()
            bullet = first_bullet(txt)
            if bullet is None:
                out.append({"bucket": bucket, "rva": rva, "status": "NO_BULLET", "plate": None})
                continue
            # Strip markdown link/bold etc — keep raw text
            # Per skill: no paraphrasing, only truncation. So just truncate.
            full = PREFIX + bullet
            plated = truncate(full, MAXLEN)
            out.append({"bucket": bucket, "rva": rva, "status": "OK", "plate": plated})
    print(json.dumps(out, indent=2, ensure_ascii=False))

if __name__ == "__main__":
    main()
