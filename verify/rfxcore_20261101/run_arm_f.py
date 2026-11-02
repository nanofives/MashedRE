"""Arm F (PREREG_D4-DC-RFX-CORE.md): Frida path1 for the 15 hooks, one original boot each, private root, under
mashed_lock.py. Before every boot: Get-PnpDevice -Class AudioEndpoint -PresentOnly must list a VB-Audio endpoint,
else the arm stops BLOCKED. A piz-collision crash (U-9677: MASHED.exe+0x1509b4 / +0xe66db) is re-run once.
Prints one "[N/M] <hook> <STATUS>" line per hook. Per-hook logs go to <outdir>/<hook>.log.
Usage: py -3.12 run_arm_f.py <outdir> [--no-lock] [hook ...]
--no-lock: the caller already holds the lock for the whole batch (mashed_lock.py run -- py run_arm_f.py ... --no-lock);
per-boot acquisition lost every round to other sessions on 2026-11-01."""
import subprocess
import sys
from pathlib import Path

HERE = Path(__file__).resolve().parent
WT = HERE.parents[1]
LOCK = WT.parent / 'orch-integration' / 're' / 'orchestrator' / 'mashed_lock.py'
HOOKS = ['particle_emitter_set_position', 'particle_emitter_set_velocity', 'particle_emitter_set_colour',
         'copy_476a10', 'particle_emitter_set_scalar', 'particle_emitter_set_rgba', 'copy_476a80',
         'batch_476ae0', 'set_476b30', 'batch_476b90', 'batch_476be0', 'batch_476c10', 'batch_476c60',
         'batch_476cb0', 'multi_array_scatter_476d00']


# Registry arg_types diff_template.js has no handler for (run_diff.py refuses them): the early-window lane, where
# their C3 evidence was earned (hooks.csv notes: "early_window GREEN").
EW_HOOKS = {'copy_476a10', 'copy_476a80', 'batch_476ae0', 'set_476b30', 'batch_476b90', 'batch_476be0',
            'batch_476c10', 'batch_476c60', 'batch_476cb0', 'multi_array_scatter_476d00'}


def audio_ok() -> bool:
    r = subprocess.run(['pwsh', '-NoProfile', '-Command',
                        'Get-PnpDevice -Class AudioEndpoint -PresentOnly | ForEach-Object FriendlyName'],
                       capture_output=True, text=True)
    return 'VB-Audio' in r.stdout


def verdict(text: str) -> str:
    if 'VERDICT: GREEN' in text:
        return 'GREEN'
    if 'VERDICT: RED' in text:
        return 'RED'
    for tag in ('INCONCLUSIVE-DEGENERATE', 'GREEN:', 'RED:'):
        if tag in text:
            return tag.rstrip(':')
    if '0x1509b4' in text or '0xe66db' in text:
        return 'PIZ-COLLISION'
    return 'ERROR'


def main() -> int:
    out = Path(sys.argv[1])
    out.mkdir(parents=True, exist_ok=True)
    args = sys.argv[2:]
    nolock = '--no-lock' in args
    path2 = '--path2' in args     # arm F2: run_verify_private.py (installer), verdict = empty FAIL list
    hooks = [a for a in args if a not in ('--no-lock', '--path2')] or HOOKS
    res = {}
    for n, h in enumerate(hooks, 1):
        st = 'ERROR'
        for attempt in (1, 2):
            if not audio_ok():
                print(f'[{n}/{len(hooks)}] {h} BLOCKED (no VB-Audio endpoint)', flush=True)
                return 2
            script = ('run_verify_private.py' if path2 else
                      'run_ew_private.py' if h in EW_HOOKS else 'run_diff_private.py')
            cmd = [sys.executable, str(HERE / script), h]
            if not nolock:
                cmd = [sys.executable, str(LOCK), 'run', '--label', 'D4-DC-RFX-CORE', '--'] + cmd
            r = subprocess.run(cmd, cwd=str(WT), capture_output=True, text=True, errors='replace')
            text = r.stdout + r.stderr
            (out / f'{h}.log' if attempt == 1 else out / f'{h}.retry.log').write_text(text, encoding='utf-8')
            if path2:
                st = ('INSTALLED' if r.returncode == 0 and 'FAIL:\n  (none)' in text
                      else ('PIZ-COLLISION' if '0x1509b4' in text or '0xe66db' in text else 'NOT-INSTALLED'))
            else:
                st = verdict(text)
            if st != 'PIZ-COLLISION':
                break
        res[h] = st
        print(f'[{n}/{len(hooks)}] {h} {st}', flush=True)
    want = 'INSTALLED' if path2 else 'GREEN'
    g = sum(1 for v in res.values() if v == want)
    print(f'arm {"F2" if path2 else "F"}: {g}/{len(hooks)} {want}', flush=True)
    (out / 'summary.tsv').write_text(''.join(f'{k}\t{v}\n' for k, v in res.items()), encoding='utf-8')
    return 0 if g == len(hooks) else 1


if __name__ == '__main__':
    sys.exit(main())
