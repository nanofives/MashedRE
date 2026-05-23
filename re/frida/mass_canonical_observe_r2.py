# mass_canonical_observe_r2.py
#
# Mass canonical-scenario observation — ROUND 2 — 20 boot-once C2 candidates.
# C2 -> C3 evidence via boot-to-menu canonical observation.
#
# Scenario (identical to round-1 mass_canonical_observe.py):
#   1. frida.spawn MASHED.exe (suspended) — process is frozen before any user
#      code runs, including CRT startup. This is critical for early-CRT candidates
#      (__mtinitlocks, ___heap_select, ___sbh_heap_init) which fire before a plain
#      attach() could land.
#   2. Interceptor.attach ALL 20 candidates BEFORE resume.
#   3. device.resume(pid) — process boots to main menu.
#   4. Idle 30 seconds on main menu, polling counts.
#   5. Kill MASHED, report per-candidate result table.
#
# All 20 candidates are boot-once (not per-frame hot paths) — Interceptor.attach
# is safe per CLAUDE.md "Frida overhead on hot paths" rules.
#
# Notes per candidate group:
#   Boot-once asset loaders (sub_00499ba0, FUN_00404830, FUN_004cc160, thunk_*,
#   FUN_004b6610, FUN_00420d00, FUN_00412890, thunk_FUN_00494460, FUN_00425bc0):
#     — fire once each at boot; HIGH yield expected.
#   WorldRenderPrePass + sibling (FUN_00426030, FUN_004260e0):
#     — may NOT fire at main menu (state=1, track render is state=3/7); INCONCLUSIVE
#       possible.
#   boot_lowrva candidates (FUN_004026d0, FUN_00402f50, FUN_004114c0):
#     — unknown firing pattern; medium yield expected.
#   Boot CRT-helpers (FUN_00484170, FUN_00499730):
#     — likely fire during boot init.
#   CRT __mtinitlocks + heap helpers (__mtinitlocks, ___heap_select, ___sbh_heap_init):
#     — same issue as round-1 MainLoopInit: fire pre-OS-resume; INCONCLUSIVE possible
#       if they fire before the Interceptor can land. Spawn-suspended pattern mitigates
#       this — they should fire after resume().
#
# Realistic prediction: 12-16 promotions of 20 attempted.
#
# Output: log/mass_canonical_observe_r2.txt
#
# Requires:
#   - All 5 disk patches applied to original/MASHED.exe.
#   - original/videocfg.bin pinned to windowed mode.
#   - d3d9 shim (original/d3d9.dll) deployed.
#   - pycaw + comtypes for per-app mute (optional).
#   - mashedmod/build/mashed_re_dev.asi built.
#
# Binary anchor (unpatched):
#   MASHED.exe SHA-256: BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E

import sys
import time
import json
from pathlib import Path

import frida
import psutil

try:
    from pycaw.pycaw import AudioUtilities
    HAS_PYCAW = True
except ImportError:
    HAS_PYCAW = False

# ---------------------------------------------------------------------------
# Paths
# ---------------------------------------------------------------------------

ROOT = Path(__file__).resolve().parent.parent.parent


def _find_original(script_root: Path) -> Path:
    candidate = script_root / 'original' / 'MASHED.exe'
    if candidate.exists():
        return candidate
    parent = script_root.parent.parent
    candidate2 = parent / 'original' / 'MASHED.exe'
    if candidate2.exists():
        return candidate2
    return candidate


MASHED_EXE = _find_original(ROOT)

# ASI path: prefer worktree-local build; fall back to main repo build.
def _find_asi(script_root: Path) -> Path:
    local = script_root / 'mashedmod' / 'build' / 'mashed_re_dev.asi'
    if local.exists():
        return local
    parent = script_root.parent.parent
    fallback = parent / 'mashedmod' / 'build' / 'mashed_re_dev.asi'
    if fallback.exists():
        return fallback
    return local

ASI_PATH = _find_asi(ROOT)

# OUT_FILE: prefer main-repo log/ directory (shared; same pattern as MASHED_EXE fallback).
def _find_log_dir(script_root: Path) -> Path:
    local_log = script_root / 'log'
    if local_log.exists():
        return local_log
    parent_log = script_root.parent.parent / 'log'
    return parent_log

OUT_FILE = _find_log_dir(ROOT) / 'mass_canonical_observe_r2.txt'

# ---------------------------------------------------------------------------
# 20 boot-once C2 candidates (RVA = VA for MASHED.exe ImageBase 0x00400000).
# ---------------------------------------------------------------------------

CANDIDATES = [
    # (va_hex, short_name, expected_context)
    # --- Boot-once asset loaders (HIGH yield) ---
    ('0x00499ba0', 'sub_00499ba0',        'WindowCreate: CoInitialize+RegisterClassA+CreateWindowExA'),
    ('0x00404830', 'FUN_00404830',        'PIZ permanent asset loader (first-load)'),
    ('0x004cc160', 'FUN_004cc160',        'RW engine init helper (stream-context close)'),
    ('0x004b6540', 'thunk_FUN_004b6640',  'TXD loader thunk'),
    ('0x004b6560', 'thunk_FUN_004b6610',  'TXD loader thunk'),
    ('0x004b6610', 'FUN_004b6610',        'TXD loader body (19B setter)'),
    ('0x00420d00', 'FUN_00420d00',        'Panel PIZ loader'),
    ('0x00412890', 'FUN_00412890',        'Panel PIZ loader (index buffer fill)'),
    ('0x00494f20', 'thunk_FUN_00494460',  'Boot thunk -> intro_splash path'),
    ('0x00425bc0', 'FUN_00425bc0',        'Perm PIZ loader sibling (Copters.txd)'),
    # --- WorldRenderPrePass + sibling (may not fire at menu; INCONCLUSIVE possible) ---
    ('0x00426030', 'FUN_00426030',        'WorldRenderPrePass — may not fire at menu'),
    ('0x004260e0', 'FUN_004260e0',        'Track loader path builder — may not fire at menu'),
    # --- boot_lowrva candidates (medium yield) ---
    ('0x004026d0', 'FUN_004026d0',        'boot_lowrva: loop+FUN_004671a0 callee'),
    ('0x00402f50', 'FUN_00402f50',        'boot_lowrva: zero 3 globals + write floats'),
    ('0x004114c0', 'FUN_004114c0',        'boot_lowrva: indirect call teardown pair'),
    # --- Boot CRT-helpers (likely fire) ---
    ('0x00484170', 'FUN_00484170',        'Boot CRT-helper: vtable[0x108] alloc 24B rec'),
    ('0x00499730', 'FUN_00499730',        'Boot getter: returns &DAT_00773818'),
    # --- CRT __mtinitlocks + heap helpers (pre-resume; INCONCLUSIVE possible) ---
    ('0x004a774d', '__mtinitlocks',       'CRT __mtinitlocks — 36-entry lock table init'),
    ('0x004aa3e4', '___heap_select',      'CRT ___heap_select — reads mode/size globals'),
    ('0x004aa44f', '___sbh_heap_init',    'CRT ___sbh_heap_init — HeapAlloc + SBH globals'),
]

# ---------------------------------------------------------------------------
# Frida agent JS — attaches all candidates before resume, exposes snapshot().
# ---------------------------------------------------------------------------

AGENT_JS = r'''
'use strict';

var CANDIDATES = $CANDIDATES$;

var counters = {};
var attachErrors = {};

CANDIDATES.forEach(function (c) {
    var va   = c[0];
    var name = c[1];
    counters[name] = 0;
    attachErrors[name] = null;
    try {
        Interceptor.attach(ptr(va), {
            onEnter: function () { counters[name]++; }
        });
    } catch (e) {
        attachErrors[name] = e.message;
    }
});

rpc.exports = {
    snapshot: function () {
        return { counters: counters, attach_errors: attachErrors };
    }
};

send({ kind: 'ready' });
'''

# ---------------------------------------------------------------------------
# Helpers
# ---------------------------------------------------------------------------


def mute_pid_audio(pid: int) -> bool:
    """Mute the audio session for a specific PID. Returns True if found+muted."""
    if not HAS_PYCAW:
        return False
    try:
        sessions = AudioUtilities.GetAllSessions()
        for sess in sessions:
            if sess.Process and sess.Process.pid == pid:
                sess.SimpleAudioVolume.SetMute(1, None)
                return True
    except Exception as e:
        print(f'  mute failed: {e}')
    return False


# ---------------------------------------------------------------------------
# Main
# ---------------------------------------------------------------------------


def main() -> int:
    print('=== mass_canonical_observe_r2.py  (ROUND 2) ===')
    print(f'MASHED.exe : {MASHED_EXE}')
    print(f'ASI        : {ASI_PATH}')
    print(f'Candidates : {len(CANDIDATES)}')
    print()

    # Pre-flight checks.
    if not MASHED_EXE.exists():
        print(f'FATAL: {MASHED_EXE} not found.')
        return 1

    shim = MASHED_EXE.parent / 'd3d9.dll'
    if not shim.exists():
        print(f'FATAL: d3d9 shim missing at {shim}.')
        print('       Run mashedmod\\build_d3d9_shim.bat to rebuild + deploy.')
        print('       Refusing to spawn MASHED without windowed-mode shim.')
        return 2

    if not ASI_PATH.exists():
        print(f'FATAL: {ASI_PATH} not found. Run mashedmod\\build.bat first.')
        return 3

    OUT_FILE.parent.mkdir(parents=True, exist_ok=True)

    # Clear mashed.log for fresh run evidence.
    log_path = MASHED_EXE.parent / 'mashed.log'
    if log_path.exists():
        try:
            log_path.unlink()
        except PermissionError:
            pass

    IDLE_SECONDS = 30

    print(f'frida.spawn {MASHED_EXE} (suspended) ...')

    device = frida.get_local_device()
    try:
        pid = device.spawn(
            [str(MASHED_EXE)],
            cwd=str(MASHED_EXE.parent),
        )
    except Exception as e:
        print(f'FATAL: frida.spawn failed: {e}')
        return 4

    print(f'  PID = {pid} (suspended)')

    # Attach session while process is still suspended.
    try:
        session = device.attach(pid)
    except Exception as e:
        print(f'FATAL: Frida attach failed: {e}')
        try:
            device.kill(pid)
        except Exception:
            pass
        return 5

    # Build agent JS — inject the candidate list as JSON.
    cand_json = json.dumps([[va, name] for va, name, _ctx in CANDIDATES])
    js = AGENT_JS.replace('$CANDIDATES$', cand_json)
    script = session.create_script(js)

    ready = {'value': False}

    def on_message(msg, data):
        if msg['type'] == 'error':
            print(f'  agent error: {msg.get("description")}')
            return
        p = msg.get('payload', {})
        if p.get('kind') == 'ready':
            ready['value'] = True

    script.on('message', on_message)
    script.load()

    # Wait for agent ready (process is suspended — should be fast).
    deadline = time.time() + 10
    while not ready['value'] and time.time() < deadline:
        time.sleep(0.05)

    if not ready['value']:
        print('FATAL: agent never sent "ready" — script load failed.')
        try:
            device.kill(pid)
        except Exception:
            pass
        return 6

    # Check per-candidate attach errors.
    try:
        snap0 = script.exports_sync.snapshot()
        for name, err in snap0.get('attach_errors', {}).items():
            if err:
                print(f'  WARNING: Interceptor.attach FAILED for {name}: {err}')
            else:
                print(f'  attached: {name}')
    except Exception as e:
        print(f'  WARNING: pre-resume snapshot failed: {e}')

    # NOW resume — all interceptors are live before any user code runs.
    print()
    print('  Resuming process ...')
    device.resume(pid)

    # Mute audio (async — audio session appears ~1s after DirectSound init).
    if HAS_PYCAW:
        muted = False
        deadline = time.time() + 10
        while time.time() < deadline:
            if mute_pid_audio(pid):
                elapsed_s = 10 - int(deadline - time.time())
                print(f'  audio muted at t+{elapsed_s}s')
                muted = True
                break
            time.sleep(0.2)
        if not muted:
            print('  WARNING: could not mute audio session within 10s')
    else:
        print('  pycaw not available — audio not muted')

    # Wait for boot to complete and menu to appear (~5-8s typically).
    BOOT_WAIT = 8
    print(f'  Waiting {BOOT_WAIT}s for main menu to appear ...')
    time.sleep(BOOT_WAIT)

    # Check process alive after boot.
    if not psutil.pid_exists(pid):
        print('FATAL: MASHED.exe died during boot — cannot assess main-menu state.')
        try:
            session.detach()
        except Exception:
            pass
        _write_output(OUT_FILE, CANDIDATES, {}, {}, False, False,
                      'PROCESS DIED DURING BOOT')
        return 7

    print(f'  Process alive after boot. Idling {IDLE_SECONDS}s at main menu ...')

    # Build column header.
    names = [name for _va, name, _ctx in CANDIDATES]
    col_w = max(len(n) for n in names) + 2

    header = f'  {"t":>4s}' + ''.join(f'  {n:>{col_w}s}' for n in names)
    print(header)

    last_snap = {'counters': {n: 0 for n in names}, 'attach_errors': {}}
    start = time.time()
    alive = True

    while time.time() - start < IDLE_SECONDS:
        alive = psutil.pid_exists(pid)
        if not alive:
            print('  PROCESS DIED before idle complete — RED result')
            break
        try:
            snap = script.exports_sync.snapshot()
            counters = snap.get('counters', {})
            elapsed = int(time.time() - start)
            row = f'  {elapsed:>4d}' + ''.join(
                f'  {counters.get(n, 0):>{col_w}d}' for n in names
            )
            print(row)
            last_snap = snap
        except Exception as e:
            print(f'  snapshot error: {e}')
            break
        time.sleep(5)

    # Final snapshot.
    try:
        last_snap = script.exports_sync.snapshot()
    except Exception:
        pass

    final_counters = last_snap.get('counters', {})
    final_attach_errors = last_snap.get('attach_errors', {})
    alive_at_end = psutil.pid_exists(pid)
    reached_menu = alive_at_end or alive

    # Cleanup.
    try:
        session.detach()
    except Exception:
        pass
    try:
        device.kill(pid)
    except Exception:
        pass

    # Report.
    print()
    print('=== RESULTS ===')
    print(f'  reached_main_menu : {reached_menu}')
    print(f'  alive_at_end      : {alive_at_end}')
    print()

    overall_verdict = 'GREEN' if reached_menu else 'RED -- process died'
    _write_output(OUT_FILE, CANDIDATES, final_counters, final_attach_errors,
                  reached_menu, alive_at_end, overall_verdict)

    # Per-candidate result table.
    promoted = []
    not_fired = []
    attach_failed = []

    print(f'  {"RVA":<12s}  {"Name":<24s}  {"Count":>6s}  {"AttachErr":>9s}  Verdict')
    print(f'  {"-"*12}  {"-"*24}  {"-"*6}  {"-"*9}  -------')
    for va, name, _ctx in CANDIDATES:
        count = final_counters.get(name, 0)
        aerr = final_attach_errors.get(name)
        if aerr:
            verdict = f'ATTACH_FAIL: {aerr[:40]}'
            attach_failed.append((va, name, count, aerr))
        elif count > 0 and reached_menu:
            verdict = 'GREEN -- C2->C3 eligible'
            promoted.append((va, name, count))
        elif count == 0 and not aerr:
            verdict = 'INCONCLUSIVE -- count=0'
            not_fired.append((va, name, count))
        else:
            verdict = f'RED -- count={count}, alive={reached_menu}'
            not_fired.append((va, name, count))
        print(f'  {va:<12s}  {name:<24s}  {count:>6d}  {"FAIL" if aerr else "ok":>9s}  {verdict}')

    print()
    print(f'  Total eligible for C2->C3 promotion: {len(promoted)}')
    if promoted:
        print('  Promoted candidates:')
        for va, name, count in promoted:
            print(f'    {va}  {name}  (count={count})')
    if not_fired:
        print('  Did NOT fire (count=0) — stay at C2:')
        for va, name, count in not_fired:
            print(f'    {va}  {name}')
    if attach_failed:
        print('  Attach failed — stay at C2:')
        for va, name, count, err in attach_failed:
            print(f'    {va}  {name}  ({err[:60]})')

    print(f'\n  Written to {OUT_FILE}')
    return 0


def _write_output(out_file: Path, candidates, counters, attach_errors,
                  reached_menu, alive_at_end, overall_verdict):
    lines = [
        'mass_canonical_observe_r2.py result  (ROUND 2)',
        '===============================================',
        f'spawn_method      : frida.spawn (suspended before resume)',
        f'idle_seconds      : 30',
        f'reached_main_menu : {reached_menu}',
        f'alive_at_end      : {alive_at_end}',
        f'overall_verdict   : {overall_verdict}',
        '',
        f'{"RVA":<12s}  {"Name":<24s}  {"Count":>6s}  {"AttachErr":>10s}  Verdict',
        f'{"-"*12}  {"-"*24}  {"-"*6}  {"-"*10}  -------',
    ]
    for va, name, _ctx in candidates:
        count = counters.get(name, 0)
        aerr = attach_errors.get(name) or 'none'
        if aerr and aerr != 'none':
            verdict = f'ATTACH_FAIL'
        elif count > 0 and reached_menu:
            verdict = 'GREEN C2->C3'
        elif count == 0:
            verdict = 'INCONCLUSIVE count=0'
        else:
            verdict = 'RED'
        lines.append(f'{va:<12s}  {name:<24s}  {count:>6d}  {aerr[:10]:>10s}  {verdict}')

    out_file.write_text('\n'.join(lines) + '\n', encoding='utf-8')


if __name__ == '__main__':
    sys.exit(main())
