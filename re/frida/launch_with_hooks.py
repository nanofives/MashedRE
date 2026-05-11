# Launch MASHED.exe with our .asi injected via Frida (bypassing
# Ultimate-ASI-Loader, which is incompatible with subprocess.Popen launches
# on this machine). Used to test live boot with hooks active.
#
# Workflow:
#   1. Park UAL (no d3d9.dll proxy) and .asi (no UAL pickup needed).
#   2. Spawn MASHED via subprocess.
#   3. Attach Frida, Module.load() our .asi — DllMain runs InjectHooks().
#   4. Detach Frida (hooks stay installed, they're inline byte patches).
#   5. Watch MASHED run for `watch_seconds` (default 30).
#   6. Report log progress.
import argparse
import os
import subprocess
import sys
import time
from pathlib import Path

import frida

ROOT       = Path(__file__).resolve().parent.parent.parent
MASHED_EXE = ROOT / 'original' / 'MASHED.exe'
ASI_PATH   = ROOT / 'mashedmod' / 'build' / 'mashed_re_dev.asi'
LOG_FILE   = ROOT / 'original' / 'mashed.log'

INJECT_JS = '''
'use strict';
const ASI_PATH = '%ASI%';
const LUT_BASE_ADDR   = ptr('0x007d3ff8');
const LUT_OFFSET_ADDR = ptr('0x007d3ffc');

function lutReady() {
    try {
        const base = LUT_BASE_ADDR.readU32();
        if (base === 0) return false;
        const off = LUT_OFFSET_ADDR.readU32();
        return ptr(base + off).readU32() !== 0;
    } catch (e) { return false; }
}

function poll(triesLeft) {
    if (lutReady()) {
        try {
            Module.load(ASI_PATH);
            const m = Process.findModuleByName('mashed_re_dev.asi');
            send({ kind: 'loaded', base: m ? m.base.toString() : null,
                   attempts: 150 - triesLeft });
        } catch (e) {
            send({ kind: 'load_error', msg: e.message });
        }
        return;
    }
    if (triesLeft <= 0) {
        send({ kind: 'lut_timeout' });
        return;
    }
    setTimeout(function () { poll(triesLeft - 1); }, 200);
}
poll(150);
'''


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('--watch-seconds', type=int, default=30)
    parser.add_argument('--no-hook', action='store_true', help='Skip hook installation (loads .asi but DllMain returns without installing).')
    args = parser.parse_args()

    if not MASHED_EXE.exists(): sys.exit(f"missing {MASHED_EXE}")
    if not ASI_PATH.exists():   sys.exit(f"missing {ASI_PATH}")

    # Sanity: warn if UAL is staged as d3d9.dll — that conflicts with us.
    ual = MASHED_EXE.parent / 'd3d9.dll'
    if ual.exists():
        print(f"WARNING: {ual} present — that's Ultimate-ASI-Loader. We don't")
        print("         need it (Frida injects directly), and it crashes Popen")
        print("         launches on this machine. Consider parking it.")

    if LOG_FILE.exists(): LOG_FILE.unlink()

    print(f"spawning {MASHED_EXE}")
    env = {**os.environ}
    if args.no_hook:
        env['MASHED_RE_NO_AUTO_HOOK'] = '1'
        print("  MASHED_RE_NO_AUTO_HOOK=1 — .asi loads but hooks DO NOT install")
    # Detach stdio to mimic Explorer/ShellExecute launch (which is the only
    # launch shape that survives long-term on this machine). Python inherits
    # console handles by default → MASHED behaves oddly under that.
    CREATE_NEW_PROCESS_GROUP = 0x00000200
    DETACHED_PROCESS         = 0x00000008
    proc = subprocess.Popen(
        [str(MASHED_EXE)], cwd=str(MASHED_EXE.parent), env=env,
        stdin=subprocess.DEVNULL, stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL,
        creationflags=CREATE_NEW_PROCESS_GROUP | DETACHED_PROCESS)
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

    state = {'loaded': False, 'error': None}
    def on_message(message, data):
        if message['type'] == 'error':
            print(f"  agent error: {message.get('description')}")
            state['error'] = message; return
        p = message.get('payload', {})
        if p.get('kind') == 'loaded':
            print(f"  .asi loaded at {p['base']}  (LUT-ready after {p['attempts']} attempts)")
            state['loaded'] = True
        elif p.get('kind') == 'load_error':
            print(f"  Module.load failed: {p['msg']}")
            state['error'] = p
        elif p.get('kind') == 'lut_timeout':
            print("  LUT never populated — .asi never loaded")
            state['error'] = p

    asi_path_escaped = str(ASI_PATH).replace('\\', '\\\\')
    script_text = INJECT_JS.replace('%ASI%', asi_path_escaped)
    script = session.create_script(script_text)
    script.on('message', on_message)
    script.load()

    # Polling the LUT takes ~10s in the worst case (game still booting).
    deadline = time.time() + 35
    while not state['loaded'] and state['error'] is None and time.time() < deadline:
        time.sleep(0.1)

    if state['error']:
        try: proc.kill()
        except Exception: pass
        return 1

    print(f"  detaching Frida; watching MASHED for {args.watch_seconds}s")
    try: session.detach()
    except Exception: pass

    deadline = time.time() + args.watch_seconds
    while time.time() < deadline:
        ec = proc.poll()
        if ec is not None:
            print(f"  process exited (code {ec}, 0x{ec & 0xffffffff:08x}) at t+{int(time.time() - (deadline - args.watch_seconds))}s")
            break
        time.sleep(0.5)
    else:
        print(f"  still running at {args.watch_seconds}s — killing")
        try: proc.kill()
        except Exception: pass

    try: proc.wait(timeout=3)
    except Exception: pass

    if LOG_FILE.exists():
        log_size = LOG_FILE.stat().st_size
        print(f"\n=== mashed.log ({log_size} bytes), last 40 informative lines ===")
        with LOG_FILE.open(encoding='utf-8', errors='replace') as f:
            lines = f.readlines()
        interesting = [ln.rstrip() for ln in lines if any(k in ln for k in
            ('RwEngine','Calling','PIZ','Opening piz','Closing piz','Reading texture',
             'Searching piz','Loading','FAILED','BYPASS','compat','Number of subsystems','OK'))]
        for ln in interesting[-40:]: print(f"  {ln}")
    else:
        print("\nNO mashed.log written")

    return 0


if __name__ == '__main__':
    sys.exit(main())
