# Live proof test: poll for MASHED.exe, attach, and CONTINUOUSLY force the working
# arrays the menu reads each frame to unlocked values, for ~120s:
#   0x007f0a40  track/cup table  -> 156 int32 = VALUE_T (default 2)
#   0x007f0e50  car unlock flags -> 156 bytes  = 1
# Re-pokes every 80ms so even the save-restore (which zeroes them on mode-entry) is
# immediately overwritten. Launch the game, walk to the Challenge/Car screens, and the
# padlocks should be gone if these are the right arrays.
import sys, time, frida

VALUE_T = int(sys.argv[1]) if len(sys.argv) > 1 else 2
HOLD_S  = int(sys.argv[2]) if len(sys.argv) > 2 else 120

JS = '''
var vt = %d;
function poke(){
  try {
    var t = ptr(0x7f0a40);
    for (var i=0;i<156;i++) t.add(i*4).writeS32(vt);
    var c = ptr(0x7f0e50);
    for (var j=0;j<156;j++) c.add(j).writeU8(1);
  } catch(e){}
}
setInterval(poke, 80);
''' % VALUE_T

dev = frida.get_local_device()
print("waiting for MASHED.exe (launch it now) | forcing 0x7f0a40=%d, 0x7f0e50=1" % VALUE_T)
deadline = time.time() + 120
pid = None
while time.time() < deadline:
    pid = next((p.pid for p in dev.enumerate_processes() if p.name.lower()=='mashed.exe'), None)
    if pid: break
    time.sleep(0.2)
if not pid:
    sys.exit("timeout: MASHED.exe never appeared")
print("attached pid", pid, "- forcing arrays every 80ms for", HOLD_S, "s. Go to Challenge/Car select and look.")
s = dev.attach(pid)
s.create_script(JS).load()
t_end = time.time() + HOLD_S
while time.time() < t_end:
    if not any(p.pid==pid for p in dev.enumerate_processes()):
        print("MASHED exited."); break
    time.sleep(1.0)
try: s.detach()
except Exception: pass
print("done holding.")
