"""DDKOBJ-42AD90: which register-convention statics (DDKOBJ-REGCONV `__ddkreg_*` bodies and their adapters) a boot
reaches. Two legs, both on the linked object-build exe:

  static: every E8/E9 rel32 and every imm32 in the exe's code that points at a body or an adapter, attributed to the
          map symbol that contains it (the in-exe caller list).
  runtime: spawn the exe under Frida (MASHED_ROOT=<root>), put a one-shot listener on each body, each adapter and
           each direct caller found by the static leg (detached on its first hit, so a hot function costs one
           callback), run <timeout_s>, kill the PID it spawned, print first-hit times.

    py -3.12 re/tools/ddkobj/regconv_reach.py static <out_dir> <out_tsv>
    py -3.12 re/tools/ddkobj/regconv_reach.py run <out_dir> <root> <timeout_s> <out_tsv> [KEY=VALUE ...]

<out_dir> is the ddkobj build dir (mashed_re_ddk.exe, mashed_re_ddk.map, ddkobj_build.json). Run `run` inside
`mashed_lock.py run`.
"""
import bisect
import json
import os
import pathlib
import re
import struct
import sys
import threading
import time


def load_map(path):
    rows = []
    for line in open(path, errors="replace"):
        m = re.match(r"\s*([0-9a-fA-F]{4}):[0-9a-fA-F]{8}\s+(\S+)\s+([0-9a-fA-F]{8})\s+(f\s+)?(i\s+)?(\S+)?", line)
        if m and int(m.group(3), 16) >= 0x10000:
            rows.append((int(m.group(3), 16), m.group(2), m.group(6) or "", bool(m.group(4))))
    rows.sort()
    return rows


def targets(out):
    """body / adapter names from the build json -> {exe symbol: (rva, kind)}"""
    rep = json.loads((out / "ddkobj_build.json").read_text())
    t = {}
    for u in rep.get("units", []):
        for d in u.get("defs", []):
            if d.get("regconv"):
                t[d["exe"]] = (d["rva"], "body")
        for a in u.get("adapters", []):
            t[a["exe"]] = (a["rva"], "adapter")
    return t


def static_scan(out):
    import pefile
    exe = out / "mashed_re_ddk.exe"
    rows = load_map(out / "mashed_re_ddk.map")
    keys = [r[0] for r in rows]
    byname = {r[1]: r[0] for r in rows}
    tg = targets(out)
    va2 = {byname[n]: (n,) + tg[n] for n in tg if n in byname}
    missing = sorted(n for n in tg if n not in byname)
    pe = pefile.PE(str(exe), fast_load=True)
    base = pe.OPTIONAL_HEADER.ImageBase
    refs = {va: [] for va in va2}
    for s in pe.sections:
        if not (s.Characteristics & 0x20000000):     # executable sections only
            continue
        data = s.get_data()
        sva = base + s.VirtualAddress
        for off in range(len(data) - 4):
            b = data[off - 1] if off else 0
            v = struct.unpack_from("<I", data, off)[0]
            here = sva + off
            kind = None
            if b in (0xE8, 0xE9):
                tgt = (here + 4 + v) & 0xFFFFFFFF
                if tgt in refs:
                    kind = "call" if b == 0xE8 else "jmp"
                    v = tgt
            if kind is None and v in refs:
                kind = "imm32"
            if kind:
                i = bisect.bisect_right(keys, here) - 1
                caller = rows[i][1] if i >= 0 else "?"
                obj = rows[i][2] if i >= 0 else "?"
                refs[v].append((kind, f"0x{here - (1 if kind != 'imm32' else 0):08x}", caller, obj))
    return va2, refs, missing, rows


def main_static(out, tsv):
    va2, refs, missing, _ = static_scan(out)
    with open(tsv, "w") as f:
        f.write("rva\tkind\texe_symbol\texe_va\tref_kind\tsite\tcaller\tcaller_obj\n")
        for va, (n, rva, kind) in sorted(va2.items(), key=lambda x: (x[1][1], x[1][2])):
            rs = refs[va] or [("none", "", "", "")]
            for r in rs:
                f.write(f"{rva}\t{kind}\t{n}\t0x{va:08x}\t" + "\t".join(r) + "\n")
        for n in missing:
            f.write(f"\tnot-linked\t{n}\t\t\t\t\t\n")
    print(f"[reach] static: {len(va2)} linked targets, {len(missing)} not linked -> {tsv}")


SCRIPT = r"""
var pts = %s;
var pref = ptr(%s);
var base = Process.enumerateModules()[0].base;     // the exe may load away from its preferred base
var t0 = Date.now();
Process.setExceptionHandler(function (d) {
  send({exc: d.type, at: d.address.sub(base).add(pref).toString()});
  return false;
});
pts.forEach(function (p) {
  var l = Interceptor.attach(ptr(p).sub(pref).add(base), { onEnter: function () {
    if (l) { l.detach(); l = null; send({va: p, ms: Date.now() - t0}); }
  }});
});
send({ready: pts.length, base: base.toString()});
"""


def main_run(out, root, tmo, tsv, kvs):
    import frida
    va2, refs, missing, rows = static_scan(out)
    byva = {r[0]: r[1] for r in rows}
    pts = {va: f"{n} ({kind} {rva})" for va, (n, rva, kind) in va2.items()}
    for va, rs in refs.items():
        for kind, site, caller, obj in rs:
            if kind in ("call", "jmp"):
                cva = next((r[0] for r in rows if r[1] == caller), None)
                if cva and cva not in pts:
                    pts[cva] = f"{caller} (caller)"
    env = dict(os.environ, MASHED_ROOT=root)
    for kv in kvs:
        k, v = kv.split("=", 1)
        env[k] = v
    exe = str(out / "mashed_re_ddk.exe")
    log = pathlib.Path(root) / "mashed_re.log"
    if log.exists():
        log.unlink()
    dev = frida.get_local_device()
    pid = dev.spawn([exe], cwd=root, env=env)
    print("spawned pid", pid, flush=True)
    hits = {}
    ready = threading.Event()
    died = threading.Event()

    def on_msg(msg, _d):
        if msg.get("type") != "send":
            print(msg, flush=True)
            return
        p = msg["payload"]
        if "ready" in p:
            print("[reach] module base", p["base"], "points", p["ready"], flush=True)
            ready.set()
        elif "exc" in p:
            print("[reach] EXCEPTION", p["exc"], "at (map va)", p["at"], flush=True)
        else:
            hits[int(p["va"], 16)] = p["ms"]

    sess = dev.attach(pid)
    sess.on("detached", lambda reason, crash: (print("detached:", reason, crash, flush=True), died.set()))
    import pefile
    pref = pefile.PE(exe, fast_load=True).OPTIONAL_HEADER.ImageBase
    sc = sess.create_script(SCRIPT % (json.dumps([f"0x{v:08x}" for v in sorted(pts)]), f'"0x{pref:x}"'))
    sc.on("message", on_msg)
    sc.load()
    ready.wait(30)
    dev.resume(pid)
    t0 = time.time()
    died.wait(tmo)
    el = time.time() - t0
    try:
        dev.kill(pid)
    except Exception:
        pass
    with open(tsv, "w") as f:
        f.write("exe_va\tsymbol\treached\tfirst_ms\n")
        for va in sorted(pts):
            f.write(f"0x{va:08x}\t{pts[va]}\t{'yes' if va in hits else 'no'}\t{hits.get(va, '')}\n")
    nb = sum(1 for va in va2 if va in hits)
    print(f"[reach] run {el:.1f}s {'process ended' if died.is_set() else 'timeout kill'}: "
          f"{len(hits)}/{len(pts)} points hit, bodies+adapters {nb}/{len(va2)} -> {tsv}", flush=True)
    if log.exists():
        tail = log.read_text(errors="replace").splitlines()[-3:]
        print("log tail:", *tail, sep="\n  ")


if __name__ == "__main__":
    a = sys.argv
    if a[1] == "static":
        main_static(pathlib.Path(a[2]), a[3])
    else:
        main_run(pathlib.Path(a[2]), a[3], float(a[4]), a[5], a[6:])
