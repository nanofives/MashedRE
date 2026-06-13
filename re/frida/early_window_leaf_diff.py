#!/usr/bin/env py -3.12
"""early_window_leaf_diff.py — diff a PURE-LEAF hook without booting MASHED to menu.

WHY THIS EXISTS
---------------
The normal lane (`run_diff.py`) spawns MASHED and waits for the engine dispatch
LUT to ready before calling functions. That requires MASHED to survive D3D9
device init and reach (near) the menu. When the host display is wedged (monitors
asleep / topology change -> CreateDevice AVs 0xC0000005 at ~3 s, see the
"No-display D3D9 wedge" memory note), the LUT root never populates and run_diff
times out — blocking ALL diffs.

But a PURE LEAF — a function whose output depends only on its arguments plus the
single global the harness explicitly seeds — does not need engine state at all.
This tool diffs such leaves in the pre-crash window:

  1. subprocess-spawn MASHED with MASHED_RE_NO_AUTO_HOOK=1 (so the .asi, when
     present, does NOT install inline-JMP hooks -> the original stays original).
  2. frida.attach to the running (pre-crash) process.
  3. LoadLibraryW the .asi ourselves (loader lock is free in a RUNNING process —
     a SUSPENDED spawn deadlocks here) and GetProcAddress the reimpl export.
  4. Assert the original's first byte != 0xE9 (guards against a false-GREEN where
     a hook patched original -> reimpl).
  5. Seed + call original (fixed RVA) and reimpl (export), compare, per arg_type.
  6. Kill before WinMain's D3D9 init ever runs -> the crash never happens.

This is logically identical to run_diff's path1 (call orig + reimpl directly,
hook bypassed) for state-independent leaves; only the attach moment differs,
which cannot change a pure leaf's behavior.

VALIDATION (2026-06-13, recorded in PROMOTION_LOOP_LEDGER.md):
  - positive: reproduces GREEN for global_67f19c_get (round 31) and
    set_77196c_1 (round 29) — matches the trusted menu-attach verdicts.
  - negative: cross-wiring orig=global_67f19c_get vs reimpl=Global67f1a0Get
    yields RED on every case -> the harness has real discriminating power.

SCOPE / LIMITS
--------------
ONLY valid for arg_types whose evidence is state-INDEPENDENT (the harness fully
controls the inputs): read_global (uint32 OR float ret), void_setter_observe,
scalars_to_scattered_globals, and int_scalar leaves that are pure functions of
their argument (no absolute-table read of live state, no pointer-deref of the
arg). State-DEPENDENT functions (scenario:'race', live arrays/tables) are NOT
promotable this way — they still need run_diff against a booted game. Refuses
any other arg_type.

Exit 0 = GREEN (promotable), 1 = RED / harness error.
"""
import frida, os, sys, time, subprocess

# this file lives at <ROOT>/re/frida/ — go up three levels to the repo root
ROOT = os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
sys.path.insert(0, os.path.join(ROOT, "re", "frida"))
import hooks_registry as HR

EXE = os.path.join(ROOT, "original", "MASHED.exe")
ASI = os.path.join(ROOT, "original", "mashed_re_dev.asi")
LOG = os.path.join(ROOT, "log")

PURE_LEAF_ARGTYPES = {
    'read_global', 'void_setter_observe', 'scalars_to_scattered_globals', 'int_scalar',
    'int2_scalar',  # pure function of TWO int args (no memory) — tests are [p1,p2] pairs
    'deref_field_write',  # fn(ptr p1, u32 p2): *(*(p1+outer_off)+inner_off)=p2 — harness allocs+links buffers
    'deref_table_read',   # fn(ptr p1, u32 i): return (*p1)[i] — harness allocs+seeds an array behind p1
    'const_return',       # fn(): return <fixed constant> — no input, no state; call + compare
    'global_field_read',  # fn(): return *(*(global)+field_off) — point global at a seeded buffer
    'float_table_read',   # fn(i): return *(float*)(base+i*stride) — seed_table bits read as float
}

SRC = r"""
rpc.exports.diff = function(cfg) {
  const LL  = new NativeFunction(Module.getGlobalExportByName('LoadLibraryW'),  'pointer', ['pointer']);
  const GPA = new NativeFunction(Module.getGlobalExportByName('GetProcAddress'),'pointer', ['pointer','pointer']);
  const hm = LL(Memory.allocUtf16String(cfg.asi));
  if (hm.isNull()) return { error: 'LoadLibraryW failed' };
  const reim = GPA(hm, Memory.allocAnsiString(cfg.export));
  if (reim.isNull()) return { error: 'GetProcAddress failed for ' + cfg.export };
  const b0 = ptr(cfg.rva).readU8();
  if (b0 === 0xE9) return { error: 'ORIGINAL PATCHED (b0=0xE9) — NO_AUTO_HOOK failed; aborting' };
  const nargs = (cfg.at === 'int2_scalar') ? ['uint32','uint32']
              : (cfg.at === 'deref_field_write') ? ['pointer','uint32']
              : (cfg.at === 'deref_table_read')  ? ['pointer','uint32']
              : (cfg.at === 'void_setter_observe' || cfg.at === 'int_scalar' || cfg.at === 'float_table_read') ? ['uint32'] : [];
  const _keep = [];
  const Orig = new NativeFunction(ptr(cfg.rva), cfg.ret, nargs, 'mscdecl');
  const Reim = new NativeFunction(reim,         cfg.ret, nargs, 'mscdecl');
  const norm = function (v) { return (cfg.ret === 'float') ? v : (v >>> 0); };
  const res = [];
  for (let i = 0; i < cfg.tests.length; i++) {
    const t = cfg.tests[i]; let o = null, r = null, eo = null, er = null;
    if (cfg.at === 'read_global') {
      try { ptr(cfg.tgt).writeU32(t >>> 0); o = norm(Orig()); } catch (e) { eo = e.message; }
      try { ptr(cfg.tgt).writeU32(t >>> 0); r = norm(Reim()); } catch (e) { er = e.message; }
    } else if (cfg.at === 'scalars_to_scattered_globals') {
      // Observe ALL written globals: fill each with a sentinel, call, read each back,
      // join into one comparison string (covers multi-store setters fully).
      const obs = cfg.observe;
      const readAll = function () { return obs.map(function (x) { return ptr(x.addr).readU32() >>> 0; }).join('|'); };
      const fillAll = function () { obs.forEach(function (x) { ptr(x.addr).writeU32(0xFFFFFFFF); }); };
      try { fillAll(); Orig(); o = readAll(); } catch (e) { eo = e.message; }
      try { fillAll(); Reim(); r = readAll(); } catch (e) { er = e.message; }
    } else if (cfg.at === 'void_setter_observe') {
      try { Orig(t >>> 0); o = ptr(cfg.tgt).readU32() >>> 0; } catch (e) { eo = e.message; }
      try { Reim(t >>> 0); r = ptr(cfg.tgt).readU32() >>> 0; } catch (e) { er = e.message; }
    } else if (cfg.at === 'int_scalar') {
      // Optional table-seed: write distinct values into an absolute table the
      // function indexes, so a state-zero table is diffed NON-degenerately and a
      // wrong stride/base in the reimpl is caught. Seed once on the first test.
      if (cfg.seed_table && i === 0) {
        const base = ptr(cfg.seed_table.base), st = cfg.seed_table.stride | 0;
        const span = cfg.seed_table.span | 0;
        for (let k = 0; k < span; k++) base.add(k * st).writeU32((0xC0DE0000 | k) >>> 0);
      }
      try { o = Orig(t >>> 0) >>> 0; } catch (e) { eo = e.message; }
      try { r = Reim(t >>> 0) >>> 0; } catch (e) { er = e.message; }
    } else if (cfg.at === 'int2_scalar') {
      try { o = Orig(t[0] >>> 0, t[1] >>> 0) >>> 0; } catch (e) { eo = e.message; }
      try { r = Reim(t[0] >>> 0, t[1] >>> 0) >>> 0; } catch (e) { er = e.message; }
    } else if (cfg.at === 'deref_field_write') {
      // *(*(p1+outer_off)+inner_off) = p2. Fresh A+inner buffers per side; check inner[inner_off].
      const oo = cfg.outer_off | 0, io = cfg.inner_off | 0;
      const A1 = Memory.alloc(0x80), I1 = Memory.alloc(0x80); _keep.push(A1, I1);
      for (let z = 0; z < 0x80; z += 4) { A1.add(z).writeU32(0); I1.add(z).writeU32(0); }
      A1.add(oo).writePointer(I1);
      try { Orig(A1, t >>> 0); o = I1.add(io).readU32() >>> 0; } catch (e) { eo = e.message; }
      const A2 = Memory.alloc(0x80), I2 = Memory.alloc(0x80); _keep.push(A2, I2);
      for (let z = 0; z < 0x80; z += 4) { A2.add(z).writeU32(0); I2.add(z).writeU32(0); }
      A2.add(oo).writePointer(I2);
      try { Reim(A2, t >>> 0); r = I2.add(io).readU32() >>> 0; } catch (e) { er = e.message; }
    } else if (cfg.at === 'const_return') {
      try { o = Orig() >>> 0; } catch (e) { eo = e.message; }
      try { r = Reim() >>> 0; } catch (e) { er = e.message; }
    } else if (cfg.at === 'float_table_read') {
      // return *(float*)(base+i*stride). Seed the table bits (read as float, distinct
      // -> non-degenerate). ret is float so DO NOT coerce with >>> 0.
      const st = cfg.seed_table;
      if (i === 0 && st) { const b = ptr(st.base); for (let k = 0; k < (st.span | 0); k++) b.add(k * (st.stride | 0)).writeU32((0xC0DE0000 | k) >>> 0); }
      try { o = Orig(t >>> 0); } catch (e) { eo = e.message; }
      try { r = Reim(t >>> 0); } catch (e) { er = e.message; }
    } else if (cfg.at === 'global_field_read') {
      // return *(*(tgt)+field_off). Point the global at a seeded buffer; the test
      // value lands at +field_off (distinct per test -> non-degenerate).
      const fo = cfg.field_off | 0;
      const buf = Memory.alloc(0x100); _keep.push(buf);
      for (let z = 0; z < 0x100; z += 4) buf.add(z).writeU32(0);
      buf.add(fo).writeU32(t >>> 0);
      ptr(cfg.tgt).writePointer(buf);
      try { o = Orig() >>> 0; } catch (e) { eo = e.message; }
      try { ptr(cfg.tgt).writePointer(buf); buf.add(fo).writeU32(t >>> 0); r = Reim() >>> 0; } catch (e) { er = e.message; }
    } else if (cfg.at === 'deref_table_read') {
      // return (*p1)[i]. Seed an array behind p1 with distinct values; non-degenerate.
      const span = (cfg.span | 0) || 16;
      const arr = Memory.alloc(span * 4), A = Memory.alloc(4); _keep.push(arr, A);
      for (let k = 0; k < span; k++) arr.add(k * 4).writeU32((0xC0DE0000 | k) >>> 0);
      A.writePointer(arr);
      try { o = Orig(A, t >>> 0) >>> 0; } catch (e) { eo = e.message; }
      try { r = Reim(A, t >>> 0) >>> 0; } catch (e) { er = e.message; }
    }
    res.push({ i: i, t: '' + t, o: (o === null ? null : '' + o), r: (r === null ? null : '' + r),
               match: (eo === null && er === null && o !== null && o === r), eo: eo, er: er });
  }
  return { b0: '0x' + b0.toString(16), reim: reim.toString(), results: res };
};
"""


def run(name):
    h = HR.HOOKS[name]
    at = h['arg_type']
    if at not in PURE_LEAF_ARGTYPES:
        print(f"REFUSED: arg_type {at!r} is not a state-independent pure-leaf type "
              f"({sorted(PURE_LEAF_ARGTYPES)}). Use run_diff.py against a booted game.")
        return None
    cfg = {'rva': h['rva'], 'export': h['export'], 'ret': h['signature']['ret'], 'at': at,
           'tgt': h.get('target_global'), 'tests': h.get('path1_tests', []),
           'observe': h.get('observe'), 'seed_table': h.get('seed_table'),
           'outer_off': h.get('outer_off'), 'inner_off': h.get('inner_off'),
           'span': h.get('span'), 'field_off': h.get('field_off'), 'asi': ASI}
    p = subprocess.Popen([EXE], cwd=os.path.join(ROOT, 'original'),
                         env={**os.environ, 'MASHED_RE_NO_AUTO_HOOK': '1'})
    session = None
    for _ in range(50):
        try:
            session = frida.attach(p.pid); break
        except Exception:
            time.sleep(0.03)
    if not session:
        print("  attach failed"); p.kill(); return None
    out = None
    try:
        sc = session.create_script(SRC); sc.load()
        out = sc.exports_sync.diff(cfg)
    except Exception as e:
        print("  script error:", e)
    finally:
        try: p.kill()
        except Exception: pass
    if not out:
        return None
    if out.get('error'):
        print("  HARNESS ERROR:", out['error']); return None
    rs = out['results']
    mism = [x for x in rs if not x['match']]
    print(f"  b0={out['b0']} reim={out['reim']} cases={len(rs)} mismatches={len(mism)}")
    # write evidence CSV (early-window lane)
    os.makedirs(LOG, exist_ok=True)
    csv = os.path.join(LOG, f"diff_{name}.csv")
    with open(csv, 'w') as f:
        f.write("idx,input,original,reimpl,match\n")
        for x in rs:
            f.write(f"{x['i']},{x['t']},{x['o']},{x['r']},{x['match']}\n")
    print(f"  evidence: {csv}")
    for m in mism[:4]:
        print("   MISM", m)
    return len(mism) == 0 and len(rs) > 0


if __name__ == '__main__':
    if len(sys.argv) < 2:
        print("usage: early_window_leaf_diff.py <hook_name>  (PURE LEAVES ONLY)")
        sys.exit(2)
    n = sys.argv[1]
    print(f"hook: {n}  (early-window pure-leaf diff — no menu boot required)")
    ok = run(n)
    print("VERDICT:", "GREEN" if ok else ("RED" if ok is False else "ERROR/REFUSED"))
    sys.exit(0 if ok else 1)
