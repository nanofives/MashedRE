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

ORIGTOP-ONLY class (HB-LINT-ORIGTOP-CLASS, L-111 (a), 2026-11-09). The MASHED_ORIG_TOP route compiles byte-matched
bodies that run only when the exe enters the matched WinMain 0x00492370; until the ORIG_TOP flip they sit beside the
hand bodies the default path still runs. Such a pair is allowed, as its own class, when:
  * the matched body's TU carries the structural marker line `// RVA-LINT: ORIGTOP-ONLY` (anywhere in its own text,
    not an included file), and that TU is EXE-ONLY (ORIG_TOP is an exe path; a marked TU in asi_sources.rsp fails);
  * exactly one body of the finding is in a marked TU and at least one is in an unmarked TU (two ORIG_TOP bodies of
    one RVA are a real duplicate, not this class);
  * its allowlist line carries the tag: `ORIGTOP-ONLY <CHECK>:<rva>:<files>   # citation`.
A tagged line whose finding is not of the class, or a class finding on an untagged line, fails the build. The lint
prints the D4 burn-down count and the ORIGTOP-ONLY count separately. The whole class is deleted at the ORIG_TOP flip.

  py -3.12 scripts/lint_rva_bodies.py              # guard mode (build.bat calls this)
  py -3.12 scripts/lint_rva_bodies.py --seed       # print an allowlist for today's findings (class lines tagged)
  py -3.12 scripts/lint_rva_bodies.py --verbose    # also print the allowlisted ones
  py -3.12 scripts/lint_rva_bodies.py --selftest   # the ORIGTOP-ONLY rules on synthetic pairs
"""
from __future__ import annotations

import pathlib
import re
import sys

sys.path.insert(0, str(pathlib.Path(__file__).resolve().parent))
import rva_body_scan as S            # noqa: E402

ROOT = S.ROOT
ALLOWLIST = ROOT / "re" / "tools" / "dual_copy_allowlist.txt"
OT_TAG = "ORIGTOP-ONLY"
OT_MARK = re.compile(r"^\s*//\s*RVA-LINT:\s*ORIGTOP-ONLY\b")


def key(check: str, rva: int, files) -> str:
    return f"{check}:{rva:08x}:{','.join(sorted(files))}"


def parse_allowlist(lines) -> dict[str, bool]:
    """{key: tagged_origtop}. A line is `[ORIGTOP-ONLY ]<CHECK>:<rva>:<files>   # comment`."""
    keys = {}
    for ln in lines:
        ln = ln.split("#", 1)[0].strip()
        if not ln:
            continue
        tagged = False
        if ln.startswith(OT_TAG):
            tagged = True
            ln = ln[len(OT_TAG):].strip()
        keys[ln] = tagged
    return keys


def load_allowlist() -> dict[str, bool]:
    if not ALLOWLIST.exists():
        return {}
    return parse_allowlist(ALLOWLIST.read_text(encoding="utf-8").splitlines())


def marked_tus(tus) -> set[str]:
    """TUs whose own text carries the ORIGTOP-ONLY marker line."""
    out = set()
    for rel in tus:
        p = S.SRC / rel
        try:
            for ln in p.read_text(encoding="utf-8", errors="replace").splitlines():
                if OT_MARK.match(ln):
                    out.add(rel)
                    break
        except OSError:
            pass
    return out


def findings(scan=None) -> list[tuple[str, int, list[str], str]]:
    """[(check, rva, files, human_detail), ...] -- deterministic order."""
    scan = scan or S.scan_all()
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


def is_origtop(files, marked) -> bool:
    m = [f for f in files if f in marked]
    return len(m) == 1 and len(files) > len(m)


def evaluate(found, allow: dict[str, bool], marked: set[str], asi_tus: set[str]):
    """-> (new, old_d4, old_ot, errors). `errors` are class / tag / marker violations (each fails the build)."""
    new, old_d4, old_ot, errors = [], [], [], []
    for rel in sorted(marked & asi_tus):
        errors.append(f"{OT_TAG} marker in {rel}, which asi_sources.rsp compiles (ORIG_TOP is an exe path)")
    for f in found:
        check, rva, files, _detail = f
        k = key(check, rva, files)
        cls = check != "DUP-INSTALL" and is_origtop(files, marked)
        if k not in allow:
            new.append(f)
            continue
        tagged = allow[k]
        if tagged and not cls:
            errors.append(f"{k} is tagged {OT_TAG} but is not of the class (needs exactly one body in a TU marked "
                          f"`// RVA-LINT: ORIGTOP-ONLY` beside an unmarked one)")
        elif cls and not tagged:
            errors.append(f"{k} is an {OT_TAG} pair on an untagged line: prefix the line with `{OT_TAG} `")
        elif cls:
            old_ot.append(f)
        else:
            old_d4.append(f)
    return new, old_d4, old_ot, errors


def selftest() -> int:
    hand, ot, ot2, asi_hand = "Gameplay/Hand.cpp", "Match/OrigTop_Synth.cpp", "Match/OrigTop_Synth2.cpp", "Util/AsiOnly.cpp"
    marked = {ot, ot2}
    f_pair = ("DUP-IN-TARGET", 0x00401000, sorted([hand, ot]), "synthetic")
    f_cross = ("CROSS-TARGET", 0x00401010, sorted([asi_hand, ot]), "synthetic")
    f_both = ("DUP-IN-TARGET", 0x00401020, sorted([ot, ot2]), "synthetic")
    f_plain = ("DUP-IN-TARGET", 0x00401030, sorted([hand, asi_hand]), "synthetic")
    cases = [
        ("tagged ORIG_TOP pair is allowed, counted ORIGTOP-ONLY", [f_pair],
         [f"{OT_TAG} {key(*f_pair[:3])}"], set(), (0, 0, 1, 0)),
        ("tagged ORIG_TOP cross-target pair is allowed", [f_cross],
         [f"{OT_TAG} {key(*f_cross[:3])}"], set(), (0, 0, 1, 0)),
        ("untagged ORIG_TOP pair fails", [f_pair], [key(*f_pair[:3])], set(), (0, 0, 0, 1)),
        ("unlisted ORIG_TOP pair is NEW", [f_pair], [], set(), (1, 0, 0, 0)),
        ("two marked bodies are not the class: tag fails", [f_both],
         [f"{OT_TAG} {key(*f_both[:3])}"], set(), (0, 0, 0, 1)),
        ("tag on a plain dual copy fails", [f_plain], [f"{OT_TAG} {key(*f_plain[:3])}"], set(), (0, 0, 0, 1)),
        ("plain dual copy untagged stays D4 burn-down", [f_plain], [key(*f_plain[:3])], set(), (0, 1, 0, 0)),
        ("marker on an asi-compiled TU fails", [f_pair], [f"{OT_TAG} {key(*f_pair[:3])}"], {ot}, (0, 0, 1, 1)),
    ]
    bad = 0
    for name, found, lines, asi, want in cases:
        new, d4, otl, err = evaluate(found, parse_allowlist(lines), marked, asi)
        got = (len(new), len(d4), len(otl), len(err))
        ok = got == want
        bad += not ok
        print(f"[rva-lint selftest] {'PASS' if ok else 'FAIL'} {name}: new/d4/origtop/errors {got} want {want}")
    # the marker regex on real text shapes
    for line, want in (("// RVA-LINT: ORIGTOP-ONLY", True), ("  //RVA-LINT:ORIGTOP-ONLY (L-111)", True),
                       ("// RVA-LINT: ORIGTOP-ONLYX", False), ("/* RVA-LINT: ORIGTOP-ONLY */", False)):
        ok = bool(OT_MARK.match(line)) == want
        bad += not ok
        print(f"[rva-lint selftest] {'PASS' if ok else 'FAIL'} marker {line!r} -> {want}")
    print(f"[rva-lint selftest] {'OK' if not bad else 'FAILED'} ({bad} failing)")
    return 1 if bad else 0


def main(argv) -> int:
    if "--selftest" in argv:
        return selftest()
    seed = "--seed" in argv
    verbose = "--verbose" in argv
    allow = load_allowlist()
    scan = S.scan_all()
    found = findings(scan)
    exe_tus, asi_tus = scan["targets"]["exe"], scan["targets"]["asi"]
    marked = marked_tus(exe_tus | asi_tus)

    if seed:
        print("# dual_copy_allowlist.txt -- seeded "
              "by `py -3.12 scripts/lint_rva_bodies.py --seed`")
        for check, rva, files, detail in found:
            tag = f"{OT_TAG} " if check != "DUP-INSTALL" and is_origtop(files, marked) else ""
            print(f"{tag}{key(check, rva, files)}   # {detail}")
        return 0

    # HB-LINT-408A50: an unresolved re/match/ or .inc include hides bodies, so every
    # count below would be wrong. Fail before reporting them (typical cause: a tree
    # built from a `git archive` that left out re/match/).
    missing = sorted(set(S.MISSING_INCLUDES))
    if missing:
        for rel, line, inc in missing:
            print(f"[rva-lint] ERROR MISSING-INCLUDE {rel}:{line}  \"{inc}\" does not resolve")
        print(f"[rva-lint] FAILED: {len(missing)} include(s) the scanner could not follow, "
              "so their bodies are invisible and the duplicate check is unsound.")
        print("[rva-lint] Build from a tree that has them (scripts/lan_release/archive_paths.txt "
              "lists what a clean `git archive` build needs).")
        return 1

    new, old_d4, old_ot, errors = evaluate(found, allow, marked, asi_tus)
    old = old_d4 + old_ot

    by_check = {}
    for check, _, _, _ in found:
        by_check[check] = by_check.get(check, 0) + 1
    print(f"[rva-lint] {len(found)} finding(s): "
          + ", ".join(f"{k}={v}" for k, v in sorted(by_check.items()))
          + f"  |  allowlisted={len(old)} NEW={len(new)}")
    print(f"[rva-lint] D4 burn-down={len(old_d4)}  {OT_TAG}={len(old_ot)} "
          f"(class deleted at the ORIG_TOP flip; {len(marked)} marked TU(s))")

    if verbose:
        for check, rva, files, detail in old_d4:
            print(f"[rva-lint]   WARN (allowlisted) {check} 0x{rva:08x}  {detail}")
        for check, rva, files, detail in old_ot:
            print(f"[rva-lint]   INFO ({OT_TAG}) {check} 0x{rva:08x}  {detail}")

    dup_install_new = [f for f in new if f[0] == "DUP-INSTALL"]
    other_new = [f for f in new if f[0] != "DUP-INSTALL"]

    for check, rva, files, detail in other_new:
        tag = f" [{OT_TAG} pair: allowlist it with the `{OT_TAG} ` tag]" if is_origtop(files, marked) else ""
        print(f"[rva-lint] ERROR {check} 0x{rva:08x}  {detail}{tag}")
    for check, rva, files, detail in dup_install_new:
        print(f"[rva-lint] ERROR {check} 0x{rva:08x}  {detail}")
    for e in errors:
        print(f"[rva-lint] ERROR {e}")

    if new or errors:
        print()
        if new:
            print(f"[rva-lint] FAILED: {len(new)} NEW duplicate-body finding(s).")
            print("[rva-lint] One RVA must have ONE body per target. Either share a "
                  "single TU between the two source lists, or -- if the divergence is "
                  "deliberate and judged against the original -- add the key above to")
            print(f"[rva-lint]   {ALLOWLIST.relative_to(ROOT)}")
            print("[rva-lint] with a citation. Seed a fresh list with --seed.")
        if errors:
            print(f"[rva-lint] FAILED: {len(errors)} {OT_TAG} class / tag / marker error(s).")
        return 1

    print("[rva-lint] OK -- no NEW duplicate bodies "
          f"({len(old)} known, tracked in {ALLOWLIST.relative_to(ROOT)})")
    return 0


if __name__ == "__main__":
    raise SystemExit(main(sys.argv[1:]))
