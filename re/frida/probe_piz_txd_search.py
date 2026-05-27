# Probe the font TXD load chain in FUN_00427ca0:
#   FUN_0042a6b0(name) -> FUN_0042a530(reg, name) "Searching piz for %s" (0 = not found)
#                      -> FUN_004b3d80() actual TXD load (0 = load failed)
# Determines whether 'fgdc20.txd' is found in the piz registry and loaded, or not.
# All gated by an in-FUN_0042a6b0 flag so we only see the font-TXD attempt.
# Usage: run, then launch MASHED.
import json, sys, time
from pathlib import Path
import frida
try:
    import psutil
except ImportError:
    sys.exit("psutil required")

ROOT     = Path(__file__).resolve().parent.parent.parent
OUT_FILE = ROOT / 'log' / 'piz_txd_search.txt'

AGENT_JS = r'''
'use strict';
function rd(p){ try { return p.readCString(); } catch(e){ return '<unreadable '+p+'>'; } }
var inAB = 0, curName = '?';
Interceptor.attach(ptr('0x0042a6b0'), {   // FUN_0042a6b0(name)
    onEnter: function(){
        inAB++;
        try { curName = rd(this.context.esp.add(4).readPointer()); } catch(e){ curName='?'; }
    },
    onLeave: function(ret){
        inAB--;
        send({ kind:'a6b0', name:curName, ret:ret.toString(),
               fail:(ret.toInt32()>>>0)===0 });
    }
});
Interceptor.attach(ptr('0x0042a530'), {   // FUN_0042a530(reg, name) "Searching piz for %s"
    onEnter: function(){ this.on = inAB>0; if(this.on){ try{ this.nm = rd(this.context.esp.add(8).readPointer()); }catch(e){ this.nm='?'; } } },
    onLeave: function(ret){ if(this.on) send({ kind:'search', name:this.nm, ret:ret.toString(),
                                               found:(ret.toInt32()>>>0)!==0 }); }
});
Interceptor.attach(ptr('0x004b3d80'), {   // FUN_004b3d80 TXD load
    onEnter: function(){ this.on = inAB>0; },
    onLeave: function(ret){ if(this.on) send({ kind:'txdload', ret:ret.toString(),
                                               fail:(ret.toInt32()>>>0)===0 }); }
});
const FATAL = { 'access-violation':1,'illegal-instruction':1,'abort':1,'bus-error':1,'division-by-zero':1,'stack-overflow':1 };
Process.setExceptionHandler(function(d){
    if (!FATAL[d.type]) return false;
    send({kind:'av', pc:d.context.pc.toString(), esi:d.context.esi.toString()});
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
        elif k=='search':
            print(f"  FUN_0042a530 'Searching piz for' name={p['name']!r} -> {p['ret']}" + ("" if p['found'] else "   <-- NOT FOUND IN PIZ")); events.append(p)
        elif k=='txdload':
            print(f"  FUN_004b3d80 TXD load -> {p['ret']}" + ("   <-- LOAD FAILED" if p['fail'] else "   (ok)")); events.append(p)
        elif k=='a6b0':
            print(f"  FUN_0042a6b0(name={p['name']!r}) -> {p['ret']}" + ("   <-- RETURNED 0 (no current TXD)" if p['fail'] else "   (ok)")); events.append(p)
        elif k=='av': print(f"\n  AV pc={p['pc']} esi={p['esi']}"); events.append(p)
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
