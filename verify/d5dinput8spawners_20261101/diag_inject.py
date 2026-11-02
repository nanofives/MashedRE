"""Spawn MASHED.exe suspended (private root), Module.load the dev asi, call InjectHooks
with an exception handler that records the faulting exception. Never resumes; kills only
its own pid. argv: <root> [HOOK_ONLY value or 'full']. Run under mashed_lock."""
import json, os, sys, time
from pathlib import Path
import frida

root = Path(sys.argv[1])
only = sys.argv[2] if len(sys.argv) > 2 else 'full'
exe = root / 'original' / 'MASHED.exe'
asi = root / 'original' / 'mashed_re_dev.asi'
env = dict(os.environ)
env['MASHED_RE_NO_AUTO_HOOK'] = '1'
env.pop('MASHED_HOOK_ONLY', None)
if only != 'full':
    env['MASHED_HOOK_ONLY'] = only
print('HOOK_ONLY', only)
dev = frida.get_local_device()
pid = dev.spawn([str(exe)], envp=env, cwd=str(exe.parent))
JS = r'''
var r = {ex: []};
Process.setExceptionHandler(function (d) {
    var e = {type: d.type, address: d.address.toString(), code: d.code !== undefined ? d.code : null};
    try { var m = Process.findModuleByAddress(d.address); e.mod = m ? m.name + '+0x' + d.address.sub(m.base).toString(16) : null; } catch (x) {}
    try { e.bt = Thread.backtrace(d.context, Backtracer.FUZZY).slice(0, 12).map(function (a) {
        var m = Process.findModuleByAddress(a); return m ? m.name + '+0x' + a.sub(m.base).toString(16) : a.toString(); }); } catch (x) {}
    r.ex.push(e);
    return false;
});
try {
    var m = Module.load(%PATH%);
    r.base = m.base.toString();
    new NativeFunction(m.getExportByName('InjectHooks'), 'void', [])();
    r.installed = true;
} catch (e) { r.error = '' + e; r.stack = '' + e.stack; }
send(r);
'''
try:
    s = dev.attach(pid)
    got = {}
    sc = s.create_script(JS.replace('%PATH%', json.dumps(str(asi))))
    sc.on('message', lambda m, d: got.update(m.get('payload') or {'err': m.get('description')}))
    sc.load()
    t = time.time()
    while not got and time.time() - t < 30:
        time.sleep(0.05)
    print('RESULT', json.dumps(got, indent=1))
finally:
    dev.kill(pid)
    print('killed', pid)
