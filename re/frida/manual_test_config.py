"""Manual test: call ConfigLogError export from .asi and check for crash."""
import frida
import subprocess
import time
import os

ASI_PATH = r"C:\Users\maria\Desktop\Proyectos\Mashed\.worktrees\c3-batch-e-s4\mashedmod\build\mashed_re_dev.asi"
EXE_PATH = r"original\MASHED.exe"

# Escape for JS string
asi_js = ASI_PATH.replace("\\", "\\\\")

SCRIPT = r"""
setTimeout(function() {
    try {
        var asi = '%s';
        Module.load(asi);
        var mod = Process.findModuleByName('mashed_re_dev.asi');
        if (!mod) { send({err: 'no module'}); return; }
        send({asi_base: '0x'+mod.base.toString(16)});

        var addr = mod.findExportByName('ConfigLogError');
        if (!addr) { send({err: 'no export ConfigLogError'}); return; }
        send({export_addr: '0x'+addr.toString(16)});

        // Read the g_fputs pointer from the .asi
        // g_fputs is a static const at some offset in .rdata
        // Let's just call the export and catch the exception
        var fn = new NativeFunction(addr, 'void', ['int32'], 'mscdecl');
        var msg_va = 0x005d012c;
        send({calling_with_msg: '0x' + msg_va.toString(16)});
        try {
            fn(msg_va);
            send({call_result: 'no_crash'});
        } catch(e) {
            send({call_err: e.message});
        }
    } catch(outer) {
        send({outer_err: outer.message});
    }
}, 3000);
""" % asi_js

env = {**os.environ, 'MASHED_RE_NO_AUTO_HOOK': '1'}
proc = subprocess.Popen([EXE_PATH], cwd='original', env=env)
print('pid:', proc.pid)
time.sleep(2)

dev = frida.get_local_device()
ses = dev.attach(proc.pid)
script_obj = ses.create_script(SCRIPT)
msgs = []

def on_msg(m, d):
    msgs.append(m)
    print('MSG:', m['payload'] if m['type'] == 'send' else m)

script_obj.on('message', on_msg)
script_obj.load()

for _ in range(100):
    if len(msgs) >= 4:
        break
    time.sleep(0.1)

ses.detach()
proc.kill()
proc.wait()
print("Done. msgs:", len(msgs))
