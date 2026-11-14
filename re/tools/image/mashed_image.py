"""Shared reader/writer for mashed_image.bin (format v1, RESULT_PORTSTATE-IMAGE.md section 2).

Layout (little endian):
  0x00 magic 'MSIM'
  0x04 u32 version = 1
  0x08 32 B SHA-256 of the source binary (MASHED.exe.unpatched anchor)
  0x28 u32 range count N
  0x2c N x { u32 va, u32 len, u32 payload offset, u32 FNV-1a 32 of the range, char name[8] }
  ...  payloads (raw file bytes of [va, va+len))
  end-32  32 B SHA-256 of every byte before it
"""
import hashlib
import struct

MAGIC = b'MSIM'
VERSION = 1
ANCHOR_SHA256 = 'BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E'
ANCHOR_SIZE = 2846720
IMAGE_BASE = 0x00400000

# Section table expected in the anchor (RESULT_PORTSTATE-IMAGE.md section 1):
# name -> (VA, VirtualSize, SizeOfRawData, PointerToRawData, Characteristics)
EXPECTED_SECTIONS = {
    '.text':   (0x00401000, 0x1c98e0, 0x1ca000, 0x001000, 0x60000020),
    '_rwcseg': (0x005cb000, 0x000451, 0x001000, 0x1cb000, 0x60000020),
    '.rdata':  (0x005cc000, 0x01d044, 0x01e000, 0x1cc000, 0x40000040),
    '.data':   (0x005ea000, 0x32a704, 0x04d000, 0x1ea000, 0xc0000040),
    '_rwdseg': (0x00915000, 0x000008, 0x001000, 0x237000, 0xc0000040),
    '.rsrc':   (0x00916000, 0x07eb18, 0x07f000, 0x238000, 0x40000040),
}
IMAGE_SECTIONS = ('.rdata', '.data', '_rwdseg')
IAT_RANGE = (0x005cc000, 0x005cc2e8)   # IAT datadir VA 0x005cc000 size 0x2e8 (header field at file 0x1e8)
RANGE_HDR = struct.Struct('<IIII8s')
HDR = struct.Struct('<4sI32sI')


def fnv1a32(data):
    h = 0x811c9dc5
    for c in data:
        h = ((h ^ c) * 0x01000193) & 0xffffffff
    return h


def read_sections(b):
    """Return [(name, va, vsize, rsize, rptr, chars)] from the PE section table."""
    e_lfanew = struct.unpack_from('<I', b, 0x3c)[0]
    if b[e_lfanew:e_lfanew + 4] != b'PE\0\0':
        raise ValueError('not a PE file')
    fh = e_lfanew + 4
    _, nsec, _, _, _, opt_size, _ = struct.unpack_from('<HHIIIHH', b, fh)
    opt = fh + 20
    image_base = struct.unpack_from('<I', b, opt + 28)[0]
    if image_base != IMAGE_BASE:
        raise ValueError('ImageBase 0x%08x != 0x%08x' % (image_base, IMAGE_BASE))
    sh = opt + opt_size
    out = []
    for i in range(nsec):
        o = sh + 40 * i
        name = b[o:o + 8].rstrip(b'\0').decode('ascii')
        vsize, va, rsize, rptr = struct.unpack_from('<IIII', b, o + 8)
        chars = struct.unpack_from('<I', b, o + 36)[0]
        out.append((name, image_base + va, vsize, rsize, rptr, chars))
    return out


def check_sections(secs):
    got = {s[0]: (s[1], s[2], s[3], s[4], s[5]) for s in secs}
    if got != EXPECTED_SECTIONS:
        diff = sorted(set(got.items()) ^ set(EXPECTED_SECTIONS.items()))
        raise ValueError('section table differs from RESULT_PORTSTATE-IMAGE.md section 1: %r' % (diff,))


def build(b):
    """Build the image file bytes from the anchored exe bytes."""
    src_sha = hashlib.sha256(b).digest()
    secs = read_sections(b)
    check_sections(secs)
    ranges = []
    for name in IMAGE_SECTIONS:
        _, va, vsize, rsize, rptr, _ = [s for s in secs if s[0] == name][0]
        n = min(vsize, rsize)   # file-backed part; the BSS tail stays the pad's zeros
        ranges.append((name, va, b[rptr:rptr + n]))
    hdr_len = HDR.size + RANGE_HDR.size * len(ranges)
    out = bytearray(HDR.pack(MAGIC, VERSION, src_sha, len(ranges)))
    off = hdr_len
    for name, va, data in ranges:
        out += RANGE_HDR.pack(va, len(data), off, fnv1a32(data), name.encode('ascii').ljust(8, b'\0'))
        off += len(data)
    for _, _, data in ranges:
        out += data
    out += hashlib.sha256(bytes(out)).digest()
    return bytes(out)


def parse(img):
    """Validate an image file and return (src_sha_hex, [(name, va, data)]). Raises ValueError on any defect."""
    if len(img) < HDR.size + 32:
        raise ValueError('short file')
    magic, ver, src_sha, n = HDR.unpack_from(img, 0)
    if magic != MAGIC:
        raise ValueError('magic %r' % magic)
    if ver != VERSION:
        raise ValueError('version %d' % ver)
    if src_sha.hex().upper() != ANCHOR_SHA256:
        raise ValueError('source sha256 %s is not the anchor' % src_sha.hex().upper())
    if hashlib.sha256(img[:-32]).digest() != img[-32:]:
        raise ValueError('trailer sha256 mismatch')
    ranges = []
    for i in range(n):
        va, ln, off, fnv, name = RANGE_HDR.unpack_from(img, HDR.size + RANGE_HDR.size * i)
        data = img[off:off + ln]
        if len(data) != ln or off + ln > len(img) - 32:
            raise ValueError('range %d out of file' % i)
        if fnv1a32(data) != fnv:
            raise ValueError('range %s fnv mismatch' % name)
        ranges.append((name.rstrip(b'\0').decode('ascii'), va, data, fnv))
    return src_sha.hex().upper(), ranges
