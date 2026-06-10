# probe_005ab100.py — verify the StreamHandlerDispatchGuard hook is installed at
# 0x005ab100 and that 0x005ab148 / 0x005ab130 still hold the original bytes.
import frida, time

EXE = r"C:\Users\maria\Desktop\Proyectos\Mashed\original\MASHED.exe"
JS = r"""
function bytes(addr, n) {
  var u = new Uint8Array(ptr(addr).readByteArray(n));
  var h = [];
  for (var i=0;i<u.length;i++){ h.push(('0'+u[i].toString(16)).slice(-2)); }
  return h.join(' ');
}
function go() {
  try {
    send({a:'0x5ab100', hex: bytes('0x005ab100', 8)});
    send({a:'0x5ab130', hex: bytes('0x005ab130', 8)});
    send({a:'0x5ab148', hex: bytes('0x005ab148', 8)});
  } catch(e) { send({err: ''+e}); }
}
setTimeout(go, 4000);
setTimeout(go, 8000);
"""
def main():
    pid = frida.spawn(EXE)
    s = frida.attach(pid)
    sc = s.create_script(JS)
    sc.on("message", lambda m,d: print("  ", m.get("payload") if m.get("type")=="send" else m, flush=True))
    sc.load()
    frida.resume(pid)
    print("spawned pid=%d" % pid, flush=True)
    time.sleep(10)
    try: frida.kill(pid)
    except Exception: pass

if __name__ == "__main__":
    main()
