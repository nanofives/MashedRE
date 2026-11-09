"""fpu_probe.py --root <root> --out <json> [--asi <dev .asi with MenuAnimTickTwin>] [--rounds N]

PORT-STATE-03-FPU (PREREG_FPU.md): which float model the ORIGINAL runs 0x004325c0 at. One boot of
<root>/original/MASHED.exe (spawned pid only, killed by pid at the end):

  arm 1 (cw)   native onEnter stub on 0x004325c0 (no JS): x87 CW (FNSTCW), MXCSR, DAT_007f1004 bits, DAT_0067eca4
               per call, from the first frontend call through one screen push (0x0043d2a0(1, 0), the
               menu_anim_diff.py method).
  arm 2 (nat)  Interceptor.replaceFast on 0x004325c0: the original body called unchanged, records 0..29 at
               0x00898ac0 read before and after, CW read just before the call. Every 30th call reloads the current
               screen's records first (0x0043d2a0(screen, 2), on the game thread) so the records animate.
  arm 3 (ctl)  inside the original's own tick call: reload, force DAT_007f1004 (and optionally DAT_0067eca4 = 4 for
               speed 2), run the original body at the thread's CW and under a forced PC_53 word, then (with --asi)
               the port twin at both words, the table restored before each run. dt and phase restored after.
Writes raw JSON to --out (keep it in D:/MashedWT/<step>_data). Score with fpu_score.py.
"""
import argparse, json, os, struct, sys, time
from pathlib import Path
import frida

ap = argparse.ArgumentParser()
ap.add_argument('--root', required=True)
ap.add_argument('--out', required=True)
ap.add_argument('--asi', default='')
ap.add_argument('--rounds', type=int, default=4)
ap.add_argument('--natural', type=int, default=600)
a = ap.parse_args()
ORIG = Path(a.root) / 'original'
EXE = ORIG / 'MASHED.exe'


def fbits(x):
    return struct.unpack('<I', struct.pack('<f', x))[0]


# PREREG dts: 0.02f, 0.015f, 0.0145f, 0.01375f, control 0.017f, 0 = the call's own dt.
DTS = [0x3ca3d70a, 0x3c75c28f, 0x3c6d9168, 0x3c6147ae, 0x3c8b4396, 0]

AGENT = r'''
'use strict';
const TBL=0x00898ac0, TBL_LEN=30*0x34;
const RVA_TICK=0x004325c0, RVA_NAV=0x0043d2a0;
const RVA_DEPTH=0x0067e9f8, RVA_SCREEN=0x0067ed7c, RVA_PHASE=0x0067eca4, RVA_DT=0x007f1004;
let DELTA=0, nav=null, twin=null, cwGet=null, cwSet=null, KEEP=[];
function abs(r){ return ptr(r+DELTA); }
function le(n){ n=n>>>0; return [n&0xff,(n>>>8)&0xff,(n>>>16)&0xff,(n>>>24)&0xff]; }
function lep(p){ return le(parseInt(p.toString(),16)); }
const CW={buf:null,cnt:null,cap:0,stub:null};
function snap(){ return abs(TBL).readByteArray(TBL_LEN); }
function put(b){ abs(TBL).writeByteArray(b); }
function compact(b){ const d=new DataView(b), r=[];
  for(let k=0;k<30;k++){ const o=k*0x34; r.push([d.getUint32(o,true), d.getInt32(o+4,true), d.getUint32(o+8,true), d.getInt32(o+0x10,true)]); }
  return r; }
function curScreen(){ const depth=abs(RVA_DEPTH).readS32(); return abs(RVA_SCREEN+(depth-1)*0x40).readS32(); }
let mode='none', want=0, rows=[], callN=0, origPtr=null, dts=[], useTwin=false;
rpc.exports={
  init:function(asiPath){
    const m=Process.findModuleByName('MASHED.exe')||Process.enumerateModules()[0];
    DELTA=m.base.toUInt32()-0x00400000;
    nav=new NativeFunction(abs(RVA_NAV),'void',['int','int']);
    const st=Memory.alloc(Process.pageSize); KEEP.push(st);   // keep the page alive (GC frees Memory.alloc)
    // get: push eax; fnstcw [esp]; pop eax; movzx eax,ax; ret      set: fldcw [esp+4]; ret
    st.writeByteArray([0x50,0xD9,0x3C,0x24,0x58,0x0F,0xB7,0xC0,0xC3]);
    st.add(16).writeByteArray([0xD9,0x6C,0x24,0x04,0xC3]);
    Memory.protect(st,Process.pageSize,'rwx');
    cwGet=new NativeFunction(st,'uint',[]); cwSet=new NativeFunction(st.add(16),'void',['uint']);
    return [DELTA, cwGet()];
  },
  loadTwin:function(asiPath){
    const asi=Module.load(asiPath); const tw=asi.findExportByName('MenuAnimTickTwin');
    if(!tw) throw new Error('twin export missing'); twin=new NativeFunction(tw,'int',[]); useTwin=true; return tw.toString();
  },
  armCw:function(cap){
    CW.cap=cap; CW.buf=Memory.alloc(cap*16); CW.cnt=Memory.alloc(4); CW.cnt.writeU32(0);
    const code=[].concat([0xA1],lep(CW.cnt),[0x3D],le(cap),[0x73,0x29,0x89,0xC1,0xC1,0xE1,0x04,0x81,0xC1],lep(CW.buf),
      [0xD9,0x39,0x0F,0xAE,0x59,0x04,0x8B,0x15],lep(abs(RVA_DT)),[0x89,0x51,0x08,0x8B,0x15],lep(abs(RVA_PHASE)),
      [0x89,0x51,0x0C,0x40,0xA3],lep(CW.cnt),[0xC3]);
    CW.stub=Memory.alloc(Process.pageSize); CW.stub.writeByteArray(code); Memory.protect(CW.stub,Process.pageSize,'rwx');
    Interceptor.attach(abs(RVA_TICK),{onEnter:CW.stub});
    return code.length;
  },
  cwCount:function(){ return CW.cnt?CW.cnt.readU32():-1; },
  cwDrain:function(){ Interceptor.detachAll(); Interceptor.flush();
    const n=Math.min(CW.cnt.readU32(),CW.cap), r=[];
    for(let i=0;i<n;i++){ const p=CW.buf.add(i*16); r.push([p.readU16(),p.add(4).readU32(),p.add(8).readU32(),p.add(12).readS32()]); }
    return r; },
  phase:function(){ return abs(RVA_PHASE).readS32(); },
  push1:function(){ nav(1,0); return abs(RVA_DEPTH).readS32(); },
  arm:function(m,n,dtList){
    mode=m; want=n; rows=[]; callN=0; dts=dtList||[];
    origPtr=Interceptor.replaceFast(abs(RVA_TICK), new NativeCallback(function(){
      const orig=new NativeFunction(origPtr,'int',[]);
      if(rows.length>=want) return orig();
      callN++;
      if(mode==='nat'){
        if(callN%30===1) nav(curScreen(),2);
        const cw=cwGet(), dt=abs(RVA_DT).readU32(), ph=abs(RVA_PHASE).readS32();
        const s0=snap(); const ra=orig(); const s1=snap();
        rows.push({cw:cw, dt:dt, ph:ph, ra:ra, b:compact(s0), a:compact(s1)});
        return ra;
      }
      // ctl: one round = every dt x {own phase, phase 4}
      const dt0=abs(RVA_DT).readU32(), ph0=abs(RVA_PHASE).readS32(), cw0=cwGet();
      const pc53=(cw0&~0x300)|0x200, scr=curScreen();
      const out=[];
      for(const fp of [0,4]){
        for(const db of dts){
          nav(scr,2);
          const s0=snap();
          if(db) abs(RVA_DT).writeU32(db);
          if(fp) abs(RVA_PHASE).writeS32(fp);
          const r={dt:abs(RVA_DT).readU32(), ph:abs(RVA_PHASE).readS32(), cw:cw0, b:compact(s0)};
          r.o_own=compact((orig(),snap())); put(s0);
          cwSet(pc53); r.o_53=compact((orig(),snap())); cwSet(cw0); put(s0);
          if(useTwin){
            r.t_own=compact((twin(),snap())); put(s0);
            cwSet(pc53); r.t_53=compact((twin(),snap())); cwSet(cw0); put(s0);
          }
          r.cw_after=cwGet();
          abs(RVA_DT).writeU32(dt0); abs(RVA_PHASE).writeS32(ph0);
          out.push(r);
        }
      }
      rows.push({scr:scr, samples:out});
      return orig();
    },'int',[]));
    return 1;
  },
  done:function(){ return rows.length; },
  take:function(){ Interceptor.revert(abs(RVA_TICK)); Interceptor.flush(); const r=JSON.stringify(rows); rows=[]; return r; }
};
'''


def wait(cond, t):
    end = time.time() + t
    while time.time() < end and not cond():
        time.sleep(0.2)
    return cond()


def main():
    env = dict(os.environ)
    for k in list(env):
        if k.startswith('MASHED_'):
            env.pop(k)
    env.update({'MASHED_RE_NO_AUTO_HOOK': '1', 'MASHED_MUTE': '1'})
    dev = frida.get_local_device()
    pid = dev.spawn(str(EXE), cwd=str(ORIG), env=env)
    print(f'spawned pid={pid}', flush=True)
    res = {'pid': pid, 'exe': str(EXE), 'asi': a.asi}
    try:
        sess = dev.attach(pid)
        scr = sess.create_script(AGENT)
        scr.on('message', lambda m, d: print('msg', m, flush=True))
        scr.load()
        E = scr.exports_sync
        print('init [delta, JS-thread cw]', E.init(a.asi), flush=True)
        print('cw stub bytes', E.arm_cw(20000), flush=True)
        dev.resume(pid)
        ok = wait(lambda: E.phase() == 3, 40)
        print('menu phase 3:', ok, 'cw calls so far', E.cw_count(), flush=True)
        time.sleep(1.5)
        print('push1 depth ->', E.push1(), flush=True)
        time.sleep(4)
        res['cw'] = E.cw_drain()
        print('arm1 cw rows', len(res['cw']), 'distinct cw', sorted({hex(r[0]) for r in res['cw']}), flush=True)
        E.arm('nat', a.natural, [])
        wait(lambda: E.done() >= a.natural, 60)
        res['nat'] = json.loads(E.take())
        print('arm2 nat calls', len(res['nat']), flush=True)
        if a.asi:
            print('twin at', E.load_twin(a.asi), flush=True)
        E.arm('ctl', a.rounds, DTS)
        wait(lambda: E.done() >= a.rounds, 60)
        res['ctl'] = json.loads(E.take())
        print('arm3 ctl rounds', len(res['ctl']), flush=True)
    finally:
        Path(a.out).write_text(json.dumps(res))
        print('->', a.out, flush=True)
        try:
            dev.kill(pid)
            print(f'killed own pid {pid}', flush=True)
        except Exception as e:
            print('kill', e, flush=True)
    return 0


if __name__ == '__main__':
    sys.exit(main())
