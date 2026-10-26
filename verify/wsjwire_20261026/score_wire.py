r"""Score the D4-WSJ-WIRE port arms against PREREG_D4-WSJ-WIRE.md predictions 3..5.
usage: py -3.12 -I score_wire.py <A.audio_events.csv> <B.audio_events.csv> <C.audio_events.csv> <ref_A_off.csv> [out.txt]
Prints one `[N/M] name STATUS detail` line per check. Gates (predictions 1, 2) are scored from run_arms.py output.
"""
import csv, sys

EV_NAMES = ['GUN', 'DRUM', 'MISSILE', 'P_MINE', 'SHOTGUN', 'FLASH', 'MORTAR', 'OIL', 'FLAME_ARM', 'FLAME_DEACT']
EV_WAVES = [{'machineg'}, {'rocket'}, {'rocket', 'missile exhaust'}, {'drop mine'}, {'shotgun'}, {'flash'},
            {'rocket', 'mortarexhaust'}, {'oilslick'}, {'flame thrower'}, set()]
IMPACT = {'impact with barrier', 'impact with other vehicle 1', 'impact with crate', 'impact with snowplough', 'mushroomhit'}
SELFTEST_N = 20


def rows(path):
    with open(path, newline='') as f:
        return [r for r in csv.DictReader(f)]


def key(r):
    return (r['kind'], r['arg'], r['resolved'], r['val'])


def main():
    a, b, c, ref = (rows(p) for p in sys.argv[1:5])
    out = []
    checks = []

    # P3: knob OFF log == the D4-WSJ-MUSIC arm A log, ms ignored
    ka, kr = [key(r) for r in a], [key(r) for r in ref]
    first = next((i for i, (x, y) in enumerate(zip(ka, kr)) if x != y), None)
    checks.append(('P3_A_equals_music_A_off', ka == kr,
                   f'rows A={len(ka)} ref={len(kr)} first_diff={first}'))

    # P4 on B
    vst = [r for r in b if r['kind'] == 'vstart']
    seg = []
    on = False
    for r in b:
        if r['kind'] == 'music_state':
            on = r['val'].startswith('2')
            continue
        if on:
            seg.append(r)
    checks.append(('P4_no_cdaudio_in_race', not any(r['kind'] == 'music_start' for r in seg),
                   f'music_start rows in race={sum(r["kind"] == "music_start" for r in seg)}'))
    n321 = [r for r in vst if r['arg'] == 'threetwoone']
    ngo = [r for r in vst if r['arg'] == 'go']
    checks.append(('P4_threetwoone_once_v65', len(n321) == 1 and n321[0]['resolved'] == 'v65',
                   f'n={len(n321)} voices={[r["resolved"] for r in n321]}'))
    checks.append(('P4_go_once_v95', len(ngo) == 1 and ngo[0]['resolved'] == 'v95',
                   f'n={len(ngo)} voices={[r["resolved"] for r in ngo]}'))
    cls = {int(float(r['val'])): r['resolved'] for r in b if r['kind'] == 'engine_class'}
    eng_ok, eng_det = True, []
    for car, wave in sorted(cls.items()):
        k = int(wave[3])                     # eng<k>
        v_main, v_d = 8 * (k - 1) + car, 8 * (k - 1) + 4 + car
        m = [r for r in vst if r['arg'] == wave and int(float(r['val'])) == car]
        d = [r for r in vst if r['arg'] == wave + 'd' and int(float(r['val'])) == car]
        ok = len(m) == 1 and m[0]['resolved'] == f'v{v_main}' and len(d) == 1 and d[0]['resolved'] == f'v{v_d}'
        eng_ok &= ok
        eng_det.append(f'car{car}:{wave} main={len(m)}@{[r["resolved"] for r in m]} d={len(d)}@{[r["resolved"] for r in d]}')
    checks.append(('P4_engine_main_and_d_per_car', eng_ok and len(cls) > 0, '; '.join(eng_det) or 'no engine_class rows'))
    bad = [r for r in vst if r['arg'] == 'fire' or r['arg'] in IMPACT]
    gtot = {r['arg']: r['val'] for r in b if r['kind'] == 'guard_closed_total'}
    checks.append(('P4_no_fire_no_impact_starts', not bad, f'starts={len(bad)} guard_closed_total={gtot}'))
    pu_bad, pu_n, cur = [], 0, None
    for r in b:
        if r['kind'] == 'pu_event':
            cur = int(float(r['val'])); pu_n += 1
        elif r['kind'] == 'vstart' and cur is not None:
            if r['arg'] not in EV_WAVES[cur]:
                pu_bad.append(f'{EV_NAMES[cur]}->{r["arg"]}')
        elif r['kind'] not in ('vbusy', 'vfull', 'vstop', 'guard_closed'):
            cur = None
    pods = [r for r in vst if r['arg'] == 'collision with pod']
    checks.append(('P4_powerup_pickup_waves', not pu_bad,
                   f'pu_events={pu_n} wrong={pu_bad} pod_starts={len(pods)}'))

    # P5: self-test
    st = [r for r in c if r['kind'] == 'selftest']
    fails = [r['arg'] for r in st if r['resolved'] != 'PASS']
    checks.append(('P5_selftest_all_pass', len(st) == SELFTEST_N and not fails, f'rows={len(st)} fail={fails}'))

    n = len(checks)
    for i, (nm, ok, det) in enumerate(checks, 1):
        line = f'[{i}/{n}] {nm} {"PASS" if ok else "FAIL"} {det}'
        print(line, flush=True)
        out.append(line)
    if len(sys.argv) > 5:
        open(sys.argv[5], 'w').write('\n'.join(out) + '\n')


if __name__ == '__main__':
    main()
