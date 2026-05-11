# Frida hook-installer verification harness for FastSqrt.
import os
import subprocess
import sys
import time
from pathlib import Path

import frida

ROOT       = Path(__file__).resolve().parent.parent.parent
MASHED_EXE = ROOT / 'original' / 'MASHED.exe'
AGENT_JS   = ROOT / 're' / 'frida' / 'verify_hook_install_sqrt.js'
LOG_DIR    = ROOT / 'log'
REPORT_TXT = LOG_DIR / 'verify_hook_install_sqrt.txt'

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
    LOG_DIR.mkdir(parents=True, exist_ok=True)
    if not MASHED_EXE.exists():
        sys.exit(f"MASHED.exe not found at {MASHED_EXE}")

    print(f"spawning {MASHED_EXE} via subprocess (auto-hook ENABLED)")
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
    script = session.create_script(AGENT_JS.read_text(encoding='utf-8'))
    script.on('message', on_message)
    script.load()

    deadline = time.time() + 30
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
    pre, asi, post, res = state['pre_snapshot'], state['asi_loaded'], state['post_snapshot'], state['results']
    pass_flags, fail_flags, lines = [], [], []
    lines.append('=== verify_hook_install_sqrt report ===')
    lines.append('')
    lines.append('target address: 0x004c3b30')
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
            ok = c['got'] is not None and c['err'] is None
            tag = 'OK' if ok else 'FAIL'
            lines.append(f"  [{tag}] input={c['input']}  got={c['got']}  err={c['err'] or ''}")
            if not ok: fail_flags.append(f"call {c['idx']} (input={c['input']}) failed: {c['err']}")
        if all(c['got'] is not None and c['err'] is None for c in res['cases']):
            pass_flags.append('all 5 calls returned without exception')
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
    REPORT_TXT.write_text(report, encoding='utf-8')
    print()
    print(report)
    print()
    print(f"report written to {REPORT_TXT}")
    return 0 if not fail_flags else 1


if __name__ == '__main__':
    sys.exit(main())
