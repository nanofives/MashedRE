# Canonical-scenario C4 harness for Championship::Complete 0x00430290.
#
# Why a new harness: 0x00430290 has ONE caller, the tail-jmp at 0x004111a1 in FUN_00411170
# (the state-6 per-frame dispatcher), taken only when FUN_00410d10 != 0 and FUN_00410510
# (race-end evaluator) != 0, i.e. once per concluded race. Its body is gated on the screen id
# DAT_0067e9fc in {3,4,5} (0x00430290..0x004302a4) and on FUN_0040dd60 (DAT_0063b90c == 1 AND
# DAT_007f0fcc, 0x0040dd60). A boot-to-menu run never calls it. This harness reaches a
# Challenge race the same way race_draw_burst.py --challenge does (Single Player, mode row 0,
# cup confirm, challenge index N by real down-presses) and drives it with statenav's 1 Hz
# control-4 pulses (c4_detect_e_scenario.py's in-race recipe) until the race concludes.
#
# The body's AutosaveTrigger tail (0x004303d9 / 0x00430665) writes the save, so every arm runs
# from an INSTALL COPY (--root DIR holding DIR/MASHED.exe), never from original\.
#
# Arms (one fresh MASHED per run, own PID only):
#   off  : copy's deployed .asi loaded, MASHED_HOOK_ONLY=__none__ -> stock 0x00430290 runs.
#   on   : copy's deployed .asi loaded, MASHED_HOOK_ONLY=0x00430290 -> inline E9 live, port runs.
#   ctl  : MASHED_RE_NO_AUTO_HOOK=1; after the title is up, Module.load --ctl-asi and call its
#          InjectHooks with MASHED_HOOK_ONLY=0x00430290.
#   stock: MASHED_RE_NO_AUTO_HOOK=1, nothing loaded.
#
# Per body call (orig entry on off/stock, the E9 target on on/ctl): snapshot every global the
# bytes read or write (screen id 0x0067e9fc, index 0x0067f17c, 0x0063b90c, 0x007f0fcc, the
# 0x007f0a40..0x007f0cc0 grid, 0x007f0f2c, the PostTrophyEvent cells 0x00898ab0 / 0x00899140)
# at entry and at return, plus every PostTrophyEvent id (orig 0x0042a920 and the .asi export)
# and every AutosaveTrigger 0x004099a0 entry while the body is on the stack.
#
# --force-win: at body entry, if 0x0063b90c == 1 and 0x007f0fcc == 0, write 0x007f0fcc = 1
# (the P1-won flag FUN_0040dd60 reads). Recorded per call. Same treatment on every arm.
#
# Usage: py -3.12 re/frida/c4_champ_complete_scenario.py --root DIR --arm off|on|ctl|stock
#        [--ctl-asi PATH] [--challenge N] [--race 300] [--force-win] [--out DIR] [--tag NAME]
import json, os, sys, time, hashlib
from pathlib import Path
import frida
try: import psutil
except ImportError: psutil = None

ROOT = Path(__file__).resolve().parent.parent.parent
sys.path.insert(0, str(ROOT / "re" / "frida"))
import statenav
from statenav import Nav, shoot, _open_handle, _exit_code

RVA = "0x00430290"

AGENT = r'''
'use strict';
const IMG=0x00400000; let DELTA=0;
const RVA_RES=0x00497310, RVA_DEPTH=0x0067e9f8, RVA_PHASE=0x0067eca4, RVA_SEL=0x0067ed40;
const RVA_BODY=0x00430290, RVA_POST=0x0042a920, RVA_SAVE=0x004099a0;
let navCtrl=-1, navUntil=0, forceWin=false;
const ST = {calls:0, entry:null, jmp:-1, postOrig:0, postPort:0, saves:0, savesInBody:0};
const CALLS = [];
let cur = null;
function abs(r){ return ptr((r+DELTA)>>>0); }
function u8(a){ return abs(a).readU8(); }
function s32(a){ return abs(a).readS32(); }
function grid(){ const g=[]; for (let a=0x007f0a40; a<0x007f0cc0; a+=4) g.push(s32(a)); return g; }
function snap(){
  return {mode:s32(0x0067e9fc), mode184:s32(0x0067f184), idx:s32(0x0067f17c), concl:s32(0x0063b90c),
          won:s32(0x007f0fcc), fd0:s32(0x007f0fd0), ui:s32(0x0063ba8c), g:grid(), f2c:s32(0x007f0f2c),
          pri:s32(0x00898ab0), ev:s32(0x00899140)};
}
function attachBody(entry){
  Interceptor.attach(entry, {
    onEnter(a){ ST.calls++;
      const ra=((this.returnAddress.toUInt32()-DELTA)>>>0).toString(16);
      const s0=snap(); let forced=false;
      if (forceWin && s0.concl===1 && s0.won===0){ abs(0x007f0fcc).writeS32(1); forced=true; }
      cur={t:Date.now(), ra:ra, pre:s0, forced:forced, posts:[], saves:[]}; },
    onLeave(r){ if (!cur) return; cur.post=snap(); CALLS.push(cur); cur=null; }});
}
function onPost(src){ return { onEnter(a){ const id=this.context.esp.add(4).readU32();
  if (src==='orig') ST.postOrig++; else ST.postPort++;
  if (cur) cur.posts.push({src:src, id:id}); } }; }
rpc.exports={
  init:function(fw){ forceWin=!!fw;
    const m=Process.findModuleByName('MASHED.exe')||Process.enumerateModules()[0];
    DELTA=m.base.toUInt32()-IMG;
    Interceptor.attach(abs(RVA_RES),{ onEnter(a){const sp=this.context.esp;this.p=sp.add(4).readS32();this.c=sp.add(8).readS32();},
      onLeave(ret){ if (this.p===0 && this.c===navCtrl && Date.now()<navUntil) ret.replace(ptr(0xff)); }});
    Interceptor.attach(abs(RVA_POST), onPost('orig'));
    Interceptor.attach(abs(RVA_SAVE), { onEnter(){ ST.saves++;
      const ra=((this.returnAddress.toUInt32()-DELTA)>>>0).toString(16);
      if (cur){ ST.savesInBody++; cur.saves.push(ra); } }});
    return DELTA; },
  arm:function(mode){
    const a=abs(RVA_BODY); const b=a.readU8(); ST.jmp=b;
    if (mode==='off'){ if (b===0xe9) return {ok:false, why:'E9 present on off arm'}; attachBody(a); ST.entry=a.toString(); return {ok:true, entry:ST.entry, jmp:b}; }
    if (b!==0xe9) return {ok:false, why:'no E9', jmp:b};
    const tgt=a.add(5).add(a.add(1).readS32()); const mm=Process.findModuleByAddress(tgt);
    let exp=null, pexp=null;
    if (mm){ try { exp=mm.findExportByName('ChampionshipComplete'); } catch(e){}
             try { pexp=mm.findExportByName('PostTrophyEvent'); } catch(e){} }
    if (pexp) Interceptor.attach(pexp, onPost('port'));
    attachBody(tgt);
    ST.entry=tgt.toString()+(mm?(' '+mm.name+'+0x'+tgt.sub(mm.base).toUInt32().toString(16)):'');
    return {ok:true, entry:ST.entry, jmp:b, dword:a.readU32().toString(16), tgt_is_export:(exp!==null && exp.equals(tgt)),
            export:exp?exp.toString():null, post_export:pexp?pexp.toString():null, mod:mm?mm.path:null}; },
  loadctl:function(path){ const m=Module.load(path); const f=m.findExportByName('InjectHooks');
    if (!f) return {ok:false, why:'no InjectHooks'}; new NativeFunction(f,'void',[])();
    return {ok:true, base:m.base.toString(), name:m.name, path:m.path}; },
  modinfo:function(){ const a=abs(RVA_BODY); const tgt=a.readU8()===0xe9 ? a.add(5).add(a.add(1).readS32()) : null;
    const mm = tgt ? Process.findModuleByAddress(tgt) : null;
    return {b0:a.readU8(), dword:a.readU32().toString(16), tgt: tgt?tgt.toString():null,
            mod: mm?{name:mm.name, path:mm.path, base:mm.base.toString()}:null}; },
  press:function(c,ms){ navCtrl=c; navUntil=Date.now()+ms; return 1; },
  state:function(){ let d=-999,ph=-999,gate=-999,sph=-999,ui=-999,mode=-999,idx=-999,concl=-999,won=-999,fd0=-999;
    try{ d=s32(RVA_DEPTH); ph=s32(RVA_PHASE); gate=s32(0x0067ed3c+d*0x40); sph=u8(0x00771968); ui=s32(0x0063ba8c);
         mode=s32(0x0067e9fc); idx=s32(0x0067f17c); concl=s32(0x0063b90c); won=s32(0x007f0fcc); fd0=s32(0x007f0fd0);}catch(e){}
    return {depth:d, phase:ph, gate:gate, sph:sph, ui:ui, mode:mode, idx:idx, concl:concl, won:won, fd0:fd0,
            calls:ST.calls, postOrig:ST.postOrig, postPort:ST.postPort, saves:ST.saves, savesInBody:ST.savesInBody,
            b0:(function(){try{return u8(RVA_BODY);}catch(e){return -1;}})()}; },
  dump:function(){ return {st:ST, calls:CALLS}; },
  depth:function(){ try{return s32(RVA_DEPTH);}catch(e){return -999;} },
  phase:function(){ try{return s32(RVA_PHASE);}catch(e){return -999;} },
  sel:function(){ try{const d=s32(RVA_DEPTH); return s32(RVA_SEL+d*0x40);}catch(e){return -999;} },
  idx:function(){ try{return s32(0x0067f17c);}catch(e){return -999;} }
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
    game = Path(opt("--root", "")).resolve()
    exe = game / "MASHED.exe"
    assert exe.exists(), f"--root must hold MASHED.exe: {game}"
    real_orig = (ROOT / "original").resolve()
    assert game != real_orig, "refusing to run from the real original\\ (the body autosaves)"
    assert game != Path(statenav.ORIG).resolve(), "refusing to run from statenav.ORIG"
    ctl_asi = opt("--ctl-asi", None)
    chal = int(opt("--challenge", "0"))
    race_secs = int(opt("--race", "300"))
    force_win = "--force-win" in av
    out = Path(opt("--out", str(ROOT / "log" / "c4_champ")))
    tag = opt("--tag", arm)
    out.mkdir(parents=True, exist_ok=True)
    shotdir = out / f"shots_{tag}"; shotdir.mkdir(exist_ok=True)

    env = dict(os.environ)
    env.setdefault("MASHED_WIN_POS", "primary-bl")
    if arm == "off":
        env.pop("MASHED_RE_NO_AUTO_HOOK", None); env["MASHED_HOOK_ONLY"] = "__none__"
    elif arm == "on":
        env.pop("MASHED_RE_NO_AUTO_HOOK", None); env["MASHED_HOOK_ONLY"] = RVA
    elif arm == "stock":
        env["MASHED_RE_NO_AUTO_HOOK"] = "1"; env["MASHED_HOOK_ONLY"] = "__none__"
    else:
        assert ctl_asi and Path(ctl_asi).exists(), "--ctl-asi required"
        env["MASHED_RE_NO_AUTO_HOOK"] = "1"; env["MASHED_HOOK_ONLY"] = RVA
    save = game / "gamesave.bin"
    reset = opt("--reset-save", None)   # pristine save copied over the copy's save before spawn
    if reset:
        import shutil; shutil.copyfile(reset, save)
    rec = {"arm": arm, "tag": tag, "root": str(game), "exe_sha256": sha256(exe),
           "deployed_asi_sha256": sha256(game / "mashed_re_dev.asi"),
           "ctl_asi": ctl_asi if arm == "ctl" else None, "ctl_asi_sha256": sha256(ctl_asi) if arm == "ctl" else None,
           "save_sha256_before": sha256(save) if save.exists() else None,
           "reset_save": reset, "challenge": chal, "force_win": force_win,
           "env": {k: env.get(k) for k in ("MASHED_HOOK_ONLY", "MASHED_RE_NO_AUTO_HOOK")},
           "log": [], "crash": []}
    def log(msg):
        print(msg, flush=True); rec["log"].append(msg)

    dev = frida.get_local_device()
    pid = dev.spawn(str(exe), cwd=str(game), env=env)
    rec["pid"] = pid; log(f"  spawned pid={pid} arm={arm} root={game}")
    sess = dev.attach(pid); hproc = _open_handle(pid)
    def on_msg(m, d):
        p = m.get("payload") if isinstance(m, dict) else None
        if isinstance(p, dict) and p.get("kind") == "crash":
            rec["crash"].append(p); print("  [CRASH]", p, flush=True)
        elif m.get("type") == "error":
            print("  agent err:", m.get("description"), flush=True)
    scr = sess.create_script(AGENT); scr.on("message", on_msg); scr.load()
    X = scr.exports_sync
    X.init(force_win)
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
            r = X.arm("off")
        else:
            r = None
            for _ in range(30):
                r = X.arm("on")
                if r.get("ok"): break
                time.sleep(0.5)
        rec["arm_info"] = r; rec["modinfo_at_arm"] = X.modinfo(); log(f"  arm: {r}  modinfo={rec['modinfo_at_arm']}")
        if not r or not r.get("ok"):
            verdict = "ARM-FAILED"; raise SystemExit
        # --- statenav Single Player entry, then mode row 0 (Challenge), no down-press ---
        nav.confirm_to_depth(2)
        log(f"  after GTS: depth={nav.depth()} sel={X.sel()}")
        nav.advance_past_load_modal(3)
        log(f"  single player: depth={nav.depth()} sel={X.sel()} (row 0 kept)")
        nav.confirm_to_depth(4, tries=4)
        log(f"  depth4: depth={nav.depth()} sel={X.sel()}")
        nav.confirm_to_depth(5, tries=4)
        log(f"  depth5 (challenge select): depth={nav.depth()} idx={X.idx()}")
        time.sleep(0.5)
        for _ in range(chal):
            nav.press(12); time.sleep(0.4)
        log(f"  challenge index 0x0067f17c={X.idx()} (wanted {chal})")
        rec["idx_at_launch"] = X.idx()
        nav.press(4); time.sleep(1.5)
        log(f"  after confirm: depth={nav.depth()} phase={nav.phase()} {X.state()}")
        for k in range(6):
            if nav.phase() != 3: break
            nav.press(4); time.sleep(1.5)
            log(f"  start-attempt {k}: depth={nav.depth()} phase={nav.phase()}")
        shoot(pid, shotdir / "race_enter.png")
        end = time.time() + race_secs; t_log = 0; t_pulse = 0; n_pulse = 0; t_call = None
        pulse_every = float(opt("--pulse", "1.0")); after = float(opt("--after", "12"))
        while time.time() < end:
            if not nav.alive(): log("   process exited during race"); break
            s = X.state()
            if s["sph"] == 3 and time.time() - t_pulse > pulse_every:
                X.press(4, 200); t_pulse = time.time(); n_pulse += 1
            if time.time() - t_log > 10:
                t_log = time.time(); log(f"   t+{int(race_secs-(end-time.time()))}s pulses={n_pulse} {s}")
            if s["calls"] > 0 and t_call is None:
                t_call = time.time(); log(f"  >>> body called: {s}"); shoot(pid, shotdir / "at_call.png")
            if t_call is not None and time.time() - t_call > after:
                break
            time.sleep(0.25)
        rec["race_pulses"] = n_pulse
        rec["final"] = X.state(); rec["modinfo_final"] = X.modinfo()
        shoot(pid, shotdir / "end.png")
        verdict = "CALLED" if rec["final"]["calls"] > 0 else "NOT-CALLED"
    except SystemExit:
        pass
    finally:
        try: rec.setdefault("final", X.state())
        except Exception: pass
        try: rec["dump"] = X.dump()
        except Exception: pass
        rec["alive_at_end"] = nav.alive()
        try:
            if nav.alive(): dev.kill(pid)
        except Exception: pass
        try: sess.detach()
        except Exception: pass
        rec["exit_code"] = _exit_code(hproc)
        rec["save_sha256_after"] = sha256(save) if save.exists() else None
        rec["verdict"] = verdict
        p = out / f"c4_champ_{tag}.json"
        p.write_text(json.dumps(rec, indent=1), encoding="utf-8")
        print(f"  verdict={verdict} calls={rec.get('final',{}).get('calls')} -> {p}", flush=True)
    return 0


if __name__ == "__main__":
    sys.exit(main())
