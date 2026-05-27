# Probe the STOCK font/draw-context builder FUN_00554390 to find which internal
# step returns NULL (it fails parsing a valid fgdc20.rwf — see fontctx_probe_fgdc20.json).
#
# Hooks FUN_00554390 enter/leave (an in-function flag) + the suspect sub-calls,
# logging their return values ONLY while inside FUN_00554390:
#   FUN_005551d0  ctx allocator (called once at top)
#   FUN_004c5cb0  per-glyph RW raster/texture create   (returns 0 -> fail)
#   FUN_00554200  branch-B texture loader
#   FUN_005c4d30  glyph-array base (0 -> array bad)
# These are RW-raster/D3D9-backed; one of them is expected to return 0.
#
# Cold path (font load fires once ~90s). Usage: run, then launch MASHED.
import json, sys, time
from pathlib import Path
import frida
try:
    import psutil
except ImportError:
    sys.exit("psutil required")

ROOT     = Path(__file__).resolve().parent.parent.parent
OUT_FILE = ROOT / 'log' / 'fontctx_build_probe.txt'

AGENT_JS = r'''
'use strict';
var inFn = 0;
function hookRet(addr, name, single){
    var seen = false;
    Interceptor.attach(ptr(addr), { onLeave: function(ret){
        if (!inFn) return;
        var v = ret.toInt32() >>> 0;
        if (single && seen) return;
        if (single && v !== 0) return;     // for per-glyph: only report the failing (0) one
        seen = true;
        send({ kind:'sub', name:name, ret:'0x'+v.toString(16) });
    }});
}
Interceptor.attach(ptr('0x00554390'), {
    onEnter: function(){ inFn++; send({kind:'enter', stream:this.context.eax.toString()}); },
    onLeave: function(ret){ inFn--; send({kind:'leave', ret:ret.toString()}); }
});
hookRet('0x005551d0', 'FUN_005551d0_alloc',      false);
hookRet('0x004c5cb0', 'FUN_004c5cb0_rastercreate', true);   // report only when it returns 0
hookRet('0x00554200', 'FUN_00554200_tex',        false);
hookRet('0x005c4d30', 'FUN_005c4d30_arraybase',  false);

const FATAL = { 'access-violation':1,'illegal-instruction':1,'abort':1,'bus-error':1,'division-by-zero':1,'stack-overflow':1 };
Process.setExceptionHandler(function(d){
    if (!FATAL[d.type]) return false;
    send({kind:'av', type:d.type, pc:d.context.pc.toString(), esi:d.context.esi.toString(),
          mem:(d.memory&&d.memory.address)?d.memory.address.toString():'?'});
    return false;
});
send({kind:'ready'});
'''

def find_pid():
    for p in psutil.process_iter(['name','pid']):
        try:
            if p.info['name'] and p.info['name'].lower()=='mashed.exe': return p.info['pid']
        except Exception: continue
    return None

def main():
    OUT_FILE.parent.mkdir(parents=True, exist_ok=True)
    print("waiting for MASHED.exe — launch it now")
    dl=time.time()+120; pid=None
    while time.time()<dl:
        pid=find_pid()
        if pid: print(f"  found pid={pid}"); break
        time.sleep(0.1)
    if not pid: sys.exit("timeout")
    time.sleep(0.05)
    session=frida.get_local_device().attach(pid); print("  attached")
    events=[]
    def on_message(m,d):
        if m['type']=='error': print("  agent error:", m.get('description')); return
        p=m.get('payload',{}); k=p.get('kind')
        if k=='ready': print("  probes armed")
        elif k=='enter': print(f"\n  FUN_00554390 ENTER (stream={p['stream']})"); events.append(p)
        elif k=='sub':
            print(f"    {p['name']} -> {p['ret']}" + ("   <-- NULL/FAIL" if int(p['ret'],16)==0 else "")); events.append(p)
        elif k=='leave': print(f"  FUN_00554390 LEAVE ret={p['ret']}" + ("   <-- BUILDER FAILED" if int(p['ret'],16)==0 else "")); events.append(p)
        elif k=='av': print(f"\n  AV pc={p['pc']} esi={p['esi']} mem={p['mem']}"); events.append(p)
    script=session.create_script(AGENT_JS); script.on('message', on_message); script.load()
    dl=time.time()+130
    while time.time()<dl:
        try:
            if psutil.Process(pid).name().lower()!='mashed.exe': break
        except Exception: print("  process gone"); break
        time.sleep(0.3)
    try: session.detach()
    except Exception: pass
    OUT_FILE.write_text(json.dumps(events, indent=2), encoding='utf-8')
    print(f"\nwrote {len(events)} event(s) -> {OUT_FILE}")
    return 0

if __name__=='__main__':
    sys.exit(main())
