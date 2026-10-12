# U-9667: dump the ORIGINAL's live per-slot menu tables, read-only.
#
# 0x0042ac90 (MenuEntryGet) reads, with no other memory access:
#   0x0042ac90  mov ecx,[0x67e9f8]        slot index (menu-stack depth)
#   0x0042ac99  mov edx,[ecx*0x40+0x67ed38]  table pointer of that slot
#   0x0042ac9f  mov ecx,[ecx*0x40+0x67ed40]  cursor of that slot
#   then table words [edx+eax*4].
# This script reads exactly those globals (plus the whole 0x40-byte slot record and the
# PTR_005f7638 screen->table array, 34 dwords) with plain Memory reads. No Interceptor in the
# dump agent. Table words are read until 0xff070000 inclusive (cap 512) plus 32 trailing
# words, so an offline walk that does not stop on 0xff070000 can be followed past it.
#
# Two lanes, each in its own boot of original/MASHED.exe (MASHED_RE_NO_AUTO_HOOK=1):
#   --lane push     from the root menu, FUN_0043d2a0(id,0) each screen id 0..33 (27 skipped:
#                   PTR_005f7638[27] = 0), dump, FUN_0043d2a0(0,1) pop. Same mechanism as
#                   re/frida/orig_nav_hold.py --scan (validated there 2026-10-05).
#   --lane input    statenav.py's in-process FUN_00497310 override (4=confirm, 11=up, 12=down)
#                   to walk the real input path: title -> depth 2 -> depth 3, then at each depth
#                   step the cursor down until it repeats and up once from item 0, dumping after
#                   every press, then select the --route cursor for that level. An entry/leave
#                   probe on 0x0042ac90 (single caller 0x0043e4ca, once per select, not per
#                   frame) records depth / table / cursor / return of every real call.
# Kills ONLY the pid it spawned.
#
# Usage: py -3.12 re/frida/u9667_menu_table_dump.py --lane push|input --out <json>
import argparse, json, os, sys, time
from pathlib import Path
import frida

ROOT = Path(__file__).resolve().parent.parent.parent
sys.path.insert(0, str(ROOT / 're' / 'frida'))
ORIG = ROOT / "original"; EXE = ORIG / "MASHED.exe"

DUMP_AGENT = r'''
'use strict';
const IMG=0x00400000; let DELTA=0;
const RVA_DEPTH=0x0067e9f8, RVA_SLOT=0x0067ed38, RVA_PHASE=0x0067eca4, RVA_PTRS=0x005f7638;
const RVA_NAV=0x0043d2a0;
// mode = DAT_0067e9fc (game mode), row = DAT_0067f17c (challenge row); added for U-9667 fix traces.
const T7=0xff070000;
let nav=null;
function abs(r){return ptr(r+DELTA);}
function u32(p){ return p.readU32(); }
function readTable(p){
  const words=[]; let term=-1, err=null;
  try{
    for(let i=0;i<512;i++){ const w=u32(p.add(i*4)); words.push(w); if(w===T7){ term=i; break; } }
  }catch(e){ err=''+e; }
  const tail=[];
  if(term>=0){
    try{ for(let i=1;i<=32;i++) tail.push(u32(p.add((term+i)*4))); }catch(e){ }
  }
  return {words:words, term_index:term, cap_hit:(term<0 && err===null), err:err, tail:tail};
}
rpc.exports={
  init:function(){
    const m=Process.findModuleByName('MASHED.exe')||Process.enumerateModules()[0];
    DELTA=m.base.toUInt32()-IMG;
    nav=new NativeFunction(abs(RVA_NAV),'void',['int','int']);
    return DELTA;
  },
  phase:function(){ return abs(RVA_PHASE).readS32(); },
  depth:function(){ return abs(RVA_DEPTH).readS32(); },
  push:function(scr){ nav(scr,0); return abs(RVA_DEPTH).readS32(); },
  pop:function(){ nav(0,1); return abs(RVA_DEPTH).readS32(); },
  ptrs:function(){ const o=[]; for(let i=0;i<34;i++) o.push(u32(abs(RVA_PTRS+i*4))); return o; },
  dump:function(nslots){
    const depth=abs(RVA_DEPTH).readS32();
    const n=Math.max(nslots, depth+1);
    const slots=[];
    for(let s=0;s<n;s++){
      const base=abs(RVA_SLOT+s*0x40);
      const rec=[]; for(let k=0;k<16;k++) rec.push(u32(base.add(k*4)));
      const tp=rec[0], cur=rec[2]|0;
      let tab=null;
      if(tp!==0){ tab=readTable(ptr(tp)); }
      slots.push({slot:s, rec:rec, table_ptr:tp, cursor:cur, table:tab});
    }
    return {depth:depth, phase:abs(RVA_PHASE).readS32(), mode:abs(0x0067e9fc).readS32(),
            row:abs(0x0067f17c).readS32(), slots:slots};
  }
};
'''


def spawn():
    env = dict(os.environ); env["MASHED_RE_NO_AUTO_HOOK"] = "1"
    dev = frida.get_local_device()
    pid = dev.spawn(str(EXE), cwd=str(ORIG), env=env)
    sess = dev.attach(pid)
    return dev, pid, sess


def load(sess, src):
    s = sess.create_script(src); s.on("message", lambda m, d: None); s.load(); return s


def wait_phase(E, want, secs=30):
    end = time.time() + secs
    while time.time() < end:
        try:
            if E.phase() == want:
                return True
        except Exception:
            pass
        time.sleep(0.2)
    return False


def lane_push(out):
    rec = {"lane": "push", "events": [], "dumps": []}
    todo = [i for i in range(34) if i != 27]
    rec["skipped"] = {27: "PTR_005f7638[27] = 0 (file and live)"}
    boots = 0
    while todo and boots < 6:
        boots += 1
        dev, pid, sess = spawn()
        D = load(sess, DUMP_AGENT).exports_sync
        D.init(); dev.resume(pid)
        ok = wait_phase(D, 1)
        time.sleep(1.5)
        rec["events"].append(f"boot{boots} pid={pid} phase1={ok} depth={D.depth()}")
        if boots == 1:
            rec["ptrs_live"] = D.ptrs()
            rec["dumps"].append({"tag": "root", "screen": None, **D.dump(4)})
        try:
            while todo:
                scr = todo[0]
                d0 = D.depth()
                d1 = D.push(scr)
                time.sleep(0.6)
                dmp = D.dump(4)
                rec["dumps"].append({"tag": f"push{scr}", "screen": scr, "depth_before": d0,
                                     "depth_after_push": d1, **dmp})
                d2 = D.pop()
                rec["events"].append(f"push {scr}: depth {d0}->{d1}, pop->{d2}")
                todo.pop(0)
                time.sleep(0.35)
        except Exception as e:
            rec["events"].append(f"boot{boots} died at screen {todo[0]}: {e}")
            todo.pop(0)
        finally:
            try: dev.kill(pid)
            except Exception: pass
    Path(out).write_text(json.dumps(rec, indent=1), encoding="utf-8")
    print("\n".join(rec["events"]))


MEG_AGENT = r'''
'use strict';
// Entry/leave probe on 0x0042ac90 ONLY. Its single caller is 0x0043e4ca (the select path of
// FUN_0043dfd0), so it fires once per select, not per frame.
const IMG=0x00400000; let DELTA=0; const calls=[];
function abs(r){return ptr(r+DELTA);}
rpc.exports={
  init:function(){
    const m=Process.findModuleByName('MASHED.exe')||Process.enumerateModules()[0];
    DELTA=m.base.toUInt32()-IMG;
    Interceptor.attach(abs(0x0042ac90),{
      onEnter:function(){
        const d=abs(0x0067e9f8).readS32();
        this.e={depth:d, table_ptr:abs(0x0067ed38+d*0x40).readU32(),
                cursor:abs(0x0067ed40+d*0x40).readS32(), t:Date.now()};
      },
      onLeave:function(r){ this.e.ret=r.toUInt32(); calls.push(this.e); }
    });
    return DELTA;
  },
  calls:function(){ return calls; }
};
'''


def lane_input(out, route, tag, max_items=12):
    """route: cursor value to select at each level ('m1' = -1). Every level: sweep the cursor
    down until it repeats, then up from the top item, dumping after every press."""
    import statenav
    from statenav import Nav
    rec = {"lane": "input", "route": route, "events": [], "dumps": []}
    dev, pid, sess = spawn()
    S = load(sess, statenav.AGENT); S.exports_sync.init()
    D = load(sess, DUMP_AGENT).exports_sync; D.init()
    M = load(sess, MEG_AGENT).exports_sync; M.init()
    dev.resume(pid)
    nav = Nav(S, pid)

    def top():
        d = D.dump(4); return d, d["slots"][d["depth"]]

    def snap(t):
        d, s = top()
        rec["dumps"].append({"tag": t, "screen": None, **d})
        rec["events"].append(f"{t}: depth={d['depth']} phase={d['phase']} ptr={hex(s['table_ptr'])} cursor={s['cursor']}"
                             f" mode={d.get('mode')} row={d.get('row')}")
        print(rec["events"][-1], flush=True)
        return s["cursor"]

    def move_to(target):
        for _ in range(max_items):
            c = top()[1]["cursor"]
            if c == target: return True
            nav.press(12 if (target == -1 or c < target) else 11); time.sleep(0.25)
        return top()[1]["cursor"] == target

    try:
        wait_phase(D, 3, 30); time.sleep(1.0)
        snap("boot")
        nav.confirm_to_depth(2); snap("depth2")
        r = nav.advance_past_load_modal(3); rec["events"].append(f"advance_past_load_modal(3) -> {r}")
        time.sleep(0.8)
        for lvl, want in enumerate(route):
            if not nav.alive():
                rec["events"].append("process gone"); break
            d, s = top(); dep = d["depth"]
            lt = f"L{lvl}-d{dep}"
            vals = [snap(f"{lt}/c")]
            for k in range(max_items):
                nav.press(12); time.sleep(0.3)
                c = snap(f"{lt}/down{k+1}")
                if c in vals: break
                vals.append(c)
            move_to(0)
            nav.press(11); time.sleep(0.3); vals.append(snap(f"{lt}/up-from-0"))
            rec["events"].append(f"{lt} cursor values seen: {sorted(set(vals))}")
            target = -1 if want == "m1" else int(want)
            ok = move_to(target)
            snap(f"{lt}/at{target}")
            ncalls = len(M.calls())
            nav.press(4); time.sleep(1.5)
            d2, _ = top()
            newc = M.calls()[ncalls:]
            rec["events"].append(f"{lt} confirm at cursor {target} (reached={ok}): depth {dep}->{d2['depth']} "
                                 f"phase->{d2['phase']} 0x0042ac90 calls={newc}")
            print(rec["events"][-1], flush=True)
            if d2["depth"] <= dep:
                snap(f"{lt}/after-confirm"); break
    except Exception as e:
        rec["events"].append(f"error: {e}")
    finally:
        try: rec["meg_calls"] = M.calls()
        except Exception: pass
        try: dev.kill(pid)
        except Exception: pass
    Path(out).write_text(json.dumps(rec, indent=1), encoding="utf-8")


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--lane", choices=["push", "input"], required=True)
    ap.add_argument("--out", required=True)
    ap.add_argument("--route", default="0,0,0,0,0",
                    help="input lane: cursor to select per level, comma list, m1 = -1")
    a = ap.parse_args()
    Path(a.out).parent.mkdir(parents=True, exist_ok=True)
    if a.lane == "push":
        lane_push(a.out)
    else:
        lane_input(a.out, a.route.split(","), a.route)


if __name__ == "__main__":
    main()
