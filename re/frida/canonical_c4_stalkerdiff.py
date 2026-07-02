# canonical_c4_stalkerdiff.py — Stalker v2 canonical-scenario C4 diff (C3 -> C4).
#
# Unlike canonical_c4_autodiff.py (static CFG-walk -> can't resolve indirect/vtable calls, so it
# HOLDs them), this FOLLOWS the target's actual execution with Stalker and records every call that
# ORIGINATES IN THE TARGET BODY -- direct AND indirect -- resolving callee + args at runtime. The
# indirect/vtable draws that v1 could only flag are now captured, so there is NO indirect veto:
# if the captured call trace (incl. indirect) + EAX match OFF vs ON, the function is verified.
#
# Body range: OFF = [rva, next-func-start] (hooks.csv). ON = [reimpl_entry, reimpl_end], where
# reimpl_entry is read from the installed E9 JMP and reimpl_end from a short walk to padded ret.
# A call is recorded iff its SITE (call instruction address) lies in the active target's body, so
# nested calls inside callees are ignored (depth handled by address range, not ret-tracking).
#
# Perf: the transform only inserts a callout on call instructions whose site is inside some target
# body, and Stalker follows only during the target's own execution, for a few samples per target.
# Do NOT run on hot (>~1000/s) targets (fire-filter first).
#
# 2 spawns/batch:  OFF capture (body from hooks.csv) ; ON capture (reimpl range discovered at setup).
#
# Usage: py -3.12 re/frida/canonical_c4_stalkerdiff.py --rvas 0x00472c60,0x004730b0 [--nav 4,4]
import json, os, sys, time, bisect
from pathlib import Path
import frida
try: import psutil
except ImportError: psutil = None

ROOT = Path(__file__).resolve().parent.parent.parent
def _orig(root):
    c = root / "original" / "MASHED.exe"
    return c if c.exists() else root.parent.parent / "original" / "MASHED.exe"
EXE = _orig(ROOT); ORIG = EXE.parent
LOG = (ROOT / "log") if (ROOT / "log").exists() else (ROOT.parent.parent / "log")

AGENT = r'''
'use strict';
const IMGBASE = 0x00400000, IMG_LO = 0x00400000, IMG_HI = 0x00600000;
let DELTA = 0;
const RVA_RESOLVER = 0x00497310;
let NAV = [], SETTLE = 7000, DWELL = 1500, t0 = 0, lastSlot = -99, GATE = 0;
const SAMPLES = 4;
const SAMP = {};           // rva -> {sig: count}
const CUR = {};            // rva -> in-flight array | null
const RANGE = {};          // rva -> [lo, hi]   (absolute)
const BUDGET = {};         // rva -> remaining Stalker samples
let activeTarget = null, activeLo = 0, activeHi = 0, activeTid = 0;

function setDelta(){ const m = Process.findModuleByName('MASHED.exe') || Process.enumerateModules()[0];
  DELTA = m.base.toUInt32() - IMGBASE; }
function abs(rva){ return ptr((rva + DELTA) >>> 0); }
function rvaOf(u){ return (u - DELTA) >>> 0; }
function inImg(r){ return r >= IMG_LO && r < IMG_HI; }
function nrm(v){ const r = rvaOf(v); if (inImg(r)) return 'i'+r.toString(16); return (v>>>0).toString(16); }
function navControl(){ const tt=Date.now()-t0-SETTLE; if(tt<0||!NAV.length) return -1;
  const k=Math.floor(tt/DWELL); if(k>=NAV.length) return -1; return NAV[k]; }
function record(key, sig){ if(Date.now()<GATE) return; if(!SAMP[key]) SAMP[key]={};
  SAMP[key][sig]=(SAMP[key][sig]||0)+1; }

// nargs at a call: cdecl post-call 'add esp,N' (passed) else callee 'ret N' (disasm target).
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
// reimpl body end: linear walk to a ret followed by int3/nop padding (our .asi is /O2-padded).
function walkEnd(entryAbs){
  let pc=entryAbs.toUInt32(); const lo=pc; let s=0, lastRet=lo+0x40;
  while(s<3000){ s++; let i; try{ i=Instruction.parse(ptr(pc)); }catch(e){ break; }
    const m=i.mnemonic;
    if(m==='ret'||m==='retn'){ lastRet=i.next.toUInt32();
      let nb=0; try{ nb=ptr(i.next).readU8(); }catch(e){}
      if(nb===0xcc||nb===0x90) return i.next.toUInt32(); }
    pc=i.next.toUInt32(); if(pc>lo+0x4000) break;
  }
  return lastRet;
}
function siteInAnyRange(site){ for(const k in RANGE){ const r=RANGE[k]; if(site>=r[0]&&site<r[1]) return true; } return false; }

function makeCallout(site, op, nextAddr){
  const isImm = (op && op.type==='imm');
  const immT = isImm ? (op.value>>>0) : 0;
  const opType = op ? op.type : 'none';
  const opVal = op ? op.value : null;          // reg name string | mem {base,index,scale,disp}
  let cdeclN = -1;
  try { const nx=Instruction.parse(nextAddr); if(nx.mnemonic==='add'){ const o=nx.operands;
    if(o.length===2&&o[0].type==='reg'&&o[0].value==='esp'&&o[1].type==='imm') cdeclN=ptr(o[1].value).toUInt32()>>>2; } } catch(e){}
  return function(context){
    if (activeTarget===null) return;
    if (site < activeLo || site >= activeHi) return;
    let tgt;
    if (isImm) tgt = ptr(immT);
    else if (opType==='reg'){ try{ tgt = ptr(context[opVal]); }catch(e){ return; } }
    else if (opType==='mem'){ let a=ptr(0); const v=opVal;
      try{ if(v.base) a=a.add(context[v.base].toUInt32?context[v.base]:context[v.base]);
           if(v.index) a=a.add((context[v.index].toUInt32?context[v.index].toUInt32():context[v.index])*(v.scale||1));
           a=a.add(v.disp||0); tgt=a.readPointer(); }catch(e){ return; } }
    else return;
    const tr = rvaOf(tgt.toUInt32());
    let n = cdeclN; if (n < 0) n = stdcallNargs(tgt); if (n < 0 || n > 8) n = 0;  // unknown -> 0 (avoid stack garbage)
    const sp = context.esp;                      // at call site (pre-CALL): [esp]=arg0
    let parts = [ inImg(tr) ? 'c'+tr.toString(16) : 'x'+(tgt.toUInt32()>>>0).toString(16) ];
    for (let j=0;j<n;j++){ try{ parts.push(nrm(sp.add(j*4).readU32())); }catch(e){ parts.push('?'); } }
    CUR[activeTarget].push('['+parts.join(',')+']');
  };
}
function transform(iterator){
  let insn;
  while ((insn = iterator.next()) !== null){
    if (insn.mnemonic==='call' && siteInAnyRange(insn.address.toUInt32()))
      iterator.putCallout(makeCallout(insn.address.toUInt32(), insn.operands[0], insn.next));
    iterator.keep();
  }
}

function targetEntry(rva, mode){
  const a = abs(rva);
  if (mode==='off') return { entry: a, jmp: a.readU8() };
  const b = a.readU8(); if (b!==0xe9) return { entry: null, jmp: b };
  return { entry: a.add(5).add(a.add(1).readS32()), jmp: 0xe9 };
}

rpc.exports = {
  setupcap: function(targets, mode, nav, settle, dwell){
    setDelta(); NAV=nav; SETTLE=settle; DWELL=dwell; t0=Date.now();
    GATE = t0 + SETTLE + NAV.length*DWELL + 800;
    try { Interceptor.attach(abs(RVA_RESOLVER), {
      onEnter(a){ const sp=this.context.esp; this.p=sp.add(4).readS32(); this.c=sp.add(8).readS32(); },
      onLeave(ret){ const k=navControl(); if(k>=0&&this.p===0&&this.c===k){
        const slot=Math.floor((Date.now()-t0-SETTLE)/DWELL); if(slot===lastSlot) return; lastSlot=slot; ret.replace(ptr(0xff)); } }
    }); } catch(e){ send({kind:'err',msg:'navarm '+e}); }
    const out = {};
    for (let i=0;i<targets.length;i++){
      const rva = targets[i][0]>>>0, offHi = targets[i][1]>>>0;
      const te = targetEntry(rva, mode);
      if (!te.entry){ out[rva]={ok:false, jmp:te.jmp}; continue; }
      let lo, hi;
      if (mode==='off'){ lo = te.entry.toUInt32(); hi = lo + ((offHi - rva)>>>0); }
      else { lo = te.entry.toUInt32(); hi = walkEnd(te.entry); }
      RANGE[rva] = [lo, hi]; BUDGET[rva] = SAMPLES; CUR[rva] = null;
      out[rva] = { ok:true, jmp: te.jmp, lo: lo, hi: hi, size: (hi-lo) };
      (function(key){
        Interceptor.attach(te.entry, {
          onEnter(a){
            if (activeTarget!==null) return;                 // nested under another target: skip
            if (BUDGET[key] <= 0) return;
            if (Date.now() < GATE) return;
            activeTarget = key; activeLo = RANGE[key][0]; activeHi = RANGE[key][1];
            activeTid = this.threadId; CUR[key] = [];
            try { Stalker.follow(this.threadId, { transform: transform }); } catch(e){ activeTarget=null; }
          },
          onLeave(r){
            if (activeTarget!==key) return;
            try { Stalker.unfollow(activeTid); } catch(e){}
            try { Stalker.flush(); } catch(e){}
            CUR[key].push('eax='+nrm(r.toUInt32()));
            record(key, CUR[key].join(' '));
            BUDGET[key]--; CUR[key]=null; activeTarget=null;
          }
        });
      })(rva);
    }
    return out;
  },
  samples: function(){ return SAMP; },
  jmpbyte: function(rva){ try { return abs((rva>>>0)).readU8(); } catch(e){ return -1; } }
};
send({kind:'ready'});
'''

def _spawn(mode, only_rvas):
    env = dict(os.environ); env["MASHED_RE_DEV"] = "1"
    env["MASHED_HOOK_ONLY"] = (",".join("0x%08x" % r for r in only_rvas) if mode == "on" else "__none__")
    dev = frida.get_local_device()
    pid = dev.spawn(str(EXE), cwd=str(ORIG), env=env)
    sess = dev.attach(pid)
    def on_msg(m, d):
        if m.get("type") == "error": print("   agent err:", m.get("description")); return
        p = m.get("payload", {})
        if p.get("kind") in ("info", "err"): print("   ", p.get("msg"))
    scr = sess.create_script(AGENT); scr.on("message", on_msg); scr.load()
    return dev, pid, sess, scr

def _kill(dev, pid, sess):
    try:
        if not psutil or psutil.pid_exists(pid): dev.kill(pid)
    except Exception: pass
    try: sess.detach()
    except Exception: pass

def capture(mode, rvas, targets, nav, settle, dwell, seconds):
    dev, pid, sess, scr = _spawn(mode, rvas if mode == "on" else [])
    samples, jmp, ranges = {}, {}, {}
    try:
        if mode == "on":
            dev.resume(pid)
            deadline = time.time() + 14
            while time.time() < deadline:
                if psutil and not psutil.pid_exists(pid): break
                try:
                    if scr.exports_sync.jmpbyte(rvas[0]) == 0xe9: break
                except Exception: pass
                time.sleep(0.5)
            ranges = scr.exports_sync.setupcap(targets, mode, nav, settle, dwell)
            # ON: process already resumed; settle/nav clock starts now (t0 reset in setupcap)
        else:
            ranges = scr.exports_sync.setupcap(targets, mode, nav, settle, dwell)
            dev.resume(pid)
        t = time.time() + seconds
        while time.time() < t:
            if psutil and not psutil.pid_exists(pid): print("   exited early"); break
            time.sleep(0.25)
        try: samples = scr.exports_sync.samples()
        except Exception: pass
        if mode == "on":
            for r in rvas:
                try: jmp[r] = scr.exports_sync.jmpbyte(r)
                except Exception: jmp[r] = -1
    finally:
        _kill(dev, pid, sess)
    return samples, jmp, ranges

def main():
    av = sys.argv
    def opt(f, d): return av[av.index(f)+1] if f in av else d
    if "--rvas-file" in av:
        rvas = [int(x, 16) for x in Path(opt("--rvas-file", "")).read_text().split() if x.strip()]
    else:
        rvas = [int(x, 16) for x in opt("--rvas", "0x00472c60").split(",") if x.strip()]
    nav = [int(x) for x in opt("--nav", "4,4").split(",")]
    settle = int(opt("--settle", "7000")); dwell = int(opt("--dwell", "1500")); seconds = int(opt("--seconds", "18"))
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
    print(f"  targets={[hex(r) for r in rvas]}  nav={nav}")

    print("\n=== OFF capture (Stalker over original bodies) ===")
    off_s, _, off_rng = capture("off", rvas, targets, nav, settle, dwell, seconds)
    for r in rvas:
        rg = off_rng.get(r, off_rng.get(str(r), {})) or {}
        print(f"  {r:#010x}  off body size={rg.get('size','?')}")
    print("=== ON capture (Stalker over reimpl bodies) ===")
    on_s, on_jmp, on_rng = capture("on", rvas, targets, nav, settle, dwell, seconds)

    print("\n=== VERDICT (indirect calls captured; no veto) ===")
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
            print(f"        off={trunc(offset)[:3]}")
            print(f"        on ={trunc(onset)[:3]}")
    (LOG / "c4_stalkerdiff_result.json").write_text(json.dumps(out, indent=2), encoding="utf-8")
    print(f"\n  {n_clean}/{len(rvas)} C4-CLEAN  ->  log/c4_stalkerdiff_result.json")
    return 0

if __name__ == "__main__":
    sys.exit(main())
