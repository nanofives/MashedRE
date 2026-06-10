# Attach to a running MASHED.exe (user is parked on a screen that shows locks) and
# record who draws the "lock" sprite vs "check"/others. The sprite-by-name draw
# helpers are:
#   FUN_0040bb50(name, f,f,f,f, color, flag)   - cdecl, name = [esp+4]
#   FUN_0040bb90(name)                          - name = [esp+4]
# For each call we capture (name, caller return address) so we can see exactly
# which renderer/branch is choosing "lock" for the current screen. Runs ~3s then
# reports distinct (name, caller) pairs with counts.
import sys, time, frida
from collections import Counter

JS = r'''
var seen = {};
function nameAt(p){ try { return p.readUtf8String(); } catch(e){ try { return p.readAnsiString(); } catch(e2){ return '??'; } } }
['0x40bb50','0x40bb90'].forEach(function(a){
  try {
    Interceptor.attach(ptr(a), {
      onEnter: function(args){
        var nm = nameAt(args[0]);
        if (nm === null) nm = '<null>';
        var caller = this.returnAddress;
        var key = a + '|' + nm + '|' + caller;
        seen[key] = (seen[key]||0)+1;
      }
    });
  } catch(e){}
});
rpc.exports = { grab: function(){ return seen; } };
'''

def main():
    dev = frida.get_local_device()
    pid = None
    for p in dev.enumerate_processes():
        if p.name.lower()=='mashed.exe': pid=p.pid; break
    if not pid: sys.exit("MASHED.exe not running")
    print("attaching", pid)
    s = dev.attach(pid)
    scr = s.create_script(JS); scr.load()
    time.sleep(3.0)
    d = scr.exports_sync.grab()
    s.detach()
    rows = []
    for k,c in d.items():
        fn,nm,caller = k.split('|',2)
        rows.append((c,fn,nm,caller))
    rows.sort(reverse=True)
    print("calls fn         name            caller")
    for c,fn,nm,caller in rows:
        print(f"{c:5d} {fn:10s} {nm:15s} {caller}")
    locks = [r for r in rows if 'lock' in (r[2] or '').lower()]
    print("\n--- LOCK draws ---")
    for c,fn,nm,caller in locks:
        print(f"{c:5d} {fn:10s} {nm:15s} caller={caller}")
    if not locks:
        print("(no 'lock' sprite drawn during the sample window)")

if __name__=='__main__':
    main()
