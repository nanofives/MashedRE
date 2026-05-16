"""Check what's at offset 0x867f in the .asi (the call target from ConfigLogError)."""
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

        // Call target from ConfigLogError = base + 0x867f
        var target = base.add(0x867f);
        var bytes = target.readByteArray(32);
        var hex = Array.from(new Uint8Array(bytes)).map(b=>b.toString(16).padStart(2,'0')).join(' ');
        send({target_867f: hex, target_addr: '0x'+target.toString(16)});

        // Also look for 'ff d0' (call eax) pattern in .asi - our __asm should have this
        // and for 54 42 4a 00 (the game's fputs RVA bytes)
        var ffD0 = Memory.scanSync(base, mod.size, 'ff d0');
        send({call_eax_count: ffD0.length, first5: JSON.stringify(ffD0.slice(0,5).map(m=>({
            addr:'0x'+m.address.toString(16),
            ctx: Array.from(new Uint8Array(m.address.sub(4).readByteArray(10))).map(b=>b.toString(16).padStart(2,'0')).join(' ')
        })))});

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
