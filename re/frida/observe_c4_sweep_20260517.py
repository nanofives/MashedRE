# C4 promotion observation harness — 2026-05-17 sweep over the 19 c3-batch-h
# hooks (plus s1 drift wins + RaceEndFlagIfEndMode retry).
#
# Pattern (per CLAUDE.md / observe_hooks_at_menu.py):
#   1. Spawn MASHED.exe with d3d9 auto-loader DISABLED (rename d3d9.dll
#      out of the way for the duration of the run, restore on exit). This
#      replicates the proven C4-evidence path: PRE bytes = original, agent
#      Module.load(.asi) + InjectHooks() -> POST bytes = E9, then N seconds
#      of idle aliveness check, NO Interceptor.
#   2. For each hook in the batch, verify opcode=0xE9 + correct rel32.
#   3. Idle N seconds, confirm process is alive throughout.
#   4. RPC-poll final bytes to confirm patches stayed live.
#   5. Write report to log/observe_c4_sweep_<batch>.txt.
#
# Usage:
#   py -3.12 re/frida/observe_c4_sweep_20260517.py --batch B1 [--seconds 10]
import argparse
import ctypes
from ctypes import wintypes
import json
import os
import shutil
import subprocess
import sys
import time
from pathlib import Path

import frida
import psutil

try:
    from pycaw.pycaw import AudioUtilities
    HAS_PYCAW = True
except ImportError:
    HAS_PYCAW = False

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


ROOT       = Path(__file__).resolve().parent.parent.parent
MASHED_EXE = ROOT / 'original' / 'MASHED.exe'
ASI_PATH   = ROOT / 'mashedmod' / 'build' / 'mashed_re_dev.asi'
AGENT_JS   = ROOT / 're' / 'frida' / 'observe_hooks_template.js'
D3D9_LIVE  = ROOT / 'original' / 'd3d9.dll'
D3D9_PARK  = ROOT / 'original' / 'd3d9.dll.parked_sweep_20260517'
LOG_DIR    = ROOT / 'log'
LOG_DIR.mkdir(parents=True, exist_ok=True)


# Hook batches for this sweep — one entry per hook with {name, rva, export}.
BATCHES = {
    'B1': [
        # s5 timer/state cluster (8 hooks)
        {'name': 'LoadingState1Enter',         'rva': '0x004098b0', 'export': 'LoadingState1Enter'},
        {'name': 'LoadingState2Enter',         'rva': '0x00409900', 'export': 'LoadingState2Enter'},
        {'name': 'LoadingState3Enter',         'rva': '0x00409930', 'export': 'LoadingState3Enter'},
        {'name': 'TimerGetBasePtr',            'rva': '0x00413f90', 'export': 'TimerGetBasePtr'},
        {'name': 'TimerDispatch10',            'rva': '0x00426c10', 'export': 'TimerDispatch10'},
        {'name': 'TimerDispatch30',            'rva': '0x00426c30', 'export': 'TimerDispatch30'},
        {'name': 'TimerDispatch70',            'rva': '0x00426c70', 'export': 'TimerDispatch70'},
        {'name': 'SetDat0067ecb8',             'rva': '0x0042c2f0', 'export': 'SetDat0067ecb8'},
    ],
    'B2': [
        # s6 mode/state/timer cluster (5 hooks)
        {'name': 'ModeGatedPlayerCheck',       'rva': '0x00442c80', 'export': 'ModeGatedPlayerCheck'},
        {'name': 'GameStateSlotsFill',         'rva': '0x00429aa0', 'export': 'GameStateSlotsFill'},
        {'name': 'HudDualLabelRender',         'rva': '0x004295a0', 'export': 'HudDualLabelRender'},
        {'name': 'TimerSlotTickDispatcher',    'rva': '0x0043c000', 'export': 'TimerSlotTickDispatcher'},
        {'name': 'PendingOpQueueFlush',        'rva': '0x00475a60', 'export': 'PendingOpQueueFlush'},
    ],
    'B3': [
        # s4 HUD font cluster (4 hooks)
        {'name': 'Sub0041db80_HudThresholdDispatch',  'rva': '0x0041db80', 'export': 'Sub0041db80_HudThresholdDispatch'},
        {'name': 'Sub00403160_SubMode0BViewport',     'rva': '0x00403160', 'export': 'Sub00403160_SubMode0BViewport'},
        {'name': 'FontText_StringTableLookup',        'rva': '0x00427780', 'export': 'FontText_StringTableLookup'},
        {'name': 'FontText_UTF16WidenCopy',           'rva': '0x00427840', 'export': 'FontText_UTF16WidenCopy'},
    ],
    'B4': [
        # s1 frontend drift wins + RaceEndFlagIfEndMode retry (3 hooks)
        {'name': 'MenuAlphaGet',               'rva': '0x0042b930', 'export': 'MenuAlphaGet'},
        {'name': 'RaceEndAltFlagIfEndMode',    'rva': '0x0042fe50', 'export': 'RaceEndAltFlagIfEndMode'},
        {'name': 'RaceEndFlagIfEndMode',       'rva': '0x0042fe30', 'export': 'RaceEndFlagIfEndMode'},
    ],
}


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
        state['pre_snapshot'] = p; print('  [agent] pre_snapshot captured')
    elif kind == 'inject_called':
        print(f"  [agent] InjectHooks() called at {p['export_addr']}")
    elif kind == 'post_snapshot':
        state['post_snapshot'] = p; print('  [agent] post_snapshot captured')
        state['done'] = True
    else:
        print('  [agent]', p)


def mute_pid_audio(pid: int) -> bool:
    if not HAS_PYCAW: return False
    try:
        for sess in AudioUtilities.GetAllSessions():
            if sess.Process and sess.Process.pid == pid:
                sess.SimpleAudioVolume.SetMute(1, None); return True
    except Exception as e:
        print(f"  mute failed: {e}")
    return False


def list_pid_windows(pid: int):
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
        found.append({'hwnd': hwnd,
                      'visible': bool(user32.IsWindowVisible(hwnd)),
                      'owned':   user32.GetWindow(hwnd, GW_OWNER) != 0,
                      'title':   title_buf.value, 'klass': klass_buf.value})
        return True
    user32.EnumWindows(cb, 0)
    return found


def find_main_window(pid: int):
    wins = list_pid_windows(pid)
    def rank(w):
        return (1 if w['visible'] else 0,
                1 if not w['owned'] else 0,
                1 if w['title'] else 0)
    wins.sort(key=rank, reverse=True)
    return wins[0]['hwnd'] if wins else None


def disable_auto_load() -> bool:
    """Park d3d9.dll so MASHED falls back to system d3d9 (no auto .asi load)."""
    if D3D9_LIVE.exists():
        if D3D9_PARK.exists():
            D3D9_PARK.unlink()
        shutil.move(str(D3D9_LIVE), str(D3D9_PARK))
        return True
    return False


def restore_auto_load(was_disabled: bool):
    if was_disabled and D3D9_PARK.exists():
        if D3D9_LIVE.exists():
            D3D9_LIVE.unlink()
        shutil.move(str(D3D9_PARK), str(D3D9_LIVE))


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('--batch', required=True, choices=list(BATCHES.keys()))
    parser.add_argument('--seconds', type=int, default=10)
    parser.add_argument('--boot-wait', type=float, default=8.0)
    parser.add_argument('--no-mute', action='store_true')
    args = parser.parse_args()

    HOOKS = BATCHES[args.batch]
    OUT_FILE = LOG_DIR / f'observe_c4_sweep_20260517_{args.batch}.txt'

    if not MASHED_EXE.exists(): sys.exit(f"missing {MASHED_EXE}")
    if not ASI_PATH.exists():   sys.exit(f"missing {ASI_PATH}")
    if not AGENT_JS.exists():   sys.exit(f"missing {AGENT_JS}")

    print(f"=== observe_c4_sweep_20260517 — batch {args.batch} ({len(HOOKS)} hooks) ===")
    for h in HOOKS:
        print(f"  {h['rva']}  {h['name']}")
    print()

    # Park d3d9.dll so .asi does NOT auto-load — agent will Module.load it
    parked = disable_auto_load()
    if parked:
        print(f"  parked {D3D9_LIVE.name} -> {D3D9_PARK.name} (auto-load disabled)")
    else:
        print(f"  d3d9.dll not present — auto-load already disabled")

    try:
        # Clear mashed.log if not locked
        log_path = ROOT / 'original' / 'mashed.log'
        try:
            if log_path.exists(): log_path.unlink()
        except Exception: pass

        env = {k: v for k, v in os.environ.items() if k != 'MASHED_RE_NO_AUTO_HOOK'}

        startupinfo = subprocess.STARTUPINFO()
        startupinfo.dwFlags    |= subprocess.STARTF_USESHOWWINDOW
        startupinfo.wShowWindow = SW_SHOWMINNOACTIVE

        proc = subprocess.Popen(
            [str(MASHED_EXE)], cwd=str(MASHED_EXE.parent),
            stdin=subprocess.DEVNULL, stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL,
            env=env, startupinfo=startupinfo,
            creationflags=0x00000200 | 0x00000008)
        pid = proc.pid
        print(f"  spawned MASHED.exe  pid={pid}")

        # Re-minimize once window appears
        hwnd = None
        deadline = time.time() + 8
        while time.time() < deadline:
            hwnd = find_main_window(pid)
            if hwnd:
                user32.ShowWindow(hwnd, SW_SHOWMINNOACTIVE)
                print(f"  hwnd 0x{hwnd:08x} re-minimized")
                break
            time.sleep(0.3)
        if not hwnd:
            print('  WARN: no window found within 8s')

        if not args.no_mute:
            muted = False
            deadline = time.time() + 6
            while time.time() < deadline:
                if mute_pid_audio(pid): muted = True; break
                time.sleep(0.2)
            print('  muted audio' if muted else '  audio not muted')

        print(f"  waiting {args.boot_wait}s for main menu")
        time.sleep(args.boot_wait)
        if not psutil.pid_exists(pid):
            print('  process died during boot')
            return 6

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

        config = {'asi_path': str(ASI_PATH).replace('\\', '\\\\'), 'hooks': HOOKS}
        script_text = AGENT_JS.read_text(encoding='utf-8').replace('$CONFIG$', json.dumps(config))
        script = session.create_script(script_text)
        script.on('message', on_message)
        script.load()

        deadline = time.time() + 20
        while not state['done'] and time.time() < deadline:
            time.sleep(0.1)
        if not state['done']:
            print('  agent never reported post_snapshot')
            try: proc.kill()
            except Exception: pass
            return 5
        if state['errors']:
            try: proc.kill()
            except Exception: pass
            return 6

        print(f"  .asi loaded; idling {args.seconds}s with hooks live, NO Interceptor")
        start = time.time()
        alive_throughout = True
        while time.time() - start < args.seconds:
            elapsed = int(time.time() - start)
            alive = psutil.pid_exists(pid)
            print(f"  t={elapsed:>3d}s  alive={alive}")
            if not alive: alive_throughout = False; break
            time.sleep(2)

        final_alive = psutil.pid_exists(pid)
        final_poll = None
        if final_alive:
            try: final_poll = script.exports_sync.poll()
            except Exception as e: print(f'  final poll failed: {e}')

        try: session.detach()
        except Exception: pass

        exit_method = 'unknown'; exit_code = None
        if final_alive:
            target_hwnd = hwnd or find_main_window(pid)
            if target_hwnd:
                user32.PostMessageW(target_hwnd, WM_CLOSE, 0, 0)
                try:
                    exit_code = proc.wait(timeout=8); exit_method = 'wm_close'
                except subprocess.TimeoutExpired:
                    try: proc.kill()
                    except Exception: pass
                    try: exit_code = proc.wait(timeout=3)
                    except Exception: pass
                    exit_method = 'terminate_after_wm_close_timeout'
            else:
                try: proc.kill()
                except Exception: pass
                try: exit_code = proc.wait(timeout=3)
                except Exception: pass
                exit_method = 'terminate_no_hwnd'
        else:
            try: exit_code = proc.wait(timeout=1)
            except Exception: pass
            exit_method = 'died_during_observation'

        return write_report(args, OUT_FILE, HOOKS, alive_throughout, final_alive,
                            final_poll, exit_method, exit_code)
    finally:
        restore_auto_load(parked)
        if parked:
            print(f"  restored {D3D9_LIVE.name} (auto-load re-enabled)")


def write_report(args, out_file, HOOKS, alive_throughout, final_alive, final_poll, exit_method, exit_code):
    pre, post = state['pre_snapshot'], state['post_snapshot']
    lines = []
    lines.append(f'=== observe_c4_sweep_20260517 batch {args.batch} ===')
    lines.append('')
    lines.append(f"observe seconds: {args.seconds}")
    lines.append(f"boot wait:       {args.boot_wait}")
    lines.append(f"hook count:      {len(HOOKS)}")
    lines.append('')

    if not post:
        lines.append('FAIL: agent did not produce post_snapshot')
        out_file.write_text('\n'.join(lines), encoding='utf-8')
        return 1

    pass_flags, fail_flags = [], []
    lines.append(f"asi module base: {post['module_base']}  size: {post['module_size']}")
    lines.append('')
    lines.append('per-hook install verification:')
    for h in post['hooks']:
        ok = h['opcode_ok'] and h['rel32_ok']
        tag = 'OK' if ok else 'FAIL'
        lines.append(f"  [{tag}] {h['name']:<36s} rva={h['rva']}  reimpl={h['reimpl_addr']}")
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
        pass_flags.append(f"process alive throughout {args.seconds}s observation window with {len(HOOKS)} hooks installed")
    else:
        fail_flags.append(f"process died during observation (alive_throughout={alive_throughout}, final_alive={final_alive})")

    if exit_method == 'wm_close' and exit_code == 0:
        pass_flags.append('clean shutdown via PostMessage WM_CLOSE (exit code 0)')
    elif exit_method == 'wm_close':
        pass_flags.append(f'shutdown via WM_CLOSE, exit code {exit_code}')
    elif exit_method == 'died_during_observation':
        fail_flags.append('process exited unexpectedly during observation')
    else:
        pass_flags.append(f'shutdown via {exit_method} (TerminateProcess fallback documented)')

    if final_poll:
        lines.append('')
        lines.append('final byte state (RPC poll after observation window):')
        for name, h in final_poll.items():
            still_patched = h['opcode'] == '0xe9'
            tag = 'OK' if still_patched else 'FAIL'
            lines.append(f"  [{tag}] {name:<36s} rva={h['rva']}  opcode={h['opcode']}  bytes={h['bytes']}")
            if still_patched: pass_flags.append(f"{name} bytes still patched at end of observation")
            else:             fail_flags.append(f"{name} bytes reverted during observation (opcode {h['opcode']})")

    lines.append('')
    lines.append('PASS:')
    for p in pass_flags: lines.append(f"  + {p}")
    if not pass_flags: lines.append('  (none)')
    lines.append('FAIL:')
    for f in fail_flags: lines.append(f"  - {f}")
    if not fail_flags: lines.append('  (none)')

    out_file.write_text('\n'.join(lines), encoding='utf-8')
    print(); print('\n'.join(lines)); print()
    print(f"report written to {out_file}")
    return 0 if not fail_flags else 1


if __name__ == '__main__':
    sys.exit(main())
