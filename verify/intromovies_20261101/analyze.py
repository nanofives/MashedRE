r"""D5-INTROMOVIES-AUDIO scoring. Per recording (<outdir>/<arm>.wav): 100 ms RMS envelope (full scale = 1),
noise floor = max RMS over the PRE window before the launch, sounding segments = runs of windows with
RMS > THR (gaps under 0.5 s merged), times relative to the launch. Per standalone arm: the INTRO lines of
its log. Pairs: Pearson r of the log-envelopes (20*log10(rms+1e-4)) of an O and an S arm at the lag in
[-10 s, +10 s] that maximises it, over the first SPAN s after the later stream's first sound.
usage: py -3.12 analyze.py <outdir> <arm> [...]
"""
import math, os, re, struct, sys, wave

THR, WIN, SPAN = 0.003, 0.1, 85.0


def env_of(path):
    with wave.open(path) as w:
        n, ch, sr = w.getnframes(), w.getnchannels(), w.getframerate()
        raw = w.readframes(n)
    s = struct.unpack(f'<{n * ch}h', raw)
    step = int(sr * WIN)
    out = []
    for i in range(0, n - step + 1, step):
        seg = s[i * ch:(i + step) * ch]
        out.append(math.sqrt(sum(v * v for v in seg) / len(seg)) / 32768.0)
    return out, n / sr


def meta(d, a):
    m = {}
    for l in open(os.path.join(d, a + '.meta')):
        k, _, v = l.rstrip('\n').partition('=')
        m[k] = v
    return m


def segments(env, off):
    on = [i for i, e in enumerate(env) if e > THR]
    segs = []
    for i in on:
        if segs and i - segs[-1][1] <= int(0.5 / WIN):
            segs[-1][1] = i
        else:
            segs.append([i, i])
    return [(round(a * WIN - off, 1), round((b + 1) * WIN - off, 1)) for a, b in segs]


def pearson(x, y):
    n = len(x)
    mx, my = sum(x) / n, sum(y) / n
    sx = math.sqrt(sum((v - mx) ** 2 for v in x)); sy = math.sqrt(sum((v - my) ** 2 for v in y))
    return sum((a - mx) * (b - my) for a, b in zip(x, y)) / (sx * sy) if sx and sy else 0.0


def main():
    d, arms = sys.argv[1], sys.argv[2:]
    E = {}
    for a in arms:
        env, dur = env_of(os.path.join(d, a + '.wav'))
        m = meta(d, a)
        off = float(m['launch_offset_s'])
        pre = env[:int(off / WIN)]
        segs = segments(env, off)
        first = next((i for i, e in enumerate(env) if e > THR and i * WIN > off), None)
        E[a] = (env, off, first)
        sounding = sum(1 for i, e in enumerate(env) if e > THR and i * WIN > off) * WIN
        print(f'## {a}  pid={m["pid"]} alive_at_wall={m["alive_at_wall"]} exe_sha16={m["exe_sha16"]} rec={dur:.1f}s launch at {off:.2f}s')
        print(f'noise floor (pre-launch max RMS) {max(pre) if pre else float("nan"):.5f}, peak RMS {max(env):.4f}, '
              f'first sound {first * WIN - off if first is not None else float("nan"):.1f}s after launch, sounding {sounding:.1f}s')
        print('segments (s after launch, >= 1 s): ' + ', '.join(f'{s0}-{s1}' for s0, s1 in segs if s1 - s0 >= 1.0))
        lp = os.path.join(d, a + '.log')
        if os.path.exists(lp):
            for l in open(lp, encoding='latin1'):
                if l.startswith('INTRO'):
                    print('    ' + re.sub(r'original/toastart/pc/movies/', '', l.rstrip()))
        print()
    os_ = [a for a in arms if a.startswith('O')]
    ss = [a for a in arms if a.startswith('S')]
    print('## envelope correlation (log-RMS, best lag in +-10 s, %.0f s span)' % SPAN)
    print('| pair | lag s (S later = +) | r |')
    print('|---|---|---|')
    pairs = [(x, y) for i, x in enumerate(arms) for y in arms[i + 1:]]
    for x, y in pairs:
        ex, _, fx = E[x]; ey, _, fy = E[y]
        if fx is None or fy is None:
            print(f'| {x} vs {y} | - | no sound |'); continue
        n = int(SPAN / WIN)
        lx = [20 * math.log10(v + 1e-4) for v in ex]; ly = [20 * math.log10(v + 1e-4) for v in ey]
        best = (-2.0, 0)
        for lag in range(-int(10 / WIN), int(10 / WIN) + 1):
            a0, b0 = fx, fy + lag
            if b0 < 0 or a0 + n > len(lx) or b0 + n > len(ly):
                continue
            r = pearson(lx[a0:a0 + n], ly[b0:b0 + n])
            if r > best[0]:
                best = (r, lag)
        print(f'| {x} vs {y} | {(fy + best[1] - fx) * WIN:+.1f} (onset-aligned residual {best[1] * WIN:+.1f}) | {best[0]:.3f} |')


if __name__ == '__main__':
    main()
