"""fpu_exe_probe.py <mashed_re.exe> <root> <out.json> <script: keys|walk>

PORT-STATE-03-FPU arm 4 (PREREG_FPU.md): native (no JS) onEnter/onLeave stubs on the MenuAnimTickTwin export of the
standalone (the matched 0x004325c0 body). Per call: x87 CW, MXCSR, DAT_007f1004 bits, DAT_0067eca4, and the 30 x 0x34
records at 0x00898ac0 at entry and at exit (the AnimTickScope puts them there for the call). Runs the PORT-STATE-03
NAV_KEYS scripts (key script or multi-screen walk) on a private MASHED_ROOT; spawned pid only. The exe's mashed_re.log
is copied next to the JSON so the NAV_KEYS log can be checked against the reference hashes.
"""
import base64, json, os, shutil, struct, sys, time
from pathlib import Path
import frida

exe, root, out, which = sys.argv[1], Path(sys.argv[2]), Path(sys.argv[3]), sys.argv[4]
SCRIPTS = {'keys': {'MASHED_GOTO': '1,2,4,6', 'MASHED_NAV_KEYS': 'd,u,d,u,e'},
           'walk': {'MASHED_GOTO': '1', 'MASHED_NAV_KEYS': 'e,d,e,b,b,d,e,d,e,b,b,d,d,e,b,b,u,u,u'}}
CAP, TBL_LEN = 2000, 30 * 0x34
ROW = 16 + 2 * TBL_LEN

AGENT = r'''
'use strict';
const CAP=%d, ROW=%d, TBL=0x00898ac0, DT=0x007f1004, PH=0x0067eca4;
function le(n){ n=n>>>0; return [n&0xff,(n>>>8)&0xff,(n>>>16)&0xff,(n>>>24)&0xff]; }
function lep(p){ return le(parseInt(p.toString(),16)); }
let buf=null, cnt=null, KEEP=[];
rpc.exports={
  arm:function(){
    const m=Process.enumerateModules()[0];
    const fn=m.findExportByName('MenuAnimTickTwin'); if(!fn) throw new Error('no MenuAnimTickTwin export');
    buf=Memory.alloc(CAP*ROW); cnt=Memory.alloc(4); cnt.writeU32(0);
    const head=[0x56,0x57,0xA1].concat(lep(cnt),[0x3D],le(CAP));
    const tail=[0x5F,0x5E,0xC3];
    const eb=[0x69,0xC0].concat(le(ROW),[0x8D,0xB8],lep(buf),[0xD9,0x3F,0x0F,0xAE,0x5F,0x04,0x8B,0x15],le(DT),
      [0x89,0x57,0x08,0x8B,0x15],le(PH),[0x89,0x57,0x0C,0x83,0xC7,0x10,0xBE],le(TBL),[0xB9],le(TBL_LEN()/4),[0xFC,0xF3,0xA5]);
    const lb=[0x69,0xC0].concat(le(ROW),[0x8D,0xB8],lep(buf.add(16+TBL_LEN())),[0xBE],le(TBL),[0xB9],le(TBL_LEN()/4),
      [0xFC,0xF3,0xA5,0xFF,0x05],lep(cnt));
    const enter=head.concat([0x73,eb.length],eb,tail), leave=head.concat([0x73,lb.length],lb,tail);
    const st=Memory.alloc(Process.pageSize); KEEP.push(st); st.writeByteArray(enter); st.add(256).writeByteArray(leave);
    Memory.protect(st,Process.pageSize,'rwx');
    Interceptor.attach(fn,{onEnter:st,onLeave:st.add(256)});
    return fn.toString();
  },
  count:function(){ return cnt.readU32(); },
  rows:function(a,b){ return buf.add(a*ROW).readByteArray((b-a)*ROW); }
};
function TBL_LEN(){ return 30*0x34; }
''' % (CAP, ROW)


def main():
    env = dict(os.environ)
    for k in list(env):
        if k.startswith('MASHED_'):
            env.pop(k)
    env.update({'MASHED_ROOT': str(root), 'MASHED_MUTE': '1', 'MASHED_WIN_POS': 'primary-bl',
                'MASHED_SETTINGS_DEFAULTS': '1'})
    env.update(SCRIPTS[which])
    for lf in root.glob('*.log'):
        lf.unlink()
    dev = frida.get_local_device()
    pid = dev.spawn(str(Path(exe).resolve()), cwd=str(root), env=env)
    print(f'spawned pid={pid} {which}', flush=True)
    sess = dev.attach(pid)
    gone = []
    sess.on('detached', lambda *x: gone.append(x))
    scr = sess.create_script(AGENT)
    scr.load()
    E = scr.exports_sync
    print('twin at', E.arm(), flush=True)
    dev.resume(pid)
    got = bytearray()
    n = 0
    t0 = time.time()
    while not gone and time.time() - t0 < 300:
        try:
            c = E.count()
            if c > n:
                got += E.rows(n, c)
                n = c
        except Exception:
            break
        time.sleep(0.1)
    if not gone:
        print(f'TIMEOUT - killing own pid {pid}', flush=True)
        dev.kill(pid)
    rows = []
    for i in range(n):
        r = got[i * ROW:(i + 1) * ROW]
        cw, = struct.unpack_from('<H', r, 0)
        mx, dt, ph = struct.unpack_from('<IIi', r, 4)
        def comp(off):
            return [list(struct.unpack_from('<IiI', r, off + k * 0x34)) + [struct.unpack_from('<i', r, off + k * 0x34 + 0x10)[0]]
                    for k in range(30)]
        rows.append({'cw': cw, 'mx': mx, 'dt': dt, 'ph': ph, 'b': comp(16), 'a': comp(16 + TBL_LEN)})
    out.write_text(json.dumps({'exe': exe, 'script': which, 'pid': pid, 'nat': rows,
                               'cw': [[r['cw'], r['mx'], r['dt'], r['ph']] for r in rows]}))
    for lf in root.glob('*.log'):
        shutil.copy(lf, out.with_suffix('.' + lf.name))
    print(f'calls captured {n} -> {out}', flush=True)


if __name__ == '__main__':
    main()
