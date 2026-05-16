"""Disassemble ConfigLogError and check g_fputs pointer value."""
import frida
import subprocess
import time
import os

ASI_PATH = r"C:\Users\maria\Desktop\Proyectos\Mashed\.worktrees\c3-batch-e-s4\mashedmod\build\mashed_re_dev.asi"
EXE_PATH = r"original\MASHED.exe"
asi_js = ASI_PATH.replace("\\", "\\\\")

SCRIPT = r"""
setTimeout(function() {
    try {
        var asi = '%s';
        Module.load(asi);
        var mod = Process.findModuleByName('mashed_re_dev.asi');
        if (!mod) { send({err: 'no module'}); return; }

        var addr = mod.findExportByName('ConfigLogError');
        if (!addr) { send({err: 'no export'}); return; }

        // Read first 64 bytes of ConfigLogError to see the code
        var bytes = addr.readByteArray(64);
        var hex = Array.from(new Uint8Array(bytes)).map(b=>b.toString(16).padStart(2,'0')).join(' ');
        send({fn_bytes: hex, fn_addr: '0x'+addr.toString(16)});

        // Find g_fputs in the module — it's a const pointer in .rdata
        // We know ConfigLogError reads the log handle then calls g_fputs
        // Let's scan for the g_fputs pointer value in .rdata
        // Try to find the module's .rdata section
        var mods = Process.enumerateModules();
        var asiMod = mods.filter(m => m.name === 'mashed_re_dev.asi')[0];
        send({asi_mod: JSON.stringify({base:'0x'+asiMod.base.toString(16), size: asiMod.size})});

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

for _ in range(80):
    if len(msgs) >= 3:
        break
    time.sleep(0.1)

ses.detach()
proc.kill()
proc.wait()
