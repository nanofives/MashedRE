#!/usr/bin/env python3
r"""xbe_flatten.py — flatten an XBE's sections into a raw memory image.

Produces <out>.bin (every section copied to its VA minus image base, header
page at offset 0) and <out>.json (section map + decoded entry point), ready
for Ghidra BinaryLoader import at the image base.

Retail XOR keys: entry 0xA8FC57AB, kernel thunk 0x5B6D40B6.

Usage:
  py -3.12 re/tools/console/xbe_flatten.py re/console/xbox/default.xbe re/console/xbox/toast_flat
"""
import json
import struct
import sys

ENTRY_XOR_RETAIL = 0xA8FC57AB
KTHUNK_XOR_RETAIL = 0x5B6D40B6


def main():
    xbe_path, out_base = sys.argv[1], sys.argv[2]
    f = open(xbe_path, "rb").read()
    assert f[:4] == b"XBEH", "not an XBE"
    base = struct.unpack_from("<I", f, 0x104)[0]
    hdr_size = struct.unpack_from("<I", f, 0x108)[0]
    nsec, sec_hdr_addr = struct.unpack_from("<II", f, 0x11C)
    entry_enc = struct.unpack_from("<I", f, 0x128)[0]
    kthunk_enc = struct.unpack_from("<I", f, 0x158)[0]

    def cstr(va):
        off = va - base
        return f[off:f.index(b"\x00", off)].decode("latin-1")

    sections = []
    top = 0
    off = sec_hdr_addr - base
    for i in range(nsec):
        sh = f[off + i * 0x38: off + (i + 1) * 0x38]
        flags, vaddr, vsize, raw, rawsize, name_addr = struct.unpack_from("<6I", sh)
        sections.append({
            "name": cstr(name_addr), "va": vaddr, "vsize": vsize,
            "raw": raw, "rawsize": rawsize, "flags": flags,
        })
        top = max(top, vaddr + vsize)

    img = bytearray(top - base)
    img[0:hdr_size] = f[0:hdr_size]  # XBE header page(s) live at the image base
    for s in sections:
        dst = s["va"] - base
        img[dst:dst + s["rawsize"]] = f[s["raw"]:s["raw"] + s["rawsize"]]

    with open(out_base + ".bin", "wb") as o:
        o.write(img)
    meta = {
        "image_base": base,
        "image_size": len(img),
        "entry_va": entry_enc ^ ENTRY_XOR_RETAIL,
        "kernel_thunk_va": kthunk_enc ^ KTHUNK_XOR_RETAIL,
        "sections": sections,
    }
    with open(out_base + ".json", "w") as o:
        json.dump(meta, o, indent=2)
    print(f"wrote {out_base}.bin ({len(img)} bytes, base {base:#x})")
    print(f"entry={meta['entry_va']:#x} kthunk={meta['kernel_thunk_va']:#x}")
    for s in sections:
        print(f"  {s['name']:12} va={s['va']:#10x} vsize={s['vsize']:<9} raw={s['rawsize']}")


if __name__ == "__main__":
    main()
