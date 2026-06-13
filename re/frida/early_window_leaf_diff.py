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
    'eax_implicit_void',  # void fn() with `this` in EAX — trampoline sets EAX=buf, check observed fields
    'pool_insert_snapshot',  # fn(mgr_ptr, key): pool/list insert — reset+call+full-state snapshot diff
    'pool_remove_snapshot',  # fn(mgr_ptr, key): pool/list remove — build list via orig insert, then remove, snapshot
    'table_clear',           # void fn(i): zero an absolute table slot [target_global + i*4]
    'ptr_fields_clear',      # void fn(ptr): zero fields of a struct arg; check observe offsets
    'stack_pop_snapshot',    # fn(stk): array-stack pop {top,cap,buf}; reset+call+snapshot+ret
    'stack_push_snapshot',   # fn(stk,val): array-stack push; reset+call+snapshot+ret
    'ptr_table_field_read',  # fn(i): return *(*(tgt))[i] + field_off  (pointer-to-table + field)
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
              : (cfg.at === 'pool_insert_snapshot') ? ['pointer','uint32']
              : (cfg.at === 'pool_remove_snapshot') ? ['pointer','uint32']
              : (cfg.at === 'table_clear') ? ['uint32']
              : (cfg.at === 'ptr_fields_clear') ? ['pointer']
              : (cfg.at === 'stack_pop_snapshot') ? ['pointer']
              : (cfg.at === 'stack_push_snapshot') ? ['pointer','uint32']
              : (cfg.at === 'ptr_table_field_read') ? ['uint32']
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
    } else if (cfg.at === 'pool_insert_snapshot') {
      // Pool/list insert into a manager. Same buffers for both sides so absolute
      // link pointers are directly comparable; reset to a fresh state, call with
      // the test key, snapshot manager+slots+pool, compare. Non-degenerate: the
      // inserted node stores `key` (distinct per test).
      const N = (cfg.capacity | 0) || 4;
      const mgr = Memory.alloc(0x18), slots = Memory.alloc(N * 4), pool = Memory.alloc(N * 0x10);
      _keep.push(mgr, slots, pool);
      const reset = function () {
        for (let z = 0; z < 0x18; z += 4) mgr.add(z).writeU32(0);
        mgr.writeU16(0); mgr.add(2).writeU16(N);
        mgr.add(0xc).writePointer(pool); mgr.add(0x10).writePointer(slots);
        for (let z = 0; z < N * 4; z += 4) slots.add(z).writeU32(0);
        for (let z = 0; z < N * 0x10; z += 4) pool.add(z).writeU32(0);
      };
      const snap = function () {
        const p = [];
        for (let z = 0; z < 0x18; z += 4) p.push(mgr.add(z).readU32() >>> 0);
        for (let z = 0; z < N * 4; z += 4) p.push(slots.add(z).readU32() >>> 0);
        for (let z = 0; z < N * 0x10; z += 4) p.push(pool.add(z).readU32() >>> 0);
        return p.join(',');
      };
      try { reset(); Orig(mgr, t >>> 0); o = snap(); } catch (e) { eo = e.message; }
      try { reset(); Reim(mgr, t >>> 0); r = snap(); } catch (e) { er = e.message; }
    } else if (cfg.at === 'pool_remove_snapshot') {
      // Build a list with the ORIGINAL insert (cfg.insert_rva), then remove the
      // test key; snapshot full state + return value. Same buffers both sides.
      const N = (cfg.capacity | 0) || 4;
      const mgr = Memory.alloc(0x18), slots = Memory.alloc(N * 4), pool = Memory.alloc(N * 0x10);
      _keep.push(mgr, slots, pool);
      const Insert = new NativeFunction(ptr(cfg.insert_rva), 'uint16', ['pointer', 'uint32'], 'mscdecl');
      const bks = cfg.build_keys;
      const build = function () {
        for (let z = 0; z < 0x18; z += 4) mgr.add(z).writeU32(0);
        mgr.writeU16(0); mgr.add(2).writeU16(N);
        mgr.add(0xc).writePointer(pool); mgr.add(0x10).writePointer(slots);
        for (let z = 0; z < N * 4; z += 4) slots.add(z).writeU32(0);
        for (let z = 0; z < N * 0x10; z += 4) pool.add(z).writeU32(0);
        for (let k = 0; k < bks.length; k++) Insert(mgr, bks[k] >>> 0);
      };
      const snap = function () {
        const p = [];
        for (let z = 0; z < 0x18; z += 4) p.push(mgr.add(z).readU32() >>> 0);
        for (let z = 0; z < N * 4; z += 4) p.push(slots.add(z).readU32() >>> 0);
        for (let z = 0; z < N * 0x10; z += 4) p.push(pool.add(z).readU32() >>> 0);
        return p.join(',');
      };
      try { build(); const ro = Orig(mgr, t >>> 0) >>> 0; o = snap() + '|ret=' + ro; } catch (e) { eo = e.message; }
      try { build(); const rr = Reim(mgr, t >>> 0) >>> 0; r = snap() + '|ret=' + rr; } catch (e) { er = e.message; }
    } else if (cfg.at === 'table_clear') {
      // void fn(i): zero [tgt + i*4]. Seed sentinel, call, read back.
      const base = ptr(cfg.tgt), idx = t >>> 0;
      try { base.add(idx * 4).writeU32(0xFFFFFFFF); Orig(idx); o = base.add(idx * 4).readU32() >>> 0; } catch (e) { eo = e.message; }
      try { base.add(idx * 4).writeU32(0xFFFFFFFF); Reim(idx); r = base.add(idx * 4).readU32() >>> 0; } catch (e) { er = e.message; }
    } else if (cfg.at === 'ptr_fields_clear') {
      // void fn(ptr): zero struct fields. Fill buffer with sentinel, call, check observe offsets.
      const obs = cfg.observe;
      const buf = Memory.alloc(0x100); _keep.push(buf);
      const fill = function () { for (let z = 0; z < 0x100; z += 4) buf.add(z).writeU32(0xFFFFFFFF); };
      const rd = function () { return obs.map(function (x) { return buf.add(x.off | 0).readU32() >>> 0; }).join('|'); };
      try { fill(); Orig(buf); o = rd(); } catch (e) { eo = e.message; }
      try { fill(); Reim(buf); r = rd(); } catch (e) { er = e.message; }
    } else if (cfg.at === 'ptr_table_field_read') {
      // return *(*(tgt)[i] + field_off). Seed *(tgt)=tableBuf, tableBuf[i]=&entry,
      // entry[field_off]=distinct value -> non-degenerate.
      const N = (cfg.capacity | 0) || 8, fo = cfg.field_off | 0, idx = t >>> 0;
      const tableBuf = Memory.alloc(N * 4), entry = Memory.alloc(0x40); _keep.push(tableBuf, entry);
      const sv = (0xC0DE0000 | (idx & 0xffff)) >>> 0;
      const setup = function () {
        for (let z = 0; z < 0x40; z += 4) entry.add(z).writeU32(0);
        entry.add(fo).writeU32(sv);
        tableBuf.add(idx * 4).writePointer(entry);
        ptr(cfg.tgt).writePointer(tableBuf);
      };
      try { setup(); o = Orig(idx) >>> 0; } catch (e) { eo = e.message; }
      try { setup(); r = Reim(idx) >>> 0; } catch (e) { er = e.message; }
    } else if (cfg.at === 'stack_pop_snapshot') {
      // array-stack {top@0, cap@4, buf@8}. test value t = initial top (exercises edge).
      const N = (cfg.capacity | 0) || 4;
      const st = Memory.alloc(0xc), buf = Memory.alloc(N * 4); _keep.push(st, buf);
      const ib = cfg.init_buf, top0 = (t | 0);
      const reset = function () { st.writeS32(top0); st.add(4).writeU32(N); st.add(8).writePointer(buf); for (let k = 0; k < N; k++) buf.add(k * 4).writeU32(ib[k] >>> 0); };
      const snap = function () { const p = [st.readS32()]; for (let k = 0; k < N; k++) p.push(buf.add(k * 4).readU32() >>> 0); return p.join(','); };
      try { reset(); const ro = Orig(st) >>> 0; o = snap() + '|ret=' + ro; } catch (e) { eo = e.message; }
      try { reset(); const rr = Reim(st) >>> 0; r = snap() + '|ret=' + rr; } catch (e) { er = e.message; }
    } else if (cfg.at === 'stack_push_snapshot') {
      // array-stack push. test value t = value pushed (distinct -> non-degenerate).
      const N = (cfg.capacity | 0) || 4;
      const st = Memory.alloc(0xc), buf = Memory.alloc(N * 4); _keep.push(st, buf);
      const top0 = (cfg.init_top | 0);
      const reset = function () { st.writeS32(top0); st.add(4).writeU32(N); st.add(8).writePointer(buf); for (let k = 0; k < N; k++) buf.add(k * 4).writeU32(0); };
      const snap = function () { const p = [st.readS32()]; for (let k = 0; k < N; k++) p.push(buf.add(k * 4).readU32() >>> 0); return p.join(','); };
      try { reset(); const ro = Orig(st, t >>> 0) >>> 0; o = snap() + '|ret=' + ro; } catch (e) { eo = e.message; }
      try { reset(); const rr = Reim(st, t >>> 0) >>> 0; r = snap() + '|ret=' + rr; } catch (e) { er = e.message; }
    } else if (cfg.at === 'eax_implicit_void') {
      // The function uses EAX as an implicit `this`. Build a tiny trampoline
      // `mov eax, buf ; jmp target` (B8 imm32 / E9 rel32), call it (no args), and
      // check the observed buffer fields. Fill with a sentinel first so a wrong
      // reimpl that leaves any field unwritten -> RED.
      const obs = cfg.observe;
      const ebuf = Memory.alloc(0x100); _keep.push(ebuf);
      const mkTramp = function (target) {
        const tr = Memory.alloc(Process.pageSize); _keep.push(tr);
        tr.writeU8(0xB8); tr.add(1).writePointer(ebuf); tr.add(5).writeU8(0xE9);
        tr.add(6).writeS32(target.sub(tr.add(10)).toInt32());
        Memory.protect(tr, 16, 'rwx');
        return new NativeFunction(tr, 'void', [], 'mscdecl');
      };
      const fillAll = function () { for (let z = 0; z < 0x100; z += 4) ebuf.add(z).writeU32(0xFFFFFFFF); };
      const readObs = function () { return obs.map(function (x) { return ebuf.add(x.off | 0).readU32() >>> 0; }).join('|'); };
      try { fillAll(); mkTramp(ptr(cfg.rva))(); o = readObs(); } catch (e) { eo = e.message; }
      try { fillAll(); mkTramp(reim)(); r = readObs(); } catch (e) { er = e.message; }
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
           'span': h.get('span'), 'field_off': h.get('field_off'),
           'capacity': h.get('capacity'), 'insert_rva': h.get('insert_rva'),
           'build_keys': h.get('build_keys'), 'init_buf': h.get('init_buf'),
           'init_top': h.get('init_top'), 'asi': ASI}
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
