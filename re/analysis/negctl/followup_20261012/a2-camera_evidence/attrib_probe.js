// a2-camera attribution probe for 0x004464c0 (prepended with re/frida/js/standins.js for __freeze).
// Variant dispatchers assembled from the original bytes; each perturbs ONE thing the .asi port differs in.
const ASI = '%ASI%';
const COUNT = ptr('0x898994'), BASE = ptr('0x8964c0'), WLEN = 0x00898998 - 0x008964c0, ARG = 0x00897fe0;
const CALLEES = [ptr('0x445aa0'), ptr('0x441d40'), ptr('0x442440')];
const PORT_EAX = [null, null, 0x442440];  // null -> count (port: mov eax,[0x898994] / mov eax,[ebp])
const ebpSlot = Memory.alloc(8);
function le32(v) { return [v & 0xff, (v >>> 8) & 0xff, (v >>> 16) & 0xff, (v >>> 24) & 0xff]; }

function build(opt) {
  // opt: {frame:bool, pad:[k..], regs:[k..]}
  const code = Memory.alloc(Process.pageSize);
  Memory.protect(code, Process.pageSize, 'rwx');
  (function (c) {
    const w = new X86Writer(c, { pc: code });
    w.putBytes([0xa1, 0x94, 0x89, 0x89, 0x00]);           // 004464c0 mov eax,[0x898994]
    w.putPushReg('ebx');                                  // 004464c5
    w.putBytes([0x33, 0xdb]);                             // 004464c6 xor ebx,ebx
    w.putBytes([0x85, 0xc0]);                             // 004464c8 test eax,eax
    w.putJccNearLabel('jle', 'end', 'no-hint');           // 004464ca
    w.putPushReg('esi'); w.putPushReg('edi');             // 004464cc/cd
    if (opt.frame) w.putPushU32(0xfeedf00d);
    w.putBytes([0x8b, 0x7c, 0x24, opt.frame ? 0x14 : 0x10]); // 004464ce mov edi,[esp+0x10]
    w.putBytes([0xbe, 0xc0, 0x64, 0x89, 0x00]);           // 004464d2 mov esi,0x8964c0
    w.putLabel('loop');
    for (let k = 0; k < 3; k++) {
      if (k === 0) { w.putBytes([0x8b, 0x46, 0x04, 0x85, 0xc0]); w.putJccNearLabel('jne', 'n0', 'no-hint'); }
      else { w.putBytes([0x83, 0x7e, 0x04, k]); w.putJccNearLabel('jne', 'n' + k, 'no-hint'); }
      const pad = opt.pad && opt.pad.indexOf(k) >= 0, regs = opt.regs && opt.regs.indexOf(k) >= 0;
      if (pad) w.putPushU32(0xfeedf00d);
      if (regs) {
        w.putBytes([0x89, 0x2d].concat(le32(ebpSlot.toUInt32())));              // mov [slot],ebp
        if (PORT_EAX[k] === null) w.putBytes([0xa1, 0x94, 0x89, 0x89, 0x00]); else w.putMovRegU32('eax', PORT_EAX[k]);
        w.putMovRegU32('ecx', 0x445aa0); w.putMovRegU32('edx', 0x441d40); w.putMovRegU32('ebp', 0x898994);
      }
      w.putPushReg('edi'); w.putPushReg('esi');
      w.putCallAddress(CALLEES[k]);
      w.putAddRegImm('esp', pad ? 12 : 8);
      if (regs) { w.putBytes([0x8b, 0x2d].concat(le32(ebpSlot.toUInt32()))); }  // mov ebp,[slot]
      w.putLabel('n' + k);
    }
    w.putBytes([0xa1, 0x94, 0x89, 0x89, 0x00]);           // 00446508 mov eax,[0x898994]
    w.putIncReg('ebx');
    w.putAddRegImm('esi', 0xd8);
    w.putBytes([0x3b, 0xd8]);                             // cmp ebx,eax
    w.putJccNearLabel('jl', 'loop', 'no-hint');
    if (opt.frame) w.putAddRegImm('esp', 4);
    w.putPopReg('edi'); w.putPopReg('esi');
    w.putLabel('end');
    w.putPopReg('ebx');
    w.putRet();
    w.flush();
  })(code);
  const nf = new NativeFunction(code, 'void', ['uint32']); nf.code = code; return nf;
}

function snap() { return new Uint8Array(BASE.readByteArray(WLEN)); }
function put(b) { BASE.writeByteArray(b.buffer); }
function diff(a, b) {
  const out = [];
  for (let i = 0; i < WLEN; i += 4) {
    let d = false; for (let j = 0; j < 4; j++) if (a[i + j] !== b[i + j]) d = true;
    if (d) out.push('e' + Math.floor(i / 0xd8) + '+0x' + (i % 0xd8).toString(16));
  }
  return out;
}

rpc.exports = {
  run: function () {
    const res = {};
    const orig = new NativeFunction(ptr('0x4464c0'), 'void', ['uint32']);
    let asiFn = null;
    try { const m = Module.load(ASI); asiFn = new NativeFunction(m.getExportByName('CameraEntryDispatchAll'), 'uint32', ['uint32']); }
    catch (e) { res.asi_err = e.message; }
    const V = {
      copy: build({}), frame: build({ frame: true }), regs_all: build({ regs: [0, 1, 2] }),
      pad0: build({ pad: [0] }), pad1: build({ pad: [1] }), pad2: build({ pad: [2] }),
      regs0: build({ regs: [0] }), regs1: build({ regs: [1] }), regs2: build({ regs: [2] }),
    };
    if (asiFn) V.asi_port = asiFn;
    __freeze.thread({ modules: ['MASHED.exe'] });
    try {
      const n = COUNT.readS32();
      res.count = n; res.entries = [];
      for (let i = 0; i < Math.min(n, 64); i++) {
        const e = BASE.add(i * 0xd8);
        res.entries.push([i, e.add(4).readU32(), e.add(8).readU32()]);
      }
      const start = snap();
      function arm(fn) { put(start); orig(ARG); fn(ARG); const s = snap(); put(start); return s; }
      let ref, ref2;
      try { ref = arm(orig); ref2 = arm(orig); } catch (e) { res.orig_err = e.message; return JSON.stringify(res); }
      res.orig_vs_orig = diff(ref, ref2);
      for (const k of Object.keys(V)) {
        try { const a = arm(V[k]), b = arm(V[k]);
          res[k] = { vs_orig: diff(ref, a), repeat: diff(a, b) }; }
        catch (e) { put(start); res[k] = { err: e.message, code: V[k].code ? V[k].code.toString() : null,
          bytes: V[k].code ? Array.from(new Uint8Array(V[k].code.readByteArray(160))).map(function (x) { return ('0' + x.toString(16)).slice(-2); }).join('') : null }; }
      }
      put(start);
    } finally { __freeze.flush(); }
    return JSON.stringify(res);
  }
};
