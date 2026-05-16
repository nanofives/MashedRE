"""Check if the game's fputs at 0x004a4254 calls eax internally (it was an import thunk).
Also try calling ConfigLogDebug with our fix."""
import frida
import subprocess
import time
import os

ASI_PATH = r"C:\Users\maria\Desktop\Proyectos\Mashed\.worktrees\c3-batch-e-s4\mashedmod\build\mashed_re_dev.asi"
EXE_PATH = r"original\MASHED.exe"
asi_js = ASI_PATH.replace("\\", "\\\\")

SCRIPT = """
setTimeout(function() {
    try {
        // Disassemble the game's 'fputs' at 0x004a4254 more carefully - 64 bytes
        var game_fputs = ptr('0x004a4254');
        var bytes = game_fputs.readByteArray(64);
        var hex = Array.from(new Uint8Array(bytes)).map(b=>b.toString(16).padStart(2,'0')).join(' ');
        send({game_fputs_full: hex});

        // Get the game's module range
        var gamemod = Process.getModuleByName('MASHED.exe');
        send({game_base: '0x'+gamemod.base.toString(16), game_size: gamemod.size});

        // Check if 0x004a4254 - 0x00400000 = 0xA4254 is in executable range
        send({offset_in_exe: '0x' + (0x004a4254 - 0x00400000).toString(16)});

    } catch(outer) {
        send({outer_err: outer.message});
    }
}, 2000);
"""

env = {**os.environ}
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
    if len(msgs) >= 3:
        break
    time.sleep(0.1)

ses.detach()
proc.kill()
proc.wait()
