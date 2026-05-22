# Generic Frida hook-installer verification harness.
#
# Usage:  py -3.12 re/frida/run_verify_hook.py <hook_name>
#   e.g.: py -3.12 re/frida/run_verify_hook.py vec3_magnitude
#   e.g.: py -3.12 re/frida/run_verify_hook.py fast_sqrt
#
# Spawns MASHED.exe with auto-hook ENABLED so the .asi installs the patch.
# Verifies bytes at target_rva became E9 rel32 -> reimpl_addr, that calls
# to the patched RVA route through our reimpl (via Interceptor), and that
# all calls return without exception.
import json
import os
import subprocess
import sys
import time
from pathlib import Path

import frida

ROOT = Path(__file__).resolve().parent.parent.parent

sys.path.insert(0, str(ROOT / 're' / 'frida'))
from hooks_registry import HOOKS


def _find_original(script_root: Path) -> Path:
    """Locate original/MASHED.exe: handles both main-repo and worktree invocations."""
    candidate = script_root / 'original' / 'MASHED.exe'
    if candidate.exists():
        return candidate
    # Walk up: worktree is at <main>/.worktrees/<name>/
    parent = script_root.parent.parent
    candidate2 = parent / 'original' / 'MASHED.exe'
    if candidate2.exists():
        return candidate2
    return candidate  # let the later check produce a clear error


MASHED_EXE = _find_original(ROOT)
ASI_PATH   = ROOT / 'mashedmod' / 'build' / 'mashed_re_dev.asi'
AGENT_JS   = ROOT / 're' / 'frida' / 'verify_hook_install_template.js'
LOG_DIR    = ROOT / 'log'

state = {'pre_snapshot': None, 'asi_loaded': None, 'post_snapshot': None,
         'results': None, 'errors': [], 'done': False}


def on_message(message, data):
    if message['type'] == 'error':
        print('AGENT ERROR:', message.get('description'), message.get('stack'))
        state['errors'].append(message); state['done'] = True; return
    payload = message.get('payload', {})
    kind = payload.get('type')
    if kind == 'error':
        print(f"  [agent] ERROR: {payload['msg']}")
        state['errors'].append(payload); state['done'] = True
    elif kind in state:
        state[kind] = payload
        print(f"  [agent] {kind}")
        if kind == 'results': state['done'] = True
    else:
        print('  [agent]', payload)


def main():
    if len(sys.argv) < 2:
        sys.exit(f"usage: {sys.argv[0]} <hook_name>\n  registered: {', '.join(HOOKS.keys())}")
    name = sys.argv[1]
    if name not in HOOKS:
        sys.exit(f"unknown hook {name!r}; registered: {', '.join(HOOKS.keys())}")

    hook = HOOKS[name]
    config = {
        'asi_path':       str(ASI_PATH).replace('\\', '\\\\'),
        'target_rva':     f"0x{hook['rva']:08x}",
        'export':         hook['export'],
        'signature':      hook['signature'],
        'arg_type':       hook['arg_type'],
        'lut_root_delta': hook['lut_root_delta'],
        'tests':          hook['path2_tests'],
    }

    LOG_DIR.mkdir(parents=True, exist_ok=True)
    report_txt = LOG_DIR / f'verify_hook_install_{name}.txt'

    if not MASHED_EXE.exists():
        sys.exit(f"MASHED.exe not found at {MASHED_EXE}")
    if not ASI_PATH.exists():
        sys.exit(f"build artifact not found at {ASI_PATH}")

    print(f"hook: {name}  rva={config['target_rva']}  export={config['export']}")
    _shim = MASHED_EXE.parent / 'd3d9.dll'
    if not _shim.exists():
        sys.exit(f"FATAL: {_shim} missing (d3d9 windowed shim). "
                 f"Run `mashedmod\\build_d3d9_shim.bat`, then retry. "
                 f"Refusing to spawn MASHED without the shim — it would go fullscreen.")
    print(f"spawning {MASHED_EXE} via subprocess (auto-hook ENABLED)")
    # Registry AppCompat shim includes RUNASINVOKER → no elevation needed.
    env = {k: v for k, v in os.environ.items() if k != 'MASHED_RE_NO_AUTO_HOOK'}
    proc = subprocess.Popen([str(MASHED_EXE)], cwd=str(MASHED_EXE.parent), env=env)
    print(f"  pid = {proc.pid}")

    time.sleep(0.2)
    device = frida.get_local_device()
    try:
        session = device.attach(proc.pid)
    except Exception as e:
        print(f"attach failed: {e}")
        try: proc.kill()
        except Exception: pass
        return 4

    script_text = AGENT_JS.read_text(encoding='utf-8').replace('$CONFIG$', json.dumps(config))
    script = session.create_script(script_text)
    script.on('message', on_message)
    script.load()

    deadline = time.time() + 60
    while not state['done'] and time.time() < deadline:
        time.sleep(0.1)

    try: session.detach()
    except Exception: pass
    try: proc.kill()
    except Exception: pass
    try: proc.wait(timeout=3)
    except Exception: pass

    return write_report(name, config, report_txt)


def write_report(name, config, out_path):
    pre, asi, post, res = state['pre_snapshot'], state['asi_loaded'], state['post_snapshot'], state['results']
    pass_flags, fail_flags, lines = [], [], []
    lines.append(f"=== verify_hook_install report ({name}) ===")
    lines.append('')
    lines.append(f"target address: {config['target_rva']}  export: {config['export']}")
    if pre: lines.append(f"PRE  bytes:     {pre['bytes']}")
    else:   lines.append('PRE  bytes:     (not captured)'); fail_flags.append('pre_snapshot missing')
    if asi:
        lines.append(f"asi base:       {asi['module_base']}")
        lines.append(f"reimpl addr:    {asi['reimpl_addr']}")
    else:
        lines.append('asi base:       (not captured)'); fail_flags.append('asi did not load')
    if post:
        lines.append(f"POST bytes:     {post['bytes']}")
        lines.append(f"opcode:         {post['opcode_hex']}  expected 0xe9  -> {'OK' if post['opcode_ok'] else 'FAIL'}")
        lines.append(f"rel32:          {post['rel32_hex']}  expected {post['expected_rel32_hex']}  -> {'OK' if post['rel32_ok'] else 'FAIL'}")
        lines.append(f"bytes changed:  {post['bytes_changed']}")
        if post['opcode_ok']: pass_flags.append('opcode == 0xE9')
        else:                 fail_flags.append(f"opcode {post['opcode_hex']} != 0xE9")
        if post['rel32_ok']:  pass_flags.append('rel32 matches reimpl - target - 5')
        else:                 fail_flags.append('rel32 mismatch')
        if post['bytes_changed']: pass_flags.append('first 8 bytes mutated by patcher')
        else:                     fail_flags.append('bytes unchanged after Module.load')
    else:
        fail_flags.append('post_snapshot missing')
    if res:
        lines.append('')
        lines.append(f"interceptor entries (reimpl):  {res['reimpl_calls_observed']} / {res['reimpl_calls_expected']}")
        if res['reimpl_calls_observed'] == res['reimpl_calls_expected']:
            pass_flags.append(f"reimpl interceptor fired {res['reimpl_calls_observed']}x as expected")
        else:
            fail_flags.append(f"reimpl interceptor fired {res['reimpl_calls_observed']} times, expected {res['reimpl_calls_expected']}")
        lines.append('')
        lines.append('per-input results:')
        for c in res['cases']:
            # For void (arg_type=none) functions, 'got' may be absent from the
            # JSON payload (JS undefined is not serialised). Treat absent or null
            # 'got' as OK provided there is no error.
            is_void = 'got' not in c or c['got'] is None
            ok = (is_void or c['got'] is not None) and c['err'] is None
            tag = 'OK' if ok else 'FAIL'
            got_display = c.get('got', '(void)')
            lines.append(f"  [{tag}] input={c['input']}  got={got_display}  err={c['err'] or ''}")
            if not ok: fail_flags.append(f"call {c['idx']} (input={c['input']}) failed: {c['err']}")
        if all(c['err'] is None for c in res['cases']):
            pass_flags.append(f"all {len(res['cases'])} calls returned without exception")
    else:
        fail_flags.append('no results captured')

    lines.append('')
    lines.append('PASS:')
    for p in pass_flags: lines.append(f"  + {p}")
    if not pass_flags: lines.append('  (none)')
    lines.append('FAIL:')
    for f in fail_flags: lines.append(f"  - {f}")
    if not fail_flags: lines.append('  (none)')

    report = '\n'.join(lines)
    out_path.write_text(report, encoding='utf-8')
    print()
    print(report)
    print()
    print(f"report written to {out_path}")
    return 0 if not fail_flags else 1


if __name__ == '__main__':
    sys.exit(main())
