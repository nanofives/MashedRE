# In-process-targeted menu input via PostMessage(MASHED_HWND, WM_KEYDOWN/UP).
#
# SAFE: PostMessageA targets a SPECIFIC window handle -> the message lands ONLY in
# MASHED's own thread message queue. It does NOT synthesize global input, does NOT
# touch the foreground or any other window (unlike the banned keybd_event/SendInput).
# So it cannot affect the user's other work.
#
# Verification built in (observation): the Frida agent hooks user32!DispatchMessageA
# and counts WM_KEYDOWN (0x100) actually dispatched by MASHED's pump, and counts the
# frontend nav handlers. If our posted WM_KEYDOWNs show up in DispatchMessageA AND the
# nav handlers fire, the menu consumes WM_KEYDOWN and this path drives navigation.
#
# Usage: py -3.12 re/frida/input_feed_wm.py [--settle MS] [--seconds N] [--count 0x..,..]
import ctypes, os, sys, time
from ctypes import wintypes
from pathlib import Path
import frida
try: import psutil
except ImportError: psutil = None

ROOT = Path(__file__).resolve().parent.parent.parent
ORIG = ROOT / "original"; EXE = ORIG / "MASHED.exe"
WM_KEYDOWN, WM_KEYUP = 0x100, 0x101
VK = {"down":0x28,"up":0x26,"left":0x25,"right":0x27,"enter":0x0D,"esc":0x1B,"space":0x20}

AGENT = r'''
'use strict';
const CNT={}; let kd=0, kdVks={};
rpc.exports = {
  countthese:function(rvas){ rvas.forEach(function(r){ CNT[r]=0;
    try{ Interceptor.attach(ptr(r),{onEnter(){CNT[r]++;}}); }catch(e){ CNT[r]=-1; } }); return Object.keys(CNT).length; },
  counts:function(){ return CNT; },
  wmkeydown:function(){ return {total:kd, vks:kdVks}; }
};
// observe WM_KEYDOWN actually dispatched by MASHED's pump
try {
  const u = Process.findModuleByName('user32.dll');
  const dm = u.findExportByName('DispatchMessageA') || u.findExportByName('DispatchMessageW');
  if (dm) Interceptor.attach(dm, { onEnter(a){
    try { const msg = a[0].add(4).readU32(); if (msg===0x100){ kd++; const vk=a[0].add(8).readU32(); kdVks[vk]=(kdVks[vk]||0)+1; } }catch(e){}
  }});
} catch(e){}
send({kind:'ready'});
'''

def find_hwnd_for_pid(target_pid):
    user32 = ctypes.windll.user32
    found = []
    proto = ctypes.WINFUNCTYPE(wintypes.BOOL, wintypes.HWND, wintypes.LPARAM)
    def cb(hwnd, _):
        pid = wintypes.DWORD()
        user32.GetWindowThreadProcessId(hwnd, ctypes.byref(pid))
        if pid.value == target_pid and user32.IsWindowVisible(hwnd):
            n = user32.GetWindowTextLengthW(hwnd)
            if n:
                buf = ctypes.create_unicode_buffer(n+1); user32.GetWindowTextW(hwnd, buf, n+1)
                if "MASHED" in buf.value.upper(): found.append(hwnd)
        return True
    user32.EnumWindows(proto(cb), 0)
    return found[0] if found else None

def main():
    settle = int(sys.argv[sys.argv.index("--settle")+1]) if "--settle" in sys.argv else 12000
    seconds = int(sys.argv[sys.argv.index("--seconds")+1]) if "--seconds" in sys.argv else 40
    count_rvas = sys.argv[sys.argv.index("--count")+1].split(",") if "--count" in sys.argv else []
    env = dict(os.environ); env["MASHED_RE_NO_AUTO_HOOK"]="1"
    dev = frida.get_local_device(); pid = dev.spawn(str(EXE), cwd=str(ORIG), env=env)
    sess = dev.attach(pid)
    scr = sess.create_script(AGENT)
    scr.on("message", lambda m,d: None)
    scr.load()
    if count_rvas: scr.exports_sync.countthese(count_rvas)
    dev.resume(pid)
    print(f"  resumed; PostMessage WM_KEYDOWN to MASHED HWND after {settle}ms (TARGETED, not global)")

    user32 = ctypes.windll.user32
    seq = (sys.argv[sys.argv.index("--seq")+1].split(",")) if "--seq" in sys.argv else ["enter","down","down","down","up","esc"]
    end = time.time() + seconds
    started = time.time()
    hwnd = None; posts = 0; i = 0
    while time.time() < end:
        if psutil and not psutil.pid_exists(pid): print("  exited"); break
        el = (time.time()-started)*1000
        if el >= settle:
            if hwnd is None:
                hwnd = find_hwnd_for_pid(pid)
                if hwnd: print(f"  MASHED hwnd={hwnd}")
            if hwnd:
                vk = VK[seq[i % len(seq)]]; i += 1
                user32.PostMessageA(hwnd, WM_KEYDOWN, vk, 0)
                time.sleep(0.05)
                user32.PostMessageA(hwnd, WM_KEYUP, vk, 0xC0000001)
                posts += 1
                time.sleep(0.35); continue
        time.sleep(0.2)

    navc = {}; wm = {}
    try: navc = scr.exports_sync.counts()
    except Exception: pass
    try: wm = scr.exports_sync.wmkeydown()
    except Exception: pass
    print(f"\n  PostMessage pairs sent: {posts}")
    print(f"  WM_KEYDOWN dispatched by MASHED pump (observed): {wm}")
    print(f"  nav-handler counts (>0 == WM nav drove the menu): {navc}")
    try:
        if not psutil or psutil.pid_exists(pid): dev.kill(pid)
    except Exception: pass
    try: sess.detach()
    except Exception: pass
    return 0

if __name__ == "__main__":
    sys.exit(main())
