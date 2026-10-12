"""negctl re-earn, race lane (2026-10-11): build the per-batch decision TSVs for the 51 scenario='race' entries
(50 hooks.csv rows) of re/analysis/negctl/race_uncovered_20261011.txt.

Same keep criterion and decision format as reearn_20261011 (reearn_judge.judge, reearn_apply.py columns). Runs:
  triage  log/negctl-race/triage   every entry, registry as of 40ff5c67
  fx1     log/negctl-race/fx1      the 19 failing entries, re-run with re/frida/negctl_fixtures/race.py
  fx2     log/negctl-race/fx2      slot_object_field8 and camera_entry_dispatch_all after their fixture fix
  fx3     log/negctl-race/fx3      car_snapshot_dword_get (its run_diff evidence boot read degenerate live values)
For each entry the latest run that covered it decides. Keep rows copy log/diff_<hook>.csv (run_diff.py, same fixture)
to reearn_20261011/evidence/ and the deciding negctl TSV to reearn_20261011/runs/<batch>/.
Demote / hold rows come from DEMOTE / HOLD below (decided by hand, reasons cited there).

Usage: py -3.12 re/analysis/negctl/race_reearn_build.py   (writes reearn_20261011/decisions/race_r*.tsv)
"""
import csv, glob, shutil, sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[3]
sys.path.insert(0, str(ROOT / 're' / 'analysis' / 'negctl'))
import reearn_judge as J                                    # noqa: E402

BASE = ROOT / 're' / 'analysis' / 'negctl' / 'reearn_20261011'
RUNS = {k: sorted(glob.glob(str(ROOT / 'log' / 'negctl-race' / k / 'negctl_2*.tsv'))) for k in ('triage', 'fx1', 'fx2', 'fx3')}
FX1 = set(open(ROOT / 'log' / 'negctl-race' / 'fail19.txt').read().strip().split(','))
FX2 = {'slot_object_field8', 'camera_entry_dispatch_all'}
FX3 = {'car_snapshot_dword_get'}

DEMOTE = {
    'sa2_car_event_trigger': (
        'C2', 'fx1',
        'Port defect: the original indexes DAT_007f1a14 by car*16 (shl esi,4 at 0x004922fd) and tests the dword '
        'guard at 0x007f105c + slot*0x4c (mov ecx,[eax+4] at 0x0049230e); the port (HUD/ScenarioWriters_sa2s2.cpp '
        'CarEventTrigger) uses car*4 and an int8 at slot*0x13, so it skips the write the original makes. Race fixture '
        '(re/frida/negctl_fixtures/race.py, every guard driven): real RED 2/6 (idx 0 and 1, re/analysis/negctl/reearn_20261011/evidence/red_diff_sa2_car_event_trigger.csv), '
        'baseline GREEN. The old entry never reached the write (DAT_0063ba8c = 3 in the race), so its GREEN was null-blind.'),
    'camera_entry_dispatch_all': (
        'C2', 'fx2',
        'Old int_scalar entry observed EAX of a void body (decomp 0x004464c0), so its GREEN said nothing about the port. '
        'Race fixture observing the entry array 0x008964c0..0x00898997 (game thread frozen, one original prep call per '
        'arm) reads baseline GREEN and real RED 4/4 (re/analysis/negctl/reearn_20261011/evidence/red_diff_camera_entry_dispatch_all.csv): the port writes '
        'different bytes than the original from the same state. [UNCERTAIN] which callee port differs (0x00445aa0 / '
        '0x00441d40 / 0x00442440, all x87 float paths).'),
}
HOLD = {
    'rw_com_release_thunk': (
        'fx1',
        'Field wrong-port controls cannot apply by construction: the body is mov eax,[esp+4]; push eax; mov ecx,[eax]; '
        'call [ecx+8]; ret (0x004cbb50..0x004cbb5a), no store except the push. The bytes the field kit saw change and '
        'flipped (0x103dbb7c, 0x0ffc1d87, 0x10066097: outside MASHED.exe) are written by the harness recorder stub, not '
        'by the port. real GREEN, baseline GREEN, null RED, ret RED in triage and fx1 (two boots).'),
    'rws_stream_read': (
        'fx1',
        'Field wrong-port controls cannot apply by construction: the body (0x00550950..0x00550978) stores only its '
        'pushes, calls [[p4+0x38]+0x28+8] and returns eax / p2 (div at 0x00550975). Both boots ERRORed the field classes '
        '(triage: field_last timeout; fx1: field_first process died, field_last and field_none timeout). real GREEN, '
        'baseline GREEN, null RED, ret RED in both.'),
}


def entries():
    out = []
    for l in open(ROOT / 're' / 'analysis' / 'negctl' / 'race_uncovered_20261011.txt'):
        k, rva, at, lvl = l.split()
        out.append((k, rva.lower(), lvl))
    return out


def tsvs_for(k):
    t = list(RUNS['triage'])
    if k in FX1:
        t += RUNS['fx1']
    if k in FX2:
        t += RUNS['fx2']
    if k in FX3:
        t += RUNS['fx3']
    return t


def main():
    ents = entries()
    hk = {r['rva'].lower(): r for r in csv.DictReader(open(ROOT / 'hooks.csv', encoding='utf-8'))}
    by_rva = {}
    for k, rva, lvl in ents:
        by_rva.setdefault(rva, []).append(k)
    order = sorted(by_rva, key=lambda r: (hk[r[2:].zfill(8)]['confidence'] != 'C4', r))
    batches = [order[i:i + 15] for i in range(0, len(order), 15)]
    (BASE / 'evidence').mkdir(parents=True, exist_ok=True)
    tally = {'keep': 0, 'demote': 0, 'hold': 0}
    for bi, rvas in enumerate(batches, 1):
        name = f'race_r{bi}'
        runs = BASE / 'runs' / name
        runs.mkdir(parents=True, exist_ok=True)
        rows, held = [], []
        for rva in rvas:
            r8 = rva[2:].zfill(8)
            old = hk[r8]['confidence']
            keys = by_rva[rva]
            cites, ok_all, why_all, ev = [], True, [], []
            for k in keys:
                t = tsvs_for(k)
                jobs = J.load(t)
                ok, why, summ = J.judge(jobs, k)
                src = Path(t[-1])
                shutil.copy2(src, runs / src.name)
                cites.append(f'{(runs / src.name).relative_to(ROOT).as_posix()} [{k}] {summ}')
                ok_all &= ok
                why_all += [f'{k}: {w}' for w in why]
                if ok:
                    csvp = ROOT / 'log' / f'diff_{k}.csv'
                    if not csvp.exists():
                        sys.exit(f'{k}: {csvp} missing, run run_diff.py first')
                    shutil.copy2(csvp, BASE / 'evidence' / csvp.name)
                    ev.append((BASE / 'evidence' / csvp.name).relative_to(ROOT).as_posix())
            k0 = keys[0]
            if k0 in HOLD:
                held.append([rva, k0, 'hold', old, old, '', '; '.join(cites), HOLD[k0][1]])
                tally['hold'] += 1
            elif k0 in DEMOTE:
                new = DEMOTE[k0][0]
                rows.append([rva, k0, 'demote', old, new, '', '; '.join(cites), DEMOTE[k0][2]])
                tally['demote'] += 1
            elif ok_all:
                why = ('Race lane (run_negctl --race, live Quick Battle; gamesave reads pinned to the reference copy, '
                       'sha256 of original/gamesave.bin unchanged across every batch).')
                if k0 in FX1 or k0 in FX2 or k0 in FX3:
                    why += ' Fixture: re/frida/negctl_fixtures/race.py.'
                if len(keys) > 1:
                    why += f' Same RVA, second port export {keys[1]} re-earned too: {ev[1]}.'
                rows.append([rva, '+'.join(keys), 'keep', old, old, ev[0], '; '.join(cites), why])
                tally['keep'] += 1
            else:
                sys.exit(f'{rva} {keys}: fails ({"; ".join(why_all)}) and has no DEMOTE/HOLD ruling')
        for nm, data in ((f'{name}.tsv', rows), (f'{name}_held.tsv', held)):
            if not data:
                continue
            with open(BASE / 'decisions' / nm, 'w', encoding='utf-8', newline='') as f:
                w = csv.writer(f, delimiter='\t', lineterminator='\n')
                w.writerow(['rva', 'hook', 'action', 'old', 'new', 'evidence', 'negctl', 'why'])
                w.writerows(data)
        print(f'{name}: {len(rvas)} rows  keep {sum(r[2] == "keep" for r in rows)}  '
              f'demote {sum(r[2] == "demote" for r in rows)}  hold {len(held)}  '
              f'C4 {sum(hk[r[2:].zfill(8)]["confidence"] == "C4" for r in rvas)}')
    print(tally)


if __name__ == '__main__':
    main()
