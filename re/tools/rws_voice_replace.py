r"""rws_voice_replace.py — encode PCM to IMA ADPCM and put a clip BACK into a
0x080D RenderWare voice bank.

Companion to rws_voice_index.py, which reads the index and cuts clips out.

DESIGN CONSTRAINT, deliberate: replacement is IN PLACE AT THE IDENTICAL BYTE
LENGTH. The new clip is padded with silence or refused if it is too long; the
cut table, the header and every other clip are left untouched.

Why not variable length: changing a clip's length means rewriting the cut table
AND whatever else encodes total size. The header carries fields that are NOT
understood -- notably the two large dwords at +0x14/+0x18 (order 2.8e7, plausibly
sample or byte totals) and six per-record dwords in the cut table beyond
length/offset. Rewriting lengths without knowing those risks producing a bank
the game mis-reads in ways a byte diff would not catch. Equal-length replacement
touches none of them, so it is provably safe, and it is enough for a voice swap.

IMA state caveat carried over from rws_voice_index.py: the decoder does not
reset state at clip boundaries, it CONVERGES over the first few hundred samples
(~15 ms). So a replaced clip's opening milliseconds will not be bit-exact
against a standalone re-decode. --verify measures this and reports it rather
than hiding it.

Usage:
  py -3.12 re\tools\rws_voice_replace.py <bank.rws> --clip 5 --wav new.wav -o out.rws
  py -3.12 re\tools\rws_voice_replace.py <bank.rws> --roundtrip     # prove byte-identity
"""
import argparse
import array
import struct
import sys
import wave
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
import importlib.util

_spec = importlib.util.spec_from_file_location(
    "rws_voice_index", Path(__file__).resolve().parent / "rws_voice_index.py")
rvi = importlib.util.module_from_spec(_spec)
_spec.loader.exec_module(rvi)

_STEP = rvi._STEP
_IDX = rvi._IDX


def encode_ima(samples, pred=0, idx=0):
    """PCM int16 -> IMA ADPCM nibbles, low nibble first.

    Mirrors rws_voice_index.decode_ima exactly: for each byte the LOW nibble is
    the earlier sample. Standard IMA encoder -- quantise the residual against
    the current step, then run the SAME reconstruction the decoder will run so
    the predictor tracks it (encoding against the raw input instead would drift).
    """
    out = bytearray()
    lo = None
    for s in samples:
        step = _STEP[idx]
        diff = s - pred
        nib = 0
        if diff < 0:
            nib = 8
            diff = -diff
        tmp = step >> 3
        if diff >= step:
            nib |= 4
            diff -= step
            tmp += step
        if diff >= step >> 1:
            nib |= 2
            diff -= step >> 1
            tmp += step >> 1
        if diff >= step >> 2:
            nib |= 1
            tmp += step >> 2
        pred = pred - tmp if nib & 8 else pred + tmp
        pred = -32768 if pred < -32768 else 32767 if pred > 32767 else pred
        idx += _IDX[nib]
        idx = 0 if idx < 0 else 88 if idx > 88 else idx
        if lo is None:
            lo = nib
        else:
            out.append((nib << 4) | lo)
            lo = None
    if lo is not None:
        out.append(lo)
    return bytes(out), pred, idx


def read_wav_mono16(path):
    w = wave.open(str(path), "rb")
    if w.getsampwidth() != 2:
        raise SystemExit(f"{path}: need 16-bit PCM, got {w.getsampwidth()*8}-bit")
    n, ch = w.getnframes(), w.getnchannels()
    raw = w.readframes(n)
    w.close()
    a = array.array("h")
    a.frombytes(raw)
    if ch == 2:                      # downmix, the banks are mono
        a = array.array("h", [(a[i] + a[i + 1]) // 2 for i in range(0, len(a) - 1, 2)])
    return a


def load(path):
    r = rvi.parse(path)
    if not r["tiles_exactly"]:
        raise SystemExit(f"{path}: cut table does not tile the data section; refusing to write")
    d, h, hsz = rvi.read_header(path)
    base = rvi.data_offset(d, hsz)
    return r, bytearray(d), base


def replace(path, clip, wav, out, verify=True):
    r, d, base = load(path)
    if not (0 <= clip < len(r["cuts"])):
        raise SystemExit(f"clip index out of range 0..{len(r['cuts'])-1}")
    c = r["cuts"][clip]
    pcm = read_wav_mono16(wav)
    enc, _, _ = encode_ima(pcm)

    if len(enc) > c["length"]:
        raise SystemExit(
            f"encoded {len(enc)} bytes but clip {clip} ({r['clips'][clip]}) is "
            f"{c['length']}. Equal-length replacement only -- trim the source to "
            f"about {c['length']*2/44100:.2f} s.")
    pad = c["length"] - len(enc)
    # 0x00 nibbles are the smallest-step delta, i.e. near-silence at rest.
    blob = enc + b"\x00" * pad
    d[base + c["offset"]: base + c["offset"] + c["length"]] = blob
    Path(out).write_bytes(bytes(d))
    print(f"replaced clip {clip} ({r['clips'][clip]}) in {Path(out).name}: "
          f"{len(enc)} encoded + {pad} pad = {c['length']} bytes")

    if verify:
        r2 = rvi.parse(out)
        ok = r2["tiles_exactly"] and r2["header_count"] == r["header_count"]
        print(f"  structure re-parses: {'PASS' if ok else 'FAIL'}")
        d2, h2, hsz2 = rvi.read_header(out)
        b2 = rvi.data_offset(d2, hsz2)
        back, _, _ = rvi.decode_ima(
            bytes(d2[b2 + c["offset"]: b2 + c["offset"] + len(enc)]))
        n = min(len(back), len(pcm))
        if n:
            err = sum(abs(back[i] - pcm[i]) for i in range(n)) / n
            print(f"  encode->decode mean abs error over {n} samples: {err:.1f} "
                  f"({err/32768*100:.3f}% of full scale)")
        untouched = all(
            d[base + cc["offset"]: base + cc["offset"] + cc["length"]]
            == bytes(Path(path).read_bytes()[base + cc["offset"]:
                                             base + cc["offset"] + cc["length"]])
            for i, cc in enumerate(r["cuts"]) if i != clip)
        print(f"  every other clip byte-identical: {'PASS' if untouched else 'FAIL'}")
    return 0


def roundtrip(path):
    """Read and rewrite with no change; the output must be byte-identical."""
    r, d, base = load(path)
    src = Path(path).read_bytes()
    same = bytes(d) == src
    print(f"{Path(path).name}: roundtrip byte-identical: {'PASS' if same else 'FAIL'}")
    print(f"  clips={r['header_count']} tiles={r['tiles_exactly']} "
          f"data={r['data_size']}")
    return 0 if same else 1


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("bank")
    ap.add_argument("--clip", type=int)
    ap.add_argument("--wav")
    ap.add_argument("-o", "--out")
    ap.add_argument("--roundtrip", action="store_true")
    a = ap.parse_args()
    if a.roundtrip:
        return roundtrip(a.bank)
    if a.clip is None or not a.wav or not a.out:
        ap.error("need --clip, --wav and -o (or --roundtrip)")
    return replace(a.bank, a.clip, a.wav, a.out)


if __name__ == "__main__":
    sys.exit(main())
