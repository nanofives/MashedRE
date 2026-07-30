#!/usr/bin/env py -3.12
# multi_state_driver.py — STAGGERED multi-instance STATE-batch parallelism.
#
# WHY THIS EXISTS
# ---------------
# The STATE batch (run_diff_scenario_batch.py) already verifies many hooks per
# ONE boot. To go wider we run several booted games at once — but the pilot on
# 2026-07-29 found that TWO near-simultaneous frida.spawn calls COLLIDE (one
# instance threw a spawn/attach error while its twin navigated normally). The
# games coexist fine once up (measured: a Stalker capture completed in one game
# while another ran); it is only the spawn INSTANT that races.
#
# So this driver STAGGERS the spawns: it launches each worker a few seconds
# after the last, past the previous spawn+attach, after which the games overlap
# for the bulk of their ~40 s runtime and we still get the parallel speedup.
# Each worker is an unmodified run_diff_scenario_batch.py invocation on its own
# chunk of hooks, so it keeps that runner's proven navigation, x87 scrub,
# both-errored handling, and — critically — its kill-ONLY-my-own-pid hygiene.
#
# The driver manages only the worker subprocesses it starts. On timeout it
# parses each worker's "spawned pid=N" line and kills THOSE specific game pids
# (never a blanket kill by name — CLAUDE.md multi-session rule).
#
# Usage:
#   py -3.12 re/frida/multi_state_driver.py <hook1> <hook2> ... \
#       [--instances K] [--stagger 12] [--scenario race] [--dwell 20]
#       [--round 130] [--timeout 400]

import os
import re
import subprocess
import sys
import time
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent.parent
sys.path.insert(0, str(ROOT / 're' / 'frida'))
from hooks_registry import HOOKS

SCRIPT = ROOT / 're' / 'frida' / 'run_diff_scenario_batch.py'
LOG_DIR = ROOT / 'log' / 'multi_state'

# Documented in-race global (handle table base) used as a fallback chunk gate
# when a chunk's hooks carry no state_gate of their own (e.g. a fully synthetic
# hook). It only has to be non-zero in a race so the worker's sentinel guard
# passes; per-hook liveness is still enforced by each hook's own state_gate.
FALLBACK_SENTINEL = 0x006c71d8


def hook_sentinels(name):
    """Base addresses that indicate the live state a hook reads (from its
    state_gate). Returns a set of ints; empty if the hook has no gate."""
    out = set()
    for ch in (HOOKS[name].get('state_gate') or []):
        if isinstance(ch, dict) and 'any_nonzero' in ch:
            out.add(ch['any_nonzero'] & 0xffffffff)
        elif isinstance(ch, dict) and 'chain' in ch and ch['chain']:
            out.add(int(ch['chain'][0]) & 0xffffffff)
        elif isinstance(ch, (list, tuple)) and ch:
            out.add(int(ch[0]) & 0xffffffff)
    return out


def _flag(name, default=None, cast=str):
    return cast(sys.argv[sys.argv.index(name) + 1]) if name in sys.argv else default


VALUE_FLAGS = ('--instances', '--stagger', '--scenario', '--dwell',
               '--round', '--timeout')


def main():
    # positional hook names = argv tokens that are neither a --flag nor the
    # value immediately following a value-taking flag.
    names, skip = [], False
    for tok in sys.argv[1:]:
        if skip:
            skip = False; continue
        if tok.startswith('--'):
            if tok in VALUE_FLAGS:
                skip = True
            continue
        names.append(tok)
    unknown = [n for n in names if n not in HOOKS]
    if not names or unknown:
        sys.exit(f'usage: multi_state_driver.py <hook> ... [--instances K] '
                 f'[--stagger 12] [--scenario race] [--dwell 20]\n'
                 f'  unknown hooks: {unknown}' if unknown else
                 f'usage: multi_state_driver.py <hook> ... [--instances K]')

    K = _flag('--instances', 2, int)
    stagger = _flag('--stagger', 12.0, float)
    scenario = _flag('--scenario', 'race')
    dwell = _flag('--dwell', 20.0, float)
    round_secs = _flag('--round', 130, int)
    timeout = _flag('--timeout', 400, float)

    # contiguous chunks, dropping empties when hooks < K
    K = min(K, len(names))
    chunks = [names[i::K] for i in range(K)]
    chunks = [c for c in chunks if c]

    LOG_DIR.mkdir(parents=True, exist_ok=True)
    print(f'multi-instance STATE driver: {len(names)} hooks -> {len(chunks)} '
          f'instances, stagger={stagger}s')

    workers = []
    t_start = time.time()
    for i, chunk in enumerate(chunks):
        sents = set().union(*(hook_sentinels(n) for n in chunk)) or {FALLBACK_SENTINEL}
        sent_arg = ','.join(f'0x{a:08x}' for a in sorted(sents))
        shotdir = f'verify/multi_state/inst_{i}'
        logf = LOG_DIR / f'inst_{i}.log'
        cmd = ['py', '-3.12', str(SCRIPT), *chunk,
               '--scenario', scenario, '--dwell', str(dwell),
               '--round', str(round_secs), '--sentinel', sent_arg,
               '--shot-dir', shotdir]
        fh = open(logf, 'w', encoding='utf-8')
        p = subprocess.Popen(cmd, stdout=fh, stderr=subprocess.STDOUT, cwd=str(ROOT))
        workers.append({'i': i, 'chunk': chunk, 'proc': p, 'log': logf, 'fh': fh,
                        'sent': sent_arg, 'game_pid': None})
        print(f'  [inst {i}] launched pid={p.pid}  hooks={chunk}  sentinel={sent_arg}')
        if i < len(chunks) - 1:
            time.sleep(stagger)   # PAST this spawn+attach before the next

    # wait for all workers
    deadline = t_start + timeout + stagger * len(chunks)
    while any(w['proc'].poll() is None for w in workers):
        if time.time() > deadline:
            print('  TIMEOUT — killing outstanding workers and their games (by pid)')
            for w in workers:
                if w['proc'].poll() is None:
                    gp = _parse_game_pid(w['log'])
                    if gp:
                        subprocess.run(['taskkill', '/PID', str(gp), '/F'],
                                       capture_output=True)
                        print(f'    [inst {w["i"]}] killed game pid={gp}')
                    w['proc'].terminate()
            break
        time.sleep(1.0)
    wall = time.time() - t_start
    for w in workers:
        try: w['fh'].close()
        except Exception: pass

    # aggregate
    print('\n=== MULTI-INSTANCE VERDICT ===')
    tot_green = tot_ran = 0
    for w in workers:
        txt = w['log'].read_text(encoding='utf-8', errors='replace')
        m = re.search(r'GREEN:\s*(\d+)/(\d+)', txt)
        aborted = 'ABORT' in txt or 'process exited during' in txt
        spawn_err = 'raise result.error' in txt or 'frida.' in txt and 'Error' in txt
        if m:
            g, r = int(m.group(1)), int(m.group(2))
            tot_green += g; tot_ran += r
            status = f'GREEN {g}/{r}'
        elif aborted:
            status = 'ABORTED (nav void)'
        elif spawn_err:
            status = 'SPAWN-ERROR'
        else:
            status = 'NO-VERDICT'
        gp = _parse_game_pid(w['log'])
        print(f'  [inst {w["i"]}] {status}  game_pid={gp}  hooks={w["chunk"]}')

    est_serial = len(chunks) * 45.0   # ~45 s per single-instance STATE boot
    print(f'\n  aggregate GREEN: {tot_green}/{tot_ran} across {len(chunks)} instances')
    print(f'  wall-clock: {wall:.0f}s   serial estimate: ~{est_serial:.0f}s   '
          f'speedup: {est_serial / wall:.2f}x' if wall > 0 else '')
    print(f'  per-instance logs: {LOG_DIR}')
    return 0


def _parse_game_pid(logf):
    try:
        m = re.search(r'spawned pid=(\d+)', Path(logf).read_text(errors='replace'))
        return int(m.group(1)) if m else None
    except Exception:
        return None


if __name__ == '__main__':
    sys.exit(main())
