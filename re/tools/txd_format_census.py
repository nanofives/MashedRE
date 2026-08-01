#!/usr/bin/env python3
"""Census every TXD in the game: which pixel formats does Mashed actually ship?

Lane M3-E2'a (gate D2 / librw). Before writing the Txd::Mip -> rw::Raster bridge
we need to know the FULL set of (depth, palette, stride) combinations the raster
bridge must handle -- not the subset the Frontend TXD happens to use. Mashed's
TXD is a proprietary chunk-id 0x23 container (mashedmod/src/mashed_re/Txd/
TxdDecoder.h:2), so librw's own TXD reader is never involved; we decode and hand
librw finished rasters.

Format decoded here is exactly TxdDecoder.h:6-27 (source of truth: FUN_0054f8d0,
deviceId != 0 branch):

    ROOT   12B RW chunk header: id=0x23, size, version
    +0x0C   4B numTex (uint16) | deviceId (uint16)
    per texture:
             4B numMips
      per mip:
            12B IMAGE  chunk header (id=0x18)
            12B STRUCT chunk header (id=1, size=0x10)
            16B width, height, depth, stride  (uint32 x4)
            stride*height  pixel bytes
            (1<<depth)*4   palette bytes, ONLY when depth < 9
            12B TEXTURE chunk header (id=6) + subchunks (name, mask, extension)

Only headers are walked; pixel payloads are skipped, so this is fast.

Usage:
    py -3.12 re/tools/txd_format_census.py [--root original/TOASTART] [--json OUT]
"""

from __future__ import annotations

import argparse
import json
import struct
import sys
from collections import Counter, defaultdict
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
from piz_extract import read_archive  # noqa: E402

ROOT_CHUNK_ID = 0x23
IMAGE_CHUNK_ID = 0x18
TEXTURE_CHUNK_ID = 0x06
STRUCT_CHUNK_ID = 0x01
RW_HEADER = 12
# TxdDecoder.h:43,46 -- two packed encodings of the same RW 3.6/3.7 build.
KNOWN_VERSIONS = (0x1803FFFF, 0x1C02000A)


class TxdError(Exception):
    pass


def _hdr(buf: bytes, off: int):
    if off + RW_HEADER > len(buf):
        raise TxdError(f"chunk header past EOF at {off}")
    cid, size, ver = struct.unpack_from("<III", buf, off)
    return cid, size, ver, off + RW_HEADER


def census_txd(buf: bytes):
    """Walk one TXD. Returns (device_id, version, [mip_record...])."""
    cid, _size, ver, off = _hdr(buf, 0)
    if cid != ROOT_CHUNK_ID:
        raise TxdError(f"root chunk id 0x{cid:x}, expected 0x23")
    if off + 4 > len(buf):
        raise TxdError("truncated before numTex/deviceId")
    num_tex, device_id = struct.unpack_from("<HH", buf, off)
    off += 4

    mips = []
    for tex_i in range(num_tex):
        if off + 4 > len(buf):
            raise TxdError(f"truncated at texture {tex_i} numMips")
        (num_mips,) = struct.unpack_from("<I", buf, off)
        off += 4
        if num_mips > 16:
            raise TxdError(f"texture {tex_i}: numMips={num_mips} exceeds 16")

        for mip_i in range(num_mips):
            # Advance by the IMAGE chunk's DECLARED SIZE, exactly as
            # TxdDecoder.cpp:146-148 does (it computes img_payload_end from the
            # header and treats pixels+palette as filling the remainder). Summing
            # stride*h + palette instead would silently desync on any chunk that
            # carries slack, and would make `slack` below unobservable.
            cid, img_size, _v, img_payload = _hdr(buf, off)
            if cid != IMAGE_CHUNK_ID:
                raise TxdError(f"tex {tex_i} mip {mip_i}: id 0x{cid:x} != IMAGE")
            img_end = img_payload + img_size

            cid, _sz, _v, s_payload = _hdr(buf, img_payload)
            if cid != STRUCT_CHUNK_ID:
                raise TxdError(f"tex {tex_i} mip {mip_i}: id 0x{cid:x} != STRUCT")
            w, h, depth, stride = struct.unpack_from("<IIII", buf, s_payload)

            pixel_off = s_payload + 16
            pixel_bytes = stride * h
            pal_bytes = (1 << depth) * 4 if depth < 9 else 0
            if pixel_off + pixel_bytes + pal_bytes > img_end:
                raise TxdError(
                    f"tex {tex_i} mip {mip_i}: pixels+palette "
                    f"({pixel_bytes}+{pal_bytes}) exceed IMAGE payload "
                    f"({img_end - pixel_off})")
            slack = img_end - (pixel_off + pixel_bytes + pal_bytes)

            off = img_end
            if off > len(buf):
                raise TxdError(f"tex {tex_i} mip {mip_i}: payload past EOF")
            mips.append(
                dict(tex=tex_i, mip=mip_i, w=w, h=h, depth=depth, stride=stride,
                     pixel_bytes=pixel_bytes, pal_bytes=pal_bytes, slack=slack)
            )

        # TEXTURE chunk: skip wholesale via its declared size.
        cid, size, _v, after_hdr = _hdr(buf, off)
        if cid != TEXTURE_CHUNK_ID:
            raise TxdError(f"tex {tex_i}: id 0x{cid:x} != TEXTURE")
        off = after_hdr + size

    return device_id, ver, mips


# ---------------------------------------------------------------------------
# --rgba-hash: independent re-implementation of the librw raster bridge's hash
# ---------------------------------------------------------------------------
#
# Deliberately written from the FORMAT SPEC, not by porting the C++. Its whole
# value is being a second opinion: if this and
# mashedmod/src/mashed_re/LibRw/RwRasterBridge.cpp agree on every texture, the
# bridge's palette lookup, nibble handling, stride walk and channel order are all
# corroborated by something that shares no code with it. A hash produced only by
# the bridge would just be the bridge agreeing with itself.
#
# Must stay byte-identical in semantics to RasterRgbaHash():
#   FNV-1a 32, base mip only, row-major, four bytes per texel in R,G,B,A order.

HASH_TARGETS = [
    ("original/TOASTART/TRACKS/dump.piz", "DUMP.TXD"),
    ("original/TOASTART/Common/Frontend.piz", "TEXTURES.TXD"),
]


def _walk_full(buf):
    """Like census_txd but also returns each texture's name and base-mip bytes."""
    cid, _size, _ver, off = _hdr(buf, 0)
    if cid != ROOT_CHUNK_ID:
        raise TxdError(f"root chunk id 0x{cid:x}, expected 0x23")
    num_tex, device_id = struct.unpack_from("<HH", buf, off)
    off += 4
    out = []
    for _ti in range(num_tex):
        (num_mips,) = struct.unpack_from("<I", buf, off)
        off += 4
        base = None
        for mi in range(num_mips):
            cid, img_size, _v, img_payload = _hdr(buf, off)
            if cid != IMAGE_CHUNK_ID:
                raise TxdError(f"id 0x{cid:x} != IMAGE")
            img_end = img_payload + img_size
            cid, _sz, _v, sp = _hdr(buf, img_payload)
            if cid != STRUCT_CHUNK_ID:
                raise TxdError(f"id 0x{cid:x} != STRUCT")
            w, h, depth, stride = struct.unpack_from("<IIII", buf, sp)
            po = sp + 16
            pb = stride * h
            palb = (1 << depth) * 4 if depth < 9 else 0
            if mi == 0:
                base = (w, h, depth, stride,
                        buf[po:po + pb], buf[po + pb:po + pb + palb])
            off = img_end
        cid, size, _v, after = _hdr(buf, off)
        if cid != TEXTURE_CHUNK_ID:
            raise TxdError(f"id 0x{cid:x} != TEXTURE")
        name, p, end = "", after, after + size
        while p < end:                       # first STRING subchunk is the name
            c, s2, _v2, pl = _hdr(buf, p)
            if c == 2:
                name = buf[pl:pl + s2].split(b"\0")[0].decode("ascii", "replace")
                break
            p = pl + s2
        off = after + size
        out.append((name, base))
    return device_id, out


def _fnv1a_rgba(w, h, depth, stride, px, pal):
    h32 = 2166136261
    for y in range(h):
        row = px[y * stride:(y + 1) * stride]
        for x in range(w):
            if depth == 32:
                r, g, b, a = row[x * 4], row[x * 4 + 1], row[x * 4 + 2], row[x * 4 + 3]
            else:
                # depth 4 and 8 are BOTH one byte per pixel; depth 4 uses only
                # the low nibble as a 16-entry palette index.
                idx = (row[x] & 0x0F) if depth == 4 else row[x]
                e = idx * 4
                r, g, b, a = pal[e], pal[e + 1], pal[e + 2], pal[e + 3]
            for byte in (r, g, b, a):
                h32 = ((h32 ^ byte) * 16777619) & 0xFFFFFFFF
    return h32


def rgba_hash_mode():
    fmt_name = {4: "PAL4", 8: "PAL8", 32: "ARGB"}
    for piz_path, entry_name in HASH_TARGETS:
        piz = Path(piz_path)
        if not piz.exists():
            print(f"# MISSING {piz}", file=sys.stderr)
            return 2
        data, _v, _a, entries, _m = read_archive(piz)
        blob = None
        for name, off, length, _i in entries:
            if name.upper().endswith(entry_name.upper()):
                blob = data[off:off + length]
                break
        if blob is None:
            print(f"# {piz} has no entry {entry_name}", file=sys.stderr)
            return 2
        device_id, texs = _walk_full(blob)
        print(f"# {piz_path}::{entry_name}  textures={len(texs)} "
              f"deviceId={device_id}")
        for i, (name, base) in enumerate(texs):
            w, h, depth, stride, px, pal = base
            hv = _fnv1a_rgba(w, h, depth, stride, px, pal)
            print(f"{entry_name}|{i}|{name}|{fmt_name.get(depth, '?')}|"
                  f"{w}x{h}|{hv:08X}")
    return 0


def main(argv=None):
    ap = argparse.ArgumentParser()
    ap.add_argument("--root", default="original/TOASTART",
                    help="directory scanned recursively for .piz archives")
    ap.add_argument("--json", default=None, help="write the full record set here")
    ap.add_argument("--rgba-hash", action="store_true",
                    help="instead of the census, emit the same per-texture "
                         "fnv1a32(RGBA) the librw raster bridge logs, so the two "
                         "independent implementations can be diffed")
    args = ap.parse_args(argv)

    if args.rgba_hash:
        return rgba_hash_mode()

    root = Path(args.root)
    if not root.is_dir():
        print(f"[ERROR] no such directory: {root}", file=sys.stderr)
        return 2

    fmt_counter = Counter()          # (depth, has_palette) -> mips
    dims = defaultdict(Counter)      # depth -> (w,h) counts
    stride_odd = []                  # mips where stride != w * depth/8
    devices = Counter()
    versions = Counter()
    failures = []
    records = []
    n_txd = 0

    for piz in sorted(root.rglob("*.piz")):
        try:
            data, _ver, _apx, entries, _mode = read_archive(piz)
        except Exception as exc:                       # noqa: BLE001
            failures.append((str(piz), "<archive>", f"piz read failed: {exc}"))
            continue
        for name, off, length, _id in entries:
            if not name.upper().endswith(".TXD"):
                continue
            n_txd += 1
            blob = data[off:off + length]
            try:
                device_id, ver, mips = census_txd(blob)
            except TxdError as exc:
                failures.append((str(piz), name, str(exc)))
                continue
            devices[device_id] += 1
            versions[ver] += 1
            for m in mips:
                has_pal = m["pal_bytes"] > 0
                fmt_counter[(m["depth"], has_pal)] += 1
                dims[m["depth"]][(m["w"], m["h"])] += 1
                expect = m["w"] * m["depth"] // 8
                if m["stride"] != expect:
                    stride_odd.append((piz.name, name, m["w"], m["depth"],
                                       m["stride"], expect))
                records.append(dict(piz=piz.name, entry=name,
                                    device_id=device_id, version=ver, **m))

    print(f"scanned {n_txd} TXD entries across {args.root}")
    print(f"decoded OK: {n_txd - len(failures)}   failed: {len(failures)}")

    print("\n=== chunk versions ===")
    for v, c in versions.most_common():
        known = "known" if v in KNOWN_VERSIONS else "UNKNOWN -- investigate"
        print(f"  0x{v:08X}  {c:5d} TXD   ({known})")

    print("\n=== device ids ===")
    for d, c in devices.most_common():
        print(f"  deviceId={d}  {c} TXD")

    print("\n=== PIXEL FORMATS (the answer E2'a needs) ===")
    if not fmt_counter:
        print("  (none)")
    for (depth, has_pal), c in sorted(fmt_counter.items()):
        label = {8: "PAL8", 32: "ARGB8888"}.get(depth, f"depth{depth}")
        if depth == 8 and not has_pal:
            label += " (NO PALETTE -- unexpected)"
        print(f"  depth={depth:3d} palette={'yes' if has_pal else 'no ':3s}"
              f"  {c:6d} mips   {label}")

    print("\n=== dimensions per depth ===")
    for depth in sorted(dims):
        sizes = dims[depth]
        shown = ", ".join(f"{w}x{h}x{c}" for (w, h), c in sizes.most_common(8))
        npot = [f"{w}x{h}" for (w, h) in sizes
                if (w & (w - 1)) or (h & (h - 1))]
        print(f"  depth {depth}: {len(sizes)} distinct sizes; top: {shown}")
        if npot:
            print(f"    NON-power-of-two: {sorted(set(npot))}")

    print(f"\n=== stride anomalies (stride != width*depth/8): {len(stride_odd)} ===")
    for row in stride_odd[:15]:
        print(f"  {row[0]}::{row[1]} w={row[2]} depth={row[3]} "
              f"stride={row[4]} expected={row[5]}")
    if len(stride_odd) > 15:
        print(f"  ... and {len(stride_odd) - 15} more")

    if failures:
        print(f"\n=== decode failures: {len(failures)} ===")
        for p, n, e in failures[:20]:
            print(f"  {p}::{n}: {e}")

    if args.json:
        Path(args.json).write_text(json.dumps(records, indent=1), encoding="utf-8")
        print(f"\nwrote {len(records)} mip records -> {args.json}")

    return 1 if failures else 0


if __name__ == "__main__":
    raise SystemExit(main())
