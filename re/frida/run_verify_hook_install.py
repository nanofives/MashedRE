# Frida hook-installer verification harness.
#
# Spawns MASHED.exe WITHOUT MASHED_RE_NO_AUTO_HOOK so our .asi installs the
# inline-JMP patch through HookSystem::InstallAll() during DllMain. Then it
# verifies (a) the 5 bytes at 0x004c3ac0 became `E9 <rel32>` pointing to our
# reimpl, (b) calls to 0x004c3ac0 now route through our function (interceptor
# count matches), and (c) outputs are correct.
import os
import struct
import subprocess
import sys
import time
from pathlib import Path

import frida

ROOT       = Path(__file__).resolve().parent.parent.parent
MASHED_EXE = ROOT / 'original' / 'MASHED.exe'
AGENT_JS   = ROOT / 're' / 'frida' / 'verify_hook_install_vec3.js'
LOG_DIR    = ROOT / 'log'
REPORT_TXT = LOG_DIR / 'verify_hook_install_vec3.txt'

state = {
    'pre_snapshot':  None,
    'asi_loaded':    None,
    'post_snapshot': None,
    'results':       None,
    'errors':        [],
    'done':          False,
}


def on_message(message, data):
    if message['type'] == 'error':
        print('AGENT ERROR:', message.get('description'), message.get('stack'))
        state['errors'].append(message)
        state['done'] = True
        return
    payload = message.get('payload', {})
    kind = payload.get('type')
    if kind == 'error':
        print(f"  [agent] ERROR: {payload['msg']}")
        state['errors'].append(payload)
        state['done'] = True
    elif kind in state:
        state[kind] = payload
        print(f"  [agent] {kind}")
        if kind == 'results':
            state['done'] = True
    else:
        print('  [agent]', payload)


def main():
    LOG_DIR.mkdir(parents=True, exist_ok=True)
    if not MASHED_EXE.exists():
        sys.exit(f"MASHED.exe not found at {MASHED_EXE}")
    if not AGENT_JS.exists():
        sys.exit(f"agent script not found at {AGENT_JS}")

    print(f"spawning {MASHED_EXE} via subprocess (auto-hook ENABLED)")
    env = {k: v for k, v in os.environ.items() if k != 'MASHED_RE_NO_AUTO_HOOK'}
    proc = subprocess.Popen([str(MASHED_EXE)], cwd=str(MASHED_EXE.parent), env=env)
    pid = proc.pid
    print(f"  pid = {pid}")

    time.sleep(0.2)
    device = frida.get_local_device()
    try:
        session = device.attach(pid)
    except Exception as e:
        print(f"attach failed: {e}")
        try: proc.kill()
        except Exception: pass
        return 4
    script = session.create_script(AGENT_JS.read_text(encoding='utf-8'))
    script.on('message', on_message)
    script.load()

    timeout_s = 30
    deadline = time.time() + timeout_s
    while not state['done'] and time.time() < deadline:
        time.sleep(0.1)

    try: session.detach()
    except Exception: pass
    try: proc.kill()
    except Exception: pass
    try: proc.wait(timeout=3)
    except Exception: pass

    return write_report()


def write_report():
    pre   = state['pre_snapshot']
    asi   = state['asi_loaded']
    post  = state['post_snapshot']
    res   = state['results']

    pass_flags = []
    fail_flags = []

    lines = []
    lines.append('=== verify_hook_install_vec3 report ===')
    lines.append('')
    lines.append(f"target address: 0x004c3ac0")
    if pre is not None:
        lines.append(f"PRE  bytes:     {pre['bytes']}")
    else:
        lines.append("PRE  bytes:     (not captured)")
        fail_flags.append('pre_snapshot missing')

    if asi is not None:
        lines.append(f"asi base:       {asi['module_base']}")
        lines.append(f"reimpl addr:    {asi['reimpl_addr']}")
    else:
        lines.append("asi base:       (not captured)")
        fail_flags.append('asi did not load')

    if post is not None:
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
        lines.append("POST bytes:     (not captured)")
        fail_flags.append('post_snapshot missing')

    if res is not None:
        lines.append('')
        lines.append(f"interceptor entries (reimpl):  {res['reimpl_calls_observed']} / {res['reimpl_calls_expected']}")
        if res['reimpl_calls_observed'] == res['reimpl_calls_expected']:
            pass_flags.append(f"reimpl interceptor fired {res['reimpl_calls_observed']}x as expected")
        else:
            fail_flags.append(f"reimpl interceptor fired {res['reimpl_calls_observed']} times, expected {res['reimpl_calls_expected']}")
        lines.append('')
        lines.append('per-vector results:')
        all_match = True
        for c in res['cases']:
            v = c['input']
            ok = c['match']
            err = c['err'] or ''
            tag = 'OK' if ok else 'FAIL'
            if not ok: all_match = False
            lines.append(f"  [{tag}] v=({v[0]}, {v[1]}, {v[2]})  expected={c['expected']}  got={c['got']}  err={err}")
        if all_match: pass_flags.append('all 5 magnitudes match expected values')
        else:         fail_flags.append('one or more magnitude mismatches')
    else:
        fail_flags.append('no results captured (agent likely crashed)')

    lines.append('')
    lines.append('PASS:')
    for p in pass_flags: lines.append(f"  + {p}")
    if not pass_flags:   lines.append('  (none)')
    lines.append('FAIL:')
    for f in fail_flags: lines.append(f"  - {f}")
    if not fail_flags:   lines.append('  (none)')

    report = '\n'.join(lines)
    REPORT_TXT.write_text(report, encoding='utf-8')
    print()
    print(report)
    print()
    print(f"report written to {REPORT_TXT}")

    return 0 if not fail_flags else 1


if __name__ == '__main__':
    sys.exit(main())
