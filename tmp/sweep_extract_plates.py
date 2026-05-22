#!/usr/bin/env python3
"""Extract plate text (first bullet/line under ## Mechanical description) per RVA.

Outputs JSON: {rva_hex: {"text": "[C1 2026-05-18] truncated...", "file": "path", "exists": true}}
"""
import json
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
ANALYSIS = ROOT / "re" / "analysis"

BUCKETS = {
    "cluster_005b_first_pass": [
        "0x005b0360,0x005b03e0,0x005b04b0,0x005b0700,0x005b0740,0x005b0970,0x005b09c0,0x005b0a90,0x005b0b10,0x005b0b40,"
        "0x005b0b60,0x005b0b90,0x005b0bb0,0x005b0c70,0x005b0ca0,0x005b0cf0,0x005b0dc0,0x005b0df0,0x005b0e70,0x005b0ec0,"
        "0x005b0f10,0x005b0f40,0x005b0f90,0x005b1030,0x005b1080,0x005b10a0,0x005b10d0,0x005b10e0,0x005b1110,0x005b1140,"
        "0x005b1160,0x005b1180,0x005b11d0,0x005b11f0,0x005b1260,0x005b1500,0x005b15d0,0x005b2080,0x005b2190,0x005b2220,"
        "0x005b2820,0x005b29e0,0x005b2b00,0x005b2de0,0x005b2fd0,0x005b3020,0x005b30e0,0x005b3300,0x005b34f0,0x005b3540,"
        "0x005b3550,0x005b3580,0x005b35a0,0x005b35e0,0x005b3620,0x005b3670,0x005b36b0,0x005b3e60,0x005b4060,0x005b4150"
    ],
    "cluster_004f_first_pass": [
        "0x004f022d,0x004f0307,0x004f07a0,0x004f07d0,0x004f0800,0x004f0810,0x004f0900,0x004f0910,0x004f0940,0x004f0970,"
        "0x004f0ae0,0x004f0bd0,0x004f0c10,0x004f0c50,0x004f0d80,0x004f0dc0,0x004f0f10,0x004f10e0,0x004f1130,0x004f1150,"
        "0x004f13d0,0x004f1800,0x004f1870,0x004f1b00,0x004f1f00,0x004f1f90,0x004f1fb0,0x004f2570,0x004f2e70,0x004f2f90,"
        "0x004f3030,0x004f3150,0x004f3440,0x004f36c0,0x004f3b00,0x004f3b60,0x004f3bc0,0x004f3bd0,0x004f3be0,0x004f3cb0,"
        "0x004f3ce0,0x004f3d40,0x004f3e90,0x004f4200,0x004f4270,0x004f42d0,0x004f4340,0x004f43b0,0x004f43f0,0x004f4470,"
        "0x004f4510,0x004f46a0,0x004f4900,0x004f5020,0x004f5030,0x004f57e0,0x004f6870,0x004f71f0,0x004f77f0,0x004f7850"
    ],
    "cluster_0055_first_pass": [
        "0x00550400,0x005504d0,0x00550520,0x00550580,0x00550670,0x00550740,0x00550750,0x005507a0,0x00550a20,0x00550bd0,"
        "0x00551090,0x00551190,0x005512b0,0x00551330,0x00551410,0x00551460,0x00551550,0x00551840,0x00551a50,0x00551ad0,"
        "0x00551c40,0x00551ca0,0x00551ce0,0x00551cf0,0x00551d40,0x00551fd0,0x00551fe0,0x00552020,0x00552230,0x005522e0,"
        "0x005524a0,0x00552720,0x00552890,0x00552920,0x00552d70,0x00552fa0,0x00553030,0x005530f0,0x005531f0,0x005533d0,"
        "0x00553590,0x00553620,0x005538a0,0x00553c50,0x00553cf0,0x00553e00,0x00553e80,0x00553ef0,0x00554010,0x00554150,"
        "0x00554200,0x00554390,0x005555b0,0x00555830,0x00556780,0x00556e40,0x00556e90,0x00557110,0x005572b0,0x005572c0"
    ],
    "cluster_0048_first_pass": [
        "0x004840b0,0x00484280,0x004842b0,0x00484310,0x004844a0,0x00484580,0x004847d0,0x00484a50,0x00484ac0,0x00484cf0,"
        "0x00484de0,0x00484f70,0x004850b0,0x004850e0,0x004852e0,0x004853b0,0x004853f0,0x00485460,0x004854e0,0x004858c0,"
        "0x00485a00,0x00485a70,0x00485b30,0x00485bd0,0x00485bf0,0x00485c20,0x00485d90,0x00485e10,0x00485e50,0x00485ef0,"
        "0x00486220,0x00486270,0x004862d0,0x00486350,0x00486370,0x00486460,0x004864f0,0x00486610,0x00486830,0x00486f50,"
        "0x00486f90,0x00487140,0x00487150,0x00487d00,0x00487d60,0x00487d70,0x00487de0,0x00487df0,0x00487e00,0x00487e20,"
        "0x00487e30,0x00487e40,0x00487e70,0x004880a0,0x00488320,0x00488390,0x004883b0,0x00489240,0x00489290,0x004892c0"
    ],
}

# Concat and split:
for k, v in BUCKETS.items():
    BUCKETS[k] = ",".join(v).replace(" ", "").split(",")


def find_md(bucket: str, rva: str) -> Path | None:
    """Try both 0x-prefixed and bare-hex filenames."""
    dirp = ANALYSIS / bucket
    bare = rva.replace("0x", "").lower()
    for cand in (f"{rva.lower()}.md", f"{bare}.md", f"0x{bare}.md"):
        p = dirp / cand
        if p.exists():
            return p
    return None


def extract_first_bullet(md_text: str) -> str | None:
    """Return the first non-empty content line under ## Mechanical description.

    If first line is a markdown list item ('- foo'), strip the leading '- '.
    Otherwise return the line verbatim.
    """
    in_section = False
    for raw in md_text.splitlines():
        line = raw.rstrip()
        if not in_section:
            if line.strip().lower().startswith("## mechanical description"):
                in_section = True
            continue
        # in_section
        if line.startswith("## "):
            return None
        s = line.strip()
        if not s:
            continue
        if s.startswith("- "):
            return s[2:].strip()
        return s
    return None


def truncate_to_120(prefix: str, body: str) -> str:
    full = prefix + body
    if len(full) <= 120:
        return full
    # truncate body so prefix+body+"…" <= 120
    avail = 120 - len(prefix) - 1  # -1 for ellipsis
    cut = body[:avail]
    # last word boundary
    sp = cut.rfind(" ")
    if sp > 20:
        cut = cut[:sp]
    return prefix + cut.rstrip() + "…"


def main():
    out = {}
    prefix = "[C1 2026-05-18] "
    for bucket, rvas in BUCKETS.items():
        for rva in rvas:
            rva_l = rva.lower()
            md = find_md(bucket, rva_l)
            if not md:
                out[rva_l] = {
                    "bucket": bucket,
                    "exists": False,
                    "file": None,
                    "text": None,
                    "error": "missing .md",
                }
                continue
            body = extract_first_bullet(md.read_text(encoding="utf-8", errors="replace"))
            if not body:
                out[rva_l] = {
                    "bucket": bucket,
                    "exists": True,
                    "file": str(md),
                    "text": None,
                    "error": "no Mechanical description body",
                }
                continue
            plate = truncate_to_120(prefix, body)
            out[rva_l] = {
                "bucket": bucket,
                "exists": True,
                "file": str(md),
                "text": plate,
                "raw_first": body,
            }
    outp = Path(__file__).resolve().parent / "sweep_plates.json"
    outp.write_text(json.dumps(out, indent=2), encoding="utf-8")
    # Print summary
    missing = [r for r, d in out.items() if not d["exists"]]
    no_body = [r for r, d in out.items() if d["exists"] and not d.get("text")]
    print(f"Total RVAs: {len(out)}")
    print(f"Missing .md: {len(missing)}")
    if missing:
        print("  ", missing[:10])
    print(f"No mechanical-description body: {len(no_body)}")
    if no_body:
        print("  ", no_body[:10])
    print(f"Written to: {outp}")


if __name__ == "__main__":
    main()
