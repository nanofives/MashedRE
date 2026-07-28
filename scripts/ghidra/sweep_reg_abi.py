# sweep_reg_abi.py — systematic sweep for the two register-ABI defect classes.
#
# Motivation (2026-07-27): three runtime defects in one session came from register
# assumptions that no existing gate checks. A value-level Frida diff cannot see either
# class BY CONSTRUCTION — it compares outputs, not the register state on return.
#
#   CLASS A  "installed-hook ABI mismatch"  (9 instances to date, e.g. 0x0045c640)
#     Our hook is plain C. Compiled C treats EAX/ECX/EDX as free scratch. If the ORIGINAL
#     never writes one of them, an UNHOOKED caller is entitled to keep a live value there
#     across the CALL — and our hook destroys it.
#       0x0045c6b1 XOR EDX,EDX / CALL 0x0045c640 / MOV [EDX*4+0x88f0c0],0
#
#   CLASS B  "naked thunk trusted by C"  (new, 0x00448700)
#     A __declspec(naked) helper in the SAME TU whose visible __asm does not write EAX.
#     MSVC reads the inline asm to decide what the call clobbers, concludes EAX survives,
#     and keeps a C variable there across the call. The callee (reached by tail-jmp) then
#     returns a value in EAX and silently overwrites it.
#       mov eax,0x64 / call <naked thunk> / sub eax,1 / jne   <- counter WAS the return value
#
# Stage 1 (this file, offline): inventory every live RH_ScopedInstall, classify the port as
# naked vs plain C, and flag CLASS B thunk patterns. Emits hooks_inventory.json.
# Stage 2 (sweep_reg_abi_eval.py): READ-ONLY ghidra_eval pass for function bodies + callers.
import json, os, re, sys

ROOT = os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
SRC = os.path.join(ROOT, "mashedmod", "src", "mashed_re")

# A live install: not preceded by // on the same line.
RE_INSTALL = re.compile(r"^(?P<pre>[^\n]*?)RH_ScopedInstall\s*\(\s*(?P<sym>[A-Za-z_][A-Za-z0-9_]*)\s*,\s*(?P<rva>0x[0-9a-fA-F]{6,8})")
RE_NAKED_DEF = re.compile(r"__declspec\s*\(\s*naked\s*\)")


def is_commented(pre):
    return "//" in pre or pre.strip().startswith("*")


def main():
    hooks = []
    naked_syms = {}
    asm_funcs = []          # (file, symbol, writes_eax, is_tailjmp)
    for dirpath, _, files in os.walk(SRC):
        for fn in files:
            if not fn.endswith((".cpp", ".h", ".hpp")):
                continue
            path = os.path.join(dirpath, fn)
            rel = os.path.relpath(path, ROOT).replace("\\", "/")
            try:
                text = open(path, encoding="utf-8", errors="replace").read()
            except Exception:
                continue
            lines = text.split("\n")

            # --- symbols defined __declspec(naked) in this TU ---
            for m in RE_NAKED_DEF.finditer(text):
                tail = text[m.end():m.end() + 400]
                sm = re.search(r"([A-Za-z_][A-Za-z0-9_]*)\s*\(", tail)
                if not sm:
                    continue
                sym = sm.group(1)
                naked_syms.setdefault(sym, rel)
                # body: from here to the closing of the __asm block (approximate: next 3000 chars)
                body = text[m.end():m.end() + 3000]
                brace = body.find("__asm")
                seg = body[brace:brace + 1500] if brace >= 0 else ""
                writes_eax = bool(re.search(r"\b(mov|xor|add|sub|inc|dec|lea|pop|movzx|movsx)\s+eax\b", seg, re.I))
                is_tailjmp = bool(re.search(r"\bjmp\b", seg, re.I)) or bool(
                    re.search(r"push\s+0x[0-9a-fA-F]+\s*\n\s*ret", seg, re.I))
                asm_funcs.append({"file": rel, "sym": sym,
                                  "writes_eax": writes_eax, "tailjmp": is_tailjmp})

            # --- live hook installs ---
            for i, line in enumerate(lines, 1):
                m = RE_INSTALL.search(line)
                if not m or is_commented(m.group("pre")):
                    continue
                hooks.append({"rva": int(m.group("rva"), 16),
                              "sym": m.group("sym"), "file": rel, "line": i})

    for h in hooks:
        h["naked"] = h["sym"] in naked_syms

    # CLASS B candidates: a naked thunk whose asm never writes EAX and which transfers
    # control elsewhere -> MSVC may assume EAX survives a call to it.
    classb = [a for a in asm_funcs if a["tailjmp"] and not a["writes_eax"]]

    out = {"hooks": hooks, "naked_syms": sorted(naked_syms), "classb_candidates": classb}
    dest = os.path.join(os.path.dirname(os.path.abspath(__file__)), "hooks_inventory.json")
    with open(dest, "w") as f:
        json.dump(out, f, indent=1)

    plain = [h for h in hooks if not h["naked"]]
    print("live RH_ScopedInstall hooks : %d" % len(hooks))
    print("  ported as naked asm       : %d" % (len(hooks) - len(plain)))
    print("  ported as plain C         : %d   <- CLASS A candidate pool" % len(plain))
    print("naked helpers seen          : %d" % len(asm_funcs))
    print("CLASS B candidates (tail-transfer thunk, asm never writes EAX): %d" % len(classb))
    for c in classb:
        print("   %-34s %s" % (c["sym"], c["file"]))
    print("json -> %s" % dest)


if __name__ == "__main__":
    main()
