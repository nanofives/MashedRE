"""Spawn an exe under Frida, catch the first access violation and print the faulting address, its map symbol and a
stack scan of map symbols (DDKOBJ-SPIKE). Run inside mashed_lock.

    py -3.12 re/tools/ddkobj/catch_crash.py <exe> <map> <root> <timeout_s> [KEY=VALUE ...]
"""
import bisect
import os
import re
import sys
import threading

import frida

SCRIPT = r"""
Process.setExceptionHandler(function (d) {
  var m = Process.findModuleByAddress(d.address);
  var base = Process.enumerateModules()[0].base;
  var stk = [];
  try {
    var sp = d.context.esp;
    for (var i = 0; i < 256; i++) { stk.push(sp.add(i * 4).readU32()); }
  } catch (e) {}
  send({type: d.type, address: d.address.toString(), module: m ? m.name : null,
        base: base.toString(), mem: d.memory ? {op: d.memory.operation, addr: d.memory.address.toString()} : null,
        ctx: {eax: d.context.eax.toString(), ebx: d.context.ebx.toString(), ecx: d.context.ecx.toString(),
              edx: d.context.edx.toString(), esi: d.context.esi.toString(), edi: d.context.edi.toString(),
              ebp: d.context.ebp.toString(), esp: d.context.esp.toString()}, stack: stk});
  return false;
});
"""


def load_map(path):
    rows = []
    for line in open(path, errors="replace"):
        m = re.match(r"\s*[0-9a-fA-F]{4}:[0-9a-fA-F]{8}\s+(\S+)\s+([0-9a-fA-F]{8})\s+(?:f\s+)?(?:i\s+)?(\S+)?", line)
        if m:
            rows.append((int(m.group(2), 16), m.group(1), m.group(3) or ""))
    rows.sort()
    return rows


def sym(rows, keys, va):
    i = bisect.bisect_right(keys, va) - 1
    if i < 0:
        return "?"
    a, n, o = rows[i]
    return f"{n}+0x{va - a:x} ({o})"


def main():
    exe, mp, root, tmo = sys.argv[1], sys.argv[2], sys.argv[3], float(sys.argv[4])
    env = dict(os.environ, MASHED_ROOT=root)
    for kv in sys.argv[5:]:
        k, v = kv.split("=", 1)
        env[k] = v
    rows = load_map(mp)
    keys = [r[0] for r in rows]
    dev = frida.get_local_device()
    pid = dev.spawn([exe], cwd=root, env=env)
    print("spawned pid", pid, flush=True)
    sess = dev.attach(pid)
    done = threading.Event()

    def on_msg(msg, _data):
        if msg.get("type") != "send":
            print(msg)
            return
        p = msg["payload"]
        a = int(p["address"], 16)
        print(f"EXCEPTION {p['type']} at 0x{a:08x} {p['module']} -> {sym(rows, keys, a)}")
        print("  mem", p["mem"], "ctx", p["ctx"])
        lo, hi = keys[0], keys[-1] + 0x10000
        n = 0
        for v in p["stack"]:
            if lo <= v < hi:
                print("   stack ->", f"0x{v:08x}", sym(rows, keys, v))
                n += 1
                if n >= 14:
                    break
        done.set()

    sc = sess.create_script(SCRIPT)
    sc.on("message", on_msg)
    sc.load()
    dev.resume(pid)
    done.wait(tmo)
    try:
        dev.kill(pid)
    except Exception:
        pass
    print("caught" if done.is_set() else "no exception within timeout")


if __name__ == "__main__":
    main()
