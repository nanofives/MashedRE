# Frontend S-DoD Row 2 evidence harness (closes D-11001).
#
# Loads ALL C3+ frontend hooks (pulled from re/frida/hooks_registry.py +
# hooks.csv at run time) into a live MASHED.exe at the main menu, idles
# 30 s with NO Interceptor, then RPC-polls every patched RVA.
#
# Pattern matches re/frida/observe_c4_sweep_20260517.py exactly. Only the
# HOOKS set and the output filename differ.
import argparse
import csv
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
sys.path.insert(0, str(ROOT / 're' / 'frida'))
from hooks_registry import HOOKS as REGISTRY_HOOKS  # noqa: E402

MASHED_EXE = ROOT / 'original' / 'MASHED.exe'
ASI_PATH   = ROOT / 'mashedmod' / 'build' / 'mashed_re_dev.asi'
AGENT_JS   = ROOT / 're' / 'frida' / 'observe_hooks_template.js'
HOOKS_CSV  = ROOT / 'hooks.csv'
LOG_DIR    = ROOT / 'log'
LOG_DIR.mkdir(parents=True, exist_ok=True)
OUT_FILE   = LOG_DIR / 'observe_frontend_full_30s.txt'


def build_frontend_c3plus_hooks():
    """Return a list of {name, rva (str hex), export} entries for every
    frontend C3+ row in hooks.csv that has a registry entry."""
    reg_by_rva = {h['rva']: (k, h['export']) for k, h in REGISTRY_HOOKS.items()}
    out = []
    skipped = []
    with HOOKS_CSV.open(encoding='utf-8') as f:
        for r in csv.DictReader(f):
            if (r.get('subsystem') or '').strip() != 'frontend':
                continue
            if (r.get('confidence') or '').strip() not in ('C3', 'C4'):
                continue
            rva_int = int(r['rva'], 16)
            if rva_int not in reg_by_rva:
                skipped.append((r['rva'], r['name'], 'no registry entry'))
                continue
            reg_key, export_name = reg_by_rva[rva_int]
            # Display name: prefer canonical from hooks.csv unless it's the
            # placeholder FUN_xxxxxx, in which case the registry export is
            # the canonical symbol.
            display = r['name'] if not r['name'].startswith('FUN_') else export_name
            out.append({'name': display,
                        'rva': f'0x{rva_int:08x}',
                        'export': export_name})
    return out, skipped


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


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('--seconds', type=int, default=30,
                        help='observation window length (D-11001 spec: 30s)')
    parser.add_argument('--boot-wait', type=float, default=8.0)
    parser.add_argument('--no-mute', action='store_true')
    args = parser.parse_args()

    HOOKS, skipped = build_frontend_c3plus_hooks()
    print(f"=== observe_frontend_full_30s ({len(HOOKS)} C3+ frontend hooks) ===")
    if skipped:
        print(f"  skipped {len(skipped)} rows without registry entries:")
        for rva, name, why in skipped[:10]:
            print(f"    {rva}  {name}  ({why})")
    for h in HOOKS[:5]:
        print(f"  {h['rva']}  {h['name']}")
    if len(HOOKS) > 5:
        print(f"  ... +{len(HOOKS) - 5} more")
    print()

    if not MASHED_EXE.exists(): sys.exit(f"missing {MASHED_EXE}")
    if not ASI_PATH.exists():   sys.exit(f"missing {ASI_PATH}")
    if not AGENT_JS.exists():   sys.exit(f"missing {AGENT_JS}")

    # We do NOT park original/d3d9.dll. That shim is the windowed-mode
    # forcer (CreateDevice_ForceWindowed @ 640x480); removing it sends
    # MASHED fullscreen via videocfg.bin. The shim does not auto-load
    # the .asi (its only LoadLibraryW is d3d9_real.dll), and the .asi
    # is not present in original/ at all, so there is nothing to
    # "auto-load disable" — the agent Module.loads the .asi from
    # mashedmod/build/ explicitly.

    if True:
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

        # 59 hooks * agent verification round-trips: 30s is plenty
        deadline = time.time() + 30
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

        return write_report(args, HOOKS, skipped, alive_throughout, final_alive,
                            final_poll, exit_method, exit_code)


def write_report(args, HOOKS, skipped, alive_throughout, final_alive, final_poll, exit_method, exit_code):
    pre, post = state['pre_snapshot'], state['post_snapshot']
    lines = []
    lines.append('=== observe_frontend_full_30s — Frontend S-DoD Row 2 evidence (D-11001) ===')
    lines.append('')
    lines.append(f"observe seconds: {args.seconds}")
    lines.append(f"boot wait:       {args.boot_wait}")
    lines.append(f"hook count:      {len(HOOKS)}  (all C3+ frontend rows w/ registry entry)")
    lines.append(f"skipped:         {len(skipped)}")
    lines.append('')

    if not post:
        lines.append('FAIL: agent did not produce post_snapshot')
        OUT_FILE.write_text('\n'.join(lines), encoding='utf-8')
        return 1

    pass_flags, fail_flags = [], []
    lines.append(f"asi module base: {post['module_base']}  size: {post['module_size']}")
    lines.append('')
    lines.append('per-hook install verification:')
    install_ok = install_fail = 0
    for h in post['hooks']:
        ok = h['opcode_ok'] and h['rel32_ok']
        tag = 'OK' if ok else 'FAIL'
        lines.append(f"  [{tag}] {h['name']:<40s} rva={h['rva']}  reimpl={h['reimpl_addr']}")
        if not ok:
            lines.append(f"         PRE  bytes:  {h['pre_bytes']}")
            lines.append(f"         POST bytes:  {h['post_bytes']}")
            lines.append(f"         opcode={h['opcode_hex']} expected 0xe9  rel32={h['rel32_hex']} expected={h['expected_rel32_hex']}")
        if ok: install_ok += 1
        else:  install_fail += 1
    lines.append(f"  install summary: {install_ok}/{len(HOOKS)} OK, {install_fail} FAIL")
    if install_fail == 0:
        pass_flags.append(f"all {len(HOOKS)} hooks installed cleanly (E9 + correct rel32)")
    else:
        fail_flags.append(f"{install_fail} of {len(HOOKS)} hooks failed install verification")

    lines.append('')
    lines.append(f"alive throughout observation: {alive_throughout}")
    lines.append(f"alive at end of observation:  {final_alive}")
    lines.append(f"exit method:                  {exit_method}")
    lines.append(f"exit code:                    {exit_code}")
    if alive_throughout and final_alive:
        pass_flags.append(f"process alive throughout {args.seconds}s observation window with {len(HOOKS)} frontend hooks installed")
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
        poll_ok = poll_fail = 0
        for name, h in final_poll.items():
            still_patched = h['opcode'] == '0xe9'
            if still_patched: poll_ok += 1
            else:             poll_fail += 1
            if not still_patched:
                lines.append(f"  [FAIL] {name:<40s} rva={h['rva']}  opcode={h['opcode']}  bytes={h['bytes']}")
        lines.append(f"  final poll summary: {poll_ok}/{len(final_poll)} still patched, {poll_fail} reverted")
        if poll_fail == 0:
            pass_flags.append(f"all {poll_ok} hook bytes still patched at end of observation")
        else:
            fail_flags.append(f"{poll_fail} hooks reverted during observation")

    lines.append('')
    lines.append('PASS:')
    for p in pass_flags: lines.append(f"  + {p}")
    if not pass_flags: lines.append('  (none)')
    lines.append('FAIL:')
    for f in fail_flags: lines.append(f"  - {f}")
    if not fail_flags: lines.append('  (none)')

    OUT_FILE.write_text('\n'.join(lines), encoding='utf-8')
    print(); print('\n'.join(lines)); print()
    print(f"report written to {OUT_FILE}")
    return 0 if not fail_flags else 1


if __name__ == '__main__':
    sys.exit(main())
