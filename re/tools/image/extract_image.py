"""Stage the file-backed .rdata + .data + _rwdseg image of MASHED.exe.unpatched as mashed_image.bin (L-149 a, IMG-1).

Usage:
  py -3.12 re/tools/image/extract_image.py --exe <MASHED.exe.unpatched> --out <dir>/original/mashed_image.bin [--replace]

Spec: verify/portstate_20261113/RESULT_PORTSTATE-IMAGE.md sections 1-2.

Safety rules (row IMG-1-EXTRACT):
- the exe is opened read-only, and its SHA-256 must be the anchor BDCAE093...3C0E (else exit 2, nothing written);
- the section table must equal the one in RESULT_PORTSTATE-IMAGE.md section 1 (else exit 2);
- the output is created with exclusive create: an existing file is never overwritten, unless --replace is given,
  and --replace is refused for any output whose folder is named "original" (an existing file there is never
  modified, overwritten or deleted);
- the output may not be the input file.
"""
import argparse
import hashlib
import os
import sys

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import mashed_image as mi  # noqa: E402


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('--exe', required=True)
    ap.add_argument('--out', required=True)
    ap.add_argument('--replace', action='store_true')
    a = ap.parse_args()

    exe = os.path.abspath(a.exe)
    out = os.path.abspath(a.out)
    if os.path.normcase(exe) == os.path.normcase(out):
        sys.exit('REFUSED: output is the input file')
    with open(exe, 'rb') as f:
        b = f.read()
    sha = hashlib.sha256(b).hexdigest().upper()
    print('exe %s size %d sha256 %s' % (exe, len(b), sha))
    if sha != mi.ANCHOR_SHA256 or len(b) != mi.ANCHOR_SIZE:
        print('REFUSED: exe is not the anchor %s size %d' % (mi.ANCHOR_SHA256, mi.ANCHOR_SIZE))
        return 2
    try:
        img = mi.build(b)
        _, ranges = mi.parse(img)   # self-check of the written format
    except ValueError as e:
        print('REFUSED: %s' % e)
        return 2

    parent = os.path.dirname(out)
    exists = os.path.exists(out)
    if exists and not a.replace:
        print('REFUSED: %s exists (never overwritten without --replace)' % out)
        return 2
    if exists and os.path.basename(parent).lower() == 'original':
        print('REFUSED: --replace of an existing file inside an "original" folder')
        return 2
    os.makedirs(parent, exist_ok=True)
    if exists:
        tmp = out + '.tmp%d' % os.getpid()
        with open(tmp, 'xb') as f:
            f.write(img)
        os.replace(tmp, out)
    else:
        with open(out, 'xb') as f:   # exclusive create: fails rather than overwrite
            f.write(img)

    for name, va, data, fnv in ranges:
        print('range %-8s va 0x%08x len 0x%06x (%d) fnv1a32 0x%08x' % (name, va, len(data), len(data), fnv))
    print('payload bytes %d' % sum(len(r[2]) for r in ranges))
    print('wrote %s size %d sha256 %s' % (out, len(img), hashlib.sha256(img).hexdigest().upper()))
    return 0


if __name__ == '__main__':
    sys.exit(main())
