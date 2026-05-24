"""Canonical observation of refused-but-now-enabled hooks: attach to running
MASHED, install Interceptor on each RVA, count fires during boot+menu window.

Fire+survive = C3-equivalent canonical-observation evidence (loader is now
working, so unlike pre-2026-05-24 these counts are meaningful).
"""
from __future__ import annotations
import subprocess, sys, time
from pathlib import Path
ROOT = Path(__file__).resolve().parent.parent.parent
import frida, psutil

try:
    from pycaw.pycaw import AudioUtilities
    HAS_PYCAW = True
except ImportError:
    HAS_PYCAW = False

ORIGINAL_DIR = ROOT / 'original'
MASHED_EXE   = ORIGINAL_DIR / 'MASHED.exe'

def mashed_alive(pid):
    try:
        p = psutil.Process(pid)
        if not p.is_running(): return False
        if p.status() in ('zombie', 'dead', 'stopped'): return False
        return p.name().lower() == 'mashed.exe'
    except (psutil.NoSuchProcess, psutil.AccessDenied):
        return False

def find_mashed_pid():
    for p in psutil.process_iter(['pid', 'name']):
        if p.info['name'] and p.info['name'].lower() == 'mashed.exe':
            return p.info['pid']
    return None

def mute_pid_audio(pid):
    if not HAS_PYCAW: return False
    try:
        for sess in AudioUtilities.GetAllSessions():
            if sess.Process and sess.Process.pid == pid:
                sess.SimpleAudioVolume.SetMute(1, None)
                return True
    except Exception: pass
    return False

def main():
    if len(sys.argv) < 2:
        print('usage: canon_observe_refused.py <rva-list-file>')
        return 1
    rvas = []
    with open(sys.argv[1]) as f:
        for line in f:
            line = line.strip()
            if not line: continue
            rvas.append(int(line, 16))
    print(f'observing {len(rvas)} RVAs')

    # Launch MASHED
    subprocess.Popen(['cmd.exe', '/c', 'start', '/b', 'MASHED.exe'],
                     cwd=str(ORIGINAL_DIR),
                     creationflags=subprocess.CREATE_NEW_PROCESS_GROUP)
    pid = None
    deadline = time.time() + 8
    while time.time() < deadline:
        pid = find_mashed_pid()
        if pid: break
        time.sleep(0.05)
    if not pid:
        print('FATAL: MASHED never appeared')
        return 1
    print(f'MASHED pid={pid}')
    time.sleep(0.3)
    if not mashed_alive(pid):
        print('DEAD before attach')
        return 3
    device = frida.get_local_device()
    for attempt in range(5):
        try:
            session = device.attach(pid)
            break
        except frida.ProcessNotRespondingError:
            if attempt == 4: print('FATAL: attach failed'); return 4
            time.sleep(0.5)
        except frida.ProcessNotFoundError:
            print('DEAD during attach'); return 3

    rvas_js = ', '.join(f'0x{r:x}' for r in rvas)
    agent = f"""
    var imageBase = Process.findModuleByName('MASHED.exe').base;
    var counts = {{}};
    var errors = {{}};
    var rvas = [{rvas_js}];
    rvas.forEach(function(rva) {{
        var key = '0x' + rva.toString(16);
        counts[key] = 0;
        try {{
            Interceptor.attach(imageBase.add(rva - 0x00400000), {{
                onEnter: function() {{ counts[key]++; }}
            }});
        }} catch (e) {{ errors[key] = e.toString(); }}
    }});
    rpc.exports = {{
        snap: function() {{ return {{counts: counts, errors: errors}}; }}
    }};
    send('ready');
    """
    s = session.create_script(agent)
    s.on('message', lambda m, d: print(f'  [agent] {m.get("payload") or m}'))
    s.load()
    if mute_pid_audio(pid): print('  audio muted')

    # Observe 30s
    print('  observing 30s ...')
    deadline = time.time() + 30
    while time.time() < deadline:
        time.sleep(1)
        if not mashed_alive(pid):
            elapsed = 30 - (deadline - time.time())
            print(f'  DEAD at t={elapsed:.1f}s')
            break

    try: snap = s.exports_sync.snap()
    except Exception as e: print(f'  snap failed: {e}'); snap = {'counts': {}, 'errors': {}}
    alive = mashed_alive(pid)
    print(f'\nalive_at_end={alive}')
    print('=== fires ===')
    fired = []
    not_fired = []
    errored = []
    for rva in rvas:
        key = f'0x{rva:x}'
        if key in snap.get('errors', {}):
            errored.append((rva, snap['errors'][key]))
        elif snap.get('counts', {}).get(key, 0) > 0:
            fired.append((rva, snap['counts'][key]))
        else:
            not_fired.append(rva)
    for rva, count in fired:
        print(f'  0x{rva:08x}  count={count}')
    print(f'\n=== NOT fired (implicit-survive if alive) ===')
    for rva in not_fired:
        print(f'  0x{rva:08x}')
    if errored:
        print(f'\n=== errored ===')
        for rva, err in errored:
            print(f'  0x{rva:08x}  {err}')
    print(f'\nsummary: fired={len(fired)} not_fired={len(not_fired)} errored={len(errored)} alive={alive}')
    try: psutil.Process(pid).kill()
    except: pass
    return 0

if __name__ == '__main__':
    sys.exit(main())
