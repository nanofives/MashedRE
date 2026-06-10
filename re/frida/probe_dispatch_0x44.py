# Probe for the eip=0x44 boot crash. One-shot attach that:
#   1. Reports live bytes at every [reg+0x44] dispatch site (is 0x5ab148 hooked
#      with an E9? => the StreamHandlerDispatchGuard installed).
#   2. Interceptor-traps each dispatch site; onEnter computes the jump/call
#      target [reg+0x44] and only sends when it is GARBAGE (< 0x401000), i.e.
#      the case that crashes. This pins which instruction actually transfers
#      control to 0x44.
#   3. Arms a fatal-exception handler to catch the AV and dump regs/stack.
#
# Usage: run this, then launch original\MASHED.exe.
import json, sys, time
from pathlib import Path
import frida
try:
    import psutil
except ImportError:
    sys.exit("psutil required")

ROOT = Path(__file__).resolve().parent.parent.parent
OUT  = ROOT / 'log' / 'probe_dispatch_0x44.txt'

# (va, reg, kind) for every [reg+0x44] site found by the static scan.
SITES = [
    (0x0041e976,'ecx','jmp'),
    (0x004949b8,'ecx','call'),
    (0x0049cc13,'eax','call'),
    (0x004a1eae,'ecx','call'),
    (0x004a2183,'ecx','call'),
    (0x004cad77,'edx','call'),
    (0x004d1119,'ecx','call'),
    (0x004dd1dc,'ecx','call'),
    (0x004dd264,'edx','call'),
    (0x004dd528,'edx','call'),
    (0x0052dec5,'esi','call'),
    (0x0052e06c,'esi','call'),
    (0x0052e252,'esi','call'),
    (0x00550afb,'ecx','jmp'),
    (0x005764b2,'ebx','call'),
    (0x005ab117,'ecx','jmp'),
    (0x005ab148,'eax','jmp'),
    (0x005ab237,'eax','call'),
    (0x005ab285,'esi','call'),
    (0x005ab2ca,'esi','call'),
    (0x005b8f1f,'ecx','call'),
    (0x005bf131,'ecx','call'),
]

AGENT = r'''
'use strict';
var SITES = %s;
var MX_LO = 0x401000, MX_HI = 0x995000;

// 1) report live bytes at each site (hook detection)
var bytesReport = [];
SITES.forEach(function(s){
    var va = ptr(s[0]);
    var b = '';
    try { for (var i=0;i<6;i++) b += va.add(i).readU8().toString(16).padStart(2,'0')+' '; }
    catch(e){ b = '<unreadable>'; }
    bytesReport.push({va: s[0].toString(16), reg: s[1], kind: s[2], bytes: b.trim()});
});
send({kind:'bytes', data: bytesReport});

// 2) trap each dispatch; report only garbage targets
SITES.forEach(function(s){
    var va = ptr(s[0]), reg = s[1], kind = s[2];
    try {
        Interceptor.attach(va, { onEnter: function(args){
            var base = this.context[reg];
            if (base.isNull()) return;
            var tgt;
            try { tgt = base.add(0x44).readU32(); } catch(e){ return; }
            // garbage handler => would transfer to a bad address
            if (tgt < MX_LO || tgt >= MX_HI) {
                send({kind:'garbage', site: s[0].toString(16), reg: reg, dispatch: kind,
                      base: base.toString(), target: tgt.toString(16),
                      eax: this.context.eax.toString(), edi: this.context.edi.toString(),
                      ecx: this.context.ecx.toString(), esi: this.context.esi.toString()});
            }
        }});
    } catch(e){ send({kind:'hookerr', site: s[0].toString(16), err: e.message}); }
});
send({kind:'hooked', count: SITES.length});

// 3) fatal exception handler
var FATAL = {'access-violation':1,'illegal-instruction':1,'abort':1,'bus-error':1,
             'division-by-zero':1,'stack-overflow':1};
Process.setExceptionHandler(function(d){
    if (!FATAL[d.type]) return false;
    var c = d.context;
    send({kind:'crash', type:d.type, address:d.address.toString(),
          eip:c.pc.toString(), eax:c.eax.toString(), edi:c.edi.toString(),
          ecx:c.ecx.toString(), esi:c.esi.toString(), esp:c.esp.toString(),
          mem:(d.memory?d.memory.address.toString()+' '+d.memory.operation:'?')});
    return false;
});
send({kind:'ready'});
''' % json.dumps([[s[0],s[1],s[2]] for s in SITES])


def find_pid():
    for p in psutil.process_iter(['name','pid']):
        try:
            if p.info['name'] and p.info['name'].lower()=='mashed.exe':
                return p.info['pid']
        except Exception: pass
    return None


def main():
    print("waiting for MASHED.exe — launch it now")
    dl = time.time()+120; pid=None
    while time.time()<dl:
        pid=find_pid()
        if pid: print(f"  found pid={pid}"); break
        time.sleep(0.05)
    if not pid: sys.exit("timeout")
    sess = frida.get_local_device().attach(pid)
    print("  attached")
    events=[]
    def on_msg(m,_):
        if m['type']=='error':
            print("  AGENT ERROR:", m.get('description')); return
        p=m.get('payload',{})
        k=p.get('kind')
        if k=='bytes':
            print("  --- live dispatch-site bytes ---")
            for r in p['data']:
                tag=' <== HOOKED (E9)' if r['bytes'].startswith('e9') else ''
                print(f"    {r['va']:>8}  {r['kind']:4} [{r['reg']}+0x44]  {r['bytes']}{tag}")
        elif k=='hooked': print(f"  trapped {p['count']} sites; handler arming")
        elif k=='ready':  print("  ready — watching")
        elif k=='hookerr':print(f"  HOOK ERR @{p['site']}: {p['err']}")
        elif k=='garbage':
            print(f"  >>> GARBAGE DISPATCH @ {p['site']} {p['dispatch']} [{p['reg']}+0x44]"
                  f" base={p['base']} target=0x{p['target']} eax={p['eax']} edi={p['edi']}")
            events.append(p)
        elif k=='crash':
            print(f"  === CRASH eip={p['eip']} mem={p['mem']} eax={p['eax']} edi={p['edi']}"
                  f" ecx={p['ecx']} esi={p['esi']} ===")
            events.append(p)
    sc=sess.create_script(AGENT); sc.on('message',on_msg); sc.load()
    dl=time.time()+90
    while time.time()<dl:
        if any(e.get('kind')=='crash' for e in events): time.sleep(0.3); break
        try:
            if not psutil.pid_exists(pid): print("  process gone"); break
        except Exception: pass
        time.sleep(0.2)
    try: sess.detach()
    except Exception: pass
    OUT.write_text(json.dumps(events, indent=2), encoding='utf-8')
    print(f"  wrote {OUT}")

if __name__=='__main__':
    main()
