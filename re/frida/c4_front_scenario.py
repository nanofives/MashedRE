# Canonical-scenario C4 harness for the four C4-FRONT rows (2026-10-12):
#   bc      0x0042f8d0 MenuMenusBC        natural Race Results (statenav Quick Battle, c4_detect_e route)
#   subinit 0x00492270 SubsystemInit      boot, called once from FUN_00492370 (0x00492370 -> 0x00492270)
#   bqf     0x004026d0 FUN_004026d0       boot, called once as FUN_004026d0(4) at the tail of FUN_00402750
#   pmr     0x00404320 PerModeRenderMachine  per race frame from FUN_00492e90 (case 3/7); records the
#           rule 0x007f0fd0 / sub-state 0x0063ba8c it is called with (its arms need rule 5/8/9/10)
#
# Why not the c4_detect_e / c4_hud 'on' arm: the dinput8 shim autoloads original\mashed_re_dev.asi,
# the shared deployed build. This harness loads a GIVEN .asi (the worktree build or a control) at
# SPAWN, before the first instruction of MASHED runs, so the two boot rows execute through the
# installed JMP. Same Nav / statenav navigation and crash handler as c4_detect_e_scenario.py.
#
# Arms (one fresh MASHED per run, own PID only, killed by PID):
#   stock : MASHED_RE_NO_AUTO_HOOK=1, nothing loaded. Instrumented at the original entry.
#   port  : MASHED_RE_NO_AUTO_HOOK=1, --asi Module.load'ed while suspended + InjectHooks with
#           MASHED_HOOK_ONLY=<rva>. Instrumented at the E9 target.
#   ctl   : as port, with the old-defect control .asi.
# Hook-live witness: the 8 bytes at the RVA are read right after InjectHooks, before any Interceptor
# is attached; the rel32 target is resolved to module + offset + export name.
#
# Usage: py -3.12 re/frida/c4_front_scenario.py --row bc|subinit|bqf|pmr --arm stock|port|ctl
#        [--asi PATH] [--out DIR] [--tag NAME] [--round 240] [--window 2.0]
import json, os, sys, time, hashlib
from pathlib import Path
import frida

ROOT = Path(__file__).resolve().parent.parent.parent
sys.path.insert(0, str(ROOT / "re" / "frida"))
import statenav
from statenav import Nav, shoot, _open_handle, _exit_code

EXE = statenav.EXE
ORIG = statenav.ORIG
ROWS = {"bc": 0x0042f8d0, "subinit": 0x00492270, "bqf": 0x004026d0, "pmr": 0x00404320}

AGENT = r'''
'use strict';
const IMG=0x00400000; let DELTA=0;
const RVA_RES=0x00497310, RVA_DEPTH=0x0067e9f8, RVA_PHASE=0x0067eca4, RVA_SEL=0x0067ed40;
let navCtrl=-1, navUntil=0;
const ST = {row:null, rva:0, entry:null, calls:0, byGate:{}, byRet:{}, witness:null, crashes:0};
const LOG = [];            // boot rows: ordered callee log while inside the row
const BC = {keys:{}, q:false, qcalls:0, qquads:0, outside:0, edge:0};
const PMR = {hist:{}};
const TLS = {};            // thread id -> depth inside the row
const PTRS = {}; let nPtr=0;
let calleeL = [];
function abs(r){ return ptr((r+DELTA)>>>0); }
function u8(a){ return abs(a).readU8(); }
function s32(a){ return abs(a).readS32(); }
function hx(v){ return (v>>>0).toString(16); }
function tok(p){ const k=p.toString(); if (PTRS[k]===undefined) PTRS[k]='P'+(nPtr++); return PTRS[k]; }
function inRow(){ return (TLS[Process.getCurrentThreadId()]||0) > 0; }
function gate(){ try{ const d=s32(RVA_DEPTH); return s32(0x0067ed3c+d*0x40); }catch(e){ return -999; } }
function argU(ctx,i){ return ctx.esp.add(4+4*i).readU32(); }
function addCallee(rva, name, nargs, opts){
  opts=opts||{};
  const l = Interceptor.attach(abs(rva), {
    onEnter(a){ this.on = inRow(); if (!this.on) return;
      const parts=[];
      for (let i=0;i<nargs;i++){ const v=argU(this.context,i);
        if (opts.tokArgs && opts.tokArgs.indexOf(i)>=0) parts.push(v===0?'0':tok(ptr(v)));
        else if (opts.deref && opts.deref.indexOf(i)>=0){ let d='AV'; try{ d='@'+hx(ptr(v).readU32()); }catch(e){} parts.push(d); }
        else parts.push(hx(v)); }
      this.s = name+'('+parts.join(',')+')'; },
    onLeave(r){ if (!this.on) return; LOG.push(opts.ret ? this.s+'='+(opts.tokRet ? (r.toUInt32()===0?'0':tok(r)) : hx(r.toUInt32())) : this.s); }});
  calleeL.push(l);
}
function attachEntry(entry, row){
  if (row==='bc'){
    Interceptor.attach(entry, {
      onEnter(a){ const t=Process.getCurrentThreadId(); TLS[t]=(TLS[t]||0)+1; ST.calls++;
        const g=gate(); ST.byGate[g]=(ST.byGate[g]||0)+1;
        const ra=hx(this.returnAddress.toUInt32()-DELTA); ST.byRet[ra]=(ST.byRet[ra]||0)+1;
        if (BC.q){ const c=this.context;
          this.k=[hx(argU(c,0)),hx(argU(c,1)),hx(argU(c,2)),hx(argU(c,3)),'al='+hx(c.eax.toUInt32()&0xff),'g='+g,'ra='+ra].join(',');
          this.quads=[]; BC.cur=this; }
        else this.k=null; },
      onLeave(r){ const t=Process.getCurrentThreadId(); TLS[t]=(TLS[t]||1)-1;
        if (this.k===null) return; BC.cur=null;
        // Window edge: a call still in flight when stopq() detached the 0x00472c60 hook has lost
        // its later quads. Only calls that START and END inside the window are recorded.
        if (!BC.q){ BC.edge++; return; }
        BC.qcalls++;
        const o=this.quads.join(' | '); let e=BC.keys[this.k]; if(!e) e=BC.keys[this.k]={n:0,outs:{}};
        e.n++; e.outs[o]=(e.outs[o]||0)+1; }});
  } else if (row==='pmr'){
    Interceptor.attach(entry, { onEnter(a){ ST.calls++;
      let fd0=-999, sub=-999, sph=-999; try{ fd0=s32(0x007f0fd0); sub=s32(0x0063ba8c); sph=u8(0x00771968);}catch(e){}
      const k='fd0='+fd0+',sub='+sub+',sph='+sph; PMR.hist[k]=(PMR.hist[k]||0)+1; }});
  } else {
    Interceptor.attach(entry, {
      onEnter(a){ const t=Process.getCurrentThreadId(); TLS[t]=(TLS[t]||0)+1; ST.calls++;
        this.s = 'ENTER('+(ST.row==='bqf' ? hx(argU(this.context,0)) : '')+')';
        LOG.push(this.s+' ra='+hx(this.returnAddress.toUInt32()-DELTA)); },
      onLeave(r){ const t=Process.getCurrentThreadId(); TLS[t]=(TLS[t]||1)-1;
        LOG.push('LEAVE='+(ST.row==='bqf' ? '-' : hx(r.toUInt32())));
        if (ST.calls>=1 && TLS[t]===0){ const ls=calleeL; calleeL=[]; setTimeout(function(){ ls.forEach(function(l){ l.detach(); }); }, 0); } }});
    if (ST.row==='subinit'){
      addCallee(0x00493710,'rwinit',1,{ret:true});
      addCallee(0x004921d0,'display',0,{});
      addCallee(0x00428590,'viewport',0,{});
    } else {
      addCallee(0x004671a0,'cam',1,{ret:true, tokRet:true});
      addCallee(0x004c1bb0,'clear',3,{tokArgs:[0], deref:[1]});
      addCallee(0x004c1a00,'begin',1,{tokArgs:[0], ret:true});
      addCallee(0x004c19f0,'end',1,{tokArgs:[0]});
      addCallee(0x004c1be0,'show',3,{tokArgs:[0]});
    }
  }
}
rpc.exports={
  init:function(){ const m=Process.findModuleByName('MASHED.exe')||Process.enumerateModules()[0];
    DELTA=m.base.toUInt32()-IMG;
    Interceptor.attach(abs(RVA_RES),{ onEnter(a){const sp=this.context.esp;this.p=sp.add(4).readS32();this.c=sp.add(8).readS32();},
      onLeave(ret){ if (this.p===0 && this.c===navCtrl && Date.now()<navUntil) ret.replace(ptr(0xff)); }});
    return DELTA; },
  loadasi:function(path){ const m=Module.load(path); const f=m.findExportByName('InjectHooks');
    if (!f) return {ok:false, why:'no InjectHooks'}; new NativeFunction(f,'void',[])();
    return {ok:true, base:m.base.toString(), name:m.name, path:m.path}; },
  arm:function(row, rva, mode){
    ST.row=row; ST.rva=rva; const a=abs(rva);
    const bytes=[]; for (let i=0;i<8;i++) bytes.push(('0'+a.add(i).readU8().toString(16)).slice(-2));
    const w={bytes:bytes.join(' '), b0:a.readU8()};
    if (mode==='stock'){ if (w.b0===0xe9) return {ok:false, why:'E9 on stock arm', w:w};
      ST.witness=w; ST.entry=a.toString(); attachEntry(a,row); return {ok:true, w:w, entry:ST.entry}; }
    if (w.b0!==0xe9) return {ok:false, why:'no E9', w:w};
    const tgt=a.add(5).add(a.add(1).readS32()); const mm=Process.findModuleByAddress(tgt);
    w.rel32=a.add(1).readS32(); w.target=tgt.toString();
    if (mm){ w.module=mm.name; w.path=mm.path; w.off='0x'+tgt.sub(mm.base).toUInt32().toString(16);
      const ex=mm.enumerateExports().filter(function(e){ return e.address.equals(tgt); }); w.exports=ex.map(function(e){return e.name;}); }
    ST.witness=w; ST.entry=tgt.toString(); attachEntry(tgt,row); return {ok:true, w:w, entry:ST.entry}; },
  // bc: start/stop the 0x00472c60 window (callee attached only while capturing).
  startq:function(){ if (BC.l) return 0; BC.q=true;
    BC.l=Interceptor.attach(abs(0x00472c60),{ onEnter(a){ if (!inRow() || !BC.cur){ if (BC.q) BC.outside++; return; }
      const c=this.context; const v=[]; for (let i=0;i<5;i++) v.push(hx(argU(c,i))); BC.cur.quads.push(v.join(',')); BC.qquads++; }});
    return 1; },
  stopq:function(){ BC.q=false; if (BC.l){ BC.l.detach(); BC.l=null; } return {qcalls:BC.qcalls, qquads:BC.qquads, outside:BC.outside, edge:BC.edge}; },
  press:function(c,ms){ navCtrl=c; navUntil=Date.now()+ms; return 1; },
  state:function(){ let d=-999,ph=-999,g=-999,sph=-999,ui=-999,fd0=-999;
    try{ d=s32(RVA_DEPTH); ph=s32(RVA_PHASE); g=s32(0x0067ed3c+d*0x40); sph=u8(0x00771968); ui=s32(0x0063ba8c); fd0=s32(0x007f0fd0);}catch(e){}
    return {depth:d, phase:ph, gate:g, sph:sph, ui:ui, fd0:fd0, calls:ST.calls}; },
  dump:function(){ return {st:ST, log:LOG.slice(0,400), nlog:LOG.length, bc:{keys:BC.keys, qcalls:BC.qcalls, qquads:BC.qquads, outside:BC.outside, edge:BC.edge}, pmr:PMR}; },
  depth:function(){ try{return s32(RVA_DEPTH);}catch(e){return -999;} },
  phase:function(){ try{return s32(RVA_PHASE);}catch(e){return -999;} },
  sel:function(){ try{const d=s32(RVA_DEPTH); return s32(RVA_SEL+d*0x40);}catch(e){return -999;} }
};
Process.setExceptionHandler(function (d) {
  if (d.type !== 'access-violation') return false;
  try { ST.crashes++; const pc=d.context.pc; const m=Process.findModuleByAddress(pc);
    send({kind:'crash', pc:pc.toString(), module: m ? (m.name+'+0x'+pc.sub(m.base).toUInt32().toString(16)) : 'UNMAPPED',
          addr: d.memory ? (d.memory.operation+' @ '+d.memory.address) : null}); } catch(e){}
  return false;
});
'''


def sha256(p):
    h = hashlib.sha256()
    with open(p, "rb") as f:
        for b in iter(lambda: f.read(1 << 20), b""): h.update(b)
    return h.hexdigest().upper()


def quick_battle(nav, X, log):
    # statenav.py Quick Battle sequence, verbatim order (as c4_detect_e_scenario.py).
    nav.confirm_to_depth(2)
    nav.advance_past_load_modal(3)
    nav.press(12)
    nav.confirm_to_depth(4, tries=4)
    nav.confirm_to_depth(5, tries=4)
    log(f"  track-select: depth={nav.depth()} sel={X.sel()}")
    nav.press(4); time.sleep(1.5)
    for k in range(5):
        if nav.phase() != 3: break
        nav.press(4); time.sleep(1.5)
        if nav.phase() == 0: break
    log(f"  race launched: {X.state()}")


def main():
    av = sys.argv
    def opt(f, d): return av[av.index(f) + 1] if f in av else d
    row = opt("--row", "bc"); assert row in ROWS
    arm = opt("--arm", "stock"); assert arm in ("stock", "port", "ctl")
    asi = opt("--asi", None)
    round_secs = int(opt("--round", "240"))
    window = float(opt("--window", "2.0"))
    out = Path(opt("--out", str(ROOT / "log" / "c4_front"))); out.mkdir(parents=True, exist_ok=True)
    tag = opt("--tag", f"{row}_{arm}")
    rva = ROWS[row]

    env = dict(os.environ)
    env["MASHED_RE_NO_AUTO_HOOK"] = "1"
    env["MASHED_HOOK_ONLY"] = "__none__" if arm == "stock" else f"0x{rva:08x}"
    if arm != "stock":
        assert asi and Path(asi).exists(), "--asi required for port/ctl"
    rec = {"row": row, "rva": f"0x{rva:08x}", "arm": arm, "tag": tag, "exe_sha256": sha256(EXE),
           "asi": str(Path(asi).resolve()) if asi else None, "asi_sha256": sha256(asi) if asi else None,
           "env": {k: env.get(k) for k in ("MASHED_HOOK_ONLY", "MASHED_RE_NO_AUTO_HOOK")},
           "log": [], "crash": []}
    def log(msg):
        print(msg, flush=True); rec["log"].append(msg)

    dev = frida.get_local_device()
    pid = dev.spawn(str(EXE), cwd=str(ORIG), env=env)
    rec["pid"] = pid; log(f"  spawned pid={pid} row={row} arm={arm}")
    sess = dev.attach(pid); hproc = _open_handle(pid)
    def on_msg(m, d):
        p = m.get("payload") if isinstance(m, dict) else None
        if isinstance(p, dict) and p.get("kind") == "crash":
            rec["crash"].append(p); print("  [CRASH]", p, flush=True)
        elif m.get("type") == "error":
            print("  agent err:", m.get("description"), flush=True); rec["log"].append("agent err: " + str(m.get("description")))
    scr = sess.create_script(AGENT); scr.on("message", on_msg); scr.load()
    X = scr.exports_sync
    X.init()
    verdict = "INCOMPLETE"
    nav = Nav(scr, pid)
    try:
        if arm != "stock":
            r = X.loadasi(str(Path(asi).resolve())); rec["load"] = r; log(f"  asi load (suspended): {r}")
            if not r.get("ok"): verdict = "LOAD-FAILED"; raise SystemExit
        r = X.arm(row, rva, arm); rec["arm_result"] = r; log(f"  arm: {r}")
        if not r.get("ok"): verdict = "ARM-FAILED"; raise SystemExit
        dev.resume(pid)
        nav.wait(lambda: nav.phase() == 3 and nav.depth() >= 1, 30.0, "title up")
        log(f"  title: {X.state()}")
        if row in ("subinit", "bqf"):
            time.sleep(5.0)
            rec["state_end"] = X.state()
            verdict = "DONE"
        else:
            quick_battle(nav, X, log)
            end = time.time() + round_secs; t_log = 0; t_pulse = 0; n_pulse = 0; reached = None
            pmr_sampled = False
            while time.time() < end:
                if not nav.alive(): log("   process exited during race"); break
                s = X.state()
                if s["sph"] == 3 and time.time() - t_pulse > 1.0:
                    X.press(4, 200); t_pulse = time.time(); n_pulse += 1
                if time.time() - t_log > 10:
                    t_log = time.time(); log(f"   wait pulses={n_pulse} {s}")
                if row == "pmr" and s["calls"] > 0 and s["sph"] == 3 and not pmr_sampled and n_pulse >= 30:
                    pmr_sampled = True; reached = s; break
                if row == "bc" and s["phase"] == 3 and s["gate"] == 5:
                    reached = s; break
                time.sleep(0.25)
            rec["reached"] = reached; rec["race_pulses"] = n_pulse
            if not reached:
                verdict = "NOT-REACHED"; raise SystemExit
            log(f"  >>> reached: {reached}")
            if row == "bc":
                time.sleep(4.0)
                rec["pre_window"] = X.state()
                X.startq(); time.sleep(window); rec["window"] = X.stopq()
                log(f"  window: {rec['window']}")
                time.sleep(3.0)
            rec["state_end"] = X.state()
            verdict = "DONE"
    except SystemExit:
        pass
    except Exception as e:
        log(f"  harness exception: {e!r}")
    finally:
        try: rec["dump"] = X.dump()
        except Exception as e: rec["dump_err"] = repr(e)
        rec["alive_at_end"] = nav.alive()
        try:
            if nav.alive(): dev.kill(pid)
        except Exception: pass
        try: sess.detach()
        except Exception: pass
        rec["exit_code"] = _exit_code(hproc)
        rec["verdict"] = verdict
        p = out / f"c4_front_{tag}.json"
        p.write_text(json.dumps(rec, indent=1), encoding="utf-8")
        print(f"  verdict={verdict} -> {p}", flush=True)
    return 0


if __name__ == "__main__":
    sys.exit(main())
