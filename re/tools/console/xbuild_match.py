#!/usr/bin/env python3
r"""xbuild_match.py — match functions between PC MASHED.exe and Xbox toast.exe.

Same source, same ISA (x86), different link layout. Normalizes function bodies
by masking relocation-sensitive bytes, then matches masked-SHA1 hashes:

  tier "full"   — full-body masked hash, unique on both sides
  tier "prefix" — first-64-byte masked hash, unique on both sides, sizes within 25%

Masking per binary (with its own VA range):
  - E8/E9 rel32 whose target lands inside the image  -> rel32 zeroed
  - any 4-byte LE dword whose value lies in the VA range -> zeroed

Inputs (defaults match the repo layout):
  re/console/pc_funcs.csv      DumpFunctions.java output for MASHED.exe.unpatched
  re/console/xbox/toast_funcs.csv                  ... for toast_flat.bin
  original/MASHED.exe.unpatched, re/console/xbox/toast_flat.bin(.json)
  hooks.csv                    names/subsystems joined onto PC side

Output: re/console/match/xbuild_match.csv + summary to stdout.
"""
import csv
import hashlib
import json
import struct
import sys
from collections import Counter, defaultdict

ROOT = "."
PC_EXE = f"{ROOT}/original/MASHED.exe.unpatched"
PC_FUNCS = f"{ROOT}/re/console/pc_funcs.csv"
XB_FLAT = f"{ROOT}/re/console/xbox/toast_flat.bin"
XB_META = f"{ROOT}/re/console/xbox/toast_flat.json"
XB_FUNCS = f"{ROOT}/re/console/xbox/toast_funcs.csv"
HOOKS = f"{ROOT}/hooks.csv"
OUT = f"{ROOT}/re/console/match/xbuild_match.csv"

MIN_SIZE = 16
MAX_SIZE = 0x4000
PREFIX = 64


class PcImage:
    """PE VA->bytes via section table."""

    def __init__(self, path):
        self.data = open(path, "rb").read()
        pe = struct.unpack_from("<I", self.data, 0x3C)[0]
        nsec = struct.unpack_from("<H", self.data, pe + 6)[0]
        opt_size = struct.unpack_from("<H", self.data, pe + 20)[0]
        self.base = struct.unpack_from("<I", self.data, pe + 24 + 28)[0]
        size_of_image = struct.unpack_from("<I", self.data, pe + 24 + 56)[0]
        self.va_lo, self.va_hi = self.base, self.base + size_of_image
        sec0 = pe + 24 + opt_size
        self.secs = []
        for i in range(nsec):
            s = sec0 + i * 40
            vsize, va, rawsize, raw = struct.unpack_from("<IIII", self.data, s + 8)
            self.secs.append((self.base + va, vsize, raw, rawsize))

    def read(self, va, size):
        for sva, vsize, raw, rawsize in self.secs:
            if sva <= va < sva + vsize:
                off = raw + (va - sva)
                avail = max(0, min(size, rawsize - (va - sva)))
                out = self.data[off:off + avail]
                return out + b"\x00" * (size - len(out))
        return b"\x00" * size


class XbImage:
    """Flat image: VA - base = offset."""

    def __init__(self, bin_path, meta_path):
        self.data = open(bin_path, "rb").read()
        meta = json.load(open(meta_path))
        self.base = meta["image_base"]
        self.va_lo, self.va_hi = self.base, self.base + meta["image_size"]
        text = next(s for s in meta["sections"] if s["name"] == ".text")
        self.text_lo, self.text_hi = text["va"], text["va"] + text["vsize"]

    def read(self, va, size):
        off = va - self.base
        out = self.data[off:off + size]
        return out + b"\x00" * (size - len(out))


def mask(body, va, lo, hi):
    b = bytearray(body)
    n = len(b)
    i = 0
    while i + 5 <= n:
        if b[i] in (0xE8, 0xE9):
            rel = struct.unpack_from("<i", b, i + 1)[0]
            tgt = (va + i + 5 + rel) & 0xFFFFFFFF
            if lo <= tgt < hi:
                b[i + 1:i + 5] = b"\x00\x00\x00\x00"
                i += 5
                continue
        i += 1
    i = 0
    while i + 4 <= n:
        v = struct.unpack_from("<I", b, i)[0]
        if lo <= v < hi and v != 0:
            b[i:i + 4] = b"\x00\x00\x00\x00"
            i += 4
        else:
            i += 1
    return bytes(b)


def load_funcs(path):
    out = []
    with open(path, newline="") as f:
        for row in csv.DictReader(f):
            entry = int(row["entry"], 16)
            end = int(row["body_end"], 16)
            span = end - entry + 1
            if MIN_SIZE <= span <= MAX_SIZE:
                out.append((entry, span, row["name"]))
    return out


def hash_side(funcs, img):
    full, prefix = defaultdict(list), defaultdict(list)
    for entry, span, name in funcs:
        body = img.read(entry, span)
        m = mask(body, entry, img.va_lo, img.va_hi)
        full[hashlib.sha1(m).digest()].append((entry, span))
        prefix[hashlib.sha1(m[:PREFIX]).digest()].append((entry, span))
    return full, prefix


def load_hooks():
    info = {}
    with open(HOOKS, newline="", encoding="utf-8", errors="replace") as f:
        for row in csv.reader(f):
            if not row or row[0].startswith("#") or row[0] == "rva":
                continue
            try:
                va = int(row[0], 16)
            except ValueError:
                continue
            info[va] = (row[1], row[2], row[3])  # name, subsystem, confidence
    return info


def main():
    pc_img = PcImage(PC_EXE)
    xb_img = XbImage(XB_FLAT, XB_META)
    pc = load_funcs(PC_FUNCS)
    xb = load_funcs(XB_FUNCS)
    print(f"PC functions in range: {len(pc)}   Xbox: {len(xb)}")

    pc_full, pc_pref = hash_side(pc, pc_img)
    xb_full, xb_pref = hash_side(xb, xb_img)

    matches = {}  # pc_entry -> (xb_entry, span, tier)
    used_xb = set()
    for h, pcs in pc_full.items():
        xbs = xb_full.get(h)
        if xbs and len(pcs) == 1 and len(xbs) == 1:
            matches[pcs[0][0]] = (xbs[0][0], pcs[0][1], "full")
            used_xb.add(xbs[0][0])
    for h, pcs in pc_pref.items():
        if len(pcs) != 1 or pcs[0][0] in matches:
            continue
        xbs = xb_pref.get(h)
        if not xbs or len(xbs) != 1 or xbs[0][0] in used_xb:
            continue
        (pe, ps), (xe, xs) = pcs[0], xbs[0]
        if ps >= PREFIX and xs >= PREFIX and abs(ps - xs) <= 0.25 * max(ps, xs):
            matches[pe] = (xe, ps, "prefix")
            used_xb.add(xe)

    hooks = load_hooks()
    import os
    os.makedirs(f"{ROOT}/re/console/match", exist_ok=True)
    by_sub, by_tier = Counter(), Counter()
    in_text = 0
    with open(OUT, "w", newline="") as f:
        w = csv.writer(f)
        w.writerow(["pc_va", "xbox_va", "size", "tier", "xbox_in_text",
                    "pc_name", "subsystem", "confidence"])
        for pe in sorted(matches):
            xe, span, tier = matches[pe]
            name, sub, conf = hooks.get(pe, ("", "", ""))
            txt = xb_img.text_lo <= xe < xb_img.text_hi
            in_text += txt
            by_tier[tier] += 1
            if sub:
                by_sub[sub] += 1
            w.writerow([f"0x{pe:08x}", f"0x{xe:08x}", span, tier, int(txt),
                        name, sub, conf])

    print(f"matched: {len(matches)}  (full={by_tier['full']}, prefix={by_tier['prefix']})")
    print(f"matches landing in Xbox .text (game code): {in_text}")
    print(f"matches joined to hooks.csv rows: {sum(by_sub.values())}")
    print("top subsystems:")
    for sub, n in by_sub.most_common(15):
        print(f"  {sub:45} {n}")
    print(f"wrote {OUT}")


if __name__ == "__main__":
    main()
