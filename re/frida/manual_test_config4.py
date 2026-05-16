"""Disassemble ConfigLogError to see if volatile changed the call pattern."""
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
        var base = mod.base;

        var addr = mod.findExportByName('ConfigLogError');
        var bytes = addr.readByteArray(48);
        var hex = Array.from(new Uint8Array(bytes)).map(b=>b.toString(16).padStart(2,'0')).join(' ');
        send({fn_bytes: hex, fn_addr: '0x'+addr.toString(16), base: '0x'+base.toString(16)});

        // Check if 0x004a4254 is now in the .asi memory (volatile should store it)
        var pattern = '54 42 4a 00';
        var matches = Memory.scanSync(base, mod.size, pattern);
        send({g_fputs_matches: JSON.stringify(matches.slice(0,5).map(m => ({addr:'0x'+m.address.toString(16)})))});

    } catch(outer) {
        send({outer_err: outer.message});
    }
}, 3000);
""" % asi_js

env = {**os.environ, 'MASHED_RE_NO_AUTO_HOOK': '1'}
proc = subprocess.Popen([EXE_PATH], cwd='original', env=env)
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
    if len(msgs) >= 2:
        break
    time.sleep(0.1)

ses.detach()
proc.kill()
proc.wait()
