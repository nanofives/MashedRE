"""Wave 2 mass canonical observation — verify 20 newly-authored C3 candidates.

Spawned MASHED with all 20 Wave 1 hooks installed via RH_ScopedInstall.
Interceptor.attach to each candidate, idle 30s at main menu, snapshot counts.
Promote each fire+survive C2->C3.

Reuses the proven spawn-with-suspend pattern from re/frida/mass_canonical_observe.py.
"""

from __future__ import annotations

import os
import subprocess
import sys
import time
from pathlib import Path

import frida
import psutil

ROOT = Path(__file__).resolve().parent.parent.parent
MASHED_EXE = ROOT / 'original' / 'MASHED.exe'
ASI = ROOT / 'mashedmod' / 'build' / 'mashed_re_dev.asi'
OUT_FILE = ROOT / 'log' / 'mass_canonical_observe_wave2.txt'

CANDIDATES = [
    # GameStateCluster
    (0x00426c00, 'GameStateFlagGet'),
    (0x0042b8e0, 'StatePhaseIsTwo'),
    (0x0042b910, 'RaceEndConstGet'),
    (0x0042b940, 'StatePhaseSubSet'),
    (0x0042c1c0, 'RaceInterruptFlagGet'),
    (0x0042c1d0, 'RaceStateArrayZero'),
    # BootLowRvaCluster
    (0x004026d0, 'BootQueueFlush'),
    (0x00402f50, 'BootDefaultParamsInit'),
    (0x004114c0, 'VtableTeardown_114c0'),
    (0x00431ae0, 'DefaultParam_SetField04'),
    (0x00431af0, 'DefaultParam_SetField08'),
    (0x00431b00, 'DefaultParam_SetField00'),
    # ReplayCluster (vehicle)
    (0x00432080, 'RaceEndCheckFinish'),
    # Render MixedC3Sweep
    (0x004cc820, 'RwFreeListCreate'),
    # TextureLoaderCluster
    (0x004c5800, 'RwTexDictionarySetCurrent'),
    (0x004c5820, 'RwTexDictionaryGetCurrent'),
    (0x004c5a00, 'RwTextureCreate'),
    (0x004c5ae0, 'RwTextureSetName'),
    (0x004c5b50, 'RwTextureSetMaskName'),
    # Audio MixedC3Sweep
    (0x005abfa0, 'AudioWaveLoad'),
]

BOOT_WAIT = 8
IDLE_SECONDS = 30


def mute_session(pid: int):
    try:
        from pycaw.pycaw import AudioUtilities
        sessions = AudioUtilities.GetAllSessions()
        for s in sessions:
            if s.Process and s.Process.pid == pid:
                s.SimpleAudioVolume.SetMute(1, None)
    except Exception as e:
        print(f'[warn] could not mute: {e}')


def main():
    print(f'=== Wave 2 mass canonical observe ===')
    print(f'Candidates: {len(CANDIDATES)}')
    print(f'Spawn:      {MASHED_EXE}')
    print(f'ASI:        {ASI}')
    print()

    if not ASI.exists():
        print(f'ERROR: ASI not built. Run mashedmod\\build.bat first.')
        return 1

    # Spawn suspended via Frida
    print('Spawning MASHED suspended ...')
    device = frida.get_local_device()
    pid = device.spawn(str(MASHED_EXE), cwd=str(MASHED_EXE.parent))
    print(f'  pid = {pid}')

    # Load .asi into the spawned process (this triggers InjectHooks() → RH_ScopedInstall fires for all hooks)
    print(f'Attaching + loading .asi ...')
    session = device.attach(pid)

    # Inject Frida agent that installs Interceptor on candidates
    rvas = [(rva, name) for rva, name in CANDIDATES]
    rvas_js = ', '.join(f'{{rva: 0x{rva:08x}, name: {name!r}}}' for rva, name in rvas)
    agent_js = f"""
    var counters = {{}};
    var attach_errors = {{}};
    var imageBase = Process.findModuleByName('MASHED.exe').base;
    var candidates = [{rvas_js}];

    candidates.forEach(function(c) {{
        counters[c.name] = 0;
        var addr = imageBase.add(c.rva - 0x00400000);
        try {{
            Interceptor.attach(addr, {{
                onEnter: function(args) {{ counters[c.name]++; }}
            }});
        }} catch (e) {{
            attach_errors[c.name] = e.toString();
        }}
    }});

    rpc.exports = {{
        snapshot: function() {{
            return {{counters: counters, attach_errors: attach_errors}};
        }}
    }};

    send({{type: 'attached', count: candidates.length, errors: Object.keys(attach_errors).length}});
    """

    script = session.create_script(agent_js)
    messages = []
    def on_message(msg, data):
        messages.append(msg)
        if msg.get('type') == 'send':
            print(f'  [agent] {msg["payload"]}')
        elif msg.get('type') == 'error':
            print(f'  [agent ERROR] {msg.get("description")}')
    script.on('message', on_message)
    script.load()

    # NOTE: Do NOT call Module.load(.asi) here — Ultimate-ASI-Loader autoloads
    # the .asi when MASHED resumes. Calling Module.load explicitly would
    # double-install all RH_ScopedInstall hooks, corrupting the inline-JMP
    # patches (lesson from c3-sweep Wave 2 RED 2026-05-23).

    # Resume process — ASI loader picks up mashed_re_dev.asi automatically.
    print(f'\nResuming process ...')
    device.resume(pid)

    print(f'  Waiting {BOOT_WAIT}s for boot (intro + window create) ...')
    time.sleep(BOOT_WAIT)

    # Mute audio
    print('  Muting MASHED audio session ...')
    mute_session(pid)

    print(f'  Idle {IDLE_SECONDS}s at main menu ...')
    deadline = time.time() + IDLE_SECONDS
    while time.time() < deadline:
        if not psutil.pid_exists(pid):
            print('  PROCESS DIED before idle complete')
            break
        time.sleep(1)

    alive_at_end = psutil.pid_exists(pid)
    reached_menu = alive_at_end

    # Snapshot counts
    try:
        snapshot = script.exports.snapshot()
    except Exception as e:
        print(f'  snapshot ERROR: {e}')
        snapshot = {'counters': {}, 'attach_errors': {}}

    final_counters = snapshot.get('counters', {})
    attach_errors = snapshot.get('attach_errors', {})

    # Cleanup
    try:
        psutil.Process(pid).kill()
    except: pass
    time.sleep(1)
    try: session.detach()
    except: pass

    # Report
    print()
    print('=== RESULTS ===')
    print(f'  reached_main_menu : {reached_menu}')
    print(f'  alive_at_end      : {alive_at_end}')
    print()

    promoted = []
    deferred = []
    for rva, name in CANDIDATES:
        count = final_counters.get(name, 0)
        aerr = attach_errors.get(name)
        if aerr:
            verdict = f'RED -- attach error: {aerr}'
            deferred.append((rva, name, count, aerr))
        elif count > 0 and reached_menu:
            verdict = f'GREEN count={count} -- C2->C3 eligible'
            promoted.append((rva, name, count))
        else:
            verdict = f'INCONCLUSIVE count={count}, alive={reached_menu} -- stay C2'
            deferred.append((rva, name, count, None))
        print(f'  0x{rva:08x}  {name:<35}  {verdict}')

    print()
    print(f'  Promoted (C2->C3 eligible): {len(promoted)}')
    print(f'  Stay C2 (inconclusive):     {len(deferred)}')

    # Write log
    OUT_FILE.parent.mkdir(parents=True, exist_ok=True)
    with open(OUT_FILE, 'w', encoding='utf-8') as f:
        f.write(f'=== Wave 2 mass canonical observe ===\n')
        f.write(f'reached_main_menu: {reached_menu}\n')
        f.write(f'alive_at_end: {alive_at_end}\n')
        f.write(f'\n=== Per-candidate ===\n')
        for rva, name in CANDIDATES:
            count = final_counters.get(name, 0)
            aerr = attach_errors.get(name)
            f.write(f'0x{rva:08x}\t{name}\tcount={count}\tattach_error={aerr or "-"}\n')

    print(f'\nWrote: {OUT_FILE}')
    return 0


if __name__ == '__main__':
    sys.exit(main())
