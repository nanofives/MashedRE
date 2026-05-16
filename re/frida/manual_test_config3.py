"""Check what's at the call target inside the .asi."""
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

        // ConfigLogError at base+0x4bf0
        // Call target: base+0x4bf0 + 19 + 0x3a7c = base+0x867f
        var callTarget = base.add(0x867f);
        var targetBytes = callTarget.readByteArray(32);
        var hex = Array.from(new Uint8Array(targetBytes)).map(b=>b.toString(16).padStart(2,'0')).join(' ');
        send({call_target_addr: '0x'+callTarget.toString(16), call_target_bytes: hex});

        // Also check what g_fputs pointer variable contains
        // It's a static const - let's look for 0x004a4254 in the .rdata section
        // by scanning the module's memory
        // g_fputs var is at some fixed offset; let's check if 0x004a4254 appears in .asi memory
        var scan_start = base;
        var scan_size = mod.size;
        // Search for the bytes of 0x004a4254 (little-endian: 54 42 4a 00)
        var pattern = '54 42 4a 00';
        var matches = Memory.scanSync(scan_start, scan_size, pattern);
        send({g_fputs_matches: JSON.stringify(matches.slice(0,5).map(m => '0x'+m.address.toString(16)))});

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
    if len(msgs) >= 2:
        break
    time.sleep(0.1)

ses.detach()
proc.kill()
proc.wait()
