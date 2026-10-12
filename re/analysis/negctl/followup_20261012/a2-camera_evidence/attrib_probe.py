"""a2-camera attribution probe driver: spawn MASHED (own PID only), drive to race, run attrib_probe.js."""
import json, os, shutil, subprocess, sys, time
from pathlib import Path

WT = Path(r'C:\Users\maria\Desktop\Proyectos\Mashed\.worktrees\a2-camera')
sys.path.insert(0, str(WT / 're' / 'frida'))
import frida
import run_diff as rd

HERE = Path(__file__).resolve().parent
asi = Path(sys.argv[1]) if len(sys.argv) > 1 else WT / 'mashedmod' / 'build' / 'mashed_re_dev.asi'
out = Path(sys.argv[2]) if len(sys.argv) > 2 else HERE / 'attrib_out.json'

shutil.copy2(str(WT / 'scripts' / 'canonical' / 'videocfg_windowed.bin'), str(rd.MASHED_EXE.parent / 'videocfg.bin'))
env = {**os.environ, 'MASHED_RE_NO_AUTO_HOOK': '1'}
proc = subprocess.Popen([str(rd.MASHED_EXE)], cwd=str(rd.MASHED_EXE.parent), env=env,
                        stdin=subprocess.DEVNULL, stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL,
                        creationflags=0x200 | 0x8)
print('pid', proc.pid, flush=True)
try:
    time.sleep(0.2)
    session = frida.get_local_device().attach(proc.pid)
    pin = rd.pin_gamesave(session, proc.pid)
    if not rd.drive_to_race(session):
        print('no race'); sys.exit(6)
    js = (WT / 're' / 'frida' / 'js' / 'standins.js').read_text(encoding='utf-8') + '\n' + \
        (HERE / 'attrib_probe.js').read_text(encoding='utf-8').replace('%ASI%', str(asi).replace('\\', '\\\\'))
    scr = session.create_script(js)
    scr.on('message', lambda m, d: print('msg', m))
    scr.load()
    res = json.loads(scr.exports_sync.run())
    out.write_text(json.dumps(res, indent=1))
    print(json.dumps(res, indent=1)[:6000])
finally:
    try: proc.kill()
    except Exception: pass
    try: proc.wait(timeout=5)
    except Exception: pass
    print('killed', proc.pid)
