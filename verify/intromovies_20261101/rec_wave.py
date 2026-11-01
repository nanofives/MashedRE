r"""D5-INTROMOVIES-AUDIO loopback recorder, standard library only (ctypes + winmm MCI, no package).
Records the default capture device (on this machine the only one is "CABLE Output (VB-Audio Virtual
Cable)", the loopback of the default render endpoint "Altavoces (VB-Audio Virtual Cable)") to a WAV.
usage: py -3.12 rec_wave.py <out.wav> <seconds>         record
       py -3.12 rec_wave.py --selftest <dir>             play a 1 kHz tone while recording, report its RMS
"""
import ctypes, math, os, struct, sys, time, wave

winmm = ctypes.WinDLL("winmm")


def mci(cmd):
    buf = ctypes.create_unicode_buffer(256)
    rc = winmm.mciSendStringW(cmd, buf, 256, None)
    if rc:
        err = ctypes.create_unicode_buffer(256)
        winmm.mciGetErrorStringW(rc, err, 256)
        raise RuntimeError(f"MCI '{cmd}' rc={rc} {err.value}")
    return buf.value


def record(out, secs, on_start=None):
    mci("open new type waveaudio alias rec")
    try:
        mci("set rec time format ms bitspersample 16 channels 2 samplespersec 44100 bytespersec 176400 alignment 4")
        mci("record rec")
        t0 = time.time()
        if on_start:
            on_start()
        while time.time() - t0 < secs:
            time.sleep(0.2)
        mci("stop rec")
        mci(f'save rec "{os.path.abspath(out)}"')
    finally:
        mci("close rec")
    return t0


def rms_envelope(path, win_ms=100):
    with wave.open(path) as w:
        n, ch, sr = w.getnframes(), w.getnchannels(), w.getframerate()
        raw = w.readframes(n)
    s = struct.unpack(f"<{n * ch}h", raw)
    step = sr * win_ms // 1000
    env = []
    for i in range(0, n - step + 1, step):
        seg = s[i * ch:(i + step) * ch]
        env.append(math.sqrt(sum(v * v for v in seg) / len(seg)) / 32768.0)
    return env, n / sr


def selftest(d):
    os.makedirs(d, exist_ok=True)
    tone = os.path.join(d, "tone_1k.wav")
    with wave.open(tone, "wb") as w:
        w.setnchannels(1); w.setsampwidth(2); w.setframerate(44100)
        w.writeframes(b"".join(struct.pack("<h", int(8000 * math.sin(2 * math.pi * 1000 * i / 44100))) for i in range(44100 * 2)))
    import winsound
    out = os.path.join(d, "selftest_rec.wav")
    record(out, 4.0, on_start=lambda: (time.sleep(1.0), winsound.PlaySound(tone, winsound.SND_FILENAME | winsound.SND_ASYNC)))
    env, dur = rms_envelope(out)
    on = sum(1 for e in env if e > 0.01)
    print(f"selftest dur={dur:.2f}s windows={len(env)} loud(>0.01)={on} max_rms={max(env):.4f} "
          f"{'PASS' if 15 <= on <= 25 else 'FAIL'}")


if __name__ == "__main__":
    if sys.argv[1] == "--selftest":
        selftest(sys.argv[2])
    else:
        record(sys.argv[1], float(sys.argv[2]))
