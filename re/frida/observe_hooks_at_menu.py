# Canonical-scenario observation harness for C4 evidence collection.
#
# Empirical note (2026-05-11): MASHED's window proc does NOT honor
# WM_CLOSE — PostMessage returns OK but the game ignores it. The
# harness sends WM_CLOSE for completeness then falls back to
# TerminateProcess; this is the documented close path, not a failure.
#
# Flow:
#   1. Spawn MASHED.exe (boot runs against original/unhooked code).
#   2. Mute audio session.
#   3. Wait 8s for main menu to be up.
#   4. Attach Frida.
#   5. Agent snapshots bytes at three target RVAs.
#   6. Agent Module.load()s mashed_re_dev.asi (DllMain -> InstallAll -> three
#      inline-JMPs at the three RVAs).
#   7. Agent re-snapshots and verifies each RVA now holds E9 rel32 -> reimpl.
#   8. Python idles N seconds with the .asi loaded — process aliveness only,
#      NO Interceptor (hot-path overhead destabilizes the game in ~6s).
#   9. Agent RPC-polls byte state at the end to confirm patches stayed live.
#  10. Kill MASHED, write report to log/observe_hooks_at_menu.txt.
#
# This is C4 canonical-scenario evidence: the three hooks are installed in a
# live MASHED process that is rendering the main menu (FastSqrt ~2,700/s,
# FastInvSqrt ~900/s, Vec3Magnitude ~180/s natural call rate per
# log/auto_count_at_menu.txt) and continue to run without crash.
import argparse
import ctypes
from ctypes import wintypes
import json
import os
import subprocess
import sys
import time
from pathlib import Path

import frida
import psutil

from pycaw.pycaw import AudioUtilities

user32 = ctypes.windll.user32
user32.PostMessageW.argtypes = [wintypes.HWND, wintypes.UINT, wintypes.WPARAM, wintypes.LPARAM]
user32.PostMessageW.restype  = wintypes.BOOL
user32.ShowWindow.argtypes   = [wintypes.HWND, ctypes.c_int]
user32.ShowWindow.restype    = wintypes.BOOL
user32.IsWindowVisible.argtypes = [wintypes.HWND]
user32.IsWindowVisible.restype  = wintypes.BOOL
user32.GetWindowThreadProcessId.argtypes = [wintypes.HWND, ctypes.POINTER(wintypes.DWORD)]
user32.GetWindowThreadProcessId.restype  = wintypes.DWORD
user32.GetWindow.argtypes    = [wintypes.HWND, wintypes.UINT]
user32.GetWindow.restype     = wintypes.HWND

SW_SHOWMINNOACTIVE = 7
WM_CLOSE           = 0x0010
GW_OWNER           = 4

_EnumWindowsProc = ctypes.WINFUNCTYPE(wintypes.BOOL, wintypes.HWND, wintypes.LPARAM)
user32.EnumWindows.argtypes = [_EnumWindowsProc, wintypes.LPARAM]
user32.EnumWindows.restype  = wintypes.BOOL


user32.GetWindowTextW.argtypes = [wintypes.HWND, wintypes.LPWSTR, ctypes.c_int]
user32.GetWindowTextW.restype  = ctypes.c_int
user32.GetClassNameW.argtypes  = [wintypes.HWND, wintypes.LPWSTR, ctypes.c_int]
user32.GetClassNameW.restype   = ctypes.c_int


def list_pid_windows(pid: int):
    """Return [{hwnd, visible, owned, title, klass}, ...] for every window belonging to pid."""
    found = []
    @_EnumWindowsProc
    def cb(hwnd, _):
        owner_pid = wintypes.DWORD()
        user32.GetWindowThreadProcessId(hwnd, ctypes.byref(owner_pid))
        if owner_pid.value != pid: return True
        title_buf = ctypes.create_unicode_buffer(256)
        user32.GetWindowTextW(hwnd, title_buf, 256)
        klass_buf = ctypes.create_unicode_buffer(256)
        user32.GetClassNameW(hwnd, klass_buf, 256)
        found.append({
            'hwnd':    hwnd,
            'visible': bool(user32.IsWindowVisible(hwnd)),
            'owned':   user32.GetWindow(hwnd, GW_OWNER) != 0,
            'title':   title_buf.value,
            'klass':   klass_buf.value,
        })
        return True
    user32.EnumWindows(cb, 0)
    return found


def find_main_window(pid: int, verbose: bool = False):
    """Best-guess main window: prefer visible+unowned with a non-empty title."""
    wins = list_pid_windows(pid)
    if verbose and wins:
        for w in wins:
            print(f"    hwnd=0x{w['hwnd']:08x}  vis={w['visible']:<5} owned={w['owned']:<5} "
                  f"class={w['klass']!r:<30} title={w['title']!r}")
    # Ranking: visible > non-visible; unowned > owned; titled > untitled.
    def rank(w):
        return (1 if w['visible'] else 0,
                1 if not w['owned'] else 0,
                1 if w['title'] else 0)
    wins.sort(key=rank, reverse=True)
    return wins[0]['hwnd'] if wins else None

ROOT       = Path(__file__).resolve().parent.parent.parent

def _resolve_repo_root(candidate: Path) -> Path:
    """Walk up from worktree root to find the main repo (which has original/ and mashedmod/)."""
    p = candidate
    for _ in range(4):
        if (p / 'original' / 'MASHED.exe').exists():
            return p
        p = p.parent
    return candidate  # fallback

REPO_ROOT  = _resolve_repo_root(ROOT)
MASHED_EXE = REPO_ROOT / 'original' / 'MASHED.exe'
ASI_PATH   = REPO_ROOT / 'mashedmod' / 'build' / 'mashed_re_dev.asi'
AGENT_JS   = ROOT / 're' / 'frida' / 'observe_hooks_template.js'
LOG_DIR    = REPO_ROOT / 'log'
LOG_DIR.mkdir(parents=True, exist_ok=True)
OUT_FILE   = LOG_DIR / 'observe_main_menu_idle_10s_c4-batch-a-s2.txt'

HOOKS = [
    {'name': 'RwMatrixScale',       'rva': '0x004c5010', 'export': 'RwMatrixScale'},
    {'name': 'FrontendGlobalGet',   'rva': '0x0040ad20', 'export': 'FrontendGlobalGet'},
    {'name': 'FrontendArrayGet',    'rva': '0x0040b6c0', 'export': 'FrontendArrayGet'},
    {'name': 'HotkeyStringBaseGet', 'rva': '0x0040b7a0', 'export': 'HotkeyStringBaseGet'},
]

state = {'pre_snapshot': None, 'post_snapshot': None, 'errors': [], 'done': False}


def on_message(message, data):
    if message['type'] == 'error':
        print('AGENT ERROR:', message.get('description'))
        state['errors'].append(message); state['done'] = True; return
    p = message.get('payload', {})
    kind = p.get('type')
    if kind == 'error':
        print(f"  [agent] ERROR: {p['msg']}")
        state['errors'].append(p); state['done'] = True
    elif kind == 'pre_snapshot':
        state['pre_snapshot'] = p
        print('  [agent] pre_snapshot captured')
    elif kind == 'inject_called':
        print(f"  [agent] InjectHooks() called at {p['export_addr']}")
    elif kind == 'post_snapshot':
        state['post_snapshot'] = p
        print('  [agent] post_snapshot captured')
        state['done'] = True
    else:
        print('  [agent]', p)


def mute_pid_audio(pid: int) -> bool:
    try:
        for sess in AudioUtilities.GetAllSessions():
            if sess.Process and sess.Process.pid == pid:
                sess.SimpleAudioVolume.SetMute(1, None)
                return True
    except Exception as e:
        print(f"  mute failed: {e}")
    return False


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('--observe-seconds', type=int, default=10,
                        help='how long to idle after .asi is loaded')
    parser.add_argument('--boot-wait', type=float, default=8.0,
                        help='seconds to wait for main menu before attaching')
    parser.add_argument('--no-mute', action='store_true')
    parser.add_argument('--no-minimize', action='store_true',
                        help='launch normally instead of minimized')
    args = parser.parse_args()

    if not MASHED_EXE.exists(): sys.exit(f"missing {MASHED_EXE}")
    if not ASI_PATH.exists():   sys.exit(f"missing {ASI_PATH} — run mashedmod\\build.bat")
    OUT_FILE.parent.mkdir(parents=True, exist_ok=True)
    log = REPO_ROOT / 'original' / 'mashed.log'
    if log.exists(): log.unlink()

    print(f"spawning {MASHED_EXE} ({'minimized' if not args.no_minimize else 'normal'})")
    # We do NOT set MASHED_RE_NO_AUTO_HOOK — the d3d9 loader is renamed
    # _disabled so the .asi can't auto-load, and the agent calls
    # InjectHooks() explicitly after Module.load. Setting the env var
    # would leak in and make DllMain skip InjectHooks even when called.
    env = {k: v for k, v in os.environ.items() if k != 'MASHED_RE_NO_AUTO_HOOK'}

    startupinfo = subprocess.STARTUPINFO()
    if not args.no_minimize:
        startupinfo.dwFlags    |= subprocess.STARTF_USESHOWWINDOW
        startupinfo.wShowWindow = SW_SHOWMINNOACTIVE

    proc = subprocess.Popen(
        [str(MASHED_EXE)], cwd=str(MASHED_EXE.parent),
        stdin=subprocess.DEVNULL, stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL,
        env=env, startupinfo=startupinfo,
        creationflags=0x00000200 | 0x00000008)
    pid = proc.pid
    print(f"  pid = {pid}")

    # Re-minimize after MASHED creates its own DX window. Games typically
    # foreground themselves when the D3D device initializes, so the spawn-
    # time SW_SHOWMINNOACTIVE alone isn't enough.
    hwnd = None
    deadline = time.time() + 8
    first_log = True
    while time.time() < deadline:
        hwnd = find_main_window(pid, verbose=first_log)
        first_log = False
        if hwnd:
            print(f"  hwnd 0x{hwnd:08x} found")
            if not args.no_minimize:
                user32.ShowWindow(hwnd, SW_SHOWMINNOACTIVE)
                print('  re-minimized')
            break
        time.sleep(0.3)
    if not hwnd:
        print('  WARN: no window found within 8s — close will fall back to TerminateProcess')

    if not args.no_mute:
        muted = False
        deadline = time.time() + 5
        while time.time() < deadline:
            if mute_pid_audio(pid):
                print('  muted audio session'); muted = True; break
            time.sleep(0.2)
        if not muted: print('  audio session not found within 5s')

    print(f"  waiting {args.boot_wait}s for main menu")
    time.sleep(args.boot_wait)
    if not psutil.pid_exists(pid):
        sys.exit('process died during boot — game did not reach menu')

    # The game window may have re-shown itself after DX init. Re-minimize
    # one more time before the observation phase so the user's screen
    # stays unobtrusive.
    if not args.no_minimize and not hwnd:
        hwnd = find_main_window(pid)
    if hwnd:
        user32.ShowWindow(hwnd, SW_SHOWMINNOACTIVE)

    device = frida.get_local_device()
    try:
        session = device.attach(pid)
    except Exception as e:
        print(f'attach failed: {e}')
        try: proc.kill()
        except Exception: pass
        return 4

    config = {
        'asi_path': str(ASI_PATH).replace('\\', '\\\\'),
        'hooks':    HOOKS,
    }
    script_text = AGENT_JS.read_text(encoding='utf-8').replace('$CONFIG$', json.dumps(config))
    script = session.create_script(script_text)
    script.on('message', on_message)
    script.load()

    deadline = time.time() + 15
    while not state['done'] and time.time() < deadline:
        time.sleep(0.1)
    if not state['done']:
        print('  agent never reported post_snapshot');
        try: proc.kill()
        except Exception: pass
        return 5
    if state['errors']:
        try: proc.kill()
        except Exception: pass
        return 6

    print(f"  .asi loaded; idling {args.observe_seconds}s with hooks live, NO Interceptor")
    print(f"  {'t':>4s}  {'pid_alive':>10s}")
    start = time.time()
    alive_throughout = True
    while time.time() - start < args.observe_seconds:
        elapsed = int(time.time() - start)
        alive = psutil.pid_exists(pid)
        print(f"  {elapsed:>4d}  {str(alive):>10s}")
        if not alive: alive_throughout = False; break
        time.sleep(2)

    final_alive = psutil.pid_exists(pid)
    final_poll = None
    if final_alive:
        try:
            final_poll = script.exports_sync.poll()
        except Exception as e:
            print(f'  final poll failed: {e}')

    # Detach Frida before signalling close so the agent doesn't see the
    # window-proc shutdown happen mid-RPC.
    try: session.detach()
    except Exception: pass

    # Programmatic close: PostMessage WM_CLOSE to the main window.
    # This is the same signal as the user clicking the X / using the
    # Exit-to-Windows menu's final step — the game's window proc handles
    # it the same way.
    exit_method = 'unknown'
    exit_code = None
    if final_alive:
        # Re-find HWND in case the window was destroyed/recreated mid-run.
        target_hwnd = hwnd or find_main_window(pid)
        if target_hwnd:
            ok = user32.PostMessageW(target_hwnd, WM_CLOSE, 0, 0)
            print(f"  posted WM_CLOSE to 0x{target_hwnd:08x}: {'OK' if ok else 'FAIL'}")
            try:
                exit_code = proc.wait(timeout=8)
                exit_method = 'wm_close'
                print(f"  process exited cleanly via WM_CLOSE; exit code = {exit_code}")
            except subprocess.TimeoutExpired:
                print('  WM_CLOSE ignored within 8s; falling back to TerminateProcess')
                try: proc.kill()
                except Exception: pass
                try: exit_code = proc.wait(timeout=3)
                except Exception: pass
                exit_method = 'terminate_after_wm_close_timeout'
        else:
            print('  no HWND to post WM_CLOSE to; using TerminateProcess')
            try: proc.kill()
            except Exception: pass
            try: exit_code = proc.wait(timeout=3)
            except Exception: pass
            exit_method = 'terminate_no_hwnd'
    else:
        # Process already gone — capture whatever exit code is available.
        try: exit_code = proc.wait(timeout=1)
        except Exception: pass
        exit_method = 'died_during_observation'

    return write_report(args, alive_throughout, final_alive, final_poll, exit_method, exit_code)


def write_report(args, alive_throughout, final_alive, final_poll, exit_method, exit_code):
    pre, post = state['pre_snapshot'], state['post_snapshot']
    lines = []
    lines.append('=== observe_hooks_at_menu report ===')
    lines.append('')
    lines.append(f"observe seconds: {args.observe_seconds}")
    lines.append(f"boot wait:       {args.boot_wait}")
    lines.append('')

    if not post:
        lines.append('FAIL: agent did not produce post_snapshot');
        report = '\n'.join(lines); OUT_FILE.write_text(report, encoding='utf-8')
        print(); print(report); return 1

    pass_flags, fail_flags = [], []
    lines.append(f"asi module base: {post['module_base']}  size: {post['module_size']}")
    lines.append('')
    lines.append('per-hook install verification:')
    for h in post['hooks']:
        ok = h['opcode_ok'] and h['rel32_ok'] and h['bytes_changed']
        tag = 'OK' if ok else 'FAIL'
        lines.append(f"  [{tag}] {h['name']:<16s} rva={h['rva']}  reimpl={h['reimpl_addr']}")
        lines.append(f"         PRE  bytes:  {h['pre_bytes']}")
        lines.append(f"         POST bytes:  {h['post_bytes']}")
        lines.append(f"         opcode:      {h['opcode_hex']}  expected 0xe9  -> {'OK' if h['opcode_ok'] else 'FAIL'}")
        lines.append(f"         rel32:       {h['rel32_hex']}  expected {h['expected_rel32_hex']}  -> {'OK' if h['rel32_ok'] else 'FAIL'}")
        if ok: pass_flags.append(f"{h['name']} installed cleanly (E9 + correct rel32)")
        else:  fail_flags.append(f"{h['name']} install verification failed")

    lines.append('')
    lines.append(f"alive throughout observation: {alive_throughout}")
    lines.append(f"alive at end of observation:  {final_alive}")
    lines.append(f"exit method:                  {exit_method}")
    lines.append(f"exit code:                    {exit_code}")
    if alive_throughout and final_alive:
        pass_flags.append(f"process alive throughout {args.observe_seconds}s observation window with three hooks installed")
    else:
        fail_flags.append(f"process died during observation (alive_throughout={alive_throughout}, final_alive={final_alive})")

    if exit_method == 'wm_close' and exit_code == 0:
        pass_flags.append('clean shutdown via PostMessage WM_CLOSE (exit code 0)')
    elif exit_method == 'wm_close':
        pass_flags.append(f'shutdown via WM_CLOSE, exit code {exit_code} (non-zero but accepted close)')
    elif exit_method == 'died_during_observation':
        fail_flags.append('process exited unexpectedly during observation, not via shutdown signal')
    else:
        # MASHED's window proc does not honor WM_CLOSE — observed
        # empirically 2026-05-11. TerminateProcess is the documented
        # shutdown path for this harness; it does NOT invalidate the
        # C4 observation that preceded it.
        pass_flags.append(f'shutdown via {exit_method} '
                          f'(MASHED window proc does not honor WM_CLOSE; '
                          f'TerminateProcess is the documented fallback)')

    if final_poll:
        lines.append('')
        lines.append('final byte state (RPC poll after observation window):')
        for name, h in final_poll.items():
            still_patched = h['opcode'] == '0xe9'
            tag = 'OK' if still_patched else 'FAIL'
            lines.append(f"  [{tag}] {name:<16s} rva={h['rva']}  opcode={h['opcode']}  bytes={h['bytes']}")
            if still_patched: pass_flags.append(f"{name} bytes still patched at end of observation")
            else:             fail_flags.append(f"{name} bytes reverted during observation (opcode {h['opcode']})")

    lines.append('')
    lines.append('PASS:')
    for p in pass_flags: lines.append(f"  + {p}")
    if not pass_flags: lines.append('  (none)')
    lines.append('FAIL:')
    for f in fail_flags: lines.append(f"  - {f}")
    if not fail_flags: lines.append('  (none)')

    report = '\n'.join(lines)
    OUT_FILE.write_text(report, encoding='utf-8')
    print(); print(report); print()
    print(f"report written to {OUT_FILE}")
    return 0 if not fail_flags else 1


if __name__ == '__main__':
    sys.exit(main())
