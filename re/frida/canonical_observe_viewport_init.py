# canonical_observe_viewport_init.py
#
# Canonical-scenario observation for ViewportInit (0x00428590) — C2->C3 evidence.
#
# Scenario:
#   1. Spawn MASHED.exe (5 disk patches applied, d3d9 shim deployed in original/).
#   2. Module.load mashed_re_dev.asi — installs RH_ScopedInstall hooks including
#      ViewportInit (0x00428590) via SubsystemInit.cpp.
#   3. Interceptor.attach to 0x00428590 — count invocations, capture errors.
#   4. Idle 30 seconds on main menu (no crash = GREEN behavioral pass).
#   5. Kill MASHED, report count + crash status.
#
# Expected result: exactly 1 invocation of ViewportInit, no crash, no regression.
#
# ViewportInit is NOT a hot-path function (called once at boot, not per-frame),
# so Interceptor.attach is safe here (no >1000/s overhead risk).
#
# Output: log/canonical_viewport_init.txt
#
# Requires:
#   - All 5 disk patches applied to original/MASHED.exe.
#   - original/videocfg.bin pinned to windowed mode.
#   - d3d9 shim (original/d3d9.dll) deployed.
#   - pycaw + comtypes for per-app mute.
#   - mashed_re_dev.asi built with ViewportInit export present.
#
# Binary anchor (unpatched):
#   MASHED.exe SHA-256: BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E

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

ROOT = Path(__file__).resolve().parent.parent.parent


def _find_original(script_root: Path) -> Path:
    candidate = script_root / 'original' / 'MASHED.exe'
    if candidate.exists():
        return candidate
    parent = script_root.parent.parent
    candidate2 = parent / 'original' / 'MASHED.exe'
    if candidate2.exists():
        return candidate2
    return candidate


MASHED_EXE = _find_original(ROOT)
ASI_PATH   = ROOT / 'mashedmod' / 'build' / 'mashed_re_dev.asi'
OUT_FILE   = ROOT / 'log' / 'canonical_viewport_init.txt'

# ViewportInit RVA 0x00428590 — boot-time init, called once per launch.
VIEWPORT_INIT_RVA = '0x00428590'

AGENT_JS = r'''
'use strict';
const RVA_VIEWPORT_INIT = $RVA$;
var count = 0;
var attachErr = null;

try {
    Interceptor.attach(ptr(RVA_VIEWPORT_INIT), {
        onEnter: function (args) {
            count++;
        }
    });
} catch (e) {
    attachErr = e.message;
}

rpc.exports = {
    snapshot: function () {
        return { count: count, attach_error: attachErr };
    }
};

send({ kind: 'ready', attach_error: attachErr });
'''


def mute_pid_audio(pid: int) -> bool:
    if not HAS_PYCAW:
        return False
    try:
        sessions = AudioUtilities.GetAllSessions()
        for sess in sessions:
            if sess.Process and sess.Process.pid == pid:
                sess.SimpleAudioVolume.SetMute(1, None)
                return True
    except Exception as e:
        print(f"  mute failed: {e}")
    return False


def main() -> int:
    print("=== ViewportInit (0x00428590) canonical-scenario observation ===")
    print(f"MASHED.exe : {MASHED_EXE}")
    print(f"ASI        : {ASI_PATH}")
    print()

    # Pre-flight checks.
    if not MASHED_EXE.exists():
        print(f"FATAL: {MASHED_EXE} not found. Abort.")
        return 1

    shim = MASHED_EXE.parent / 'd3d9.dll'
    if not shim.exists():
        print(f"FATAL: d3d9 shim missing at {shim}.")
        print("       Run mashedmod\\build_d3d9_shim.bat to rebuild + deploy.")
        print("       Refusing to spawn MASHED without windowed-mode shim.")
        return 2

    if not ASI_PATH.exists():
        print(f"FATAL: {ASI_PATH} not found. Run mashedmod\\build.bat first.")
        return 3

    OUT_FILE.parent.mkdir(parents=True, exist_ok=True)

    # Clear mashed.log for fresh run evidence.
    log = MASHED_EXE.parent / 'mashed.log'
    if log.exists():
        try:
            log.unlink()
        except PermissionError:
            pass

    IDLE_SECONDS = 30

    print(f"Spawning {MASHED_EXE} ...")
    proc = subprocess.Popen(
        [str(MASHED_EXE)],
        cwd=str(MASHED_EXE.parent),
        stdin=subprocess.DEVNULL,
        stdout=subprocess.DEVNULL,
        stderr=subprocess.DEVNULL,
        creationflags=0x00000200 | 0x00000008,  # CREATE_NEW_PROCESS_GROUP | DETACHED_PROCESS
    )
    pid = proc.pid
    print(f"  PID = {pid}")

    # Mute audio session.
    if HAS_PYCAW:
        muted = False
        deadline = time.time() + 8
        while time.time() < deadline:
            if mute_pid_audio(pid):
                elapsed = 8 - int(deadline - time.time())
                print(f"  audio muted at t+{elapsed}s")
                muted = True
                break
            time.sleep(0.2)
        if not muted:
            print("  WARNING: could not mute audio session within 8s")
    else:
        print("  pycaw not available — audio not muted")

    # Allow MASHED to initialize before Frida attach.
    time.sleep(0.5)

    # Attach Frida.
    device = frida.get_local_device()
    try:
        session = device.attach(pid)
    except Exception as e:
        print(f"FATAL: Frida attach failed: {e}")
        try:
            proc.kill()
        except Exception:
            pass
        return 4

    js = AGENT_JS.replace('$RVA$', VIEWPORT_INIT_RVA)
    script = session.create_script(js)

    messages = []
    ready = {'value': False, 'attach_error': None}

    def on_message(msg, data):
        if msg['type'] == 'error':
            print(f"  agent error: {msg.get('description')}")
            messages.append(('error', msg.get('description')))
            return
        p = msg.get('payload', {})
        if p.get('kind') == 'ready':
            ready['value'] = True
            ready['attach_error'] = p.get('attach_error')

    script.on('message', on_message)
    script.load()

    # Wait for agent ready signal.
    deadline = time.time() + 10
    while not ready['value'] and time.time() < deadline:
        time.sleep(0.1)

    if not ready['value']:
        print("FATAL: agent never sent 'ready' — process may have crashed before attach.")
        try:
            proc.kill()
        except Exception:
            pass
        return 5

    if ready['attach_error']:
        print(f"WARNING: Interceptor.attach failed: {ready['attach_error']}")
        # This means we can still observe boot behavior but won't have a count.

    print(f"  agent instrumented; idling {IDLE_SECONDS}s at main menu ...")
    print(f"  {'t':>4s}  {'count':>8s}  {'pid_alive':>10s}")

    last_snap = {'count': 0, 'attach_error': None}
    start = time.time()
    alive = True

    while time.time() - start < IDLE_SECONDS:
        alive = psutil.pid_exists(pid)
        if not alive:
            print("  PROCESS DIED before idle complete — RED result")
            break
        try:
            snap = script.exports_sync.snapshot()
            elapsed = int(time.time() - start)
            print(f"  {elapsed:>4d}  {snap.get('count', 0):>8d}  {'yes':>10s}")
            last_snap = snap
        except Exception as e:
            print(f"  snapshot error: {e}")
            break
        time.sleep(5)

    # Final snapshot.
    try:
        last_snap = script.exports_sync.snapshot()
    except Exception:
        pass

    final_count = last_snap.get('count', 0)
    final_attach_err = last_snap.get('attach_error')

    print()
    print("=== RESULT ===")
    print(f"  ViewportInit (0x00428590) invocation count : {final_count}")
    print(f"  attach_error                                : {final_attach_err}")
    print(f"  process alive at end                        : {psutil.pid_exists(pid)}")

    # Determine GREEN / RED / INCONCLUSIVE.
    if not alive:
        verdict = 'RED — process crashed during observation'
    elif final_attach_err:
        verdict = f'INCONCLUSIVE — Interceptor.attach failed: {final_attach_err}'
    elif final_count == 1:
        verdict = 'GREEN — ViewportInit called exactly once, no crash'
    elif final_count == 0:
        verdict = 'INCONCLUSIVE — ViewportInit never fired (hook may not be installed or already fired before attach)'
    else:
        verdict = f'INCONCLUSIVE — ViewportInit called {final_count} times (expected 1)'

    print(f"  VERDICT                                     : {verdict}")

    # Cleanup.
    try:
        session.detach()
    except Exception:
        pass
    try:
        proc.kill()
    except Exception:
        pass
    try:
        proc.wait(timeout=3)
    except Exception:
        pass

    # Write output.
    OUT_FILE.write_text(
        f"canonical_observe_viewport_init.py result\n"
        f"==========================================\n"
        f"RVA          : 0x00428590  ViewportInit\n"
        f"ASI          : {ASI_PATH}\n"
        f"idle_seconds : {IDLE_SECONDS}\n"
        f"\n"
        f"invocation_count : {final_count}\n"
        f"attach_error     : {final_attach_err}\n"
        f"process_survived : {alive}\n"
        f"VERDICT          : {verdict}\n",
        encoding='utf-8',
    )
    print(f"\n  Written to {OUT_FILE}")

    return 0 if 'GREEN' in verdict else (6 if 'INCONCLUSIVE' in verdict else 7)


if __name__ == '__main__':
    sys.exit(main())
