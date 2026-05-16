"""Search for the naked jmp thunk and check what ConfigLogError actually calls."""
import frida
import subprocess
import time
import os

ASI_PATH = r"C:\Users\maria\Desktop\Proyectos\Mashed\.worktrees\c3-batch-e-s4\mashedmod\build\mashed_re_dev.asi"
EXE_PATH = r"original\MASHED.exe"

SCRIPT = """
setTimeout(function() {
    try {
        Module.load('%s');
        var mod = Process.findModuleByName('mashed_re_dev.asi');
        var base = mod.base;

        // Search for 'ff 25' (jmp dword ptr [abs]) or 'ff 15' pattern
        // jmp dword ptr [target] = ff 25 <4 bytes addr>
        var jmpPat = 'ff 25';
        var jmps = Memory.scanSync(base, mod.size, jmpPat);
        send({jmp_dword_ptr_count: jmps.length, jmps: JSON.stringify(jmps.slice(0,10).map(m=>({
            addr:'0x'+m.address.toString(16),
            ctx: Array.from(new Uint8Array(m.address.readByteArray(8))).map(b=>b.toString(16).padStart(2,'0')).join(' ')
        })))});

        // What is at the call target offset 0x867f?
        var target = base.add(0x867f);
        // Read 6 bytes before it too (might show how it was reached)
        var preBytes = target.sub(6).readByteArray(38);
        var preHex = Array.from(new Uint8Array(preBytes)).map(b=>b.toString(16).padStart(2,'0')).join(' ');
        send({target_with_pre: preHex, at: '0x'+target.sub(6).toString(16)});

    } catch(e) { send({err: e.message}); }
}, 3000);
""" % ASI_PATH.replace('\\', '\\\\')

env = {**os.environ, 'MASHED_RE_NO_AUTO_HOOK': '1'}
proc = subprocess.Popen([EXE_PATH], cwd='original', env=env)
time.sleep(2)
dev = frida.get_local_device()
ses = dev.attach(proc.pid)
s = ses.create_script(SCRIPT)
msgs = []
def on_msg(m,d): msgs.append(m); print('MSG:', m['payload'])
s.on('message', on_msg)
s.load()
for _ in range(80):
    if len(msgs)>=2: break
    time.sleep(0.1)
ses.detach(); proc.kill(); proc.wait()
