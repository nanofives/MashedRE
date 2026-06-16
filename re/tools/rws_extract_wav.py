#!/usr/bin/env python3
"""rws_extract_wav.py — extract PCM waves from a Mashed RenderWare RWS audio bank.

Format (cracked 2026-06-15, verified on arctic.rws; see re/analysis/audio_rws_loader/):
  0x809  wrapper
    0x80A  bank header (68B): ends with ASCII "<bank>\0<name>.WAV"
    0x80C  data
      u32  waveCount
      waveCount x:
        0x802  wave wrapper
          0x803  format descriptor (176B):
                   +4  u32 sampleRate (e.g. 0x5622 = 22050)
                   +12 u32 pcmByteSize (matches the following 0x804 size)
                   +112 ASCIIZ wave name ("tire on wet tarmac", ...)
                 (two format sub-blocks + two 16-byte format GUIDs; the loader
                  FUN_005a7b60 compares these against DAT_005e6414/0x005e6444)
          0x804  PCM sample data (16-bit; stereo per the bank's ".WAV" name)

Usage:
  rws_extract_wav.py <bank.rws> [--list] [--out DIR] [--channels 1|2]
  --list      : just print the wave table (name, rate, bytes, seconds)
  --out DIR   : write each wave as DIR/<NN>_<name>.wav (default: no write)
  --channels  : interpret PCM as mono/stereo for the WAV header (default 2)
"""
import struct, sys, os, wave, argparse, array

def chunks(data, off, end):
    """Yield (type, size, version, data_off) for chunks in [off,end)."""
    while off + 12 <= end:
        t, s, v = struct.unpack_from("<III", data, off)
        yield t, s, v, off + 12
        off += 12 + s

def parse(path):
    data = open(path, "rb").read()
    # 0x809 wrapper at 0
    t, s, v = struct.unpack_from("<III", data, 0)
    assert t == 0x809, f"not an RWS audio bank (top type {t:#x})"
    bank_name = ""
    waves = []
    for t, s, v, d in chunks(data, 12, 12 + s):
        if t == 0x80A:
            # trailing ASCII bank/name
            tail = data[d:d+s]
            bank_name = tail.split(b"\x00")[-1].decode("latin1", "ignore")
        elif t == 0x80C:
            wave_count = struct.unpack_from("<I", data, d)[0]
            # waves begin right after the count
            for wt, ws, wv, wd in chunks(data, d + 4, d + s):
                if wt != 0x802:
                    continue
                rate = 0; pcm = b""; name = ""
                for st, ss, sv, sd in chunks(data, wd, wd + ws):
                    if st == 0x803:
                        rate = struct.unpack_from("<I", data, sd + 4)[0]
                        # ASCIIZ name near +112
                        raw = data[sd:sd+ss]
                        z = raw.find(b"\x00", 112)
                        name = raw[112:z if z > 0 else 112+24].decode("latin1", "ignore").strip()
                    elif st == 0x804:
                        pcm = data[sd:sd+ss]
                waves.append((name, rate, pcm))
    return bank_name, waves

# --- 0x80d streamed bank (continuous IMA ADPCM, 44100; cracked 2026-06-16) -----
_IMA_STEP = [7,8,9,10,11,12,13,14,16,17,19,21,23,25,28,31,34,37,41,45,50,55,60,66,
    73,80,88,97,107,118,130,143,157,173,190,209,230,253,279,307,337,371,408,449,
    494,544,598,658,724,796,876,963,1060,1166,1282,1411,1552,1707,1878,2066,2272,
    2499,2749,3024,3327,3660,4026,4428,4871,5358,5894,6484,7132,7845,8630,9493,
    10442,11487,12635,13899,15289,16818,18500,20350,22385,24623,27086,29794,32767]
_IMA_IDX = [-1,-1,-1,-1,2,4,6,8,-1,-1,-1,-1,2,4,6,8]

def decode_stream(path, max_samples=0):
    """Decode a 0x80d streamed RWS bank -> (pcm bytes, rate). Continuous IMA."""
    data = open(path, "rb").read()
    t, s, v = struct.unpack_from("<III", data, 0)
    assert t == 0x80D, f"not a 0x80d streamed bank (top {t:#x})"
    rate = 44100
    for i in range(12, min(len(data) - 4, 12000)):
        x = struct.unpack_from("<I", data, i)[0]
        if x in (44100, 22050, 32000, 48000):
            rate = x; break
    best_off = best_len = 0
    off = 12
    while off + 12 <= len(data):
        ct, cs, cv = struct.unpack_from("<III", data, off)
        cd = off + 12
        cs = min(cs, len(data) - cd)
        if not (ct == 0x80E and off == 12):
            if cs > best_len:
                best_off, best_len = cd, cs
        off = cd + cs
    pred = 0; idx = 0
    out = array.array("h")
    blob = data[best_off:best_off + best_len]
    for byte in blob:
        for nib in (byte & 0xF, byte >> 4):
            step = _IMA_STEP[idx]; diff = step >> 3
            if nib & 4: diff += step
            if nib & 2: diff += step >> 1
            if nib & 1: diff += step >> 2
            pred = pred - diff if nib & 8 else pred + diff
            pred = -32768 if pred < -32768 else 32767 if pred > 32767 else pred
            idx += _IMA_IDX[nib]; idx = 0 if idx < 0 else 88 if idx > 88 else idx
            out.append(pred)
        if max_samples and len(out) >= max_samples:
            break
    return out.tobytes(), rate

def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("rws")
    ap.add_argument("--list", action="store_true")
    ap.add_argument("--out")
    ap.add_argument("--channels", type=int, default=2)
    ap.add_argument("--stream", help="decode a 0x80d streamed bank to this WAV")
    ap.add_argument("--seconds", type=int, default=0, help="cap --stream length")
    a = ap.parse_args()
    if a.stream:
        pcm, rate = decode_stream(a.rws, a.seconds * 44100)
        with wave.open(a.stream, "wb") as w:
            w.setnchannels(1); w.setsampwidth(2); w.setframerate(rate); w.writeframes(pcm)
        print(f"decoded {len(pcm)//2} samples @{rate} Hz -> {a.stream}")
        return
    bank, waves = parse(a.rws)
    print(f"bank='{bank}'  waves={len(waves)}")
    for i, (name, rate, pcm) in enumerate(waves):
        frames = len(pcm) // (2 * a.channels)
        secs = frames / rate if rate else 0
        print(f"  [{i:2d}] rate={rate:5d} bytes={len(pcm):8d} "
              f"~{secs:5.2f}s  '{name}'")
    if a.out:
        os.makedirs(a.out, exist_ok=True)
        for i, (name, rate, pcm) in enumerate(waves):
            safe = "".join(c if c.isalnum() else "_" for c in name)[:24] or f"w{i}"
            p = os.path.join(a.out, f"{i:02d}_{safe}.wav")
            with wave.open(p, "wb") as w:
                w.setnchannels(a.channels); w.setsampwidth(2)
                w.setframerate(rate or 22050); w.writeframes(pcm)
            print(f"   wrote {p}")

if __name__ == "__main__":
    main()
