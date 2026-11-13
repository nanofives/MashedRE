# PROV-INSITU per-tick verdict (2026-11-11): one detorig.py run with DDK bodies installed (--prov) against a reference
# run of the same configuration with no body, using a second reference run to name the blocks that differ between two
# unmodified runs (the MASK / RULE blocks of RO-WARP-NORMALIZE).
#
# Unlike detorig_ticks.py (which does not end the count on a CONSTANT block, one that differs from tick 0 on), every
# block outside the allowed set counts from tick 0: the bodies go live at the warp, before the load, so a divergence
# made during the load shows as a block that already differs at tick 0.
#
#   py -3.12 re/frida/prov_compare.py RUN REF_A REF_B [--json OUT]
# Verdict: IDENTICAL-FOR-N (N = ticks compared) or DIVERGED at tick t with the blocks (and raw words) that differ.
import argparse, json, struct, sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
from detorig import DATA_LO, is_ptr_pair, region_of
from detorig_ticks import load


def diffs(ra, rb, nblk, n):
    return [{j for j in range(2, 2 + nblk) if ra[i][j] != rb[i][j]} for i in range(n)]


def _masked():
    """MASK:* rows of re/frida/warp_normalize.tsv (OS-assigned, library-owned, set before any hook) plus the RULE rows
    that hold OS handles / messages / the stdout cursor (HWND, WINMSG, FILEPTR). The STACKROW / RINGSTALE / RWPAD /
    RING4 rules are not masked: a body under test can write those structures."""
    out = set()
    for ln in (Path(__file__).resolve().parent / "warp_normalize.tsv").read_text(encoding="utf-8").splitlines():
        f = ln.split("	")
        if len(f) < 5 or not f[0].startswith("0x"):
            continue
        act = f[4]
        if act.startswith("MASK:") or act in ("RULE:HWND", "RULE:WINMSG", "RULE:FILEPTR"):
            va, cnt, stride, width = int(f[0], 16), int(f[1]), int(f[2], 16), int(f[3])
            for k in range(cnt):
                for o in range(0, max(width, 4), 4):
                    out.add(va + k * stride + o)
    return out


MASKED = _masked()
MASKED_LIST = sorted(MASKED)


# TIMER-PHASE rule (PROV-INSITU, measured 2026-11-11): three dwords of the frame-timer block hold values set during
# the load and rewritten at the first tick: 0x007f1010 (float), 0x007f102c and 0x007f1030 (one 50,000-count step
# apart between runs). At tick 0 they differ between unmodified-body runs: x_b1b (body 0x00477e60, 0 calls before the
# race anchor) and g95 against x_ref, with load lengths 118 / 119, and t95_150443_001_14 at 119 / 119. The block is
# equal again from tick 1 in all of them. Applied only at tick 0, only when that block is the one bad block and the
# raw copy of 0x007f0c00..0x007f1400 shows no other differing dword in it. Any reader that used the tick-0 values
# would move state that the later ticks still compare.
LOADLEN_BLOCK, LOADLEN_DWORDS = 0x007f1000, {0x007f1010, 0x007f102c, 0x007f1030}


def loadlen_rule(x, y, bad, raw_va, nblk, blk, mr, ma, notes):
    if [DATA_LO + (j - 2) * blk for j in bad] != [LOADLEN_BLOCK]:
        return False
    words = {raw_va[j - 2 - nblk] for j in range(2 + nblk, len(x)) if x[j] != y[j]
             and LOADLEN_BLOCK <= raw_va[j - 2 - nblk] < LOADLEN_BLOCK + blk}
    if not raw_va or not any(LOADLEN_BLOCK <= v < LOADLEN_BLOCK + blk for v in raw_va) or not words or not words <= LOADLEN_DWORDS:
        return False
    notes.append(f"race tick 0: {sorted(hex(w) for w in words)} differ, load lengths {mr['race'].get('dn_warp')} / {ma['race'].get('dn_warp')} (TIMER-PHASE rule)")
    return True


PHASE_NAME = {0: "boot", 1: "menu", 2: "load", 3: "race"}


# BOOT-VARIANT block (RO-BOOT-BLOCK-MASK, 2026-11-13; measured in RO-CONTROL-DRIFT): block 0x00773c00 holds two CRT-startup
# values written before any hook or body runs: the exe path buffer 0x00773c40 (0x104 bytes, GetModuleFileNameA at
# 0x004abeb0, it carries the private root folder name) and a kernel32 proc pointer at 0x00773d60 (GetProcAddress result
# stored at 0x004ae2d8, fixed for one boot of the machine). The block is hash-only in the tick records, and a MASK row in a
# hash-only block would drop the whole block, so the block is decided here: when its hash differs, the two runs' checkpoint
# dumps (ckptNN.data.bin = .data 0x005ea000..) must agree on every dword of the block outside BOOT_WORDS, and the hash must
# change at the same ticks in both runs. A run with a raw range over the block takes the per-dword path with BOOT_WORDS dropped.
BOOT_BLOCK = 0x00773c00
BOOT_WORDS = set(range(0x00773c40, 0x00773d44, 4)) | {0x00773d60}

# PAD-VARIANT words (RO-CTL-68BC0C, 2026-11-13): FUN_0045a6b0 (C4 by match, re/match/s29/0045a6b0.c:73; the copy is the
# rep movsd at 0x0045a736, ecx = 0x10 at 0x0045a72c) copies the 0x40-byte frame matrix FUN_004c0ed0 returns (frame +0x50)
# into each of the four 0x68-byte records at 0x0068bb60, at record +0x28. 0x0068bc0c / 0x0068bc1c are record 1 (base
# 0x0068bbc8) +0x44 / +0x54 = matrix words m[7] / m[11]. Across 195 recorded runs they are 4 / heap pointer in 160 and
# 0 / 0 in 35, while no other .data dword splits the two groups at checkpoints 0, 7 or 14, and r1 / r2 hold different
# pointers in m[11] (0x0f0e0038 / 0x0f0f0038). The block is hash-only; the tick hash already counts a heap-pointer dword
# as 0 (detorig.js tick hash, detorig.is_ptr), so the check below takes pointer pairs as equal, like the hash does.
PAD_BLOCK = 0x0068bc00
PAD_WORDS = {0x0068bc0c, 0x0068bc1c}

# hash-only block -> (words dropped, pointer pairs equal outside them)
VARIANT_BLOCKS = {BOOT_BLOCK: (BOOT_WORDS, False), PAD_BLOCK: (PAD_WORDS, True)}
VARIANT_WORDS = BOOT_WORDS | PAD_WORDS
VARIANT_NAME = {BOOT_BLOCK: "boot-variant", PAD_BLOCK: "pad-variant"}


def _ckpt_blocks(rundir, lo, size):
    """{ckpt index: bytes of [lo, lo+size) from ckptNN.data.bin}; {} when the run has none."""
    out = {}
    for f in sorted(Path(rundir).glob("ckpt*.data.bin")):
        try:
            k = int(f.name[4:6])
            with open(f, "rb") as fh:
                fh.seek(lo - DATA_LO)
                b = fh.read(size)
        except (ValueError, OSError):
            continue
        if len(b) == size:
            out[k] = b
    return out


def boot_block_check(run, ref, blk, lo=BOOT_BLOCK):
    """(ok, detail): VARIANT_BLOCKS block lo of run against ref outside its words, over the checkpoint dumps both runs have."""
    words, ptr_eq = VARIANT_BLOCKS[lo]
    a, b = _ckpt_blocks(run, lo, blk), _ckpt_blocks(ref, lo, blk)
    ks = sorted(set(a) & set(b))
    if not ks:
        return False, "no common checkpoint dump"
    for k in ks:
        for o in range(0, blk, 4):
            w = lo + o
            if w in words or a[k][o:o + 4] == b[k][o:o + 4]:
                continue
            if ptr_eq and is_ptr_pair(struct.unpack_from("<I", a[k], o)[0], struct.unpack_from("<I", b[k], o)[0]):
                continue
            return False, f"ckpt{k:02d} dword {w:#010x} differs {a[k][o:o+4][::-1].hex()} vs {b[k][o:o+4][::-1].hex()}"
    kind = "boot" if lo == BOOT_BLOCK else "pad"
    return True, f"{len(ks)} checkpoints equal outside the {len(words)} {kind} words" + (" (pointer pairs equal)" if ptr_eq else "")


def segments(meta):
    """RO-MENU-REPLAY boot mode: [[name, first record, last record], ...] from the tick phase bytes (detorig.py phase_runs).
    The warp tick ends the menu segment. Runs made without --boot have no phases: one 'race' segment."""
    ph = (meta.get("ticks") or {}).get("phases")
    if not ph:
        return None
    return [[PHASE_NAME.get(p, f"phase{p}"), a, b] for p, a, b in ph]


def segment_of(segs, i):
    for name, a, b in segs or []:
        if a <= i <= b:
            return name
    return None


def verdict(run, ref_a, ref_b, extra_pairs=()):
    try:
        mr, tr, rr = load(run)
    except (SystemExit, FileNotFoundError, KeyError) as e:
        m = {}
        try:
            m = json.loads((Path(run) / "meta.json").read_text())
        except Exception:
            pass
        return {"run": str(run), "verdict": "NO-TICKS", "identical_for": 0, "ticks": 0, "first": None,
                "rc": m.get("rc"), "exit": m.get("exit"), "detail": str(e)[:200]}
    ma, ta, ra = load(ref_a)
    mb, tb, rb = load(ref_b)
    nblk, blk = ta["nblk"], ta["block"]
    assert tr["recw"] == ta["recw"] == tb["recw"]
    n0 = min(len(ra), len(rb))
    allowed = set().union(*diffs(ra, rb, nblk, n0)) if n0 else set()
    for pa, pb in extra_pairs:
        _, tpa, rpa = load(pa)
        _, _, rpb = load(pb)
        assert tpa["nblk"] == nblk
        allowed |= set().union(*diffs(rpa, rpb, nblk, min(len(rpa), len(rpb))))
    n = min(len(rr), len(ra))
    raw_va = []
    for lo, hi in ta.get("raw") or []:
        raw_va += list(range(lo, hi, 4))
    va = lambda j: DATA_LO + (j - 2) * blk
    rawpos = {v: 2 + nblk + k for k, v in enumerate(raw_va)}
    first = None
    notes = []
    var_ok = {}
    # TIMER-PHASE applies at the race's first tick: record 0 in a warp-at-menu run, the first phase-3 record in a boot run
    race0 = next((a for name, a, b in (segments(ma) or []) if name == "race"), 0)
    for i in range(n):
        x, y = rr[i], ra[i]
        if (x[1] - mr["race"]["n"]) != (y[1] - ma["race"]["n"]):
            # the per-tick clock-call count is not a divergence by itself under --clock tick (REPLAY-ORACLE-DESIGN);
            # recorded only
            pass
        bad = []
        for j in range(2, 2 + nblk):
            if x[j] == y[j]:
                continue
            lo = DATA_LO + (j - 2) * blk
            if lo in rawpos and all(lo + 4 * k in rawpos for k in range(blk // 4)):
                # block copied raw every tick: decided per dword, MASK / OS dwords dropped, heap-pointer pairs dropped
                if any(x[rawpos[w]] != y[rawpos[w]] and w not in MASKED and w not in VARIANT_WORDS and not is_ptr_pair(x[rawpos[w]], y[rawpos[w]])
                       for w in range(lo, lo + blk, 4)):
                    bad.append(j)
            elif lo in VARIANT_BLOCKS:
                if lo not in var_ok:
                    var_ok[lo] = boot_block_check(run, ref_a, blk, lo)
                    notes.append(f"{VARIANT_NAME[lo]} block {lo:#010x}: {'masked' if var_ok[lo][0] else 'NOT masked'}, {var_ok[lo][1]}")
                boot_ok = var_ok[lo]
                if not boot_ok[0] and j not in allowed:
                    bad.append(j)
                elif boot_ok[0]:
                    # the hash must also move at the same ticks in both runs (a body writing the block elsewhere shows here)
                    if i and ((x[j] != rr[i - 1][j]) != (y[j] != ra[i - 1][j])):
                        bad.append(j)
            elif j not in allowed and not any(lo <= w < lo + blk for w in MASKED_LIST):
                bad.append(j)
        if i == race0 and bad and loadlen_rule(x, y, bad, raw_va, nblk, blk, mr, ma, notes):
            bad = []
        if bad:
            raw = [(hex(raw_va[j - 2 - nblk]), f"{x[j]:08x}", f"{y[j]:08x}") for j in range(2 + nblk, len(x))
                   if x[j] != y[j] and not is_ptr_pair(x[j], y[j])]
            first = {"tick": i, "blocks": [[hex(va(j)), region_of(va(j))] for j in bad[:24]], "nblocks": len(bad), "raw": raw[:24]}
            break
    rep = {"run": str(run), "ref": str(ref_a), "ref2": str(ref_b), "ticks": n, "ref_ticks": n0,
           "allowed_blocks": [hex(va(j)) for j in sorted(allowed)], "complete": n >= 2724,
           "verdict": ("IDENTICAL-FOR-N" if n >= n0 else "INCOMPLETE") if first is None else "DIVERGED", "identical_for": n if first is None else first["tick"],
           "first": first, "rc": mr.get("rc"), "exit": mr.get("exit"), "notes": notes,
           "dn_warp": [mr["race"].get("dn_warp"), ma["race"].get("dn_warp")]}
    segs = segments(ma)
    if segs:
        # boot mode: the segments of the reference, the run's own, and where the first divergence falls
        rep["segments"] = segs
        rep["run_segments"] = segments(mr)
        rep["segments_match"] = rep["run_segments"] == segs[:len(rep["run_segments"] or [])] if rep["run_segments"] else None
        if first:
            first["segment"] = segment_of(segs, first["tick"])
        rep["identical_by_segment"] = {name: max(0, min(b, rep["identical_for"] - 1) - a + 1) for name, a, b in segs}
        rep["ticks_by_segment"] = {name: b - a + 1 for name, a, b in segs}
        # BOOT-CLASS (RO-MENU-REPLAY, measured 2026-11-12): unmodified boot runs fall into classes that stay apart for
        # the whole run: the tick-0 value of 0x007f1030 (frame-timer block; 4 values seen in 25 runs, r5 vs r3 then
        # diverges at race tick 1752 in 0x00618000 with the timer dwords masked) and 0x00773d60 (kernel32 pointer, one value per boot of the machine;
        # a run and a reference of different boots differ in 0x00773c00 from tick 0, RO-CONTROL-DRIFT pair test). Recorded, not scored: a run whose
        # class differs from its reference's is re-run or scored against a same-class reference.
        # RO-BOOT-BLOCK-MASK: the kernel32 pointer 0x00773d60 (changes per reboot) replaces 0x006147d8 (a MASK:os toggle that
        # differs between two same-boot runs and caused no 0x00773c00 difference, RESULT_RO-CONTROL-DRIFT). It is in a
        # hash-only block, so it is read from checkpoint 0 of the run.
        def cls(r, d):
            out = {hex(w): f"{r[0][rawpos[w]]:08x}" for w in BOOT_CLASS_DWORDS if w in rawpos} if r else {}
            c0 = _ckpt_blocks(d, BOOT_BLOCK, blk).get(0)
            if c0 is not None:
                out[hex(BOOT_KEY)] = c0[BOOT_KEY - BOOT_BLOCK:BOOT_KEY - BOOT_BLOCK + 4][::-1].hex()
            return out
        rep["boot_class"] = {"run": cls(rr, run), "ref": cls(ra, ref_a), "ref2": cls(rb, ref_b)}
        rep["boot_class_match"] = rep["boot_class"]["run"] == rep["boot_class"]["ref"] == rep["boot_class"]["ref2"]
        if not rep["boot_class_match"]:
            notes.append(f"BOOT-CLASS mismatch {rep['boot_class']}: verdict not body evidence")
    return rep


BOOT_CLASS_DWORDS = (0x007f1030,)
BOOT_KEY = 0x00773d60


if __name__ == "__main__":
    ap = argparse.ArgumentParser()
    ap.add_argument("run"); ap.add_argument("ref_a"); ap.add_argument("ref_b")
    ap.add_argument("--json", default="")
    ap.add_argument("--pair", action="append", default=[], help="A,B: another reference pair whose differing blocks are allowed")
    a = ap.parse_args()
    r = verdict(a.run, a.ref_a, a.ref_b, [p.split(",") for p in a.pair])
    print(f"{a.run}: {r['verdict']} {r['identical_for']} of {r['ticks']} (allowed blocks {len(r['allowed_blocks'])}, rc {r['rc']})")
    if r.get("segments"):
        print(f"  segments {r['segments']}  identical by segment {r['identical_by_segment']}  run segments match {r['segments_match']}")
    if r["first"]:
        print(f"  first divergence tick {r['first']['tick']} ({r['first'].get('segment')}): {r['first']['nblocks']} blocks {r['first']['blocks'][:8]}")
        for w in r["first"]["raw"][:10]:
            print("   raw", w)
    if a.json:
        Path(a.json).write_text(json.dumps(r, indent=1))
