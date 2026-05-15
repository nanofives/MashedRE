# Canonical-scenario observation for c4-batch-a s1 — render-math hooks.
# Loads mashed_re_dev.asi (auto-hook ENABLED), verifies E9+rel32 at 4 RVAs,
# idles at main-menu for 10s confirming process aliveness.
# NO Interceptor — safe for hot-path functions.
#
# Per CLAUDE.md: "For hot-path verification, use behavioral observation:
# Module.load the .asi, watch the game for N seconds with no Interceptor,
# confirm no visual/crash regression."
#
# Usage:
#   py -3.12 re/frida/observe_c4_render_math_hooks.py [--seconds 10]
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

try:
    from pycaw.pycaw import AudioUtilities
    HAS_PYCAW = True
except ImportError:
    HAS_PYCAW = False

user32 = ctypes.windll.user32
user32.PostMessageW.argtypes = [wintypes.HWND, wintypes.UINT, wintypes.WPARAM, wintypes.LPARAM]
user32.PostMessageW.restype  = wintypes.BOOL

ROOT       = Path(__file__).resolve().parent.parent.parent
MASHED_EXE = ROOT / 'original' / 'MASHED.exe'
ASI_PATH   = ROOT / 'mashedmod' / 'build' / 'mashed_re_dev.asi'
AGENT_JS   = ROOT / 're' / 'frida' / 'observe_hooks_template.js'

HOOKS = [
    {'name': 'RwV3dTransformPoint',  'rva': '0x004c3730', 'export': 'RwV3dTransformPoint'},
    {'name': 'RwV3dTransformVector', 'rva': '0x004c3880', 'export': 'RwV3dTransformVector'},
    {'name': 'Vec2Length',           'rva': '0x004c3bf0', 'export': 'Vec2Length'},
    {'name': 'Vec2Normalize',        'rva': '0x004c3c60', 'export': 'Vec2Normalize'},
]

state = {'pre_snapshot': None, 'post_snapshot': None, 'errors': [], 'done': False}


def on_message(message, data):
    if message['type'] == 'error':
        print('AGENT ERROR:', message.get('description'), message.get('stack', ''))
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
    if not HAS_PYCAW:
        return False
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
    parser.add_argument('--seconds', type=int, default=10)
    parser.add_argument('--no-mute', action='store_true')
    args = parser.parse_args()

    if not MASHED_EXE.exists():
        sys.exit(f"MASHED.exe not found: {MASHED_EXE}")
    if not ASI_PATH.exists():
        sys.exit(f"ASI not found: {ASI_PATH}")
    if not AGENT_JS.exists():
        sys.exit(f"Agent JS not found: {AGENT_JS}")

    print("=== observe_c4_render_math_hooks — main_menu_idle_10s ===")
    print(f"ASI:   {ASI_PATH}")
    print(f"EXE:   {MASHED_EXE}")
    print()

    agent_src = AGENT_JS.read_text(encoding='utf-8')
    config = {
        'hooks':    HOOKS,
        'asi_path': str(ASI_PATH).replace('\\', '\\\\'),
    }
    agent_src = agent_src.replace('$CONFIG$', json.dumps(config))

    # Spawn with auto-hook ENABLED (no MASHED_RE_NO_AUTO_HOOK in env)
    env = {k: v for k, v in os.environ.items() if k != 'MASHED_RE_NO_AUTO_HOOK'}

    # Clear mashed.log if not locked
    log_path = ROOT / 'original' / 'mashed.log'
    try:
        if log_path.exists():
            log_path.unlink()
    except Exception:
        pass  # log locked by prior run — ignore

    proc = subprocess.Popen(
        [str(MASHED_EXE)], cwd=str(MASHED_EXE.parent), env=env,
        stdin=subprocess.DEVNULL, stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL,
        creationflags=0x00000200 | 0x00000008)
    pid = proc.pid
    print(f"spawned MASHED.exe  pid={pid}  (auto-hook ENABLED)")

    if not args.no_mute:
        muted = False
        deadline_mute = time.time() + 8
        while time.time() < deadline_mute:
            if mute_pid_audio(pid):
                print(f"  muted audio session")
                muted = True
                break
            time.sleep(0.2)
        if not muted:
            print("  could not mute audio (pycaw unavailable or session not found)")

    # Wait for game to boot before attaching
    BOOT_WAIT = 8.0
    print(f"  waiting {BOOT_WAIT}s for game boot...")
    t0 = time.time()
    while time.time() - t0 < BOOT_WAIT:
        if not psutil.pid_exists(pid):
            print("  process died during boot wait — FAIL")
            sys.exit(6)
        time.sleep(0.5)

    device = frida.get_local_device()
    try:
        session = device.attach(pid)
    except Exception as e:
        print(f"Frida attach failed: {e}")
        try: proc.kill()
        except Exception: pass
        sys.exit(3)

    script = session.create_script(agent_src)
    script.on('message', on_message)
    script.load()

    deadline_done = time.time() + 15
    while not state['done'] and time.time() < deadline_done:
        time.sleep(0.1)

    if state['errors'] or state['post_snapshot'] is None:
        print("  agent failed to produce post_snapshot")
        try: proc.kill()
        except Exception: pass
        sys.exit(7)

    # Parse results
    ps = state['post_snapshot']
    print()
    print("=== observe_hooks_at_menu report ===")
    print()
    print(f"observe seconds: {args.seconds}")
    print(f"boot wait:       {BOOT_WAIT}")
    print()
    print(f"asi module base: {ps.get('module_base', '?')}  size: {ps.get('module_size', '?')}")
    print()
    print("per-hook install verification:")
    all_pass = True
    for h in ps.get('hooks', []):
        ok = h['opcode_ok'] and h['rel32_ok']
        tag = "[OK]" if ok else "[FAIL]"
        if not ok:
            all_pass = False
        print(f"  {tag} {h['name']:<22s} rva={h['rva']}  reimpl={h['reimpl_addr']}")
        print(f"         PRE  bytes:  {h['pre_bytes']}")
        print(f"         POST bytes:  {h['post_bytes']}")
        print(f"         opcode:      {h['opcode_hex']}  expected 0xe9  -> {'OK' if h['opcode_ok'] else 'FAIL'}")
        print(f"         rel32:       {h['rel32_hex']}  expected {h['expected_rel32_hex']}  -> {'OK' if h['rel32_ok'] else 'FAIL'}")

    print()

    # Idle observation - NO Interceptor
    print(f"  observing for {args.seconds}s (no Interceptor, alive-check only)...")
    obs_start = time.time()
    alive_throughout = True
    while time.time() - obs_start < args.seconds:
        if not psutil.pid_exists(pid):
            print(f"  process died at t+{int(time.time()-obs_start)}s — FAIL")
            alive_throughout = False
            break
        elapsed = int(time.time() - obs_start)
        print(f"  t={elapsed:>3d}s  process alive  pid={pid}")
        time.sleep(2)

    final_alive = psutil.pid_exists(pid)

    # Final byte poll
    print()
    print("final byte state (RPC poll after observation window):")
    final_hooks_ok = []
    try:
        final_snap = script.exports_sync.poll()
        for h in HOOKS:
            name = h['name']
            rva  = h['rva']
            bs   = final_snap.get(name, {})
            opcode = bs.get('opcode', '?')
            bytes_hex = bs.get('bytes', '?')
            ok = opcode == '0xe9'
            final_hooks_ok.append(ok)
            tag = "[OK]" if ok else "[FAIL]"
            print(f"  {tag} {name:<22s} rva={rva}  opcode={opcode}  bytes={bytes_hex}")
    except Exception as e:
        print(f"  final byte poll failed: {e}")

    try: session.detach()
    except Exception: pass
    try: proc.kill()
    except Exception: pass
    try: proc.wait(timeout=3)
    except Exception: pass

    print()
    print(f"alive throughout observation: {alive_throughout}")
    print(f"alive at end of observation:  {final_alive}")

    print()
    pass_items = []
    fail_items = []
    for h in ps.get('hooks', []):
        name = h['name']
        if h['opcode_ok'] and h['rel32_ok']:
            pass_items.append(f"{name} installed cleanly (E9 + correct rel32)")
        else:
            fail_items.append(f"{name} E9/rel32 FAIL")
    if alive_throughout and final_alive:
        pass_items.append(f"process alive throughout {args.seconds}s observation window")
    else:
        fail_items.append("process died during observation")

    print("PASS:")
    for p in pass_items:
        print(f"  + {p}")
    print("FAIL:")
    for f in fail_items:
        print(f"  - {f}")
    if not fail_items:
        print("  (none)")

    overall = "PASS" if (all_pass and alive_throughout and final_alive) else "FAIL"
    print()
    print(f"OVERALL: {overall}")
    return 0 if overall == "PASS" else 1


if __name__ == '__main__':
    sys.exit(main())
