#!/usr/bin/env python3
r"""xiso_extract.py — extract files from an Xbox XDVDFS (XISO) image.

Pure-stdlib. Handles plain XISO (volume descriptor at 0x10000) and
redump-style images (game partition at 0x18300000).

Usage:
  py -3.12 re\tools\console\xiso_extract.py <image.iso> --list [--dir PATH]
  py -3.12 re\tools\console\xiso_extract.py <image.iso> --extract <PATH> --out <file>
"""
import argparse
import struct

SECTOR = 2048
MAGIC = b"MICROSOFT*XBOX*MEDIA"
REDUMP_BASE = 0x18300000


class Xiso:
    def __init__(self, path):
        self.f = open(path, "rb")
        self.base = None
        for base in (0, REDUMP_BASE):
            self.f.seek(base + 32 * SECTOR)
            hdr = self.f.read(SECTOR)
            if hdr[:20] == MAGIC:
                self.base = base
                self.root_sector, self.root_size = struct.unpack_from("<II", hdr, 20)
                break
        if self.base is None:
            raise SystemExit("XDVDFS magic not found at 0x10000 or redump offset")

    def read_at(self, sector, size):
        self.f.seek(self.base + sector * SECTOR)
        return self.f.read(size)

    def walk_dir(self, sector, size):
        """Yield (name, sector, size, is_dir) by walking the dirent AVL tree."""
        table = self.read_at(sector, size)
        out = []

        def visit(off):
            if off + 14 > len(table):
                return
            left, right, start, fsize = struct.unpack_from("<HHII", table, off)
            attrs = table[off + 12]
            nlen = table[off + 13]
            if left == 0xFFFF:  # empty-tree sentinel
                return
            name = table[off + 14:off + 14 + nlen].decode("latin-1")
            if left:
                visit(left * 4)
            out.append((name, start, fsize, bool(attrs & 0x10)))
            if right:
                visit(right * 4)

        visit(0)
        return out

    def listdir(self, path=""):
        sector, size = self.root_sector, self.root_size
        parts = [p for p in path.replace("\\", "/").split("/") if p]
        for part in parts:
            for name, s, sz, is_dir in self.walk_dir(sector, size):
                if name.upper() == part.upper():
                    if not is_dir:
                        raise SystemExit(f"{name} is not a directory")
                    sector, size = s, sz
                    break
            else:
                raise SystemExit(f"dir not found: {part}")
        return self.walk_dir(sector, size)

    def find(self, path):
        parts = [p for p in path.replace("\\", "/").split("/") if p]
        parent = "/".join(parts[:-1])
        for name, s, sz, is_dir in self.listdir(parent):
            if name.upper() == parts[-1].upper():
                return s, sz, is_dir
        raise SystemExit(f"not found: {path}")


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("image")
    ap.add_argument("--list", action="store_true")
    ap.add_argument("--dir", default="")
    ap.add_argument("--extract")
    ap.add_argument("--out")
    args = ap.parse_args()

    xiso = Xiso(args.image)

    if args.list:
        for name, s, sz, is_dir in xiso.listdir(args.dir):
            kind = "DIR " if is_dir else "FILE"
            print(f"{kind} sector={s:<8} size={sz:<12} {name}")
        return

    if args.extract:
        s, sz, is_dir = xiso.find(args.extract)
        if is_dir:
            raise SystemExit(f"{args.extract} is a directory")
        out = args.out or args.extract.replace("/", "_")
        with open(out, "wb") as f:
            f.write(xiso.read_at(s, sz))
        print(f"wrote {out} ({sz} bytes)")
        return

    ap.print_help()


if __name__ == "__main__":
    main()
