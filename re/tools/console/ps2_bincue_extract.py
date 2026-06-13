#!/usr/bin/env python3
r"""ps2_bincue_extract.py — extract files from a MODE2/2352 bin/cue PS2 CD image.

Pure-stdlib ISO9660 reader over raw 2352-byte sectors (CD-ROM XA Form 1:
sync 12 + header 4 + subheader 8 + 2048 user bytes + EDC/ECC).

Usage:
  py -3.12 re\tools\console\ps2_bincue_extract.py <image.bin> --list
  py -3.12 re\tools\console\ps2_bincue_extract.py <image.bin> --extract <ISO_PATH> --out <file>
  py -3.12 re\tools\console\ps2_bincue_extract.py <image.bin> --boot --out-dir <dir>
      (--boot reads SYSTEM.CNF, resolves BOOT2, extracts the boot ELF)
"""
import argparse
import re
import struct
import sys

RAW = 2352
USER = 2048
USER_OFF = 24  # MODE2 Form 1: 12 sync + 4 header + 8 subheader


class BinImage:
    def __init__(self, path):
        self.f = open(path, "rb")

    def sector(self, lba):
        self.f.seek(lba * RAW + USER_OFF)
        return self.f.read(USER)

    def read(self, lba, size):
        out = bytearray()
        n = (size + USER - 1) // USER
        for i in range(n):
            out += self.sector(lba + i)
        return bytes(out[:size])


def parse_dir_records(data):
    """Yield (name, lba, size, is_dir) from an ISO9660 directory extent."""
    pos = 0
    while pos < len(data):
        ln = data[pos]
        if ln == 0:
            # rest of this 2048-byte sector is padding; jump to next sector
            pos = (pos // USER + 1) * USER
            continue
        rec = data[pos:pos + ln]
        lba = struct.unpack_from("<I", rec, 2)[0]
        size = struct.unpack_from("<I", rec, 10)[0]
        flags = rec[25]
        name_len = rec[32]
        name = rec[33:33 + name_len].decode("ascii", "replace")
        if name not in ("\x00", "\x01"):
            yield name.split(";")[0], lba, size, bool(flags & 2)
        pos += ln


class Iso9660:
    def __init__(self, img: BinImage):
        self.img = img
        pvd = img.sector(16)
        if pvd[0] != 1 or pvd[1:6] != b"CD001":
            raise SystemExit("PVD not found at sector 16 — not ISO9660 or wrong sector mode")
        root_rec = pvd[156:156 + 34]
        self.root_lba = struct.unpack_from("<I", root_rec, 2)[0]
        self.root_size = struct.unpack_from("<I", root_rec, 10)[0]

    def listdir(self, lba=None, size=None):
        if lba is None:
            lba, size = self.root_lba, self.root_size
        return list(parse_dir_records(self.img.read(lba, size)))

    def find(self, path):
        """Resolve an ISO path like DATA/FILE.BIN (case-insensitive)."""
        parts = [p for p in path.replace("\\", "/").upper().split("/") if p]
        lba, size = self.root_lba, self.root_size
        for i, part in enumerate(parts):
            for name, e_lba, e_size, is_dir in self.listdir(lba, size):
                if name.upper() == part:
                    if i == len(parts) - 1:
                        return e_lba, e_size, is_dir
                    if not is_dir:
                        raise SystemExit(f"{name} is not a directory")
                    lba, size = e_lba, e_size
                    break
            else:
                raise SystemExit(f"not found: {part} (in {path})")
        raise SystemExit(f"not found: {path}")


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("image")
    ap.add_argument("--list", action="store_true")
    ap.add_argument("--extract", help="ISO path of file to extract")
    ap.add_argument("--out", help="output file path")
    ap.add_argument("--boot", action="store_true", help="extract boot ELF per SYSTEM.CNF")
    ap.add_argument("--out-dir", default=".")
    args = ap.parse_args()

    img = BinImage(args.image)
    iso = Iso9660(img)

    if args.list:
        for name, lba, size, is_dir in iso.listdir():
            kind = "DIR " if is_dir else "FILE"
            print(f"{kind} lba={lba:<8} size={size:<12} {name}")
        return

    if args.extract:
        lba, size, is_dir = iso.find(args.extract)
        if is_dir:
            raise SystemExit(f"{args.extract} is a directory")
        out = args.out or args.extract.replace("/", "_")
        with open(out, "wb") as f:
            f.write(img.read(lba, size))
        print(f"wrote {out} ({size} bytes)")
        return

    if args.boot:
        lba, size, _ = iso.find("SYSTEM.CNF")
        cnf = img.read(lba, size).decode("ascii", "replace")
        print("--- SYSTEM.CNF ---")
        print(cnf.strip())
        m = re.search(r"BOOT2\s*=\s*cdrom0?:\\?([^;\s]+)", cnf)
        if not m:
            raise SystemExit("BOOT2 line not found in SYSTEM.CNF")
        boot = m.group(1).split(";")[0]
        lba, size, _ = iso.find(boot)
        out = f"{args.out_dir.rstrip('/\\')}/{boot}"
        with open(out, "wb") as f:
            f.write(img.read(lba, size))
        print(f"wrote {out} ({size} bytes)")
        return

    ap.print_help()


if __name__ == "__main__":
    main()
