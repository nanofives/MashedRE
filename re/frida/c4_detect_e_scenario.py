# Canonical-scenario C4 harness for MenuButtonDetectE 0x0042b770 (contcfg action 7, "Change Stat").
#
# Why a new harness: 0x0042b770 has ONE call site, 0x004409ce in FUN_0043dfd0, guarded by the
# per-screen enable [ESP+0x34] (0x004409c8), which only screen 5 (Race Results) sets, via the
# ff33 opcode in its record (U-9060/U-9061). At every other menu the game never calls it
# (CHANGELOG 2026-08-31: 0/s at the menu and while paused), so the boot-to-menu tools
# (canonical_install_observe / canonical_c4_autodiff / canonical_c4_navigate at 4,4) cannot
# exercise it. This harness reuses statenav.py's Quick Battle navigation verbatim to reach a
# naturally-entered Race Results screen, then presses action 7 for each of players 0..3 through
# the FUN_00497310 return override (no OS input).
#
# Arms (one fresh MASHED per run, own PID only):
#   off : deployed .asi loaded, MASHED_HOOK_ONLY=__none__  -> stock 0x0042b770 runs.
#   on  : deployed .asi loaded, MASHED_HOOK_ONLY=0x0042b770 -> inline E9 live, the port runs.
#   ctl : MASHED_RE_NO_AUTO_HOOK=1 (shim loads nothing); after the title is up, Module.load
#         --ctl-asi and call its InjectHooks export with MASHED_HOOK_ONLY=0x0042b770. Pointing
#         --ctl-asi at the deployed .asi gives the late-load twin of 'on'.
#   stock: MASHED_RE_NO_AUTO_HOOK=1 and nothing loaded -> the setup-matched reference for ctl.
#
# Per call of the detector (orig entry on 'off', the E9 target on 'on'/'ctl'): snapshot every
# input the original bytes read (0x007f1a0c, 0x007f1a14..0x007f1a50, held 0x007f1049+k*0x4c,
# previous 0x007f1509+k*0x4c, *( *0x005f2770 + i*4 + 0x34)) and the return. Two byte-level
# reference models are evaluated on the same snapshot: m_orig (0x0042b770..0x0042b888) and
# m_old (the pre-dc98c93a port: 4-byte stride, 8 records). Consumer side: entries into
# FUN_0042c510 (the page advance FUN_0042c960 runs when 0x0067ec28 is set) and the page index
# 0x0067ea08, sampled around each press.
#
# Usage: py -3.12 re/frida/c4_detect_e_scenario.py --arm off|on|ctl [--ctl-asi PATH]
#        [--round 240] [--out DIR] [--tag NAME]
import json, os, sys, time, hashlib
from pathlib import Path
import frida
try: import psutil
except ImportError: psutil = None

ROOT = Path(__file__).resolve().parent.parent.parent
sys.path.insert(0, str(ROOT / "re" / "frida"))
import statenav
from statenav import Nav, shoot, _open_handle, _exit_code

EXE = statenav.EXE
ORIG = statenav.ORIG

AGENT = r'''
'use strict';
const IMG=0x00400000; let DELTA=0;
const RVA_RES=0x00497310, RVA_DEPTH=0x0067e9f8, RVA_PHASE=0x0067eca4, RVA_SEL=0x0067ed40;
const RVA_E=0x0042b770, RVA_C510=0x0042c510;
let navCtrl=-1, navUntil=0;
const P7 = {};                    // player -> until (action 7 override)
const ST = {eCalls:0, eRet1:0, c510:0, mmOrig:0, mmOld:0, diffModels:0, entry:null, jmp:-1};
const KEYS = {};                  // input-state key -> {ret:{0:n,1:n}, mo, mold}
const EV = [];                    // ret==1 / any-held / model-split detector calls, and FUN_0042c510 entries
const C510BY = {};                // FUN_0042c510 return-address RVA -> count
function abs(r){ return ptr((r+DELTA)>>>0); }
function u8(a){ return abs(a).readU8(); }
function s32(a){ return abs(a).readS32(); }
function snap(){
  const s = {sel:s32(0x007f1a0c), ids:[], held:[], prev:[], st:[]};
  for (let i=0;i<16;i++) s.ids.push(s32(0x007f1a14+i*4));
  for (let k=0;k<8;k++){ s.held.push(u8(0x007f1049+k*0x4c)); s.prev.push(u8(0x007f1509+k*0x4c)); }
  let base=null; try { base = abs(0x005f2770).readPointer(); } catch(e){}
  for (let i=0;i<8;i++){ let v=-999; try { v = base.add(i*4+0x34).readS32(); } catch(e){} s.st.push(v); }
  return s;
}
function edge(s,id){ if (id<0 || id>255) return -1;  // out-of-range id: original reads outside the 8 rows
  let h,p; try { h=abs(0x007f1049+id*0x4c).readU8(); p=abs(0x007f1509+id*0x4c).readU8(); } catch(e){ return -1; }
  return (h!==0 && p===0) ? 1 : 0; }
function mOrig(s){   // 0x0042b770..0x0042b888
  let ebx=0;
  if (s.sel===0x1000){ for (let k=0;k<7;k++) if (s.held[k]!==0 && s.prev[k]===0) ebx=1;
    if (s.held[7]!==0 && s.prev[7]===0) return 1; return ebx; }
  for (let i=0;i<4;i++){ const id=s.ids[i*4]; if (id===-1) continue; if (s.st[i]===2) continue;
    if (edge(s,id)===1) ebx=1; }
  return ebx; }
function mOld(s){    // pre-dc98c93a port: piVar3++ (4-byte stride), 8 records, index 0..7
  let ebx=0;
  if (s.sel===0x1000){ for (let k=0;k<7;k++) if (s.held[k]!==0 && s.prev[k]===0) ebx=1;
    if (s.held[7]!==0 && s.prev[7]===0) return 1; return ebx; }
  for (let i=0;i<8;i++){ const id=s.ids[i]; if (id===-1) continue; if (s.st[i]===2) continue;
    if (edge(s,id)===1) ebx=1; }
  return ebx; }
function keyOf(s){ return [s.sel.toString(16), s.ids.join(','), s.st.join(','),
  s.held.map(x=>x?1:0).join(''), s.prev.map(x=>x?1:0).join('')].join('|'); }
function attachE(entry){
  Interceptor.attach(entry, {
    onEnter(a){ try { this.s = snap(); } catch(e){ this.s = null; } },
    onLeave(r){ ST.eCalls++; const ret = r.toUInt32(); if (ret===1) ST.eRet1++;
      const s=this.s; if (!s) return;
      const mo=mOrig(s), mold=mOld(s);
      if (mo!==ret) ST.mmOrig++; if (mold!==ret) ST.mmOld++; if (mo!==mold) ST.diffModels++;
      const k=keyOf(s); let e=KEYS[k]; if (!e){ e=KEYS[k]={r0:0,r1:0,mo:mo,mold:mold}; }
      if (ret===1) e.r1++; else e.r0++;
      const anyHeld = s.held.some(x=>x!==0);
      if ((ret===1 || anyHeld || mo!==mold) && EV.length<4000)
        EV.push({t:Date.now(), ret:ret, mo:mo, mold:mold, k:k, page:s32(0x0067ea08)});
    }});
}
rpc.exports={
  init:function(){ const m=Process.findModuleByName('MASHED.exe')||Process.enumerateModules()[0];
    DELTA=m.base.toUInt32()-IMG;
    Interceptor.attach(abs(RVA_RES),{ onEnter(a){const sp=this.context.esp;this.p=sp.add(4).readS32();this.c=sp.add(8).readS32();},
      onLeave(ret){ const now=Date.now();
        if (this.p===0 && this.c===navCtrl && now<navUntil){ ret.replace(ptr(0xff)); return; }
        if (this.c===7 && P7[this.p]!==undefined && now<P7[this.p]) ret.replace(ptr(0xff)); }});
    Interceptor.attach(abs(RVA_C510),{ onEnter(){ ST.c510++;
      const ra=((this.returnAddress.toUInt32()-DELTA)>>>0).toString(16); C510BY[ra]=(C510BY[ra]||0)+1;
      if (EV.length<4000) EV.push({t:Date.now(), c510:ra, page:s32(0x0067ea08)}); }});
    return DELTA; },
  // mode 'off': attach the original entry. 'on': follow the installed E9 to the port.
  arme:function(mode){
    const a=abs(RVA_E); const b=a.readU8(); ST.jmp=b;
    if (mode==='off'){ if (b===0xe9) return {ok:false, why:'E9 present on off arm'}; attachE(a); ST.entry=a.toString(); return {ok:true, entry:ST.entry, jmp:b}; }
    if (b!==0xe9) return {ok:false, why:'no E9', jmp:b};
    const tgt=a.add(5).add(a.add(1).readS32()); const mm=Process.findModuleByAddress(tgt);
    attachE(tgt); ST.entry=tgt.toString()+(mm?(' '+mm.name+'+0x'+tgt.sub(mm.base).toUInt32().toString(16)):'');
    return {ok:true, entry:ST.entry, jmp:b, dword:a.readU32().toString(16), rel:a.add(1).readS32()}; },
  loadctl:function(path){ const m=Module.load(path); const f=m.findExportByName('InjectHooks');
    if (!f) return {ok:false, why:'no InjectHooks'}; new NativeFunction(f,'void',[])();
    return {ok:true, base:m.base.toString(), name:m.name, path:m.path}; },
  modinfo:function(){ const a=abs(RVA_E); const tgt=a.readU8()===0xe9 ? a.add(5).add(a.add(1).readS32()) : null;
    const mm = tgt ? Process.findModuleByAddress(tgt) : null;
    return {b0:a.readU8(), dword:a.readU32().toString(16), tgt: tgt?tgt.toString():null,
            mod: mm?{name:mm.name, path:mm.path, base:mm.base.toString()}:null}; },
  press:function(c,ms){ navCtrl=c; navUntil=Date.now()+ms; return 1; },
  press7:function(players,ms){ const u=Date.now()+ms; players.forEach(function(p){ P7[p]=u; }); return 1; },
  state:function(){ let d=-999,ph=-999,gate=-999,flag=-999,page=-999,sel=-999;
    try{ d=s32(RVA_DEPTH); ph=s32(RVA_PHASE); gate=s32(0x0067ed3c+d*0x40); flag=s32(0x0067ec28); page=s32(0x0067ea08); sel=s32(0x007f1a0c);}catch(e){}
    let sph=-999, ui=-999, fc=-999; try{ sph=u8(0x00771968); ui=s32(0x0063ba8c); fc=s32(0x007f0ff4);}catch(e){}
    return {depth:d, phase:ph, gate:gate, flag:flag, page:page, sel:sel, sph:sph, ui:ui, fc:fc, b0:(function(){try{return u8(RVA_E);}catch(e){return -1;}})(),
            eCalls:ST.eCalls, eRet1:ST.eRet1, c510:ST.c510, c510e:(C510BY['42c9c4']||0), c510o:ST.c510-(C510BY['42c9c4']||0), mmOrig:ST.mmOrig, mmOld:ST.mmOld, diffModels:ST.diffModels}; },
  snapnow:function(){ return snap(); },
  dump:function(){ return {st:ST, keys:KEYS, ev:EV, c510by:C510BY}; },
  depth:function(){ try{return s32(RVA_DEPTH);}catch(e){return -999;} },
  phase:function(){ try{return s32(RVA_PHASE);}catch(e){return -999;} },
  sel:function(){ try{const d=s32(RVA_DEPTH); return s32(RVA_SEL+d*0x40);}catch(e){return -999;} }
};
Process.setExceptionHandler(function (d) {
  if (d.type !== 'access-violation') return false;
  try { const pc=d.context.pc; const m=Process.findModuleByAddress(pc);
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


def main():
    av = sys.argv
    def opt(f, d): return av[av.index(f) + 1] if f in av else d
    arm = opt("--arm", "off"); assert arm in ("off", "on", "ctl", "stock")
    ctl_asi = opt("--ctl-asi", None)
    round_secs = int(opt("--round", "240"))
    out = Path(opt("--out", str(ROOT / "log" / "c4_detect_e")))
    tag = opt("--tag", arm)
    out.mkdir(parents=True, exist_ok=True)
    shotdir = out / f"c4_detect_e_shots_{tag}"; shotdir.mkdir(exist_ok=True)

    env = dict(os.environ)
    if arm == "off":
        env.pop("MASHED_RE_NO_AUTO_HOOK", None); env["MASHED_HOOK_ONLY"] = "__none__"
    elif arm == "on":
        env.pop("MASHED_RE_NO_AUTO_HOOK", None); env["MASHED_HOOK_ONLY"] = "0x0042b770"
    elif arm == "stock":
        env["MASHED_RE_NO_AUTO_HOOK"] = "1"; env["MASHED_HOOK_ONLY"] = "__none__"
    else:
        assert ctl_asi and Path(ctl_asi).exists(), "--ctl-asi required"
        env["MASHED_RE_NO_AUTO_HOOK"] = "1"; env["MASHED_HOOK_ONLY"] = "0x0042b770"
    rec = {"arm": arm, "tag": tag, "exe_sha256": sha256(EXE),
           "deployed_asi": str(ORIG / "mashed_re_dev.asi"),
           "deployed_asi_sha256": sha256(ORIG / "mashed_re_dev.asi"),
           "ctl_asi": ctl_asi if arm == "ctl" else None, "ctl_asi_sha256": sha256(ctl_asi) if arm == "ctl" else None,
           "env": {k: env.get(k) for k in ("MASHED_HOOK_ONLY", "MASHED_RE_NO_AUTO_HOOK")},
           "log": [], "presses": [], "crash": []}
    def log(msg):
        print(msg, flush=True); rec["log"].append(msg)

    dev = frida.get_local_device()
    pid = dev.spawn(str(EXE), cwd=str(ORIG), env=env)
    rec["pid"] = pid; log(f"  spawned pid={pid} arm={arm}")
    sess = dev.attach(pid); hproc = _open_handle(pid)
    def on_msg(m, d):
        p = m.get("payload") if isinstance(m, dict) else None
        if isinstance(p, dict) and p.get("kind") == "crash":
            rec["crash"].append(p); print("  [CRASH]", p, flush=True)
        elif m.get("type") == "error":
            print("  agent err:", m.get("description"), flush=True)
    scr = sess.create_script(AGENT); scr.on("message", on_msg); scr.load()
    X = scr.exports_sync
    X.init()
    dev.resume(pid)
    nav = Nav(scr, pid)
    verdict = "INCOMPLETE"
    try:
        log("  booting...")
        nav.wait(lambda: nav.phase() == 3 and nav.depth() >= 1, 25.0, "title up")
        log(f"  title: depth={nav.depth()} phase={nav.phase()}")
        if arm == "ctl":
            r = X.loadctl(str(Path(ctl_asi).resolve())); log(f"  ctl load: {r}")
        if arm in ("off", "stock"):
            r = X.arme("off")
        else:
            r = None
            for _ in range(30):
                r = X.arme("on")
                if r.get("ok"): break
                time.sleep(0.5)
        rec["arme"] = r; rec["modinfo_at_arm"] = X.modinfo(); log(f"  arme: {r}  modinfo={rec['modinfo_at_arm']}")
        if not r or not r.get("ok"):
            verdict = "ARM-FAILED"; raise SystemExit
        # --- statenav.py Quick Battle sequence, verbatim order ---
        nav.confirm_to_depth(2)
        log(f"  after GTS: depth={nav.depth()} sel={X.sel()}")
        nav.advance_past_load_modal(3)
        log(f"  single player: depth={nav.depth()} sel={X.sel()}")
        nav.press(12)
        log(f"  mode sel (Quick Battle): sel={X.sel()}")
        nav.confirm_to_depth(4, tries=4)
        log(f"  colour-select: depth={nav.depth()} sel={X.sel()}")
        nav.confirm_to_depth(5, tries=4)
        log(f"  track-select: depth={nav.depth()} sel={X.sel()}")
        nav.press(4); time.sleep(1.5)
        log(f"  after track confirm: depth={nav.depth()} phase={nav.phase()}")
        for k in range(5):
            if nav.phase() != 3: break
            nav.press(4); time.sleep(1.5)
            log(f"  start-attempt {k}: depth={nav.depth()} phase={nav.phase()}")
            if nav.phase() == 0: break
        shoot(pid, shotdir / "race_enter.png")
        # --- wait (no input) for Race Results: gate TYPE(depth-1)==5 and the detector being called
        end = time.time() + round_secs; t_log = 0; reached = None
        t_pulse = 0; n_pulse = 0; pulse_every = float(opt("--pulse", "1.0"))
        while time.time() < end:
            if not nav.alive(): log("   process exited during race"); break
            s = X.state()
            # scenario_launch.py's in-race recipe: pulse control 4 (player 0) to skip the start
            # intro, keep the car driving and continue between rounds. Only while the session
            # phase byte 0x00771968 == 3 (race), so no pulse lands on a frontend screen.
            if s["sph"] == 3 and time.time() - t_pulse > pulse_every:
                X.press(4, 200); t_pulse = time.time(); n_pulse += 1
            if time.time() - t_log > 10:
                t_log = time.time(); log(f"   wait t+{int(round_secs-(end-time.time()))}s pulses={n_pulse} {s}")
            if s["phase"] == 3 and s["gate"] == 5 and s["eCalls"] > 0:
                reached = s; break
            time.sleep(0.25)
        rec["reached"] = reached; rec["race_pulses"] = n_pulse
        if not reached:
            verdict = "SCREEN5-NOT-REACHED"; shoot(pid, shotdir / "timeout.png"); raise SystemExit
        log(f"  >>> Race Results reached: {reached}")
        time.sleep(2.0)
        shoot(pid, shotdir / "results_before.png")
        rec["snap_results"] = X.snapnow()
        log(f"  inputs at results: {rec['snap_results']}")
        base = X.state(); rec["baseline"] = base
        time.sleep(3.0)
        b2 = X.state(); rec["baseline_after_3s"] = b2
        log(f"  idle 3s: eCalls {base['eCalls']}->{b2['eCalls']} eRet1 {base['eRet1']}->{b2['eRet1']} c510 {base['c510']}->{b2['c510']} page {base['page']}->{b2['page']}")
        # --- presses: each player alone, twice round, then all four together
        seq = [[0], [1], [2], [3], [0], [1], [2], [3], [0, 1, 2, 3]]
        for players in seq:
            if not nav.alive(): log("   process exited during presses"); break
            s0 = X.state()
            X.press7(players, 200); time.sleep(2.0)
            s1 = X.state()
            # c510e: FUN_0042c510 entries returning to 0x0042c9c4 (the 0x0067ec28 flag path that
            # 0x0042b770's return drives via 0x004409e8). c510o: every other caller, i.e. the
            # scroll-driven call at 0x0042d01b, whose timing varies run to run even on stock.
            pr = {"players": players, "eRet1": s1["eRet1"] - s0["eRet1"], "c510": s1["c510"] - s0["c510"],
                  "c510e": s1["c510e"] - s0["c510e"], "c510o": s1["c510o"] - s0["c510o"],
                  "page0": s0["page"], "page1": s1["page"], "gate": s1["gate"], "flag1": s1["flag"],
                  "eCalls": s1["eCalls"] - s0["eCalls"]}
            rec["presses"].append(pr); log(f"  press {players}: {pr}")
        shoot(pid, shotdir / "results_after.png")
        rec["final"] = X.state(); rec["modinfo_final"] = X.modinfo()
        d = X.dump(); rec["dump"] = d
        verdict = "DONE"
    except SystemExit:
        pass
    finally:
        try: rec.setdefault("final", X.state())
        except Exception: pass
        try: rec.setdefault("dump", X.dump())
        except Exception: pass
        rec["alive_at_end"] = nav.alive()
        try:
            if nav.alive(): dev.kill(pid)
        except Exception: pass
        try: sess.detach()
        except Exception: pass
        rec["exit_code"] = _exit_code(hproc)
        rec["verdict"] = verdict
        p = out / f"c4_detect_e_{tag}.json"
        p.write_text(json.dumps(rec, indent=1), encoding="utf-8")
        print(f"  verdict={verdict} -> {p}", flush=True)
    return 0


if __name__ == "__main__":
    sys.exit(main())
