# Instrument FUN_005ab100 (RW chunk state machine) entry only — a cold function.
# Logs node, state(+0x40), handler(+0x44), p48(+0x48) for entries whose handler
# is GARBAGE (< 0x401000) or whose state==1 (the guarded 0x5ab148 path).
# Also reports live bytes at 0x5ab148 (guard installed?) and catches the AV.
import json, sys, time
from pathlib import Path
import frida
try: import psutil
except ImportError: sys.exit("psutil required")

ROOT = Path(__file__).resolve().parent.parent.parent
OUT  = ROOT / 'log' / 'probe_statemachine.txt'

AGENT = r'''
'use strict';
var MX_LO=0x401000, MX_HI=0x995000;
// guard-install check
(function(){
  var va=ptr(0x5ab148), b='';
  try{ for(var i=0;i<6;i++) b+=va.add(i).readU8().toString(16).padStart(2,'0')+' '; }catch(e){b='<unreadable>';}
  send({kind:'guardbytes', bytes:b.trim()});
})();
var nState1=0, nGarbage=0;
Interceptor.attach(ptr(0x5ab100), { onEnter: function(args){
  // FUN_005ab100 reads node from [esp+0x14]; at onEnter [esp]=retaddr so node=args[4]
  var node;
  try { node = this.context.esp.add(0x14).readPointer(); } catch(e){ return; }
  if (node.isNull()) return;
  var state, handler, p48;
  try {
    state   = node.add(0x40).readU32();
    handler = node.add(0x44).readU32();
    p48     = node.add(0x48).readU32();
  } catch(e){ return; }
  var garbage = (handler < MX_LO || handler >= MX_HI);
  if (state===1) nState1++;
  if (garbage)   nGarbage++;
  if (garbage || (state===1 && nState1<=8)) {
    send({kind:'entry', node:node.toString(), state:state,
          handler:'0x'+handler.toString(16), p48:'0x'+p48.toString(16),
          garbage:garbage, retaddr:this.context.esp.readPointer().toString(),
          ecx_arg:'0x'+this.context.esp.add(0x4).readU32().toString(16)});
  }
}});
send({kind:'hooked'});
var FATAL={'access-violation':1,'illegal-instruction':1,'abort':1,'bus-error':1,'division-by-zero':1,'stack-overflow':1};
Process.setExceptionHandler(function(d){
  if(!FATAL[d.type]) return false;
  var c=d.context;
  send({kind:'crash', type:d.type, address:d.address.toString(), eip:c.pc.toString(),
        eax:c.eax.toString(), edi:c.edi.toString(), ecx:c.ecx.toString(),
        esi:c.esi.toString(), esp:c.esp.toString(),
        mem:(d.memory?d.memory.address.toString()+' '+d.memory.operation:'?'),
        counts:'state1='+nState1+' garbage='+nGarbage});
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
    ev=[]
    def on(m,_):
        if m['type']=='error': print("  AGENT ERR:",m.get('description')); return
        p=m.get('payload',{}); k=p.get('kind')
        if k=='guardbytes':
            tag=' <== HOOKED' if p['bytes'].startswith('e9') else ' <== NOT HOOKED!'
            print("  0x5ab148 bytes:",p['bytes'],tag)
        elif k=='hooked': print("  FUN_005ab100 instrumented")
        elif k=='ready': print("  watching")
        elif k=='entry':
            print(f"  ENTER node={p['node']} state={p['state']} handler={p['handler']}"
                  f" p48={p['p48']} garbage={p['garbage']} ret={p['retaddr']} arg1={p['ecx_arg']}")
            ev.append(p)
        elif k=='crash':
            print(f"  === CRASH eip={p['eip']} mem={p['mem']} eax={p['eax']} edi={p['edi']}"
                  f" ecx={p['ecx']} esi={p['esi']} | {p['counts']} ===")
            ev.append(p)
    sc=sess.create_script(AGENT); sc.on('message',on); sc.load()
    dl=time.time()+90
    while time.time()<dl:
        if any(e.get('kind')=='crash' for e in ev): time.sleep(0.3); break
        try:
            if not psutil.pid_exists(pid): print("  gone"); break
        except Exception: pass
        time.sleep(0.2)
    try: sess.detach()
    except Exception: pass
    OUT.write_text(json.dumps(ev,indent=2),encoding='utf-8'); print("  wrote",OUT)

if __name__=='__main__': main()
