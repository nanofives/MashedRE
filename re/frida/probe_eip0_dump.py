# Non-perturbing rich dump of the unperturbed eip=0 crash: ONLY an exception
# handler (no Interceptor near the fault, so the crash stays in its native form).
# Dumps regs, the esp window, the RW device global [0x7d3ff8] + object [0x771a0c]
# and their vtables around +0xe4, to see whether a vtable slot is NULL.
import json, sys, time
from pathlib import Path
import frida
try: import psutil
except ImportError: sys.exit("psutil required")

ROOT = Path(__file__).resolve().parent.parent.parent
OUT  = ROOT / 'log' / 'probe_eip0_dump.txt'

AGENT = r'''
'use strict';
function rd(p){ try{return ptr(p).readU32()>>>0;}catch(e){return null;} }
function hx(v){ return v===null?'<unmapped>':'0x'+v.toString(16); }
function dumpVtable(label, objAddr){
  if (objAddr===null || objAddr===0) { send({kind:'note', t:label+': obj null/unmapped ('+hx(objAddr)+')'}); return; }
  var vt = rd(objAddr);
  if (vt===null){ send({kind:'note', t:label+': obj='+hx(objAddr)+' [obj] unmapped'}); return; }
  var slots = {};
  for (var off=0xc0; off<=0x120; off+=4){ slots['+0x'+off.toString(16)] = hx(rd(vt+off)); }
  send({kind:'vtable', label:label, obj:hx(objAddr), vtable:hx(vt), slots:slots});
}
var FATAL={'access-violation':1,'illegal-instruction':1,'abort':1,'bus-error':1,'division-by-zero':1,'stack-overflow':1};
var done=false;
Process.setExceptionHandler(function(d){
  if(!FATAL[d.type] || done) return false;
  done=true;
  var c=d.context;
  var esp=ptr(c.esp.toString());
  var win=[];
  for (var i=0;i<28;i++){ win.push(hx(rd(esp.add(i*4)))); }
  send({kind:'crash', type:d.type, eip:c.pc.toString(),
        mem:(d.memory?d.memory.address.toString()+' '+d.memory.operation:'?'),
        eax:c.eax.toString(),ebx:c.ebx.toString(),ecx:c.ecx.toString(),edx:c.edx.toString(),
        esi:c.esi.toString(),edi:c.edi.toString(),ebp:c.ebp.toString(),esp:c.esp.toString(),
        espwin:win,
        g_771a0c:hx(rd(0x771a0c)), g_7d3ff8:hx(rd(0x7d3ff8)), g_7d4568:hx(rd(0x7d4568))});
  dumpVtable('device[0x7d3ff8]', rd(0x7d3ff8));
  dumpVtable('obj[0x771a0c]',    rd(0x771a0c));
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
        if k=='ready': print("  watching (exception-handler only, non-perturbing)")
        elif k=='note': print("  NOTE:",p['t']); ev.append(p)
        elif k=='vtable':
            print(f"  {p['label']}  obj={p['obj']} vtable={p['vtable']}")
            for s,v in p['slots'].items():
                mark = '  <== NULL' if v=='0x0' else ''
                print(f"      {s} = {v}{mark}")
            ev.append(p)
        elif k=='crash':
            print(f"  === CRASH {p['type']} eip={p['eip']} mem={p['mem']} ===")
            print(f"   eax={p['eax']} ebx={p['ebx']} ecx={p['ecx']} edx={p['edx']} esi={p['esi']} edi={p['edi']} ebp={p['ebp']} esp={p['esp']}")
            print(f"   globals: [0x771a0c]={p['g_771a0c']} [0x7d3ff8]={p['g_7d3ff8']} [0x7d4568]={p['g_7d4568']}")
            print(f"   esp window: {p['espwin']}")
            ev.append(p)
    sc=sess.create_script(AGENT); sc.on('message',on); sc.load()
    dl=time.time()+90
    while time.time()<dl:
        if any(e.get('kind')=='crash' for e in ev): time.sleep(0.5); break
        try:
            if not psutil.pid_exists(pid): print("  gone"); break
        except Exception: pass
        time.sleep(0.2)
    try: sess.detach()
    except Exception: pass
    OUT.write_text(json.dumps(ev,indent=2),encoding='utf-8'); print("  wrote",OUT)

if __name__=='__main__': main()
