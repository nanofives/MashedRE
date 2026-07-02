# canonical_c4_racediff.py — Phase 2: in-RACE canonical C3->C4 diff (Stalker, frame-synced).
#
# Enabled by canonical_c4_determinism_canary.py proving the canonical race is BIT-DETERMINISTIC
# across runs (0 forcing needed). So a two-run OFF-vs-ON Stalker diff is comparable in-race, for
# the 491 playable-core C3 (vehicle/particle/gameplay/in-race hud+render) that only fire in a race.
#
# Same Stalker capture as canonical_c4_stalkerdiff.py (records every call ORIGINATING in the target
# body, direct + indirect, with args + EAX), but:
#   - scenario driver = drive to a canonical Quick race (nav_agent press), not menu nav.
#   - capture gate = a FRAME WINDOW (frames counted from race start via the input-poll hook),
#     not wall-clock, so OFF and ON capture the SAME deterministic frames -> comparable sets.
#
# OFF run = original (MASHED_HOOK_ONLY=__none__); ON run = reimpl subset (<=5 for boot stability).
# C4-CLEAN iff behavior sets equal+non-empty, jmp==0xe9. Indirect calls captured (no veto).
#
# Usage: py -3.12 re/frida/canonical_c4_racediff.py --rvas 0x..,0x.. [--lo 120 --hi 900]
import json, os, shutil, subprocess, sys, time, bisect
from pathlib import Path
import frida

ROOT = Path(__file__).resolve().parent.parent.parent
ORIG = ROOT / "original"; EXE = ORIG / "MASHED.exe"
LOG = (ROOT / "log") if (ROOT / "log").exists() else (ROOT.parent.parent / "log")
NAV = (Path(__file__).resolve().parent / "nav_agent.js").read_text(encoding="utf-8")

CAP = NAV + r"""
;(function(){
  // ---- shares nav_agent's top-level DELTA + abs() (set by init()) ----
  const IMG_LO = 0x00400000, IMG_HI = 0x00600000;
  const A4_CLOCK = 0x00470670;        // physics fn: fires per-car-per-frame, only in-race; deterministic clock
  let FLO = 300, FHI = 1200;
  function rvaOf(u){ return (u - DELTA) >>> 0; }
  function inImg(r){ return r >= IMG_LO && r < IMG_HI; }
  function nrm(v){ const r = rvaOf(v); if (inImg(r)) return 'i'+r.toString(16); return (v>>>0).toString(16); }

  let frame = 0, raceStarted = false;
  const SAMP = {}, CUR = {}, RANGE = {}, BUDGET = {};
  let activeTarget = null, activeLo = 0, activeHi = 0, activeTid = 0;
  const SAMPLES = 6;

  function gateOpen(){ return raceStarted && frame >= FLO && frame <= FHI; }
  function record(key, sig){ if(!SAMP[key]) SAMP[key]={}; SAMP[key][sig]=(SAMP[key][sig]||0)+1; }

  function stdcallNargs(tgtAbs){
    try { let pc=tgtAbs.toUInt32(), s=0;
      while(s<200){ s++; let i; try{ i=Instruction.parse(ptr(pc)); }catch(e){ break; }
        const m=i.mnemonic;
        if(m==='ret'||m==='retn'){ if(i.operands.length>0&&i.operands[0].type==='imm') return ptr(i.operands[0].value).toUInt32()>>>2; return 0; }
        if(m==='jmp'){ const op=i.operands[0]; if(op&&op.type==='imm'){ pc=ptr(op.value).toUInt32(); continue; } break; }
        pc=i.next.toUInt32();
      } } catch(e){}
    return -1;
  }
  function walkEnd(entryAbs){
    let pc=entryAbs.toUInt32(); const lo=pc; let s=0, lastRet=lo+0x40;
    while(s<3000){ s++; let i; try{ i=Instruction.parse(ptr(pc)); }catch(e){ break; }
      const m=i.mnemonic;
      if(m==='ret'||m==='retn'){ lastRet=i.next.toUInt32(); let nb=0; try{ nb=ptr(i.next).readU8(); }catch(e){}
        if(nb===0xcc||nb===0x90) return i.next.toUInt32(); }
      pc=i.next.toUInt32(); if(pc>lo+0x4000) break;
    }
    return lastRet;
  }
  function siteInAnyRange(site){ for(const k in RANGE){ const r=RANGE[k]; if(site>=r[0]&&site<r[1]) return true; } return false; }
  function makeCallout(site, op, nextAddr){
    const isImm=(op&&op.type==='imm'); const immT=isImm?(op.value>>>0):0;
    const opType=op?op.type:'none'; const opVal=op?op.value:null;
    let cdeclN=-1; try{ const nx=Instruction.parse(nextAddr); if(nx.mnemonic==='add'){ const o=nx.operands;
      if(o.length===2&&o[0].type==='reg'&&o[0].value==='esp'&&o[1].type==='imm') cdeclN=ptr(o[1].value).toUInt32()>>>2; } }catch(e){}
    return function(context){
      if (activeTarget===null) return;
      if (site<activeLo||site>=activeHi) return;
      let tgt;
      if (isImm) tgt=ptr(immT);
      else if (opType==='reg'){ try{ tgt=ptr(context[opVal]); }catch(e){ return; } }
      else if (opType==='mem'){ let a=ptr(0); const v=opVal;
        try{ if(v.base) a=a.add(context[v.base]); if(v.index) a=a.add((context[v.index].toUInt32?context[v.index].toUInt32():context[v.index])*(v.scale||1));
             a=a.add(v.disp||0); tgt=a.readPointer(); }catch(e){ return; } }
      else return;
      const tr=rvaOf(tgt.toUInt32());
      let n=cdeclN; if(n<0) n=stdcallNargs(tgt); if(n<0||n>8) n=0;
      const sp=context.esp; let parts=[ inImg(tr)?'c'+tr.toString(16):'x'+(tgt.toUInt32()>>>0).toString(16) ];
      for(let j=0;j<n;j++){ try{ parts.push(nrm(sp.add(j*4).readU32())); }catch(e){ parts.push('?'); } }
      CUR[activeTarget].push('['+parts.join(',')+']');
    };
  }
  function transform(iterator){
    let insn;
    while((insn=iterator.next())!==null){
      if(insn.mnemonic==='call' && siteInAnyRange(insn.address.toUInt32()))
        iterator.putCallout(makeCallout(insn.address.toUInt32(), insn.operands[0], insn.next));
      iterator.keep();
    }
  }
  function targetEntry(rva, mode){
    const a=abs(rva);
    if(mode==='off') return { entry:a, jmp:a.readU8() };
    const b=a.readU8(); if(b!==0xe9) return { entry:null, jmp:b };
    return { entry:a.add(5).add(a.add(1).readS32()), jmp:0xe9 };
  }

  rpc.exports.setupcap = function(targets, mode, flo, fhi){
    FLO=flo; FHI=fhi;
    // frame clock: A4 physics fires per-car-per-frame and ONLY in-race -> first call = race start.
    Interceptor.attach(abs(A4_CLOCK), { onEnter(a){ raceStarted=true; frame++; }});
    const out={};
    for(let i=0;i<targets.length;i++){
      const rva=targets[i][0]>>>0, offHi=targets[i][1]>>>0;
      const te=targetEntry(rva, mode);
      if(!te.entry){ out[rva]={ok:false, jmp:te.jmp}; continue; }
      let lo,hi;
      if(mode==='off'){ lo=te.entry.toUInt32(); hi=lo+((offHi-rva)>>>0); }
      else { lo=te.entry.toUInt32(); hi=walkEnd(te.entry); }
      RANGE[rva]=[lo,hi]; BUDGET[rva]=SAMPLES; CUR[rva]=null;
      out[rva]={ok:true, jmp:te.jmp, lo:lo, hi:hi, size:(hi-lo)};
      (function(key){
        Interceptor.attach(te.entry, {
          onEnter(a){
            if(activeTarget!==null) return;
            if(BUDGET[key]<=0) return;
            if(!gateOpen()) return;
            activeTarget=key; activeLo=RANGE[key][0]; activeHi=RANGE[key][1];
            activeTid=this.threadId; CUR[key]=[];
            try{ Stalker.follow(this.threadId, { transform:transform }); }catch(e){ activeTarget=null; }
          },
          onLeave(r){
            if(activeTarget!==key) return;
            try{ Stalker.unfollow(activeTid); }catch(e){}
            try{ Stalker.flush(); }catch(e){}
            CUR[key].push('eax='+nrm(r.toUInt32()));
            record(key, CUR[key].join(' '));
            BUDGET[key]--; CUR[key]=null; activeTarget=null;
          }
        });
      })(rva);
    }
    return out;
  };
  rpc.exports.frame = function(){ return raceStarted ? frame : -1; };
  rpc.exports.stopcap = function(){ try{ Interceptor.detachAll(); }catch(e){} return frame; };
  rpc.exports.samples = function(){ return SAMP; };
  rpc.exports.jmpbyte = function(rva){ try{ return abs((rva>>>0)).readU8(); }catch(e){ return -1; } };
})();
"""

def run(mode, rvas, targets, flo, fhi):
    canon = ROOT / "scripts" / "canonical" / "videocfg_windowed.bin"
    if canon.exists(): shutil.copy2(str(canon), str(ORIG / "videocfg.bin"))
    env = dict(os.environ); env["MASHED_RE_DEV"] = "1"
    env["MASHED_HOOK_ONLY"] = (",".join("0x%08x" % r for r in rvas) if mode == "on" else "__none__")
    NPG, DET = 0x00000200, 0x00000008
    dev = frida.get_local_device()
    proc = sess = None
    for _ in range(5):
        proc = subprocess.Popen([str(EXE)], cwd=str(ORIG), env=env,
            stdin=subprocess.DEVNULL, stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL,
            creationflags=NPG | DET)
        time.sleep(0.2)
        try: sess = dev.attach(proc.pid); break
        except Exception:
            try: proc.kill()
            except Exception: pass
            time.sleep(1.0)
    if sess is None: print(f"  [{mode}] attach failed"); return {}, {}
    scr = sess.create_script(CAP)
    scr.on("message", lambda m, d: print("  agent:", m.get("description")) if m["type"] == "error" else None)
    scr.load(); E = scr.exports_sync; E.init()
    samples, jmp = {}, {}

    def wait(pred, t, what):
        end = time.time()+t
        while time.time() < end:
            if pred(): return True
            time.sleep(0.1)
        return False
    def press(c, ms=180): E.press(c, ms); time.sleep(ms/1000.0+0.3)
    def confirm_to(tg, tries=6):
        for _ in range(tries):
            if E.depth() >= tg: return True
            press(4)
            if wait(lambda: E.depth() >= tg, 2.0, f"d{tg}"): return True
        return E.depth() >= tg
    try:
        if not wait(lambda: E.phase() == 3 and E.depth() >= 1, 30, "title"):
            print(f"  [{mode}] no title"); proc.kill(); return {}, {}
        time.sleep(1.0)
        E.setupcap(targets, mode, flo, fhi)          # arm BEFORE race so frame clock catches start
        confirm_to(2); time.sleep(0.4); press(4); time.sleep(0.8)
        confirm_to(3); E.setsel(1); time.sleep(0.3)
        confirm_to(4, 4); confirm_to(5, 4)
        press(4); time.sleep(1.5)
        for _ in range(5):
            if E.phase() != 3: break
            press(4); time.sleep(1.5)
        if E.phase() == 3: print(f"  [{mode}] not in race"); proc.kill(); return {}, {}
        print(f"  [{mode}] in race; waiting for frame window [{flo},{fhi}] ...")
        end = time.time() + 25
        while time.time() < end and E.frame() <= fhi:
            time.sleep(0.05)
        try: E.stopcap()                          # bound hot-path exposure once window closes
        except Exception: pass
        print(f"  [{mode}] frame={E.frame()}")
        samples = E.samples()
        if mode == "on":
            for r in rvas:
                try: jmp[r] = E.jmpbyte(r)
                except Exception: jmp[r] = -1
    finally:
        try: proc.kill()
        except Exception: pass
    return samples, jmp

def main():
    av = sys.argv
    def opt(f, d): return av[av.index(f)+1] if f in av else d
    rvas = [int(x, 16) for x in opt("--rvas", "").split(",") if x.strip()]
    if not rvas: print("need --rvas"); return 1
    flo = int(opt("--lo", "300")); fhi = int(opt("--hi", "1200"))   # A4-call (frame) window
    LOG.mkdir(parents=True, exist_ok=True)
    hooks = ROOT / "hooks.csv"
    if not hooks.exists(): hooks = ROOT.parent.parent / "hooks.csv"
    fs = []
    for line in hooks.read_text(encoding="utf-8", errors="ignore").splitlines():
        if not line or line[0] == "#" or line.startswith("rva,"): continue
        c = line.split(",", 1)[0].strip()
        try: fs.append(int(c, 16))
        except ValueError: pass
    fs = sorted(set(fs))
    def boundary(r):
        i = bisect.bisect_right(fs, r); return fs[i] if i < len(fs) else (r + 0x1800)
    targets = [[r, boundary(r)] for r in rvas]
    print(f"  targets={[hex(r) for r in rvas]}  frame-window=[{flo},{fhi}]")

    print("\n=== OFF race ==="); off_s, _ = run("off", rvas, targets, flo, fhi)
    print("=== ON race ===");  on_s, on_jmp = run("on", rvas, targets, flo, fhi)

    print("\n=== VERDICT (in-race, frame-synced, indirect captured) ===")
    out = {}; n_clean = 0
    for r in rvas:
        offset = set((off_s.get(r, off_s.get(str(r), {})) or {}).keys())
        onset = set((on_s.get(r, on_s.get(str(r), {})) or {}).keys())
        installed = (on_jmp.get(r, on_jmp.get(str(r), -1)) == 0xe9)
        why = []
        if not offset: why.append("orig-not-observed")
        if not onset: why.append("modded-not-observed")
        if offset and onset and offset != onset: why.append("BEHAVIOR-DIFF")
        if not installed: why.append("no-JMP")
        clean = bool(offset) and bool(onset) and offset == onset and installed
        verdict = "C4-CLEAN" if clean else "HOLD"
        if clean: n_clean += 1
        def trunc(s): return sorted({(x[:120] + '..' if len(x) > 122 else x) for x in s})
        out[r] = {"rva": "%#010x" % r, "verdict": verdict, "why": why, "installed": installed,
                  "off_n": len(offset), "on_n": len(onset), "off": trunc(offset)[:4], "on": trunc(onset)[:4]}
        print(f"  {r:#010x} -> {verdict}  {','.join(why)}  (off {len(offset)} sigs, on {len(onset)} sigs)")
        if verdict != "C4-CLEAN":
            print(f"        off={trunc(offset)[:3]}"); print(f"        on ={trunc(onset)[:3]}")
    (LOG / "c4_racediff_result.json").write_text(json.dumps(out, indent=2), encoding="utf-8")
    print(f"\n  {n_clean}/{len(rvas)} C4-CLEAN  ->  log/c4_racediff_result.json")
    return 0

if __name__ == "__main__":
    sys.exit(main())
