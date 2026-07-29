# State-aware menu navigation: instead of fixed-dwell taps (which desync past ~3 screens),
# poll the menu-stack depth (DAT_0067e9f8) / phase (DAT_0067eca4) and issue each input only
# when the expected screen is reached, waiting for the transition before the next input. This
# is the reliable deep-nav prerequisite for Part 2 (reach Player Colour Select, then race setup).
#
# Input is the in-process FUN_00497310 return override (4=confirm,11=up,12=down,...) — OS-input
# free. After reaching Player Colour Select it PROBES each control to find the player-JOIN
# (the control that advances past colour-select), since blind confirm does not.
#
# Usage: py -3.12 re/frida/statenav.py [--seconds N] [--shot-dir verify/p2]
import ctypes, json, os, sys, time
from ctypes import wintypes
from pathlib import Path
import frida
try: import psutil
except ImportError: psutil = None

ROOT = Path(__file__).resolve().parent.parent.parent
ORIG = ROOT / "original"; EXE = ORIG / "MASHED.exe"

def _find_hwnd(pid):
    u=ctypes.windll.user32; found=[]
    proto=ctypes.WINFUNCTYPE(wintypes.BOOL,wintypes.HWND,wintypes.LPARAM)
    def cb(h,_):
        p=wintypes.DWORD(); u.GetWindowThreadProcessId(h,ctypes.byref(p))
        if p.value==pid and u.IsWindowVisible(h):
            n=u.GetWindowTextLengthW(h)
            if n:
                b=ctypes.create_unicode_buffer(n+1); u.GetWindowTextW(h,b,n+1)
                if "MASHED" in b.value.upper(): found.append(h)
        return True
    u.EnumWindows(proto(cb),0); return found[0] if found else None
def shoot(pid, path):
    try:
        from PIL import Image
        h=_find_hwnd(pid)
        if not h: return False
        u=ctypes.windll.user32; g=ctypes.windll.gdi32
        r=wintypes.RECT(); u.GetClientRect(h,ctypes.byref(r)); w,ht=r.right,r.bottom
        if w<=0 or ht<=0: return False
        hdc=u.GetDC(h); md=g.CreateCompatibleDC(hdc); bm=g.CreateCompatibleBitmap(hdc,w,ht); g.SelectObject(md,bm)
        u.PrintWindow(h,md,2)
        class BH(ctypes.Structure):
            _fields_=[("biSize",wintypes.DWORD),("biWidth",wintypes.LONG),("biHeight",wintypes.LONG),
                      ("biPlanes",wintypes.WORD),("biBitCount",wintypes.WORD),("biCompression",wintypes.DWORD),
                      ("biSizeImage",wintypes.DWORD),("biXPelsPerMeter",wintypes.LONG),("biYPelsPerMeter",wintypes.LONG),
                      ("biClrUsed",wintypes.DWORD),("biClrImportant",wintypes.DWORD)]
        bi=BH(); bi.biSize=ctypes.sizeof(BH); bi.biWidth=w; bi.biHeight=-ht; bi.biPlanes=1; bi.biBitCount=32
        buf=(ctypes.c_char*(w*ht*4))(); g.GetDIBits(md,bm,0,ht,buf,ctypes.byref(bi),0)
        Image.frombuffer("RGBA",(w,ht),bytes(buf),"raw","BGRA",0,1).convert("RGB").save(str(path))
        g.DeleteObject(bm); g.DeleteDC(md); u.ReleaseDC(h,hdc); print(f"  [shot] {path}"); return True
    except Exception as e: print("  [shot] err",e); return False

AGENT = r'''
'use strict';
const IMG=0x00400000; let DELTA=0;
const RVA_RES=0x00497310, RVA_DEPTH=0x0067e9f8, RVA_PHASE=0x0067eca4, RVA_SEL=0x0067ed40;
let pressCtrl=-1, pressUntil=0; const CNT={};
function abs(r){return ptr(r+DELTA);}
rpc.exports={
  init:function(){ const m=Process.findModuleByName('MASHED.exe')||Process.enumerateModules()[0];
    DELTA=m.base.toUInt32()-IMG;
    Interceptor.attach(abs(RVA_RES),{ onEnter(a){const sp=this.context.esp;this.p=sp.add(4).readS32();this.c=sp.add(8).readS32();},
      onLeave(ret){ if(this.p===0 && this.c===pressCtrl && Date.now()<pressUntil) ret.replace(ptr(0xff)); }});
    return DELTA; },
  press:function(c,ms){ pressCtrl=c; pressUntil=Date.now()+ms; return 1; },
  clear:function(){ pressCtrl=-1; return 1; },
  countthese:function(rvas){ rvas.forEach(function(r){ const a=parseInt(r,16); CNT[r]=0;
    try{ Interceptor.attach(abs(a),{onEnter:function(){CNT[r]++;}}); }catch(e){ CNT[r]=-1; } }); return 1; },
  // Count entries into OUR reimplementation by its .asi EXPORT, not by the patched original
  // RVA: the inline JMP installed at the RVA makes an RVA-anchored probe ambiguous, while the
  // export address is unambiguously the ported body.
  countexports:function(mod, names){ names.forEach(function(n){ const k='exp:'+n; CNT[k]=0;
    // Frida 17 removed the static Module.findExportByName; go through the module object.
    try{ const m=Process.findModuleByName(mod); if(!m){ CNT[k]=-2; return; }
         const p=m.findExportByName(n); if(!p){ CNT[k]=-2; return; }
         Interceptor.attach(p,{onEnter:function(){CNT[k]++;}}); }catch(e){ CNT[k]=-1; } }); return 1; },
  counts:function(){ return CNT; },
  peek:function(rva){ try{ return abs(parseInt(rva,16)).readU32(); }catch(e){ return 0; } },
  // U-9025 semaphore tracer. The wedge is the GUI thread parked in
  // WaitForSingleObject([0x007dcae0], INFINITE) at 0x005a8406 on a binary semaphore
  // (CreateSemaphoreA init=1 max=1) that probes WAIT_TIMEOUT -> an UNPAIRED ACQUIRE.
  // Instrument the two IMPORTED functions (not the 30 in-cluster call sites): patching
  // 30 mid-function CALLs is 30 chances to relocate wrongly, whereas the import is one
  // attach point and `this.returnAddress` still names the exact call site (site+6).
  // Both are filtered on handle == *[semGlobal] read LIVE each call, because the handle
  // does not exist yet at spawn time.
  semtrace:function(rvaHex, iatWaitHex, iatRelHex){
    const g = abs(parseInt(rvaHex,16));
    let pW, pR;
    try { pW = abs(parseInt(iatWaitHex,16)).readPointer(); } catch(e) { pW = null; }
    try { pR = abs(parseInt(iatRelHex,16)).readPointer(); } catch(e) { pR = null; }
    // Fallback only if the loader has not bound the IAT yet.
    if (!pW || pW.isNull()) { const k=Process.getModuleByName('kernel32.dll'); pW=k.findExportByName('WaitForSingleObject'); }
    if (!pR || pR.isNull()) { const k=Process.getModuleByName('kernel32.dll'); pR=k.findExportByName('ReleaseSemaphore'); }
    const held = {};            // tid -> net acquires (re-entrancy shows as depth>1)
    let seq = 0, diag = 0;
    const mod = Process.findModuleByName('MASHED.exe') || Process.enumerateModules()[0];
    const lo = mod.base, hi = mod.base.add(mod.size);
    function sem(){ try { return g.readU32(); } catch(e) { return 0; } }
    // DIAGNOSTIC: the handle filter is only sound if [semGlobal] is actually populated and
    // actually passed. A first smoke run logged ZERO events, which is indistinguishable
    // between "never called" and "filter wrong" -- so the first 40 calls made FROM MASHED
    // code are logged unconditionally, with both the passed handle and the live global.
    function diagOk(ra){ return diag < 6 && ra.compare(lo) >= 0 && ra.compare(hi) < 0; }
    // A bare return address is not evidence of WHO called. Two wedge traces showed waits on
    // this handle from addresses outside MASHED.exe, which cannot be attributed without the
    // owning module -- so every site is reported as "module+0xoffset".
    function where(p){ try { const m = Process.findModuleByAddress(p);
        return m ? (m.name + '+0x' + p.sub(m.base).toUInt32().toString(16)) : ('UNMAPPED@' + p);
      } catch(e) { return 'ERR@' + p; } }
    Interceptor.attach(pW, {
      onEnter(a){ const s = sem();
        this.hit = (s !== 0 && a[0].toUInt32() === s);
        const tid = Process.getCurrentThreadId();
        if (!this.hit && diagOk(this.returnAddress)) { diag++;
          send({kind:'sem', ev:'diag-wait', seq:seq++, tid:tid,
                site:where(this.returnAddress), handle:a[0].toUInt32(),
                semGlobal:s, timeout:a[1].toUInt32()}); }
        if (this.hit) {
          send({kind:'sem', ev:'wait-enter', seq:seq++, tid:tid,
                site:where(this.returnAddress), timeout:a[1].toUInt32(),
                depth:(held[tid]||0)}); } },
      onLeave(r){ if (!this.hit) return; const tid = Process.getCurrentThreadId();
        const rv = r.toUInt32(); if (rv === 0) held[tid] = (held[tid]||0) + 1;
        send({kind:'sem', ev:'wait-leave', seq:seq++, tid:tid, rv:rv,
              depth:(held[tid]||0)}); } });
    Interceptor.attach(pR, {
      onEnter(a){ const s = sem();
        if (s === 0 || a[0].toUInt32() !== s) return;
        const tid = Process.getCurrentThreadId(); held[tid] = (held[tid]||0) - 1;
        send({kind:'sem', ev:'release', seq:seq++, tid:tid,
              site:where(this.returnAddress), count:a[1].toUInt32(),
              depth:held[tid]}); } });
    // 2026-07-28: the first hunt caught a wedge whose trace showed the FIRST EVER gated wait
    // blocking, with zero prior acquires and zero releases. A semaphore created init=1 cannot
    // block its first waiter, so the count was consumed through an API this pair does not
    // cover -- and the wedged process had 10 threads parked in ZwWaitForMultipleObjects.
    // Cover every wait API that can consume a semaphore count, plus the creation itself so
    // the runtime initial count is observed rather than inferred from the disassembly.
    const k32 = Process.getModuleByName('kernel32.dll');
    const pC = k32.findExportByName('CreateSemaphoreA');
    if (pC) Interceptor.attach(pC, {
      onEnter(a){ this.init = a[1].toInt32(); this.max = a[2].toInt32(); },
      onLeave(r){ send({kind:'sem', ev:'create', seq:seq++,
                        tid:Process.getCurrentThreadId(), handle:r.toUInt32(),
                        site:where(this.returnAddress),
                        init:this.init, max:this.max, semGlobal:sem()}); } });
    // (nCount, lpHandles, ...) forms. `which` is the index of our handle in the array.
    ['WaitForMultipleObjects','WaitForMultipleObjectsEx',
     'MsgWaitForMultipleObjects','MsgWaitForMultipleObjectsEx'].forEach(function(nm){
      const p = k32.findExportByName(nm); if (!p) return;
      Interceptor.attach(p, {
        onEnter(a){ this.which = -1; const s = sem(); if (s === 0) return;
          const n = a[0].toUInt32(); if (n === 0 || n > 64) return;
          try { for (let i = 0; i < n; i++) {
                  if (a[1].add(i*4).readU32() === s) { this.which = i; break; } } } catch(e) { return; }
          if (this.which < 0) return;
          send({kind:'sem', ev:'multi-enter', api:nm, seq:seq++,
                tid:Process.getCurrentThreadId(), site:where(this.returnAddress),
                n:n, which:this.which}); },
        onLeave(r){ if (this.which < 0) return;
          send({kind:'sem', ev:'multi-leave', api:nm, seq:seq++,
                tid:Process.getCurrentThreadId(), rv:r.toUInt32(), which:this.which}); } });
    });
    // Thread census. The stock/hooked control showed stock running the stream lock on a
    // dedicated worker while the hooked build runs it on the GUI thread. Log EVERY
    // CreateThread with its caller, start routine and resulting tid, so "the worker was
    // never spawned" can be checked against the tids that actually take the lock rather
    // than inferred. Unfiltered on purpose: a filtered census cannot show an absence.
    const pT = k32.findExportByName('CreateThread');
    const pGTI = k32.findExportByName('GetThreadId');
    // GetThreadId(hThread) resolves the handle to the tid the lock traffic is reported under,
    // so "which thread was spawned" and "which thread takes the lock" are the same key.
    const getTid = pGTI ? new NativeFunction(pGTI, 'uint32', ['pointer'], 'stdcall') : null;
    if (pT) Interceptor.attach(pT, {
      onEnter(a){ this.start = a[2]; this.site = where(this.returnAddress); },
      onLeave(r){ let tid = 0; try { if (getTid && !r.isNull()) tid = getTid(r); } catch(e) {}
        send({kind:'sem', ev:'thread-create', seq:seq++,
              by:Process.getCurrentThreadId(), site:this.site,
              start:where(this.start), tid:tid}); } });
    const pWx = k32.findExportByName('WaitForSingleObjectEx');
    if (pWx) Interceptor.attach(pWx, {
      onEnter(a){ const s = sem(); this.hit = (s !== 0 && a[0].toUInt32() === s);
        if (this.hit) send({kind:'sem', ev:'waitex-enter', seq:seq++,
                            tid:Process.getCurrentThreadId(),
                            site:where(this.returnAddress)}); },
      onLeave(r){ if (this.hit) send({kind:'sem', ev:'waitex-leave', seq:seq++,
                                      tid:Process.getCurrentThreadId(), rv:r.toUInt32()}); } });
    return {wait:pW.toString(), release:pR.toString()};
  },
  depth:function(){ try{return abs(RVA_DEPTH).readS32();}catch(e){return -999;} },
  phase:function(){ try{return abs(RVA_PHASE).readS32();}catch(e){return -999;} },
  sel:function(){ try{const d=abs(RVA_DEPTH).readS32(); return abs(RVA_SEL+d*0x40).readS32();}catch(e){return -999;} }
};
// Exception-handler EIP catcher (same class as poll_attach_catch_crash.py, but in-process so
// it survives a nav-driven AV). Reports the faulting pc, its owning module, and the registers.
Process.setExceptionHandler(function (d) {
  // Only AVs. C++ `throw` surfaces here as a RaiseException in KERNELBASE and is normal
  // traffic on this path — reporting it drowns the signal we are bisecting for.
  if (d.type !== 'access-violation') return false;
  try {
    const pc = d.context.pc;
    const m = Process.findModuleByAddress(pc);
    send({kind:'crash', type:d.type, pc:pc.toString(),
          module: m ? (m.name+'+0x'+pc.sub(m.base).toUInt32().toString(16)) : 'UNMAPPED',
          addr: d.memory ? (d.memory.operation+' @ '+d.memory.address) : null,
          regs: {eax:d.context.eax.toString(), ebx:d.context.ebx.toString(),
                 ecx:d.context.ecx.toString(), edx:d.context.edx.toString(),
                 esi:d.context.esi.toString(), edi:d.context.edi.toString(),
                 esp:d.context.esp.toString(), ebp:d.context.ebp.toString()},
          stack: (function(){ const o=[]; for (let i=0;i<10;i++){
                    try{ const v=d.context.esp.add(i*4).readPointer();
                         const mm=Process.findModuleByAddress(v);
                         o.push('[esp+'+(i*4)+']='+v+(mm?(' '+mm.name+'+0x'+v.sub(mm.base).toUInt32().toString(16)):''));
                    }catch(e){ o.push('[esp+'+(i*4)+']=?'); } } return o; })()});
  } catch (e) {}
  return false;   // let it propagate; we only observe
});
send({kind:'ready'});
'''

def _open_handle(pid):
    # PROCESS_QUERY_LIMITED_INFORMATION | SYNCHRONIZE — kept open so the exit code is
    # still readable after the process dies (the PID alone is not enough).
    return ctypes.windll.kernel32.OpenProcess(0x1000 | 0x00100000, False, pid)

def _exit_code(h):
    if not h: return None
    c = wintypes.DWORD()
    if not ctypes.windll.kernel32.GetExitCodeProcess(h, ctypes.byref(c)): return None
    return c.value

class Nav:
    def __init__(s, scr, pid): s.scr=scr; s.pid=pid
    def depth(s): return s.scr.exports_sync.depth()
    def phase(s): return s.scr.exports_sync.phase()
    def alive(s): return (not psutil) or psutil.pid_exists(s.pid)
    def press(s, c, ms=180): s.scr.exports_sync.press(c, ms); time.sleep(ms/1000.0+0.25)
    def wait(s, pred, timeout=8.0, what=""):
        end=time.time()+timeout
        while time.time()<end:
            if not s.alive(): print("   process exited"); return False
            if pred(): return True
            time.sleep(0.1)
        print(f"   wait timeout: {what} (depth={s.depth()} phase={s.phase()})"); return False
    def confirm_to_depth(s, target, tries=6):
        for _ in range(tries):
            if s.depth()>=target: return True
            s.press(4)
            if s.wait(lambda: s.depth()>=target, 2.0): return True
        return s.depth()>=target

def main():
    seconds=int(sys.argv[sys.argv.index("--seconds")+1]) if "--seconds" in sys.argv else 60
    shotdir=sys.argv[sys.argv.index("--shot-dir")+1] if "--shot-dir" in sys.argv else "verify/p2"
    Path(ROOT/shotdir).mkdir(parents=True, exist_ok=True)
    # --hooks: leave the dev .asi ARMED (default is stock, input-drive only). Used to
    # behaviourally exercise ported hooks on the menu-driven loading path, which
    # scenario_launch.py cannot reach (it pokes DAT_00771968=2 and bypasses the loader).
    hooks_on = "--hooks" in sys.argv
    count_exports = []
    if "--count-export" in sys.argv:
        count_exports = sys.argv[sys.argv.index("--count-export")+1].split(",")
    env=dict(os.environ)
    if not hooks_on:
        env["MASHED_RE_NO_AUTO_HOOK"]="1"   # stock; we only drive input
    else:
        env.pop("MASHED_RE_NO_AUTO_HOOK", None)
        print("  [hooks] dev .asi ARMED")
    dev=frida.get_local_device(); pid=dev.spawn(str(EXE),cwd=str(ORIG),env=env); sess=dev.attach(pid)
    hproc=_open_handle(pid)
    sess.on("detached", lambda reason, *a: print(f"  [detached] reason={reason} exit_code=0x{(_exit_code(hproc) or 0):08x}"))
    def _on_msg(m, d):
        p = m.get("payload") if isinstance(m, dict) else None
        if isinstance(p, dict) and p.get("kind") == "crash":
            print("  [CRASH] " + json.dumps(p, indent=2))
        elif isinstance(p, dict) and p.get("kind") == "sem":
            # One line per event, flushed, so the trace survives a wedge (the GUI thread
            # blocks but Frida's thread keeps delivering).
            print("  [SEM] " + json.dumps(p, sort_keys=True), flush=True)
    scr=sess.create_script(AGENT); scr.on("message", _on_msg); scr.load()
    scr.exports_sync.init()
    # representative gameplay/results-gated HOLD hooks (non-hot: init/event/results, not per-frame)
    # RESULTS/round-end subset (the 12 that were 0 in Time Trial) + a few in-race positives.
    # These fire on round END in the competitive arena (Quick Battle), which ends on
    # elimination/timeout with the AI playing it out -> no driving needed.
    GAMEPLAY = ["0x00423b40","0x00423b60","0x00423c40","0x00424920","0x00422fd0",
                "0x0040b6b0","0x00431d80","0x0046c700","0x004241b0","0x00424100",
                "0x00492340","0x0046c790","0x0045ba00","0x00408a70","0x00436810"]
    scr.exports_sync.countthese(GAMEPLAY)
    # Ad-hoc entry counters, e.g. MASHED_COUNT_RVAS=0x005bb000,0x005aef00. Counting is by RVA
    # on purpose here: the question is "did control REACH this address", which an RVA probe
    # answers correctly even when our inline JMP is installed there.
    extra_rvas = [r.strip() for r in os.environ.get("MASHED_COUNT_RVAS","").split(",") if r.strip()]
    # MASHED_COUNT_LATE=1 — arm the counters AFTER the race is entered instead of
    # before resume. Arming early means every probe's Interceptor is live through
    # the whole menu navigation, and CLAUDE.md's rule is explicit that attaching
    # on hot paths destabilises MASHED in seconds. Measured 2026-07-29: a
    # 48-probe pre-screen never left the frontend, a 24-probe one immediately
    # after DID reach a race, three more 24-probe runs worked, then two whose
    # candidate sets include hot RenderWare functions (0x004b40f0, 0x004c7730,
    # 0x004c5860) failed the same way — phase stuck at 3, first_results_at=None.
    # I first attributed that to a progressive d3d9/GPU wedge. THAT WAS WRONG and
    # the run order disproves it: a wedged driver does not heal itself for the
    # next boot, yet the 24-probe run right after the failed 48-probe one worked.
    # The failures track the probe SET, not elapsed boots. Late arming keeps the
    # nav at native speed and costs only the pre-race baseline.
    count_late = os.environ.get("MASHED_COUNT_LATE") == "1"
    if extra_rvas and not count_late:
        scr.exports_sync.countthese(extra_rvas)
        print(f"  [count-rvas] {extra_rvas}", flush=True)
    elif extra_rvas:
        print(f"  [count-rvas] {len(extra_rvas)} DEFERRED until in-race "
              f"(MASHED_COUNT_LATE=1)", flush=True)
    if os.environ.get("MASHED_SEMTRACE"):
        # U-9025: trace acquire/release of the binary semaphore at [0x007dcae0].
        r = scr.exports_sync.semtrace("0x007dcae0", "0x005cc090", "0x005cc094")
        print(f"  [SEM] tracer armed wait={r['wait']} release={r['release']}", flush=True)
    dev.resume(pid)
    nav=Nav(scr,pid)
    if count_exports:
        # The .asi is loaded by the dinput8 proxy AFTER resume, so its exports do not exist
        # at spawn time; retry until the module is present.
        armed=False
        for _ in range(40):
            time.sleep(0.5)
            try:
                scr.exports_sync.countexports("mashed_re_dev.asi", count_exports)
                cc=scr.exports_sync.counts()
                if all(cc.get("exp:"+n, -2) >= 0 for n in count_exports):
                    armed=True; break
            except Exception:
                pass
        print(f"  [count-export] {count_exports} armed={armed}")
    # A wedged run never reaches the end-of-run counts dump, so the numbers that matter are
    # lost exactly when the failure is interesting. Snapshot at each milestone instead.
    def dump_counts(tag):
        if not extra_rvas: return
        try:
            cc = scr.exports_sync.counts()
            print(f"  [counts @{tag}] " + " ".join(f"{r}={cc.get(r)}" for r in extra_rvas), flush=True)
        except Exception as e:
            print(f"  [counts @{tag}] err {e}", flush=True)
    print("  booting...")
    nav.wait(lambda: nav.phase()==3 and nav.depth()>=1, 18.0, "title up")
    print(f"  title: depth={nav.depth()} phase={nav.phase()}")
    dump_counts("title")
    # confirm title -> GTS (depth 2), then dismiss the Load-Successful modal (extra confirm)
    nav.confirm_to_depth(2); time.sleep(0.3); nav.press(4); time.sleep(0.5)
    print(f"  after GTS+modal: depth={nav.depth()} sel={scr.exports_sync.sel()}")
    shoot(pid, ROOT/shotdir/"sn_gts.png")
    # GTS cursor on Single Player(0). confirm -> Single Player mode-select (depth 3)
    nav.confirm_to_depth(3)
    print(f"  single player: depth={nav.depth()} sel={scr.exports_sync.sel()}")
    # mode-select: down ONCE to Quick Battle(1) (the competitive arena: rounds END on
    # elimination/timeout -> end-of-round scoring fires WITHOUT driving a lap). confirm -> colour.
    nav.press(12)
    print(f"  mode sel after down (Quick Battle): sel={scr.exports_sync.sel()}")
    nav.confirm_to_depth(4, tries=4)
    print(f"  colour-select: depth={nav.depth()} sel={scr.exports_sync.sel()}")
    shoot(pid, ROOT/shotdir/"sn_colour.png")
    nav.confirm_to_depth(5, tries=4)   # colour -> track select
    print(f"  track-select: depth={nav.depth()} sel={scr.exports_sync.sel()}")
    shoot(pid, ROOT/shotdir/"sn_track.png")
    # DESCEND toward the race: confirm, wait for depth-increase OR phase-change (race leaves the
    # frontend), screenshot+log each new state. Stop when stuck or phase leaves 3 (in race).
    nav.press(4); time.sleep(1.5)   # confirm track -> Quick Battle "Game Mode" setup screen
    print(f"  after track confirm: depth={nav.depth()} phase={nav.phase()}")
    dump_counts("track-confirm")
    shoot(pid, ROOT/shotdir/"sn_gamemode.png")
    # On the Game Mode screen "Play Game" is the top option; confirm to START the arena round.
    # Robust: press confirm until phase leaves the menu (==0 = in arena) or we stop progressing.
    for k in range(5):
        if nav.phase()!=3: break
        nav.press(4); time.sleep(1.5)
        print(f"  start-attempt {k}: depth={nav.depth()} phase={nav.phase()}")
        dump_counts(f"start-attempt{k}")
        if nav.phase()==0: break
    shoot(pid, ROOT/shotdir/"sn_race_enter.png")
    # Late arming: navigation is done, so the probes cost nothing that could
    # stall it. Everything counted from here on is by definition in-race.
    if extra_rvas and count_late:
        scr.exports_sync.countthese(extra_rvas)
        print(f"  [count-rvas] armed IN-RACE: {len(extra_rvas)} probes", flush=True)
    # MASHED_COUNT_GATE — RVAs that CERTIFY the simulation is running. Wait for one
    # of them to actually fire before letting the round begin.
    #
    # The start-attempt loop above exits on `phase != 3`, i.e. "no longer in the
    # menu", which is not the same as "racing". Measured 2026-07-29: it lands on
    # phase=2 often enough that ~half of all pre-screen chunks came back with
    # every probe at zero and had to be discarded — correct, but it halved
    # throughput. Pressing confirm again from that state usually gets the rest of
    # the way in, so poll for a gate probe and nudge rather than starting a round
    # that is not a round.
    gate_rvas = [r.strip() for r in os.environ.get("MASHED_COUNT_GATE","").split(",") if r.strip()]
    if gate_rvas and count_late:
        gate_wait = float(os.environ.get("MASHED_COUNT_GATE_WAIT", "30"))
        end_g, nudged, fired = time.time() + gate_wait, 0, False
        last_nudge = 0.0
        while time.time() < end_g:
            if not nav.alive(): break
            cc = scr.exports_sync.counts()
            if any(isinstance(cc.get(r), int) and cc[r] > 0 for r in gate_rvas):
                fired = True
                break
            if time.time() - last_nudge > 3.0:
                nav.press(4); nudged += 1; last_nudge = time.time()
            time.sleep(0.5)
        print(f"  [gate] simulation {'CONFIRMED' if fired else 'NOT confirmed'} "
              f"after {nudged} nudge(s), phase={nav.phase()}", flush=True)
    # WAIT for the arena round to play out (AI drives; round ends on elimination/timeout ->
    # end-of-round scoring fires the results hooks). No forced input needed. Watch for results
    # hooks to start firing and for a phase change (round-end/results screen).
    round_secs = int(sys.argv[sys.argv.index("--round")+1]) if "--round" in sys.argv else 110
    end=time.time()+round_secs; t_shot=0; first_results=None
    RESULTS_SUBSET={"0x00423b40","0x00423b60","0x00423c40","0x00424920","0x00422fd0","0x0046c700","0x004241b0","0x00424100"}
    # induce car movement/elimination to trigger a SCORING event (point) -> the score-accumulator
    # hooks fire on round-point. Cycle a held control (rotate every ~6s) to find throttle/steer
    # and drive the car into collisions / off the arena.
    drive_ctrls=[4,0,1,11,12,5,6,7]; di=0; dt=0
    while time.time()<end:
        if not nav.alive(): print("   exited in race"); break
        if time.time()-dt>6: di=(di+1)%len(drive_ctrls); dt=time.time()
        scr.exports_sync.press(drive_ctrls[di], 600)
        time.sleep(0.5)
        if time.time()-t_shot>10:
            t_shot=time.time()
            cc=scr.exports_sync.counts()
            nz=[r for r,c in cc.items() if isinstance(c,int) and c>0]
            res_hit=[r for r in RESULTS_SUBSET if isinstance(cc.get(r),int) and cc[r]>0]
            tnow=int(round_secs-(end-time.time()))
            print(f"   round t+{tnow}s phase={nav.phase()} exercised={len(nz)} results_hit={len(res_hit)}")
            shoot(pid, ROOT/shotdir/f"sn_round_{tnow}.png")
            if res_hit and first_results is None:
                first_results=tnow; print(f"   >>> RESULTS hooks firing at t+{tnow}s: {res_hit}")
    print(f"  FINAL: depth={nav.depth()} phase={nav.phase()} first_results_at={first_results}")
    shoot(pid, ROOT/shotdir/"sn_final.png")
    try:
        counts = scr.exports_sync.counts()
        print("  === gameplay-gated hook counts (in-race exercise) ===")
        for r,c in counts.items(): print(f"     {r} : {c}")
        exercised=[r for r,c in counts.items() if isinstance(c,int) and c>0]
        print(f"  EXERCISED in-race: {len(exercised)}/{len(counts)} -> {exercised}")
    except Exception as e: print("  counts err", e)
    try:
        if nav.alive(): dev.kill(pid)
    except Exception: pass
    try: sess.detach()
    except Exception: pass
    return 0

if __name__=="__main__":
    sys.exit(main())
