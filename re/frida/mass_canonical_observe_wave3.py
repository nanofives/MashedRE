"""Wave 3 mass canonical observation — verify ~41 newly-authored C3 candidates."""
from __future__ import annotations
import sys, time
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent.parent
import frida, psutil

MASHED = ROOT / 'original' / 'MASHED.exe'

# Pull all newly-authored RVAs from Wave 3 cluster files
import re
CANDIDATES = []
for cluster_file in [
    'mashedmod/src/mashed_re/Vehicle/Wave3Cluster.cpp',
    'mashedmod/src/mashed_re/Boot/Wave3Cluster.cpp',
    'mashedmod/src/mashed_re/Boot/Wave3Misc.cpp',
    'mashedmod/src/mashed_re/Piz/Wave3PizMisc.cpp',
    'mashedmod/src/mashed_re/Render/Wave3Cluster.cpp',
    'mashedmod/src/mashed_re/Audio/AudioWave3.cpp',
    'mashedmod/src/mashed_re/HUD/HudWave3.cpp',
    'mashedmod/src/mashed_re/Frontend/FrontendWave3.cpp',
    'mashedmod/src/mashed_re/Input/InputWave3.cpp',
    'mashedmod/src/mashed_re/Util/UtilWave3.cpp',
]:
    p = ROOT / cluster_file
    if not p.exists(): continue
    text = p.read_text(encoding='utf-8')
    for m in re.finditer(r'RH_ScopedInstall\s*\(\s*(\w+)\s*,\s*0x([0-9a-fA-F]+)\s*\)', text):
        CANDIDATES.append((int(m.group(2), 16), m.group(1)))

print(f'Found {len(CANDIDATES)} Wave 3 RH_ScopedInstall hooks')


def main():
    device = frida.get_local_device()
    pid = device.spawn(str(MASHED), cwd=str(MASHED.parent))
    print(f'pid={pid}')
    session = device.attach(pid)
    rvas_js = ', '.join(f'{{rva: 0x{rva:08x}, name: {name!r}}}' for rva, name in CANDIDATES)
    agent = f"""
    var counters = {{}};
    var attach_errors = {{}};
    var imageBase = Process.findModuleByName('MASHED.exe').base;
    var candidates = [{rvas_js}];
    candidates.forEach(function(c) {{
        counters[c.name] = 0;
        var addr = imageBase.add(c.rva - 0x00400000);
        try {{
            Interceptor.attach(addr, {{ onEnter: function() {{ counters[c.name]++; }} }});
        }} catch (e) {{ attach_errors[c.name] = e.toString(); }}
    }});
    rpc.exports = {{ snapshot: function() {{ return {{counters: counters, attach_errors: attach_errors}}; }} }};
    send('agent-ready');
    """
    s = session.create_script(agent)
    s.on('message', lambda m, d: print(f'  [agent] {m.get("payload") or m}'))
    s.load()
    device.resume(pid)
    print('resumed; waiting 10s for intro + menu boot ...')
    time.sleep(10)
    try:
        from pycaw.pycaw import AudioUtilities
        for sess in AudioUtilities.GetAllSessions():
            if sess.Process and sess.Process.pid == pid:
                sess.SimpleAudioVolume.SetMute(1, None)
    except Exception: pass
    print('idle 30s ...')
    deadline = time.time() + 30
    while time.time() < deadline:
        time.sleep(1)
        if not psutil.pid_exists(pid): break
    alive = psutil.pid_exists(pid)
    print(f'alive_at_end: {alive}')
    snap = s.exports.snapshot()
    counts = snap.get('counters', {})
    errs = snap.get('attach_errors', {})
    try: psutil.Process(pid).kill()
    except: pass
    print()
    print('=== RESULTS ===')
    fire = []; implicit = []; cantp = []
    for rva, name in CANDIDATES:
        c = counts.get(name, 0); e = errs.get(name)
        if e: cantp.append((rva, name, c, f'attach-err: {e}'))
        elif not alive: cantp.append((rva, name, c, 'process-died'))
        elif c > 0: fire.append((rva, name, c))
        else: implicit.append((rva, name))
    for rva, name, c in fire:
        print(f'  0x{rva:08x}  {name:<35}  FIRE count={c} -- C3+evidence-could-be-C4')
    for rva, name in implicit:
        print(f'  0x{rva:08x}  {name:<35}  IMPLICIT-SURVIVE -- C3-weaker')
    for rva, name, c, r in cantp:
        print(f'  0x{rva:08x}  {name:<35}  CANT-PROMOTE: {r}')
    print()
    print(f'  fire+survive (C4-eligible):  {len(fire)}')
    print(f'  implicit-survive (C3-weaker): {len(implicit)}')
    print(f'  cant-promote:                 {len(cantp)}')
    return 0 if alive else 1


if __name__ == '__main__':
    sys.exit(main())
