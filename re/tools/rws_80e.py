r"""rws_80e.py -- decode the 0x080E header of a 0x080D RWS bank by replaying the exe's own walk.

The walk is a transcription of MASHED.exe's reader, not a fit:

  FUN_005b15e0  stream state machine. On chunk 0x80E it reads a 0x14-byte PREFIX
                (into stream dwords 0x47..0x4b, 0x005b1812 push 0x80e / 0x005b17e9),
                then reads PREFIX[+0x00] bytes of BODY into a buffer (state 0x80 with no
                UUID/name filter set) and hands it to FUN_005b8200.
  FUN_005b8200  in-place pointer fix-up of BODY. Every pointer-sized field in the file is
                a "present" flag (non-zero = the item follows); the fix-up overwrites it
                with the address of the item, consuming the bytes in a fixed order.
  FUN_005aca80  size of one format descriptor (0x1c, 0x2c with UUID, + extra bytes).
  FUN_005a6190  alignment DAT_00914700 = max(0x10, arg). The walk uses 0x10; the check
                below (cursor lands exactly on PREFIX[+0x00]) is what confirms it.

BODY layout (offsets from BODY start = chunk data + 0x14), all little-endian dwords:

  +0x00 uuid-present   +0x04 name-present   +0x08 (raw)   +0x0c nA (record-A count)
  +0x10 A-array ptr    +0x14 nB (record-B count)          +0x18 B-array ptr
  +0x1c (raw)          +0x20 block size (FUN_005b15e0 reads *(body+0x20) after 0x80F)
  +0x24 (raw, added to the seek offset in FUN_005b30e0)    +0x28 (raw)
  then the variable part, in FUN_005b8200 order (see walk()).

Record A, stride 0x20 (FUN_005b8200 loops of 0x20; FUN_005b30e0 reads +0x18/+0x1c):
  +0x00 uuid-present  +0x04 name-present  +0x08 (raw)  +0x0c per-B dword array ptr
  +0x10 nC  +0x14 C-array ptr  +0x18 byte length in 0x80F  +0x1c byte offset in 0x80F
Record B, stride 0x28: +0x00 uuid-present +0x04 name-present +0x14 format-desc ptr
  +0x18 u16 +0x1a u16 +0x20 dword (FUN_005b30e0 divides by it) ... other fields raw.
Record C, stride 0x14: +0x00 uuid-present +0x04 name-present, rest raw.

Usage:
  py -3.12 -I re\tools\rws_80e.py <bank.rws> [...] [--json out.json] [--md out.md]
"""
import argparse
import json
import struct
import sys
from pathlib import Path

ALIGN = 0x10


def u32(b, o):
    return struct.unpack_from("<I", b, o)[0]


def cstr(b, o):
    z = b.find(b"\0", o)
    return b[o:z].decode("latin1", "replace"), z - o


def adv_name(b, o, nul_counts):
    s, n = cstr(b, o)
    ln = n + 1 if nul_counts else n
    return s, (ln + ALIGN) & ~(ALIGN - 1)        # 0x005b8200: (len + A) & ~(A-1)


def walk(body, nul_counts=False):
    """Replay FUN_005b8200 on BODY. Returns (decoded dict, end cursor)."""
    h = list(struct.unpack_from("<11I", body, 0))
    nA, nB = h[3], h[5]
    p = 0x2c                                       # piVar7 = param_1 + 0xb
    out = {"fixed": {f"+0x{4*i:02x}": h[i] for i in range(11)}}
    if h[0]:
        out["uuid"] = body[p:p + 16].hex(); p += 16
    if h[1]:
        out["name"], a = adv_name(body, p, nul_counts); p += a
    A0 = p; p += nA * 0x20
    A = []
    for i in range(nA):
        r = struct.unpack_from("<8I", body, A0 + i * 0x20)
        A.append({"i": i, "raw": list(r)})
    for a in A:                                    # per-A dword arrays, nB entries each
        if a["raw"][3]:
            a["perB"] = list(struct.unpack_from(f"<{nB}I", body, p)); p += nB * 4
    for a in A:
        if a["raw"][0]:
            a["uuid"] = body[p:p + 16].hex(); p += 16
    for a in A:
        if a["raw"][1]:
            a["name"], s = adv_name(body, p, nul_counts); p += s
    for a in A:                                    # C arrays
        if a["raw"][5]:
            a["C0"] = p; p += a["raw"][4] * 0x14
    for a in A:
        for k in range(a["raw"][4] if a["raw"][5] else 0):
            if u32(body, a["C0"] + k * 0x14):
                p += 16
    for a in A:
        for k in range(a["raw"][4] if a["raw"][5] else 0):
            if u32(body, a["C0"] + k * 0x14 + 4):
                _, s = adv_name(body, p, nul_counts); p += s
    B0 = p; p += nB * 0x28
    B = []
    for i in range(nB):
        o = B0 + i * 0x28
        r = struct.unpack_from("<10I", body, o)
        B.append({"i": i, "raw": list(r),
                  "u16_18": struct.unpack_from("<H", body, o + 0x18)[0],
                  "u16_1a": struct.unpack_from("<H", body, o + 0x1a)[0]})
    for b in B:
        if b["raw"][5]:
            f = body[p:p + 0x1c]
            fd = {"+0x00": u32(f, 0), "+0x04": u32(f, 4), "+0x08": u32(f, 8),
                  "+0x0c_u8": f[0xc], "+0x0d_u8": f[0xd], "+0x10": u32(f, 0x10),
                  "+0x14": u32(f, 0x14), "+0x18_u8": f[0x18], "+0x19_u8": f[0x19]}
            size = 0x2c if fd["+0x04"] else 0x1c   # FUN_005aca80
            if fd["+0x04"]:
                fd["codec_uuid"] = body[p + 0x1c:p + 0x2c].hex()
            if fd["+0x10"]:
                size += fd["+0x14"]
            fd["size"] = size
            b["fmt"] = fd
            p += (size - 1 + ALIGN) & ~(ALIGN - 1)
    for b in B:
        if b["raw"][0]:
            b["uuid"] = body[p:p + 16].hex(); p += 16
    for b in B:
        if b["raw"][1]:
            b["name"], s = adv_name(body, p, nul_counts); p += s
    for a in A:
        a.pop("C0", None)
    out["A"], out["B"] = A, B
    return out, p


def parse(path):
    d = Path(path).read_bytes()
    t, sz, ver = struct.unpack_from("<III", d, 0)
    if t != 0x80D:
        return {"file": str(path), "error": f"top chunk {t:#x}, not 0x80d"}
    t2, sz2, _ = struct.unpack_from("<III", d, 12)
    if t2 != 0x80E:
        return {"file": str(path), "error": f"first child {t2:#x}, not 0x80e"}
    pre = list(struct.unpack_from("<5I", d, 24))
    body = d[24 + 0x14:24 + sz2]
    t3, sz3, _ = struct.unpack_from("<III", d, 24 + sz2)
    res = {"file": Path(path).name, "path": str(path), "chunk_80e_size": sz2,
           "prefix": {f"+0x{4*i:02x}": v for i, v in enumerate(pre)},
           "data_chunk": hex(t3), "data_size": sz3}
    fits = {}
    for nul in (False, True):
        dec, end = walk(body, nul)
        fits["strlen+1" if nul else "strlen"] = end
        if end == pre[0]:
            res.update(dec); res["name_len_rule"] = "strlen+1" if nul else "strlen"
            break
    res["walk_end"] = fits
    res["walk_ok"] = "name_len_rule" in res
    if not res["walk_ok"]:
        dec, _ = walk(body, False); res.update(dec)
    A = res["A"]
    res["A_tiles_data"] = (all(A[i]["raw"][7] + A[i]["raw"][6] == A[i + 1]["raw"][7]
                               for i in range(len(A) - 1))
                           and A[0]["raw"][7] == 0
                           and A[-1]["raw"][7] + A[-1]["raw"][6] == sz3)
    return res


def md(rows):
    L = ["| bank | 80E size | prefix +0/+4/+8/+c/+10 | nA | nB | body +0x08 +0x1c +0x20 +0x24 +0x28 | walk | A tiles 0x80F | 0x80F size |",
         "|---|---|---|---|---|---|---|---|---|"]
    for r in rows:
        if "error" in r:
            L.append(f"| {r['file']} | {r['error']} |||||||| "); continue
        p, f = r["prefix"], r["fixed"]
        L.append(f"| {r['path'].split('pcdics')[-1].lstrip(chr(92)+'/')} | {r['chunk_80e_size']} | "
                 f"{p['+0x00']:#x}/{p['+0x04']:#x}/{p['+0x08']:#x}/{p['+0x0c']:#x}/{p['+0x10']:#x} | "
                 f"{f['+0x0c']} | {f['+0x14']} | {f['+0x08']:#x} {f['+0x1c']:#x} {f['+0x20']:#x} {f['+0x24']:#x} {f['+0x28']:#x} | "
                 f"{'end=' + hex(p['+0x00']) + ' ' + r['name_len_rule'] if r['walk_ok'] else 'MISS ' + str(r['walk_end'])} | "
                 f"{'YES' if r['A_tiles_data'] else 'NO'} | {r['data_size']} |")
    return "\n".join(L)


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("banks", nargs="+")
    ap.add_argument("--json")
    ap.add_argument("--md")
    a = ap.parse_args()
    rows = [parse(p) for p in a.banks]
    for r in rows:
        if "error" in r:
            print(f"{r['file']}: {r['error']}"); continue
        print(f"{r['path']}: nA={r['fixed']['+0x0c']} nB={r['fixed']['+0x14']} name={r.get('name')!r} "
              f"walk_ok={r['walk_ok']} rule={r.get('name_len_rule')} A_tiles={r['A_tiles_data']}")
    if a.json:
        Path(a.json).write_text(json.dumps(rows, indent=1))
    if a.md:
        Path(a.md).write_text(md(rows) + "\n")
    return 0 if all(r.get("walk_ok") for r in rows if "error" not in r) else 1


if __name__ == "__main__":
    sys.exit(main())
