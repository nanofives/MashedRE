"""RWC-FIRST-FRAME-IM2D: capture the ORIGINAL's first loading-disc draw 0x00428450 at boot (ViewportInit 0x00428590's
call), with the Im2D traffic it makes through RwGlobals (DAT_007d3ff8) +0x20 (fpRenderStateSet) and +0x30
(fpIm2DRenderPrimitive), in the same text form the exe's [IM2D] lines use (Match/PortTop_00493710.cpp), so
compare_im2d.py reads both sides.

usage (inside mashed_lock.py run): py -3.12 verify/rwcim2d_20261113/orig_im2d_boot.py <root> <out.log> [timeout_s]
  <root>  a private root whose original/ holds the patched MASHED.exe (MASHED_RE_NO_AUTO_HOOK=1: the dev .asi installs
          no hook, the reference is stock)
Spawns MASHED.exe suspended, hooks 0x00428590 / 0x00428450 / 0x00450b10 (each fires once at boot; no hot path), resumes,
waits for the first 0x00428450 to return, kills ONLY the pid it spawned.
"""
import os
import sys
import time

import frida

AGENT = r"""
var log = [];
function L(s) { log.push(s); }
function u32(a) { return ptr(a).readU32(); }
function hex(v) { return ('00000000' + (v >>> 0).toString(16).toUpperCase()).slice(-8); }
var inDisc = false, done = false, attached = false;
function attachDev() {
  if (attached) return;
  attached = true;
  var g = ptr(u32(0x007d3ff8));
  var rs = g.add(0x20).readPointer(), prim = g.add(0x30).readPointer();
  L('[IM2D] device entries at ViewportInit: RwGlobals ' + hex(g.toUInt32()) + ' +0x20 ' + hex(rs.toUInt32()) +
    ' +0x30 ' + hex(prim.toUInt32()));
  Interceptor.attach(rs, {
    onEnter: function (a) { if (inDisc) { this.on = 1; this.s = a[0].toInt32(); this.v = a[1].toUInt32(); } },
    onLeave: function (r) { if (this.on) L('[IM2D] RenderStateSet(' + this.s + ', 0x' + hex(this.v) + ') = ' + r.toInt32()); }
  });
  Interceptor.attach(prim, {
    onEnter: function (a) {
      if (!inDisc) return;
      this.on = 1;
      var t = a[0].toInt32(), v = a[1], n = a[2].toInt32();
      L('[IM2D] Im2DRenderPrimitive(type ' + t + ', verts ' + hex(v.toUInt32()) + ', n ' + n + ')');
      for (var i = 0; i < n && i < 8; i++) {
        var b = v.add(i * 0x1c), h = [], f = [];
        for (var k = 0; k < 7; k++) { h.push(hex(b.add(4 * k).readU32())); f.push(b.add(4 * k).readFloat()); }
        L('[IM2D] v' + i + ' hex ' + h.join(' ') + ' = x ' + f[0] + ' y ' + f[1] + ' z ' + f[2] + ' rhw ' + f[3] +
          ' col ' + h[4] + ' u ' + f[5] + ' v ' + f[6]);
      }
    },
    onLeave: function (r) { if (this.on) L('[IM2D] Im2DRenderPrimitive = ' + r.toInt32()); }
  });
}
Interceptor.attach(ptr(0x00428590), { onEnter: function () { L('[IM2D] ViewportInit 0x00428590 entered'); attachDev(); } });
Interceptor.attach(ptr(0x00428450), {
  onEnter: function (a) {
    if (done) return;
    this.first = 1;
    attachDev();
    var ang = ptr(0x0067d974), t = u32(0x00771960);
    L('[IM2D] 0x00428450(' + a[0].toInt32() + ', ' + a[1].toInt32() + ') original: [0x0067d974] ' + hex(ang.readU32()) +
      ' (' + ang.readFloat() + '), [0x00771960] ' + hex(t) + ' -> tex ' + hex(t ? u32(t) : 0) + ', screen [0x0067ea54] ' +
      ptr(0x0067ea54).readU16() + ' x [0x0067ea56] ' + ptr(0x0067ea56).readU16() + ', RwGlobals ' + hex(u32(0x007d3ff8)));
    inDisc = true;
  },
  onLeave: function () {
    if (!this.first) return;
    inDisc = false; done = true;
    var ang = ptr(0x0067d974);
    L('[IM2D] 0x00428450 returned; [0x0067d974] ' + hex(ang.readU32()) + ' (' + ang.readFloat() + ')');
    var b = ptr(0x00898a20), h = [];
    for (var k = 0; k < 28; k++) h.push(hex(b.add(4 * k).readU32()));
    L('[IM2D] buffer 0x00898a20 after: ' + h.join(' '));
  }
});
Interceptor.attach(ptr(0x00450b10), {
  onEnter: function (a) {
    if (!inDisc) return;
    var uv = a[6];
    L('[IM2D] 0x00450b10(tex ' + hex(a[0].toUInt32()) + ', x ' + hex(a[1].toUInt32()) + ' y ' + hex(a[2].toUInt32()) +
      ' w ' + hex(a[3].toUInt32()) + ' h ' + hex(a[4].toUInt32()) + ', col ' + hex(a[5].toUInt32()) + ', uv ' +
      [0, 4, 8, 12].map(function (o) { return hex(uv.add(o).readU32()); }).join(' ') + ')');
  }
});
rpc.exports = { done: function () { return done; }, log: function () { return log; } };
"""


def main():
    root, out = sys.argv[1], sys.argv[2]
    tmo = float(sys.argv[3]) if len(sys.argv) > 3 else 60
    orig = os.path.join(root, 'original')
    env = dict(os.environ)
    env.setdefault('MASHED_RE_NO_AUTO_HOOK', '1')
    dev = frida.get_local_device()
    pid = dev.spawn(os.path.join(orig, 'MASHED.exe'), cwd=orig, env=env)
    print('spawned pid', pid, flush=True)
    try:
        sess = dev.attach(pid)
        scr = sess.create_script(AGENT)
        scr.load()
        dev.resume(pid)
        end = time.time() + tmo
        while time.time() < end and not scr.exports_sync.done():
            time.sleep(0.2)
        lines = scr.exports_sync.log()
        ok = scr.exports_sync.done()
    finally:
        try:
            dev.kill(pid)
            print('killed own pid', pid)
        except Exception as e:  # noqa: BLE001
            print('kill', pid, e)
    with open(out, 'w', encoding='utf-8', newline='\n') as f:
        f.write('\n'.join(lines) + '\n')
    print('%s: %d lines, first disc draw %s' % (out, len(lines), 'captured' if ok else 'NOT reached'))
    for ln in lines:
        print('  ' + ln)
    return 0 if ok else 1


if __name__ == '__main__':
    sys.exit(main())
