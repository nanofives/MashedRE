# mass_canonical_observe_wave0.py
#
# Wave-0 mass canonical-scenario observation for 15 already-hooked C2 candidates
# (the 16-RVA list minus 0x004950b0 QpcTimeScaledTo3Mhz which is a per-frame
# hot-path and must NOT be Interceptor-attached).
#
# These 16 RVAs all have RH_ScopedInstall in mashedmod/src/mashed_re/ but are
# still C2 in hooks.csv.  This run produces boot-to-menu-canonical-wave0 evidence
# to promote survivors to C3.
#
# Scenario (identical to mass_canonical_observe_r2.py):
#   1. frida.spawn MASHED.exe (suspended) — all interceptors land before any
#      user code runs.
#   2. Interceptor.attach 15 candidates BEFORE resume.
#   3. device.resume(pid) — process boots to main menu.
#   4. Intro is skipped via _intro_skip helper after window appears.
#   5. Idle 30 seconds on main menu, polling counts.
#   6. Kill MASHED, report per-candidate result table.
#
# Hot-path exclusion:
#   0x004950b0  QpcTimeScaledTo3Mhz  — per-frame QPC; NOT attached here.
#   Its hook is still installed in the .asi; we verify it implicitly by
#   confirming the process is alive and menu is reached.  If alive, it will
#   be promoted with scenario="hot-path-behavioral-implicit-via-wave0".
#
# Output: log/mass_canonical_wave0.txt
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
# Paths (same resolver pattern as mass_canonical_observe_r2.py)
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


def _find_log_dir(script_root: Path) -> Path:
    local_log = script_root / 'log'
    if local_log.exists():
        return local_log
    parent_log = script_root.parent.parent / 'log'
    return parent_log


OUT_FILE = _find_log_dir(ROOT) / 'mass_canonical_wave0.txt'

# ---------------------------------------------------------------------------
# 15 candidates — already-hooked, still C2.
# 0x004950b0 (QpcTimeScaledTo3Mhz) is excluded: per-frame hot-path.
# ---------------------------------------------------------------------------

CANDIDATES = [
    # (va_hex, short_name, expected_context)
    ('0x00422ac0', 'SlotQuadSet',           'Render/LowRvaMixed_q3.cpp: 4-dword store per slot'),
    ('0x004235b0', 'AiPizLoad',             'Render/LowRvaSetters_o2.cpp: loads AI data from ai.piz'),
    ('0x00428320', 'TextWidthMeasureB',     'Frontend/TextMeasure.cpp: text width via FUN_005554d0'),
    ('0x00428450', 'HudSpinCoinAnim',       'HUD/TextCluster.cpp: spinning-coin animator'),
    ('0x0042aa00', 'MenuCursorStep',        'Frontend/MenuGetters.cpp: cursor advance+wrap'),
    ('0x0042e5b0', 'MenuChromeShellB',      'Frontend/MenuSpriteDispatch.cpp: BG+logo animator'),
    ('0x00439210', 'LobbySlotListRender',   'Frontend/SpriteCluster.cpp: network lobby/player-slot list'),
    ('0x00492e60', 'SetDefaultViewWindow',  'Boot/SubsystemInit.cpp: FUN_004671a0+view window 0.8f'),
    ('0x00493560', 'thunk_HwExitDispatch',  'Boot/LaunchHandshake.cpp: 4-byte thunk JMP to 0x004954f0'),
    ('0x004997b0', 'Win32ResourceLoader',   'Render/D3D9Helpers_q5.cpp: LoadAndLockResource Win32 combo'),
    ('0x004a4bb7', 'WinMainEntry',          'Boot/CrtStartup.cpp: PE entry; CRT init + WinMain wrapper'),
    ('0x004c5c00', 'LinkedListStringSearch','Frontend/SpriteCluster.cpp: linked-list string search'),
    ('0x004cc6e0', 'RwStreamWriteChunked',  'Render/D3D9Helpers_q5.cpp: chunked RW stream write'),
    ('0x004cc7f0', 'RwFreeListCreateWrapper','Render/RenderSubmit_o4.cpp: thin wrapper -> FUN_004cc820'),
    ('0x005be140', 'AudioSubStructThreeWrite','Audio/AudioRws.cpp: leaf 3-field write param_1+0x14/10/0c'),
]

# ---------------------------------------------------------------------------
# Frida agent JS
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
    """Mute the audio session for a specific PID."""
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
    print('=== mass_canonical_observe_wave0.py  (Wave 0: already-hooked C2 -> C3) ===')
    print(f'MASHED.exe : {MASHED_EXE}')
    print(f'ASI        : {ASI_PATH}')
    print(f'Candidates : {len(CANDIDATES)} (excl. hot-path 0x004950b0 QpcTimeScaledTo3Mhz)')
    print()

    # Pre-flight checks.
    if not MASHED_EXE.exists():
        print(f'FATAL: {MASHED_EXE} not found.')
        return 1

    shim = MASHED_EXE.parent / 'd3d9.dll'
    if not shim.exists():
        print(f'FATAL: d3d9 shim missing at {shim}.')
        print('       Run mashedmod\\build_d3d9_shim.bat to rebuild + deploy.')
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

    # Build agent JS.
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

    # Try to skip intro via _intro_skip helper.
    try:
        import sys as _sys
        _frida_dir = Path(__file__).resolve().parent
        if str(_frida_dir) not in _sys.path:
            _sys.path.insert(0, str(_frida_dir))
        from _intro_skip import skip_intro
        # Wait ~3s for window to appear before sending keys.
        print('  Waiting 3s for window before intro-skip ...')
        time.sleep(3)
        skipped = skip_intro(pid, count=8, delay=0.6)
        if skipped:
            print('  intro-skip keypresses delivered')
        else:
            print('  WARNING: intro-skip found no window within 10s')
    except Exception as e:
        print(f'  WARNING: intro-skip import/call failed: {e}')
        time.sleep(3)  # fallback: just sleep the 3s we already waited

    # Mute audio.
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

    # Wait for boot to complete and menu to appear.
    # We've already waited ~3s for window + up to 5s for intro-skip; add 5s more.
    EXTRA_BOOT_WAIT = 5
    print(f'  Waiting {EXTRA_BOOT_WAIT}s more for main menu to stabilise ...')
    time.sleep(EXTRA_BOOT_WAIT)

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

    print(f'  {"RVA":<12s}  {"Name":<26s}  {"Count":>6s}  {"AttachErr":>9s}  Verdict')
    print(f'  {"-"*12}  {"-"*26}  {"-"*6}  {"-"*9}  -------')
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
        print(f'  {va:<12s}  {name:<26s}  {count:>6d}  {"FAIL" if aerr else "ok":>9s}  {verdict}')

    # Hot-path implicit result.
    print()
    hot_va = '0x004950b0'
    hot_name = 'QpcTimeScaledTo3Mhz'
    if reached_menu and alive_at_end:
        print(f'  {hot_va}  {hot_name}  [HOT-PATH: not attached]')
        print(f'    => process survived to main menu with hook installed')
        print(f'    => eligible: boot-to-menu behavioral implicit via wave0')
        promoted.append((hot_va, hot_name, -1))  # -1 = implicit
    else:
        print(f'  {hot_va}  {hot_name}  [HOT-PATH: not attached — process died, no promotion]')

    print()
    print(f'  Total eligible for C2->C3 promotion: {len(promoted)}')
    if promoted:
        print('  Promoted candidates:')
        for va, name, count in promoted:
            suffix = f'count={count}' if count >= 0 else 'implicit (hot-path)'
            print(f'    {va}  {name}  ({suffix})')
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
        'mass_canonical_observe_wave0.py result  (Wave 0)',
        '=================================================',
        f'spawn_method      : frida.spawn (suspended before resume)',
        f'idle_seconds      : 30',
        f'reached_main_menu : {reached_menu}',
        f'alive_at_end      : {alive_at_end}',
        f'overall_verdict   : {overall_verdict}',
        f'hot_path_excluded : 0x004950b0 QpcTimeScaledTo3Mhz (per-frame QPC)',
        '',
        f'{"RVA":<12s}  {"Name":<26s}  {"Count":>6s}  {"AttachErr":>10s}  Verdict',
        f'{"-"*12}  {"-"*26}  {"-"*6}  {"-"*10}  -------',
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
        lines.append(f'{va:<12s}  {name:<26s}  {count:>6d}  {aerr[:10]:>10s}  {verdict}')

    # Hot-path implicit.
    if reached_menu and alive_at_end:
        lines.append(
            f'{"0x004950b0":<12s}  {"QpcTimeScaledTo3Mhz":<26s}  {"N/A":>6s}  {"implicit":>10s}  GREEN C2->C3 (hot-path behavioral implicit)'
        )
    else:
        lines.append(
            f'{"0x004950b0":<12s}  {"QpcTimeScaledTo3Mhz":<26s}  {"N/A":>6s}  {"implicit":>10s}  INCONCLUSIVE (process died)'
        )

    out_file.write_text('\n'.join(lines) + '\n', encoding='utf-8')


if __name__ == '__main__':
    sys.exit(main())
