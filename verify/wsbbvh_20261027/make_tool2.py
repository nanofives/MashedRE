"""D4-WSB-BVH-WRITESET: generate c4_racediff_v6.py from the v5 tool (verify/wsbbvh_20261026/c4_racediff_bvh.py).

  py -3.12 verify/wsbbvh_20261027/make_tool2.py

Every replacement is asserted to apply exactly once. Two changes, both fixed in the PREREG:

1. Declared-width arg mask (ARGW). Arg index 2 (0-based) of a call to 0x005646c0 is masked to 0xffff
   before encoding, on both sides. Static proof: the original 0x005646c0 reads that stack slot at exactly
   4 sites (0x005647e3, 0x00564830, 0x00564aab, 0x00564af8; recursive descent from the entry, esp delta
   tracked per path incl. the 0x2ad8 stack probe at 0x004a3440, no delta conflict, one ret at 0x00564c78
   with delta 0) and each load is masked with `and reg, 0xffff` (0x005647ec, 0x0056483d, 0x00564ab4,
   0x00564b05) before any use. No other arg and no other callee is masked.
2. Census scenario driver (C4_SCENARIO=census). scenario_launch.py --track 0 --mode 10 --cars 4 boots the
   game, the v5 capture agent is attached to the PID it prints (second Frida session), armed once the .asi
   is loaded (ON: once the target's JMP byte reads 0xE9). ON = --hooks 0x<rva>, OFF = no --hooks
   (MASHED_RE_NO_AUTO_HOOK=1). Both boots MASHED_NO_SELFTEST=1 (no shadow A/B, no C4 self-test re-run).
   The game PID is killed by PID, then the launcher.
"""
from pathlib import Path

HERE = Path(__file__).resolve().parent
SRC = HERE.parent / "wsbbvh_20261026" / "c4_racediff_bvh.py"
DST = HERE / "c4_racediff_v6.py"

REPL = [
    ("  const IMG_LO = 0x00400000, IMG_HI = 0x00600000;\n",
     "  const IMG_LO = 0x00400000, IMG_HI = 0x00600000;\n"
     "  const ARGW = { 0x5646c0: { 2: 0xffff } };   // v6: declared-width arg mask (see make_tool2.py)\n"),
    ("      for(let j=0;j<n;j++){ try{ parts.push(nrm(sp.add((TB+j)*4).readU32())); }catch(e){ parts.push('?'); } }",
     "      const AWK=TOK?(parseInt(TOK.slice(1),16)>>>0):(inImg(tr)?tr:-1); const AW=ARGW[AWK];   // v6\n"
     "      for(let j=0;j<n;j++){ try{ let av=sp.add((TB+j)*4).readU32()>>>0; if(AW&&AW[j]!==undefined) av=(av&AW[j])>>>0;"
     " parts.push(nrm(av)); }catch(e){ parts.push('?'); } }"),
    ("  rpc.exports.obsar = function(){ return OBS_AR; };",
     "  rpc.exports.obsar = function(){ return OBS_AR; };\n"
     "  rpc.exports.asiloaded = function(){ try{ return Process.findModuleByName('mashed_re_dev.asi')!==null; }catch(e){ return false; } };   // v6"),
    ('    print("  bvh tool v5")', '    print("  bvh tool v6 scenario=" + os.environ.get("C4_SCENARIO", "quickbattle"))'),
]

CENSUS = r'''

# ---- v6: census scenario driver -------------------------------------------------------------------
import threading
LAUNCH = ROOT / "re" / "frida" / "scenario_launch.py"
def run_census(mode, rvas, targets, flo, fhi):
    env = dict(os.environ); env["MASHED_NO_SELFTEST"] = "1"; env.pop("MASHED_SHADOW_AB", None)
    env.setdefault("MASHED_MUTE", "1")
    cmd = [sys.executable, str(LAUNCH), "--track", "0", "--mode", "10", "--cars", "4", "--hold",
           os.environ.get("C4_HOLD", "60")]
    if mode == "on": cmd += ["--hooks", ",".join("0x%08x" % r for r in rvas)]
    lp = subprocess.Popen(cmd, cwd=str(ROOT), env=env, stdout=subprocess.PIPE, stderr=subprocess.STDOUT,
                          text=True, encoding="utf-8", errors="replace")
    box = {"pid": 0, "lines": []}
    got = threading.Event()
    def drain():
        for ln in lp.stdout:
            box["lines"].append(ln)
            m = _re.search(r"scenario_launch\s+pid=(\d+)", ln)
            if m and not box["pid"]: box["pid"] = int(m.group(1)); got.set()
    th = threading.Thread(target=drain, daemon=True); th.start()
    samples, jmp = {}, {}
    pid = 0
    try:
        if not got.wait(60): print(f"  [{mode}] launcher printed no pid"); return {}, {}
        pid = box["pid"]; print(f"  [{mode}] SPAWNED_PID={pid}", flush=True)
        dev = frida.get_local_device(); sess = None
        for _ in range(100):
            try: sess = dev.attach(pid); break
            except Exception: time.sleep(0.1)
        if sess is None: print(f"  [{mode}] attach failed"); return {}, {}
        scr = sess.create_script(CAP)
        scr.on("message", lambda m, d: print("  agent:", m.get("description")) if m["type"] == "error" else None)
        scr.load(); E = scr.exports_sync; E.init()
        end = time.time() + 90
        while time.time() < end:
            ok = E.asiloaded()
            if ok and mode == "on": ok = all(E.jmpbyte(r) == 0xe9 for r in rvas)
            if ok: break
            time.sleep(0.05)
        print(f"  [{mode}] armed: asi={E.asiloaded()} jmp={[E.jmpbyte(r) for r in rvas]}", flush=True)
        E.setupcap(targets, mode, flo, fhi)
        end = time.time() + 150
        while time.time() < end and E.frame() <= fhi and lp.poll() is None:
            time.sleep(0.05)
        try: E.stopcap()
        except Exception: pass
        print(f"  [{mode}] frame={E.frame()}", flush=True)
        samples = E.samples()
        if mode == "on":
            OBS.update(E.obsar() or {})
            for r in rvas:
                try: jmp[r] = E.jmpbyte(r)
                except Exception: jmp[r] = -1
        try: sess.detach()
        except Exception: pass
    finally:
        if pid: subprocess.run(["taskkill", "/PID", str(pid), "/F"], capture_output=True)
        try: lp.wait(30)
        except Exception: lp.kill()
        (LOG / f"launcher_{mode}.txt").write_text("".join(box["lines"]), encoding="utf-8")
    return samples, jmp
if os.environ.get("C4_SCENARIO") == "census":
    run = run_census
'''


def main():
    s = SRC.read_text(encoding="utf-8")
    for a, b in REPL:
        n = s.count(a)
        assert n == 1, (n, a[:80])
        s = s.replace(a, b)
    tail = 'if __name__ == "__main__":'
    assert s.count(tail) == 1
    s = s.replace(tail, CENSUS.lstrip("\n") + "\n" + tail)
    s = "# GENERATED by verify/wsbbvh_20261027/make_tool2.py from verify/wsbbvh_20261026/c4_racediff_bvh.py (v5 -> v6)\n" + s
    DST.write_text(s, encoding="utf-8")
    print("wrote", DST)


if __name__ == "__main__":
    main()
