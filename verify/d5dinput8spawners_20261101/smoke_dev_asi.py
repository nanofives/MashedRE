"""Smoke for re/frida/dev_asi.py: spawn the original with the asi Module.loaded
before the first resume, in a root with NO dinput8 proxy, and check the hook site.

  py -3.12 smoke_dev_asi.py <root-with-original> <rva> [seconds]

Env: MASHED_HOOK_ONLY=<rva> is set for the spawn, so InjectHooks installs one hook.
Prints JSON: asi base, whether dinput8 in the process is the system one, the bytes at
<rva> after resume, whether rel32 lands inside the asi, and whether the pid survived.
"""
import json
import sys
import time
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parents[2] / 're' / 'frida'))
import dev_asi  # noqa: E402

root = Path(sys.argv[1])
rva = int(sys.argv[2], 16)
secs = float(sys.argv[3]) if len(sys.argv) > 3 else 15.0
exe = root / 'original' / 'MASHED.exe'
import os  # noqa: E402
env = dict(os.environ)
env['MASHED_HOOK_ONLY'] = f'0x{rva:08x}'
env.setdefault('MASHED_MUTE', '1')
env.pop(dev_asi.NO_AUTO, None)   # helper must force it back on

proc, sess, info = dev_asi.spawn_original(exe, hooks=True, env=env)
print('pid', proc.pid, flush=True)
probe = sess.create_script(r'''
rpc.exports = { look: function (rva) {
  var a = ptr(rva); var asi = Process.findModuleByName('mashed_re_dev.asi');
  var di = Process.findModuleByName('dinput8.dll');
  var op = a.readU8(); var rel = a.add(1).readS32();
  var tgt = a.add(5).add(rel);
  return { op: op, tgt: tgt.toString(),
           in_asi: asi !== null && tgt.compare(asi.base) >= 0 && tgt.compare(asi.base.add(asi.size)) < 0,
           asi_count: Process.enumerateModules().filter(function (m) { return m.name.toLowerCase() === 'mashed_re_dev.asi'; }).length,
           asi_path: asi ? asi.path : null, dinput8: di ? di.path : null,
           qol: Process.findModuleByName('mashed_qol.asi') !== null };
} };''')
probe.load()
time.sleep(secs)
alive = proc.poll() is None
look = probe.exports_sync.look(hex(rva)) if alive else None
out = {'info': info, 'alive_after_s': secs if alive else None, 'look': look}
print(json.dumps(out, indent=1), flush=True)
try:
    sess.detach()
except Exception:
    pass
proc.kill()
ok = bool(alive and look and look['op'] == 0xE9 and look['in_asi'] and look['asi_count'] == 1
          and 'original' not in (look['dinput8'] or 'original').lower())
print('SMOKE', 'PASS' if ok else 'FAIL')
sys.exit(0 if ok else 1)
