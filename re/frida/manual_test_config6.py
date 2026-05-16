"""Search for game_fputs asm pattern and check exports."""
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
        var size = mod.size;

        // Search for: b8 54 42 4a 00 (mov eax, 0x004a4254)
        var pat1 = 'b8 54 42 4a 00';
        var m1 = Memory.scanSync(base, size, pat1);
        send({game_fputs_mov: m1.length, addrs: JSON.stringify(m1.slice(0,3).map(m=>({
            addr:'0x'+m.address.toString(16),
            ctx: Array.from(new Uint8Array(m.address.readByteArray(10))).map(b=>b.toString(16).padStart(2,'0')).join(' ')
        })))});

        // Also list first 20 exports
        var exports = mod.enumerateExports().slice(0,20).map(e=>e.name+'@0x'+e.address.toString(16));
        send({exports: JSON.stringify(exports)});

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
