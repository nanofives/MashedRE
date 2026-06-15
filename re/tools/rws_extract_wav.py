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
import struct, sys, os, wave, argparse

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

def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("rws")
    ap.add_argument("--list", action="store_true")
    ap.add_argument("--out")
    ap.add_argument("--channels", type=int, default=2)
    a = ap.parse_args()
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
