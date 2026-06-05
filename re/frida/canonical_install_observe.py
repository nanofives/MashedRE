# Canonical installed-hook observer for C3->C4 verification.
#
# Installs ONLY the named candidate hooks (via MASHED_HOOK_ONLY), boots MASHED
# to the menu with their inline-JMPs LIVE, confirms each JMP is installed
# (first byte == 0xE9), and watches for a crash for N seconds. This is the
# subset-install canonical scenario: one (or a few) hooks live, everything else
# stock -> a clean "modded vs original" test that sidesteps the full-install
# multi-bug field.
#
# Usage:
#   py -3.12 re/frida/canonical_install_observe.py <name-or-0xRVA>[,<...>] [--seconds N]
#
# Requires: original/ has the deployed dinput8 proxy + fresh mashed_re_dev.asi,
# the 4 disk patches applied, videocfg.bin windowed, d3d9 shim deployed.
import json, os, sys, time
from pathlib import Path
import frida
try:
    import psutil
except ImportError:
    psutil = None

ROOT    = Path(__file__).resolve().parent.parent.parent
ORIG    = ROOT / "original"
EXE     = ORIG / "MASHED.exe"
LOG_DIR = ROOT / "log"
MANIFEST = LOG_DIR / "hook_install_manifest.txt"

AGENT_JS = r'''
'use strict';
function moduleTable() {
    const out = [];
    Process.enumerateModules().forEach(function (m) {
        out.push({ name: m.name, base: m.base.toString(),
                   end: m.base.add(m.size).toString(), size: m.size });
    });
    return out;
}
const FATAL = { 'access-violation':1,'illegal-instruction':1,'abort':1,
                'bus-error':1,'division-by-zero':1,'stack-overflow':1 };
Process.setExceptionHandler(function (details) {
    if (!FATAL[details.type]) return false;
    const ctx = details.context;
    let bAt='', bBefore='';
    try { const eip = ptr(details.address.toString());
        for (let i=-16;i<0;i++){ try{bBefore+=eip.add(i).readU8().toString(16).padStart(2,'0')+' ';}catch(e){bBefore+='?? ';} }
        for (let i=0;i<16;i++){ try{bAt+=eip.add(i).readU8().toString(16).padStart(2,'0')+' ';}catch(e){bAt+='?? ';} }
    } catch(e){}
    let stack=[];
    try { const esp=ptr(ctx.esp.toString());
        for (let i=0;i<320;i++){ try{ stack.push({off:i*4, val:esp.add(i*4).readU32().toString(16)});}catch(e){break;} }
    } catch(e){}
    send({ kind:'crash', type:details.type, address:details.address.toString(),
        eax:ctx.eax?ctx.eax.toString():'?', ebx:ctx.ebx?ctx.ebx.toString():'?',
        ecx:ctx.ecx?ctx.ecx.toString():'?', edx:ctx.edx?ctx.edx.toString():'?',
        esi:ctx.esi?ctx.esi.toString():'?', edi:ctx.edi?ctx.edi.toString():'?',
        ebp:ctx.ebp?ctx.ebp.toString():'?', esp:ctx.esp?ctx.esp.toString():'?',
        eip:ctx.pc?ctx.pc.toString():'?', bytes_before_eip:bBefore.trim(),
        bytes_at_eip:bAt.trim(),
        mem_address:(details.memory&&details.memory.address)?details.memory.address.toString():'?',
        mem_op:(details.memory&&details.memory.operation)?details.memory.operation:'?',
        stack:stack, modules:moduleTable() });
    return false;
});
rpc.exports = {
    firstbyte: function (rva) { try { return ptr(rva).readU8(); } catch(e){ return -1; } },
    asibase: function () {
        const m = Process.findModuleByName('mashed_re_dev.asi');
        return m ? m.base.toString() : null;
    }
};
send({ kind:'ready' });
'''

def main():
    if len(sys.argv) < 2:
        sys.exit("usage: canonical_install_observe.py <name-or-0xRVA>[,...] [--seconds N]")
    candidates = sys.argv[1]
    seconds = 25
    if "--seconds" in sys.argv:
        seconds = int(sys.argv[sys.argv.index("--seconds")+1])
    # RVA tokens for JMP-live verification (only the 0x... entries)
    rvas = [c.strip() for c in candidates.split(",") if c.strip().lower().startswith("0x")]

    LOG_DIR.mkdir(parents=True, exist_ok=True)
    if MANIFEST.exists(): MANIFEST.unlink()

    env = dict(os.environ)
    env["MASHED_RE_DEV"]       = "1"
    env["MASHED_HOOK_ONLY"]    = candidates
    env["MASHED_HOOK_MANIFEST"] = str(MANIFEST)

    print(f"spawning MASHED.exe  ONLY=[{candidates}]  watch={seconds}s")
    device = frida.get_local_device()
    pid = device.spawn(str(EXE), cwd=str(ORIG), env=env)
    print(f"  pid={pid}")
    session = device.attach(pid)

    state = {"crash": None, "ready": False}
    def on_message(msg, data):
        if msg.get("type") == "error":
            print(f"  agent error: {msg.get('description')}"); return
        p = msg.get("payload", {})
        if p.get("kind") == "ready":
            state["ready"] = True
        elif p.get("kind") == "crash":
            state["crash"] = p

    script = session.create_script(AGENT_JS)
    script.on("message", on_message)
    script.load()
    device.resume(pid)
    print("  resumed; booting...")

    deadline = time.time() + seconds
    while time.time() < deadline:
        if state["crash"]: break
        if psutil and not psutil.pid_exists(pid):
            print("  process exited (no crash-handler hit)"); break
        time.sleep(0.25)

    alive = psutil.pid_exists(pid) if psutil else None
    # JMP-live check (only if still alive / before teardown)
    jmp = {}
    if not state["crash"]:
        for r in rvas:
            try: jmp[r] = script.exports_sync.firstbyte(r)
            except Exception as e: jmp[r] = f"err:{e}"
        try: print(f"  asi base: {script.exports_sync.asibase()}")
        except Exception: pass

    print("\n=== RESULT ===")
    if state["crash"]:
        c = state["crash"]
        print(f"  CRASH: {c['type']} eip={c['eip']} mem={c.get('mem_address')} op={c.get('mem_op')}")
        (LOG_DIR / "crash_eip.txt").write_text(json.dumps(c, indent=2), encoding="utf-8")
        print(f"  -> log/crash_eip.txt (symbolize: py -3.12 re/frida/symbolize_asi_crash.py)")
    else:
        print(f"  survived {seconds}s, alive={alive}")
        for r, b in jmp.items():
            tag = "JMP-LIVE(0xE9)" if b == 0xE9 else f"NOT-INSTALLED(0x{b:02x})" if isinstance(b,int) and b>=0 else str(b)
            print(f"  {r}: {tag}")
    if MANIFEST.exists():
        lines = MANIFEST.read_text(encoding="utf-8", errors="replace").strip().splitlines()
        inst = [l for l in lines if l.split('\t')[2:3] == ['1']]
        print(f"  manifest: {len(lines)} hooks seen, {len(inst)} installed")
        for l in inst[:20]: print(f"    {l}")

    try:
        if alive: device.kill(pid)
    except Exception: pass
    try: session.detach()
    except Exception: pass
    return 0 if not state["crash"] else 2

if __name__ == "__main__":
    sys.exit(main())
