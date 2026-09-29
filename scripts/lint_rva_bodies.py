"""One RVA, one body -- build-time guard.

Run from `mashedmod/build.bat` before the compile step. It scans the TUs compiled
into EACH target and reports an RVA that has more than one body, which is how
five shipping defects reached `mashed_re.exe` without any build or tracker
noticing (`re/analysis/DUAL_COPY_AUDIT_2026-09-29.md`).

Three checks:

  DUP-IN-TARGET   one RVA, two bodies in TUs compiled into the SAME target.
                  Only one of them can be the one that runs.
  CROSS-TARGET    one RVA with a body in an EXE-ONLY TU and a body in an
                  ASI-ONLY TU. This is the dual-copy class: Frida evidence
                  measures the .asi body, the shipping exe runs the other one,
                  and nothing links the two.
  DUP-INSTALL     one RVA with two ACTIVE `RH_ScopedInstall` in .asi-compiled
                  TUs. One body is then silently dead in the .asi -- the U-9065
                  failure (`path1-green-does-not-prove-install`). Reported as a
                  SEPARATE list because it is an independent defect: it is about
                  which body installs, not about which body exists.

Policy (2026-09-29): WARN on anything already listed in
`re/tools/dual_copy_allowlist.txt`, **FAIL on anything new**. The allowlist is a
burn-down list, not a permanent exemption -- ROADMAP D4 takes it to zero by
consolidating each pair into one shared TU judged against the original.

  py -3.12 scripts/lint_rva_bodies.py            # guard mode (build.bat calls this)
  py -3.12 scripts/lint_rva_bodies.py --seed     # print an allowlist for today's findings
  py -3.12 scripts/lint_rva_bodies.py --verbose  # also print the allowlisted ones
"""
from __future__ import annotations

import pathlib
import sys

sys.path.insert(0, str(pathlib.Path(__file__).resolve().parent))
import rva_body_scan as S            # noqa: E402

ROOT = S.ROOT
ALLOWLIST = ROOT / "re" / "tools" / "dual_copy_allowlist.txt"


def key(check: str, rva: int, files) -> str:
    return f"{check}:{rva:08x}:{','.join(sorted(files))}"


def load_allowlist() -> set[str]:
    keys = set()
    if not ALLOWLIST.exists():
        return keys
    for ln in ALLOWLIST.read_text(encoding="utf-8").splitlines():
        ln = ln.split("#", 1)[0].strip()
        if ln:
            keys.add(ln)
    return keys


def findings() -> list[tuple[str, int, list[str], str]]:
    """[(check, rva, files, human_detail), ...] -- deterministic order."""
    scan = S.scan_all()
    exe_tus, asi_tus = scan["targets"]["exe"], scan["targets"]["asi"]
    exe_only, asi_only = exe_tus - asi_tus, asi_tus - exe_tus
    out = []

    # DUP-IN-TARGET
    for target in ("exe", "asi"):
        for rva, tus in sorted(S.rva_to_tus(scan, target).items()):
            if len(tus) > 1:
                where = ", ".join(f"{t}:{scan['bodies'][t][rva][1]} "
                                  f"({scan['bodies'][t][rva][0]})" for t in sorted(tus))
                out.append(("DUP-IN-TARGET", rva, sorted(tus),
                            f"[{target}] two bodies: {where}"))

    # CROSS-TARGET
    eb, ab = S.rva_to_tus(scan, "exe"), S.rva_to_tus(scan, "asi")
    for rva in sorted(set(eb) & set(ab)):
        e = [t for t in eb[rva] if t in exe_only]
        a = [t for t in ab[rva] if t in asi_only]
        if e and a:
            out.append(("CROSS-TARGET", rva, sorted(set(e) | set(a)),
                        f"exe-only {', '.join(sorted(e))}  vs  "
                        f"asi-only {', '.join(sorted(a))}"))

    # DUP-INSTALL (.asi only -- in the exe RH_ScopedInstall is a no-op)
    per_rva: dict[int, list[tuple[str, str, int]]] = {}
    for rel, ins in scan["installs"].items():
        if rel not in asi_tus:
            continue
        for rva, sites in ins.items():
            for sym, line in sites:
                per_rva.setdefault(rva, []).append((rel, sym, line))
    for rva, sites in sorted(per_rva.items()):
        files = {f for f, _, _ in sites}
        if len(files) > 1:
            where = ", ".join(f"{f}:{ln} {sym}" for f, sym, ln in sorted(sites))
            out.append(("DUP-INSTALL", rva, sorted(files),
                        f"installed from {len(files)} files: {where}"))
    return out


def main(argv) -> int:
    seed = "--seed" in argv
    verbose = "--verbose" in argv
    allow = load_allowlist()
    found = findings()

    if seed:
        print("# dual_copy_allowlist.txt -- seeded "
              "by `py -3.12 scripts/lint_rva_bodies.py --seed`")
        for check, rva, files, detail in found:
            print(f"{key(check, rva, files)}   # {detail}")
        return 0

    new = [f for f in found if key(f[0], f[1], f[2]) not in allow]
    old = [f for f in found if key(f[0], f[1], f[2]) in allow]

    by_check = {}
    for check, _, _, _ in found:
        by_check[check] = by_check.get(check, 0) + 1
    print(f"[rva-lint] {len(found)} finding(s): "
          + ", ".join(f"{k}={v}" for k, v in sorted(by_check.items()))
          + f"  |  allowlisted={len(old)} NEW={len(new)}")

    if verbose:
        for check, rva, files, detail in old:
            print(f"[rva-lint]   WARN (allowlisted) {check} 0x{rva:08x}  {detail}")

    dup_install_new = [f for f in new if f[0] == "DUP-INSTALL"]
    other_new = [f for f in new if f[0] != "DUP-INSTALL"]

    for check, rva, files, detail in other_new:
        print(f"[rva-lint] ERROR {check} 0x{rva:08x}  {detail}")
    for check, rva, files, detail in dup_install_new:
        print(f"[rva-lint] ERROR {check} 0x{rva:08x}  {detail}")

    if new:
        print()
        print(f"[rva-lint] FAILED: {len(new)} NEW duplicate-body finding(s).")
        print("[rva-lint] One RVA must have ONE body per target. Either share a "
              "single TU between the two source lists, or -- if the divergence is "
              "deliberate and judged against the original -- add the key above to")
        print(f"[rva-lint]   {ALLOWLIST.relative_to(ROOT)}")
        print("[rva-lint] with a citation. Seed a fresh list with --seed.")
        return 1

    print("[rva-lint] OK -- no NEW duplicate bodies "
          f"({len(old)} known, tracked in {ALLOWLIST.relative_to(ROOT)})")
    return 0


if __name__ == "__main__":
    raise SystemExit(main(sys.argv[1:]))
