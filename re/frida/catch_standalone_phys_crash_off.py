# Same as catch_standalone_phys_crash.py but with MASHED_REAL_PHYSICS UNSET
# (scaffold baseline) — to confirm whether the captured crash is physics-specific.
import json, os, sys, time
from pathlib import Path
import frida
import importlib.util

ROOT = Path(__file__).resolve().parent.parent.parent
EXE  = ROOT / 'mashedmod' / 'build' / 'mashed_re.exe'
OUT  = ROOT / 'log' / 'standalone_phys_crash_OFF.json'

# reuse AGENT from the ON harness
spec = importlib.util.spec_from_file_location('onh', str(Path(__file__).parent / 'catch_standalone_phys_crash.py'))
onh = importlib.util.module_from_spec(spec)
spec.loader.exec_module(onh)
AGENT = onh.AGENT

def main():
    OUT.parent.mkdir(parents=True, exist_ok=True)
    env = dict(os.environ)
    env.pop('MASHED_REAL_PHYSICS', None)
    env.update({'MASHED_RACE_DEMO':'1','MASHED_PLAY_DEMO':'1',
                'MASHED_GOTO':'6','MASHED_TRACK_SEL':'0','MASHED_CAR_SEL':'0'})
    dev = frida.get_local_device()
    pid = dev.spawn([str(EXE)], cwd=str(EXE.parent), env=env)
    print('spawned pid', pid)
    session = dev.attach(pid)
    cap = {'v': None}
    def on_msg(m, d):
        if m['type'] == 'error': print('err', m.get('description')); return
        p = m.get('payload', {})
        if p.get('kind') == 'ready': print('armed base', p.get('mashed_base'))
        elif p.get('kind') == 'crash':
            print('=== CRASH (OFF) ===', p.get('type'), p.get('address'),
                  'rva', p.get('eip_rva'), 'mem_op', p.get('mem_op'), 'eax', p.get('eax'))
            cap['v'] = p
    s = session.create_script(AGENT); s.on('message', on_msg); s.load()
    dev.resume(pid)
    deadline = time.time() + 30
    while time.time() < deadline and cap['v'] is None:
        time.sleep(0.2)
    try: session.detach()
    except Exception: pass
    try: dev.kill(pid)
    except Exception: pass
    if cap['v']:
        OUT.write_text(json.dumps(cap['v'], indent=2), encoding='utf-8')
        print('written', OUT); return 0
    print('no crash in 30s (OFF clean)'); return 1

if __name__ == '__main__':
    sys.exit(main())
