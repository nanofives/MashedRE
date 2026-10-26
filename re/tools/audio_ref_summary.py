"""D4-WSJ-REF: summarize an original-side audio capture (scenario_launch.py --audio-probe) and,
optionally, a standalone MASHED_AUDIO_EVENT_LOG csv, into per-wave tables.

Original side:
  <prefix>.audio.csv      tick,phase,state,kind,a0,a1,a2,voice_idx,ret   (AUDIO_AGENT rows)
  <prefix>.audiosnap.json the 'end' snap holds the wave descriptor tables raw:
      permdict 0x006044f0 stride 0x94, course *0x0088e670 stride 0x90;
      name = C string at +0, voice base = s32 at +0x80, voice count = s32 at +0x84 (read from the bytes,
      checked: bases are contiguous 0..DAT_0068fc84-1, then the course bank).
  A voice start is kind=voice with a1 (selector) != 0 (bucket plate 005a66d0.md: sel != 0 -> FUN_005a7560).
  Rounds: DAT_007f101c restarts at each round (zeroed at race start 0x0040ff17), so a tick drop opens a
  new segment.

usage: py -3.12 re/tools/audio_ref_summary.py --orig <prefix> [--orig <prefix> ...] [--port <csv> ...] [--md out.md]
"""
import argparse, collections, csv, json, os, struct, sys


def cstr(b):
    e = b.find(b'\0')
    b = b[:e if e >= 0 else len(b)]
    return b.decode('latin1')


def tables(snap_path):
    s = json.load(open(snap_path))
    sn = [x for x in s['snaps'] if x['label'] == 'end'][0]
    waves = []   # (bank, idx, name, base, count)
    perm = bytes.fromhex(sn['perm'])
    for w in range(len(perm) // 0x94):
        e = perm[w * 0x94:(w + 1) * 0x94]
        waves.append(('perm', w, cstr(e[:0x80]), struct.unpack_from('<i', e, 0x80)[0], struct.unpack_from('<i', e, 0x84)[0]))
    c = bytes.fromhex(sn['course'])
    for j in range(len(c) // 0x90):
        e = c[j * 0x90:(j + 1) * 0x90]
        waves.append(('course', j, cstr(e[:0x80]), struct.unpack_from('<i', e, 0x80)[0], struct.unpack_from('<i', e, 0x84)[0]))
    v2w = {}
    for bank, i, name, base, cnt in waves:
        for v in range(base, base + cnt):
            v2w[v] = (bank, i, name)
    return waves, v2w, sn


def orig_summary(prefix):
    waves, v2w, sn = tables(prefix + '.audiosnap.json')
    perm_names = {i: n for b, i, n, _, _ in waves if b == 'perm'}
    starts = collections.Counter(); stops = collections.Counter(); asks = collections.Counter()
    first_tick = {}; callers = collections.defaultdict(collections.Counter)
    seg = 0; last = None; seg_of_first = {}; kinds = collections.Counter(); unmapped = 0
    per_seg = collections.defaultdict(collections.Counter)
    for r in csv.DictReader(open(prefix + '.audio.csv')):
        t = int(r['tick']); k = r['kind']; kinds[k] += 1
        if r['phase'] != '3':
            continue
        if last is not None and t < last:
            seg += 1
        last = t
        if k == 'voice':
            vi = int(r['voice_idx'])
            w = v2w.get(vi)
            if w is None:
                unmapped += 1; continue
            name = w[2]
            if int(r['a1']) != 0:
                starts[name] += 1; per_seg[seg][name] += 1; callers[name][r['ret']] += 1
                if name not in first_tick: first_tick[name] = t; seg_of_first[name] = seg
            else:
                stops[name] += 1
        elif k in ('oneshot', 'oneshot_slot', 'oneshot_pos_slot'):
            asks[perm_names.get(int(r['a0']), 'w%s' % r['a0'])] += 1
    return dict(waves=waves, starts=starts, stops=stops, asks=asks, first_tick=first_tick, seg_of_first=seg_of_first,
                callers=callers, segs=seg + 1, kinds=kinds, unmapped=unmapped, per_seg=per_seg, g=sn['g'])


def port_summary(path):
    kinds = collections.Counter(); names = collections.Counter(); resolved = {}
    for r in csv.DictReader(open(path)):
        kinds[r['kind']] += 1
        key = r['kind'] + ':' + r['arg']
        names[key] += 1; resolved[key] = r['resolved']
    return dict(kinds=kinds, names=names, resolved=resolved)


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('--orig', action='append', default=[])
    ap.add_argument('--port', action='append', default=[])
    ap.add_argument('--md', default='')
    a = ap.parse_args()
    out = []
    for p in a.orig:
        s = orig_summary(p)
        out.append(f'## original {p}')
        out.append(f'rows by kind: {dict(s["kinds"])}; race segments (rounds) {s["segs"]}; unmapped voice rows {s["unmapped"]}')
        out.append(f'globals at end: {s["g"]}')
        out.append('')
        out.append('| bank | idx | wave | voices | starts (phase 3) | stops | oneshot asks | first tick | top caller ret |')
        out.append('|---|---|---|---|---|---|---|---|---|')
        for bank, i, name, base, cnt in s['waves']:
            c = s['callers'].get(name)
            top = ', '.join(f'{k}x{v}' for k, v in c.most_common(2)) if c else ''
            out.append(f'| {bank} | {i} | {name} | {base}..{base + cnt - 1} | {s["starts"][name]} | {s["stops"][name]} | '
                       f'{s["asks"].get(name, 0) if bank == "perm" else ""} | {s["first_tick"].get(name, "")} | {top} |')
        out.append('')
    for p in a.port:
        s = port_summary(p)
        out.append(f'## port {p}')
        out.append(f'rows by kind: {dict(s["kinds"])}')
        out.append('')
        out.append('| kind:arg | n | resolved |')
        out.append('|---|---|---|')
        for k, n in sorted(s['names'].items()):
            out.append(f'| {k} | {n} | {s["resolved"][k]} |')
        out.append('')
    if a.orig and a.port:
        # One row per wave name: race-phase voice starts in each original capture vs the port's requests that
        # resolve to that bank wave (sfx / music_bankloop by the resolved name; engine_start = eng1; loop rows by
        # their wave argument). Port rows with no original wave of that name are listed after.
        os_ = [orig_summary(p) for p in a.orig]; ps = [port_summary(p) for p in a.port]
        pn = []
        for s in ps:
            c = collections.Counter()
            for k, n in s['names'].items():
                kind, arg = k.split(':', 1)
                if kind in ('sfx', 'music_bankloop'): c[s['resolved'][k] or ('?' + arg)] += n
                elif kind == 'engine_start': c['eng1'] += n
                elif kind in ('loop', 'once'): c[arg] += n
                elif kind == 'music_start': c[arg] += n
            pn.append(c)
        out.append('## compare (original race-phase voice starts vs port requests)')
        out.append('')
        out.append('| wave | ' + ' | '.join(f'orig {os.path.basename(p)}' for p in a.orig) + ' | '
                   + ' | '.join(f'port {os.path.basename(p)}' for p in a.port) + ' |')
        out.append('|---|' + '---|' * (len(a.orig) + len(a.port)))
        seen = set()
        for bank, i, name, base, cnt in os_[0]['waves']:
            vals = [s['starts'][name] for s in os_] + [c.get(name, 0) for c in pn]
            seen.add(name)
            if any(vals):
                out.append(f'| {name} | ' + ' | '.join(str(v) for v in vals) + ' |')
        for c in pn:
            for k in c:
                if k not in seen:
                    seen.add(k)
                    out.append(f'| {k} (port only) | ' + ' | '.join('0' for _ in a.orig) + ' | '
                               + ' | '.join(str(cc.get(k, 0)) for cc in pn) + ' |')
        out.append('')
    txt = '\n'.join(out)
    print(txt)
    if a.md:
        open(a.md, 'w', encoding='utf-8').write(txt + '\n')


if __name__ == '__main__':
    sys.exit(main())
