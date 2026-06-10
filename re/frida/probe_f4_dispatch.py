# Instrument the call dword ptr [ecx+0xf4] at 0x5a7f2c (ecx = *(0x7d3ff8)),
# plus FUN_005a7ee0 entry and FUN_005a7b60 entry. Reports the dispatch target
# and the global object. Confirms whether [*(0x7d3ff8)+0xf4] is the garbage 0x44.
import json, sys, time
from pathlib import Path
import frida
try: import psutil
except ImportError: sys.exit("psutil required")

ROOT = Path(__file__).resolve().parent.parent.parent
OUT  = ROOT / 'log' / 'probe_f4_dispatch.txt'

AGENT = r'''
'use strict';
function rd(p){ try{return ptr(p).readU32();}catch(e){return -1;} }
var GLOB = 0x7d3ff8;

Interceptor.attach(ptr(0x5a7b60), { onEnter: function(){ send({kind:'enter5a7b60'}); }});
Interceptor.attach(ptr(0x5a7ee0), { onEnter: function(){
  var g = rd(GLOB);
  send({kind:'enter5a7ee0', glob:'0x'+(g>>>0).toString(16),
        objF4: (g>0? '0x'+(rd(g+0xf4)>>>0).toString(16):'n/a')});
}});
Interceptor.attach(ptr(0x5a7f2c), { onEnter: function(){
  var ecx = this.context.ecx;
  var tgt = -1; try{ tgt = ecx.add(0xf4).readU32()>>>0; }catch(e){}
  send({kind:'f4call', ecx:ecx.toString(), target:'0x'+tgt.toString(16),
        edi:this.context.edi.toString(), esi:this.context.esi.toString()});
}});
send({kind:'hooked'});

var FATAL={'access-violation':1,'illegal-instruction':1,'abort':1,'bus-error':1,'division-by-zero':1,'stack-overflow':1};
Process.setExceptionHandler(function(d){
  if(!FATAL[d.type]) return false;
  var c=d.context;
  send({kind:'crash', type:d.type, eip:c.pc.toString(),
        mem:(d.memory?d.memory.address.toString()+' '+d.memory.operation:'?'),
        eax:c.eax.toString(), ecx:c.ecx.toString(), edi:c.edi.toString(), esi:c.esi.toString(),
        glob:'0x'+(rd(GLOB)>>>0).toString(16)});
  return false;
});
send({kind:'ready'});
'''

def find_pid():
    for p in psutil.process_iter(['name','pid']):
        try:
            if p.info['name'] and p.info['name'].lower()=='mashed.exe': return p.info['pid']
        except Exception: pass
    return None

def main():
    print("waiting for MASHED.exe — launch now")
    dl=time.time()+120; pid=None
    while time.time()<dl:
        pid=find_pid()
        if pid: print("  pid=%d"%pid); break
        time.sleep(0.05)
    if not pid: sys.exit("timeout")
    sess=frida.get_local_device().attach(pid); print("  attached")
    ev=[]; n7ee0=0; nf4=0; n7b60=0
    def on(m,_):
        nonlocal n7ee0,nf4,n7b60
        if m['type']=='error': print("  AGENT ERR:",m.get('description')); return
        p=m.get('payload',{}); k=p.get('kind')
        if k=='hooked': print("  hooked dispatch sites")
        elif k=='ready': print("  watching")
        elif k=='enter5a7b60': n7b60+=1
        elif k=='enter5a7ee0':
            n7ee0+=1
            if n7ee0<=6 or p['objF4'] in ('0x44','0x0'):
                print(f"  ENTER FUN_005a7ee0 #{n7ee0} *(0x7d3ff8)={p['glob']} [obj+0xf4]={p['objF4']}")
            ev.append(p)
        elif k=='f4call':
            nf4+=1
            garbage = p['target'] in ('0x44','0x0') or int(p['target'],16) < 0x401000
            if nf4<=6 or garbage:
                print(f"  CALL [ecx+0xf4] #{nf4} ecx={p['ecx']} target={p['target']}"
                      f"{'  <== GARBAGE!' if garbage else ''}")
            ev.append(p)
        elif k=='crash':
            print(f"  === CRASH eip={p['eip']} mem={p['mem']} ecx={p['ecx']} edi={p['edi']}"
                  f" *(0x7d3ff8)={p['glob']} | f4calls={nf4} ee0={n7ee0} b60={n7b60} ===")
            ev.append(p)
    sc=sess.create_script(AGENT); sc.on('message',on); sc.load()
    dl=time.time()+90
    while time.time()<dl:
        if any(e.get('kind')=='crash' for e in ev): time.sleep(0.3); break
        try:
            if not psutil.pid_exists(pid): print("  gone (f4calls=%d ee0=%d b60=%d)"%(nf4,n7ee0,n7b60)); break
        except Exception: pass
        time.sleep(0.2)
    try: sess.detach()
    except Exception: pass
    OUT.write_text(json.dumps(ev,indent=2),encoding='utf-8'); print("  wrote",OUT)

if __name__=='__main__': main()
