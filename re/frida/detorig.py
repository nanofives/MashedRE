# Deterministic-original scenario run (PROMOTE-DETORIG-PILOT, 2026-11-01). Port of SimGolf's
# re/frida/scenario_ab.py + js/scenario_det.js, cut to one pilot scenario: agent re/frida/js/detorig.js.
#
# One run = Popen MASHED.exe (never frida.spawn), attach, wait for the menu (DAT_00771968 == 1), point the
# main thread's timeGetTime / GetTickCount / QueryPerformanceCounter IAT slots at virtual-clock stubs, then
# on the virtual timeline (N = main-thread clock calls): warp into the scenario race on the main thread
# (selection globals, FUN_0040e480, RNG pins, phase 2), feed scripted player-0 input through
# FUN_00497310, and dump the whole writable image (.data + _rwdseg) at fixed checkpoints.
# Kills only the pid it launched.
#
#   py -3.12 re/frida/detorig.py --label off_a --out D:/MashedWT/<step>_data/off_a [--hooks 0x00468980,0x00470670]
#   py -3.12 re/frida/detorig.py --compare DIR_A DIR_B          # checkpoint-by-checkpoint diff
#
# MASHED_ROOT selects the game install (<root>/original/MASHED.exe).
import argparse, ctypes, ctypes.wintypes as wt, hashlib, json, os, subprocess, sys, time
from pathlib import Path

HERE = Path(__file__).resolve().parent
ROOT = HERE.parent.parent
GAME_ROOT = Path(os.environ.get("MASHED_ROOT", ROOT))
EXE = GAME_ROOT / "original" / "MASHED.exe"
DATA_LO = 0x005ea000

# The pilot scenario: QuickRace (mode 10) on track 0, player + 3 AI, rule 0. Player 0 holds control 4
# (accelerate, scenario_launch.py's press(4)) from the warp on, and steers with controls 0 / 1 in pulses.
SCENARIO = {"track": 0, "mode": 10, "rule": 0, "car": 0, "team": 0, "cars": 4}
INPUT = [  # [dN after the warp, control, value]
    [0, 4, 0xff],
    [6000, 0, 0xff], [6600, 0, 0],
    [9000, 1, 0xff], [9900, 1, 0],
    [12000, 0, 0xff], [12300, 0, 0],
]
CKPTS = [2000 * k for k in range(1, 16)]   # ~180 game frames apart at ~11 main-thread QPC calls per frame


def main_thread_of(pid, timeout=40.0):
    """Thread id that owns MASHED's top-level visible window."""
    u = ctypes.windll.user32
    found = []
    PROC = ctypes.WINFUNCTYPE(wt.BOOL, wt.HWND, wt.LPARAM)

    def cb(h, _):
        p = wt.DWORD()
        tid = u.GetWindowThreadProcessId(h, ctypes.byref(p))
        if p.value == pid and u.IsWindowVisible(h):
            found.append(tid)
        return True
    t_end = time.time() + timeout
    while time.time() < t_end:
        found.clear()
        u.EnumWindows(PROC(cb), 0)
        if found:
            return found[0]
        time.sleep(0.25)
    return None


def run(args):
    import frida
    out = Path(args.out); out.mkdir(parents=True, exist_ok=True)
    env = dict(os.environ)
    env["MASHED_FPS_CAP"] = str(args.fps)
    env.setdefault("MASHED_MUTE", "1")
    if args.hooks:
        env["MASHED_RE_DEV"] = "1"
        env["MASHED_HOOK_ONLY"] = args.hooks
        env["MASHED_HOOK_MANIFEST"] = str(out / "hook_manifest.txt")
        env.pop("MASHED_RE_NO_AUTO_HOOK", None)
    else:
        env["MASHED_RE_NO_AUTO_HOOK"] = "1"
    for k in ("MASHED_PHYS_C4_SELFTEST", "MASHED_SHADOW_AB"):
        env.pop(k, None)
    proc = subprocess.Popen([str(EXE)], cwd=str(EXE.parent), env=env)
    pid = proc.pid
    print(f"=== detorig {args.label} pid={pid} hooks={args.hooks or '-'} exe={EXE}", flush=True)
    meta = {"label": args.label, "pid": pid, "hooks": args.hooks, "exe": str(EXE), "fps": args.fps,
            "scenario": SCENARIO, "input": INPUT, "ckpts": CKPTS, "ckpt": []}
    rc = 1
    try:
        dev = frida.get_local_device()
        sess = None
        for _ in range(200):
            try:
                sess = dev.attach(pid); break
            except Exception:
                time.sleep(0.1)
        if sess is None:
            print("error: attach failed"); return 3
        main = main_thread_of(pid)
        print(f"  main thread {main}", flush=True)
        cfg = {"main": main, "t0": 0x01000000, "step": 1, "q0": int(args.q0, 16), "qstep": args.qstep,
               "warpAt": args.warp_at, "scen": SCENARIO, "input": INPUT, "ckpts": CKPTS,
               "holdrand": 0x12345678, "witness": [0x00468980, 0x00470670],
               "negctl": int(args.negctl, 16) if args.negctl else 0, "rebase": args.rebase,
               "loadSleepMs": args.load_sleep_ms, "callLog": args.call_log,
               "watch": [int(x, 16) for x in args.watch.split(",") if x][:2],
               "callLogRace": args.call_log_race}
        if args.audio_off:
            cfg["audioOff"] = args.audio_off   # RO-AUDIO-ISOLATION: suspend the __beginthread workers (detorig.js audioOff)
        if args.poison:
            # RO-STACKROW-READERS: poison the event-ring fields each row's producer leaves unwritten (detorig.js, cfg.poison)
            cfg["poison"] = {"mode": args.poison, "value": int(args.poison_value, 16)}
        if args.normalize:
            cfg["norm"] = load_norm(args.normalize)["pin"]; cfg["normAt"] = args.normalize_at
            print(f"  normalise {len(cfg['norm'])} dwords at {args.normalize_at} ({args.normalize})", flush=True)
        if args.clock == "tick" or args.tick_hash:
            # REPLAY-ORACLE-DESIGN: per-tick block hashes at --tick-va (detorig.js, cfg.tick). With --clock tick the
            # clock also advances only there; with --clock n --tick-hash it keeps the pilot's per-call steps.
            tick = args.clock == "tick"
            if tick:
                cfg["step"] = 0; cfg["qstep"] = 0
            cfg["tick"] = {"va": int(args.tick_va, 16), "qtick": args.qtick if tick else 0, "ttick": args.ttick if tick else 0,
                           "cap": args.tick_cap, "block": args.tick_block, "pin": tick,
                           "raw": [[int(x, 16), int(y, 16)] for x, y in (r.split("-") for r in args.tick_raw.split(",") if r)]}
        if args.prov or args.prov_count:
            # PROV-INSITU: DDK bodies (re/tools/prov/provlink.py JSON) behind inline JMPs from the warp on, and/or
            # native call counters only (census). detorig.js provInstall.
            bodies = []
            if args.prov:
                allb = json.loads(Path(args.prov).read_text())["bodies"]
                want = {x.strip().lower() for x in args.prov_rvas.split(",") if x.strip()} if args.prov_rvas else None
                bodies = [{"rva": b["rva"], "code": b["code"], "abs": b["abs"], "ext": b["ext"], "entry": b["entry"]}
                          for b in allb if b["status"] == "OK" and (want is None or b["rva"].lower() in want)]
            cnt = []
            if args.prov_count:
                cnt = [x.strip() for x in (Path(args.prov_count).read_text().split() if Path(args.prov_count).exists()
                                           else args.prov_count.split(",")) if x.strip()]
            cfg["prov"] = {"bodies": bodies, "count": cnt, "wlog": args.prov_wlog, "wlive": (str(out / "wlive.jsonl").replace("\\", "/") if args.prov_wlog else "")}
            print(f"  prov: {len(bodies)} bodies, {len(cnt)} counters", flush=True)
        if args.warp_clock:
            cfg["warpClock"] = args.warp_clock
        meta["cfg"] = {k: v for k, v in cfg.items() if k != "prov"}
        if "prov" in cfg:
            meta["prov_cfg"] = {"bodies": [b["rva"] for b in cfg["prov"]["bodies"]], "count": cfg["prov"]["count"], "file": args.prov}
        src = "const DETORIG_CFG = " + json.dumps(cfg) + ";\n" + (HERE / "js" / "detorig.js").read_text(encoding="utf-8")
        state = {"done": False, "err": None}

        def on_msg(m, data):
            if m.get("type") == "error":
                state["err"] = m.get("description"); print("  agent error:", m.get("description"), m.get("stack", ""), flush=True); return
            p = m["payload"]
            if p["kind"] == "ckpt":
                f = out / f"ckpt{p['k']:02d}.data.bin"
                f.write_bytes(data)
                p["sha256"] = hashlib.sha256(data).hexdigest()
                p["file"] = f.name
                meta["ckpt"].append(p)
                print(f"  ckpt {p['k']} dn={p['dn']} phase={p['phase']} clk={p['clk_7f0ff0']} sha={p['sha256'][:12]}", flush=True)
            elif p["kind"] == "watch":
                with open(out / "watch.jsonl", "a") as f:
                    f.write(json.dumps(p) + "\n")
                meta["watch_hits"] = meta.get("watch_hits", 0) + 1
            elif p["kind"] == "calllog":
                (out / "calllog.bin").write_bytes(data or b"")
                meta["calllog"] = {"count": p["count"], "rec": p["rec"], "file": "calllog.bin"}
                print(f"  call log {p['count']} calls", flush=True)
            elif p["kind"] == "warp":
                meta["warp"] = p; print(f"  warp at N={p['n']} pins={p['pins']}", flush=True)
            elif p["kind"] == "race":
                meta["race"] = p; print(f"  race anchor N={p['n']} dn_warp={p['dn_warp']} frames={p['frames_7f101c']} clk={p['clk_7f0ff0']}/{p['clk_7f0ff4']} witness={p['witness']}", flush=True)
            elif p["kind"] == "ticks":
                with open(out / "ticks.bin", "ab" if p["first"] else "wb") as f:
                    f.write(data)
                meta["ticks"] = {"total": p["total"], "nblk": p["nblk"], "recw": p["recw"], "block": p["block"], "raw": p.get("raw"), "file": "ticks.bin"}
                if p["first"] + p["count"] >= p["total"]:
                    print(f"  ticks {p['total']} x {p['nblk']} blocks", flush=True)
            elif p["kind"] == "wlog":
                with open(out / "wlog.bin", "ab" if p["first"] else "wb") as f:
                    f.write(data or b"")
                meta["wlog"] = {"records": p["total"], "stats": p["stats"], "file": "wlog.bin"}
                if p["first"] + p["count"] >= p["total"]:
                    print(f"  wlog {p['total']} records stats(buf,cap,count,lo,hi,depth,overflow,traps,frames,strays,bpmiss,lastStrayIp,strayDepth,-,-,otherThreadSteps,tidFn,mainTid)={p['stats']}", flush=True)
            elif p["kind"] == "wcrash":
                (out / "wcrash.json").write_text(json.dumps(p, indent=0)); (out / "wcrash_tail.bin").write_bytes(data or b"")
                print("  WCRASH", {k: p[k] for k in ("type", "address", "memory", "ctx", "stats", "blob", "n")}, flush=True)
            elif p["kind"] == "provinst":
                (out / "prov_install.json").write_text(json.dumps(p, indent=0))
                bad = [x for x in p["installed"] if x.get("err") or x.get("jmp_ok") is False]
                print(f"  prov install {len(p['installed'])} sites, {len(bad)} failed {bad[:3]}", flush=True)
            elif p["kind"] == "prov":
                (out / "prov.json").write_text(json.dumps(p, indent=0))
                meta["prov"] = {"file": "prov.json", "executed": sum(1 for r in p["rows"] if r["calls_total"]),
                                "install_fail": sum(1 for x in p["installed"] if x.get("err") or x.get("jmp_ok") is False)}
                print(f"  prov rows {len(p['rows'])} executed {meta['prov']['executed']} install_fail {meta['prov']['install_fail']}", flush=True)
            elif p["kind"] == "done":
                state["done"] = True
            else:
                print("  [agent]", p, flush=True)
                if p["kind"] == "err": state["err"] = p["msg"]

        scr = sess.create_script(src); scr.on("message", on_msg); scr.load()
        E = scr.exports_sync
        t_end = time.time() + 60
        while time.time() < t_end and E.phase() != 1:
            if proc.poll() is not None: print("  game exited before the menu"); return 4
            time.sleep(0.25)
        if E.phase() != 1:
            print("  timeout waiting for the menu"); return 5
        time.sleep(args.menu_wait)
        print("  install", E.install(), flush=True)
        t_end = time.time() + args.timeout
        last = 0
        while time.time() < t_end and not state["done"] and not state["err"]:
            if proc.poll() is not None:
                print(f"  game exited rc={proc.returncode}"); meta["exit"] = proc.returncode; break
            if time.time() - last > 5:
                try: print("  stats", E.stats(), flush=True)
                except Exception as e: print("  stats failed", e)
                last = time.time()
            time.sleep(0.2)
        meta["stats_end"] = E.stats() if proc.poll() is None else None
        rc = 0 if state["done"] else 6
    finally:
        meta["rc"] = rc
        (out / "meta.json").write_text(json.dumps(meta, indent=1))
        if proc.poll() is None:
            proc.kill()
    print(f"=== rc={rc} ckpts={len(meta['ckpt'])}", flush=True)
    return rc


# Named ranges for the diff summary (addresses from re/frida/scenario_launch.py and the survey of 2026-11-01).
REGIONS = [
    (0x008815a0, 0x008815a0 + 4 * 0xd04, "car records 0x008815a0 (4 x 0xd04)"),
    (0x007f0ff0, 0x007f1038, "frame timer block 0x007f0ff0..0x007f1034"),
    (0x007f1038, 0x007f1038 + 4 * 0x4c, "cooked input 0x007f1038"),
    (0x007f14f8, 0x007f14f8 + 4 * 0x4c, "previous cooked input 0x007f14f8"),
    (0x00881560, 0x00881590, "A5 scratch 0x00881560"),
    (0x0088fbe0, 0x0088fbe0 + 64 * 0xb4, "pickup slots 0x0088fbe0"),
    (0x00771968, 0x00771969, "phase 0x00771968"),
    (0x00771984, 0x00771988, "timer drift 0x00771984"),
]


PTR_LO = 0x00995000


def is_ptr(v):
    # Heap range on this machine (0x0146xxxx..0x0f9xxxxx seen 2026-11-01), dword-aligned. The same bit
    # pattern as a float is a magnitude below 4.7e-10, so a float is not taken for a pointer in practice.
    # REPLAY-ORACLE-DESIGN (2026-11-11): lower bound moved from 0x01000000 to PTR_LO = 0x00995000, the end of
    # MASHED.exe's image (ImageBase + SizeOfImage): run t_b put heap blocks at 0x00d0a644 / 0x00d0eee0.
    return PTR_LO <= v < 0x30000000 and v & 3 == 0


def load_norm(path):
    """re/frida/warp_normalize.tsv (RO-WARP-NORMALIZE). Columns: va count stride width action value writer readers note.
    action PIN = write `value` at the warp / race anchor (both sides). MASK:<family> / RULE:<name> = not pinned, scored by
    detorig_report.py as that class. Returns {"pin": [[va, width, value], ...], "mask": {va: class}}."""
    pin, mask = [], {}
    for line in Path(path).read_text(encoding="utf-8").splitlines():
        if not line.strip() or line.startswith("#") or line.startswith("va\t"):
            continue
        f = line.split("\t")
        va, cnt, stride, width, action = int(f[0], 16), int(f[1]), int(f[2], 16), int(f[3]), f[4]
        for k in range(cnt):
            a = va + k * stride
            if action == "PIN":
                pin.append([a, width, int(f[5], 16)])
            else:
                for j in range(0, max(width, 4), 4):
                    mask[a + j] = action
    return {"pin": pin, "mask": mask}


def is_ptr_pair(a, b):
    """A heap-layout difference, not a value difference: the heap starts 0x40000 apart between OFF runs
    and at non-page offsets when the .asi installs hooks (measured 2026-11-01)."""
    return is_ptr(a) and is_ptr(b)


def region_of(va):
    for lo, hi, name in REGIONS:
        if lo <= va < hi:
            return name
    return "other page 0x%08x" % (va & ~0xfff)


def compare(a, b, ign=None):
    A = json.loads((Path(a) / "meta.json").read_text()); B = json.loads((Path(b) / "meta.json").read_text())
    ka = {c["k"]: c for c in A["ckpt"]}; kb = {c["k"]: c for c in B["ckpt"]}
    rows = []
    for k in sorted(set(ka) | set(kb)):
        if k not in ka or k not in kb:
            rows.append({"k": k, "verdict": "MISSING"}); continue
        x, y = ka[k], kb[k]
        da = (Path(a) / x["file"]).read_bytes(); db = (Path(b) / y["file"]).read_bytes()
        diff_all = [i for i in range(0, min(len(da), len(db)), 4) if da[i:i + 4] != db[i:i + 4]]
        # A dword whose two values are both heap addresses is a heap-layout difference (is_ptr_pair).
        ptr = [i for i in diff_all if is_ptr_pair(int.from_bytes(da[i:i + 4], "little"), int.from_bytes(db[i:i + 4], "little"))]
        ptrset = set(ptr)
        diff = [i for i in diff_all if i not in ptrset]
        pages = sorted({(DATA_LO + i) & ~0xfff for i in diff})
        meta_keys = ["n", "dn", "T", "Qlo", "phase", "clk_7f0ff0", "clk_7f0ff4", "frames_7f101c", "holdrand", "rw_cur"]
        mdiff = {m: (x.get(m), y.get(m)) for m in meta_keys if x.get(m) != y.get(m)}
        if x.get("rw_ring") != y.get("rw_ring"): mdiff["rw_ring"] = "differs"
        by_region = {}
        for i in diff:
            r = region_of(DATA_LO + i); by_region[r] = by_region.get(r, 0) + 1
        rows.append({"k": k, "dn": x["dn"], "by_region": by_region, "verdict": "SAME" if not diff and not mdiff else "DIFF",
                     "raw_identical": not diff_all, "ptr_dwords": len(ptr), "dwords": len(diff), "pages": len(pages), "meta": mdiff,
                     "first": [hex(DATA_LO + i) for i in diff[:40]], "page_list": [hex(p) for p in pages[:60]],
                     "sha_a": x["sha256"], "sha_b": y["sha256"]})
    return rows


if __name__ == "__main__":
    ap = argparse.ArgumentParser()
    ap.add_argument("--label", default="run")
    ap.add_argument("--out", default="")
    ap.add_argument("--hooks", default="")
    ap.add_argument("--fps", default="60")
    ap.add_argument("--negctl", default="", help="negative control at the race anchor: 1 = RW RNG word at the cursor +1, else a hex VA bumped by one")
    ap.add_argument("--rebase", action="store_true", help="set T / Q at the race anchor (runs off_e/off_f/neg_b; made worse, see RESULT)")
    ap.add_argument("--load-sleep-ms", type=int, default=50, help="load hold: real ms slept per main-thread clock call until phase 3 (0 = off)")
    ap.add_argument("--watch", default="", help="comma list (max 2) of hex VAs: hardware write watchpoints from the race anchor on (watch.jsonl)")
    ap.add_argument("--call-log", action="store_true", help="log every main-thread clock call from the warp to the race anchor (calllog.bin, 64 B per call)")
    ap.add_argument("--qstep", type=int, default=15000, help="QPC counts per main-thread QPC call")
    ap.add_argument("--clock", choices=["n", "tick"], default="n",
                    help="n: the clock advances per main-thread call (pilot); tick: only at --tick-va's entry (REPLAY-ORACLE-DESIGN)")
    ap.add_argument("--tick-va", default="0x00493390", help="tick boundary: FUN_00493390 frame timer, once per main-loop iteration")
    ap.add_argument("--qtick", type=int, default=166667, help="QPC counts per tick (QPF 10^7 / 60)")
    ap.add_argument("--ttick", type=int, default=17, help="timeGetTime / GetTickCount ms per tick")
    ap.add_argument("--tick-cap", type=int, default=3000, help="ticks hashed from the race anchor on")
    ap.add_argument("--tick-block", type=int, default=1024, help="bytes per hashed block")
    ap.add_argument("--tick-hash", action="store_true", help="per-tick block hashes with --clock n")
    ap.add_argument("--tick-raw", default="", help="comma list of LO-HI hex VA ranges copied raw into every tick record")
    ap.add_argument("--q0", default="0x40000000", help="virtual QPC start (low dword)")
    ap.add_argument("--call-log-race", action="store_true", help="keep the call log on past the race anchor (until 4096 calls)")
    ap.add_argument("--normalize", default="", help="boot-counter normalisation file (re/frida/warp_normalize.tsv): its PIN rows are written on the original")
    ap.add_argument("--normalize-at", choices=["warp", "anchor", "both"], default="both", help="when the PIN rows are written")
    ap.add_argument("--audio-off", choices=["warp", "anchor"], default="", help="suspend every non-main thread whose start address is in MASHED.exe (the __beginthread audio workers) at the warp or the race anchor")
    ap.add_argument("--poison", choices=["stale", "pos"], default="", help="overwrite the unwritten event-ring fields at every ring consumer from the race anchor on (pos = also type-4 +0x24, a read field)")
    ap.add_argument("--poison-value", default="0x7fc0dead", help="poison dword (hex)")
    ap.add_argument("--warp-clock", type=int, default=0, help="tick clock: set the virtual T / Q to start + K ticks at the warp, before the timer pin (PROV-INSITU)")
    ap.add_argument("--prov", default="", help="PROV-INSITU bodies JSON (re/tools/prov/provlink.py): inline JMP to each DDK body from the warp on")
    ap.add_argument("--prov-rvas", default="", help="comma list: only these bodies of --prov")
    ap.add_argument("--prov-wlog", type=int, default=0, help="PROV-WRITESET: single-step log of the first N calls of every --prov body (detorig.js wsInstall)")
    ap.add_argument("--prov-count", default="", help="comma list or file of RVAs that get a native call counter only (census)")
    ap.add_argument("--warp-at", type=int, default=600, help="N after install at which the warp runs")
    ap.add_argument("--menu-wait", type=float, default=2.0)
    ap.add_argument("--timeout", type=float, default=600)
    ap.add_argument("--compare", nargs=2, metavar=("DIR_A", "DIR_B"))
    a = ap.parse_args()
    if a.compare:
        rows = compare(*a.compare)
        for r in rows:
            print(json.dumps({k: v for k, v in r.items() if k not in ("page_list",)}))
        same = sum(r["verdict"] == "SAME" for r in rows)
        print(f"determinism: {same}/{len(rows)} checkpoints identical")
        sys.exit(0)
    if not a.out:
        ap.error("--out required")
    sys.exit(run(a))
