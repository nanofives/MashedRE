# mass_canonical_observe_r3.py
#
# Mass canonical-scenario observation — ROUND 3 — 20 boot-once C2 candidates.
# C2 -> C3 evidence via boot-to-menu canonical observation.
#
# Scenario (identical to rounds 1+2):
#   1. frida.spawn MASHED.exe (suspended) — process is frozen before any user
#      code runs, including CRT startup. This is critical for early-init candidates
#      which fire before a plain attach() could land.
#   2. Interceptor.attach ALL 20 candidates BEFORE resume.
#   3. device.resume(pid) — process boots to main menu.
#   4. Idle 30 seconds on main menu, polling counts.
#   5. Kill MASHED, report per-candidate result table.
#
# All 20 candidates are boot-once or rare-fire (not per-frame hot paths) —
# Interceptor.attach is safe per CLAUDE.md "Frida overhead on hot paths" rules.
#
# Candidate groups:
#   Boot-time D3D/RW init helpers (0x00493f70, 0x00495270, 0x004c2f60,
#   0x004c5c80, 0x004c9eb0, 0x004c9f50, 0x004c9f60, 0x004cbc60, 0x004cbc70,
#   0x004cbc80, 0x004c2d90, 0x00498c00, 0x00498b60):
#     — fire once at boot; HIGH yield expected (70-80%).
#   Util TimerInit (0x00495120):
#     — expected to fire at boot (QueryPerformanceFrequency call).
#   Frontend menu state init (0x00428320, 0x00428390, 0x004274d0, 0x004274e0):
#     — 50-60% yield; some may be track-state-only setters.
#   PIZ loader (0x004671a0):
#     — may or may not fire at boot depending on which assets load.
#   D3D matrix init (0x00499ce0):
#     — expected to fire (identity matrix init at boot).
#
# Realistic prediction: 11-16 promotions of 20 attempted.
#
# Output: log/mass_canonical_observe_r3.txt
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

OUT_FILE = _find_log_dir(ROOT) / 'mass_canonical_observe_r3.txt'

# ---------------------------------------------------------------------------
# 20 C2 candidates — boot-time D3D/RW init + frontend + util.
# (RVA = VA for MASHED.exe ImageBase 0x00400000)
# ---------------------------------------------------------------------------

CANDIDATES = [
    # (va_hex, short_name, expected_context)
    # --- Boot-time D3D/RW init helpers (HIGH yield) ---
    ('0x00493f70', 'sub_00493f70',        'VideoStateFlagGet: returns DAT_00771a04; leaf; video state flag getter'),
    ('0x00495270', 'FUN_00495270',        'HWNDGet: *param_1=FUN_00499710(); D3D init area'),
    ('0x004c2f60', 'FUN_004c2f60',        'RW engine plugin helper: dispatcher cmds 0x12+0x03 engine-stop path'),
    ('0x004c5c80', 'FUN_004c5c80',        'RW init: writes param_1 to *(DAT_007d4054+0x10+DAT_007d3ff8)'),
    ('0x004c9eb0', 'FUN_004c9eb0',        'RW init: writes param_1 to DAT_006181c4; double-indirect dispatch'),
    ('0x004c9f50', 'FUN_004c9f50',        'RW init setter: DAT_007d4134=param_1'),
    ('0x004c9f60', 'FUN_004c9f60',        'RW init conditional setter: DAT_007d413c+vtable dispatch'),
    ('0x004cbc60', 'FUN_004cbc60',        'RW init setter: DAT_007d4598=param_1 (9B trivial setter)'),
    ('0x004cbc70', 'FUN_004cbc70',        'RW init getter: returns DAT_007d4598 (5B trivial getter)'),
    ('0x004cbc80', 'FUN_004cbc80',        'RW init setter: DAT_007d459c=param_1 (9B trivial setter)'),
    ('0x004c2d90', 'FUN_004c2d90',        'RwEngineRegisterPlugin shim: FUN_004d7de0 forward; 17 callers'),
    ('0x00498c00', 'VideoModeTableInit',  'D3D subsystem+mode enumeration init; sole caller FUN_00499400'),
    ('0x00498b60', 'FUN_00498b60',        'video_cfg teardown: frees pointer-array at DAT_00773408'),
    # --- Util TimerInit (expected fire) ---
    ('0x00495120', 'FUN_00495120',        'TimerInit/QPFStore: QueryPerformanceFrequency->DAT_00771e70/e74'),
    # --- Frontend menu/state init (50-60% yield) ---
    ('0x00428320', 'FUN_00428320',        'text width measure B: FUN_00427840 item ctx -> FUN_005554d0 scaled'),
    ('0x00428390', 'FUN_00428390',        'game state setter: DAT_0067d960=param_1'),
    ('0x004274d0', 'FUN_004274d0',        'LOC_INIT_FN: copies DAT_007719e8->DAT_007f0f60'),
    ('0x004274e0', 'FUN_004274e0',        'LOC_FN: switch lang->filename'),
    # --- PIZ loader (fires per-PIZ during boot asset load) ---
    ('0x004671a0', 'sub_004671a0',        'vehicle-0 getter via FUN_0042b930==3 + param_1 dispatch'),
    # --- D3D matrix init (expected fire) ---
    ('0x00499ce0', 'FUN_00499ce0',        'InitViewportMatrix: zeros 12 floats+sets 4 to 1.0f at DAT_00773928'),
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
    print('=== mass_canonical_observe_r3.py  (ROUND 3) ===')
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
        'mass_canonical_observe_r3.py result  (ROUND 3)',
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
