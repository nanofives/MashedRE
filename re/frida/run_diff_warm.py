# Warm-pool C2->C3 diff runner (the default batch path: run_diff_parallel.py calls this).
#
# The expensive part of a diff is NOT the force-call (that's ~1-2s) — it's
# spawning + initializing a MASHED.exe instance. The cold path spawns a FRESH
# instance per hook (K spawns for K hooks). This runner instead spawns a small
# pool of WARM instances ONCE, then streams every hook's force-call diff through
# the already-running processes — so boot cost is paid N times (pool size), not
# K times (hook count).
#
# Usage:
#   py -3.12 re/frida/run_diff_warm.py <hook1> [<hook2> ...] [--slots N]
#   py -3.12 re/frida/run_diff_warm.py --file hooks.txt [--slots N] [--summary out.json]
#   flags: --allow-degenerate --allow-unknown-argtype (honoured warm), --self-baseline
#          --negctl <class> --x87-cw=0xNNNN (whole batch goes cold), --cold (everything cold),
#          --no-confirm (keep a warm non-GREEN verdict instead of re-running it cold)
#
# Per-hook CSVs land in log/diff_<hook>.csv (identical contract to run_diff.py),
# so frida-sweep / re-classify consume them unchanged.
#
# EQUIVALENCE WITH run_diff.py (2026-10-27). The verdict logic is run_diff.py's own
# (argtype_preflight, write_results_csv, degenerate_check are imported, not copied),
# and the return codes are the same (0 GREEN, 1 RED / FATAL arg_type, 2 no results,
# 3 timeout, 5 INCONCLUSIVE-DEGENERATE). What warm cannot reproduce is routed to the
# cold run_diff.py path automatically, per hook (cold_reason below):
#   - scenario hooks (drive_to_race + race_sentinel + the gamesave pin run per spawn),
#   - call_log hooks (run_diff.py writes their drawstream_*.csv),
#   - dialog hooks (they build a real window tree in the process),
#   - registry `cold_only: '<why>'` and COLD_ONLY_ARG_TYPES: fixtures that change
#     process-wide state a later hook on the same instance would see,
#   - every hook, when a flag only run_diff.py implements is passed.
# boot_settle_s is honoured per instance: a hook waits until its instance has been
# attached that long (so the batch pays the largest settle once per instance).
# A warm instance that times out, errors or dies is killed and respawned before its
# next hook. Every warm non-GREEN verdict is re-run cold and the cold verdict is the
# one reported, so state left behind by an earlier hook can never produce a RED.
#
# WINDOW ERGONOMICS: each instance gets a titled, draggable, minimizable border
# (d3d9 shim, on by default) so the pool is easy to arrange on screen. With the
# intro-skip patch applied the old minimize-freeze does not apply.
#
# ISOLATION MODEL (v1 — shared original/): all instances spawn from the
# layer-registered original/ directory. This is deliberate: the AppCompat shim
# (RUNASINVOKER etc., set by scripts/setup_mashed_compat.ps1) is keyed to the
# exe's FULL PATH in HKCU\...\AppCompatFlags\Layers, so running copies from
# fresh paths would NOT inherit it (-> elevation prompt / spawn hang). The
# canonical windowed videocfg.bin is written ONCE before any spawn (no per-spawn
# race), and force-calling leaf functions does not write gamesave.bin, so shared
# original/ is race-free for this workload. A fully-isolated per-slot-dir variant
# (junction TOASTART/toastaudio read-only, copy the small files, AND register a
# per-path AppCompat layer) is possible but adds registry mutation — deferred to
# v2 until write contention actually bites.
import argparse
import json
import os
import shutil
import subprocess
import sys
import threading
import time
from concurrent.futures import ThreadPoolExecutor
from pathlib import Path

import frida

ROOT = Path(__file__).resolve().parent.parent.parent
sys.path.insert(0, str(ROOT / 're' / 'frida'))
import run_diff  # reuse build_config / verdict helpers / paths (single source of truth)
import run_diff_parallel  # the cold per-hook runner (run_one)
from hooks_registry import HOOKS

MASHED_EXE = run_diff.MASHED_EXE
ASI_PATH   = run_diff.ASI_PATH
AGENT_JS   = run_diff.AGENT_JS
LOG_DIR    = run_diff.LOG_DIR

SPAWN_STAGGER_S = 3.0   # gap between successive MASHED spawns (window + D3D init)
HOOK_TIMEOUT_S  = 60.0  # per-hook results deadline (run_diff.py uses the same 60 s)

# Handlers whose fixture changes process-wide state that is not restored after the
# vectors run: teardown_call_pair NULLs an engine-state global, the other three create
# OS / DirectSound objects that outlive the script. A cold spawn isolates them.
COLD_ONLY_ARG_TYPES = {'teardown_call_pair', 'dsound_secondary_init', 'semaphore_create',
                       'thread_desc_init'}

RC_LABEL = {0: 'GREEN', 1: 'RED', 2: 'NO-RESULTS', 3: 'TIMEOUT', 4: 'ATTACH-FAIL',
            5: 'INCONCLUSIVE-DEGENERATE', 6: 'RACE-UNREACHED'}


def cold_reason(hook, cold_flags):
    """Why this hook must take the cold run_diff.py path, or None if warm reproduces it."""
    if cold_flags:
        return 'flag ' + ' '.join(cold_flags)
    if hook.get('scenario'):
        return f"scenario={hook['scenario']}"
    if hook.get('call_log'):
        return 'call_log (drawstream CSV)'
    if hook.get('dialog'):
        return 'dialog'
    if hook.get('cold_only'):
        return f"cold_only: {hook['cold_only']}"
    if hook.get('arg_type') in COLD_ONLY_ARG_TYPES:
        return f"stateful arg_type {hook['arg_type']}"
    return None


def preflight():
    if not MASHED_EXE.exists():
        sys.exit(f"MASHED.exe not found at {MASHED_EXE}")
    if not ASI_PATH.exists():
        sys.exit(f"build artifact not found at {ASI_PATH} — run mashedmod\\build.bat first")
    shim = MASHED_EXE.parent / 'd3d9.dll'
    if not shim.exists():
        sys.exit(f"FATAL: {shim} missing (d3d9 windowed shim). "
                 f"Run mashedmod\\build_d3d9_shim.bat, then retry.")
    # Enforce windowed videocfg ONCE for all instances (no per-spawn race).
    cfg = ROOT / 'scripts' / 'canonical' / 'videocfg_windowed.bin'
    if cfg.exists():
        shutil.copy2(str(cfg), str(MASHED_EXE.parent / 'videocfg.bin'))


class Instance:
    """One warm MASHED.exe + its Frida session. Killed only by its own PID."""

    def __init__(self, idx):
        self.idx = idx
        self.proc = None
        self.session = None
        self.detached = False
        self.t_attach = 0.0
        self.spawns = 0

    def start(self):
        env = {**os.environ, 'MASHED_RE_NO_AUTO_HOOK': '1'}
        # CRITICAL for a multi-instance pool: detach each instance from the parent's
        # console and give it its own process group. A plain Popen leaves every
        # instance in the terminal's console group, so a Ctrl-C / Ctrl-Break (or the
        # parent exiting) broadcasts a console signal to ALL of them — they'd close
        # simultaneously. Detached, each is independent and is killed only by its own
        # proc.kill() (TerminateProcess by this exact PID).
        _CREATE_NEW_PROCESS_GROUP = 0x00000200
        _DETACHED_PROCESS         = 0x00000008
        self.proc = subprocess.Popen(
            [str(MASHED_EXE)], cwd=str(MASHED_EXE.parent), env=env,
            stdin=subprocess.DEVNULL, stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL,
            creationflags=_CREATE_NEW_PROCESS_GROUP | _DETACHED_PROCESS)
        self.spawns += 1
        self.detached = False
        print(f"  [slot {self.idx}] spawned pid={self.proc.pid}")
        time.sleep(0.4)
        try:
            self.session = frida.get_local_device().attach(self.proc.pid)
            self.session.on('detached', lambda *a: setattr(self, 'detached', True))
        except Exception as e:
            print(f"  [slot {self.idx}] attach failed: {e}")
            self.session = None
        self.t_attach = time.time()

    def healthy(self):
        return (self.session is not None and not self.detached
                and self.proc is not None and self.proc.poll() is None)

    def stop(self):
        try:
            if self.session is not None: self.session.detach()
        except Exception: pass
        if self.proc is not None:
            try: self.proc.kill()
            except Exception: pass
            try: self.proc.wait(timeout=3)
            except Exception: pass
        self.session = None


def run_hook_on_session(session, name):
    """Force-call A/B one hook against an already-attached warm session.
    Returns (results_list, error_or_None). Reuses diff_template.js verbatim;
    the agent's repeated Module.load of the .asi is OS-idempotent (refcount)."""
    hook = HOOKS[name]
    config = run_diff.build_config(hook)
    script_text = run_diff.agent_source(AGENT_JS, config)

    results = []
    err = {'msg': None}
    done = threading.Event()

    def on_message(message, data):
        if message.get('type') == 'error':
            err['msg'] = message.get('description')
            done.set()
            return
        payload = message.get('payload', {})
        kind = payload.get('type')
        if kind == 'results':
            results.extend(payload['data'])
            done.set()
        elif kind == 'error':
            err['msg'] = payload.get('msg')
            done.set()
        # other payload kinds (lut_ready / asi_loaded) are informational; ignore

    script = session.create_script(script_text)
    script.on('message', on_message)
    script.load()
    done.wait(timeout=HOOK_TIMEOUT_S)
    try: script.unload()
    except Exception: pass

    if not results and err['msg'] is None:
        err['msg'] = 'TIMEOUT'
    return results, err['msg']


def warm_verdict(name, results, err, allow_degenerate):
    """run_diff.py's verdict and return code for one warm run (writes the CSV)."""
    hook = HOOKS[name]
    if not results:
        return (3, 'TIMEOUT') if err == 'TIMEOUT' else (2, f'NO-RESULTS ({err})')
    mism = run_diff.write_results_csv(LOG_DIR / f'diff_{name}.csv', hook, results)
    if mism:
        return 1, f'RED {mism}/{len(results)} mismatch'
    all_trivial, why = run_diff.degenerate_check(hook, results, allow_degenerate)
    if all_trivial and why is None:
        return 5, 'INCONCLUSIVE-DEGENERATE'
    return 0, 'GREEN' + (f' (all trivial; {why})' if all_trivial else '')


def slot_worker(inst, hook_names, out_rows, lock, allow_degenerate):
    """Drive one warm instance through its assigned slice of hooks."""
    for name in hook_names:
        if not inst.healthy():
            if inst.spawns:
                print(f"  [slot {inst.idx}] instance unhealthy, respawning")
            inst.stop()
            inst.start()
        t0 = time.time()
        settle = float(HOOKS[name].get('boot_settle_s') or 0) - (time.time() - inst.t_attach)
        if settle > 0:
            print(f"  [slot {inst.idx}] {name}: boot_settle_s, waiting {settle:.1f}s")
            time.sleep(settle)
        if inst.session is None:
            rc, status = 4, 'ATTACH-FAIL'
        else:
            try:
                results, err = run_hook_on_session(inst.session, name)
            except Exception as e:
                results, err = [], f'EXC {e}'
            rc, status = warm_verdict(name, results, err, allow_degenerate)
            if not results:
                inst.stop()   # state unknown after a timeout / agent failure: next hook gets a fresh instance
        with lock:
            out_rows.append({'hook': name, 'mode': f'warm/{inst.idx}', 'rc': rc, 'status': status,
                             'took': round(time.time() - t0, 1), 'note': ''})
        print(f"  [slot {inst.idx}] {name}: {status}")


def run_cold(names, slots, extra, reasons):
    """run_diff.py per hook (fresh spawn each), at most `slots` at once."""
    rows = []
    if not names:
        return rows
    print(f"cold path: {len(names)} hook(s)")
    with ThreadPoolExecutor(max_workers=slots) as ex:
        futs = [ex.submit(run_diff_parallel.run_one, h, i if i < slots else 0, extra)
                for i, h in enumerate(names)]
        for f in futs:
            hook, slot, rc, took, log_path = f.result()
            rows.append({'hook': hook, 'mode': 'cold', 'rc': rc, 'status': RC_LABEL.get(rc, f'rc={rc}'),
                         'took': round(took, 1), 'note': reasons.get(hook, '')})
            print(f"  [cold] {hook}: {rows[-1]['status']}")
    return rows


def run_batch(names, slots=4, cold_flags=(), extra=(), allow_degenerate=False,
              allow_unknown_argtype=False, all_cold=False, confirm=True):
    """Run a batch; returns one row dict per hook (hook, mode, rc, status, took, note)."""
    LOG_DIR.mkdir(parents=True, exist_ok=True)
    preflight()
    rows, warm, reasons = [], [], {}
    for name in names:
        why = 'all cold (--cold)' if all_cold else cold_reason(HOOKS[name], cold_flags)
        if why:
            reasons[name] = why
            continue
        fatal = run_diff.argtype_preflight(HOOKS[name], name)
        if fatal and not allow_unknown_argtype:
            print(f"  {fatal}")
            rows.append({'hook': name, 'mode': 'none', 'rc': 1, 'status': 'FATAL-ARGTYPE',
                         'took': 0.0, 'note': 'no handler in diff_template.js'})
            continue
        warm.append(name)

    n_slots = max(1, min(slots, len(warm))) if warm else 0
    print(f"warm pool: {n_slots} instance(s) for {len(warm)} hook(s); "
          f"{len(reasons)} hook(s) routed cold")
    for name, why in reasons.items():
        print(f"  cold: {name} ({why})")

    warm_rows = []
    instances = [Instance(i) for i in range(n_slots)]
    if warm:
        # Shard hooks round-robin across slots (balances mixed per-hook costs).
        shards = [warm[i::n_slots] for i in range(n_slots)]
        lock = threading.Lock()
        try:
            for i, inst in enumerate(instances):
                if i > 0:
                    time.sleep(SPAWN_STAGGER_S)
                inst.start()
            with ThreadPoolExecutor(max_workers=n_slots) as ex:
                futs = [ex.submit(slot_worker, instances[i], shards[i], warm_rows, lock, allow_degenerate)
                        for i in range(n_slots)]
                for f in futs:
                    f.result()
        finally:
            for inst in instances:
                inst.stop()

    # Warm non-GREEN verdicts are confirmed cold; the cold verdict is the one reported.
    confirm_names = [r['hook'] for r in warm_rows if r['rc'] != 0] if confirm else []
    for r in warm_rows:
        if r['hook'] in confirm_names:
            reasons[r['hook']] = f"confirming warm {r['status']}"
    cold_rows = run_cold([n for n in names if n in reasons], max(1, slots), list(extra), reasons)
    cold_by = {r['hook']: r for r in cold_rows}
    for r in warm_rows:
        c = cold_by.get(r['hook'])
        if c is not None:
            c['mode'] = 'cold(confirm)'
            c['note'] = f"warm said {r['status']}"
            c['warm_rc'] = r['rc']
        else:
            rows.append(r)
    rows.extend(cold_rows)
    spawns = sum(inst.spawns for inst in instances) + len(cold_rows)
    return rows, spawns


def main(argv):
    ap = argparse.ArgumentParser(description="warm-pool C2->C3 diff runner")
    ap.add_argument('hooks', nargs='*', help="hook names (registry keys)")
    ap.add_argument('--file', help="file with one hook name per line")
    ap.add_argument('--slots', type=int, default=4, help="warm instances / cold concurrency (default 4)")
    ap.add_argument('--summary', help="write the result rows as JSON here")
    ap.add_argument('--cold', action='store_true', help="run every hook through run_diff.py")
    ap.add_argument('--no-confirm', action='store_true', help="do not re-run warm non-GREEN verdicts cold")
    ap.add_argument('--allow-degenerate', action='store_true')
    ap.add_argument('--allow-unknown-argtype', action='store_true')
    ap.add_argument('--self-baseline', action='store_true')
    ap.add_argument('--negctl')
    ap.add_argument('--x87-cw')
    args = ap.parse_args(argv[1:])

    names = list(args.hooks)
    if args.file:
        names += [ln.strip() for ln in Path(args.file).read_text().splitlines()
                  if ln.strip() and not ln.strip().startswith('#')]
    if not names:
        sys.exit(f"usage: {argv[0]} <hook> [<hook> ...] [--slots N]\n"
                 f"  registered: {', '.join(sorted(HOOKS.keys()))}")
    unknown = [h for h in names if h not in HOOKS]
    if unknown:
        sys.exit(f"unknown hooks: {unknown}")

    # Flags only run_diff.py implements send the whole batch cold; all flags pass through to it.
    cold_flags = []
    if args.self_baseline: cold_flags.append('--self-baseline')
    if args.negctl: cold_flags += ['--negctl', args.negctl]
    if args.x87_cw: cold_flags.append(f'--x87-cw={args.x87_cw}')
    extra = list(cold_flags)
    if args.allow_degenerate: extra.append('--allow-degenerate')
    if args.allow_unknown_argtype: extra.append('--allow-unknown-argtype')

    t0 = time.time()
    rows, spawns = run_batch(names, args.slots, cold_flags, extra, args.allow_degenerate,
                             args.allow_unknown_argtype, args.cold, not args.no_confirm)
    elapsed = time.time() - t0

    print(f"\n=== diff batch complete in {elapsed:.1f}s ({spawns} spawns for {len(names)} hooks) ===")
    print(f"{'hook':32s} {'mode':14s} rc  took   status")
    print('-' * 90)
    for r in sorted(rows, key=lambda r: r['hook']):
        note = f"  [{r['note']}]" if r['note'] else ''
        print(f"{r['hook']:32s} {r['mode']:14s} {r['rc']:>2d} {r['took']:5.1f}s  {r['status']}{note}")
    if args.summary:
        Path(args.summary).write_text(json.dumps({'elapsed_s': round(elapsed, 1), 'spawns': spawns,
                                                  'rows': rows}, indent=1), encoding='utf-8')
    return 0 if all(r['rc'] == 0 for r in rows) else 1


if __name__ == '__main__':
    sys.exit(main(sys.argv))
