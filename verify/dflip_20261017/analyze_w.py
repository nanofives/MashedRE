"""Warzone live arms (W_*): object cycles from <arm>.pontoon.txt, AI reader totals from <arm>.seek.txt.
    py -3.12 analyze_w.py <arm> [<arm> ...]"""
import os
import re
import struct
import sys

HERE = os.path.join(os.path.dirname(os.path.abspath(__file__)), 'arms')


def f(h):
    return struct.unpack('<f', struct.pack('<I', int(h, 16)))[0]


def main():
    out = []
    for arm in sys.argv[1:]:
        pt = os.path.join(HERE, arm + '.pontoon.txt')
        sk = os.path.join(HERE, arm + '.seek.txt')
        line = f'{arm}:'
        if os.path.exists(pt):
            rows = [l for l in open(pt) if l.startswith('pontoon n=')]
            init = [l.strip() for l in open(pt) if l.startswith('pontoon init')]
            trans, words, states = [], 0, {}
            maxc8 = 0.0
            for l in rows:
                m = re.search(r'n=(\d+) units=(-?\d+) state (\d)->(\d) a4=(\w+) c8=(\w+) ff50=(\d{4}) b4=(\d{4})', l)
                if not m:
                    continue
                n, u, a, b, a4, c8, ff, b4 = m.groups()
                states[b] = states.get(b, 0) + 1
                if a != b:
                    trans.append((int(n), int(a), int(b), ff, round(f(c8), 4)))
                if ff != '0000':
                    words += 1
                maxc8 = max(maxc8, f(c8))
            line += f' init={"ok=1" in (init[0] if init else "")} rows={len(rows)} state_rows={states} word_rows={words} max_c8={maxc8:.4f}'
            line += ' transitions=' + ' '.join(f'{n}:{a}->{b}(ff{ff},c8={c})' for n, a, b, ff, c in trans[:12])
        else:
            line += ' no pontoon trace (knob off or object inactive)'
        if os.path.exists(sk):
            ls = [l.strip() for l in open(sk) if l.startswith('seek414f00')]
            if ls:
                m = re.search(r'calls=(\d+) gate1=(\d+) ret1=(\d+)', ls[-1])
                r1 = [l for l in ls if ' ret=1 ' in l]
                line += f' | seek last: calls={m.group(1)} gate1={m.group(2)} ret1={m.group(3)}; ret=1 lines {len(r1)}'
                bad = [l for l in r1 if not re.search(r'ff50v=00000001', l)]
                line += f' (ret=1 without the word: {len(bad)})'
        out.append(line)
    txt = '\n'.join(out)
    print(txt)
    open(os.path.join(HERE, 'W_summary.txt'), 'a').write(txt + '\n')


if __name__ == '__main__':
    main()
