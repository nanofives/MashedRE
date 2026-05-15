# Canonical-scenario observation for c4-batch-a-s5 lap score getter hooks.
# Loads the .asi, then instruments LapLapsGetBySlot / LapSecsGetBySlot /
# LapFracGetBySlot / LapTimeALessThanB via Interceptor during main-menu idle.
# Expects 0 invocations (results screen not shown during idle boot).
import argparse, ctypes, json, os, subprocess, sys, time
from ctypes import wintypes
from pathlib import Path
import frida, psutil
from pycaw.pycaw import AudioUtilities

ROOT       = Path(__file__).resolve().parent.parent.parent
MASHED_EXE = ROOT / 'original' / 'MASHED.exe'
ASI_PATH   = ROOT / 'mashedmod' / 'build' / 'mashed_re_dev.asi'
OUT_FILE   = ROOT / 'log' / 'observe_main_menu_idle_10s_c4-batch-a-s5.txt'

LAP_HOOKS = {
    'LapLapsGetBySlot':  '0x00429a80',
    'LapSecsGetBySlot':  '0x00429a90',
    'LapFracGetBySlot':  '0x00429a70',
    'LapTimeALessThanB': '0x00429870',
}

SW_SHOWMINNOACTIVE = 7
WM_CLOSE = 0x0010
GW_OWNER = 4
user32 = ctypes.windll.user32
_EnumWindowsProc = ctypes.WINFUNCTYPE(wintypes.BOOL, wintypes.HWND, wintypes.LPARAM)
user32.EnumWindows.argtypes = [_EnumWindowsProc, wintypes.LPARAM]
user32.EnumWindows.restype  = wintypes.BOOL
user32.GetWindowThreadProcessId.argtypes = [wintypes.HWND, ctypes.POINTER(wintypes.DWORD)]
user32.GetWindowThreadProcessId.restype  = wintypes.DWORD
user32.IsWindowVisible.argtypes = [wintypes.HWND]
user32.IsWindowVisible.restype  = wintypes.BOOL
user32.ShowWindow.argtypes = [wintypes.HWND, ctypes.c_int]
user32.ShowWindow.restype  = wintypes.BOOL


def find_main_window(pid):
    found = []
    @_EnumWindowsProc
    def cb(hwnd, _):
        dpid = wintypes.DWORD()
        user32.GetWindowThreadProcessId(hwnd, ctypes.byref(dpid))
        if dpid.value == pid:
            found.append((bool(user32.IsWindowVisible(hwnd)), hwnd))
        return True
    user32.EnumWindows(cb, 0)
    found.sort(reverse=True)
    return found[0][1] if found else None


def mute_pid(pid):
    try:
        for s in AudioUtilities.GetAllSessions():
            if s.Process and s.Process.pid == pid:
                s.SimpleAudioVolume.SetMute(1, None)
                return True
    except Exception as e:
        print(f'  mute failed: {e}')
    return False


AGENT_TEMPLATE = r"""
'use strict';
const ASI_PATH = REPLACEME_ASI;
const LAP_HOOKS = REPLACEME_HOOKS;
const BASE_RVAS = REPLACEME_RVAS;
const counters = {};

const pre = {};
BASE_RVAS.forEach(function(info) {
    try { pre[info.name] = Array.from(new Uint8Array(Memory.readByteArray(ptr(info.rva), 5))); }
    catch(e) { pre[info.name] = null; }
});
send({type: 'pre_snapshot', data: pre});

const mod = Module.load(ASI_PATH);
send({type: 'asi_loaded', base: mod.base.toString()});

const post = {};
BASE_RVAS.forEach(function(info) {
    try { post[info.name] = Array.from(new Uint8Array(Memory.readByteArray(ptr(info.rva), 5))); }
    catch(e) { post[info.name] = null; }
});
send({type: 'post_snapshot', data: post});

Object.keys(LAP_HOOKS).forEach(function(name) {
    counters[name] = 0;
    try {
        Interceptor.attach(ptr(LAP_HOOKS[name]), {
            onEnter: function() { counters[name]++; }
        });
    } catch(e) {
        send({type: 'attach_fail', name: name, err: e.message});
    }
});
rpc.exports = { snapshot: function() { return counters; } };
send({type: 'ready'});
"""


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('--observe-seconds', type=int, default=10)
    parser.add_argument('--boot-wait', type=float, default=8.0)
    parser.add_argument('--no-mute', action='store_true')
    args = parser.parse_args()

    lines = []
    def log(s):
        print(s)
        lines.append(s)

    if not MASHED_EXE.exists():
        sys.exit(f'missing {MASHED_EXE}')
    if not ASI_PATH.exists():
        sys.exit(f'missing {ASI_PATH} — run mashedmod\\build.bat')
    OUT_FILE.parent.mkdir(parents=True, exist_ok=True)

    log('=== observe_lap_hooks_at_menu — c4-batch-a-s5 ===')
    log(f'scenario: main_menu_idle_{args.observe_seconds}s  hooks: {list(LAP_HOOKS.keys())}')

    startupinfo = subprocess.STARTUPINFO()
    startupinfo.dwFlags    |= subprocess.STARTF_USESHOWWINDOW
    startupinfo.wShowWindow = SW_SHOWMINNOACTIVE

    env = {k: v for k, v in os.environ.items() if k != 'MASHED_RE_NO_AUTO_HOOK'}
    proc = subprocess.Popen(
        [str(MASHED_EXE)], cwd=str(MASHED_EXE.parent),
        stdin=subprocess.DEVNULL, stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL,
        env=env, startupinfo=startupinfo, creationflags=0x00000200 | 0x00000008)
    pid = proc.pid
    log(f'spawned MASHED.exe  pid={pid}')

    hwnd = None
    deadline = time.time() + 8
    while time.time() < deadline:
        hwnd = find_main_window(pid)
        if hwnd:
            user32.ShowWindow(hwnd, SW_SHOWMINNOACTIVE)
            log(f'  hwnd=0x{hwnd:08x} minimized')
            break
        time.sleep(0.3)

    if not args.no_mute:
        muted = False
        deadline = time.time() + 5
        while time.time() < deadline:
            if mute_pid(pid):
                log('  muted audio')
                muted = True
                break
            time.sleep(0.2)
        if not muted:
            log('  audio session not found within 5s')

    log(f'  waiting {args.boot_wait}s for main menu')
    time.sleep(args.boot_wait)
    if not psutil.pid_exists(pid):
        sys.exit('ABORT: process died during boot — game did not reach main menu')

    device = frida.get_local_device()
    try:
        session = device.attach(pid)
    except Exception as e:
        proc.kill()
        sys.exit(f'attach failed: {e}')

    asi_escaped = str(ASI_PATH).replace('\\', '\\\\')
    base_rvas = [{'name': n, 'rva': r} for n, r in LAP_HOOKS.items()]
    js = (AGENT_TEMPLATE
          .replace('REPLACEME_ASI', json.dumps(asi_escaped))
          .replace('REPLACEME_HOOKS', json.dumps(LAP_HOOKS))
          .replace('REPLACEME_RVAS', json.dumps(base_rvas)))

    state = {'ready': False, 'pre': None, 'post': None, 'asi_base': None, 'errors': []}

    def on_message(msg, data):
        if msg['type'] == 'error':
            log(f"  AGENT ERROR: {msg.get('description')}")
            state['errors'].append(msg)
            return
        p = msg.get('payload', {})
        t = p.get('type')
        if t == 'pre_snapshot':
            state['pre'] = p.get('data')
            log('  [agent] pre_snapshot')
        elif t == 'asi_loaded':
            state['asi_base'] = p.get('base')
            log(f"  [agent] asi_loaded base={state['asi_base']}")
        elif t == 'post_snapshot':
            state['post'] = p.get('data')
            log('  [agent] post_snapshot')
        elif t == 'ready':
            state['ready'] = True
            log('  [agent] ready (interceptors armed)')
        elif t == 'attach_fail':
            log(f"  [agent] ATTACH FAIL {p.get('name')}: {p.get('err')}")

    script = session.create_script(js)
    script.on('message', on_message)
    script.load()

    deadline = time.time() + 15
    while not state['ready'] and time.time() < deadline:
        time.sleep(0.1)
    if not state['ready']:
        log('ABORT: agent never ready')
        proc.kill()
        sys.exit(1)
    if state['errors']:
        log(f'ABORT: agent errors during setup: {state["errors"]}')
        proc.kill()
        sys.exit(1)

    log(f'\n  asi base={state["asi_base"]}; idling {args.observe_seconds}s with Interceptor on lap hooks')
    hdr = f'  {"t":>4s}  {"LapLapsGetBySlot":>18s}  {"LapSecsGetBySlot":>18s}  {"LapFracGetBySlot":>18s}  {"LapTimeALessThanB":>19s}  {"alive":>7s}'
    log(hdr)
    start = time.time()
    last = {k: 0 for k in LAP_HOOKS}
    alive_throughout = True
    while time.time() - start < args.observe_seconds:
        elapsed = int(time.time() - start)
        alive = psutil.pid_exists(pid)
        if not alive:
            alive_throughout = False
        try:
            snap = script.exports_sync.snapshot()
            last = snap
        except Exception as e:
            log(f'  snapshot err: {e}')
            snap = last
        log(f'  {elapsed:>4d}  {snap.get("LapLapsGetBySlot", 0):>18d}  '
            f'{snap.get("LapSecsGetBySlot", 0):>18d}  '
            f'{snap.get("LapFracGetBySlot", 0):>18d}  '
            f'{snap.get("LapTimeALessThanB", 0):>19d}  '
            f'{str(alive):>7s}')
        if not alive:
            break
        time.sleep(2)

    try:
        final = script.exports_sync.snapshot()
    except Exception:
        final = last

    log('\n=== FINAL COUNTS ===')
    all_zero = True
    for name in LAP_HOOKS:
        v = final.get(name, 0)
        if v > 0:
            all_zero = False
        log(f'  {name:<22s}  {v}')
    log(f'  process_alive_throughout: {alive_throughout}')
    log(f'  all_zero: {all_zero}')

    if all_zero:
        log('\nVERDICT: 0 invocations for all 4 hooks')
        log('results screen not reached during main_menu_idle_10s.')
        log('C4 REFUSED: rebatch with main_menu_navigate_all to cycle through all screens.')
    else:
        log('\nVERDICT: at least one hook fired — see counts above.')

    try:
        session.detach()
    except Exception:
        pass
    try:
        proc.kill()
    except Exception:
        pass
    try:
        proc.wait(timeout=3)
    except Exception:
        pass

    OUT_FILE.write_text('\n'.join(lines), encoding='utf-8')
    print(f'report written to {OUT_FILE}')


if __name__ == '__main__':
    main()
