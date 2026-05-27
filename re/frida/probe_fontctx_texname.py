# Capture the texture/glyph name(s) passed to the failing FUN_004c5cb0 (RW texture
# lookup-or-load) inside the font builder FUN_00554390. FUN_004c5cb0(name, dict=0)
# returns 0 (err 0x16) for a glyph texture not in the current TXD / unloadable.
#
# Gated by an in-FUN_00554390 flag so we only see the font-builder lookups.
# Logs name (string at the param_1 pointer) + the register file + return value.
# Usage: run, then launch MASHED.
import json, sys, time
from pathlib import Path
import frida
try:
    import psutil
except ImportError:
    sys.exit("psutil required")

ROOT     = Path(__file__).resolve().parent.parent.parent
OUT_FILE = ROOT / 'log' / 'fontctx_texname.txt'

AGENT_JS = r'''
'use strict';
var inFn = 0, n = 0;
Interceptor.attach(ptr('0x00554390'), {
    onEnter: function(){ inFn++; }, onLeave: function(){ inFn--; }
});
Interceptor.attach(ptr('0x004c5cb0'), {
    onEnter: function(){
        if (!inFn) { this.skip = true; return; }
        this.skip = false;
        var sp = this.context.esp;
        // __cdecl candidates: [esp+4]=param_1(name ptr), [esp+8]=param_2(dict)
        var p1 = '?', name = '?', dict = '?';
        try { var a = sp.add(4).readPointer(); p1 = a.toString();
              try { name = a.readCString(); } catch(e){ name = '<unreadable>'; } } catch(e){}
        try { dict = sp.add(8).readPointer().toString(); } catch(e){}
        this.info = { p1:p1, name:name, dict:dict,
                      eax:this.context.eax.toString(), ecx:this.context.ecx.toString(),
                      edx:this.context.edx.toString() };
    },
    onLeave: function(ret){
        if (this.skip) return;
        if (n++ < 24) {
            var i = this.info || {};
            send({ kind:'lookup', idx:n, name:i.name, p1:i.p1, dict:i.dict,
                   ecx:i.ecx, edx:i.edx, ret:ret.toString(),
                   fail:(ret.toInt32()>>>0)===0 });
        }
    }
});
const FATAL = { 'access-violation':1,'illegal-instruction':1,'abort':1,'bus-error':1,'division-by-zero':1,'stack-overflow':1 };
Process.setExceptionHandler(function(d){
    if (!FATAL[d.type]) return false;
    send({kind:'av', pc:d.context.pc.toString(), esi:d.context.esi.toString(),
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
        elif k=='lookup':
            tag = "  <-- FAIL (NULL)" if p['fail'] else ""
            print(f"  [{p['idx']}] FUN_004c5cb0 name={p['name']!r} dict={p['dict']} ecx={p['ecx']} -> {p['ret']}{tag}")
            events.append(p)
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
