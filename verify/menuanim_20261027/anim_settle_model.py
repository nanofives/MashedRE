# Settle-time model of FUN_004325c0 / Anim_Tick under the verbatim frame clock (FUN_00493480,
# MenuNavSM.cpp FrameClockUpdate). Same integer/float math as both bodies: step = trunc(dt*base*K).
# base 1.0 = original (phase<4), base 6.0 = exe kMenuAnimSpeedup. Prints wall-clock ms to settle.
import json, struct, sys
def f32(x): return struct.unpack("<f", struct.pack("<f", x))[0]
def clock(raw_ms, st):
    ms = raw_ms
    if 0x2f <= ms <= 0x35: ms = 0x32
    elif 0x61 <= ms <= 0x67: ms = 0x64
    elif 0x93 <= ms <= 0x99: ms = 0x96
    elif 0xc5 <= ms <= 0xcb: ms = 0xc8
    acc = st[0] + ms; ticks = 0
    if acc >= 0x32: ticks = acc // 50; acc -= ticks * 50
    st[0] = acc
    if acc > 0x2f: ticks += 1; st[0] = 0
    elif acc < 3: st[0] = 0
    return f32(ticks * 0x32 * f32(3.3333333e-4))
def settle(fps, K, base, start=0x1ff):
    st = [0]; t = 0.0; slide = start; frames = 0; period = 1000.0 / fps; acc = 0.0
    while slide > 0 and frames < 100000:
        raw = int(round((frames + 1) * period) - round(frames * period))   # integer ms per frame
        dt = clock(raw, st); frames += 1; t += raw
        slide += int(f32(f32(dt * base) * K))                              # trunc toward zero
    return t, frames
def fpath(fps, base_unused):                                               # float path, no base multiplier
    st = [0]; f8 = 0.0; t = 0.0; frames = 0; period = 1000.0 / fps
    while f8 < 180.0 and frames < 100000:
        raw = int(round((frames + 1) * period) - round(frames * period))
        dt = clock(raw, st); frames += 1; t += raw; f8 += f32(dt * 300.0)
    return t, frames
out = {}
for fps in (30, 60, 144):
    for name, K in (("item_0xff040000_K-2000", -2000.0), ("slide_class_K-800", -800.0)):
        for base, label in ((1.0, "orig_base1"), (2.0, "orig_phase4_base2"), (6.0, "exe_x6"), (12.0, "exe_x6_phase4")):
            t, fr = settle(fps, K, base); out[f"{fps}fps/{name}/{label}"] = {"wall_ms": t, "frames": fr}
    t, fr = fpath(fps, 1.0); out[f"{fps}fps/float_path_K300_to_180/no_base"] = {"wall_ms": t, "frames": fr}
json.dump(out, open(sys.argv[1], "w"), indent=1)
for k, v in out.items(): print(f"{k:60s} {v['wall_ms']:8.0f} ms {v['frames']:5d} frames")
