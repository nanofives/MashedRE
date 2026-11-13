"""Offline check of mashed_image.bin against the four per-address seed tables (IMG-1, RESULT_PORTSTATE-IMAGE.md s5).

Usage:
  py -3.12 re/tools/image/image_check.py --image <mashed_image.bin> [--src <mashed_re source root>]

Validates the file format (magic, version, source SHA-256 = anchor, trailer SHA-256, per-range FNV-1a), then for
every entry of
  Compat/PortStateSeed.inc       (kPortStateSeedData + {addr, len, off} entries)
  Match/PortTop_RdataSeed.inc    ({addr, len, "bytes"})
  Match/PortTop_DInputSeed.inc   ({addr, len, "bytes"})
  Compat/PortTopCodePtr.inc      (CP_SEED(addr, len, "bytes") blocks)
checks that the entry lies inside one image range and that its bytes equal the image bytes. Also reports entries
overlapping the IAT range 0x005cc000..0x005cc2e8 (zeroed by the IMG-2 loader), the CP_FIX slots whose image dword
equals the CP_FIX original target, and the two B16 constants written at exe_main.cpp:9951-9952.
Exit 0 only when every entry is equal and none overlaps the IAT.
"""
import argparse
import os
import re
import struct
import sys

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import mashed_image as mi  # noqa: E402

REPO = os.path.abspath(os.path.join(os.path.dirname(os.path.abspath(__file__)), '..', '..', '..'))
B16_CONSTS = ((0x005cd5a8, 0x3acccccd), (0x005cc560, 0x3b088889))


def parse_portstate(path):
    t = open(path).read()
    data_m = re.search(r'kPortStateSeedData\[\d+\] = \{(.*?)\};', t, re.S)
    data = bytes(int(x, 16) for x in re.findall(r'0x([0-9a-fA-F]{2})\b', re.sub(r'//[^\n]*', '', data_m.group(1))))
    ents = [(int(a, 16), int(n), int(o)) for a, n, o in re.findall(r'\{ 0x([0-9a-f]+)u,\s*(\d+)u,\s*(\d+)u \}', t)]
    m = re.search(r'ENTRIES\s+(\d+)u', t)
    declared = int(m.group(1)) if m else None
    return [(a, data[o:o + n]) for a, n, o in ents], declared


def parse_strtab(path):
    out = []
    for a, n, s in re.findall(r'\{ 0x([0-9a-f]+)u, (\d+), "((?:[^"\\]|\\.)*)" \}', open(path).read()):
        bs = bytes(int(h, 16) for h in re.findall(r'\\x([0-9a-f]{2})', s))
        if len(bs) != int(n):
            raise ValueError('%s: entry 0x%s declares %s bytes, string has %d' % (path, a, n, len(bs)))
        out.append((int(a, 16), bs))
    return out, None


def parse_cpseed(path):
    t = open(path).read()
    out = []
    for a, n, s in re.findall(r'CP_SEED\(0x([0-9a-fA-F]+)u?,\s*(\d+)u?,\s*"((?:[^"\\]|\\.)*)"\)', t):
        bs = bytes(int(h, 16) for h in re.findall(r'\\x([0-9a-f]{2})', s))
        if len(bs) != int(n):
            raise ValueError('%s: CP_SEED 0x%s declares %s bytes, string has %d' % (path, a, n, len(bs)))
        out.append((int(a, 16), bs))
    fixes = [(int(s, 16), int(o, 16)) for s, o in
             re.findall(r'CP_FIX\(0x([0-9a-fA-F]+)u?,\s*[A-Za-z_]\w*,\s*0x([0-9a-fA-F]+)', t)]
    return out, fixes


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('--image', required=True)
    ap.add_argument('--src', default=os.path.join(REPO, 'mashedmod', 'src', 'mashed_re'))
    a = ap.parse_args()

    img = open(a.image, 'rb').read()
    try:
        src_sha, ranges = mi.parse(img)
    except ValueError as e:
        print('FORMAT FAIL: %s' % e)
        return 1
    print('image %s size %d source sha256 %s format OK (magic, version, trailer sha256, %d range fnv)'
          % (a.image, len(img), src_sha, len(ranges)))
    for name, va, data, fnv in ranges:
        print('  range %-8s 0x%08x..0x%08x len 0x%06x fnv1a32 0x%08x' % (name, va, va + len(data), len(data), fnv))

    def image_bytes(addr, n):
        for name, va, data, _ in ranges:
            if va <= addr and addr + n <= va + len(data):
                return name, data[addr - va:addr - va + n]
        return None, None

    tables = (
        ('PortStateSeed.inc', parse_portstate, os.path.join(a.src, 'Compat', 'PortStateSeed.inc')),
        ('PortTop_RdataSeed.inc', parse_strtab, os.path.join(a.src, 'Match', 'PortTop_RdataSeed.inc')),
        ('PortTop_DInputSeed.inc', parse_strtab, os.path.join(a.src, 'Match', 'PortTop_DInputSeed.inc')),
        ('PortTopCodePtr.inc CP_SEED', parse_cpseed, os.path.join(a.src, 'Compat', 'PortTopCodePtr.inc')),
    )
    tot_eq = tot = iat_hits = 0
    fixes = []
    ok = True
    for label, fn, path in tables:
        ents, extra = fn(path)
        if label.startswith('PortTopCodePtr'):
            fixes = extra
        elif extra is not None and extra != len(ents):
            print('  %s: declared ENTRIES %d != parsed %d' % (label, extra, len(ents)))
            ok = False
        eq = 0
        secs = {}
        for addr, bs in ents:
            name, ib = image_bytes(addr, len(bs))
            if ib == bs:
                eq += 1
                secs[name] = secs.get(name, 0) + 1
            else:
                print('  MISMATCH %s 0x%08x len %d range %s table %s image %s'
                      % (label, addr, len(bs), name, bs.hex(), ib.hex() if ib else '-'))
            if addr < mi.IAT_RANGE[1] and addr + len(bs) > mi.IAT_RANGE[0]:
                iat_hits += 1
                print('  IAT-OVERLAP %s 0x%08x len %d' % (label, addr, len(bs)))
        print('%s: entries %d bytes %d equal %d of %d by range %s'
              % (label, len(ents), sum(len(x) for _, x in ents), eq, len(ents), secs))
        tot += len(ents)
        tot_eq += eq
        if eq != len(ents):
            ok = False

    fx_eq = 0
    for slot, orig in fixes:
        _, ib = image_bytes(slot, 4)
        if ib is not None and struct.unpack('<I', ib)[0] == orig:
            fx_eq += 1
        else:
            print('  CP_FIX slot 0x%08x image %s != original target 0x%08x' % (slot, ib.hex() if ib else '-', orig))
    print('PortTopCodePtr.inc CP_FIX: slots %d, image dword equals the original target %d of %d'
          % (len(fixes), fx_eq, len(fixes)))
    for addr, want in B16_CONSTS:
        _, ib = image_bytes(addr, 4)
        got = struct.unpack('<I', ib)[0] if ib else None
        print('B16 const 0x%08x image %s expected 0x%08x equal %s'
              % (addr, ('0x%08x' % got) if got is not None else '-', want, got == want))
        ok = ok and got == want
    ok = ok and iat_hits == 0 and fx_eq == len(fixes)
    print('TOTAL seed entries equal %d of %d, IAT overlaps %d -> %s'
          % (tot_eq, tot, iat_hits, 'PASS' if ok else 'FAIL'))
    return 0 if ok else 1


if __name__ == '__main__':
    sys.exit(main())
