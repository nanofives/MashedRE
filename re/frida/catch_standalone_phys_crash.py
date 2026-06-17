# WS-PHYS-CRASH-FIX: spawn mashed_re.exe with MASHED_REAL_PHYSICS=1 + race demo,
# arm a Frida exception handler, capture the faulting EIP + register state + a
# stack walk of return-address candidates inside the mashed_re.exe code range.
#
# Unlike poll_attach_catch_crash.py (which targets MASHED.exe at fixed base
# 0x400000), this spawns the standalone (rebased /BASE:0x10000) and classifies
# stack values against the LIVE module range so the caller chain resolves against
# mashed_re.map (Rva+Base column).
import json, os, sys, time
from pathlib import Path
import frida

ROOT    = Path(__file__).resolve().parent.parent.parent
EXE     = ROOT / 'mashedmod' / 'build' / 'mashed_re.exe'
OUT     = ROOT / 'log' / 'standalone_phys_crash.json'

AGENT = r'''
'use strict';
function moduleTable() {
    const out = [];
    Process.enumerateModules().forEach(function (m) {
        out.push({ name: m.name, base: m.base.toString(),
                   end: m.base.add(m.size).toString(), size: m.size });
    });
    return out;
}
// locate mashed_re.exe range for stack classification
let MX_LO = 0, MX_HI = 0;
Process.enumerateModules().forEach(function (m) {
    if (m.name.toLowerCase() === 'mashed_re.exe') {
        MX_LO = m.base.toUInt32();
        MX_HI = m.base.add(m.size).toUInt32();
    }
});
const FATAL = { 'access-violation':1,'illegal-instruction':1,'abort':1,
                'bus-error':1,'division-by-zero':1,'stack-overflow':1 };
// Report EVERY fatal exception (do NOT stop at the first) and RETURN TRUE for the
// known benign MainLoopInit call-into-zeros AV at 0x495110 (SEH-recoverable in the
// real boot chain) so we reach the actually-terminating physics fault. The harness
// records the sequence; the LAST one before process exit is the real crash.
let seen = 0;
Process.setExceptionHandler(function (details) {
    if (!FATAL[details.type]) return false;
    const op = details.memory ? details.memory.operation : '?';
    // The boot chain raises MANY execute-into-zero AVs (call thru image-padded RVA
    // thunks) that the app's own __try/__except SEH recovers. We must NOT touch
    // those: return false so the OS continues SEH dispatch (app handler unwinds).
    // We ONLY capture/stop on a DATA (read/write) fault — the real physics bug.
    if (op === 'execute') return false;
    seen++;
    const faultVA = details.address.toUInt32();
    const benignExec = false;
    const ctx = details.context;
    let bytesAt = '', bytesBefore = '';
    try {
        const eip = ptr(details.address.toString());
        for (let i=-16;i<0;i++){ try{bytesBefore+=eip.add(i).readU8().toString(16).padStart(2,'0')+' ';}catch(e){bytesBefore+='?? ';} }
        for (let i=0;i<16;i++){ try{bytesAt+=eip.add(i).readU8().toString(16).padStart(2,'0')+' ';}catch(e){bytesAt+='?? ';} }
    } catch(e){}
    let stack=[], codeAddrs=[];
    try {
        const esp = ptr(ctx.esp.toString());
        for (let i=0;i<400;i++){
            try {
                const v = esp.add(i*4).readU32();
                if (v >= MX_LO && v < MX_HI)
                    codeAddrs.push({ off:i*4, val:v.toString(16), rva:(v-MX_LO).toString(16) });
            } catch(e){ break; }
        }
    } catch(e){}
    const inImg = (faultVA >= MX_LO && faultVA < MX_HI);
    send({
        kind:'crash', seq:seen, type:details.type, address:details.address.toString(),
        mashed_base: ptr(MX_LO).toString(),
        eip_rva: inImg ? (faultVA-MX_LO).toString(16) : ('EXT:'+faultVA.toString(16)),
        recovered: benignExec,
        eax:ctx.eax.toString(), ebx:ctx.ebx.toString(), ecx:ctx.ecx.toString(),
        edx:ctx.edx.toString(), esi:ctx.esi.toString(), edi:ctx.edi.toString(),
        ebp:ctx.ebp.toString(), esp:ctx.esp.toString(), eip:ctx.pc.toString(),
        bytes_before_eip:bytesBefore.trim(), bytes_at_eip:bytesAt.trim(),
        mem_address:(details.memory&&details.memory.address)?details.memory.address.toString():'?',
        mem_op:(details.memory&&details.memory.operation)?details.memory.operation:'?',
        code_addrs:codeAddrs, modules:moduleTable(),
    });
    // execute-into-zeros boot AVs are SEH-recovered in the real chain: skip them.
    // A real DATA-access physics fault (read/write) is the one we want to STOP on.
    if (benignExec) return true;
    return false;
});
send({ kind:'ready', mashed_base: ptr(MX_LO).toString() });
'''

def main():
    OUT.parent.mkdir(parents=True, exist_ok=True)
    env = dict(os.environ)
    env.update({
        'MASHED_REAL_PHYSICS':'1',
        'MASHED_RACE_DEMO':'1', 'MASHED_PLAY_DEMO':'1',
        'MASHED_GOTO':'6', 'MASHED_TRACK_SEL':'0', 'MASHED_CAR_SEL':'0',
    })
    dev = frida.get_local_device()
    pid = dev.spawn([str(EXE)], cwd=str(EXE.parent), env=env)
    print(f'spawned pid={pid}')
    session = dev.attach(pid)
    crashes = []
    stop = {'fatal': None}
    def on_msg(m, d):
        if m['type'] == 'error':
            print('agent error:', m.get('description')); return
        p = m.get('payload', {})
        if p.get('kind') == 'ready':
            print('handler armed, mashed_base=', p.get('mashed_base'))
        elif p.get('kind') == 'crash':
            crashes.append(p)
            tag = 'RECOVERED' if p.get('recovered') else 'FATAL'
            print(f"  [{tag}] seq={p.get('seq')} {p.get('type')} eip={p.get('address')} "
                  f"rva={p.get('eip_rva')} mem={p.get('mem_op')}@{p.get('mem_address')} eax={p.get('eax')}")
            if not p.get('recovered'):
                stop['fatal'] = p
    script = session.create_script(AGENT)
    script.on('message', on_msg)
    script.load()
    dev.resume(pid)
    deadline = time.time() + 30
    while time.time() < deadline and stop['fatal'] is None:
        time.sleep(0.2)
    fatal = stop['fatal']
    if fatal:
        print('\n=== FATAL CRASH ===')
        for k in ('type','address','eip_rva','mem_address','mem_op','eax','ebx','ecx','edx','esi','edi','esp','ebp'):
            print(f'  {k:12s} {fatal.get(k)}')
        print('  bytes_at_eip', fatal.get('bytes_at_eip'))
        print('  code_addrs (rva):', [c['rva'] for c in fatal.get('code_addrs', [])][:24])
    try: session.detach()
    except Exception: pass
    try: dev.kill(pid)
    except Exception: pass
    out = {'fatal': fatal, 'recovered_count': sum(1 for c in crashes if c.get('recovered')),
           'all': crashes}
    OUT.write_text(json.dumps(out, indent=2), encoding='utf-8')
    print('written', OUT, '| total exceptions:', len(crashes))
    return 0 if fatal else 1

if __name__ == '__main__':
    sys.exit(main())
