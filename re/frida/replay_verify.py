# Deterministic-replay VERIFICATION — ATTACH variant (matches record_attach.py origin).
#
# replay_session.py uses dev.spawn(), but 001 was recorded via record_attach.py
# (capture="attach"): frameCount starts at ATTACH time, and Frida-spawn crashes MASHED
# at boot (layout perturbation, see project-boot-crash-static-init-not-heap). So a
# spawn-replay can neither boot reliably nor align frame origins with an attach-recorded
# trace. This tool replays by ATTACH instead:
#   - resolves IDirectInputDevice8::GetDeviceState (vtable slot 9) by dinput8 vtable scan
#     (same resolver as record_attach.py; DI8Create already fired by attach time),
#   - per GDS(cb=256) frame, writes the recorded down-key set into MASHED's OWN buffer
#     IN-PROCESS (no OS injection; CANONICAL_INPUT_DESIGN.md compliant),
#   - one-shot-instruments the scenario's coverage_candidate RVAs, recording first-hit frame,
#   - diffs the replay coverage (which RVAs fired, at what frame) against the recorded
#     coverage.tsv -> a determinism verdict.
#
# Does NOT spawn and does NOT kill: you launch MASHED (direct launch boots reliably),
# pass its PID; this attaches + detaches only. Multi-session safe.
#
# Usage:
#   py -3.12 re/frida/replay_verify.py --pid <PID> re/scenarios/001-nav-demo
import os, sys, time, json, argparse, subprocess
from pathlib import Path
import frida
try: import psutil
except ImportError: psutil = None

ROOT = Path(__file__).resolve().parent.parent.parent

AGENT = r'''
'use strict';
let frameCount=0, t0=0;
let PLAN=[], pi=0, cur=[], writes=0;
const COV=[]; const _toDetach=[];
let textLo=null, textHi=null;
function inText(p){ return textLo && p.compare(textLo)>=0 && p.compare(textHi)<0; }
function addText(mod){ mod.enumerateRanges('r-x').forEach(function(r){ const e=r.base.add(r.size);
  if(textLo===null||r.base.compare(textLo)<0)textLo=r.base; if(textHi===null||e.compare(textHi)>0)textHi=e; }); }

function onFrame(buf){
  frameCount++; if(t0===0) t0=Date.now();
  while (pi<PLAN.length && PLAN[pi].f<=frameCount){ cur=PLAN[pi].d; pi++; }
  for (const k of cur){ try{ buf.add(k).writeU8(0x80); writes++; }catch(e){} }  // IN-PROCESS only
}
const seen={};
function hookGDS(addr){ const k=addr.toString(); if(seen[k]) return; seen[k]=1;
  Interceptor.attach(addr,{ onEnter(a){ this.cb=a[1].toInt32(); this.buf=a[2]; },
                            onLeave(){ if(this.cb===256) onFrame(this.buf); } }); }
function resolveGDS(){
  let armed=0;
  ['dinput8_real.DLL','DINPUT8.dll','dinput8.dll'].forEach(function(mn){
    const mod=Process.findModuleByName(mn); if(!mod) return; addText(mod);
    mod.enumerateRanges('r--').forEach(function(r){
      let a=r.base; const end=r.base.add(r.size).sub(4); let runStart=null, runLen=0;
      function flush(){ if(runStart!==null && runLen>=16){ try{ const g=runStart.add(9*4).readPointer();
        if(inText(g)){ hookGDS(g); armed++; } }catch(e){} } runStart=null; runLen=0; }
      while(a.compare(end)<0){
        let p=null; try{ p=a.readPointer(); }catch(e){}
        if(p && inText(p)){ if(runStart===null){runStart=a;runLen=0;} runLen++; } else flush();
        a=a.add(4);
      }
      flush();
    });
  });
  return armed;
}
let N_GDS = resolveGDS();
// attached-early (during boot) -> dinput8 not loaded yet; keep retrying until armed.
if (N_GDS === 0) {
  const iv = setInterval(function(){ const n = resolveGDS(); if (n > 0) { N_GDS = n; clearInterval(iv);
    send({kind:'info', msg:'GetDeviceState armed late='+n}); } }, 100);
}

function armCoverage(modName, imgBase, rvas){
  const mod=Process.findModuleByName(modName); if(!mod){ send({kind:'err',msg:'cov: '+modName+' missing'}); return 0; }
  let n=0;
  rvas.forEach(function(rva){ try{ const addr=mod.base.add(rva-imgBase); const h={hit:false,rva:rva};
    h.L=Interceptor.attach(addr,{ onEnter(){ if(h.hit)return; h.hit=true; COV.push({rva:rva,f:frameCount,t:Date.now()-t0}); _toDetach.push(h.L); } }); n++;
  }catch(e){} });
  setInterval(function(){ if(_toDetach.length){ const d=_toDetach.splice(0); d.forEach(function(L){try{L.detach();}catch(e){}}); } },200);
  return n;
}
// Crash catcher: on a fatal fault, dump the ordered MASHED return-address chain off
// the stack (spells the real call chain — better than minidump's deduped scan).
const MX_LO=0x400000, MX_HI=0x995000;
let CRASH=null;
// spin stub (`jmp $`) — redirect the faulting thread here so the process stays alive
// long enough to deliver the crash message (eip=0 would otherwise terminate instantly).
const _spin = Memory.alloc(Process.pageSize);
Memory.patchCode(_spin, 2, function(p){ p.writeByteArray([0xeb,0xfe]); });
const FATAL={'access-violation':1,'illegal-instruction':1,'abort':1,'bus-error':1,'division-by-zero':1,'stack-overflow':1};
let firstChanceN = 0;
function chain(ctx){ const a=[]; try{ const esp=ptr(ctx.esp.toString());
  for(let i=0;i<320;i++){ let v; try{ v=esp.add(i*4).readU32(); }catch(e){ break; }
    if(v>=MX_LO && v<MX_HI) a.push('+0x'+(i*4).toString(16)+':0x'+v.toString(16)); } }catch(e){} return a; }
Process.setExceptionHandler(function(d){
  if(!FATAL[d.type]) return false;
  const ctx=d.context;
  const pc = ctx.pc ? parseInt(ctx.pc.toString(),16) : 0;
  // PRIMARY (eip != 0): log first-chance and let the app's own handler run (return false),
  // so we capture the real gameplay fault before the corrupted shutdown.
  if(pc !== 0 && !CRASH){
    firstChanceN++;
    if(firstChanceN <= 6){
      send({kind:'firstchance', av:{n:firstChanceN, type:d.type, eip:ctx.pc.toString(),
        mem:(d.memory?(''+d.memory.address+'/'+d.memory.operation):'?'),
        eax:''+ctx.eax, ebx:''+ctx.ebx, ecx:''+ctx.ecx, edx:''+ctx.edx, esi:''+ctx.esi, edi:''+ctx.edi,
        ebp:''+ctx.ebp, esp:''+ctx.esp, code_chain:chain(ctx).slice(0,20), frame:frameCount, idx:pi}});
    }
    return false;   // let the app handle it (it may recover or proceed to shutdown)
  }
  if(CRASH) return true;
  // eip == 0 (the secondary shutdown crash): park + report
  CRASH={type:d.type, eip:(ctx.pc?ctx.pc.toString():'?'), address:d.address.toString(),
    eax:''+ctx.eax, ecx:''+ctx.ecx, edx:''+ctx.edx, esi:''+ctx.esi, edi:''+ctx.edi,
    ebp:''+ctx.ebp, esp:''+ctx.esp,
    mem:(d.memory?(''+d.memory.address+'/'+d.memory.operation):'?'),
    code_chain:chain(ctx).slice(0,40), frame:frameCount, idx:pi};
  send({kind:'crash', crash:CRASH});
  ctx.pc = _spin;     // park the thread in a tight loop; we'll kill the pid from Python
  return true;        // resume (into the spin) so the message is delivered
});
rpc.exports = {
  setplan: function(p){ PLAN=p; pi=0; cur=[]; return PLAN.length; },
  armcoverage: function(m,b,r){ return armCoverage(m,b,r); },
  status: function(){ return {frames:frameCount, idx:pi, writes:writes, cur:cur, cov:COV.length, gds:N_GDS}; },
  dump: function(){ return {frames:frameCount, writes:writes, cov:COV, gds:N_GDS, crash:CRASH}; }
};
send({kind:'ready', msg:'replay-verify armed; GetDeviceState vtable hooks='+N_GDS});
'''


def load_trace(scn_dir: Path):
    plan = []
    with open(scn_dir / "input_trace.tsv", encoding="utf-8") as f:
        next(f, None)
        for line in f:
            parts = line.rstrip("\n").split("\t")
            if len(parts) < 3:
                continue
            frame = int(parts[0]); dik_csv = parts[2].strip()
            diks = [] if (not dik_csv or dik_csv == "(none)") else \
                   [int(x.replace("0x", ""), 16) for x in dik_csv.split(",") if x.strip()]
            plan.append({"f": frame, "d": diks})
    return plan


def first_input_frame(plan):
    for ev in plan:
        if ev["d"]:
            return ev["f"]
    return plan[0]["f"] if plan else 0


def load_recorded_cov(scn_dir: Path):
    rows = {}
    cf = scn_dir / "coverage.tsv"
    if not cf.exists():
        return rows
    with open(cf, encoding="utf-8") as f:
        next(f, None)
        for line in f:
            p = line.rstrip("\n").split("\t")
            if len(p) < 2:
                continue
            rva = int(p[0], 16)
            rows[rva] = {"first_frame": int(p[1]),
                         "name": p[3] if len(p) > 3 else "",
                         "conf": p[5] if len(p) > 5 else ""}
    return rows


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("scenario")
    ap.add_argument("--pid", type=int, default=0, help="attach to an already-running MASHED")
    ap.add_argument("--launch", action="store_true",
                    help="launch the scenario's binary ourselves and attach ASAP (during boot) so the "
                         "menu-poll frame counter starts fresh at menu-appear; dodges the frontend.mpg "
                         "movie-crash window. Kills ONLY the pid we launch.")
    ap.add_argument("--fps", default="60", help="MASHED_FPS_CAP for --launch (default 60)")
    ap.add_argument("--seconds", type=int, default=0)
    ap.add_argument("--lead", type=int, default=0,
                    help="shift plan so the first INPUT lands at this replay-frame "
                         "(compresses the leading menu-idle to dodge the attract-timeout crash; "
                         "0 = keep recorded timing). Coverage drift is reported vs the shifted origin.")
    args = ap.parse_args()

    scn_dir = Path(args.scenario)
    if not scn_dir.is_absolute():
        scn_dir = ROOT / scn_dir
    meta = json.loads((scn_dir / "meta.json").read_text(encoding="utf-8"))
    img_base = int(meta.get("image_base", "0x00400000"), 16)
    mod_name = Path(meta["exe"]).name
    cand = [int(x, 16) for x in meta.get("coverage_candidates", [])]
    plan = load_trace(scn_dir)
    rec_cov = load_recorded_cov(scn_dir)

    # --lead: shift the whole plan so the first INPUT event lands at frame `lead`.
    # Preserves relative spacing of every event (that is what drives menu->race), only
    # the leading idle is compressed. delta is added to recorded frames in the verdict.
    delta = 0
    if args.lead > 0 and plan:
        fi = first_input_frame(plan)
        delta = args.lead - fi
        for ev in plan:
            ev["f"] = max(1, ev["f"] + delta)

    last_frame = plan[-1]["f"] if plan else 0
    seconds = args.seconds or max(120, int(last_frame / 50) + 60)

    print(f"=== replay-verify {scn_dir.name} on pid {args.pid} ===")
    print(f"  module={mod_name} base=0x{img_base:08x}  plan-events={len(plan)}  last-frame={last_frame}")
    print(f"  coverage candidates={len(cand)}  recorded-hits={len(rec_cov)}  window={seconds}s  lead-shift={delta:+d}")

    dev = frida.get_local_device()
    we_launched = False
    pid = args.pid
    if args.launch:
        exe = Path(meta["exe"])
        env = dict(os.environ)
        env["MASHED_FPS_CAP"] = str(args.fps)
        env["MASHED_RE_NO_AUTO_HOOK"] = "1"   # stock original behavior (no installed hooks)
        proc = subprocess.Popen([str(exe)], cwd=str(exe.parent), env=env)
        pid = proc.pid; we_launched = True
        print(f"  launched {exe.name} pid={pid} (fps={args.fps}); attaching ASAP...")
        sess = None
        for _ in range(200):  # ~20s of retries; attach the instant frida can
            try:
                sess = dev.attach(pid); break
            except Exception:
                time.sleep(0.1)
        if sess is None:
            print("  error: could not attach");
            try: proc.kill()
            except Exception: pass
            return 3
        print(f"  attached at +{0}; GDS resolver will arm when dinput8 loads")
    else:
        if not pid:
            print("error: pass --pid <PID> or --launch"); return 2
        if psutil and not psutil.pid_exists(pid):
            print(f"error: pid {pid} not running"); return 2
        sess = dev.attach(pid)

    crash_box = {}
    def on_msg(m, d):
        if m.get("type") == "error":
            print("  agent error:", m.get("description")); return
        p = m.get("payload", {})
        if p.get("kind") in ("ready", "err"):
            print("  [agent]", p.get("msg"))
        elif p.get("kind") == "firstchance":
            a = p.get("av", {})
            print(f"\n  >>> FIRST-CHANCE {a.get('type')} eip={a.get('eip')} mem={a.get('mem')} @frame={a.get('frame')} idx={a.get('idx')}")
            print(f"      eax={a.get('eax')} ebx={a.get('ebx')} ecx={a.get('ecx')} edx={a.get('edx')} esi={a.get('esi')} edi={a.get('edi')} ebp={a.get('ebp')} esp={a.get('esp')}")
            print( "      stack code-chain: " + " ".join(a.get("code_chain", [])[:12]))
            crash_box.setdefault("av", []).append(a)
        elif p.get("kind") == "crash":
            crash_box["c"] = p.get("crash")
            c = p.get("crash", {})
            print(f"\n  *** CRASH CAUGHT *** type={c.get('type')} eip={c.get('eip')} mem={c.get('mem')}"
                  f"  @frame={c.get('frame')} plan-idx={c.get('idx')}")
            print(f"      eax={c.get('eax')} ecx={c.get('ecx')} edx={c.get('edx')} esi={c.get('esi')} edi={c.get('edi')} ebp={c.get('ebp')} esp={c.get('esp')}")
            print( "      MASHED stack call-chain (esp-order):")
            for e in c.get("code_chain", [])[:24]:
                print("        " + e)

    scr = sess.create_script(AGENT); scr.on("message", on_msg); scr.load()
    n = scr.exports_sync.setplan(plan)
    scr.exports_sync.armcoverage(mod_name, img_base, cand)
    print(f"  plan loaded ({n} events), coverage armed; replaying...\n")

    dump = {"frames": 0, "writes": 0, "cov": [], "gds": 0}
    try:
        deadline = time.time() + seconds
        all_hit_at = None
        while time.time() < deadline:
            if crash_box.get("c"):
                print("\n  crash captured; stopping."); break
            if psutil and not psutil.pid_exists(pid):
                print("\n  game process exited (crash or close)."); break
            try:
                st = scr.exports_sync.status()
                cur_names = ",".join("0x%02x" % k for k in st["cur"]) or "(none)"
                print(f"\r  frame={st['frames']}  plan-idx={st['idx']}/{len(plan)}  writes={st['writes']}"
                      f"  cov={st['cov']}/{len(cand)}  held={cur_names}   ", end="", flush=True)
                # early-finish: stop ~5s after plan done AND all coverage seen
                if st["idx"] >= len(plan) and st["cov"] >= len(cand) and all_hit_at is None:
                    all_hit_at = time.time()
                if all_hit_at and time.time() - all_hit_at > 5:
                    print("\n  plan complete + all coverage hit; stopping early."); break
            except Exception:
                pass
            time.sleep(1.0)
    except KeyboardInterrupt:
        print("\n  interrupted.")
    finally:
        try: dump = scr.exports_sync.dump()
        except Exception: pass
        try: sess.detach()
        except Exception: pass
        if we_launched:                      # kill ONLY the pid we launched
            try:
                if (not psutil) or psutil.pid_exists(pid): dev.kill(pid)
            except Exception: pass

    # ---- verdict ----
    print("\n\n=== REPLAY COVERAGE vs RECORDED ===")
    rep = {c["rva"]: c for c in dump.get("cov", [])}
    print(f"  GDS hooks armed: {dump.get('gds',0)}   frames replayed: {dump.get('frames',0)}   input writes: {dump.get('writes',0)}")
    # expected replay frame = recorded frame + lead-shift delta; drift = actual - expected.
    print(f"  (expected replay frame = recorded + lead-shift {delta:+d}; drift = actual - expected)")
    print(f"  {'rva':<12}{'name':<26}{'rec_f':>8}{'exp_f':>8}{'rep_f':>8}{'drift':>8}  status")
    hits = 0; drifts = []
    for rva in cand:
        rc = rec_cov.get(rva); rp = rep.get(rva)
        name = (rc or {}).get("name", "")
        rf = rc["first_frame"] if rc else None
        ef = (rf + delta) if rf is not None else None
        pf = rp["f"] if rp else None
        if rp:
            hits += 1
            dr = (pf - ef) if ef is not None else None
            if dr is not None: drifts.append(dr)
            print(f"  0x{rva:08x}  {name:<24}{(rf if rf is not None else '-'):>8}{(ef if ef is not None else '-'):>8}{pf:>8}{(f'{dr:+d}' if dr is not None else 'n/a'):>8}  HIT")
        else:
            print(f"  0x{rva:08x}  {name:<24}{(rf if rf is not None else '-'):>8}{(ef if ef is not None else '-'):>8}{'-':>8}{'-':>8}  MISS")
    if drifts:
        import statistics
        print(f"\n  drift: min={min(drifts):+d} max={max(drifts):+d} mean={statistics.mean(drifts):+.0f} (frames)")
    print(f"  coverage reproduced: {hits}/{len(cand)}")
    if hits == len(cand):
        print("  VERDICT: replay re-reached the recorded state (all coverage RVAs fired).")
    elif hits == 0:
        print("  VERDICT: replay reached NONE of the recorded coverage — state not re-reached.")
    else:
        print("  VERDICT: PARTIAL — some coverage reproduced; see MISS rows.")
    return 0


if __name__ == "__main__":
    sys.exit(main())
