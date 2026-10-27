# Parallel C2->C3 diff runner.
#
# DEFAULT (2026-10-27): the warm pool, run_diff_warm.py — 4 warm MASHED instances
# stream the batch, and every hook warm cannot reproduce (scenario, call_log,
# dialog, cold_only, flags only run_diff.py has) is routed to the cold path below
# automatically, as is every warm non-GREEN verdict (re-run cold, cold verdict
# reported). Equivalence evidence (11-hook batch, cold vs warm): the commit
# message of the change that made warm the default.
#
# --cold: the old path. Spawns one MASHED.exe + Frida session per hook, in
# parallel, each tied to its own pool slot (scripts/frida_pool.sh). Mirrors the
# Ghidra pool pattern: slots bound the concurrency, sessions are self-contained,
# and hooks.csv writes still serialize through re-classify (not this script).
#
# Usage:
#   py -3.12 re/frida/run_diff_parallel.py <hook1> [<hook2> ...] [run_diff_warm.py flags]
#   py -3.12 re/frida/run_diff_parallel.py --cold <hook1> [<hook2> ...] [run_diff.py flags]
#
# Each cold child invocation is `run_diff.py <hook> [flags]`. Results land in
# log/diff_<hook>.csv per the existing per-hook contract. The cold wrapper:
#   - acquires a frida pool slot per child
#   - spawns the child in parallel (subprocess.Popen)
#   - collects stdout/stderr per child so the streams don't interleave
#   - prints a final GREEN/RED table
#   - releases slots on exit
import os
import subprocess
import sys
import time
from concurrent.futures import ThreadPoolExecutor, as_completed
from pathlib import Path

ROOT      = Path(__file__).resolve().parent.parent.parent
POOL_SH   = ROOT / 'scripts' / 'frida_pool.sh'
RUN_DIFF  = ROOT / 're' / 'frida' / 'run_diff.py'
LOG_DIR   = ROOT / 'log'

sys.path.insert(0, str(ROOT / 're' / 'frida'))
from hooks_registry import HOOKS  # noqa: E402


def pool(cmd, *args):
    """Call frida_pool.sh and return stdout-stripped result. Raises on nonzero."""
    out = subprocess.check_output(['bash', str(POOL_SH), cmd, *map(str, args)],
                                  cwd=str(ROOT), text=True)
    return out.strip()


LAUNCH_STAGGER_S = 5  # seconds between successive MASHED.exe spawns


def run_one(hook_name, stagger_index=0, extra=()):
    """Acquire slot, run run_diff.py, release slot. Returns (hook, slot, rc, took, log_path).
    `extra` is passed to run_diff.py after the hook name (--allow-degenerate, --negctl X, ...)."""
    if stagger_index > 0:
        time.sleep(stagger_index * LAUNCH_STAGGER_S)
    t0 = time.time()
    slot = pool('acquire')
    log_path = LOG_DIR / f'parallel_diff_{hook_name}.log'
    LOG_DIR.mkdir(parents=True, exist_ok=True)
    try:
        with log_path.open('w', encoding='utf-8') as fh:
            fh.write(f"=== {hook_name} on slot {slot} ===\n")
            fh.flush()
            # Pass FRIDA_POOL_SLOT so the child can record its PID into the slot
            # tracker (informational — not load-bearing for cleanup).
            env = {**os.environ, 'FRIDA_POOL_SLOT': slot}
            proc = subprocess.Popen(
                ['py', '-3.12', str(RUN_DIFF), hook_name, *extra],
                cwd=str(ROOT), env=env,
                stdout=fh, stderr=subprocess.STDOUT,
            )
            rc = proc.wait()
    finally:
        try: pool('release', slot)
        except Exception: pass
    return hook_name, slot, rc, time.time() - t0, log_path


def main(argv):
    if len(argv) < 2:
        sys.exit(f"usage: {argv[0]} [--cold] <hook> [<hook> ...]\n  registered: {', '.join(HOOKS.keys())}")
    if '--cold' not in argv[1:]:
        import run_diff_warm  # lazy: run_diff_warm imports this module for run_one
        return run_diff_warm.main(argv)
    args = [a for a in argv[1:] if a != '--cold']
    hooks, extra = [], []
    for i, a in enumerate(args):
        if a.startswith('--') or (i and args[i - 1] == '--negctl'):
            extra.append(a)
        else:
            hooks.append(a)
    unknown = [h for h in hooks if h not in HOOKS]
    if unknown:
        sys.exit(f"unknown hooks: {unknown}\n  registered: {', '.join(HOOKS.keys())}")

    print(f"running {len(hooks)} hook(s) in parallel: {', '.join(hooks)}")
    print(f"pool: {POOL_SH}")

    t0 = time.time()
    results = []
    with ThreadPoolExecutor(max_workers=4) as ex:  # 4 = frida pool size
        futures = {ex.submit(run_one, h, i, extra): h for i, h in enumerate(hooks)}
        for fut in as_completed(futures):
            hook = futures[fut]
            try:
                results.append(fut.result())
            except Exception as e:
                print(f"  [{hook}] EXC {e}")
                results.append((hook, '?', -1, 0.0, None))
    elapsed = time.time() - t0

    print(f"\n=== parallel diff complete in {elapsed:.1f}s ===")
    print(f"{'hook':24s}  slot  rc  took   log")
    print('-' * 80)
    all_green = True
    for hook, slot, rc, took, log_path in sorted(results):
        status = 'GREEN' if rc == 0 else f'RED rc={rc}'
        if rc != 0: all_green = False
        log_str = str(log_path.relative_to(ROOT)) if log_path else '-'
        print(f"{hook:24s}  {slot:>4s}  {rc:>2d}  {took:5.1f}s  {log_str}  [{status}]")
    return 0 if all_green else 1


if __name__ == '__main__':
    sys.exit(main(sys.argv))
