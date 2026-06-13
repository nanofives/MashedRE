#!/usr/bin/env py -3.12
"""promote_classify.py — disasm-shape auto-classifier for the promotion loop.

Consumes the frontier (scripts/promote_frontier.py output) and, for each
candidate, decodes the FULL function body and matches it against the
display-INDEPENDENT shapes that early_window_leaf_diff.py already supports.
For an exact, fully-accounted-for match it emits a ready-to-paste
hooks_registry entry + reimpl C++. Anything not fully recognized is tagged
MANUAL (with the disasm) or STATE (reads live globals / derefs a runtime arg →
needs run_diff against a booted game). This turns a round into review-and-confirm.

NO-GUESSING: a shape is auto-emitted ONLY when every instruction in the body is
consumed by the template (so the reimpl is mechanically derived, not inferred).
Partial matches fall to MANUAL — never auto-author a body we didn't fully decode.

Recognized display-independent shapes (→ early_window arg_type):
  const_return        mov eax,imm32; ret                         → const_return
  read_global_u32     mov eax,[g]; ret                           → read_global (u32)
  read_global_f32     fld dword[g]; ret                          → read_global (float)
  global_field_read   mov eax,[g]; mov eax,[eax+off]; ret        → global_field_read
  abs_table_idx4      mov eax,[esp+4]; mov eax,[eax*4+tbl]; ret  → int_scalar+seed_table
  const_setter        mov dword[g],imm; ret                      → scalars_to_scattered_globals
  param_setter        mov eax,[esp+4]; mov [g],eax; ret          → void_setter_observe
  multi_const_store   [xor eax,eax;] mov [g_k],(eax|imm); …; ret → scalars_to_scattered_globals

Output:
  re/analysis/plans/promote_classified.tsv         (rva  klass  shape  arg_type  reason)
  re/analysis/plans/promote_auto_<n>.cpp           (generated reimpl cluster, AUTO rows)
  re/analysis/plans/promote_auto_<n>.registry.txt  (paste-ready hooks_registry entries)

Run:  py -3.12 scripts/promote_classify.py [--emit <tag>]
"""
import os
import struct
import sys

import capstone

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
sys.path.insert(0, os.path.join(ROOT, "scripts"))
import promote_frontier as PF

FRONTIER = os.path.join(ROOT, "re", "analysis", "plans", "promote_frontier.tsv")
PLANS = os.path.join(ROOT, "re", "analysis", "plans")

GLOBAL_LO, GLOBAL_HI = 0x005EA000, 0x00915000   # .data / _rwdseg (writable globals)
RDATA_LO, RDATA_HI = 0x005CC000, 0x005E9044     # read-only consts (NOT seedable)


def u32(b, i=0):
    return struct.unpack_from("<I", b, i)[0]


def decode_body(code, tva, va, size):
    md = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_32)
    off = va - tva
    return list(md.disasm(code[off:off + size], va))


def is_writable_global(a):
    return GLOBAL_LO <= a < GLOBAL_HI


def classify(insns, va):
    """Return (arg_type, config, reim_cpp, reason) or (None, None, None, why-not)."""
    if not insns:
        return None, None, None, "no-decode"
    ms = [(i.mnemonic, i.op_str, i.bytes) for i in insns]
    # strip a trailing ret for sequence matching
    def is_ret(m):
        return m[2] and m[2][0] in (0xC3, 0xC2)

    body = ms[:]
    if not body or not is_ret(body[-1]):
        return None, None, None, "no-clean-ret (multi-exit/tail)"
    core = body[:-1]

    # --- const_return: mov eax, imm32 ; ret
    if len(core) == 1 and core[0][2][:1] == b"\xb8":
        imm = u32(core[0][2], 1)
        return ("const_return", {"ret": "u32"},
                f"return 0x{imm:08x}u;", f"const_return 0x{imm:08x}")

    # --- const_return 0: xor eax,eax ; ret   (33 C0 / 31 C0)
    if len(core) == 1 and core[0][2][:2] in (b"\x33\xc0", b"\x31\xc0"):
        return ("const_return", {"ret": "u32"}, "return 0u;", "const_return 0")

    # --- read_global_u32: mov eax,[imm32] ; ret   (A1 disp32)
    if len(core) == 1 and core[0][2][:1] == b"\xa1":
        g = u32(core[0][2], 1)
        return ("read_global", {"ret": "u32", "target_global": g},
                f"return *(uint32_t*)0x{g:08x};", f"read_global_u32 [0x{g:08x}]")

    # --- read_global_f32: fld dword[imm32] ; ret   (D9 05 disp32)
    if len(core) == 1 and core[0][2][:2] == b"\xd9\x05":
        g = u32(core[0][2], 2)
        return ("read_global", {"ret": "float", "target_global": g},
                f"return *(float*)0x{g:08x};", f"read_global_f32 [0x{g:08x}]")

    # --- global_field_read: mov eax,[g] ; mov eax,[eax+off8] ; ret
    if (len(core) == 2 and core[0][2][:1] == b"\xa1"
            and core[1][2][:2] == b"\x8b\x40"):
        g = u32(core[0][2], 1)
        off = core[1][2][2]
        return ("global_field_read", {"ret": "u32", "target_global": g, "field_off": off},
                f"return *(uint32_t*)(*(uint32_t*)0x{g:08x} + 0x{off:x});",
                f"global_field_read *(*[0x{g:08x}]+0x{off:x})")

    # --- abs_table_idx4: mov eax,[esp+4] ; mov eax,[eax*4+tbl] ; ret
    if (len(core) == 2 and core[0][2][:4] == b"\x8b\x44\x24\x04"
            and core[1][2][:3] == b"\x8b\x04\x85"):
        tbl = u32(core[1][2], 3)
        if RDATA_LO <= tbl < RDATA_HI:
            return None, None, None, f"abs_table in .rdata 0x{tbl:08x} (not seedable)"
        return ("int_scalar",
                {"ret": "u32", "seed_table": {"base": tbl, "stride": 4, "span": 8}},
                f"return *(uint32_t*)(0x{tbl:08x} + (i << 2));",
                f"abs_table_idx4 [0x{tbl:08x}+i*4]")

    # --- const_setter (single): mov dword[g], imm32 ; ret   (C7 05 disp32 imm32)
    if len(core) == 1 and core[0][2][:2] == b"\xc7\x05":
        g = u32(core[0][2], 2)
        imm = u32(core[0][2], 6)
        if not is_writable_global(g):
            return None, None, None, f"const_setter target 0x{g:08x} not writable .data"
        return ("scalars_to_scattered_globals",
                {"ret": "none", "observe": [{"addr": g}]},
                f"*(uint32_t*)0x{g:08x} = 0x{imm:08x}u;",
                f"const_setter [0x{g:08x}]=0x{imm:08x}")

    # --- param_setter: mov eax,[esp+4] ; mov [g],eax ; ret  (8B 44 24 04 / A3 disp32)
    if (len(core) == 2 and core[0][2][:4] == b"\x8b\x44\x24\x04"
            and core[1][2][:1] == b"\xa3"):
        g = u32(core[1][2], 1)
        if not is_writable_global(g):
            return None, None, None, f"param_setter target 0x{g:08x} not writable .data"
        return ("void_setter_observe", {"ret": "none", "target_global": g},
                f"*(uint32_t*)0x{g:08x} = v;", f"param_setter [0x{g:08x}]=arg")

    # --- multi_const_store: void(); writes only constants/zero to writable globals.
    #     allowed insns: xor eax,eax | mov eax,imm32 | mov dword[g],imm32 |
    #                     mov [g],eax (where eax holds the last loaded const)
    obs, writes, eax_val, ok = [], [], None, True
    if core:
        for m, ops, bs in core:
            if bs[:2] == b"\x31\xc0" or bs[:2] == b"\x33\xc0":          # xor eax,eax
                eax_val = 0
            elif bs[:1] == b"\xb8":                                     # mov eax,imm32
                eax_val = u32(bs, 1)
            elif bs[:2] == b"\xc7\x05":                                 # mov [g],imm32
                g = u32(bs, 2); v = u32(bs, 6)
                if not is_writable_global(g): ok = False; break
                writes.append((g, v)); obs.append({"addr": g})
            elif bs[:1] == b"\xa3" and eax_val is not None:             # mov [g],eax
                g = u32(bs, 1)
                if not is_writable_global(g): ok = False; break
                writes.append((g, eax_val)); obs.append({"addr": g})
            else:
                ok = False; break
        if ok and len(writes) >= 2:
            cpp = " ".join(f"*(uint32_t*)0x{g:08x} = 0x{v:08x}u;" for g, v in writes)
            return ("scalars_to_scattered_globals", {"ret": "none", "observe": obs},
                    cpp, f"multi_const_store x{len(writes)}")

    # ---- not a recognized display-independent shape ----
    # Distinguish STATE (reads .data globals / derefs the arg) from MANUAL.
    reads_global = any(("0x6" in o or "0x7" in o or "0x8" in o or "0x9" in o)
                       and "[" in o for _, o, _ in ms)
    derefs_arg = any("[eax" in o or "[ecx" in o or "[edx" in o for _, o, _ in ms)
    if reads_global or derefs_arg:
        return None, None, None, "STATE: reads live global / derefs arg → needs booted game"
    return None, None, None, "MANUAL: unrecognized leaf shape"


def gen_name(va, arg_type):
    tag = f"{va & 0xffffff:06x}"
    if arg_type == "const_return":
        return f"AutoRet_{tag}", f"auto_ret_{tag}"
    if arg_type == "read_global":
        return f"AutoGet_{tag}", f"auto_get_{tag}"
    if arg_type == "global_field_read":
        return f"AutoFieldGet_{tag}", f"auto_field_{tag}"
    if arg_type == "int_scalar":
        return f"AutoTableGet_{tag}", f"auto_table_{tag}"
    if arg_type in ("void_setter_observe", "scalars_to_scattered_globals"):
        return f"AutoSet_{tag}", f"auto_set_{tag}"
    return f"Auto_{tag}", f"auto_{tag}"


def main():
    emit_tag = None
    if "--emit" in sys.argv:
        emit_tag = sys.argv[sys.argv.index("--emit") + 1]

    a = PF.analyze()
    code, tva, size_of, hooks = a["code"], a["text_va"], a["size_of"], a["hooks"]

    rows = []
    with open(FRONTIER, encoding="utf-8") as f:
        next(f)
        for line in f:
            p = line.rstrip("\n").split("\t")
            rows.append((int(p[0], 16), p[1], p[2], int(p[3])))

    auto, state, manual = [], [], []
    for va, name, sub, sz in rows:
        insns = decode_body(code, tva, va, size_of.get(va, sz))
        at, cfg, cpp, reason = classify(insns, va)
        if at:
            auto.append((va, name, sub, sz, at, cfg, cpp, reason))
        elif reason.startswith("STATE"):
            state.append((va, name, sub, sz, reason))
        else:
            manual.append((va, name, sub, sz, reason))

    out_tsv = os.path.join(PLANS, "promote_classified.tsv")
    with open(out_tsv, "w", encoding="utf-8") as f:
        f.write("rva\tklass\tshape\targ_type\tsubsystem\tsize\treason\n")
        for va, name, sub, sz, at, cfg, cpp, reason in auto:
            f.write(f"{va:08x}\tAUTO\t{reason}\t{at}\t{sub}\t{sz}\t{cpp}\n")
        for va, name, sub, sz, reason in state:
            f.write(f"{va:08x}\tSTATE\t-\t-\t{sub}\t{sz}\t{reason}\n")
        for va, name, sub, sz, reason in manual:
            f.write(f"{va:08x}\tMANUAL\t-\t-\t{sub}\t{sz}\t{reason}\n")

    print(f"wrote {out_tsv}")
    print(f"AUTO (display-independent, emittable): {len(auto)}")
    print(f"STATE (needs booted game / run_diff):  {len(state)}")
    print(f"MANUAL (unrecognized leaf shape):      {len(manual)}")
    from collections import Counter
    print("\nAUTO by arg_type:")
    for at, n in Counter(x[4] for x in auto).most_common():
        print(f"   {at:30s} {n}")
    print("\nAUTO candidates:")
    for va, name, sub, sz, at, cfg, cpp, reason in auto:
        print(f"   {va:08x} {sub:10s} {at:28s} {reason}")

    if emit_tag and auto:
        emit(auto, emit_tag, hooks)


def emit(auto, tag, hooks):
    cpp_path = os.path.join(PLANS, f"promote_auto_{tag}.cpp")
    reg_path = os.path.join(PLANS, f"promote_auto_{tag}.registry.txt")
    cl, rl = [], []
    cl.append('// AUTO-GENERATED by scripts/promote_classify.py — REVIEW before building.')
    cl.append('// Each reimpl is mechanically derived from a fully-decoded leaf body.')
    cl.append('#include "../rwcore.h"  // adjust include to the cluster convention')
    cl.append("")
    for va, name, sub, sz, at, cfg, cpp, reason in auto:
        cppname, export = gen_name(va, at)
        ret = cfg.get("ret", "u32")
        ctype = {"u32": "uint32_t", "float": "float", "none": "void"}[ret]
        arg = "" if at in ("const_return", "read_global", "global_field_read",
                           "scalars_to_scattered_globals") else \
              ("uint32_t i" if at == "int_scalar" else "uint32_t v")
        cl.append(f"// 0x{va:08x}  {reason}")
        cl.append(f'extern "C" {ctype} {export}({arg}) {{ {cpp} }}')
        # registry fragment
        entry = [f"  '{export}': {{",
                 f"    'rva': '0x{va:08x}', 'export': '{export}',",
                 f"    'signature': {{'ret': '{ret}'}},",
                 f"    'arg_type': '{at}',"]
        if "target_global" in cfg:
            entry.append(f"    'target_global': 0x{cfg['target_global']:08x},")
        if "field_off" in cfg:
            entry.append(f"    'field_off': 0x{cfg['field_off']:x},")
        if "observe" in cfg:
            obs = ", ".join("{'addr': 0x%08x}" % o["addr"] for o in cfg["observe"])
            entry.append(f"    'observe': [{obs}],")
        if "seed_table" in cfg:
            st = cfg["seed_table"]
            entry.append(f"    'seed_table': {{'base': 0x{st['base']:08x}, 'stride': {st['stride']}, 'span': {st['span']}}},")
        entry.append("    'path1_tests': [0, 1, 2, 3, 7],")
        entry.append("  },")
        rl.extend(entry)
    open(cpp_path, "w", encoding="utf-8").write("\n".join(cl) + "\n")
    open(reg_path, "w", encoding="utf-8").write("\n".join(rl) + "\n")
    print(f"\nemitted {cpp_path}")
    print(f"emitted {reg_path}")


if __name__ == "__main__":
    sys.exit(main())
