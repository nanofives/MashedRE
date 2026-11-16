"""RWC-FRAME-BOMB-DESTROY original-side trace: who calls RpClumpDestroy (0x004e6e00) on the clump held in DAT_00636c00.

usage: py -3.12 orig_bomb_destroy.py <MASHED.exe path> <out.txt> [menu_wait_s]
Spawns the given MASHED.exe (cwd = its folder) under Frida, logs entry/exit of 0x00402a40, 0x0045b930, 0x00404820, every
write of DAT_00636c00 seen at 0x004039c0's return, and each 0x004e6e00 call whose argument equals [0x00636c00] (with the
return address). After menu_wait_s it posts WM_CLOSE to the process's top-level windows and waits for the exit.
Kills only the PID it spawned (on timeout).
"""
import ctypes, sys, time
from ctypes import wintypes
import frida

EXE, OUT = sys.argv[1], sys.argv[2]
WAIT = float(sys.argv[3]) if len(sys.argv) > 3 else 25.0

AGENT = r'''
'use strict';
var BOMB = ptr('0x00636c00');
function hook(name, va) {
  Interceptor.attach(ptr(va), {
    onEnter: function () { send({k: 'enter', n: name, ra: this.returnAddress.toString(), bomb: BOMB.readU32().toString(16)}); },
    onLeave: function () { send({k: 'leave', n: name, bomb: BOMB.readU32().toString(16)}); }
  });
}
hook('AppDestroy_00402a40', '0x00402a40');
hook('FUN_0045b930', '0x0045b930');
hook('FUN_00404820', '0x00404820');
Interceptor.attach(ptr('0x004039c0'), { onLeave: function () { send({k: 'bombload', bomb: BOMB.readU32().toString(16)}); } });
Interceptor.attach(ptr('0x004e6e00'), {
  onEnter: function (args) {
    var a = args[0].toUInt32 ? args[0].toUInt32() : parseInt(args[0].toString(), 16);
    var b = BOMB.readU32();
    if (b !== 0 && a === b) send({k: 'destroy_bomb', clump: a.toString(16), ra: this.returnAddress.toString()});
  }
});
send({k: 'hooked'});
'''

lines = []
def log(s):
    lines.append(s); print(s, flush=True)

def on_msg(m, _d):
    if m['type'] == 'send':
        log('MSG ' + repr(m['payload']))
    else:
        log('ERR ' + repr(m))

pid = frida.spawn([EXE], cwd=EXE.rsplit('\\', 1)[0] if '\\' in EXE else None)
log('spawned pid %d' % pid)
sess = frida.attach(pid)
sc = sess.create_script(AGENT); sc.on('message', on_msg); sc.load()
frida.resume(pid)
time.sleep(WAIT)

user32 = ctypes.WinDLL('user32', use_last_error=True)
WNDENUMPROC = ctypes.WINFUNCTYPE(wintypes.BOOL, wintypes.HWND, wintypes.LPARAM)
hwnds = []
def cb(h, _l):
    p = wintypes.DWORD(); user32.GetWindowThreadProcessId(h, ctypes.byref(p))
    if p.value == pid and user32.IsWindowVisible(h): hwnds.append(h)
    return True
user32.EnumWindows(WNDENUMPROC(cb), 0)
log('WM_CLOSE to %d window(s)' % len(hwnds))
for h in hwnds: user32.PostMessageW(h, 0x0010, 0, 0)

import psutil
try:
    rc = psutil.Process(pid).wait(timeout=60)
    log('exited rc %s' % rc)
except psutil.TimeoutExpired:
    log('timeout: killing own pid %d' % pid)
    psutil.Process(pid).kill()
except psutil.NoSuchProcess:
    log('exited (already gone)')
open(OUT, 'w').write('\n'.join(lines) + '\n')
