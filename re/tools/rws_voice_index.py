r"""rws_voice_index.py — read the CLIP INDEX out of a 0x080D RenderWare audio bank.

`rws_extract_wav.py` already decodes a 0x080D bank as ONE continuous IMA ADPCM
stream (cracked 2026-06-16). What it does not do is tell you the bank is a
DICTIONARY of individually addressable clips. This tool reads that index.

Container (measured on original\toastaudio\pc\audio\pcdics\english\*.rws):

    0x080D  top                     e.g. red.rws 1,841,140 bytes
      0x080E  header                8,156 bytes  <- the index lives here
      0x080F  data                  1,832,960 bytes (the IMA stream)

Inside the 0x080E header:

    +0x20   dword   CLIP COUNT      98 in every english/ bank
    +0x50   asciiz  BANK NAME       "red", "bluejay", ...
    ...
    name table      stride 0x10     10-byte ASCIIZ name + 6 bytes padding

Clip names are `<PP>_<n>` where PP is the character prefix (RD BJ GD MN PK SW
for RED BLUEJAY GOLD MELON PINK SHADOW) and n RESTARTS at 1 for each GROUP.
Measured groups: 13, 25, 25, 35 (= 98). Those groups are load-bearing — they
are exactly the four ranges the audio-event template table at DAT_005fcb50
addresses (tpl0 base 0 count 13, tpl1 base 13 count 25, tpl19 base 38 count 25,
tpl39 base 63 count 35), and there are four queues in the FUN_0045c640 selector.
See re/analysis/structs/contcfg_record.md.

A second table carries the CUT POINTS -- stride 0x20 starting at offset 120,
98 entries, `length` at +0x00 and `byte offset` at +0x04 into the 0x080F stream.
PROVEN, not fitted: for every bank tested the 98 clips tile the data section
exactly (off[i] + len[i] == off[i+1] for all i, and the last clip ends precisely
at the data size). Lengths are multiples of 0x2800 = 10240, the block size that
also appears verbatim at header +0x34.

Usage:
  py -3.12 re\tools\rws_voice_index.py <bank.rws> [--json]
  py -3.12 re\tools\rws_voice_index.py --all
"""
import argparse
import array
import json
import re
import struct
import sys
import wave
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
ENGLISH = ROOT / "original/toastaudio/pc/audio/pcdics/english"

T_TOP, T_HDR, T_DATA = 0x080D, 0x080E, 0x080F
OFF_COUNT = 0x20
OFF_NAME = 0x50
NAME_STRIDE = 0x10
NAME_MAX = 10
CUT_TABLE = 120        # stride 0x20: +0x00 length, +0x04 byte offset
CUT_STRIDE = 0x20


def read_header(path):
    d = Path(path).read_bytes()
    t, sz, _ = struct.unpack_from("<III", d, 0)
    if t != T_TOP:
        raise ValueError(f"{path}: top chunk is {t:#x}, expected {T_TOP:#x} "
                         f"(0x809 banks are handled by rws_extract_wav.py)")
    t2, sz2, _ = struct.unpack_from("<III", d, 12)
    if t2 != T_HDR:
        raise ValueError(f"{path}: first child is {t2:#x}, expected {T_HDR:#x}")
    return d, d[24:24 + sz2], sz2


def data_size(d, hsz):
    """Size of the 0x080F payload."""
    off = 24 + hsz
    t, sz, _ = struct.unpack_from("<III", d, off)
    if t != T_DATA:
        raise ValueError(f"second child is {t:#x}, expected {T_DATA:#x}")
    return sz


def data_offset(d, hsz):
    return 24 + hsz + 12


def parse(path):
    d, h, hsz = read_header(path)
    count = struct.unpack_from("<I", h, OFF_COUNT)[0]
    z = h.find(b"\0", OFF_NAME)
    bank = h[OFF_NAME:z].decode("latin1", "replace")

    # Locate the name table by finding the first <PP>_<n> record, then walk at
    # a fixed stride. Anchoring on the first match rather than scanning for all
    # matches matters: a regex sweep silently DROPS records whose name bytes are
    # not clean ASCII, which under-counts (melon/pink return 27/28 that way
    # while their header count says 98).
    m = re.search(rb"([A-Z]{2})_(\d+)\x00", h)
    if not m:
        return {"file": str(path), "bank": bank, "count": count,
                "prefix": None, "clips": [], "groups": [],
                "note": "no clip-name record found"}
    start = m.start()
    prefix = m.group(1).decode()

    clips = []
    for i in range(count):
        o = start + i * NAME_STRIDE
        if o + NAME_MAX > hsz:
            break
        raw = h[o:o + NAME_MAX]
        z = raw.find(b"\0")
        nm = raw[:z if z >= 0 else NAME_MAX]
        clips.append(nm.decode("latin1", "replace"))

    groups, cur = [], 0
    for c in clips:
        mm = re.fullmatch(r"[A-Za-z]{2}_(\d+)", c)
        n = int(mm.group(1)) if mm else None
        if n == 1 and cur:
            groups.append(cur)
            cur = 0
        cur += 1
    if cur:
        groups.append(cur)

    cuts = []
    for i in range(count):
        o = CUT_TABLE + i * CUT_STRIDE
        if o + 8 > hsz:
            break
        ln, of = struct.unpack_from("<II", h, o)
        cuts.append({"offset": of, "length": ln})

    # Integrity: the clips must tile the data section exactly. If they do not,
    # the cut table was misread and the offsets must NOT be used.
    dsz = data_size(d, hsz)
    tiles = (len(cuts) == count and all(
        cuts[i]["offset"] + cuts[i]["length"] == cuts[i + 1]["offset"]
        for i in range(len(cuts) - 1))
        and cuts[-1]["offset"] + cuts[-1]["length"] == dsz)

    return {"file": str(path), "bank": bank, "header_count": count,
            "names_read": len(clips), "prefix": prefix,
            "table_offset": start, "groups": groups, "clips": clips,
            "cuts": cuts, "data_size": dsz, "tiles_exactly": tiles}


_STEP = [7,8,9,10,11,12,13,14,16,17,19,21,23,25,28,31,34,37,41,45,50,55,60,66,
    73,80,88,97,107,118,130,143,157,173,190,209,230,253,279,307,337,371,408,449,
    494,544,598,658,724,796,876,963,1060,1166,1282,1411,1552,1707,1878,2066,2272,
    2499,2749,3024,3327,3660,4026,4428,4871,5358,5894,6484,7132,7845,8630,9493,
    10442,11487,12635,13899,15289,16818,18500,20350,22385,24623,27086,29794,32767]
_IDX = [-1,-1,-1,-1,2,4,6,8,-1,-1,-1,-1,2,4,6,8]


def decode_ima(blob, pred=0, idx=0):
    """Continuous IMA ADPCM, same nibble order as rws_extract_wav.decode_stream."""
    out = array.array("h")
    for byte in blob:
        for nib in (byte & 0xF, byte >> 4):
            step = _STEP[idx]
            diff = step >> 3
            if nib & 4: diff += step
            if nib & 2: diff += step >> 1
            if nib & 1: diff += step >> 2
            pred = pred - diff if nib & 8 else pred + diff
            pred = -32768 if pred < -32768 else 32767 if pred > 32767 else pred
            idx += _IDX[nib]
            idx = 0 if idx < 0 else 88 if idx > 88 else idx
            out.append(pred)
    return out, pred, idx


def extract(path, outdir, rate=44100, channels=1):
    """Cut every clip out of the bank.

    IMA is STATEFUL, so cutting at a byte offset is only valid if the decoder
    state resets there. That is checked, not assumed: each clip is decoded twice
    -- once standalone from (pred=0, idx=0) and once as a slice of the fully
    continuous decode -- and any clip whose two decodes differ is reported.
    """
    r = parse(path)
    if not r["tiles_exactly"]:
        raise SystemExit(f"{path}: cut table does not tile the data section; refusing to cut")
    d, h, hsz = read_header(path)
    base = data_offset(d, hsz)
    blob = d[base:base + r["data_size"]]

    cont, _, _ = decode_ima(blob)
    outdir = Path(outdir); outdir.mkdir(parents=True, exist_ok=True)
    mismatches = []
    for i, (nm, c) in enumerate(zip(r["clips"], r["cuts"])):
        piece = blob[c["offset"]:c["offset"] + c["length"]]
        solo, _, _ = decode_ima(piece)
        s0 = c["offset"] * 2
        ref = cont[s0:s0 + len(solo)]
        if solo != ref:
            diff = sum(1 for a, b in zip(solo, ref) if a != b)
            mismatches.append((i, nm, diff, len(solo)))
        w = wave.open(str(outdir / f"{i:03d}_{nm}.wav"), "wb")
        w.setnchannels(channels); w.setsampwidth(2); w.setframerate(rate)
        w.writeframes(solo.tobytes()); w.close()
    print(f"{Path(path).name}: wrote {len(r['clips'])} clips to {outdir}")
    if mismatches:
        print(f"  !! {len(mismatches)} clip(s) differ from the continuous decode "
              f"-- IMA state does NOT reset at those cut points:")
        for i, nm, diff, n in mismatches[:5]:
            print(f"     {i:3d} {nm}: {diff}/{n} samples differ")
    else:
        print("  state-reset check: PASS (every clip decodes identically "
              "standalone and in-stream)")
    return len(mismatches)


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("bank", nargs="?")
    ap.add_argument("--all", action="store_true",
                    help=f"scan every .rws in {ENGLISH}")
    ap.add_argument("--json", action="store_true")
    ap.add_argument("--extract", metavar="DIR",
                    help="cut every clip to DIR as WAV (validates IMA state reset)")
    ap.add_argument("--rate", type=int, default=44100)
    a = ap.parse_args()

    paths = sorted(ENGLISH.glob("*.rws")) if a.all else [Path(a.bank)]
    if not a.bank and not a.all:
        ap.error("give a bank path or --all")

    if a.extract:
        bad = 0
        for p in paths:
            bad += extract(p, Path(a.extract) / p.stem, a.rate)
        return 1 if bad else 0

    out = []
    for p in paths:
        try:
            out.append(parse(p))
        except Exception as e:
            out.append({"file": str(p), "error": str(e)})

    if a.json:
        print(json.dumps(out, indent=2))
        return 0

    for r in out:
        if "error" in r:
            print(f"{Path(r['file']).name}: ERROR {r['error']}")
            continue
        print(f"{Path(r['file']).name}: bank={r['bank']!r} prefix={r['prefix']} "
              f"header_count={r['header_count']} names_read={r['names_read']} "
              f"groups={r['groups']} table@{r['table_offset']} "
              f"tiles={'YES' if r['tiles_exactly'] else 'NO'}")
        if not a.all:
            for i, c in enumerate(r["clips"]):
                print(f"    {i:3d}  {c}")
    return 0


if __name__ == "__main__":
    sys.exit(main())
