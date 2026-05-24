"""Diagnostic: cmd-start MASHED, frida.attach (not spawn), observe survival.

Pattern rationale:
- .asi autoloads via the dinput8 shim during MASHED's normal import
  resolution — no Frida involvement needed for the load.
- Attach-to-running sidesteps the frida.spawn+resume × intro-skip race
  that crashes MASHED at t<5s.
- Intro-skip (empty MPGs in original/toastart/pc/movies/) brings MASHED
  to menu in <3s.
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

def mute_pid_audio(pid):
    if not HAS_PYCAW: return False
    try:
        for sess in AudioUtilities.GetAllSessions():
            if sess.Process and sess.Process.pid == pid:
                sess.SimpleAudioVolume.SetMute(1, None)
                return True
    except Exception: pass
    return False

def find_mashed_pid():
    for p in psutil.process_iter(['pid', 'name']):
        if p.info['name'] and p.info['name'].lower() == 'mashed.exe':
            return p.info['pid']
    return None

def mashed_alive(pid):
    """Real aliveness: Frida holds a PID-handle even after MASHED exits, so
    psutil.pid_exists falsely returns True. Use process_iter (tasklist-level)
    truth-source instead."""
    try:
        p = psutil.Process(pid)
        if not p.is_running(): return False
        st = p.status()
        if st in ('zombie', 'dead', 'stopped'): return False
        if p.name().lower() != 'mashed.exe': return False
        return True
    except (psutil.NoSuchProcess, psutil.AccessDenied):
        return False

def main():
    # cmd-start in original/ so MASHED finds toastart/ etc. /b = no new window.
    subprocess.Popen(['cmd.exe', '/c', 'start', '/b', 'MASHED.exe'],
                     cwd=str(ORIGINAL_DIR),
                     creationflags=subprocess.CREATE_NEW_PROCESS_GROUP)
    # Poll for MASHED process — tight loop because a crashing .asi can
    # take MASHED down inside 200ms (faster than coarse polling can see).
    pid = None
    deadline = time.time() + 8
    while time.time() < deadline:
        pid = find_mashed_pid()
        if pid: break
        time.sleep(0.05)
    if not pid:
        print('FATAL: MASHED never appeared — likely .asi crashed before psutil saw it')
        return 1
    print(f'MASHED started pid={pid}')
    # Give MASHED a beat to finish DllMain chain (so .asi appears in modules).
    time.sleep(0.3)

    if not psutil.pid_exists(pid):
        print(f'  DEAD before attach — MASHED crashed during init')
        return 3

    device = frida.get_local_device()
    detached = []
    # Frida sometimes returns ProcessNotResponding during MASHED's heavy init.
    # Retry a few times with backoff.
    session = None
    for attempt in range(5):
        try:
            session = device.attach(pid)
            break
        except frida.ProcessNotFoundError:
            print(f'  DEAD before attach — MASHED crashed during init')
            return 3
        except frida.ProcessNotRespondingError:
            if attempt == 4:
                print(f'  FATAL: MASHED never accepted Frida agent (5 retries)')
                return 4
            time.sleep(0.5)
    if not session: return 4
    session.on('detached', lambda reason, *a: detached.append((time.time(), reason)))

    # 0x00771a04 — DAT_00771a04 — written 1 when an intro stage starts (callers
    # 0x0049447a / 0x004942f6 / 0x00494aa6), reset 0 when the video signals end.
    # FUN_00493f70 returns it; FUN_00495350's outer loop advances stage when 0.
    # Writing 0 from Frida forces stage advance — boots through 4 stages in <1s,
    # but each iteration still gets Sleep(5) so the message pump runs.
    agent = """
    var INTRO_STAGE_FLAG = ptr('0x00771a04');
    var stageWrites = 0;
    var introFlagVal = -1;

    function forceStageDone() {
        try {
            introFlagVal = INTRO_STAGE_FLAG.readU32();
            INTRO_STAGE_FLAG.writeU32(0);
            stageWrites++;
            return true;
        } catch (e) { return false; }
    }
    function readIntroFlag() {
        try { return INTRO_STAGE_FLAG.readU32(); } catch (e) { return -1; }
    }
    var user32 = Module.load('user32.dll');
    var FindWindowA = new NativeFunction(user32.findExportByName('FindWindowA'),
        'pointer', ['pointer', 'pointer']);
    var GetWindowThreadProcessId = new NativeFunction(
        user32.findExportByName('GetWindowThreadProcessId'),
        'uint32', ['pointer', 'pointer']);
    function countWindows() {
        // Cheap check: 3 FindWindowA calls vs enumerating all desktop windows.
        var myPid = Process.id;
        var titles = ['MASHED [D3D9] [Release]', 'MASHED [D3D9]', 'MASHED'];
        var found = [];
        for (var i = 0; i < titles.length; i++) {
            var t = Memory.allocUtf8String(titles[i]);
            var h = FindWindowA(NULL, t);
            if (!h.isNull()) {
                var outPid = Memory.alloc(4);
                GetWindowThreadProcessId(h, outPid);
                if (outPid.readU32() === myPid) {
                    found.push(titles[i]);
                }
            }
        }
        return found;
    }

    rpc.exports = {
        modnow: function() {
            return Process.enumerateModules().map(function(m) {
                return m.name + ' @ ' + m.base + ' size=0x' + m.size.toString(16);
            });
        },
        forcestage: forceStageDone,
        stagewrites: function() { return stageWrites; },
        introflag: readIntroFlag,
        windows: countWindows
    };
    send('ready');
    """
    s = session.create_script(agent)
    s.on('message', lambda m, d: print(f'  [agent] {m.get("payload") or m}'))
    s.load()

    mods = s.exports_sync.modnow()
    asi_mods = [m for m in mods if 'mashed_re_dev' in m.lower() or '.asi' in m.lower()]
    print(f'modules at attach ({len(mods)} total): asi={asi_mods if asi_mods else "NOT LOADED"}')

    # Mute audio
    if mute_pid_audio(pid):
        print('  muted')

    # Observe survival + window appearance for 25s. PASS = window opens
    # within 15s AND process survives full 25s.
    t0 = time.time()
    intro_deadline = 0  # force-stage disabled while we bisect boot-hangs
    window_seen_at = -1.0
    sub_tick_period = 0.1   # 100ms — 10 hz force-stage during intro window
    for tick in range(1, 26):
        # Fine-grained loop within this 1s tick for force-stage writes.
        end_tick = time.time() + 1.0
        while time.time() < end_tick:
            if time.time() < intro_deadline:
                try: s.exports_sync.forcestage()
                except Exception: pass
            time.sleep(sub_tick_period)
        alive = mashed_alive(pid)
        if not alive:
            elapsed = time.time() - t0
            print(f'  t={elapsed:.1f}s DEAD detached={detached}')
            return 2
        try:
            wins = s.exports_sync.windows()
        except Exception: wins = []
        if wins and window_seen_at < 0:
            window_seen_at = time.time() - t0
            print(f'  t={tick}s WINDOW APPEARED at +{window_seen_at:.1f}s: {wins}')
        if tick == 1 or tick % 2 == 0:
            try:
                sw = s.exports_sync.stagewrites()
                flag = s.exports_sync.introflag()
            except Exception:
                sw, flag = -1, -1
            print(f'  t={tick}s alive  stagewrites={sw} flag={flag} wins={wins}')
    print(f'\nfinal alive={mashed_alive(pid)}; window_seen_at={window_seen_at}; detached events: {detached}')
    # DIAG: leave MASHED running for inspection. Caller must taskkill.
    return 0

if __name__ == '__main__':
    sys.exit(main())
