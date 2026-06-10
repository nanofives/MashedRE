# One-shot: attach to a running MASHED.exe and dump the unlock-state globals
# so we can see what the menu actually reads. Reads:
#   0x0042ef40 / 0x00430830  - patched getter prologues (confirm patch is live)
#   0x007f0a40  - championship/track table (156 int32; 0=locked, nonzero=available)
#   0x007f0e50  - car unlock byte array (stride 0xc; 1=unlocked)
import sys, time, frida

JS = r'''
function hexbytes(addr, n) {
  var p = ptr(addr); var out = [];
  for (var i=0;i<n;i++){ out.push(('0'+p.add(i).readU8().toString(16)).slice(-2)); }
  return out.join(' ');
}
function i32arr(addr, n) {
  var p = ptr(addr); var out = [];
  for (var i=0;i<n;i++){ out.push(p.add(i*4).readS32()); }
  return out;
}
rpc.exports = {
  dump: function() {
    return {
      get_42ef40: hexbytes(0x42ef40, 8),
      get_430830: hexbytes(0x430830, 8),
      track_7f0a40: i32arr(0x7f0a40, 156),
      car_7f0e50: hexbytes(0x7f0e50, 156),
      flag_7f0f2c: ptr(0x7f0f2c).readS32()
    };
  }
};
'''

def main():
    dev = frida.get_local_device()
    pid = None
    for _ in range(50):
        for p in dev.enumerate_processes():
            if p.name.lower() == 'mashed.exe':
                pid = p.pid; break
        if pid: break
        time.sleep(0.1)
    if not pid:
        sys.exit("MASHED.exe not running")
    print("attaching pid", pid)
    s = dev.attach(pid)
    scr = s.create_script(JS)
    scr.load()
    d = scr.exports_sync.dump()
    print("getter 0x42ef40 :", d['get_42ef40'])
    print("getter 0x430830 :", d['get_430830'])
    print("flag 0x7f0f2c   :", d['flag_7f0f2c'])
    t = d['track_7f0a40']
    print("track table 0x7f0a40 (156 i32):")
    for r in range(13):
        print("  row %2d:" % r, t[r*12:(r+1)*12])
    from collections import Counter
    print("track value histogram:", dict(Counter(t)))
    print("car bytes 0x7f0e50:", d['car_7f0e50'])
    s.detach()

if __name__ == '__main__':
    main()
