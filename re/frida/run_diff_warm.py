# Warm-pool C2->C3 diff runner.
#
# The expensive part of a diff is NOT the force-call (that's ~1-2s) — it's
# spawning + initializing a MASHED.exe instance. run_diff_parallel.py spawns a
# FRESH instance per hook (K spawns for K hooks). This runner instead spawns a
# small pool of WARM instances ONCE, then streams every hook's force-call diff
# through the already-running processes — so boot cost is paid N times (pool
# size), not K times (hook count). On a 60-hook batch with a 4-instance pool
# that's 4 spawns instead of 60.
#
# Usage:
#   py -3.12 re/frida/run_diff_warm.py <hook1> [<hook2> ...] [--slots N]
#   py -3.12 re/frida/run_diff_warm.py --file hooks.txt [--slots N]
#
# Per-hook CSVs land in log/diff_<hook>.csv (identical contract to run_diff.py),
# so frida-sweep / re-classify consume them unchanged.
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
import csv
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
import run_diff  # reuse build_config / value_bits / paths (single source of truth)
from hooks_registry import HOOKS

MASHED_EXE = run_diff.MASHED_EXE
ASI_PATH   = run_diff.ASI_PATH
AGENT_JS   = run_diff.AGENT_JS
LOG_DIR    = run_diff.LOG_DIR

SPAWN_STAGGER_S = 3.0   # gap between successive MASHED spawns (window + D3D init)
HOOK_TIMEOUT_S  = 60.0  # per-hook results deadline


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


def spawn_instance(idx):
    """Spawn one warm MASHED.exe from original/ and attach a Frida session.
    Returns (proc, session) or (proc, None) on attach failure."""
    env = {**os.environ, 'MASHED_RE_NO_AUTO_HOOK': '1'}
    proc = subprocess.Popen([str(MASHED_EXE)], cwd=str(MASHED_EXE.parent), env=env)
    print(f"  [slot {idx}] spawned pid={proc.pid}")
    time.sleep(0.4)
    try:
        session = frida.get_local_device().attach(proc.pid)
        return proc, session
    except Exception as e:
        print(f"  [slot {idx}] attach failed: {e}")
        return proc, None


def run_hook_on_session(session, name):
    """Force-call A/B one hook against an already-attached warm session.
    Returns (results_list, error_or_None). Reuses diff_template.js verbatim;
    the agent's repeated Module.load of the .asi is OS-idempotent (refcount)."""
    hook = HOOKS[name]
    config = run_diff.build_config(hook)
    script_text = AGENT_JS.read_text(encoding='utf-8').replace(
        '$CONFIG$', __import__('json').dumps(config))

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


def write_csv(name, results):
    """Write log/diff_<name>.csv in the exact run_diff.py format. Returns
    (total, mismatches)."""
    import json
    ret_kind = HOOKS[name]['signature']['ret']
    csv_out = LOG_DIR / f'diff_{name}.csv'
    mism = 0
    with csv_out.open('w', newline='', encoding='utf-8') as fh:
        w = csv.writer(fh)
        w.writerow(['idx', 'input', 'original', 'original_bits',
                    'reimpl', 'reimpl_bits', 'match', 'err_original', 'err_reimpl'])
        for r in results:
            ob = run_diff.value_bits(r['original'], ret_kind)
            rb = run_diff.value_bits(r['reimpl'], ret_kind)
            ob_s = f"0x{ob:08x}" if ob is not None else ''
            rb_s = f"0x{rb:08x}" if rb is not None else ''
            inp = r['input']
            inp_repr = json.dumps(inp) if isinstance(inp, (list, dict)) else inp
            w.writerow([r['idx'], inp_repr, r['original'], ob_s, r['reimpl'], rb_s,
                        r['match'], r.get('err_original') or '', r.get('err_reimpl') or ''])
            if not r['match']:
                mism += 1
    return len(results), mism


def slot_worker(idx, session, hook_names, out_rows, lock):
    """Drive one warm session through its assigned slice of hooks."""
    for name in hook_names:
        if session is None:
            with lock:
                out_rows.append((name, idx, 'NO-SESSION', 0, 0))
            continue
        try:
            results, err = run_hook_on_session(session, name)
        except Exception as e:
            results, err = [], f'EXC {e}'
        if not results:
            with lock:
                out_rows.append((name, idx, f'RED ({err})', 0, 0))
            continue
        total, mism = write_csv(name, results)
        status = 'GREEN' if mism == 0 else f'RED {mism}/{total} mismatch'
        with lock:
            out_rows.append((name, idx, status, total, mism))
        print(f"  [slot {idx}] {name}: {status}")


def main(argv):
    ap = argparse.ArgumentParser(description="warm-pool C2->C3 diff runner")
    ap.add_argument('hooks', nargs='*', help="hook names (registry keys)")
    ap.add_argument('--file', help="file with one hook name per line")
    ap.add_argument('--slots', type=int, default=4, help="warm instances (default 4)")
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

    n_slots = max(1, min(args.slots, len(names)))
    LOG_DIR.mkdir(parents=True, exist_ok=True)
    preflight()

    # Shard hooks round-robin across slots (balances mixed per-hook costs).
    shards = [[] for _ in range(n_slots)]
    for i, name in enumerate(names):
        shards[i % n_slots].append(name)

    print(f"warm pool: {n_slots} instance(s), {len(names)} hook(s)")
    t0 = time.time()

    # Spawn + attach the pool (staggered).
    instances = []
    for idx in range(n_slots):
        if idx > 0:
            time.sleep(SPAWN_STAGGER_S)
        instances.append(spawn_instance(idx))

    out_rows = []
    lock = threading.Lock()
    try:
        with ThreadPoolExecutor(max_workers=n_slots) as ex:
            futs = [ex.submit(slot_worker, idx, instances[idx][1], shards[idx], out_rows, lock)
                    for idx in range(n_slots)]
            for f in futs:
                f.result()
    finally:
        for idx, (proc, session) in enumerate(instances):
            try:
                if session is not None: session.detach()
            except Exception: pass
            try: proc.kill()
            except Exception: pass
            try: proc.wait(timeout=3)
            except Exception: pass

    elapsed = time.time() - t0
    print(f"\n=== warm diff complete in {elapsed:.1f}s "
          f"({n_slots} spawns for {len(names)} hooks) ===")
    print(f"{'hook':28s} slot  status")
    print('-' * 70)
    all_green = True
    for name, idx, status, total, mism in sorted(out_rows):
        if not status.startswith('GREEN'):
            all_green = False
        print(f"{name:28s} {idx:>4d}  {status}")
    return 0 if all_green else 1


if __name__ == '__main__':
    sys.exit(main(sys.argv))
