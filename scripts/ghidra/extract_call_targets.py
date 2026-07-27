"""Extract every hardcoded address used as a CALL TARGET in the reimpl sources.

A wrong data-global address usually misbehaves quietly; a wrong CALL target executes
garbage. So we only collect addresses that reach a call:
  - reinterpret_cast<RET(__cdecl*)(ARGS)>(0xADDR)     function-pointer cast
  - as_fn<Fn_x_t>(0xADDR)                             UtilMid helper
  - kFn_... = 0xADDR                                  named callee constants
  - RH_ScopedInstall(Sym, 0xADDR)                     hook install targets
"""
import os, re, json, sys

ROOT = r"C:\Users\maria\Desktop\Proyectos\Mashed\mashedmod\src\mashed_re"

PATS = [
    ("fnptr_cast", re.compile(r"reinterpret_cast\s*<[^;]*?\(\s*(?:__cdecl|__stdcall|__fastcall|__thiscall)?\s*\*\s*\)[^;]*?>\s*\(\s*(0x0[0-9a-fA-F]{7})\s*\)", re.S)),
    ("as_fn",      re.compile(r"as_fn\s*<[^>]*>\s*\(\s*(0x0[0-9a-fA-F]{7})\s*\)")),
    ("kFn_const",  re.compile(r"\bk[A-Za-z0-9_]*\s*=\s*(0x0[0-9a-fA-F]{7})u?\s*;")),
    ("rh_install", re.compile(r"RH_ScopedInstall\s*\(\s*[A-Za-z0-9_]+\s*,\s*(0x0[0-9a-fA-F]{7})")),
]

rows = []
for dirpath, _, files in os.walk(ROOT):
    for fn in files:
        if not fn.endswith((".cpp", ".h", ".hpp")):
            continue
        path = os.path.join(dirpath, fn)
        try:
            text = open(path, encoding="utf-8", errors="replace").read()
        except Exception:
            continue
        lines = text.split("\n")
        for kind, pat in PATS:
            for m in pat.finditer(text):
                addr = int(m.group(1), 16)
                if not (0x00400000 <= addr < 0x00995000):
                    continue
                line_no = text.count("\n", 0, m.start()) + 1
                snippet = lines[line_no - 1].strip()[:110] if line_no - 1 < len(lines) else ""
                rows.append({
                    "addr": addr,
                    "kind": kind,
                    "file": os.path.relpath(path, ROOT).replace("\\", "/"),
                    "line": line_no,
                    "snippet": snippet,
                })

# de-dup on (addr, file, line)
seen, uniq = set(), []
for r in rows:
    k = (r["addr"], r["file"], r["line"])
    if k not in seen:
        seen.add(k); uniq.append(r)

addrs = sorted({r["addr"] for r in uniq})
out = os.path.join(os.path.dirname(os.path.abspath(__file__)), "call_targets.json")
json.dump(uniq, open(out, "w"), indent=1)

print("call-target sites: %d   distinct addresses: %d" % (len(uniq), len(addrs)))
by_kind = {}
for r in uniq:
    by_kind[r["kind"]] = by_kind.get(r["kind"], 0) + 1
for k, v in sorted(by_kind.items(), key=lambda t: -t[1]):
    print("   %-12s %d" % (k, v))
print("json ->", out)
print("\naddress list (for the Ghidra entry check):")
print(" ".join("0x%08x" % a for a in addrs[:12]), "..." if len(addrs) > 12 else "")
