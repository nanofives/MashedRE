# Probe FUN_00427880 (the font/draw-context creator) to find why it returns NULL.
#
# It runs once at ~90s (post-intro font init). We attach light Interceptors at
# four points (cold path, ~no overhead) to capture:
#   entry  0x00427880 : EAX = filename ptr (RwStreamOpen rwSTREAMFILENAME arg)
#   0x0042788e         : EAX = RwStreamOpen (FUN_004cc230) return (stream or NULL)
#   0x004278a4         : EAX = RwStreamFindChunk (FUN_004cc5e0) return
#   0x004278be         : EDI = final returned ctx (NULL means creation failed)
# Plus a filtered exception handler so we still see the eventual AV.
#
# Usage: run this, then launch original\MASHED.exe normally.
import json
import sys
import time
from pathlib import Path

import frida

try:
    import psutil
except ImportError:
    sys.exit("psutil required")

ROOT     = Path(__file__).resolve().parent.parent.parent
OUT_FILE = ROOT / 'log' / 'fontctx_probe.txt'

AGENT_JS = r'''
'use strict';
function rd(p){ try { return p.readCString(); } catch(e){ return '<unreadable '+p+'>'; } }
Interceptor.attach(ptr('0x00427880'), { onEnter: function(){
    var eax = this.context.eax;
    send({ kind:'enter', eax: eax.toString(), filename: rd(eax) });
}});
Interceptor.attach(ptr('0x0042788e'), { onEnter: function(){
    send({ kind:'streamopen', eax: this.context.eax.toString() });   // RwStreamOpen result
}});
Interceptor.attach(ptr('0x004278a4'), { onEnter: function(){
    send({ kind:'findchunk', eax: this.context.eax.toString() });    // FindChunk result
}});
Interceptor.attach(ptr('0x004278be'), { onEnter: function(){
    send({ kind:'ctxresult', edi: this.context.edi.toString() });    // final ctx (EDI) or 0
}});

const FATAL = { 'access-violation':1, 'illegal-instruction':1, 'abort':1,
                'bus-error':1, 'division-by-zero':1, 'stack-overflow':1 };
Process.setExceptionHandler(function(d){
    if (!FATAL[d.type]) return false;
    send({ kind:'av', type:d.type, pc:d.context.pc.toString(),
           esi:d.context.esi.toString(),
           mem:(d.memory&&d.memory.address)?d.memory.address.toString():'?' });
    return false;
});
send({ kind:'ready' });
'''


def find_pid():
    for p in psutil.process_iter(['name', 'pid']):
        try:
            if p.info['name'] and p.info['name'].lower() == 'mashed.exe':
                return p.info['pid']
        except Exception:
            continue
    return None


def main():
    OUT_FILE.parent.mkdir(parents=True, exist_ok=True)
    print("waiting for MASHED.exe — launch it now")
    deadline = time.time() + 120
    pid = None
    while time.time() < deadline:
        pid = find_pid()
        if pid:
            print(f"  found pid={pid}"); break
        time.sleep(0.1)
    if not pid:
        sys.exit("timeout")
    time.sleep(0.05)
    session = frida.get_local_device().attach(pid)
    print("  attached")

    events = []
    def on_message(message, data):
        if message['type'] == 'error':
            print("  agent error:", message.get('description')); return
        p = message.get('payload', {})
        k = p.get('kind')
        if k == 'ready':
            print("  probes armed; watching")
        elif k == 'enter':
            print(f"\n  FUN_00427880 ENTER: filename = {p['filename']!r}  (eax={p['eax']})")
            events.append(p)
        elif k == 'streamopen':
            print(f"  RwStreamOpen (FUN_004cc230) -> {p['eax']}  ({'NULL=OPEN FAILED' if int(p['eax'],16)==0 else 'ok'})")
            events.append(p)
        elif k == 'findchunk':
            print(f"  RwStreamFindChunk (FUN_004cc5e0) -> {p['eax']}  ({'NULL=CHUNK NOT FOUND' if int(p['eax'],16)==0 else 'ok'})")
            events.append(p)
        elif k == 'ctxresult':
            print(f"  FUN_00427880 RETURN ctx (EDI) = {p['edi']}  ({'NULL=FAILED' if int(p['edi'],16)==0 else 'ok'})")
            events.append(p)
        elif k == 'av':
            print(f"\n  AV: pc={p['pc']} esi={p['esi']} mem={p['mem']}")
            events.append(p)

    script = session.create_script(AGENT_JS)
    script.on('message', on_message)
    script.load()

    deadline = time.time() + 130
    while time.time() < deadline:
        try:
            if psutil.Process(pid).name().lower() != 'mashed.exe':
                break
        except Exception:
            print("  process gone"); break
        time.sleep(0.3)

    try: session.detach()
    except Exception: pass
    OUT_FILE.write_text(json.dumps(events, indent=2), encoding='utf-8')
    print(f"\nwrote {len(events)} event(s) -> {OUT_FILE}")
    return 0


if __name__ == '__main__':
    sys.exit(main())
