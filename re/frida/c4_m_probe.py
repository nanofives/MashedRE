# Reachability probe for the C4 group M rows (frontend menu). Observation only: entry/leave
# interceptors on the ORIGINAL entries of the six RVAs, no hook installed, no writes except the
# FUN_00497310 return override that statenav.py already uses to press controls.
#
# Rows: GameStateSlotsFill 0x00429aa0, MenuButtonDetectA 0x0042aff0, MenuButtonDetectB 0x0042b180,
#       MenuButtonDetectC 0x0042b310, TimerSlotTickDispatcher 0x0043c000, FrontendPlayerSlotCheck 0x0042ebe0.
# Records, per function, call counts keyed by (screen type, depth, sel 0x7f1a0c, sub, ret), and the
# inputs that decide whether each row's old defect is observable live.
#
# Usage: py -3.12 re/frida/c4_m_probe.py [--plan qb|modes] [--out DIR] [--tag NAME]
import json, os, sys, time
from pathlib import Path
import frida
try: import psutil
except ImportError: psutil = None

ROOT = Path(__file__).resolve().parent.parent.parent
sys.path.insert(0, str(ROOT / "re" / "frida"))
import statenav
from statenav import Nav, _open_handle, _exit_code

EXE = statenav.EXE
ORIG = statenav.ORIG

AGENT = r'''
'use strict';
const IMG=0x00400000; let DELTA=0;
let navCtrl=-1, navUntil=0, navPlayer=0;
const SLOTS=[0x67e7a8,0x67e7b0,0x67e7b8,0x67e7c0,0x67e7c8,0x67e7d0,0x67e7d8,0x67e7e0,0x67e7e8,0x67e7f0,
             0x67e7f8,0x67e800,0x67e808,0x67e810,0x67e818,0x67e820,0x67e828,0x67e830,0x67e838];
function abs(r){ return ptr((r+DELTA)>>>0); }
function u8(a){ return abs(a).readU8(); }
function s32(a){ return abs(a).readS32(); }
function f32(a){ return abs(a).readFloat(); }
function scr(){ const d=s32(0x0067e9f8); return {d:d, t:s32(0x0067ed3c+d*0x40), sub:s32(0x0067ed40+d*0x40)}; }
const C={}; const K={}; const EV=[]; let curT=-999;
function bump(fn,key){ C[fn]=(C[fn]||0)+1; const k=fn+'|'+key; K[k]=(K[k]||0)+1; }
const T43={calls:0, st1:{}, st2:{}, padNZ:{}, xs:{}, x0:0};
const T429={calls:0, keys:{}};
const T42e={calls:0, keys:{}};
function att(rva, fn, mk){
  Interceptor.attach(abs(rva), { onEnter(a){ try{ this.s=scr(); this.e = mk ? mk.enter(this) : null; }catch(e){ this.s=null; } },
    onLeave(r){ const ret=r.toInt32(); const s=this.s; if(!s) return;
      let extra=''; try{ if (mk && mk.leave) extra = mk.leave(this, ret); }catch(e){ extra='ERR'; }
      bump(fn, 't'+s.t+'@'+s.d+'|sel'+s32(0x007f1a0c).toString(16)+'|sub'+s.sub+'|r'+ret+(extra?('|'+extra):'')); }});
}
rpc.exports={
  init:function(){ const m=Process.findModuleByName('MASHED.exe')||Process.enumerateModules()[0];
    DELTA=m.base.toUInt32()-IMG;
    Interceptor.attach(abs(0x00497310),{ onEnter(a){const sp=this.context.esp;this.p=sp.add(4).readS32();this.c=sp.add(8).readS32();},
      onLeave(ret){ if (this.p===navPlayer && this.c===navCtrl && Date.now()<navUntil) ret.replace(ptr(0xff)); }});
    att(0x0042aff0,'A',{enter(t){ return null; }, leave(t,r){ return 'h'+[0,1,2,3,4,5,6,7].map(k=>u8(0x7f1046+k*0x4c)?1:0).join('')+'p'+[0,1,2,3,4,5,6,7].map(k=>u8(0x7f1506+k*0x4c)?1:0).join(''); }});
    att(0x0042b180,'B',{enter(t){ return null; }, leave(t,r){ return 'h'+[0,1,2,3,4,5,6,7].map(k=>u8(0x7f1047+k*0x4c)?1:0).join(''); }});
    att(0x0042b310,'C',{enter(t){ return null; }, leave(t,r){ return 'h'+[0,1,2,3,4,5,6,7].map(k=>u8(0x7f1044+k*0x4c)?1:0).join(''); }});
    att(0x00429aa0,'G',{enter(t){ return {ed6c:s32(0x0067ed6c), idx:s32(0x0067f17c)}; },
      leave(t,r){ return 'ed6c'+t.e.ed6c+'|idx'+t.e.idx+'|o'+s32(0x0067d990)+','+s32(0x0067d998)+','+f32(0x0067d9a0); }});
    att(0x0042ebe0,'F',{enter(t){ return {p:t.context.esp.add(4).readS32()}; },
      leave(t,r){ return 'p'+t.e.p; }});
    Interceptor.attach(abs(0x0043c000),{ onEnter(){ T43.calls++; const x=f32(0x0067e840);
        if (x===0) T43.x0++; const xk=(Math.round(x*100)/100).toString(); T43.xs[xk]=(T43.xs[xk]||0)+1;
        const s=scr(); const tk='t'+s.t;
        for (const S of SLOTS){ const v=s32(S);
          if (v===1){ const k=S.toString(16)+'@'+tk; T43.st1[k]=(T43.st1[k]||0)+1; }
          if (v===2){ const k=S.toString(16)+'@'+tk; T43.st2[k]=(T43.st2[k]||0)+1; }
          const pad=(u8(S+5)|u8(S+6)|u8(S+7)); if (pad){ const k=S.toString(16); T43.padNZ[k]=(T43.padNZ[k]||0)+1; } }
        if (s.t!==curT){ curT=s.t; if (EV.length<400) EV.push({t:Date.now(), type:s.t, d:s.d}); } }});
    return DELTA; },
  press:function(c,ms){ navPlayer=0; navCtrl=c; navUntil=Date.now()+ms; return 1; },
  pressp:function(p,c,ms){ navPlayer=p; navCtrl=c; navUntil=Date.now()+ms; return 1; },
  depth:function(){ try{return s32(0x0067e9f8);}catch(e){return -999;} },
  phase:function(){ try{return s32(0x0067eca4);}catch(e){return -999;} },
  state:function(){ const s=scr(); return {d:s.d, t:s.t, sub:s.sub, ph:s32(0x0067eca4), sel:s32(0x007f1a0c), sph:u8(0x00771968), fsel:s32(0x0067f17c), ed6c:s32(0x0067ed6c), gm:s32(0x0067e9fc), col:s32(0x0067f184), eac4:u8(0x0067eac4), eabc:s32(0x0067eabc), eab0:s32(0x0067eab0), C:C}; },
  table:function(){ const o=[]; for (let i=0;i<0x80;i++) o.push(s32(0x007f0a40+i*4)); return o; },
  dump:function(){ return {C:C, K:K, T43:T43, EV:EV}; }
};
Process.setExceptionHandler(function (d) {
  if (d.type !== 'access-violation') return false;
  try { const pc=d.context.pc; const m=Process.findModuleByAddress(pc);
    send({kind:'crash', pc:pc.toString(), module: m ? (m.name+'+0x'+pc.sub(m.base).toUInt32().toString(16)) : 'UNMAPPED'}); } catch(e){}
  return false;
});
'''


def main():
    av = sys.argv
    def opt(f, d): return av[av.index(f) + 1] if f in av else d
    plan = opt("--plan", "qb")
    out = Path(opt("--out", str(ROOT / "log" / "c4_m"))); out.mkdir(parents=True, exist_ok=True)
    tag = opt("--tag", "probe_" + plan)
    env = dict(os.environ); env["MASHED_RE_NO_AUTO_HOOK"] = "1"; env["MASHED_HOOK_ONLY"] = "__none__"
    rec = {"plan": plan, "log": [], "crash": []}
    def log(m): print(m, flush=True); rec["log"].append(m)
    dev = frida.get_local_device()
    pid = dev.spawn(str(EXE), cwd=str(ORIG), env=env); rec["pid"] = pid; log(f"  spawned pid={pid}")
    sess = dev.attach(pid); hproc = _open_handle(pid)
    def on_msg(m, d):
        p = m.get("payload") if isinstance(m, dict) else None
        if isinstance(p, dict) and p.get("kind") == "crash": rec["crash"].append(p); print("  [CRASH]", p, flush=True)
        elif m.get("type") == "error": print("  agent err:", m.get("description"), flush=True)
    sc = sess.create_script(AGENT); sc.on("message", on_msg); sc.load(); X = sc.exports_sync
    X.init(); dev.resume(pid)
    nav = Nav(sc, pid)
    def st(msg): s = X.state(); log(f"  {msg}: d={s['d']} t={s['t']} sub={s['sub']} ph={s['ph']} sel={s['sel']:#x} sph={s['sph']} fsel={s['fsel']} ed6c={s['ed6c']} gm={s['gm']} col={s['col']} eac4={s['eac4']} eabc={s['eabc']:#x} eab0={s['eab0']} C={s['C']}")
    try:
        nav.wait(lambda: nav.phase() == 3 and nav.depth() >= 1, 25.0, "title up"); st("title")
        rec["table_title"] = X.table()
        time.sleep(2.0); st("title+2s")
        nav.confirm_to_depth(2); st("after GTS")
        if plan == "gts":
            kk = int(opt("--k", "1")); time.sleep(1.0)
            for j in range(kk):
                nav.press(12); time.sleep(0.6)
            st(f"gts cursor {kk}")
            G0 = 0
            for j in range(10):
                if nav.phase() != 3: break
                nav.press(4); time.sleep(1.8); st(f"gts{kk} confirm {j}")
            end = time.time() + 200; t_p = 0; t_l = 0
            while time.time() < end and nav.alive():
                s = X.state()
                if s["sph"] == 3 and time.time() - t_p > 1.0: X.press(4, 200); t_p = time.time()
                if time.time() - t_l > 15: t_l = time.time(); st("race")
                if s["ph"] == 3: break
                time.sleep(0.25)
            st("back in frontend"); time.sleep(3); st("fe+3")
            for k in range(4):
                nav.press(4); time.sleep(2.0); st(f"post confirm {k}")
            rec["dump"] = X.dump(); raise SystemExit
        nav.advance_past_load_modal(3); st("single player")
        rec["table_sp"] = X.table()
        if plan == "challcol":
            nav.confirm_to_depth(4, tries=4); st("chall colour")
            nav.confirm_to_depth(5, tries=4); st("chall select"); time.sleep(1.0)
            seq = opt("--seq", "11,11,11,12,12,12,12,11,11")
            for c in [int(x) for x in seq.split(",")]:
                nav.press(c); time.sleep(0.6); st(f"press {c}")
            nav.press(4); time.sleep(1.5); st("chall confirm")
            for k in range(6):
                if nav.phase() != 3: break
                nav.press(4); time.sleep(1.2); st(f"chall start {k}")
            end = time.time() + 30
            while time.time() < end and nav.alive(): time.sleep(1)
            st("race+30")
            rec["dump"] = X.dump(); raise SystemExit
        if plan == "mode":
            kk = int(opt("--k", "2"))
            for j in range(kk):
                nav.press(12); time.sleep(0.5)
            st(f"mode cursor {kk}")
            for j in range(8):
                if nav.phase() != 3: break
                nav.press(4); time.sleep(1.6); st(f"mode{kk} confirm {j}")
            end = time.time() + 200; t_p = 0; t_l = 0
            while time.time() < end and nav.alive():
                s = X.state()
                if s["sph"] == 3 and time.time() - t_p > 1.0: X.press(4, 200); t_p = time.time()
                if time.time() - t_l > 15: t_l = time.time(); st("race")
                if s["ph"] == 3: break
                time.sleep(0.25)
            st("back in frontend"); time.sleep(3); st("fe+3")
            for k in range(4):
                nav.press(4); time.sleep(2.0); st(f"post confirm {k}")
        elif plan == "chall":
            # mode list cursor 0 = Challenge Cup (nav_champ_probe.py challlaunch), no down-press
            nav.confirm_to_depth(4, tries=4); st("chall colour")
            nav.confirm_to_depth(5, tries=4); st("chall select")
            time.sleep(1.0); st("chall select+1")
            nav.press(4); time.sleep(1.5); st("chall confirm")
            for k in range(6):
                if nav.phase() != 3: break
                nav.press(4); time.sleep(1.2); st(f"chall start {k}")
            end = time.time() + 200; t_p = 0; t_l = 0
            while time.time() < end and nav.alive():
                s = X.state()
                if s["sph"] == 3 and time.time() - t_p > 1.0: X.press(4, 200); t_p = time.time()
                if time.time() - t_l > 15: t_l = time.time(); st("race")
                if s["ph"] == 3: break
                time.sleep(0.25)
            st("back in frontend"); time.sleep(3); st("fe+3")
            for k in range(6):
                nav.press(4); time.sleep(2.0); st(f"post confirm {k}")
        elif plan == "modes":
            # walk the mode list: down x k then confirm, look, back
            for k in range(6):
                nav.press(4); time.sleep(1.5); st(f"mode{k} confirm")
                for j in range(3):
                    nav.press(12); time.sleep(0.6); st(f"mode{k} down{j}")
                nav.press(4); time.sleep(1.5); st(f"mode{k} confirm2")
                nav.press(5); time.sleep(1.2); st(f"mode{k} back")
                nav.press(5); time.sleep(1.2); st(f"mode{k} back2")
                for j in range(k + 1):
                    nav.press(12); time.sleep(0.5)
                st(f"next mode {k+1}")
        else:
            nav.press(12); st("mode sel (QB)")
            nav.confirm_to_depth(4, tries=4); st("colour-select")
            time.sleep(2.0); st("colour+2s")
            nav.confirm_to_depth(5, tries=4); st("track-select")
            time.sleep(1.0)
            nav.press(4); time.sleep(1.5); st("after track confirm")
            for k in range(5):
                if nav.phase() != 3: break
                nav.press(4); time.sleep(1.5); st(f"start-attempt {k}")
                if nav.phase() == 0: break
            end = time.time() + 200; t_p = 0; t_l = 0
            while time.time() < end and nav.alive():
                s = X.state()
                if s["sph"] == 3 and time.time() - t_p > 1.0: X.press(4, 200); t_p = time.time()
                if time.time() - t_l > 15: t_l = time.time(); st("race")
                if s["ph"] == 3 and s["t"] == 5: break
                time.sleep(0.25)
            st("results?")
            time.sleep(3); st("results+3")
            for k in range(3):
                nav.press(4)
                for j in range(20): time.sleep(0.1); st(f"post-results confirm {k}.{j}")
        rec["dump"] = X.dump()
    except SystemExit:
        pass
    except Exception as e:
        log(f"  EXC {e!r}")
        try: rec["dump"] = X.dump()
        except Exception: pass
    finally:
        rec["alive_at_end"] = nav.alive()
        try:
            if nav.alive(): dev.kill(pid)
        except Exception: pass
        try: sess.detach()
        except Exception: pass
        rec["exit_code"] = _exit_code(hproc)
        p = out / f"c4_m_{tag}.json"; p.write_text(json.dumps(rec, indent=1), encoding="utf-8"); print("->", p)


if __name__ == "__main__":
    sys.exit(main())
