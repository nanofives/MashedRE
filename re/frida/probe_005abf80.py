# probe_005abf80.py — settle whether our AudioWaveVtableSlot1cDispatch hook is
# actually installed at 0x005abf80, and capture the callback value the geometry
# (clump) load path feeds through it just before the loading-screen crash.
#
# Spawns original\MASHED.exe (so the dinput8 shim loads the .asi as usual),
# waits for the .asi, reads the first bytes at 0x005abf80 (E9 => our inline-JMP
# hook is live; otherwise the original mov runs), then attaches an Interceptor at
# the ORIGINAL body offset to log *(arg0+0xc) and *(*(arg0+0xc)+0x1c).
import frida, sys, time

EXE = r"C:\Users\maria\Desktop\Proyectos\Mashed\original\MASHED.exe"
RVA = 0x005abf80  # absolute VA (image base 0x400000, non-relocatable)

JS = r"""
var BASE = 0x400000;
var FN   = ptr('0x005abf80');
function dump() {
  try {
    var b = FN.readByteArray(16);
    var u = new Uint8Array(b);
    var hex = [];
    for (var i=0;i<u.length;i++){ hex.push(('0'+u[i].toString(16)).slice(-2)); }
    send({t:'bytes', hex: hex.join(' ')});
    send({t:'hooked', v: (u[0]===0xe9 || u[0]===0xeb)});
  } catch(e) { send({t:'err', m:''+e}); }
}
// Read the patch site after a short delay so InjectHooks has run.
setTimeout(dump, 4000);
setTimeout(dump, 9000);

// Watch the dispatcher. If our hook is live, FN points at an E9 to the .asi;
// Interceptor.attach still fires on entry either way and lets us read args.
function arm() {
  try {
    Interceptor.attach(FN, {
      onEnter: function (args) {
        var node = args[0];
        try {
          var ctx = node.add(0xc).readPointer();
          var cb  = ctx.add(0x1c).readPointer();
          var v = cb.toUInt32 ? cb.toUInt32() : parseInt(cb.toString(),16);
          // Only shout about the dangerous (non-.text) callbacks.
          if (v < 0x401000 || v >= 0x995000) {
            send({t:'badcb', node: node.toString(), ctx: ctx.toString(), cb: cb.toString()});
          }
        } catch(e) { send({t:'enter_err', m:''+e}); }
      }
    });
    send({t:'armed'});
  } catch(e) { send({t:'arm_err', m:''+e}); }
}
setTimeout(arm, 4200);
"""

def main():
    pid = frida.spawn(EXE)
    session = frida.attach(pid)
    script = session.create_script(JS)
    seen = {"bad": 0}
    def on_msg(m, data):
        if m.get("type") == "send":
            p = m["payload"]
            t = p.get("t")
            if t == "bytes":
                print("  bytes@0x5abf80:", p["hex"], flush=True)
            elif t == "hooked":
                print("  inline-JMP hook installed?:", p["v"], flush=True)
            elif t == "armed":
                print("  interceptor armed", flush=True)
            elif t == "badcb":
                seen["bad"] += 1
                if seen["bad"] <= 8:
                    print("  >>> GARBAGE CALLBACK node=%s ctx=%s cb=%s" % (p["node"], p["ctx"], p["cb"]), flush=True)
            elif t in ("err","arm_err","enter_err"):
                print("  [%s] %s" % (t, p.get("m")), flush=True)
        elif m.get("type") == "error":
            print("  [script-error]", m.get("description"), flush=True)
    script.on("message", on_msg)
    script.load()
    frida.resume(pid)
    print("spawned pid=%d, watching ~14s" % pid, flush=True)
    t0 = time.time()
    while time.time() - t0 < 14:
        try:
            if not frida.enumerate_processes() or all(pp.pid != pid for pp in frida.enumerate_processes()):
                print("  process gone at t=%.1f" % (time.time()-t0), flush=True)
                break
        except Exception:
            pass
        time.sleep(0.5)
    print("  total bad callbacks seen:", seen["bad"], flush=True)
    try:
        session.detach()
    except Exception:
        pass
    try:
        frida.kill(pid)
    except Exception:
        pass

if __name__ == "__main__":
    main()
