"""dcphys2 tracker transaction (re-classify): A6b 0x00468980 / A5 0x0046ddb0 / A4 0x00470670 are one body each
(file == exe_file); levels at the default body (knobs OFF): A6b C4 -> C2, A4 C3 -> C2, A5 stays C2. U-9785, U-9786 filed.
Line endings kept."""
import csv, io, os
W = os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
R = 'verify/dcphys2_20261022/RESULT_dcphys2.md'
D = '2026-10-22'
V = 'mashedmod/src/mashed_re/Vehicle/'


def rw(path, fn):
    p = os.path.join(W, path)
    s = open(p, encoding='utf-8', newline='').read()
    open(p, 'w', encoding='utf-8', newline='').write(fn(s, '\r\n' if '\r\n' in s else '\n'))


COMMON = ("Levels describe the ONE body as it ships (exe knobs OFF); the knob-ON body has C4-grade evidence (hook installed "
          "live, canonical race, in-process A/B vs the live original over the whole 0xd04 record, no-op control 0 on every "
          "call, no stub in the body) and goes C4 if the knobs are flipped default ON (decision pending). Exe default "
          "unchanged by the refactor: G-CTRL 23ac657a / 7bf2f27f / 3186dd09 x2, B 0e3790c4 / 55b84dd8 / 2324715b, (b) 9/9, "
          "(e) 3/3, D2 3/3. Harness: x20 quotas (MASHED_PHYS_SELFTEST_CAP). " + R + " | ")
ROWS = {
    '00468980': ('C4', 'C2', 'AeroStabilize.cpp',
                 "dcphys2 " + D + ": ONE body, Vehicle/AeroStabilize.cpp in both rsp files (the naked .asi copy A6b_BodyAsm "
                 "deleted; its C4 described that copy). DEFAULT NOT CLEAN: the CRT acos chain at PC_24 "
                 "(0x00468a09..0x00468a22 / 0x00468a89..0x00468aa0) and the LUT normalise (0x00468af1) need "
                 "MASHED_A6B_ACOS_CRT / MASHED_A6B_NORM_LUT (exe default OFF, .asi seeds ON); knobs OFF 142/147 boosted "
                 "airborne calls differ. Knobs ON: 160 grounded + 242 boosted airborne bit-identical on the final .asi apart "
                 "from the matrix pad words m[7]/m[11]/m[15] (stack contents FUN_004c4d20 copies; reader 0x004dc410 drops "
                 "lane 3; counted as npad). Coverage U-9786: state != 0 and natural air unsampled. "),
    '0046ddb0': ('C2', 'C2', 'ForceIntegrator.cpp',
                 "dcphys2 " + D + ": ONE body, Vehicle/ForceIntegrator.cpp in both rsp files with A5* thunks (the .asi C++ "
                 "A5_Body deleted; it missed 18..385 sampled calls per race); the exe-only leaf ports 0x00442ce0 / "
                 "0x00442c80 / 0x0046dbe0 moved verbatim to Vehicle/ForceIntegratorLeaves.cpp. DEFAULT NOT CLEAN: "
                 "MASHED_A5_RING_ORDER (0x0046e6f3..0x0046e728; OFF = 960/960 differ), new MASHED_A5_DOT_ORDER (steer "
                 "projection 0x0046e419.., drafting dot 0x0046dff6..), MASHED_A5_AIR_F32 (0x0046e927..0x0046e94c), "
                 "MASHED_A5_SPIN_FILD (+0x2c FILD at 0x0046e82f), MASHED_A5_CONTACT_INT (0x0046e761, no sampled effect), "
                 "all exe default OFF, .asi seeds ON. Knobs ON on the final .asi: 960 + 1280 (320 boosted airborne) + 974 "
                 "Warzone (14 natural partial-ground) bit-identical incl. scratch 0x00881560..0x0088158f. Input U-9785 (the "
                 "exe's random draw is a stand-in), U-9784 (game mode); coverage U-9786 (random surface unsampled). "),
    '00470670': ('C3', 'C2', 'VehicleControl.cpp',
                 "dcphys2 " + D + ": ONE body, Vehicle/VehicleControl.cpp in both rsp files (the .asi A4_BodyMath / A4_Body "
                 "deleted; its C3 described that copy); the .asi calls A5 / A6a / A6b at their live RVAs with the original's "
                 "register ABI (0x00470914..0x00470943) and passes the original's +0x928 matrix, the exe its body basis "
                 "(U-9152). The A/B is now the WHOLE function (the original runs its own dispatch). DEFAULT NOT CLEAN: the "
                 "slide dot (0x004706db..0x00470701) needs new MASHED_A4_DOT_ORDER (exe default OFF, .asi seeds ON); OFF = "
                 "204/1828 differ. Knob ON on the final .asi: 1830 + 1783 Warzone + 1790 boosted (25 airborne, pad-only) "
                 "bit-identical incl. scratch. Input U-9784 (game mode). "),
}


def hooks(s, nl):
    lines = s.split(nl)
    h = next(csv.reader([lines[0]]))
    ix = {k: i for i, k in enumerate(h)}
    hit = set()
    for li, line in enumerate(lines[1:], 1):
        k = line[:8]
        if k not in ROWS or line[8] != ',':
            continue
        old, new, ef, note = ROWS[k]
        r = next(csv.reader([line]))
        assert r[ix['confidence']] == old, (k, r[ix['confidence']])
        assert r[ix['file']] == V + 'PhysicsChainHooks.cpp' and r[ix['exe_file']] == V + ef, k
        r[ix['confidence']] = new
        r[ix['file']] = V + ef
        r[ix['notes']] = note + COMMON + r[ix['notes']]
        out = io.StringIO(newline='')
        csv.writer(out, lineterminator='').writerow(r)
        lines[li] = out.getvalue()
        hit.add(k)
    assert hit == set(ROWS), hit
    return nl.join(lines)


def uncertainties(s, nl):
    assert '| U-9785 |' not in s and '| U-9786 |' not in s
    hdr = '|----|------|-------|-----------|------------------|--------------------|--------|' + nl
    assert s.count(hdr) == 1
    r86 = ("| U-9786 | coverage | dcphys2's in-process A/B of A6b `0x00468980` and A5 `0x0046ddb0` (shared bodies) | Not "
           "sampled on any call: A6b's motion-state != 0 leg (velocity align, `0x00468abb..0x00468b29`) and natural air "
           "(5 Warzone runs of 60 / 120 s gave 0 airborne A6b calls; every airborne sample is `--boost`, state 0); A5's "
           "random-surface branch (key `0xffff32ff`, FUN_00472650 x2 at `0x0046e7ac` / `0x0046e7c3`, `rng=1` on 0 of 14 "
           "runs); and no sampled call distinguishes `MASHED_A5_CONTACT_INT` from the float compare. | whether the shared "
           "bodies match the original on those paths | natural-air captures with A6b hooked (Warzone jumps, more attempts "
           "or a powerup launch), a track with surface key `0xffff32ff`, and a contact word of `0x80000000` | C4 coverage "
           "of those three paths only |" + nl)
    r85 = ("| U-9785 | structural | the standalone's random draw for A5 `0x0046ddb0`'s random-surface branch, "
           "`Fi_RandRange` (`Vehicle/ForceIntegratorStubs.cpp:77`) | The original draws twice from FUN_00472650 (the "
           "`0x007dc578` PRNG) at `0x0046e7ac` / `0x0046e7c3` into `+0xb00` / `+0xb08`. The exe's thunk returns `lo` "
           "without drawing unless `MASHED_FI_RAND=1`, which draws from the port's own ring (`Ai/AiStandalone.cpp` AiRand). "
           "The .asi calls the live FUN_00472650, which is what the A/B measured. | which values the standalone should draw "
           "there and from which stream | bind `Fi_RandRange` to the port's FUN_00472650 equivalent once the PRNG stream is "
           "shared, then trace a random-surface track on both sides | the exe's `+0xb00` / `+0xb08` on random-surface "
           "wheels (A5's input, not its body) |" + nl)
    return s.replace(hdr, hdr + r86 + r85, 1)


def changelog(s, nl):
    m = '<!-- ENTRIES -->' + nl
    assert s.count(m) == 1 and '  dcphys2:' not in s
    lines = [
        D + "  00470670  VehicleControlUpdate  C3->C2  dcphys2: one body (VehicleControl.cpp, both targets); the C3 was the "
            "deleted .asi copy; default body not clean (slide dot, MASHED_A4_DOT_ORDER default OFF: 204/1828); knob ON "
            "5403/5403 bit-identical, whole-function A/B, noo 0. " + R,
        D + "  0046ddb0  VehicleWheelForceIntegrator  C2->C2  dcphys2: one body (ForceIntegrator.cpp, both targets); default "
            "not clean (A5_RING_ORDER / DOT_ORDER / AIR_F32 / SPIN_FILD default OFF); knobs ON 3214/3214 bit-identical, noo "
            "0. U-9785, U-9786 filed. " + R,
        D + "  00468980  VehicleAeroStabilizer  C4->C2  dcphys2: one body (AeroStabilize.cpp, both targets); the C4 was the "
            "deleted naked .asi copy; default not clean (A6B_ACOS_CRT / NORM_LUT default OFF: 142/147 airborne); knobs ON "
            "402/402 bit-identical apart from the matrix pad words, noo 0. " + R,
    ]
    return s.replace(m, m + nl.join(lines) + nl, 1)


rw('hooks.csv', hooks)
rw('UNCERTAINTIES.md', uncertainties)
rw('re/analysis/CHANGELOG.md', changelog)
print('applied')
