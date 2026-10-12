// Heap / import stand-ins + main-thread freeze for the Mashed A/B harness (plan item #3, 2026-10-11).
// Ported from SimGolf re/frida/js/{heapstandin,importstandin,freeze}.js. Prepended to diff_template.js and
// verify_hook_install_template.js by run_diff.py / run_verify_hook.py, so both agents share one copy.
//
// WHY. A function that allocates, frees or calls a Win32 import cannot be A/B'd as is: the two arms get different
// blocks, a free in the first arm releases a block the second arm still reads, and an import acts on the real system
// with fixture values (CloseHandle on a handle number someone else owns, DeleteCriticalSection on fixture memory).
// While ARMED, these stand-ins replace, FOR THE A/B THREAD ONLY:
//   heap   - the CRT allocator (_malloc 0x004a45fb, __nh_malloc 0x004a45cf, _calloc 0x004a467e, _free 0x004a460d)
//            and the RwGlobals memory slots (*(0x007d3ff8) + 0x108 rwmalloc(size,hint), +0x10c rwfree(p),
//            +0x110 rwrealloc(p,size,hint), +0x114 rwcalloc(n,size,hint), +0x118 RwFreeListAlloc(fl,hint),
//            +0x11c RwFreeListFree(fl,p); offsets re/analysis/rwglobals_layout_20260911.md, arities checked against
//            every call site's `add esp` in the binary 2026-10-11: 8/4/0xc/0xc/8/8).
//            Allocations are CARVED from one arena that is reset at every arm, so both arms receive the SAME
//            addresses. Frees are RECORDED (entry tag + pointer), never performed.
//   import - IAT slots of MASHED.exe and of the loaded .asi: each listed import returns the fixture's value and the
//            call (import, recorded arguments) is appended to the arm's log.
// The log of each arm (alloc sizes, frees with their entry tag, imports with their arguments, arena digest) is part
// of the compared observation, so a port that frees through the wrong allocator, skips a free, frees twice, or
// makes an extra / missing / mis-argued import call reads RED even when its return value is right.
//
// HOW. Every patched entry jumps to a small native stub:
//     mov eax, fs:[0x24]        ; current thread id
//     cmp eax, <A/B tid>   / jne fwd
//     cmp dword [armed], 0 / je  fwd
//     jmp <NativeCallback>      ; A/B thread while armed: JS carves / records
//   fwd:
//     lock inc dword [counter]
//     jmp dword [prev]          ; real allocator / API (trampoline for CRT entries, old slot value otherwise)
// so the game's own threads never enter JS or take Frida's JS lock (the hot-path rule in CLAUDE.md: they pay four
// instructions), and the A/B thread's fixture allocations made while DISARMED reach the real allocator.
// CRT entries are patched with Interceptor.replaceFast (it returns the trampoline the stub forwards to); RwGlobals
// slots and IAT slots are data, so the dword is swapped. The RW slots are read through *(0x007d3ff8) by both the
// original and the .asi ports, so both arms see the stand-in. The .asi calls the CRT by absolute address
// (Boot/CrtEnvArgv.cpp:21/27), so it sees the CRT stand-in too; an .asi body that used its OWN CRT would bypass it,
// and the free log then reads RED (the missing-free negative control).
//
// API (one install per diff run; arm/disarm around every arm call):
//   const S = __standins.install({ heap: {...}|true, imports: {...}, asi: <Module>, regions: fn })
//   S.arm();  ...call one arm...;  const obs = S.disarm();      // obs: normalized string
//   S.uninstall();
// heap opts: crt (default true), rw (default true), arenaSize (0x10000), fill (0xcd), failMask (bit k fails alloc
//   call k), failSize (a request of exactly this size fails), exhaust ('throw'|'null'), observeArena (true).
// imports: { 'KERNEL32.dll!CloseHandle': {ret: 1}, 'WINMM.dll!timeEndPeriod': {ret: 0, args: 1}, ... }
//   spec: args (stack args; known APIs in ARGS), abi ('stdcall'), ret (number or array by call number),
//         record ('all' | N leading args | [indices] | false), forward (call the real API, still recorded),
//         deref ({argIdx: 'a' | 'w'}: log the string at that arg instead of its address).
//   S.arm({impRet: {api: v | [..]}}) overrides `ret` for that arm (objectCall: per-test t.import_ret).
// regions: optional function returning [[name, base(NativePointer), size], ...]; obs values inside a region are
//   written as name+off (the arena is always 'A'), so pointer identity is compared, not raw addresses.
(function () {
  if (globalThis.__standins) return;
  const keep = globalThis.__standinKeep = globalThis.__standinKeep || [];
  function keepAlloc(n) { const p = Memory.alloc(n); keep.push(p); return p; }

  const CRT_ALLOC = [[0x004a45fb, '_malloc', 1, 1], [0x004a45cf, '__nh_malloc', 2, 1], [0x004a467e, '_calloc', 2, 2]];
  const CRT_FREE = [[0x004a460d, '_free']];
  const RWG_PTR = ptr('0x007d3ff8');
  const ARGS = {
    CloseHandle: 1, EnterCriticalSection: 1, LeaveCriticalSection: 1, DeleteCriticalSection: 1,
    InitializeCriticalSection: 1, InterlockedIncrement: 1, InterlockedDecrement: 1, FreeLibrary: 1,
    LoadLibraryA: 1, GetProcAddress: 2, CreateEventA: 4, SetEvent: 1, ResetEvent: 1, WaitForSingleObject: 2,
    CreateFileA: 7, ReadFile: 5, WriteFile: 5, SetFilePointer: 4, GetFileSize: 2, FindFirstFileA: 2, FindClose: 1,
    GetLastError: 0, Sleep: 1, GetTickCount: 0, QueryPerformanceCounter: 1, QueryPerformanceFrequency: 1,
    OutputDebugStringA: 1, HeapAlloc: 3, HeapFree: 3, VirtualAlloc: 4, VirtualFree: 3,
    timeBeginPeriod: 1, timeEndPeriod: 1, timeGetTime: 0,
    GetSystemMetrics: 1, GetAsyncKeyState: 1, GetKeyState: 1, ShowCursor: 1, SetCursorPos: 2,
  };

  // Native gate stub (see header). `prevSlot` holds the forward target, set after the patch.
  //   00 64 a1 24 00 00 00     mov eax, fs:[0x24]
  //   06 3d <tid>              cmp eax, tid
  //   0b 75 0e                 jne 0x1b
  //   0d 83 3d <armed> 00      cmp dword ptr [armed], 0
  //   14 74 05                 je  0x1b
  //   16 e9 <rel cb>           jmp cb
  //   1b f0 ff 05 <counter>    lock inc dword ptr [counter]
  //   22 ff 25 <prevSlot>      jmp dword ptr [prevSlot]
  function u32(v) { return [v & 0xff, (v >>> 8) & 0xff, (v >>> 16) & 0xff, (v >>> 24) & 0xff]; }
  function makeStub(tid, armed, cb, prevSlot, counter) {
    const stub = keepAlloc(Process.pageSize);
    const bytes = [0x64, 0xa1, 0x24, 0x00, 0x00, 0x00]
      .concat([0x3d], u32(tid >>> 0))
      .concat([0x75, 0x0e])
      .concat([0x83, 0x3d], u32(armed.toUInt32()), [0x00])
      .concat([0x74, 0x05])
      .concat([0xe9], u32(cb.sub(stub.add(0x1b)).toUInt32()))
      .concat([0xf0, 0xff, 0x05], u32(counter.toUInt32()))
      .concat([0xff, 0x25], u32(prevSlot.toUInt32()));
    Memory.patchCode(stub, bytes.length, function (code) { code.writeByteArray(bytes); });
    const back = new Uint8Array(stub.readByteArray(bytes.length));
    if (back.some(function (b, i) { return b !== bytes[i]; })) throw new Error('standins: stub write failed');
    return stub;
  }

  // The IAT slot of dll!name in the module at `base`, read from its own import descriptors (SimGolf found
  // Module.enumerateImports() returning the wrong slot on an unwrapped exe; this reads the tables directly).
  function findSlot(base, dll, name) {
    const nt = base.add(base.add(0x3c).readU32());
    const impRva = nt.add(0x80).readU32();
    if (!impRva) return null;
    for (let d = base.add(impRva); ; d = d.add(20)) {
      const oft = d.readU32(), nameRva = d.add(12).readU32(), ft = d.add(16).readU32();
      if (!nameRva && !ft) break;
      if (base.add(nameRva).readCString().toLowerCase() !== dll.toLowerCase()) continue;
      const names = oft || ft;
      for (let i = 0; ; i++) {
        const e = base.add(names + 4 * i).readU32();
        if (!e) break;
        if (e & 0x80000000) continue;
        if (base.add(e + 2).readCString() === name) return base.add(ft + 4 * i);
      }
    }
    return null;
  }

  function writeSlot(slot, value) {
    const range = Process.findRangeByAddress(slot);
    const prot = range ? range.protection : 'rw-';
    if (prot.indexOf('w') < 0) Memory.protect(slot, 4, 'rw-');
    slot.writePointer(value);
    if (prot.indexOf('w') < 0) Memory.protect(slot, 4, prot);
  }

  function hex(v) { return '0x' + (v >>> 0).toString(16); }

  // NUL-terminated byte ('a') or UTF-16 ('w') string, at most 256 units, as a"..." / w"...". Bytes outside 0x20..0x7e
  // and the log's own separators are escaped \xNN, so two different strings never print alike.
  function impStr(p, wide) {
    let s = '';
    try {
      for (let z = 0; z < 256; z++) {
        const c = wide ? p.add(2 * z).readU16() : p.add(z).readU8();
        if (c === 0) return (wide ? 'w"' : 'a"') + s + '"';
        s += (c >= 0x20 && c < 0x7f && c !== 0x22 && c !== 0x5c && c !== 0x3a && c !== 0x2c && c !== 0x20)
          ? String.fromCharCode(c) : '\\x' + ('000' + c.toString(16)).slice(wide ? -4 : -2);
      }
      return (wide ? 'w"' : 'a"') + s + '"+';
    } catch (e) { return (wide ? 'w' : 'a') + '@AV'; }
  }

  function install(opts) {
    opts = opts || {};
    const tid = Process.getCurrentThreadId();
    const armed = keepAlloc(4); armed.writeU32(0);
    const undo = [];               // newest last; uninstall runs them newest first
    const counters = [];           // [label, NativePointer] forwarded-call counters (other threads)
    let log = [];                  // current arm's log entries (strings before normalization: arrays)
    const sizes = new Map();
    const hcfg = opts.heap === true ? {} : (opts.heap || null);
    let arena = null, arenaSize = 0, fill = 0xcd, cursor = 0, allocCalls = 0;

    function gate(cb, label) {
      const prevSlot = keepAlloc(4);
      const counter = keepAlloc(4); counter.writeU32(0);
      counters.push([label, counter]);
      keep.push(cb);
      return { stub: makeStub(tid, armed, cb, prevSlot, counter), prevSlot: prevSlot };
    }
    function patchCode(addr, cb, label) {      // CRT entry: replaceFast, forward to the trampoline
      const g = gate(cb, label);
      const tramp = Interceptor.replaceFast(addr, g.stub);
      g.prevSlot.writePointer(tramp);
      undo.push(function () { Interceptor.revert(addr); });
    }
    function patchSlot(slot, cb, label) {      // data slot (RwGlobals / IAT): swap the dword
      const g = gate(cb, label);
      const prev = slot.readPointer();
      g.prevSlot.writePointer(prev);
      writeSlot(slot, g.stub);
      undo.push(function () { writeSlot(slot, prev); });
      return prev;
    }

    let armFail = {};
    function carve(size, byte, tag) {
      const k = allocCalls++;
      const failMask = ((armFail.failMask !== undefined ? armFail.failMask : hcfg.failMask) || 0) >>> 0;
      const failSize = armFail.failSize !== undefined ? armFail.failSize : hcfg.failSize;
      if ((k < 32 && ((failMask >>> k) & 1)) || (failSize !== undefined && size === (failSize >>> 0))) {
        log.push(['alloc', tag, size, 'FAIL']);
        return ptr(0);
      }
      const need = ((size + 15) & ~15) >>> 0;
      if (size > arenaSize || cursor + need > arenaSize) {
        log.push(['alloc', tag, size, 'EXHAUSTED']);
        if (hcfg.exhaust === 'null') return ptr(0);
        throw new Error('standins: arena of ' + arenaSize + ' bytes exhausted');
      }
      const p = arena.add(cursor);
      if (size) p.writeByteArray(new Array(size).fill(byte));
      cursor += need;
      sizes.set(p.toString(), size);
      log.push(['alloc', tag, size, p]);
      return p;
    }

    if (hcfg) {
      arenaSize = hcfg.arenaSize || 0x10000;
      fill = hcfg.fill === undefined ? 0xcd : hcfg.fill;
      arena = keepAlloc(arenaSize);
      if (hcfg.crt !== false) {
        CRT_ALLOC.forEach(function (e) {
          const nargs = e[2], nsize = e[3], tag = e[1];
          const argt = Array(nargs).fill('uint32');
          patchCode(ptr(e[0]), new NativeCallback(function () {
            let size = arguments[0] >>> 0;
            for (let i = 1; i < nsize; i++) size = (size * (arguments[i] >>> 0)) >>> 0;
            return carve(size, tag === '_calloc' ? 0 : fill, tag);
          }, 'pointer', argt, 'mscdecl'), tag);
        });
        CRT_FREE.forEach(function (e) {
          const tag = e[1];
          patchCode(ptr(e[0]), new NativeCallback(function (p) {
            log.push(['free', tag, p]);
          }, 'void', ['pointer'], 'mscdecl'), tag);
        });
      }
      if (hcfg.rw !== false) {
        const rwg = RWG_PTR.readPointer();
        if (rwg.isNull()) throw new Error('standins: RwGlobals (*0x007d3ff8) is NULL');
        patchSlot(rwg.add(0x108), new NativeCallback(function (size, hint) {
          return carve(size >>> 0, fill, 'rwmalloc');
        }, 'pointer', ['uint32', 'uint32'], 'mscdecl'), 'rwmalloc');
        patchSlot(rwg.add(0x10c), new NativeCallback(function (p) {
          log.push(['free', 'rwfree', p]);
        }, 'void', ['pointer'], 'mscdecl'), 'rwfree');
        patchSlot(rwg.add(0x110), new NativeCallback(function (old, n, hint) {
          n = n >>> 0;
          if (old.isNull()) return carve(n, fill, 'rwrealloc');
          const key = old.toString();
          if (!sizes.has(key)) throw new Error('standins: rwrealloc of a block the arena did not carve ' + key);
          const p = carve(n, fill, 'rwrealloc');
          if (p.isNull()) return p;
          const copy = Math.min(sizes.get(key), n);
          if (copy) p.writeByteArray(old.readByteArray(copy));
          log.push(['free', 'rwrealloc', old]);
          return p;
        }, 'pointer', ['pointer', 'uint32', 'uint32'], 'mscdecl'), 'rwrealloc');
        patchSlot(rwg.add(0x114), new NativeCallback(function (num, size, hint) {
          return carve(((num >>> 0) * (size >>> 0)) >>> 0, 0, 'rwcalloc');
        }, 'pointer', ['uint32', 'uint32', 'uint32'], 'mscdecl'), 'rwcalloc');
        // RwFreeListAlloc(fl, hint): the entry size is fl->entrySize at +0 (non-RWDEBUG RwFreeList, rwplcore.h)
        patchSlot(rwg.add(0x118), new NativeCallback(function (fl, hint) {
          log.push(['freelist', fl]);
          return carve(fl.readU32(), fill, 'fl_alloc');
        }, 'pointer', ['pointer', 'uint32'], 'mscdecl'), 'fl_alloc');
        patchSlot(rwg.add(0x11c), new NativeCallback(function (fl, p) {
          log.push(['free', 'fl_free', p, fl]);
          return fl;
        }, 'pointer', ['pointer', 'pointer'], 'mscdecl'), 'fl_free');
      }
    }

    const imports = opts.imports || {};
    const names = Object.keys(imports);
    const impCalls = names.map(function () { return 0; });
    if (names.length) {
      const bases = [['exe', Process.getModuleByName('MASHED.exe').base]];
      if (opts.asi) bases.push(['asi', opts.asi.base]);
      names.forEach(function (full, idx) {
        const spec = imports[full] || {};
        const bang = full.indexOf('!');
        const dll = full.slice(0, bang), api = full.slice(bang + 1);
        const nargs = spec.args !== undefined ? spec.args : ARGS[api];
        if (nargs === undefined) throw new Error('standins: give `args` for ' + full);
        const abi = spec.abi || 'stdcall';
        const which = spec.record === undefined ? 'all' : spec.record;
        let found = 0;
        bases.forEach(function (mb) {
          const slot = findSlot(mb[1], dll, api);
          if (!slot) return;
          found++;
          const argt = Array(nargs).fill('uint32');
          let orig = null;
          const deref = spec.deref || {};
          const cb = new NativeCallback(function () {
            const a = Array.prototype.slice.call(arguments);
            const n = impCalls[idx]++;
            let ix;
            if (which === false) ix = [];
            else if (which === 'all') ix = a.map(function (v, i) { return i; });
            else if (typeof which === 'number') ix = a.slice(0, which).map(function (v, i) { return i; });
            else ix = which;
            // deref {argIdx: 'a' | 'w'} (bregargs 2026-10-12): log the string at that arg, not its address. A buffer on
            // the caller's stack has a different address on each arm (0x004987b0 formats into one and passes it to
            // OutputDebugStringA), its text does not.
            log.push(['imp', api].concat(ix.map(function (i) {
              return deref[i] ? impStr(ptr(a[i] >>> 0), deref[i] === 'w') : ptr(a[i] >>> 0);
            })));
            // armFail.impRet {api: v | [v_call0, ..]} (bregargs 2026-10-12): this arm's return, from the test, over
            // the install-time `ret` (0x00495530 branches on the DirectInput8Create HRESULT's sign).
            const rs = (armFail.impRet && armFail.impRet[api] !== undefined) ? armFail.impRet[api] : spec.ret;
            let r;
            if (spec.forward) r = orig.apply(null, a);
            else if (Array.isArray(rs)) r = rs[Math.min(n, rs.length - 1)];
            else r = rs === undefined ? 0 : rs;
            return (r >>> 0);
          }, 'uint32', argt, abi);
          const prev = patchSlot(slot, cb, mb[0] + ':' + api);
          orig = new NativeFunction(prev, 'uint32', argt, abi);
          keep.push(orig);
        });
        if (!found) throw new Error('standins: neither MASHED.exe nor the .asi imports ' + full);
      });
    }

    function norm(v) {
      const p = ptr(v);
      if (arena && p.compare(arena) >= 0 && p.compare(arena.add(arenaSize)) < 0) return 'A+' + hex(p.sub(arena).toUInt32());
      const regs = opts.regions ? opts.regions() : [];
      for (let i = 0; i < regs.length; i++) {
        const b = ptr(regs[i][1]);
        if (p.compare(b) >= 0 && p.compare(b.add(regs[i][2])) < 0) return regs[i][0] + '+' + hex(p.sub(b).toUInt32());
      }
      return hex(p.toUInt32());
    }
    function fnv(p, n) {
      let h = 0x811c9dc5;
      const b = new Uint8Array(p.readByteArray(n));
      for (let i = 0; i < b.length; i++) { h ^= b[i]; h = Math.imul(h, 0x01000193) >>> 0; }
      return h >>> 0;
    }

    Interceptor.flush();
    return {
      norm: norm,
      // fail: optional per-arm {failMask, failSize} overriding the install-time schedule
      arm: function (fail) {
        armFail = fail || {};
        log = [];
        cursor = 0;
        allocCalls = 0;
        sizes.clear();
        for (let i = 0; i < impCalls.length; i++) impCalls[i] = 0;
        // 0xa5, not 0: carved blocks are filled with `fill` (0xcd) only over the requested size, so the slack past a
        // block reads 0xa5 and a body that writes past its block (a zero fill one dword too long) changes the digest.
        if (arena) arena.writeByteArray(new Array(arenaSize).fill(0xa5));
        armed.writeU32(1);
      },
      // Returns this arm's observation: every log entry (pointers normalized) + the arena digest.
      disarm: function () {
        armed.writeU32(0);
        const parts = log.map(function (e) {
          return e.map(function (x) {
            return (x instanceof NativePointer) ? norm(x) : (typeof x === 'number' ? hex(x) : String(x));
          }).join(':');
        });
        if (arena && hcfg.observeArena !== false) parts.push('arena=' + hex(cursor) + '/' + hex(fnv(arena, Math.max(cursor, 1))));
        // observeArena: 'full' -- every carved dword, normalized (diagnosis: which object field differs)
        if (arena && hcfg.observeArena === 'full') {
          const w = [];
          for (let o = 0; o + 4 <= cursor; o += 4) w.push(norm(arena.add(o).readU32()));
          parts.push('arenafull=' + w.join(','));
        }
        return parts.join(' ');
      },
      forwarded: function () {
        const o = {};
        counters.forEach(function (c) { o[c[0]] = c[1].readU32(); });
        return o;
      },
      uninstall: function () {
        armed.writeU32(0);
        for (let i = undo.length - 1; i >= 0; i--) { try { undo[i](); } catch (e) { /* best effort */ } }
        undo.length = 0;
        Interceptor.flush();
      },
    };
  }

  // Frozen-main-thread helper (SimGolf freeze.js, round-22 method): suspend the process's first thread at a safe
  // point (eip inside MASHED.exe's r-x ranges) so a fixture can write live globals that thread reads. The A/B code in
  // this harness runs synchronously in one JS turn, so callers flush explicitly at the end (no setTimeout).
  // Allocate fixtures BEFORE freezing: the safe point can be inside the CRT (also MASHED.exe code) holding its lock.
  const freeze = (function () {
    const k32 = Process.getModuleByName('kernel32.dll');
    const nf = function (name, ret, args) {
      return new NativeFunction(k32.getExportByName(name), ret, args, { abi: 'stdcall', scheduling: 'exclusive' });
    };
    let api = null;
    function A() {
      if (!api) api = {
        openThread: nf('OpenThread', 'pointer', ['uint32', 'int', 'uint32']),
        suspendThread: nf('SuspendThread', 'uint32', ['pointer']),
        resumeThread: nf('ResumeThread', 'uint32', ['pointer']),
        getThreadContext: nf('GetThreadContext', 'int', ['pointer', 'pointer']),
        closeHandle: nf('CloseHandle', 'int', ['pointer']),
        sleep: new NativeFunction(k32.getExportByName('Sleep'), 'void', ['uint32'], 'stdcall'),
        ctx: keepAlloc(0x2d0),
      };
      return api;
    }
    const pend = { undo: [], held: [] };
    const status = { freezes: 0, flushes: 0, lastTries: 0, lastEip: 0 };
    function flush() {
      const undo = pend.undo.splice(0);
      for (let i = undo.length - 1; i >= 0; i--) undo[i]();
      pend.held.splice(0).forEach(function (rel) { rel(); });
      status.flushes++;
    }
    function thread(o) {
      o = o || {};
      const a = A();
      const ranges = [];
      (o.modules || ['MASHED.exe']).forEach(function (m) {
        Process.getModuleByName(m).enumerateRanges('r-x').forEach(function (r) { ranges.push([r.base, r.base.add(r.size)]); });
      });
      const threads = Process.enumerateThreads();
      const tid = threads.length ? threads[0].id : 0;
      if (!tid || tid === Process.getCurrentThreadId()) throw new Error('freeze: no first thread to suspend');
      const h = a.openThread(0x0002 | 0x0008, 0, tid);
      if (h.isNull()) throw new Error('freeze: OpenThread failed');
      const tries = o.tries || 10000;
      for (let t = 1; t <= tries; t++) {
        const prev = a.suspendThread(h);
        if (prev === 0xffffffff) { a.closeHandle(h); throw new Error('freeze: SuspendThread failed'); }
        if (prev !== 0) { a.resumeThread(h); a.closeHandle(h); throw new Error('freeze: thread already suspended'); }
        a.ctx.writeU32(0x00010001);                                   // CONTEXT_CONTROL
        const eip = a.getThreadContext(h, a.ctx) ? a.ctx.add(0xb8).readU32() : 0;
        if (eip && ranges.some(function (r) { const p = ptr(eip); return p.compare(r[0]) >= 0 && p.compare(r[1]) < 0; })) {
          let done = false;
          const release = function () { if (done) return; done = true; a.resumeThread(h); a.closeHandle(h); };
          pend.held.push(release);
          status.freezes++; status.lastTries = t; status.lastEip = eip;
          return release;
        }
        a.resumeThread(h);
        a.sleep(1);
      }
      a.closeHandle(h);
      throw new Error('freeze: first thread never stopped inside the safe-point modules');
    }
    function save(p, n) { const b = p.readByteArray(n); pend.undo.push(function () { p.writeByteArray(b); }); }
    return { thread: thread, flush: flush, save: save,
             setU32: function (p, v) { save(p, 4); p.writeU32(v); }, status: status };
  })();

  // ── arg_type 'standin_object_call' (shared by diff_template.js and verify_hook_install_template.js) ──
  // One fixture OBJECT, allocated from the GAME's heap (_malloc 0x004a45fb, called while disarmed) unless
  // cfg.object_alloc === 'frida', is reused by every arm: it is re-seeded before each arm, so both arms see the same
  // `this` and the same field pointers (SimGolf's same-address model, no pointer normalization needed for identity).
  // Per test t:
  //   t.blocks  [size, ...]           extra game-heap blocks B0, B1, ... (allocated once per test, while disarmed)
  //   t.fill    byte (default 0)      object fill before the fields are written
  //   t.fields  [[off, value], ...]   u32 stores into the object; value = number | 'OBJ[+off]' | 'B<i>[+off]'
  //   t.args    [value, ...]          call arguments, same value syntax ('OBJ' first for a thiscall `this`)
  // Observation per arm: ret=<normalized> (unless void), obj=<dwords that differ from the seed, normalized>, and the
  // stand-in log (allocs, frees with entry tag, imports with arguments, arena digest).
  //   t.fields entries may also be [target, off, value] with target 'OBJ' | 'B<i>' (stores into a block)
  //   t.failMask / t.failSize         per-test allocation failure schedule (overrides heap_standin's)
  //   t.import_ret {api: v | [..]}    per-test import return (overrides the import_standin spec's `ret`)
  // cfg keys: object_size (0x100), object_alloc ('crt'|'frida'), heap_standin, import_standin, freeze_main,
  //           reg_args {reg: argIndex} + callee_pops (register-delivered arguments, see regThunk),
  //           signature.ret, observe_ret_len (dump that many bytes at the returned pointer when non-NULL),
  //           observe_globals [{addr, len, seed, deref, off}]: saved before each arm, filled with per-dword sentinels 0x5eed0000+i
  //           (or t.globals [[addr, value]] stores), observed after the arm (normalized dwords) and RESTORED right
  //           after it (seed: false skips the sentinel fill, for live engine blocks such as RwGlobals) -- a live game global holds a fixture or arena pointer only for the length of one arm (use
  //           freeze_main when the game's first thread reads it).
  //           fns: [[side, NativeFunction], ...].  Returns per-test arrays of {side: obs, err}.
  // Register-argument thunk (cfg.reg_args = {eax: 0, esi: 2, ...}: register <- logical argument index). The thunk is
  // cdecl over ALL logical arguments: it saves ebx/esi/edi/ebp, loads each named register from its argument, pushes
  // the remaining arguments right to left, calls `target`, drops them (unless cfg.callee_pops), restores and returns.
  // Both arms go through a thunk built the same way, so register delivery is identical on both sides.
  //   push ebx/esi/edi/ebp          53 56 57 55            (+0x10)
  //   mov r32, [esp+0x14+4*i]       8b <modrm> 24 <d8>
  //   push dword [esp+d]            ff 74 24 <d8>          d = 0x14 + 4*j + 4*k for argument j as the k-th push
  //   call target                   e8 <rel32>
  //   add esp, 4*k                  83 c4 <k*4>
  //   pop ebp/edi/esi/ebx; ret      5d 5f 5e 5b c3
  const REGCODE = { eax: 0x44, ecx: 0x4c, edx: 0x54, ebx: 0x5c, ebp: 0x6c, esi: 0x74, edi: 0x7c };
  function regThunk(target, cfg) {
    const nargs = cfg.signature.args.length;
    const regs = cfg.reg_args || {};
    const inReg = {};
    Object.keys(regs).forEach(function (r) {
      if (!(r in REGCODE)) throw new Error('standins: reg_args register ' + r + ' not supported');
      inReg[regs[r]] = r;
    });
    const stackIdx = [];
    for (let i = 0; i < nargs; i++) if (!(i in inReg)) stackIdx.push(i);
    const code = keepAlloc(Process.pageSize);
    let b = [0x53, 0x56, 0x57, 0x55];
    Object.keys(regs).forEach(function (r) { b = b.concat([0x8b, REGCODE[r], 0x24, 0x14 + 4 * regs[r]]); });
    for (let k = 0; k < stackIdx.length; k++) {
      const j = stackIdx[stackIdx.length - 1 - k];
      const d = 0x14 + 4 * j + 4 * k;
      if (d > 0x7f) throw new Error('standins: reg thunk displacement too large');
      b = b.concat([0xff, 0x74, 0x24, d]);
    }
    const callAt = b.length;
    b = b.concat([0xe8, 0, 0, 0, 0]);
    const drop = cfg.callee_pops ? 0 : 4 * stackIdx.length;
    if (drop) b = b.concat([0x83, 0xc4, drop]);
    b = b.concat([0x5d, 0x5f, 0x5e, 0x5b, 0xc3]);
    const rel = u32(ptr(target).sub(code.add(callAt + 5)).toUInt32());
    for (let i = 0; i < 4; i++) b[callAt + 1 + i] = rel[i];
    Memory.patchCode(code, b.length, function (c) { c.writeByteArray(b); });
    return new NativeFunction(code, cfg.signature.ret, cfg.signature.args, 'mscdecl');
  }

  function objectCall(fns, cfg, asi) {
    const size = cfg.object_size || 0x100;
    // reg_args: route each arm through a register thunk (NativeFunction is a NativePointer: ptr(fn) is its target)
    if (cfg.reg_args) fns = fns.map(function (sf) { return [sf[0], regThunk(ptr(sf[1]), cfg)]; });
    const gameMalloc = new NativeFunction(ptr('0x004a45fb'), 'pointer', ['uint32'], 'mscdecl');
    const galloc = function (n) {
      const p = cfg.object_alloc === 'frida' ? keepAlloc(n) : gameMalloc(n);
      if (p.isNull()) throw new Error('standin_object_call: allocation of ' + n + ' failed');
      return p;
    };
    const obj = galloc(size);
    const blocks = cfg.tests.map(function (t) { return (t.blocks || []).map(function (n) { return [galloc(n), n]; }); });
    let cur = [];
    const regions = function () {
      return [['OBJ', obj, size]].concat(cur.map(function (b, i) { return ['B' + i, b[0], b[1]]; }));
    };
    const val = function (v) {
      if (typeof v === 'number') return ptr(v >>> 0);
      const m = /^(OBJ|B(\d+))(?:\+(0x[0-9a-fA-F]+|\d+))?$/.exec(String(v));
      if (!m) throw new Error('standin_object_call: bad value ' + v);
      const base = m[1] === 'OBJ' ? obj : cur[parseInt(m[2], 10)][0];
      return base.add(m[3] ? parseInt(m[3]) : 0);
    };
    let release = null;
    if (cfg.freeze_main) release = freeze.thread({ modules: ['MASHED.exe'] });
    const S = install({ heap: cfg.heap_standin, imports: cfg.import_standin, asi: asi, regions: regions });
    const out = [];
    try {
      cfg.tests.forEach(function (t, ti) {
        cur = blocks[ti];
        const seed = function () {
          obj.writeByteArray(new Array(size).fill(t.fill === undefined ? 0 : t.fill));
          cur.forEach(function (b) { b[0].writeByteArray(new Array(b[1]).fill(0xbb)); });
          (t.fields || []).forEach(function (f) {
            if (f.length === 3) val(f[0]).add(f[1]).writePointer(val(f[2]));
            else obj.add(f[0]).writePointer(val(f[1]));
          });
          return new Uint8Array(obj.readByteArray(size));
        };
        // per-block seed snapshot (taken after seed()), diffed after each arm like the object
        const blockSeed = function () { return cur.map(function (b) { return new Uint8Array(b[0].readByteArray(b[1])); }); };
        const dwordDiff = function (before, base, n) {
          const after = new Uint8Array(base.readByteArray(n)), out = [];
          for (let o = 0; o + 4 <= n; o += 4) {
            const a = before[o] | (before[o + 1] << 8) | (before[o + 2] << 16) | (before[o + 3] << 24);
            const c = after[o] | (after[o + 1] << 8) | (after[o + 2] << 16) | (after[o + 3] << 24);
            if (a !== c) out.push('+' + hex(o) + '=' + S.norm(c >>> 0));
          }
          return out;
        };
        // deref: true -> the window starts at *(addr), read when the test starts (RwGlobals lives on the heap at run
        // time: *(0x007d3ff8) is not the static block 0x007d3ec8, which keeps stale pointers)
        const globs = (cfg.observe_globals || []).map(function (g) {
          return [(g.deref ? ptr(g.addr).readPointer() : ptr(g.addr)).add(g.off || 0), g.len || 4, g.seed !== false];
        });
        const row = {};
        fns.forEach(function (sf) {
          const side = sf[0], fn = sf[1];
          const before = seed();
          const bBefore = blockSeed();
          const args = (t.args || []).map(function (v, i) {
            const p = val(v);
            return (cfg.signature.args[i] === 'pointer') ? p : p.toUInt32();
          });
          let ret = null, err = null;
          const saved = globs.map(function (g) { return g[1] ? g[0].readByteArray(g[1]) : null; });
          globs.forEach(function (g) {
            if (!g[2]) return;                       // seed: false -- observe + restore only (live engine state)
            for (let o = 0; o + 4 <= g[1]; o += 4) g[0].add(o).writeU32((0x5eed0000 + o / 4) >>> 0);
          });
          (t.globals || []).forEach(function (gv) { ptr(gv[0]).writePointer(val(gv[1])); });
          S.arm({ failMask: t.failMask, failSize: t.failSize, impRet: t.import_ret });
          try { ret = fn.apply(null, args); } catch (e) { err = e.message; }
          const log = S.disarm();
          const gobs = globs.map(function (g) {
            const w = [];
            for (let o = 0; o + 4 <= g[1]; o += 4) w.push(S.norm(g[0].add(o).readU32()));
            return hex(g[0].toUInt32()) + ':' + w.join(',');
          });
          globs.forEach(function (g, i) { g[0].writeByteArray(saved[i]); });
          let rmem = '';
          if (cfg.observe_ret_len && ret !== null && !ptr(ret).isNull()) {
            // A wrong returned pointer must read RED, not abort the fixture (negctl ret mutant, 2026-10-11).
            try {
              const rp = ptr(ret), w = [];
              for (let o = 0; o + 4 <= cfg.observe_ret_len; o += 4) w.push(S.norm(rp.add(o).readU32()));
              rmem = ' retmem=[' + w.join(',') + ']';
            } catch (e) { rmem = ' retmem=AV'; }
          }
          const after = new Uint8Array(obj.readByteArray(size));
          const changed = [];
          for (let o = 0; o + 4 <= size; o += 4) {
            const a = before[o] | (before[o + 1] << 8) | (before[o + 2] << 16) | (before[o + 3] << 24);
            const b = after[o] | (after[o + 1] << 8) | (after[o + 2] << 16) | (after[o + 3] << 24);
            if (a !== b) changed.push('+' + hex(o) + '=' + S.norm(b >>> 0));
          }
          const r = (cfg.signature && cfg.signature.ret === 'void') ? '' :
                    'ret=' + (ret === null ? 'null' : S.norm(typeof ret === 'number' ? ret : ptr(ret).toUInt32())) + ' ';
          const bobs = cur.map(function (b, i) {
            const d = dwordDiff(bBefore[i], b[0], b[1]);
            return d.length ? ' B' + i + '=[' + d.join(',') + ']' : '';
          }).join('');
          row[side] = { obs: r + 'obj=[' + changed.join(',') + ']' + bobs + rmem +
                               (gobs.length ? ' globals=[' + gobs.join(' ') + ']' : '') + ' log=[' + log + ']', err: err };
        });
        out.push(row);
      });
    } finally {
      S.uninstall();
      if (release) freeze.flush();
    }
    return { rows: out, forwarded: S.forwarded() };
  }

  globalThis.__standins = { install: install, objectCall: objectCall, ARGS: ARGS, regThunk: regThunk };
  globalThis.__freeze = freeze;
})();
