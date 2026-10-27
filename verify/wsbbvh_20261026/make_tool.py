"""Generate c4_racediff_bvh.py, a worktree-safe copy of re/frida/canonical_c4_racediff.py.

Same pattern as verify/dupinstall2_20261016/make_c4_tools.py: the capture, the scenario driver,
the frame window, the sample budget and the verdict rule are unchanged. Each replacement is
asserted to apply exactly once.

Plumbing (as in dupinstall2):
  ORIG = $MASHED_ROOT/original (private root), LOG = $C4_OUT (per-RVA dir), NAV = repo copy,
  spawned PID printed.

Normalisation (new, needed for the RW-Physics band; pilot 0x0055fe50 read HOLD on artefacts only):
  1. Heap addresses differ between the OFF and the ON boot (pilot: 0x0055fe50 returns param_1,
     `mov eax, esi` at 0x0055fe99, and the port returns param_1, yet eax read f1eb0b8 vs fbfc340).
     A value is now encoded relative to the target's own entry state:
       S+<off>   stack at or above the entry ESP (the caller's frame, incl. the args)
       Sloc      stack below the entry ESP (the target's own locals; layout differs by build)
       A<k>+<off> mapped address within 0x4000 above entry stack arg k, k < the target's real
                 arity: the param count of the port's own __cdecl FUN_<rva>(...) declaration
                 (batch 1 used only the fallback below and got 0 for the init-sequence callees,
                 whose caller 0x0047e9c0 defers stack cleanup, so everything read h<n>); fallback
                 the caller's add esp,N / pop ecx after the return address, else ret N. Args
                 past the arity are caller stack junk (pilot: a heap value anchored on "arg 7" of
                 the 1-arg 0x0055fe50 read A7+c28 vs A7+11e8). Registers are not anchors.
       h<n>      any other mapped address, renamed by first appearance within that one call
       hex       anything not mapped (ints, floats), unchanged
  2. The port calls its own .asi copies of ported callees (0x0055fe50 -> FUN_0055a1f0 in
     RwpSolverBroadphase3.cpp). .asi addresses are emitted as a<off> and resolved through
     mashedmod/build/mashed_re_dev.map: a symbol carrying FUN_<rva> reads as c<rva>, the same
     token the original's call to that RVA produces. Any other .asi symbol reads a:<name>.
  3. The installed JMP lands on the wrapper, which may call FUN_<rva>_impl or a lambda. In the
     ON run every .asi code symbol whose name contains FUN_<target rva> is part of the target's
     range, and a call from the target into its own range is not recorded.
     Calls INTO the ShadowAB / ShadowTrack templates the wrappers route through (unarmed unless
     MASHED_SHADOW_AB is set: they only call _impl) and into the /GS __security_check_cookie are
     not recorded. Sites inside those templates are not recorded either, so a callee's wrapper
     never leaks into the target's record.
  4. Call args. The stock tool reads the `add esp,N` after a call inside the Stalker transform,
     where the parse fails, so it compared no call argument at all (C4_DEBUG_N=1 pilot: '#-1/0'
     on both sides of the 10-arg cdecl call at 0x0055fe91). The count is now re-read at callout
     time from the call site, the cap is 16 (stock: 8), and when the callee is a ported FUN_<rva>
     with one consistent declaration, that declared count is used on both sides. Needed because
     a deferred `add esp` reads 0 args on one side only (batch 1, 0x0055ff90 -> 0x00567c60: OFF
     '[c567c60]', ON '[c567c60,A0+c4,...]' with 10 args).
  5. A call from the target's own range INTO a ShadowAB Run/RunTracked/RunRegion template is a
     call to another port whose one-line wrapper the compiler inlined (0x00564c80_impl calls
     RunTracked<void(int,float*,ushort)> at .asi 0x100586f6 for FUN_005646c0). It is recorded as
     c<Counter.rva> with the params after the 2/3/4 leading template args, unless Counter.rva is
     the target itself (its own wrapper path).
  6. Return value by the port's declared return kind: void -> not compared ('eax=-'), a 16-bit
     type -> AX only, else EAX as above. Only valid where the original's callers do not read the
     rest of EAX; RESULT_D4-WSB-C4-BVH.md cites the caller scan for every row that relies on it.
     The image's stack probe 0x004a3440 and the .asi's CRT __chkstk read as the same callee.
     (v4 dropped the return kind when it rebuilt `targets` for the OFF boot, so OFF compared full
     EAX against ON's declared kind: batch 3 '005651b0 off eax=h0 / on eax=-'. Fixed in v5.)
"""
import pathlib

ROOT = pathlib.Path(__file__).resolve().parent.parent.parent
HERE = pathlib.Path(__file__).resolve().parent


def rep(t, old, new):
    assert t.count(old) == 1, (old, t.count(old))
    return t.replace(old, new)


r = (ROOT / "re/frida/canonical_c4_racediff.py").read_text(encoding="utf-8")

# ---- plumbing ----
r = rep(r, 'ORIG = ROOT / "original"; EXE = ORIG / "MASHED.exe"',
        'ORIG = Path(os.environ["MASHED_ROOT"]) / "original"; EXE = ORIG / "MASHED.exe"   # bvh copy: private root')
r = rep(r, 'LOG = (ROOT / "log") if (ROOT / "log").exists() else (ROOT.parent.parent / "log")',
        'LOG = Path(os.environ["C4_OUT"])   # bvh copy: per-RVA output dir')
r = rep(r, 'NAV = (Path(__file__).resolve().parent / "nav_agent.js").read_text(encoding="utf-8")',
        'NAV = (ROOT / "re" / "frida" / "nav_agent.js").read_text(encoding="utf-8")   # bvh copy: repo copy')
r = rep(r, "        proc = subprocess.Popen([str(EXE)], cwd=str(ORIG), env=env,",
        "        print(f\"  [{mode}] EXE={EXE}\", flush=True)\n"
        "        proc = subprocess.Popen([str(EXE)], cwd=str(ORIG), env=env,")
r = rep(r, "        try: sess = dev.attach(proc.pid); break",
        "        print(f\"  [{mode}] SPAWNED_PID={proc.pid}\", flush=True)\n"
        "        try: sess = dev.attach(proc.pid); break")

# ---- agent: value normaliser ----
r = rep(r, "  function nrm(v){ const r = rvaOf(v); if (inImg(r)) return 'i'+r.toString(16); return (v>>>0).toString(16); }",
        r"""  let ASI_LO=0, ASI_HI=0, ENTRY_ESP=0, ENTRY_ARGS=[], HMAP=null, HN=0;   // bvh copy
  const RANGES = {};
  const OBS_AR = {};
  let IMG_AR = {}, ASI_AR = {};   // declared arity of ported callees, by image RVA / by .asi wrapper offset
  const RETK = {};    // port's declared return: 'void' (not compared), '16' (AX), 'full' (EAX)
  const ARITY = {};   // from the port's own __cdecl FUN_<rva>(...) declaration (python side), -1 = unknown
  // fallback stack arity of the target: caller cleanup after the return address (add esp,N / pop ecx),
  // else the target's own ret N; same caller and same original entry on the OFF and the ON side
  function entryArity(esp, rva){ try{ const i=Instruction.parse(esp.readPointer());
      if(i.mnemonic==='add'&&i.operands.length===2&&i.operands[0].type==='reg'&&i.operands[0].value==='esp'&&i.operands[1].type==='imm') return Math.min(8,(Number(i.operands[1].value)>>>0)>>>2);
      if(i.mnemonic==='pop'&&i.operands.length===1&&i.operands[0].value==='ecx') return 1; }catch(e){}
    const s=stdcallNargs(abs(rva>>>0)); return (s>0&&s<=8)?s:0; }
  function isMapped(v){ try{ return Process.findRangeByAddress(ptr(v))!==null; }catch(e){ return false; } }
  let TRANSP = [];
  // -2 not transparent, -1 transparent (not recorded), k>=0 a ShadowAB Run template with k leading args
  function transpSkip(a){ for(let i=0;i<TRANSP.length;i++){ if(a>=TRANSP[i][0]&&a<TRANSP[i][1]) return TRANSP[i][2]; } return -2; }
  function inOwn(key, a){ const L=RANGES[key]; if(!L) return false; for(let i=0;i<L.length;i++){ if(a>=L[i][0]&&a<L[i][1]) return true; } return false; }
  function nrm(v){ v=v>>>0; const r = rvaOf(v); if (inImg(r)) return 'i'+r.toString(16);
    if (ASI_LO && v>=ASI_LO && v<ASI_HI) return 'a'+(v-ASI_LO).toString(16);
    if (v < 0x10000) return v.toString(16);
    if (ENTRY_ESP && v <= ENTRY_ESP + 0x2000 && v + 0x100000 >= ENTRY_ESP) {
      if (v >= ENTRY_ESP) return 'S+'+(v-ENTRY_ESP).toString(16); return 'Sloc'; }
    if (!isMapped(v)) return v.toString(16);
    for (let k=0;k<ENTRY_ARGS.length;k++){ const a=ENTRY_ARGS[k]>>>0;
      if (a>=0x10000 && v>=a && v-a<0x4000) return 'A'+k+'+'+(v-a).toString(16); }
    if (HMAP){ if (!(v in HMAP)) HMAP[v]='h'+(HN++); return HMAP[v]; }
    return 'h?';
  }""")

# ---- agent: sites and call targets use the per-target range list ----
r = rep(r, "  function siteInAnyRange(site){ for(const k in RANGE){ const r=RANGE[k]; if(site>=r[0]&&site<r[1]) return true; } return false; }",
        "  function siteInAnyRange(site){ for(const k in RANGES){ if(inOwn(k, site)) return true; } return false; }   // bvh copy")
r = rep(r, "      if (site<activeLo||site>=activeHi) return;",
        "      if (!inOwn(activeTarget, site)) return;   // bvh copy")
r = rep(r, "      const sp=context.esp; let parts=[ inImg(tr)?'c'+tr.toString(16):'x'+(tgt.toUInt32()>>>0).toString(16) ];",
        "      const tu=tgt.toUInt32()>>>0; if (inOwn(activeTarget, tu)) return;   // bvh copy: wrapper -> own impl\n"
        "      const tsk=transpSkip(tu); if (tsk===-1) return;   // bvh copy: /GS cookie, ShadowAB helpers\n"
        "      let TOK=null, TB=0;\n"
        "      if (tsk>=0){ let crva=0; try{ crva=context.esp.readPointer().add(4).readU32()>>>0; }catch(e){ return; }   // Counter.rva\n"
        "        if (crva===(activeTarget>>>0)) return;   // the target's own wrapper path\n"
        "        TOK='c'+crva.toString(16); TB=tsk; n=(IMG_AR[crva]!==undefined)?IMG_AR[crva]:Math.max(0,n-tsk); }   // inlined callee wrapper\n"
        "      const sp=context.esp; let parts=[ TOK?TOK:(inImg(tr)?'c'+tr.toString(16):((ASI_LO&&tu>=ASI_LO&&tu<ASI_HI)?'a'+(tu-ASI_LO).toString(16):'x'+tu.toString(16))) ];")
r = rep(r, "      for(let j=0;j<n;j++){ try{ parts.push(nrm(sp.add(j*4).readU32())); }catch(e){ parts.push('?'); } }",
        "      for(let j=0;j<n;j++){ try{ parts.push(nrm(sp.add((TB+j)*4).readU32())); }catch(e){ parts.push('?'); } }   // bvh copy: TB")
r = rep(r, "    FLO=flo; FHI=fhi;",
        "    FLO=flo; FHI=fhi;\n"
        "    try{ const am=Process.findModuleByName('mashed_re_dev.asi'); if(am){ ASI_LO=am.base.toUInt32(); ASI_HI=ASI_LO+am.size; } }catch(e){}   // bvh copy")
r = rep(r, "      RANGE[rva]=[lo,hi]; BUDGET[rva]=SAMPLES; CUR[rva]=null;",
        "      RANGE[rva]=[lo,hi]; BUDGET[rva]=SAMPLES; CUR[rva]=null;\n"
        "      RANGES[rva]=[[lo,hi]];   // bvh copy: + the wrapper/impl/lambda symbols (ON only)\n"
        "      ARITY[rva]=(targets[i].length>4)?targets[i][4]:-1;\n"
        "      RETK[rva]=(targets[i].length>7)?targets[i][7]:'full';\n"
        "      if(targets[i].length>5){ IMG_AR=targets[i][5]; ASI_AR=targets[i][6]; }\n"
        "      if(mode!=='off' && ASI_LO){ const X=targets[i][2]||[]; for(let j=0;j<X.length;j++) RANGES[rva].push([ASI_LO+X[j][0], ASI_LO+X[j][1]]);\n"
        "        const T=targets[i][3]||[]; TRANSP=[]; for(let j=0;j<T.length;j++) TRANSP.push([ASI_LO+T[j][0], ASI_LO+T[j][1], T[j][2]]); }\n"
        "      out[rva+'_ranges']=RANGES[rva].map(q=>[q[0].toString(16),q[1].toString(16)]);")
r = rep(r, "            activeTid=this.threadId; CUR[key]=[];",
        "            activeTid=this.threadId; CUR[key]=[];\n"
        "            ENTRY_ESP=this.context.esp.toUInt32(); ENTRY_ARGS=[]; HMAP={}; HN=0;   // bvh copy\n"
        "            const NA=(ARITY[key]>=0)?ARITY[key]:entryArity(this.context.esp, key); for(let j=1;j<=NA;j++){ try{ ENTRY_ARGS.push(this.context.esp.add(j*4).readU32()); }catch(e){ ENTRY_ARGS.push(0); } }")

# ---- agent: the return is compared by the port's declared return kind ----
r = rep(r, "            CUR[key].push('eax='+nrm(r.toUInt32()));",
        "            { const v=r.toUInt32()>>>0; const k=RETK[key];   // bvh copy\n"
        "              CUR[key].push(k==='void'?'eax=-':(k==='16'?'ax='+(v&0xffff).toString(16):'eax='+nrm(v))); }")

# ---- agent: compare up to 16 stack args per call (stock drops every arg of a call with > 8) ----
r = rep(r, "if(n<0||n>8) n=0;", "if(n<0||n>16) n=0;   /* bvh copy: was > 8 */\n"
        "      if (DBGN) CUR[activeTarget].push('#'+cdeclN+'/'+n);")
r = rep(r, "  function makeCallout(site, op, nextAddr){",
        "  const DBGN = %s;   // bvh copy: C4_DEBUG_N=1 records the arg count per call\n"
        "  function makeCallout(site, op, nextAddr){" % ("true" if __import__("os").environ.get("C4_DEBUG_N") else "false"))
# Number() instead of ptr() for the imm operands (equivalent: ptr(Int64) reads 40 for `add esp,0x28`
# at 0x0055fe96, probed 2026-10-26), kept for clarity.
r = rep(r, "cdeclN=ptr(o[1].value).toUInt32()>>>2;", "cdeclN=(Number(o[1].value)>>>0)>>>2;")
# The stock tool reads the `add esp, N` after the call INSIDE the Stalker transform, where the parse
# fails (C4_DEBUG_N=1 pilot: '#-1/0' on both sides for the 10-arg cdecl call at 0x0055fe91), so it
# never compared any call argument. Re-derive it at callout time from the call site itself (cached).
r = rep(r, "      let n=cdeclN; if(n<0) n=stdcallNargs(tgt);",
        "      if (cdeclN===-1 && !LZ){ LZ=true; try{ const ci=Instruction.parse(ptr(site)); const nx=Instruction.parse(ci.next); if(nx.mnemonic==='add'){ const o=nx.operands;\n"
        "        if(o.length===2&&o[0].type==='reg'&&o[0].value==='esp'&&o[1].type==='imm') cdeclN=(Number(o[1].value)>>>0)>>>2; } }catch(e){} }   // bvh copy\n"
        "      let n=cdeclN; if(n<0) n=stdcallNargs(tgt);\n"
        "      { const tk=tgt.toUInt32()>>>0; const kr=inImg(tr)?IMG_AR[tr]:((ASI_LO&&tk>=ASI_LO&&tk<ASI_HI)?ASI_AR[tk-ASI_LO]:undefined);\n"
        "        if (cdeclN>=0) OBS_AR[inImg(tr)?('c'+tr.toString(16)):('a'+((tk-ASI_LO)>>>0).toString(16))]=cdeclN;   // bvh copy: call-site count seen\n"
        "        if (kr!==undefined) n=kr; }   // bvh copy: declared / ON-observed arity of a callee wins, same on both sides")
r = rep(r, "  rpc.exports.samples = function(){ return SAMP; };",
        "  rpc.exports.samples = function(){ return SAMP; };\n"
        "  rpc.exports.obsar = function(){ return OBS_AR; };   // bvh copy")
r = rep(r, "        samples = E.samples()\n",
        "        samples = E.samples()\n"
        "        if mode == \"on\": OBS.update(E.obsar() or {})   # bvh copy\n")
# ON first: the port's own call sites clean up with `add esp,N` right after the call, so they give
# every callee's arg count; the OFF run then reads the same count (batch 1, 0x00560260: the
# original's call to 0x0056e680 read 0 args, the port's 16, and that shifted every later ordinal).
r = rep(r, '    print("\\n=== OFF race ==="); off_s, _ = run("off", rvas, targets, flo, fhi)\n'
           '    print("=== ON race ===");  on_s, on_jmp = run("on", rvas, targets, flo, fhi)\n',
        '    print("=== ON race ===");  on_s, on_jmp = run("on", rvas, targets, flo, fhi)   # bvh copy: ON first\n'
        '    for tok, n in OBS.items():\n'
        '        c = tok if tok.startswith("c") else _sym(int(tok[1:], 16))\n'
        '        if c.startswith("c") and n <= 16: IMG_AR.setdefault(int(c[1:], 16), n)\n'
        '    print(f"  ON-observed callee arities: {len(OBS)}")\n'
        '    targets = [t[:5] + [IMG_AR] + t[6:] for t in targets]   # keeps ASI_AR and the return kind (v4 dropped it on OFF)\n'
        '    print("\\n=== OFF race ==="); off_s, _ = run("off", rvas, targets, flo, fhi)\n')
r = rep(r, "def main():", "OBS = {}   # bvh copy: ON-observed callee arg counts\ndef main():")
r = rep(r, "    return function(context){\n      if (activeTarget===null) return;",
        "    let LZ=false;   // bvh copy: lazy cdecl arg count\n    return function(context){\n      if (activeTarget===null) return;")
r = rep(r, "return ptr(i.operands[0].value).toUInt32()>>>2;", "return (Number(i.operands[0].value)>>>0)>>>2;")
r = rep(r, "if(op&&op.type==='imm'){ pc=ptr(op.value).toUInt32(); continue; }", "if(op&&op.type==='imm'){ pc=(Number(op.value)>>>0); continue; }")

# ---- python: map file, own ranges, symbolisation ----
r = rep(r, "def main():",
        r'''import re as _re   # bvh copy
MAPF = ROOT / "mashedmod" / "build" / "mashed_re_dev.map"
def _load_map():
    syms = []
    pat = _re.compile(r"^\s*0001:[0-9a-fA-F]{8}\s+(\S+)\s+([0-9a-fA-F]{8})\s")
    for line in MAPF.read_text(encoding="utf-8", errors="ignore").splitlines():
        m = pat.match(line)
        if m: syms.append((int(m.group(2), 16) - 0x10000000, m.group(1)))
    syms = sorted(set(syms))
    return syms
SYMS = _load_map(); SYM_ADDR = [a for a, _ in SYMS]
def own_ranges(rva):
    tag = "fun_%08x" % rva; out = []
    for i, (a, n) in enumerate(SYMS):
        if tag in n.lower():
            end = SYMS[i + 1][0] if i + 1 < len(SYMS) else a + 0x1000
            out.append([a, end])
    return out
SRC = ROOT / "mashedmod" / "src" / "mashed_re"
def _all_decls():
    """{rva: {param counts}} over every `__cdecl/__stdcall ... FUN_<rva>[_impl](...)` declaration in the tree."""
    pat = _re.compile(r"(?:__cdecl|__stdcall)[^;{(]*?\bFUN_([0-9a-fA-F]{8})(?:_impl)?\s*\(([^)]*)\)")
    d = {}
    for f in SRC.rglob("*.cpp"):
        for r_, a in pat.findall(f.read_text(encoding="utf-8", errors="ignore")):
            a = a.strip(); d.setdefault(int(r_, 16), set()).add(0 if a in ("", "void") else a.count(",") + 1)
    return d
DECLS = _all_decls()
def src_arity(rva):
    """Param count of the port's `__cdecl ... FUN_<rva>(...)` declaration; -1 if absent or the
    declarations disagree. Same value on the OFF and ON side (it only picks which entry args anchor)."""
    s = DECLS.get(rva, set())
    return next(iter(s)) if len(s) == 1 else -1
def ret_kind(rva):
    """'void' | '16' | 'full' from the port's extern "C" declaration (one consistent type, else 'full')."""
    pat = _re.compile(r'extern\s+"C"\s+([\w\s\*]+?)\s+__cdecl\s+FUN_%08x\s*\(' % rva)
    kinds = set()
    for f in SRC.rglob("*.cpp"):
        t = f.read_text(encoding="utf-8", errors="ignore")
        if ("FUN_%08x" % rva) not in t: continue
        for ty in pat.findall(t):
            ty = " ".join(ty.split())
            kinds.add("void" if ty == "void" else "16" if ty in ("undefined2", "ushort", "short", "unsigned short", "uint16_t") else "full")
    return kinds.pop() if len(kinds) == 1 else "full"
def callee_arity_maps():
    """Declared arity of every ported callee, keyed by image RVA and by .asi wrapper offset (`_FUN_<rva>`)."""
    img = {r_: next(iter(s)) for r_, s in DECLS.items() if len(s) == 1 and next(iter(s)) <= 16}
    asi = {}
    for a, n in SYMS:
        m = _re.fullmatch(r"_FUN_([0-9a-fA-F]{8})", n)
        if m and int(m.group(1), 16) in img: asi[a] = img[int(m.group(1), 16)]
    return img, asi
TRANSP_TAGS = ("shadowab@@", "shadowtrack@@", "security_check_cookie")
def transparent_ranges():
    """[lo, hi, skip]: skip = leading template args of ShadowAB::Run (Counter&, impl) = 2,
    RunTracked (+ stackwin) = 3, RunRegion (+ region, bytes) = 4 (ShadowAB.h:223/280,
    ShadowTrack.h:647); -1 for everything else (not recorded)."""
    out = []
    for i, (a, n) in enumerate(SYMS):
        if any(x in n.lower() for x in TRANSP_TAGS):
            end = SYMS[i + 1][0] if i + 1 < len(SYMS) else a + 0x100
            skip = (3 if n.startswith("??$RunTracked@") else 4 if n.startswith("??$RunRegion@")
                    else 2 if n.startswith("??$Run@") else -1)
            out.append([a, end, skip])
    return out
def _sym(off):
    i = bisect.bisect_right(SYM_ADDR, off) - 1
    if i < 0: return "a:?%x" % off
    n = SYMS[i][1]; m = _re.search(r"FUN_([0-9a-fA-F]{8})", n)
    if m: return "c%x" % int(m.group(1), 16)
    if n in ("__chkstk", "__alloca_probe") and off == SYMS[i][0]: return "c4a3440"   # same role: 0x004a3440
    return "a:" + n + ("+%x" % (off - SYMS[i][0]) if off != SYMS[i][0] else "")
_ATOK = _re.compile(r"(?<=[\[,= ])a([0-9a-f]+)(?=[\],\s]|$)")
def symb(samples):
    out = {}
    for k, d in (samples or {}).items():
        nd = {}
        for sig, c in (d or {}).items():
            s2 = _ATOK.sub(lambda m: _sym(int(m.group(1), 16)), sig)
            nd[s2] = nd.get(s2, 0) + c
        out[k] = nd
    return out

def main():''')
r = rep(r, "    targets = [[r, boundary(r)] for r in rvas]",
        "    TR = transparent_ranges()\n"
        "    print(\"  bvh tool v5\")\n"
        "    IMG_AR, ASI_AR = callee_arity_maps()\n"
        "    targets = [[r, boundary(r), own_ranges(r), TR, src_arity(r), IMG_AR, ASI_AR, ret_kind(r)] for r in rvas]   # bvh copy: + .asi own / transparent ranges, arities, return kind\n"
        "    print(f\"  return kind={[t[7] for t in targets]}\")\n"
        "    print(f\"  arity={[t[4] for t in targets]}\")\n"
        "    print(f\"  own asi ranges={[[hex(a), hex(b)] for t in targets for a, b in t[2]]}\")")
r = rep(r, '    print("\\n=== VERDICT (in-race, frame-synced, indirect captured) ===")',
        '    off_s = symb(off_s); on_s = symb(on_s)   # bvh copy\n'
        '    (LOG / "c4_racediff_samples.json").write_text(json.dumps({"off": off_s, "on": on_s}, indent=1), encoding="utf-8")\n'
        '    print("\\n=== VERDICT (in-race, frame-synced, indirect captured) ===")')

(HERE / "c4_racediff_bvh.py").write_text("# GENERATED by make_tool.py from re/frida/canonical_c4_racediff.py\n" + r, encoding="utf-8")
print("wrote c4_racediff_bvh.py")
