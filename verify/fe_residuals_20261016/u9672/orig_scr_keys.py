"""U-9672: ORIGINAL behaviour of a directly pushed screen under a key list (own PID only, game memory untouched
except the statenav FUN_00497310 return override, the same input path u9667_menu_table_dump.py --lane input uses).
Usage: py -3.12 orig_scr_keys.py <screen> <keys d,u,e,r,l,...> <out.json>
Boots original\MASHED.exe (MASHED_RE_NO_AUTO_HOOK=1), waits for the menu (phase 3), pushes the screen with
FUN_0043d2a0(S,0), then presses each key (d=12 down, u=11 up, e=4 confirm, r=10 right, l=9 left) and records the top
slot (table ptr, cursor), depth, phase, game mode, row, sel after every press, plus every 0x0042ac90 call."""
import json, sys, time
from pathlib import Path
ROOT = Path(__file__).resolve().parents[3]
sys.path.insert(0, str(ROOT / 're' / 'frida'))
import u9667_menu_table_dump as U
import statenav
from statenav import Nav
KEY = {'d': 12, 'u': 11, 'e': 4, 'r': 10, 'l': 9}
scr = int(sys.argv[1]); keys = sys.argv[2].split(','); out = Path(sys.argv[3])
dev, pid, sess = U.spawn()
rec = {"screen": scr, "keys": keys, "pid": pid, "snaps": [], "exceptions": []}
try:
    S = U.load(sess, statenav.AGENT); S.exports_sync.init()
    ds = sess.create_script(U.DUMP_AGENT); ds.on("message", lambda m, d: rec["exceptions"].append(m.get("payload", m))); ds.load()
    D = ds.exports_sync; D.init()
    M = U.load(sess, U.MEG_AGENT).exports_sync; M.init()
    dev.resume(pid)
    nav = Nav(S, pid)
    def snap(tag):
        d = D.dump(4); s = d["slots"][d["depth"]]
        rec["snaps"].append({"tag": tag, "depth": d["depth"], "phase": d["phase"], "mode": d["mode"], "row": d["row"],
                             "sel": d["sel"], "ptr": hex(s["table_ptr"]), "cursor": s["cursor"]})
        print(rec["snaps"][-1], flush=True)
    U.wait_phase(D, 3, 30); time.sleep(1.5)
    snap("boot")
    D.push(scr); time.sleep(1.0)
    snap(f"push{scr}")
    for k, key in enumerate(keys):
        n0 = len(M.calls())
        if key == 'w':
            time.sleep(2.5); snap(f"k{k+1}w"); continue
        nav.press(KEY[key]); time.sleep(1.5 if key == 'e' else 0.4)
        snap(f"k{k+1}{key}")
        if key == 'e': rec["snaps"][-1]["meg_calls"] = M.calls()[n0:]
except Exception as e:
    rec["error"] = repr(e); print("error", e)
finally:
    try: dev.kill(pid)
    except Exception: pass
    out.parent.mkdir(parents=True, exist_ok=True); out.write_text(json.dumps(rec, indent=1))
