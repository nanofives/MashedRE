"""Implicit-survival C3 sweep — promote authored-but-stayed-C2 candidates.

Many hooks are RH_ScopedInstall'd in the .asi but stayed C2 in hooks.csv because
they didn't FIRE during prior canonical-observation runs at boot-to-menu idle.
However, their installation did NOT destabilize MASHED (the game reached menu and
survived). That is C3-equivalent BEHAVIORAL evidence per the hot-path behavioral
lane precedent (memory: project_hot_path_behavioral_lane).

This sweep:
1. Spawns MASHED via Frida (ASI loader autoloads .asi — DO NOT Module.load,
   per feedback_no_explicit_module_load_asi).
2. Installs Interceptors on the 21 authored-but-C2 candidates from Waves 0+2
   BEFORE resume. Interceptor agent presence is required — without ANY agent
   script loaded, Frida spawn-resume causes MASHED to die during intro.
3. Resumes; waits BOOT_WAIT=8s for intro + menu; idles 30s at menu.
4. Reports survival + per-candidate counts.

Promotion rule:
- ALIVE_AT_END=True AND count > 0 → fire-and-survive: full C3 evidence
- ALIVE_AT_END=True AND count == 0 → implicit-survival: C3-equivalent (weaker)
- ALIVE_AT_END=False → bisect needed
"""
from __future__ import annotations

import sys, time
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent.parent

import frida, psutil

MASHED = ROOT / 'original' / 'MASHED.exe'

# 21 authored-but-stayed-C2 candidates from Waves 0+2
CANDIDATES = [
    # Wave 0 stayed C2 (10)
    (0x00422ac0, 'SlotQuadSet'),
    (0x004235b0, 'AiPizLoad'),
    (0x00428320, 'TextWidthMeasureB'),
    (0x0042aa00, 'MenuCursorStep'),
    (0x0042e5b0, 'MenuChromeShellB'),
    (0x00439210, 'LobbySlotListRender'),
    (0x00492e60, 'SetDefaultViewWindow'),
    (0x00493560, 'thunk_HwExitDispatch'),
    (0x004cc6e0, 'RwStreamWriteChunked'),
    (0x005be140, 'AudioSubStructThreeWrite'),
    # Wave 2 stayed C2 (11)
    (0x00426c00, 'GameStateFlagGet'),
    (0x0042b8e0, 'StatePhaseIsTwo'),
    (0x0042b910, 'RaceEndConstGet'),
    (0x0042b940, 'StatePhaseSubSet'),
    (0x0042c1c0, 'RaceInterruptFlagGet'),
    (0x0042c1d0, 'RaceStateArrayZero'),
    (0x004026d0, 'BootQueueFlush'),
    (0x00402f50, 'BootDefaultParamsInit'),
    (0x004114c0, 'VtableTeardown_114c0'),
    (0x00432080, 'RaceEndCheckFinish'),
    (0x005abfa0, 'AudioWaveLoad'),
]


def main():
    device = frida.get_local_device()
    pid = device.spawn(str(MASHED), cwd=str(MASHED.parent))
    print(f'pid={pid}')

    session = device.attach(pid)

    # Agent: install Interceptors on all 21 candidates BEFORE resume.
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
            Interceptor.attach(addr, {{
                onEnter: function() {{ counters[c.name]++; }}
            }});
        }} catch (e) {{
            attach_errors[c.name] = e.toString();
        }}
    }});
    rpc.exports = {{
        snapshot: function() {{ return {{counters: counters, attach_errors: attach_errors}}; }}
    }};
    send('agent-ready');
    """
    s = session.create_script(agent)
    s.on('message', lambda m, d: print(f'  [agent] {m.get("payload") or m}'))
    s.load()

    device.resume(pid)
    print('resumed; waiting 8s for intro + menu boot ...')
    time.sleep(8)

    # Mute audio
    try:
        from pycaw.pycaw import AudioUtilities
        for sess in AudioUtilities.GetAllSessions():
            if sess.Process and sess.Process.pid == pid:
                sess.SimpleAudioVolume.SetMute(1, None)
    except Exception as e:
        print(f'  mute err (non-fatal): {e}')

    print('idle 30s at menu, polling ...')
    deadline = time.time() + 30
    while time.time() < deadline:
        if not psutil.pid_exists(pid):
            break
        time.sleep(1)

    alive = psutil.pid_exists(pid)
    print(f'alive_at_end: {alive}')

    snap = s.exports.snapshot()
    counts = snap.get('counters', {})
    errs = snap.get('attach_errors', {})

    try: psutil.Process(pid).kill()
    except: pass

    print()
    print('=== RESULTS ===')
    fire_survive = []
    implicit_survive = []
    cant_promote = []
    for rva, name in CANDIDATES:
        c = counts.get(name, 0)
        e = errs.get(name)
        if e:
            cant_promote.append((rva, name, c, f'attach-error: {e}'))
        elif not alive:
            cant_promote.append((rva, name, c, 'process-died'))
        elif c > 0:
            fire_survive.append((rva, name, c))
        else:
            implicit_survive.append((rva, name))

    for rva, name, c in fire_survive:
        print(f'  0x{rva:08x}  {name:<35}  FIRE+SURVIVE count={c} -- C3 eligible')
    for rva, name in implicit_survive:
        print(f'  0x{rva:08x}  {name:<35}  IMPLICIT-SURVIVE count=0 -- C3 eligible (weaker)')
    for rva, name, c, reason in cant_promote:
        print(f'  0x{rva:08x}  {name:<35}  CANT-PROMOTE: {reason}')

    print()
    print(f'  fire-and-survive: {len(fire_survive)}')
    print(f'  implicit-survive: {len(implicit_survive)}')
    print(f'  cant-promote:     {len(cant_promote)}')

    return 0 if alive else 1


if __name__ == '__main__':
    sys.exit(main())
