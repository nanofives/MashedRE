"""Why does the race-scenario path2 lose its Frida session during `[nav] booting to menu...`?

Re-creates run_verify_hook.py's spawn + run_diff.drive_to_race(), with a crash handler loaded first and the process
exit code read afterwards. Variants:
    --detach   spawn with CREATE_NEW_PROCESS_GROUP | DETACHED_PROCESS and DEVNULL std handles (run_diff.py's spawn);
               without it the child inherits the parent's console / pipes (run_verify_hook.py's spawn)
    --only X   MASHED_HOOK_ONLY=X (default: unset = the whole auto-hook set; use __none__ for no hooks installed)
    --noauto   MASHED_RE_NO_AUTO_HOOK=1 (run_diff.py's env)
Only the pid this script spawns is ever touched.

    py -3.12 nav_probe.py [--detach] [--only __none__] [--noauto] <label>
"""
import json
import os
import subprocess
import sys
import time
from pathlib import Path

import frida

ROOT = Path(__file__).resolve().parent.parent.parent
sys.path.insert(0, str(ROOT / "re" / "frida"))
import run_diff as RD  # noqa: E402

CRASH_JS = r"""
'use strict';
const FATAL = {'access-violation':1,'illegal-instruction':1,'abort':1,'bus-error':1,'division-by-zero':1,'stack-overflow':1};
Process.setExceptionHandler(function (d) {
  if (!FATAL[d.type]) return false;
  const c = d.context; let st = [];
  try { const sp = ptr(c.esp.toString()); for (let i = 0; i < 24; i++) { try { st.push(sp.add(i*4).readU32().toString(16)); } catch (e) { break; } } } catch (e) {}
  send({kind:'crash', type:d.type, eip:c.pc.toString(), addr:(d.memory&&d.memory.address)?d.memory.address.toString():'?',
        op:(d.memory&&d.memory.operation)?d.memory.operation:'?', eax:c.eax.toString(), ecx:c.ecx.toString(), esp:c.esp.toString(), stack:st,
        mods:Process.enumerateModules().filter(m => /asi|MASHED/i.test(m.name)).map(m => [m.name, m.base.toString(), m.size])});
  return false;
});
send({kind:'ready'});
"""


def main() -> int:
    av = sys.argv[1:]
    detach = "--detach" in av
    noauto = "--noauto" in av
    only = av[av.index("--only") + 1] if "--only" in av else None
    extra = [av[i + 1] for i, a in enumerate(av) if a == "--env"]          # K=V, repeatable (MASHED_HOOK_LO/HI/SKIP ...)
    label = [a for a in av if not a.startswith("--") and a != only and a not in extra][-1]
    env = {k: v for k, v in os.environ.items() if k not in ("MASHED_RE_NO_AUTO_HOOK", "MASHED_HOOK_ONLY")}
    for kv in extra:
        k, v = kv.split("=", 1)
        env[k] = v
    if noauto:
        env["MASHED_RE_NO_AUTO_HOOK"] = "1"
    if only:
        env["MASHED_HOOK_ONLY"] = only
    kw = {}
    if detach:
        kw = dict(stdin=subprocess.DEVNULL, stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL,
                  creationflags=0x200 | 0x8)
    t0 = time.time()
    proc = subprocess.Popen([str(RD.MASHED_EXE)], cwd=str(RD.MASHED_EXE.parent), env=env, **kw)
    print(f"[{label}] pid={proc.pid} detach={detach} only={only} noauto={noauto}", flush=True)
    time.sleep(0.2)
    dev = frida.get_local_device()
    sess = dev.attach(proc.pid)
    crashes = []
    detached = []
    sess.on("detached", lambda reason, crash: detached.append((reason, str(crash)[:300])))
    scr = sess.create_script(CRASH_JS)
    scr.on("message", lambda m, d: crashes.append(m["payload"]) if m.get("type") == "send" and m["payload"].get("kind") == "crash" else None)
    scr.load()
    keep = RD.pin_gamesave(sess, proc.pid)
    err = None
    try:
        ok = RD.drive_to_race(sess)
    except Exception as e:  # noqa: BLE001
        ok, err = False, f"{type(e).__name__}: {e}"
    time.sleep(1.0)
    rc = proc.poll()
    print(f"[{label}] drive_to_race={ok} err={err} after {time.time()-t0:.0f}s  proc.poll()={rc if rc is None else hex(rc & 0xffffffff)}")
    print(f"[{label}] frida detached events={detached}")
    for c in crashes:
        print(f"[{label}] CRASH {json.dumps(c)}")
    try:
        proc.kill()
    except Exception:
        pass
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
