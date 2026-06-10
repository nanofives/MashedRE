# Verify the permanent restore patch without navigating: zero the working arrays,
# then CALL the patched FUN_00404e80 (save-state restore) directly. If the patch works,
# the arrays come back unlocked (0x7f0a40 all=2, 0x7f0e50 all=1) even though the restore
# copies from the (zero) save snapshot.
import sys, time, frida

JS = r'''
function zero(){
  var t=ptr(0x7f0a40); for(var i=0;i<156;i++) t.add(i*4).writeS32(0);
  var c=ptr(0x7f0e50); for(var j=0;j<156;j++) c.add(j).writeU8(0);
}
function row0(){ var t=ptr(0x7f0a40); var r=[]; for(var k=0;k<12;k++) r.push(t.add(k*4).readS32()); return r; }
function car0(){ var c=ptr(0x7f0e50); var r=[]; for(var k=0;k<6;k++) r.push(c.add(k).readU8()); return r; }
var restore = new NativeFunction(ptr(0x404e80), 'void', []);
rpc.exports = {
  run: function(){
    zero();
    var before = {row0: row0(), car0: car0()};
    restore();
    var after  = {row0: row0(), car0: car0()};
    var t=ptr(0x7f0a40); var allset=true; for(var i=0;i<156;i++){ if(t.add(i*4).readS32()!==2){allset=false;break;} }
    var c=ptr(0x7f0e50); var carset=true; for(var j=0;j<156;j++){ if(c.add(j).readU8()!==1){carset=false;break;} }
    return {before:before, after:after, tracks_all_2:allset, cars_all_1:carset};
  }
};
'''

dev = frida.get_local_device()
deadline = time.time()+30; pid=None
while time.time()<deadline:
    pid = next((p.pid for p in dev.enumerate_processes() if p.name.lower()=='mashed.exe'), None)
    if pid: break
    time.sleep(0.2)
if not pid: sys.exit("MASHED.exe not running")
print("attached", pid)
s = dev.attach(pid)
scr = s.create_script(JS); scr.load()
r = scr.exports_sync.run()
print("before zero->restore  row0:", r['before']['row0'], "car0:", r['before']['car0'])
print("after  restore        row0:", r['after']['row0'],  "car0:", r['after']['car0'])
print("RESULT  tracks_all_2 =", r['tracks_all_2'], " cars_all_1 =", r['cars_all_1'])
print("PATCH WORKS" if (r['tracks_all_2'] and r['cars_all_1']) else "PATCH DID NOT PRODUCE UNLOCKED STATE")
s.detach()
